#include "physics/plasma_particles.hpp"
#include <cmath>
#include <algorithm>

PlasmaParticles::PlasmaParticles() : m_rng(42), m_stepCount(0) {
    init();
}

PlasmaParticles::PlasmaParticles(const PlasmaConfig& config)
    : m_config(config), m_rng(42), m_stepCount(0) {
    init();
}

void PlasmaParticles::init() {
    m_stepCount = 0;
    initParticles();
    updateArrays();
}

void PlasmaParticles::initParticles() {
    m_particles.clear();
    m_particles.reserve(m_config.numParticles);

    std::uniform_real_distribution<float> posDist(-m_config.domainSize * 0.4f, m_config.domainSize * 0.4f);

    // Thermal velocity from temperature: v_th = sqrt(k_B * T / m)
    // Using normalized units where k_B = 1
    float electronThermalVel = std::sqrt(m_config.temperature / m_config.electronMass) * 0.001f;
    float ionThermalVel = std::sqrt(m_config.temperature / m_config.ionMass) * 0.001f;

    int numElectrons = static_cast<int>(m_config.numParticles * m_config.electronRatio);

    for (int i = 0; i < m_config.numParticles; ++i) {
        PlasmaParticle p;

        // Position - uniform in domain
        p.x = posDist(m_rng);
        p.y = posDist(m_rng);
        p.z = posDist(m_rng);

        // Determine particle type
        if (i < numElectrons) {
            p.type = PlasmaParticleType::Electron;
            p.charge = m_config.electronCharge;
            p.mass = m_config.electronMass;

            // Maxwell-Boltzmann distribution for electrons
            std::normal_distribution<float> velDist(0.0f, electronThermalVel);
            p.vx = velDist(m_rng);
            p.vy = velDist(m_rng);
            p.vz = velDist(m_rng);
        } else {
            p.type = PlasmaParticleType::Ion;
            p.charge = m_config.ionCharge;
            p.mass = m_config.ionMass;

            // Maxwell-Boltzmann distribution for ions (slower due to mass)
            std::normal_distribution<float> velDist(0.0f, ionThermalVel);
            p.vx = velDist(m_rng);
            p.vy = velDist(m_rng);
            p.vz = velDist(m_rng);
        }

        // Compute initial energy
        float v2 = p.vx * p.vx + p.vy * p.vy + p.vz * p.vz;
        p.energy = 0.5f * p.mass * v2;

        m_particles.push_back(p);
    }
}

void PlasmaParticles::updateArrays() {
    int n = static_cast<int>(m_particles.size());

    m_positions.resize(n * 3);
    m_velocities.resize(n * 3);
    m_colors.resize(n * 3);
    m_charges.resize(n);

    for (int i = 0; i < n; ++i) {
        const PlasmaParticle& p = m_particles[i];

        m_positions[i * 3 + 0] = p.x;
        m_positions[i * 3 + 1] = p.y;
        m_positions[i * 3 + 2] = p.z;

        m_velocities[i * 3 + 0] = p.vx;
        m_velocities[i * 3 + 1] = p.vy;
        m_velocities[i * 3 + 2] = p.vz;

        m_charges[i] = p.charge;

        float r, g, b;
        computeParticleColor(p, r, g, b);
        m_colors[i * 3 + 0] = r;
        m_colors[i * 3 + 1] = g;
        m_colors[i * 3 + 2] = b;
    }
}

void PlasmaParticles::getMagneticField(float x, float y, float z,
                                        float& bx, float& by, float& bz) const {
    float B0 = m_config.magneticFieldStrength;
    float angle = m_config.magneticFieldAngle;

    if (m_config.enableMagneticMirror) {
        // Magnetic mirror: B increases toward ends
        // B(z) = B0 * (1 + (mirrorRatio-1) * (2z/L)^2)
        float L = m_config.domainSize;
        float zNorm = 2.0f * z / L;
        float mirrorFactor = 1.0f + (m_config.mirrorRatio - 1.0f) * zNorm * zNorm;

        bx = 0.0f;
        by = 0.0f;
        bz = B0 * mirrorFactor;
    } else {
        // Uniform field at specified angle
        bx = B0 * std::sin(angle);
        by = 0.0f;
        bz = B0 * std::cos(angle);
    }
}

void PlasmaParticles::getElectricField(float /*x*/, float /*y*/, float /*z*/,
                                        float& ex, float& ey, float& ez) const {
    // Uniform electric field (could be made position-dependent)
    float E0 = m_config.electricFieldStrength;

    ex = 0.0f;
    ey = E0;  // E field in y direction
    ez = 0.0f;
}

void PlasmaParticles::computeLorentzForce(const PlasmaParticle& p,
                                           float& fx, float& fy, float& fz) const {
    float bx, by, bz;
    getMagneticField(p.x, p.y, p.z, bx, by, bz);

    float ex, ey, ez;
    getElectricField(p.x, p.y, p.z, ex, ey, ez);

    // Lorentz force: F = q(E + v × B)
    // v × B = (vy*bz - vz*by, vz*bx - vx*bz, vx*by - vy*bx)
    float crossX = p.vy * bz - p.vz * by;
    float crossY = p.vz * bx - p.vx * bz;
    float crossZ = p.vx * by - p.vy * bx;

    fx = p.charge * (ex + crossX);
    fy = p.charge * (ey + crossY);
    fz = p.charge * (ez + crossZ);
}

void PlasmaParticles::computeCoulombForce(int idx, float& fx, float& fy, float& fz) const {
    fx = fy = fz = 0.0f;

    if (!m_config.enableCoulomb) return;

    const PlasmaParticle& p1 = m_particles[idx];
    float k = m_config.coulombStrength;

    for (size_t j = 0; j < m_particles.size(); ++j) {
        if (static_cast<int>(j) == idx) continue;

        const PlasmaParticle& p2 = m_particles[j];

        float dx = p1.x - p2.x;
        float dy = p1.y - p2.y;
        float dz = p1.z - p2.z;

        float r2 = dx * dx + dy * dy + dz * dz;
        float minR2 = 0.1f;  // Softening to prevent singularity
        r2 = std::max(r2, minR2);

        float r = std::sqrt(r2);
        float r3 = r * r2;

        // Coulomb force: F = k * q1 * q2 / r^2, direction along r
        float forceMag = k * p1.charge * p2.charge / r2;

        fx += forceMag * dx / r;
        fy += forceMag * dy / r;
        fz += forceMag * dz / r;
    }
}

void PlasmaParticles::applyBoundary(PlasmaParticle& p) {
    float halfSize = m_config.domainSize * 0.5f;

    if (m_config.periodicBoundary) {
        // Periodic boundary conditions
        if (p.x > halfSize) p.x -= m_config.domainSize;
        if (p.x < -halfSize) p.x += m_config.domainSize;
        if (p.y > halfSize) p.y -= m_config.domainSize;
        if (p.y < -halfSize) p.y += m_config.domainSize;
        if (p.z > halfSize) p.z -= m_config.domainSize;
        if (p.z < -halfSize) p.z += m_config.domainSize;
    } else {
        // Reflective boundary
        if (p.x > halfSize) { p.x = halfSize; p.vx *= -1.0f; }
        if (p.x < -halfSize) { p.x = -halfSize; p.vx *= -1.0f; }
        if (p.y > halfSize) { p.y = halfSize; p.vy *= -1.0f; }
        if (p.y < -halfSize) { p.y = -halfSize; p.vy *= -1.0f; }
        if (p.z > halfSize) { p.z = halfSize; p.vz *= -1.0f; }
        if (p.z < -halfSize) { p.z = -halfSize; p.vz *= -1.0f; }
    }
}

void PlasmaParticles::computeParticleColor(const PlasmaParticle& p,
                                            float& r, float& g, float& b) const {
    // Color by particle type and energy
    float speed = std::sqrt(p.vx * p.vx + p.vy * p.vy + p.vz * p.vz);
    float energyFactor = std::min(speed * 10.0f, 1.0f);

    if (p.type == PlasmaParticleType::Electron) {
        // Electrons: cyan to white (high energy)
        r = 0.2f + 0.8f * energyFactor;
        g = 0.8f + 0.2f * energyFactor;
        b = 1.0f;
    } else {
        // Ions: orange to yellow (high energy)
        r = 1.0f;
        g = 0.4f + 0.6f * energyFactor;
        b = 0.1f + 0.4f * energyFactor;
    }
}

void PlasmaParticles::step() {
    float dt = m_config.dt;

    // Boris algorithm for stable Lorentz force integration
    for (size_t i = 0; i < m_particles.size(); ++i) {
        PlasmaParticle& p = m_particles[i];

        // Get fields at particle position
        float bx, by, bz;
        getMagneticField(p.x, p.y, p.z, bx, by, bz);

        float ex, ey, ez;
        getElectricField(p.x, p.y, p.z, ex, ey, ez);

        // Add Coulomb force if enabled
        float coulombFx = 0, coulombFy = 0, coulombFz = 0;
        if (m_config.enableCoulomb) {
            computeCoulombForce(static_cast<int>(i), coulombFx, coulombFy, coulombFz);
        }

        // Boris push algorithm
        float qmdt = p.charge / p.mass * dt * 0.5f;

        // Half electric field acceleration
        float vxMinus = p.vx + qmdt * ex + coulombFx / p.mass * dt * 0.5f;
        float vyMinus = p.vy + qmdt * ey + coulombFy / p.mass * dt * 0.5f;
        float vzMinus = p.vz + qmdt * ez + coulombFz / p.mass * dt * 0.5f;

        // Rotation from magnetic field
        float tx = qmdt * bx;
        float ty = qmdt * by;
        float tz = qmdt * bz;
        float t2 = tx * tx + ty * ty + tz * tz;
        float sx = 2.0f * tx / (1.0f + t2);
        float sy = 2.0f * ty / (1.0f + t2);
        float sz = 2.0f * tz / (1.0f + t2);

        // v' = v- + v- × t
        float vxPrime = vxMinus + (vyMinus * tz - vzMinus * ty);
        float vyPrime = vyMinus + (vzMinus * tx - vxMinus * tz);
        float vzPrime = vzMinus + (vxMinus * ty - vyMinus * tx);

        // v+ = v- + v' × s
        float vxPlus = vxMinus + (vyPrime * sz - vzPrime * sy);
        float vyPlus = vyMinus + (vzPrime * sx - vxPrime * sz);
        float vzPlus = vzMinus + (vxPrime * sy - vyPrime * sx);

        // Final half electric acceleration
        p.vx = vxPlus + qmdt * ex + coulombFx / p.mass * dt * 0.5f;
        p.vy = vyPlus + qmdt * ey + coulombFy / p.mass * dt * 0.5f;
        p.vz = vzPlus + qmdt * ez + coulombFz / p.mass * dt * 0.5f;

        // Update position
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;

        // Apply boundary conditions
        applyBoundary(p);

        // Update energy
        float v2 = p.vx * p.vx + p.vy * p.vy + p.vz * p.vz;
        p.energy = 0.5f * p.mass * v2;
    }

    m_stepCount++;
    updateArrays();
}

void PlasmaParticles::step(int numSteps) {
    for (int i = 0; i < numSteps; ++i) {
        step();
    }
}

void PlasmaParticles::setMagneticField(float strength, float angle) {
    m_config.magneticFieldStrength = strength;
    m_config.magneticFieldAngle = angle;
}

void PlasmaParticles::setElectricField(float strength) {
    m_config.electricFieldStrength = strength;
}

void PlasmaParticles::setTemperature(float temp) {
    m_config.temperature = temp;
}

void PlasmaParticles::enableCoulombInteractions(bool enable) {
    m_config.enableCoulomb = enable;
}

void PlasmaParticles::enableMagneticMirror(bool enable, float ratio) {
    m_config.enableMagneticMirror = enable;
    m_config.mirrorRatio = ratio;
}

// Presets
void PlasmaParticles::presetTokamak() {
    m_config.magneticFieldStrength = 2.0f;
    m_config.electricFieldStrength = 0.1f;
    m_config.enableMagneticMirror = false;
    m_config.periodicBoundary = true;
    m_config.temperature = 10000.0f;
    m_config.electronRatio = 0.5f;
    init();
}

void PlasmaParticles::presetAurora() {
    m_config.magneticFieldStrength = 0.5f;
    m_config.magneticFieldAngle = 0.3f;  // Slightly angled field
    m_config.electricFieldStrength = 0.8f;
    m_config.enableMagneticMirror = true;
    m_config.mirrorRatio = 3.0f;
    m_config.periodicBoundary = false;
    m_config.temperature = 5000.0f;
    m_config.electronRatio = 0.8f;  // More electrons for aurora effect
    init();
}

void PlasmaParticles::presetCyclotron() {
    m_config.magneticFieldStrength = 3.0f;
    m_config.magneticFieldAngle = 0.0f;  // Field along z
    m_config.electricFieldStrength = 0.0f;
    m_config.enableMagneticMirror = false;
    m_config.periodicBoundary = true;
    m_config.temperature = 2000.0f;
    m_config.electronRatio = 0.5f;
    init();
}

void PlasmaParticles::presetMagneticBottle() {
    m_config.magneticFieldStrength = 1.5f;
    m_config.electricFieldStrength = 0.0f;
    m_config.enableMagneticMirror = true;
    m_config.mirrorRatio = 4.0f;
    m_config.periodicBoundary = false;
    m_config.temperature = 3000.0f;
    m_config.electronRatio = 0.6f;
    init();
}

PlasmaStats PlasmaParticles::getStats() const {
    PlasmaStats stats = {};

    if (m_particles.empty()) return stats;

    stats.particleCount = static_cast<int>(m_particles.size());

    float totalKE = 0.0f;
    float totalSpeed = 0.0f;
    float maxSpeed = 0.0f;
    float electronKE = 0.0f;
    float ionKE = 0.0f;

    for (const auto& p : m_particles) {
        float v2 = p.vx * p.vx + p.vy * p.vy + p.vz * p.vz;
        float speed = std::sqrt(v2);
        float ke = 0.5f * p.mass * v2;

        totalKE += ke;
        totalSpeed += speed;
        maxSpeed = std::max(maxSpeed, speed);

        if (p.type == PlasmaParticleType::Electron) {
            stats.electronCount++;
            electronKE += ke;
        } else {
            stats.ionCount++;
            ionKE += ke;
        }
    }

    stats.totalKineticEnergy = totalKE;
    stats.avgSpeed = totalSpeed / stats.particleCount;
    stats.maxSpeed = maxSpeed;

    // Temperature from kinetic energy: T = 2/3 * KE / (n * k_B), using k_B = 1
    if (stats.electronCount > 0) {
        stats.electronTemperature = (2.0f / 3.0f) * electronKE / stats.electronCount;
    }
    if (stats.ionCount > 0) {
        stats.ionTemperature = (2.0f / 3.0f) * ionKE / stats.ionCount;
    }

    // Gyration radius: r_g = m*v_perp / (q*B)
    // Using average perpendicular velocity
    if (m_config.magneticFieldStrength > 0) {
        float avgPerpVel = stats.avgSpeed * 0.707f;  // Approximate
        stats.gyrationRadius = m_config.electronMass * avgPerpVel /
                               (std::abs(m_config.electronCharge) * m_config.magneticFieldStrength);
    }

    return stats;
}

void PlasmaParticles::sampleMagneticField(float x, float y, float z,
                                           float& bx, float& by, float& bz) const {
    getMagneticField(x, y, z, bx, by, bz);
}

void PlasmaParticles::sampleElectricField(float x, float y, float z,
                                           float& ex, float& ey, float& ez) const {
    getElectricField(x, y, z, ex, ey, ez);
}
