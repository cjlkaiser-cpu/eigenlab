#include "physics/protein_folding.hpp"
#include <cmath>
#include <algorithm>

ProteinFolding::ProteinFolding() : m_rng(42), m_stepCount(0), m_currentTemp(1.0f) {
    init();
}

ProteinFolding::ProteinFolding(const ProteinConfig& config)
    : m_config(config), m_rng(42), m_stepCount(0), m_currentTemp(config.temperature) {
    init();
}

void ProteinFolding::init() {
    m_stepCount = 0;
    m_currentTemp = m_config.temperature;
    generateRandomSequence();
    initResidues();
    updateArrays();
}

void ProteinFolding::generateRandomSequence() {
    m_sequence.clear();
    m_sequence.reserve(m_config.sequenceLength);

    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (int i = 0; i < m_config.sequenceLength; ++i) {
        // 50% H, 50% P by default
        m_sequence += (dist(m_rng) < 0.5f) ? 'H' : 'P';
    }
}

void ProteinFolding::initFromSequence(const std::string& sequence) {
    m_sequence = sequence;
    m_config.sequenceLength = static_cast<int>(sequence.length());
    initResidues();
    updateArrays();
}

void ProteinFolding::initResidues() {
    m_residues.clear();
    m_residues.reserve(m_config.sequenceLength);
    m_bondIndices.clear();

    float bondLen = m_config.bondLength;

    // Initialize as extended chain
    for (int i = 0; i < m_config.sequenceLength; ++i) {
        Residue r;
        r.index = i;

        // Extended chain configuration
        if (m_config.use3D) {
            // 3D: slight zigzag
            r.x = i * bondLen * 0.9f;
            r.y = (i % 2) * bondLen * 0.3f;
            r.z = ((i / 2) % 2) * bondLen * 0.3f;
        } else {
            // 2D: zigzag pattern
            r.x = i * bondLen * 0.9f;
            r.y = (i % 2) * bondLen * 0.4f;
            r.z = 0.0f;
        }

        r.vx = r.vy = r.vz = 0.0f;
        r.fx = r.fy = r.fz = 0.0f;

        // Set type from sequence
        char c = (i < static_cast<int>(m_sequence.length())) ? m_sequence[i] : 'P';
        r.type = (c == 'H' || c == 'h') ? ResidueType::Hydrophobic : ResidueType::Polar;

        m_residues.push_back(r);

        // Bond to previous residue
        if (i > 0) {
            m_bondIndices.push_back(i - 1);
            m_bondIndices.push_back(i);
        }
    }

    // Center the protein
    float cx = 0, cy = 0, cz = 0;
    for (const auto& r : m_residues) {
        cx += r.x;
        cy += r.y;
        cz += r.z;
    }
    cx /= m_residues.size();
    cy /= m_residues.size();
    cz /= m_residues.size();

    for (auto& r : m_residues) {
        r.x -= cx;
        r.y -= cy;
        r.z -= cz;
    }
}

void ProteinFolding::updateArrays() {
    int n = static_cast<int>(m_residues.size());

    m_positions.resize(n * 3);
    m_colors.resize(n * 3);

    for (int i = 0; i < n; ++i) {
        const Residue& r = m_residues[i];

        m_positions[i * 3 + 0] = r.x;
        m_positions[i * 3 + 1] = r.y;
        m_positions[i * 3 + 2] = r.z;

        float red, green, blue;
        computeResidueColor(r, red, green, blue);
        m_colors[i * 3 + 0] = red;
        m_colors[i * 3 + 1] = green;
        m_colors[i * 3 + 2] = blue;
    }
}

void ProteinFolding::computeResidueColor(const Residue& r,
                                          float& red, float& green, float& blue) const {
    if (r.type == ResidueType::Hydrophobic) {
        // Hydrophobic: orange/red
        red = 1.0f;
        green = 0.4f;
        blue = 0.1f;
    } else {
        // Polar: cyan/blue
        red = 0.2f;
        green = 0.7f;
        blue = 1.0f;
    }
}

void ProteinFolding::computeForces() {
    // Reset forces
    for (auto& r : m_residues) {
        r.fx = r.fy = r.fz = 0.0f;
    }

    int n = static_cast<int>(m_residues.size());

    // Bond forces (harmonic springs)
    for (int i = 0; i < n - 1; ++i) {
        Residue& r1 = m_residues[i];
        Residue& r2 = m_residues[i + 1];

        float dx = r2.x - r1.x;
        float dy = r2.y - r1.y;
        float dz = r2.z - r1.z;

        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (dist < 0.001f) dist = 0.001f;

        float stretch = dist - m_config.bondLength;
        float force = m_config.bondStiffness * stretch;

        float fx = force * dx / dist;
        float fy = force * dy / dist;
        float fz = force * dz / dist;

        r1.fx += fx;
        r1.fy += fy;
        r1.fz += fz;

        r2.fx -= fx;
        r2.fy -= fy;
        r2.fz -= fz;
    }

    // Non-bonded forces (Lennard-Jones)
    float sigma = m_config.ljSigma;
    float epsilon = m_config.ljEpsilon;

    for (int i = 0; i < n - 2; ++i) {
        for (int j = i + 2; j < n; ++j) {
            Residue& r1 = m_residues[i];
            Residue& r2 = m_residues[j];

            float dx = r2.x - r1.x;
            float dy = r2.y - r1.y;
            float dz = r2.z - r1.z;

            float r2dist = dx * dx + dy * dy + dz * dz;
            float cutoff = 2.5f * sigma;

            if (r2dist < cutoff * cutoff && r2dist > 0.01f) {
                float dist = std::sqrt(r2dist);
                float sr6 = std::pow(sigma / dist, 6);
                float sr12 = sr6 * sr6;

                // LJ force magnitude: F = 24*eps/r * (2*sr12 - sr6)
                float forceMag = 24.0f * epsilon / dist * (2.0f * sr12 - sr6);

                // Scale based on residue types (HP model influence)
                float scale = 1.0f;
                if (r1.type == ResidueType::Hydrophobic && r2.type == ResidueType::Hydrophobic) {
                    scale = 1.5f;  // Stronger attraction between H-H
                }

                forceMag *= scale;

                float fx = forceMag * dx / dist;
                float fy = forceMag * dy / dist;
                float fz = forceMag * dz / dist;

                r1.fx -= fx;
                r1.fy -= fy;
                r1.fz -= fz;

                r2.fx += fx;
                r2.fy += fy;
                r2.fz += fz;
            }
        }
    }
}

float ProteinFolding::computeContactEnergy() const {
    float energy = 0.0f;
    int n = static_cast<int>(m_residues.size());
    float contactR2 = m_config.contactRadius * m_config.contactRadius;

    for (int i = 0; i < n - 2; ++i) {
        for (int j = i + 2; j < n; ++j) {
            const Residue& r1 = m_residues[i];
            const Residue& r2 = m_residues[j];

            float dx = r2.x - r1.x;
            float dy = r2.y - r1.y;
            float dz = r2.z - r1.z;
            float r2dist = dx * dx + dy * dy + dz * dz;

            if (r2dist < contactR2) {
                if (r1.type == ResidueType::Hydrophobic && r2.type == ResidueType::Hydrophobic) {
                    energy += m_config.hhContactEnergy;
                } else if (r1.type == ResidueType::Polar && r2.type == ResidueType::Polar) {
                    energy += m_config.ppContactEnergy;
                } else {
                    energy += m_config.hpContactEnergy;
                }
            }
        }
    }

    return energy;
}

float ProteinFolding::computeLJEnergy() const {
    float energy = 0.0f;
    int n = static_cast<int>(m_residues.size());
    float sigma = m_config.ljSigma;
    float epsilon = m_config.ljEpsilon;
    float cutoff = 2.5f * sigma;

    for (int i = 0; i < n - 2; ++i) {
        for (int j = i + 2; j < n; ++j) {
            const Residue& r1 = m_residues[i];
            const Residue& r2 = m_residues[j];

            float dx = r2.x - r1.x;
            float dy = r2.y - r1.y;
            float dz = r2.z - r1.z;
            float r2dist = dx * dx + dy * dy + dz * dz;

            if (r2dist < cutoff * cutoff && r2dist > 0.01f) {
                float dist = std::sqrt(r2dist);
                float sr6 = std::pow(sigma / dist, 6);
                float sr12 = sr6 * sr6;

                energy += 4.0f * epsilon * (sr12 - sr6);
            }
        }
    }

    return energy;
}

float ProteinFolding::computeTotalEnergy() const {
    float bondEnergy = 0.0f;
    int n = static_cast<int>(m_residues.size());

    // Bond energy
    for (int i = 0; i < n - 1; ++i) {
        const Residue& r1 = m_residues[i];
        const Residue& r2 = m_residues[i + 1];

        float dx = r2.x - r1.x;
        float dy = r2.y - r1.y;
        float dz = r2.z - r1.z;
        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
        float stretch = dist - m_config.bondLength;

        bondEnergy += 0.5f * m_config.bondStiffness * stretch * stretch;
    }

    return bondEnergy + computeContactEnergy() + computeLJEnergy();
}

void ProteinFolding::velocityVerletStep() {
    float dt = m_config.dt;
    float halfDt = 0.5f * dt;
    float mass = 1.0f;  // Unit mass

    // Half-step velocity update
    for (auto& r : m_residues) {
        r.vx += halfDt * r.fx / mass;
        r.vy += halfDt * r.fy / mass;
        r.vz += halfDt * r.fz / mass;

        // Position update
        r.x += dt * r.vx;
        r.y += dt * r.vy;
        if (m_config.use3D) {
            r.z += dt * r.vz;
        }
    }

    // Compute new forces
    computeForces();

    // Second half-step velocity update
    for (auto& r : m_residues) {
        r.vx += halfDt * r.fx / mass;
        r.vy += halfDt * r.fy / mass;
        r.vz += halfDt * r.fz / mass;

        // Damping
        float damping = 0.99f;
        r.vx *= damping;
        r.vy *= damping;
        r.vz *= damping;
    }
}

bool ProteinFolding::metropolisAccept(float deltaE) {
    if (deltaE <= 0) return true;

    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float prob = std::exp(-deltaE / m_currentTemp);
    return dist(m_rng) < prob;
}

void ProteinFolding::perturbResidue(int idx, float amount) {
    std::uniform_real_distribution<float> dist(-amount, amount);

    m_residues[idx].x += dist(m_rng);
    m_residues[idx].y += dist(m_rng);
    if (m_config.use3D) {
        m_residues[idx].z += dist(m_rng);
    }
}

void ProteinFolding::step() {
    foldMolecularDynamics(1);
    m_stepCount++;
    updateArrays();
}

void ProteinFolding::step(int numSteps) {
    for (int i = 0; i < numSteps; ++i) {
        step();
    }
}

void ProteinFolding::foldMonteCarlo(int numSteps) {
    std::uniform_int_distribution<int> idxDist(0, static_cast<int>(m_residues.size()) - 1);

    for (int s = 0; s < numSteps; ++s) {
        float oldEnergy = computeTotalEnergy();

        // Pick random residue (avoid endpoints for stability)
        int idx = idxDist(m_rng);

        // Save old position
        float oldX = m_residues[idx].x;
        float oldY = m_residues[idx].y;
        float oldZ = m_residues[idx].z;

        // Perturb
        perturbResidue(idx, 0.3f);

        float newEnergy = computeTotalEnergy();
        float deltaE = newEnergy - oldEnergy;

        if (!metropolisAccept(deltaE)) {
            // Reject move
            m_residues[idx].x = oldX;
            m_residues[idx].y = oldY;
            m_residues[idx].z = oldZ;
        }

        m_stepCount++;
    }

    updateArrays();
}

void ProteinFolding::foldSimulatedAnnealing(int numSteps) {
    for (int s = 0; s < numSteps; ++s) {
        foldMonteCarlo(1);
        m_currentTemp *= m_config.coolingRate;
        m_currentTemp = std::max(0.001f, m_currentTemp);
    }
}

void ProteinFolding::foldMolecularDynamics(int numSteps) {
    computeForces();

    for (int s = 0; s < numSteps; ++s) {
        velocityVerletStep();
        m_stepCount++;
    }

    updateArrays();
}

void ProteinFolding::setTemperature(float temp) {
    m_config.temperature = temp;
    m_currentTemp = temp;
}

void ProteinFolding::setCoolingRate(float rate) {
    m_config.coolingRate = rate;
}

void ProteinFolding::setContactEnergies(float hh, float hp, float pp) {
    m_config.hhContactEnergy = hh;
    m_config.hpContactEnergy = hp;
    m_config.ppContactEnergy = pp;
}

// Presets
void ProteinFolding::presetAlphaHelix() {
    // Alternating pattern tends to form helical structures
    m_sequence = "HPHPHPHPHPHPHPHPHPH";
    m_config.sequenceLength = static_cast<int>(m_sequence.length());
    m_config.hhContactEnergy = -2.0f;
    initResidues();
    updateArrays();
}

void ProteinFolding::presetBetaSheet() {
    // Blocks of H residues tend to form sheet-like
    m_sequence = "HHHPPPHHHPPPHHHPPP";
    m_config.sequenceLength = static_cast<int>(m_sequence.length());
    m_config.hhContactEnergy = -1.5f;
    initResidues();
    updateArrays();
}

void ProteinFolding::presetRandomCoil() {
    // Mostly polar, no specific structure
    m_sequence = "PPPPPPPPPPPPPPPPPPP";
    m_config.sequenceLength = static_cast<int>(m_sequence.length());
    m_config.hhContactEnergy = -0.5f;
    initResidues();
    updateArrays();
}

void ProteinFolding::presetGlobular() {
    // Hydrophobic core with polar shell tendency
    m_sequence = "PHHHHHHHHHHHHHHHHP";
    m_config.sequenceLength = static_cast<int>(m_sequence.length());
    m_config.hhContactEnergy = -2.5f;
    initResidues();
    updateArrays();
}

ProteinStats ProteinFolding::getStats() const {
    ProteinStats stats = {};

    if (m_residues.empty()) return stats;

    int n = static_cast<int>(m_residues.size());

    // Energies
    float bondEnergy = 0.0f;
    for (int i = 0; i < n - 1; ++i) {
        const Residue& r1 = m_residues[i];
        const Residue& r2 = m_residues[i + 1];

        float dx = r2.x - r1.x;
        float dy = r2.y - r1.y;
        float dz = r2.z - r1.z;
        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
        float stretch = dist - m_config.bondLength;
        bondEnergy += 0.5f * m_config.bondStiffness * stretch * stretch;
    }

    stats.bondEnergy = bondEnergy;
    stats.contactEnergy = computeContactEnergy();
    stats.ljEnergy = computeLJEnergy();
    stats.totalEnergy = bondEnergy + stats.contactEnergy + stats.ljEnergy;

    // Count contacts
    float contactR2 = m_config.contactRadius * m_config.contactRadius;
    stats.numHHContacts = 0;
    stats.numHPContacts = 0;

    for (int i = 0; i < n - 2; ++i) {
        for (int j = i + 2; j < n; ++j) {
            const Residue& r1 = m_residues[i];
            const Residue& r2 = m_residues[j];

            float dx = r2.x - r1.x;
            float dy = r2.y - r1.y;
            float dz = r2.z - r1.z;
            float r2dist = dx * dx + dy * dy + dz * dz;

            if (r2dist < contactR2) {
                if (r1.type == ResidueType::Hydrophobic && r2.type == ResidueType::Hydrophobic) {
                    stats.numHHContacts++;
                } else if (r1.type != r2.type) {
                    stats.numHPContacts++;
                }
            }
        }
    }

    // Radius of gyration
    float cx = 0, cy = 0, cz = 0;
    for (const auto& r : m_residues) {
        cx += r.x;
        cy += r.y;
        cz += r.z;
    }
    cx /= n;
    cy /= n;
    cz /= n;

    float rg2 = 0;
    for (const auto& r : m_residues) {
        float dx = r.x - cx;
        float dy = r.y - cy;
        float dz = r.z - cz;
        rg2 += dx * dx + dy * dy + dz * dz;
    }
    stats.radiusOfGyration = std::sqrt(rg2 / n);

    // End-to-end distance
    const Residue& first = m_residues.front();
    const Residue& last = m_residues.back();
    float dx = last.x - first.x;
    float dy = last.y - first.y;
    float dz = last.z - first.z;
    stats.endToEndDistance = std::sqrt(dx * dx + dy * dy + dz * dz);

    stats.temperature = m_currentTemp;
    stats.stepCount = m_stepCount;

    // Consider "folded" if compact (small Rg) and has HH contacts
    float expectedExtendedRg = m_config.bondLength * n / std::sqrt(12.0f);
    stats.isFolded = stats.radiusOfGyration < expectedExtendedRg * 0.6f && stats.numHHContacts > 2;

    return stats;
}
