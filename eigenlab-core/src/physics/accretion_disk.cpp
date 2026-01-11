#include "../../include/physics/accretion_disk.hpp"
#include <cmath>
#include <algorithm>

namespace eigenlab {
namespace physics {

AccretionDisk::AccretionDisk()
    : numParticles_(0)
    , accretedCount_(0)
    , emitAccum_(0)
    , rng_(std::random_device{}())
{
    setConfig(AccretionConfig{});
}

AccretionDisk::AccretionDisk(const AccretionConfig& config)
    : numParticles_(0)
    , accretedCount_(0)
    , emitAccum_(0)
    , rng_(std::random_device{}())
{
    setConfig(config);
}

void AccretionDisk::setConfig(const AccretionConfig& config) {
    config_ = config;

    particles_.reserve(config.numParticles);
    positions_.reserve(config.numParticles * 3);
    velocities_.reserve(config.numParticles * 3);
    temperatures_.reserve(config.numParticles);

    reset();
}

void AccretionDisk::reset() {
    particles_.clear();
    numParticles_ = 0;
    accretedCount_ = 0;
    emitAccum_ = 0;

    // Initialize with particles distributed in the disk
    std::uniform_real_distribution<float> angleDist(0, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> zDist(-1.0f, 1.0f);

    int initialCount = config_.numParticles / 2;
    for (int i = 0; i < initialCount; ++i) {
        // Distribute radially with r^(-1) density (more particles near center)
        float u = static_cast<float>(i + 1) / (initialCount + 1);
        float r = config_.innerRadius + (config_.outerRadius - config_.innerRadius) * std::sqrt(u);

        float angle = angleDist(rng_);
        float z = zDist(rng_) * config_.diskHeight * r;

        Particle p;
        p.x = r * std::cos(angle);
        p.y = r * std::sin(angle);
        p.z = z;
        p.r = r;

        // Keplerian velocity (circular orbit)
        float vk = keplerianVelocity(r);
        p.vx = -vk * std::sin(angle);
        p.vy = vk * std::cos(angle);
        p.vz = 0;

        p.temperature = diskTemperature(r);
        p.active = true;

        particles_.push_back(p);
        numParticles_++;
    }

    updateDataBuffers();
}

// ==================================================
// Physics
// ==================================================

float AccretionDisk::keplerianVelocity(float r) const {
    // v_k = sqrt(GM/r)
    if (r < 0.1f) r = 0.1f;
    return std::sqrt(config_.G * config_.centralMass / r);
}

float AccretionDisk::diskTemperature(float r) const {
    // Standard thin disk: T ~ r^(-3/4)
    // T = T_inner * (r_inner / r)^(3/4)
    float T_inner = 1000.0f;  // Arbitrary units
    float ratio = config_.innerRadius / std::max(r, config_.innerRadius);
    return T_inner * std::pow(ratio, 0.75f);
}

void AccretionDisk::emitParticles() {
    float dt = config_.dt;
    emitAccum_ += config_.emissionRate * dt;

    std::uniform_real_distribution<float> angleDist(0, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> radiusDist(0.9f, 1.0f);
    std::uniform_real_distribution<float> zDist(-1.0f, 1.0f);

    while (emitAccum_ >= 1.0f && numParticles_ < config_.numParticles) {
        float r = config_.outerRadius * radiusDist(rng_);
        float angle = angleDist(rng_);
        float z = zDist(rng_) * config_.diskHeight * r;

        Particle p;
        p.x = r * std::cos(angle);
        p.y = r * std::sin(angle);
        p.z = z;
        p.r = r;

        float vk = keplerianVelocity(r);
        // Slight inward drift
        float inwardVel = config_.accretionRate * vk * 0.1f;
        p.vx = -vk * std::sin(angle) - inwardVel * std::cos(angle);
        p.vy = vk * std::cos(angle) - inwardVel * std::sin(angle);
        p.vz = 0;

        p.temperature = diskTemperature(r);
        p.active = true;

        particles_.push_back(p);
        numParticles_++;
        emitAccum_ -= 1.0f;
    }
}

void AccretionDisk::updateParticle(Particle& p, float dt) {
    if (!p.active) return;

    // Calculate radius
    float r = std::sqrt(p.x * p.x + p.y * p.y);
    if (r < 0.01f) r = 0.01f;
    p.r = r;

    // Gravitational acceleration (towards center)
    float a = -config_.G * config_.centralMass / (r * r);
    float ax = a * p.x / r;
    float ay = a * p.y / r;
    float az = -config_.G * config_.centralMass * p.z / (r * r * r) * 10.0f;  // Restore to disk plane

    // Viscous torque (angular momentum transport)
    // This causes slow inward drift
    float vTheta = (-p.y * p.vx + p.x * p.vy) / r;  // Tangential velocity
    float vk = keplerianVelocity(r);

    // Viscous acceleration (inward radial drift)
    float viscAccel = -config_.viscosity * vk / r;
    ax += viscAccel * p.x / r;
    ay += viscAccel * p.y / r;

    // Update velocity
    p.vx += ax * dt;
    p.vy += ay * dt;
    p.vz += az * dt;

    // Damping for z oscillations
    p.vz *= 0.99f;

    // Update position
    p.x += p.vx * dt;
    p.y += p.vy * dt;
    p.z += p.vz * dt;

    // Update temperature
    p.temperature = diskTemperature(p.r);

    // Check for accretion (fell into central object)
    if (p.r < config_.innerRadius) {
        p.active = false;
        accretedCount_++;
    }

    // Check for escape
    if (p.r > config_.outerRadius * 1.5f) {
        p.active = false;
    }
}

void AccretionDisk::step() {
    float dt = config_.dt;

    // Emit new particles
    emitParticles();

    // Update all particles
    for (auto& p : particles_) {
        updateParticle(p, dt);
    }

    // Remove inactive particles (compact the vector periodically)
    if (particles_.size() > config_.numParticles * 1.2) {
        particles_.erase(
            std::remove_if(particles_.begin(), particles_.end(),
                [](const Particle& p) { return !p.active; }),
            particles_.end()
        );
        numParticles_ = static_cast<int>(particles_.size());
    }

    updateDataBuffers();
}

void AccretionDisk::stepMultiple(int steps) {
    for (int i = 0; i < steps; ++i) {
        step();
    }
    computeStatistics();
}

// ==================================================
// Control
// ==================================================

void AccretionDisk::setCentralMass(float mass) {
    config_.centralMass = mass;
}

void AccretionDisk::setViscosity(float visc) {
    config_.viscosity = visc;
}

void AccretionDisk::setEmissionRate(float rate) {
    config_.emissionRate = rate;
}

void AccretionDisk::addParticleBurst(float radius, int count) {
    std::uniform_real_distribution<float> angleDist(0, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> radiusDist(0.9f, 1.1f);
    std::uniform_real_distribution<float> zDist(-1.0f, 1.0f);

    for (int i = 0; i < count && numParticles_ < config_.numParticles; ++i) {
        float r = radius * radiusDist(rng_);
        float angle = angleDist(rng_);
        float z = zDist(rng_) * config_.diskHeight * r;

        Particle p;
        p.x = r * std::cos(angle);
        p.y = r * std::sin(angle);
        p.z = z;
        p.r = r;

        float vk = keplerianVelocity(r);
        p.vx = -vk * std::sin(angle);
        p.vy = vk * std::cos(angle);
        p.vz = 0;

        p.temperature = diskTemperature(r);
        p.active = true;

        particles_.push_back(p);
        numParticles_++;
    }
}

// ==================================================
// Statistics
// ==================================================

void AccretionDisk::computeStatistics() {
    float sumL = 0;
    float sumT = 0;
    float sumMass = 0;
    int active = 0;

    for (const auto& p : particles_) {
        if (!p.active) continue;

        // Angular momentum: L = r x v (z component for 2D)
        float L = p.x * p.vy - p.y * p.vx;
        sumL += std::abs(L);

        sumT += p.temperature;
        sumMass += 1.0f;  // Unit mass per particle
        active++;
    }

    stats_.numParticles = active;
    stats_.particlesAccreted = accretedCount_;
    stats_.totalAngularMomentum = sumL;
    stats_.avgTemperature = active > 0 ? sumT / active : 0;
    stats_.diskMass = sumMass;

    // Luminosity from accretion (L ~ dM/dt * GM/r_inner)
    float accretionPower = accretedCount_ * config_.G * config_.centralMass / config_.innerRadius;
    stats_.luminosity = accretionPower * 0.1f;  // Efficiency factor
}

void AccretionDisk::updateDataBuffers() {
    // Count active particles
    int activeCount = 0;
    for (const auto& p : particles_) {
        if (p.active) activeCount++;
    }

    positions_.resize(activeCount * 3);
    velocities_.resize(activeCount * 3);
    temperatures_.resize(activeCount);

    int idx = 0;
    for (const auto& p : particles_) {
        if (!p.active) continue;

        positions_[idx * 3] = p.x;
        positions_[idx * 3 + 1] = p.y;
        positions_[idx * 3 + 2] = p.z;

        velocities_[idx * 3] = p.vx;
        velocities_[idx * 3 + 1] = p.vy;
        velocities_[idx * 3 + 2] = p.vz;

        temperatures_[idx] = p.temperature;
        idx++;
    }

    numParticles_ = activeCount;
}

// ==================================================
// Presets
// ==================================================

AccretionConfig accretionPresetProtostar() {
    AccretionConfig config;
    config.numParticles = 5000;
    config.centralMass = 500.0f;
    config.innerRadius = 8.0f;
    config.outerRadius = 100.0f;
    config.diskHeight = 0.15f;
    config.viscosity = 0.02f;
    config.accretionRate = 0.05f;
    config.emissionRate = 30.0f;
    config.dt = 0.02f;
    return config;
}

AccretionConfig accretionPresetBlackHole() {
    AccretionConfig config;
    config.numParticles = 8000;
    config.centralMass = 2000.0f;
    config.innerRadius = 3.0f;    // Closer to ISCO
    config.outerRadius = 80.0f;
    config.diskHeight = 0.05f;    // Thinner disk
    config.viscosity = 0.03f;
    config.accretionRate = 0.15f;
    config.emissionRate = 60.0f;
    config.dt = 0.01f;
    return config;
}

AccretionConfig accretionPresetCompact() {
    AccretionConfig config;
    config.numParticles = 3000;
    config.centralMass = 1500.0f;
    config.innerRadius = 5.0f;
    config.outerRadius = 50.0f;
    config.diskHeight = 0.08f;
    config.viscosity = 0.025f;
    config.accretionRate = 0.1f;
    config.emissionRate = 40.0f;
    config.dt = 0.015f;
    return config;
}

AccretionConfig accretionPresetThin() {
    AccretionConfig config;
    config.numParticles = 6000;
    config.centralMass = 800.0f;
    config.innerRadius = 10.0f;
    config.outerRadius = 120.0f;
    config.diskHeight = 0.02f;    // Very thin
    config.viscosity = 0.01f;
    config.accretionRate = 0.03f;
    config.emissionRate = 25.0f;
    config.dt = 0.02f;
    return config;
}

} // namespace physics
} // namespace eigenlab
