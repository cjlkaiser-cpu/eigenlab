#include "../../include/physics/molecular_dynamics.hpp"
#include <algorithm>
#include <cstring>

namespace eigenlab {
namespace physics {

// ==================================================
// Constructor & Initialization
// ==================================================

MolecularDynamics::MolecularDynamics()
    : boxSize_(50.0f)
    , halfBox_(25.0f)
    , cutoffSq_(6.25f)
    , simulationTime_(0.0f)
    , cellsPerSide_(10)
    , cellSize_(5.0f)
    , rng_(std::random_device{}())
{
    setConfig(MDConfig{});
}

MolecularDynamics::MolecularDynamics(const MDConfig& config)
    : simulationTime_(0.0f)
    , rng_(std::random_device{}())
{
    setConfig(config);
}

void MolecularDynamics::setConfig(const MDConfig& config) {
    config_ = config;
    boxSize_ = config.boxSize;
    halfBox_ = boxSize_ / 2.0f;
    cutoffSq_ = config.cutoff * config.sigma * config.cutoff * config.sigma;

    // Setup cell list
    cellSize_ = config.cutoff * config.sigma * 1.1f;
    cellsPerSide_ = std::max(3, static_cast<int>(boxSize_ / cellSize_));
    cellSize_ = boxSize_ / cellsPerSide_;
    cells_.resize(cellsPerSide_ * cellsPerSide_);
}

void MolecularDynamics::initializeLattice() {
    atoms_.clear();
    simulationTime_ = 0.0f;

    // Calculate lattice spacing for desired number of atoms
    int side = static_cast<int>(std::sqrt(config_.numAtoms)) + 1;
    float spacing = boxSize_ / (side + 1);

    // Ensure spacing is at least sigma to avoid overlap
    spacing = std::max(spacing, config_.sigma * 1.1f);

    std::normal_distribution<float> velDist(0.0f, std::sqrt(config_.temperature));

    int count = 0;
    for (int i = 0; i < side && count < config_.numAtoms; ++i) {
        for (int j = 0; j < side && count < config_.numAtoms; ++j) {
            Atom atom;
            atom.x = spacing * (i + 0.5f) + (j % 2) * spacing * 0.5f;
            atom.y = spacing * (j + 0.5f);
            atom.x0 = atom.x;
            atom.y0 = atom.y;
            atom.vx = velDist(rng_);
            atom.vy = velDist(rng_);
            atom.fx = 0.0f;
            atom.fy = 0.0f;
            atom.active = true;

            atoms_.push_back(atom);
            count++;
        }
    }

    // Remove center of mass velocity
    float vxSum = 0.0f, vySum = 0.0f;
    for (const auto& atom : atoms_) {
        vxSum += atom.vx;
        vySum += atom.vy;
    }
    vxSum /= atoms_.size();
    vySum /= atoms_.size();
    for (auto& atom : atoms_) {
        atom.vx -= vxSum;
        atom.vy -= vySum;
    }

    // Scale to target temperature
    rescaleVelocities(config_.temperature);

    updateDataBuffers();
    computeStatistics();
}

void MolecularDynamics::initializeRandom() {
    atoms_.clear();
    simulationTime_ = 0.0f;

    std::uniform_real_distribution<float> posDist(config_.sigma, boxSize_ - config_.sigma);
    std::normal_distribution<float> velDist(0.0f, std::sqrt(config_.temperature * 2.0f));

    for (int i = 0; i < config_.numAtoms; ++i) {
        Atom atom;
        atom.x = posDist(rng_);
        atom.y = posDist(rng_);
        atom.x0 = atom.x;
        atom.y0 = atom.y;
        atom.vx = velDist(rng_);
        atom.vy = velDist(rng_);
        atom.fx = 0.0f;
        atom.fy = 0.0f;
        atom.active = true;

        atoms_.push_back(atom);
    }

    // Remove center of mass velocity
    float vxSum = 0.0f, vySum = 0.0f;
    for (const auto& atom : atoms_) {
        vxSum += atom.vx;
        vySum += atom.vy;
    }
    vxSum /= atoms_.size();
    vySum /= atoms_.size();
    for (auto& atom : atoms_) {
        atom.vx -= vxSum;
        atom.vy -= vySum;
    }

    rescaleVelocities(config_.temperature);
    updateDataBuffers();
    computeStatistics();
}

void MolecularDynamics::initializeLiquid() {
    // Same as random but with moderate density
    initializeRandom();
}

void MolecularDynamics::clear() {
    atoms_.clear();
    positionData_.clear();
    velocityData_.clear();
    simulationTime_ = 0.0f;
    stats_ = MDStats{};
}

// ==================================================
// Simulation
// ==================================================

void MolecularDynamics::step(float dt) {
    if (atoms_.empty()) return;

    float actualDt = (dt > 0) ? dt : config_.dt;

    // Velocity Verlet integration
    integrateVerlet(actualDt);

    // Apply thermostat if enabled
    if (config_.useThermostat) {
        applyBerendsenThermostat(config_.temperature, config_.thermostatTau);
    }

    simulationTime_ += actualDt;
    updateDataBuffers();
}

void MolecularDynamics::stepMultiple(int steps, float dt) {
    for (int i = 0; i < steps; ++i) {
        step(dt);
    }
    computeStatistics();
}

void MolecularDynamics::integrateVerlet(float dt) {
    float halfDt = dt * 0.5f;
    float halfDtSq = halfDt * dt;
    float invMass = 1.0f / config_.mass;

    // First half: update velocities and positions
    for (auto& atom : atoms_) {
        // v(t + dt/2) = v(t) + a(t) * dt/2
        atom.vx += atom.fx * invMass * halfDt;
        atom.vy += atom.fy * invMass * halfDt;

        // x(t + dt) = x(t) + v(t + dt/2) * dt
        atom.x += atom.vx * dt;
        atom.y += atom.vy * dt;

        // Apply periodic boundary conditions
        if (config_.periodicBC) {
            applyPeriodicBC(atom);
        } else {
            // Reflective boundaries
            if (atom.x < 0) { atom.x = -atom.x; atom.vx = -atom.vx; }
            if (atom.x > boxSize_) { atom.x = 2*boxSize_ - atom.x; atom.vx = -atom.vx; }
            if (atom.y < 0) { atom.y = -atom.y; atom.vy = -atom.vy; }
            if (atom.y > boxSize_) { atom.y = 2*boxSize_ - atom.y; atom.vy = -atom.vy; }
        }
    }

    // Compute new forces
    computeForces();

    // Second half: update velocities with new forces
    for (auto& atom : atoms_) {
        atom.vx += atom.fx * invMass * halfDt;
        atom.vy += atom.fy * invMass * halfDt;
    }
}

void MolecularDynamics::computeForces() {
    // Reset forces
    for (auto& atom : atoms_) {
        atom.fx = 0.0f;
        atom.fy = 0.0f;
    }

    // Build cell list for efficient neighbor search
    buildCellList();

    stats_.potentialEnergy = 0.0f;

    // Compute pairwise forces using cell list
    for (int cy = 0; cy < cellsPerSide_; ++cy) {
        for (int cx = 0; cx < cellsPerSide_; ++cx) {
            int cellIdx = cy * cellsPerSide_ + cx;
            const auto& cell = cells_[cellIdx];

            // Check neighboring cells (including self)
            for (int dcy = -1; dcy <= 1; ++dcy) {
                for (int dcx = -1; dcx <= 1; ++dcx) {
                    int ncx = cx + dcx;
                    int ncy = cy + dcy;

                    // Periodic cell wrapping
                    if (config_.periodicBC) {
                        if (ncx < 0) ncx += cellsPerSide_;
                        if (ncx >= cellsPerSide_) ncx -= cellsPerSide_;
                        if (ncy < 0) ncy += cellsPerSide_;
                        if (ncy >= cellsPerSide_) ncy -= cellsPerSide_;
                    } else {
                        if (ncx < 0 || ncx >= cellsPerSide_) continue;
                        if (ncy < 0 || ncy >= cellsPerSide_) continue;
                    }

                    int neighborIdx = ncy * cellsPerSide_ + ncx;
                    const auto& neighborCell = cells_[neighborIdx];

                    // Compute forces between atoms in these cells
                    for (int i : cell) {
                        for (int j : neighborCell) {
                            if (i >= j) continue; // Avoid double counting

                            float dx = atoms_[j].x - atoms_[i].x;
                            float dy = atoms_[j].y - atoms_[i].y;

                            // Minimum image convention
                            if (config_.periodicBC) {
                                dx = minimumImage(dx);
                                dy = minimumImage(dy);
                            }

                            float r2 = dx * dx + dy * dy;

                            if (r2 < cutoffSq_ && r2 > 0.01f) {
                                float force = ljForce(r2);
                                float fx = force * dx;
                                float fy = force * dy;

                                atoms_[i].fx += fx;
                                atoms_[i].fy += fy;
                                atoms_[j].fx -= fx;
                                atoms_[j].fy -= fy;

                                stats_.potentialEnergy += ljPotential(r2);
                            }
                        }
                    }
                }
            }
        }
    }
}

void MolecularDynamics::buildCellList() {
    // Clear all cells
    for (auto& cell : cells_) {
        cell.clear();
    }

    // Assign atoms to cells
    for (size_t i = 0; i < atoms_.size(); ++i) {
        int cx = static_cast<int>(atoms_[i].x / cellSize_);
        int cy = static_cast<int>(atoms_[i].y / cellSize_);

        // Clamp to valid range
        cx = std::max(0, std::min(cellsPerSide_ - 1, cx));
        cy = std::max(0, std::min(cellsPerSide_ - 1, cy));

        cells_[cy * cellsPerSide_ + cx].push_back(static_cast<int>(i));
    }
}

void MolecularDynamics::applyPeriodicBC(Atom& atom) {
    while (atom.x < 0) atom.x += boxSize_;
    while (atom.x >= boxSize_) atom.x -= boxSize_;
    while (atom.y < 0) atom.y += boxSize_;
    while (atom.y >= boxSize_) atom.y -= boxSize_;
}

float MolecularDynamics::minimumImage(float dx) const {
    if (dx > halfBox_) dx -= boxSize_;
    else if (dx < -halfBox_) dx += boxSize_;
    return dx;
}

// ==================================================
// Lennard-Jones Calculations
// ==================================================

inline float MolecularDynamics::ljPotential(float r2) const {
    float sigma2 = config_.sigma * config_.sigma;
    float sr2 = sigma2 / r2;
    float sr6 = sr2 * sr2 * sr2;
    float sr12 = sr6 * sr6;
    return 4.0f * config_.epsilon * (sr12 - sr6);
}

inline float MolecularDynamics::ljForce(float r2) const {
    float sigma2 = config_.sigma * config_.sigma;
    float sr2 = sigma2 / r2;
    float sr6 = sr2 * sr2 * sr2;
    float sr12 = sr6 * sr6;
    return 24.0f * config_.epsilon * (2.0f * sr12 - sr6) / r2;
}

// ==================================================
// Temperature Control
// ==================================================

void MolecularDynamics::setTemperature(float T) {
    config_.temperature = T;
}

void MolecularDynamics::rescaleVelocities(float targetT) {
    if (atoms_.empty()) return;

    // Calculate current kinetic energy
    float ke = 0.0f;
    for (const auto& atom : atoms_) {
        ke += atom.vx * atom.vx + atom.vy * atom.vy;
    }
    ke *= 0.5f * config_.mass;

    // Current temperature (2D: T = KE / N)
    float currentT = ke / atoms_.size();

    if (currentT > 0.0001f) {
        float scale = std::sqrt(targetT / currentT);
        for (auto& atom : atoms_) {
            atom.vx *= scale;
            atom.vy *= scale;
        }
    }
}

void MolecularDynamics::applyBerendsenThermostat(float targetT, float tau) {
    if (atoms_.empty() || tau <= 0) return;

    // Calculate current temperature
    float ke = 0.0f;
    for (const auto& atom : atoms_) {
        ke += atom.vx * atom.vx + atom.vy * atom.vy;
    }
    float currentT = 0.5f * config_.mass * ke / atoms_.size();

    if (currentT > 0.0001f) {
        float lambda = std::sqrt(1.0f + config_.dt / tau * (targetT / currentT - 1.0f));
        for (auto& atom : atoms_) {
            atom.vx *= lambda;
            atom.vy *= lambda;
        }
    }
}

// ==================================================
// Parameter Setters
// ==================================================

void MolecularDynamics::setEpsilon(float eps) {
    config_.epsilon = eps;
}

void MolecularDynamics::setSigma(float sig) {
    config_.sigma = sig;
    cutoffSq_ = config_.cutoff * sig * config_.cutoff * sig;
}

void MolecularDynamics::setCutoff(float rc) {
    config_.cutoff = rc;
    cutoffSq_ = rc * config_.sigma * rc * config_.sigma;
}

void MolecularDynamics::setThermostatEnabled(bool enabled) {
    config_.useThermostat = enabled;
}

void MolecularDynamics::setThermostatTau(float tau) {
    config_.thermostatTau = tau;
}

// ==================================================
// Statistics
// ==================================================

void MolecularDynamics::computeStatistics() {
    if (atoms_.empty()) {
        stats_ = MDStats{};
        return;
    }

    // Kinetic energy
    float ke = 0.0f;
    for (const auto& atom : atoms_) {
        ke += atom.vx * atom.vx + atom.vy * atom.vy;
    }
    ke *= 0.5f * config_.mass;

    stats_.kineticEnergy = ke;
    stats_.totalEnergy = ke + stats_.potentialEnergy;
    stats_.temperature = ke / atoms_.size(); // 2D: T = KE / N (k_B = 1)
    stats_.numAtoms = static_cast<int>(atoms_.size());
    stats_.density = stats_.numAtoms / (boxSize_ * boxSize_);
    stats_.simulationTime = simulationTime_;

    // Mean square displacement
    float msd = 0.0f;
    for (const auto& atom : atoms_) {
        float dx = atom.x - atom.x0;
        float dy = atom.y - atom.y0;
        msd += dx * dx + dy * dy;
    }
    stats_.meanSquareDisplacement = msd / atoms_.size();

    // Detect phase
    stats_.phase = detectPhase();

    // Simple virial pressure estimate
    stats_.pressure = stats_.density * stats_.temperature;
}

MatterPhase MolecularDynamics::detectPhase() const {
    if (atoms_.empty()) return MatterPhase::UNKNOWN;

    // Use Lindemann criterion and temperature
    // Solid: low MSD, ordered structure
    // Liquid: moderate MSD, disordered
    // Gas: high MSD, low density

    float T = stats_.temperature;
    float density = stats_.density;
    float msd = stats_.meanSquareDisplacement;

    // Very rough phase detection based on reduced units
    if (T < 0.5f && density > 0.3f) {
        return MatterPhase::SOLID;
    } else if (T > 2.0f || density < 0.1f) {
        return MatterPhase::GAS;
    } else {
        return MatterPhase::LIQUID;
    }
}

void MolecularDynamics::updateDataBuffers() {
    positionData_.resize(atoms_.size() * 2);
    velocityData_.resize(atoms_.size() * 2);

    for (size_t i = 0; i < atoms_.size(); ++i) {
        positionData_[i * 2] = atoms_[i].x;
        positionData_[i * 2 + 1] = atoms_[i].y;
        velocityData_[i * 2] = atoms_[i].vx;
        velocityData_[i * 2 + 1] = atoms_[i].vy;
    }
}

// ==================================================
// Presets
// ==================================================

MDConfig mdPresetSolid() {
    MDConfig config;
    config.numAtoms = 150;
    config.boxSize = 18.0f;
    config.sigma = 1.0f;
    config.epsilon = 1.0f;
    config.temperature = 0.3f;  // Low temperature
    config.dt = 0.002f;
    config.cutoff = 2.5f;
    config.useThermostat = true;
    config.thermostatTau = 1.0f;
    return config;
}

MDConfig mdPresetLiquid() {
    MDConfig config;
    config.numAtoms = 200;
    config.boxSize = 22.0f;
    config.sigma = 1.0f;
    config.epsilon = 1.0f;
    config.temperature = 1.0f;
    config.dt = 0.004f;
    config.cutoff = 2.5f;
    config.useThermostat = true;
    config.thermostatTau = 0.5f;
    return config;
}

MDConfig mdPresetGas() {
    MDConfig config;
    config.numAtoms = 100;
    config.boxSize = 35.0f;
    config.sigma = 1.0f;
    config.epsilon = 1.0f;
    config.temperature = 3.0f;  // High temperature
    config.dt = 0.005f;
    config.cutoff = 2.5f;
    config.useThermostat = true;
    config.thermostatTau = 0.3f;
    return config;
}

MDConfig mdPresetMelting() {
    MDConfig config;
    config.numAtoms = 200;
    config.boxSize = 20.0f;
    config.sigma = 1.0f;
    config.epsilon = 1.0f;
    config.temperature = 0.7f;  // Near melting point
    config.dt = 0.003f;
    config.cutoff = 2.5f;
    config.useThermostat = true;
    config.thermostatTau = 2.0f;  // Slower coupling to see transition
    return config;
}

MDConfig mdPresetLarge() {
    MDConfig config;
    config.numAtoms = 2000;
    config.boxSize = 60.0f;
    config.sigma = 1.0f;
    config.epsilon = 1.0f;
    config.temperature = 1.0f;
    config.dt = 0.004f;
    config.cutoff = 2.5f;
    config.useThermostat = true;
    config.thermostatTau = 0.5f;
    return config;
}

} // namespace physics
} // namespace eigenlab
