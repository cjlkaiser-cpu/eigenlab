/**
 * @file galaxy_collision.cpp
 * @brief Implementation of galaxy collision simulation
 *
 * Wraps GalaxySimulator with collision-specific tracking and metrics
 */

#include "../../include/physics/galaxy_collision.hpp"
#include <algorithm>
#include <cmath>

namespace eigenlab {
namespace physics {

// ============================================================================
// Constructor
// ============================================================================

GalaxyCollision::GalaxyCollision() {
    GalaxyConfig gConfig;
    gConfig.size = m_config.size;
    gConfig.maxParticles = 60000;
    gConfig.gravitationalConstant = m_config.gravitationalConstant;
    gConfig.softeningLength = m_config.softeningLength;
    gConfig.theta = m_config.theta;
    gConfig.centralBlackHole = false; // We manage black holes manually
    m_simulator.setConfig(gConfig);
}

GalaxyCollision::GalaxyCollision(const CollisionConfig& config)
    : m_config(config)
{
    GalaxyConfig gConfig;
    gConfig.size = config.size;
    gConfig.maxParticles = 60000;
    gConfig.gravitationalConstant = config.gravitationalConstant;
    gConfig.softeningLength = config.softeningLength;
    gConfig.theta = config.theta;
    gConfig.centralBlackHole = false;
    m_simulator.setConfig(gConfig);
}

void GalaxyCollision::setConfig(const CollisionConfig& config) {
    m_config = config;

    GalaxyConfig gConfig = m_simulator.config();
    gConfig.size = config.size;
    gConfig.gravitationalConstant = config.gravitationalConstant;
    gConfig.softeningLength = config.softeningLength;
    gConfig.theta = config.theta;
    m_simulator.setConfig(gConfig);
}

// ============================================================================
// Initialization
// ============================================================================

void GalaxyCollision::initialize() {
    m_simulator.clear();
    m_metrics = CollisionMetrics{};
    m_galaxy1.trajectory.clear();
    m_galaxy2.trajectory.clear();

    // Setup both galaxies
    setupGalaxy1();
    setupGalaxy2();

    // Initial metrics
    updateGalaxyTrackers();
    updateMetrics();
}

void GalaxyCollision::setupGalaxy1() {
    Real halfSize = m_config.size * 0.5f;

    // Galaxy 1 position (left side, moving right)
    Vec2 center1 = {
        halfSize - m_config.separation * 0.5f,
        halfSize + m_config.impactParameter * 0.5f
    };

    // Add central black hole for galaxy 1
    m_galaxy1.startIndex = 0;
    m_galaxy1.blackHoleIndex = m_simulator.addBlackHole(
        center1,
        {m_config.approachVelocity * 0.5f, 0},
        m_config.galaxy1Mass
    );
    m_galaxy1.blackHolePos = center1;
    m_galaxy1.blackHoleVel = {m_config.approachVelocity * 0.5f, 0};

    // Add stars for galaxy 1
    Real scaleRadius = m_config.galaxy1Radius / 3.0f;
    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_real_distribution<Real> uniform(0.0f, 1.0f);
    std::normal_distribution<Real> normal(0.0f, 1.0f);

    for (u32 i = 0; i < m_config.galaxy1Stars; ++i) {
        // Exponential disk profile
        Real u = uniform(rng);
        Real r = -scaleRadius * std::log(1.0f - u * (1.0f - std::exp(-m_config.galaxy1Radius / scaleRadius)));

        // Random angle with optional disk inclination
        Real theta = uniform(rng) * 2.0f * constants::PI;

        // Position relative to center
        Real cosAngle = std::cos(m_config.galaxy1Angle * constants::PI / 180.0f);
        Vec2 pos = {
            center1.x + r * std::cos(theta),
            center1.y + r * std::sin(theta) * cosAngle
        };

        // Orbital velocity
        Real enclosedMass = m_config.galaxy1Mass * (1.0f - std::exp(-r / scaleRadius));
        Real v = std::sqrt(m_config.gravitationalConstant * (m_config.galaxy1Mass + enclosedMass) / std::max(r, 1.0f));

        Vec2 vel = {
            -m_config.galaxy1Rotation * v * std::sin(theta) + m_config.approachVelocity * 0.5f,
            m_config.galaxy1Rotation * v * std::cos(theta) * cosAngle
        };

        // Add dispersion
        Real dispersion = v * 0.1f;
        vel.x += normal(rng) * dispersion;
        vel.y += normal(rng) * dispersion;

        // Temperature (hotter near center)
        Real temp = 3000.0f + 7000.0f * std::exp(-r / scaleRadius);

        // Random mass
        Real mass = 1.0f + uniform(rng) * 4.0f;

        m_simulator.addParticle(pos, vel, mass, ParticleType::Star);
    }

    m_galaxy1.endIndex = m_simulator.particleCount();
}

void GalaxyCollision::setupGalaxy2() {
    Real halfSize = m_config.size * 0.5f;

    // Galaxy 2 position (right side, moving left)
    Vec2 center2 = {
        halfSize + m_config.separation * 0.5f,
        halfSize - m_config.impactParameter * 0.5f
    };

    // Add central black hole for galaxy 2
    m_galaxy2.startIndex = m_simulator.particleCount();
    m_galaxy2.blackHoleIndex = m_simulator.addBlackHole(
        center2,
        {-m_config.approachVelocity * 0.5f, 0},
        m_config.galaxy2Mass
    );
    m_galaxy2.blackHolePos = center2;
    m_galaxy2.blackHoleVel = {-m_config.approachVelocity * 0.5f, 0};

    // Add stars for galaxy 2
    Real scaleRadius = m_config.galaxy2Radius / 3.0f;
    std::mt19937 rng(123); // Different seed
    std::uniform_real_distribution<Real> uniform(0.0f, 1.0f);
    std::normal_distribution<Real> normal(0.0f, 1.0f);

    for (u32 i = 0; i < m_config.galaxy2Stars; ++i) {
        Real u = uniform(rng);
        Real r = -scaleRadius * std::log(1.0f - u * (1.0f - std::exp(-m_config.galaxy2Radius / scaleRadius)));

        Real theta = uniform(rng) * 2.0f * constants::PI;

        Real cosAngle = std::cos(m_config.galaxy2Angle * constants::PI / 180.0f);
        Vec2 pos = {
            center2.x + r * std::cos(theta),
            center2.y + r * std::sin(theta) * cosAngle
        };

        Real enclosedMass = m_config.galaxy2Mass * (1.0f - std::exp(-r / scaleRadius));
        Real v = std::sqrt(m_config.gravitationalConstant * (m_config.galaxy2Mass + enclosedMass) / std::max(r, 1.0f));

        Vec2 vel = {
            -m_config.galaxy2Rotation * v * std::sin(theta) - m_config.approachVelocity * 0.5f,
            m_config.galaxy2Rotation * v * std::cos(theta) * cosAngle
        };

        Real dispersion = v * 0.1f;
        vel.x += normal(rng) * dispersion;
        vel.y += normal(rng) * dispersion;

        Real temp = 4000.0f + 5000.0f * std::exp(-r / scaleRadius);
        Real mass = 1.0f + uniform(rng) * 3.0f;

        m_simulator.addParticle(pos, vel, mass, ParticleType::Star);
    }

    m_galaxy2.endIndex = m_simulator.particleCount();
}

void GalaxyCollision::initializePreset(CollisionPreset preset) {
    switch (preset) {
        case CollisionPreset::Milkdromeda:
            m_config = presetMilkdromeda();
            break;
        case CollisionPreset::Antennae:
            m_config = presetAntennae();
            break;
        case CollisionPreset::Mice:
            m_config = presetMice();
            break;
        case CollisionPreset::Whirlpool:
            m_config = presetWhirlpool();
            break;
        case CollisionPreset::Cartwheel:
            m_config = presetCartwheel();
            break;
        default:
            break;
    }

    setConfig(m_config);
    initialize();
}

void GalaxyCollision::reset() {
    initialize();
}

// ============================================================================
// Simulation step
// ============================================================================

void GalaxyCollision::step(Real dt) {
    m_simulator.step(dt * m_config.timeScale);
    m_metrics.simulationTime += dt * m_config.timeScale;

    // Update tracking
    updateGalaxyTrackers();
    updateTrajectories();
    updateMetrics();
    detectMerger();
}

void GalaxyCollision::stepMultiple(Real dt, u32 steps) {
    for (u32 i = 0; i < steps; ++i) {
        step(dt);
    }
}

// ============================================================================
// Galaxy tracking
// ============================================================================

void GalaxyCollision::updateGalaxyTrackers() {
    const auto& particles = m_simulator.particles();

    // Galaxy 1
    m_galaxy1.centerOfMass = {0, 0};
    m_galaxy1.velocity = {0, 0};
    m_galaxy1.totalMass = 0;
    m_galaxy1.particleCount = 0;

    for (u32 i = m_galaxy1.startIndex; i < m_galaxy1.endIndex && i < particles.size(); ++i) {
        const auto& p = particles[i];
        if (!p.active) continue;

        m_galaxy1.centerOfMass.x += p.position.x * p.mass;
        m_galaxy1.centerOfMass.y += p.position.y * p.mass;
        m_galaxy1.velocity.x += p.velocity.x * p.mass;
        m_galaxy1.velocity.y += p.velocity.y * p.mass;
        m_galaxy1.totalMass += p.mass;
        m_galaxy1.particleCount++;

        // Track black hole
        if (i == m_galaxy1.blackHoleIndex) {
            m_galaxy1.blackHolePos = p.position;
            m_galaxy1.blackHoleVel = p.velocity;
        }
    }

    if (m_galaxy1.totalMass > 0) {
        m_galaxy1.centerOfMass.x /= m_galaxy1.totalMass;
        m_galaxy1.centerOfMass.y /= m_galaxy1.totalMass;
        m_galaxy1.velocity.x /= m_galaxy1.totalMass;
        m_galaxy1.velocity.y /= m_galaxy1.totalMass;
    }

    // Galaxy 2
    m_galaxy2.centerOfMass = {0, 0};
    m_galaxy2.velocity = {0, 0};
    m_galaxy2.totalMass = 0;
    m_galaxy2.particleCount = 0;

    for (u32 i = m_galaxy2.startIndex; i < m_galaxy2.endIndex && i < particles.size(); ++i) {
        const auto& p = particles[i];
        if (!p.active) continue;

        m_galaxy2.centerOfMass.x += p.position.x * p.mass;
        m_galaxy2.centerOfMass.y += p.position.y * p.mass;
        m_galaxy2.velocity.x += p.velocity.x * p.mass;
        m_galaxy2.velocity.y += p.velocity.y * p.mass;
        m_galaxy2.totalMass += p.mass;
        m_galaxy2.particleCount++;

        if (i == m_galaxy2.blackHoleIndex) {
            m_galaxy2.blackHolePos = p.position;
            m_galaxy2.blackHoleVel = p.velocity;
        }
    }

    if (m_galaxy2.totalMass > 0) {
        m_galaxy2.centerOfMass.x /= m_galaxy2.totalMass;
        m_galaxy2.centerOfMass.y /= m_galaxy2.totalMass;
        m_galaxy2.velocity.x /= m_galaxy2.totalMass;
        m_galaxy2.velocity.y /= m_galaxy2.totalMass;
    }
}

void GalaxyCollision::updateTrajectories() {
    // Add current positions to trajectory
    m_galaxy1.trajectory.push_back(m_galaxy1.blackHolePos);
    m_galaxy2.trajectory.push_back(m_galaxy2.blackHolePos);

    // Limit trajectory length
    if (m_galaxy1.trajectory.size() > m_config.trajectoryLength) {
        m_galaxy1.trajectory.erase(m_galaxy1.trajectory.begin());
    }
    if (m_galaxy2.trajectory.size() > m_config.trajectoryLength) {
        m_galaxy2.trajectory.erase(m_galaxy2.trajectory.begin());
    }
}

// ============================================================================
// Metrics computation
// ============================================================================

void GalaxyCollision::updateMetrics() {
    // Separation between black holes
    Real dx = m_galaxy2.blackHolePos.x - m_galaxy1.blackHolePos.x;
    Real dy = m_galaxy2.blackHolePos.y - m_galaxy1.blackHolePos.y;
    Real newSeparation = std::sqrt(dx * dx + dy * dy);

    // Separation velocity
    Real dvx = m_galaxy2.blackHoleVel.x - m_galaxy1.blackHoleVel.x;
    Real dvy = m_galaxy2.blackHoleVel.y - m_galaxy1.blackHoleVel.y;
    m_metrics.separationVelocity = (dx * dvx + dy * dvy) / std::max(newSeparation, 1.0f);

    // Track closest approach
    if (newSeparation < m_metrics.closestApproach) {
        m_metrics.closestApproach = newSeparation;
    }

    // Approach/recede detection
    bool wasApproaching = m_metrics.isApproaching;
    m_metrics.isApproaching = m_metrics.separationVelocity < 0;

    // Count passages (when switching from approaching to receding)
    if (wasApproaching && !m_metrics.isApproaching) {
        m_metrics.passageCount++;
    }

    m_metrics.separation = newSeparation;

    // Energetics
    computeTidalStrength();

    // Kinetic energy
    m_metrics.kineticEnergy = 0;
    const auto& particles = m_simulator.particles();
    for (const auto& p : particles) {
        if (p.active) {
            Real vSq = p.velocity.x * p.velocity.x + p.velocity.y * p.velocity.y;
            m_metrics.kineticEnergy += 0.5f * p.mass * vSq;
        }
    }

    // Binding energy (simplified: between the two black holes)
    Real reducedMass = (m_config.galaxy1Mass * m_config.galaxy2Mass) /
                       (m_config.galaxy1Mass + m_config.galaxy2Mass);
    Real relVelSq = dvx * dvx + dvy * dvy;
    m_metrics.bindingEnergy = 0.5f * reducedMass * relVelSq -
                              m_config.gravitationalConstant * m_config.galaxy1Mass * m_config.galaxy2Mass / std::max(newSeparation, 1.0f);
}

void GalaxyCollision::computeTidalStrength() {
    // Tidal strength: ratio of differential gravity to self-gravity
    // F_tidal ~ G*M2*R1/d^3
    // F_self ~ G*M1/R1^2

    Real d = m_metrics.separation;
    if (d < 1.0f) d = 1.0f;

    Real R1 = m_config.galaxy1Radius;
    Real R2 = m_config.galaxy2Radius;
    Real M1 = m_config.galaxy1Mass;
    Real M2 = m_config.galaxy2Mass;

    Real tidalForce1 = m_config.gravitationalConstant * M2 * R1 / (d * d * d);
    Real selfForce1 = m_config.gravitationalConstant * M1 / (R1 * R1);

    Real tidalForce2 = m_config.gravitationalConstant * M1 * R2 / (d * d * d);
    Real selfForce2 = m_config.gravitationalConstant * M2 / (R2 * R2);

    m_metrics.tidalStrength = std::max(tidalForce1 / selfForce1, tidalForce2 / selfForce2);

    // Compute tidal (Jacobi) radius
    m_galaxy1.tidalRadius = d * std::pow(M1 / (3.0f * M2), 1.0f / 3.0f);
    m_galaxy2.tidalRadius = d * std::pow(M2 / (3.0f * M1), 1.0f / 3.0f);
}

void GalaxyCollision::detectMerger() {
    // Consider merged if separation < sum of scale radii and bound
    Real mergeThreshold = (m_config.galaxy1Radius + m_config.galaxy2Radius) * 0.3f;

    if (m_metrics.separation < mergeThreshold && m_metrics.bindingEnergy < 0) {
        m_metrics.hasMerged = true;
    }

    // Estimate time to merger (very rough)
    if (!m_metrics.hasMerged && m_metrics.isApproaching && m_metrics.separationVelocity < -0.1f) {
        m_metrics.timeToMerger = m_metrics.separation / std::abs(m_metrics.separationVelocity);
    }
}

// ============================================================================
// Data access
// ============================================================================

u32 GalaxyCollision::particleCount() const {
    return m_simulator.particleCount();
}

u32 GalaxyCollision::activeParticleCount() const {
    return m_galaxy1.particleCount + m_galaxy2.particleCount;
}

void GalaxyCollision::updateDataBuffers() const {
    const auto& particles = m_simulator.particles();

    m_positionBuffer.clear();
    m_velocityBuffer.clear();
    m_temperatureBuffer.clear();

    m_positionBuffer.reserve(particles.size() * 2);
    m_velocityBuffer.reserve(particles.size() * 2);
    m_temperatureBuffer.reserve(particles.size());

    for (const auto& p : particles) {
        if (p.active) {
            m_positionBuffer.push_back(p.position.x);
            m_positionBuffer.push_back(p.position.y);
            m_velocityBuffer.push_back(p.velocity.x);
            m_velocityBuffer.push_back(p.velocity.y);
            m_temperatureBuffer.push_back(p.temperature);
        }
    }

    // Update trajectory buffers
    m_trajectory1Buffer.clear();
    m_trajectory2Buffer.clear();

    for (const auto& pos : m_galaxy1.trajectory) {
        m_trajectory1Buffer.push_back(pos.x);
        m_trajectory1Buffer.push_back(pos.y);
    }

    for (const auto& pos : m_galaxy2.trajectory) {
        m_trajectory2Buffer.push_back(pos.x);
        m_trajectory2Buffer.push_back(pos.y);
    }
}

const Real* GalaxyCollision::positionData() const {
    updateDataBuffers();
    return m_positionBuffer.data();
}

const Real* GalaxyCollision::velocityData() const {
    updateDataBuffers();
    return m_velocityBuffer.data();
}

const Real* GalaxyCollision::temperatureData() const {
    updateDataBuffers();
    return m_temperatureBuffer.data();
}

u32 GalaxyCollision::positionDataSize() const {
    return static_cast<u32>(m_positionBuffer.size());
}

const Real* GalaxyCollision::trajectory1Data() const {
    updateDataBuffers();
    return m_trajectory1Buffer.data();
}

const Real* GalaxyCollision::trajectory2Data() const {
    updateDataBuffers();
    return m_trajectory2Buffer.data();
}

u32 GalaxyCollision::trajectorySize() const {
    return static_cast<u32>(m_galaxy1.trajectory.size());
}

// ============================================================================
// Parameter setters
// ============================================================================

void GalaxyCollision::setGravitationalConstant(Real G) {
    m_config.gravitationalConstant = G;
    m_simulator.setGravitationalConstant(G);
}

void GalaxyCollision::setSofteningLength(Real eps) {
    m_config.softeningLength = eps;
    m_simulator.setSofteningLength(eps);
}

void GalaxyCollision::setTheta(Real theta) {
    m_config.theta = theta;
    m_simulator.setTheta(theta);
}

void GalaxyCollision::setTimeScale(Real scale) {
    m_config.timeScale = scale;
    m_simulator.setTimeScale(scale);
}

void GalaxyCollision::setGalaxy1Mass(Real mass) {
    m_config.galaxy1Mass = mass;
}

void GalaxyCollision::setGalaxy2Mass(Real mass) {
    m_config.galaxy2Mass = mass;
}

void GalaxyCollision::setApproachVelocity(Real vel) {
    m_config.approachVelocity = vel;
}

void GalaxyCollision::setImpactParameter(Real b) {
    m_config.impactParameter = b;
}

// ============================================================================
// Presets
// ============================================================================

CollisionConfig GalaxyCollision::presetMilkdromeda() const {
    CollisionConfig c;
    c.size = 3000.0f;
    c.gravitationalConstant = 1.0f;
    c.softeningLength = 5.0f;
    c.theta = 0.5f;

    // Milky Way
    c.galaxy1Stars = 12000;
    c.galaxy1Mass = 2000.0f;
    c.galaxy1Radius = 350.0f;
    c.galaxy1Rotation = 1.0f;
    c.galaxy1Angle = 0.0f;

    // Andromeda (larger)
    c.galaxy2Stars = 15000;
    c.galaxy2Mass = 3000.0f;
    c.galaxy2Radius = 400.0f;
    c.galaxy2Rotation = 1.0f;
    c.galaxy2Angle = 20.0f;

    c.separation = 900.0f;
    c.impactParameter = 150.0f;
    c.approachVelocity = 0.6f;
    c.timeScale = 1.2f;

    return c;
}

CollisionConfig GalaxyCollision::presetAntennae() const {
    CollisionConfig c;
    c.size = 2500.0f;
    c.gravitationalConstant = 1.0f;
    c.softeningLength = 4.0f;
    c.theta = 0.5f;

    // NGC 4038
    c.galaxy1Stars = 10000;
    c.galaxy1Mass = 1800.0f;
    c.galaxy1Radius = 280.0f;
    c.galaxy1Rotation = 1.0f;
    c.galaxy1Angle = 30.0f;

    // NGC 4039
    c.galaxy2Stars = 10000;
    c.galaxy2Mass = 1800.0f;
    c.galaxy2Radius = 280.0f;
    c.galaxy2Rotation = -1.0f;
    c.galaxy2Angle = -30.0f;

    c.separation = 700.0f;
    c.impactParameter = 80.0f;
    c.approachVelocity = 0.7f;
    c.timeScale = 1.0f;

    return c;
}

CollisionConfig GalaxyCollision::presetMice() const {
    CollisionConfig c;
    c.size = 3000.0f;
    c.gravitationalConstant = 1.0f;
    c.softeningLength = 5.0f;
    c.theta = 0.5f;

    // NGC 4676A
    c.galaxy1Stars = 8000;
    c.galaxy1Mass = 1500.0f;
    c.galaxy1Radius = 250.0f;
    c.galaxy1Rotation = 1.0f;
    c.galaxy1Angle = 45.0f;

    // NGC 4676B
    c.galaxy2Stars = 8000;
    c.galaxy2Mass = 1500.0f;
    c.galaxy2Radius = 250.0f;
    c.galaxy2Rotation = 1.0f;
    c.galaxy2Angle = -45.0f;

    // Long tails form from prograde encounters
    c.separation = 600.0f;
    c.impactParameter = 200.0f;  // Larger impact parameter for tails
    c.approachVelocity = 0.5f;
    c.timeScale = 1.0f;

    return c;
}

CollisionConfig GalaxyCollision::presetWhirlpool() const {
    CollisionConfig c;
    c.size = 2500.0f;
    c.gravitationalConstant = 1.0f;
    c.softeningLength = 5.0f;
    c.theta = 0.5f;

    // M51a (main galaxy)
    c.galaxy1Stars = 12000;
    c.galaxy1Mass = 2500.0f;
    c.galaxy1Radius = 350.0f;
    c.galaxy1Rotation = 1.0f;
    c.galaxy1Angle = 0.0f;

    // NGC 5195 (companion - smaller)
    c.galaxy2Stars = 4000;
    c.galaxy2Mass = 800.0f;
    c.galaxy2Radius = 150.0f;
    c.galaxy2Rotation = 1.0f;
    c.galaxy2Angle = 0.0f;

    c.separation = 500.0f;
    c.impactParameter = 50.0f;
    c.approachVelocity = 0.4f;
    c.timeScale = 1.0f;

    return c;
}

CollisionConfig GalaxyCollision::presetCartwheel() const {
    CollisionConfig c;
    c.size = 2500.0f;
    c.gravitationalConstant = 1.0f;
    c.softeningLength = 4.0f;
    c.theta = 0.5f;

    // Target galaxy (face-on disk)
    c.galaxy1Stars = 10000;
    c.galaxy1Mass = 2000.0f;
    c.galaxy1Radius = 300.0f;
    c.galaxy1Rotation = 1.0f;
    c.galaxy1Angle = 0.0f;

    // Intruder (small, fast)
    c.galaxy2Stars = 3000;
    c.galaxy2Mass = 600.0f;
    c.galaxy2Radius = 100.0f;
    c.galaxy2Rotation = 0.0f;  // No rotation
    c.galaxy2Angle = 90.0f;    // Coming from above

    // Head-on collision
    c.separation = 600.0f;
    c.impactParameter = 10.0f;  // Nearly head-on
    c.approachVelocity = 1.2f;  // Fast
    c.timeScale = 0.8f;

    return c;
}

// ============================================================================
// Namespace preset functions
// ============================================================================

namespace collision_presets {

CollisionConfig milkdromeda() {
    GalaxyCollision temp;
    return temp.presetMilkdromeda();
}

CollisionConfig antennae() {
    GalaxyCollision temp;
    return temp.presetAntennae();
}

CollisionConfig mice() {
    GalaxyCollision temp;
    return temp.presetMice();
}

CollisionConfig whirlpool() {
    GalaxyCollision temp;
    return temp.presetWhirlpool();
}

CollisionConfig cartwheel() {
    GalaxyCollision temp;
    return temp.presetCartwheel();
}

} // namespace collision_presets

} // namespace physics
} // namespace eigenlab
