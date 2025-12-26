/**
 * @file galaxy.cpp
 * @brief Implementation of N-body gravitational simulation
 *
 * Uses Barnes-Hut quadtree for O(n log n) complexity
 */

#include "../../include/physics/galaxy.hpp"
#include <algorithm>
#include <numeric>

namespace eigenlab {
namespace physics {

// ============================================================================
// Constructor
// ============================================================================

GalaxySimulator::GalaxySimulator()
    : m_rng(std::random_device{}())
{
    m_particles.reserve(m_config.maxParticles);
}

GalaxySimulator::GalaxySimulator(const GalaxyConfig& config)
    : m_config(config)
    , m_rng(std::random_device{}())
{
    m_particles.reserve(config.maxParticles);
}

void GalaxySimulator::setConfig(const GalaxyConfig& config) {
    m_config = config;
}

// ============================================================================
// Initialization
// ============================================================================

void GalaxySimulator::clear() {
    m_particles.clear();
    m_root.reset();
    m_stats = GalaxyStats{};
}

void GalaxySimulator::initializeDiskGalaxy(u32 count, Vec2 center, Real radius, Real rotation) {
    // Scale radius for exponential disk
    Real scaleRadius = radius / 3.0f;

    // Add central black hole
    if (m_config.centralBlackHole) {
        addBlackHole(center, {0, 0}, m_config.blackHoleMass);
    }

    // Generate stars in exponential disk profile
    for (u32 i = 0; i < count && m_particles.size() < m_config.maxParticles; ++i) {
        // Sample radius from exponential distribution
        Real u = m_uniformDist(m_rng);
        Real r = -scaleRadius * std::log(1.0f - u * (1.0f - std::exp(-radius / scaleRadius)));

        // Random angle
        Real theta = m_uniformDist(m_rng) * 2.0f * constants::PI;

        // Position
        Vec2 pos = {
            center.x + r * std::cos(theta),
            center.y + r * std::sin(theta)
        };

        // Calculate orbital velocity for circular orbit
        // Enclosed mass = black hole + disk mass within r
        Real enclosedMass = m_config.blackHoleMass;
        enclosedMass += m_config.blackHoleMass * 0.5f * (1.0f - std::exp(-r / scaleRadius));

        Real v = orbitalVelocity(r, enclosedMass);

        // Tangential velocity (perpendicular to radius)
        Vec2 vel = {
            -rotation * v * std::sin(theta),
            rotation * v * std::cos(theta)
        };

        // Add some random dispersion
        Real dispersion = v * 0.1f;
        vel.x += m_normalDist(m_rng) * dispersion;
        vel.y += m_normalDist(m_rng) * dispersion;

        // Mass varies slightly
        Real mass = m_config.minMass + m_uniformDist(m_rng) * (m_config.maxMass - m_config.minMass);

        // Temperature based on radius (hotter near center)
        Real temp = 3000.0f + 7000.0f * std::exp(-r / scaleRadius);

        GalaxyParticle p;
        p.position = pos;
        p.velocity = vel;
        p.acceleration = {0, 0};
        p.mass = mass;
        p.type = ParticleType::Star;
        p.temperature = temp;
        p.active = true;

        m_particles.push_back(p);
    }
}

void GalaxySimulator::initializeEllipticalGalaxy(u32 count, Vec2 center, Real radius) {
    // Add central black hole
    if (m_config.centralBlackHole) {
        addBlackHole(center, {0, 0}, m_config.blackHoleMass);
    }

    // Plummer model for elliptical galaxy
    Real plummerRadius = radius / 2.0f;

    for (u32 i = 0; i < count && m_particles.size() < m_config.maxParticles; ++i) {
        // Sample from Plummer distribution
        Real u = m_uniformDist(m_rng);
        Real r = plummerRadius / std::sqrt(std::pow(u, -2.0f/3.0f) - 1.0f);
        r = std::min(r, radius);

        // Isotropic distribution
        Real theta = m_uniformDist(m_rng) * 2.0f * constants::PI;

        Vec2 pos = {
            center.x + r * std::cos(theta),
            center.y + r * std::sin(theta)
        };

        // Velocity dispersion (no net rotation)
        Real sigma = std::sqrt(m_config.gravitationalConstant * m_config.blackHoleMass /
                               std::sqrt(r * r + plummerRadius * plummerRadius));

        Vec2 vel = {
            m_normalDist(m_rng) * sigma * 0.5f,
            m_normalDist(m_rng) * sigma * 0.5f
        };

        Real mass = m_config.minMass + m_uniformDist(m_rng) * (m_config.maxMass - m_config.minMass);
        Real temp = 4000.0f + 3000.0f * std::exp(-r / plummerRadius);

        GalaxyParticle p;
        p.position = pos;
        p.velocity = vel;
        p.acceleration = {0, 0};
        p.mass = mass;
        p.type = ParticleType::Star;
        p.temperature = temp;
        p.active = true;

        m_particles.push_back(p);
    }
}

void GalaxySimulator::initializeCollision(u32 countEach, Real separation, Real impactParam) {
    Real halfSize = m_config.size * 0.5f;
    Real galaxyRadius = m_config.size * 0.15f;

    // First galaxy (left, moving right)
    Vec2 center1 = {halfSize - separation * 0.5f, halfSize + impactParam};
    initializeDiskGalaxy(countEach, center1, galaxyRadius, 1.0f);

    // Adjust velocity for approach
    Real approachVel = 0.5f;
    for (u32 i = 0; i < m_particles.size(); ++i) {
        m_particles[i].velocity.x += approachVel;
    }

    // Second galaxy (right, moving left, opposite rotation)
    u32 firstGalaxyEnd = static_cast<u32>(m_particles.size());
    Vec2 center2 = {halfSize + separation * 0.5f, halfSize - impactParam};
    initializeDiskGalaxy(countEach, center2, galaxyRadius, -1.0f);

    // Adjust velocity for approach
    for (u32 i = firstGalaxyEnd; i < m_particles.size(); ++i) {
        m_particles[i].velocity.x -= approachVel;
    }
}

void GalaxySimulator::initializeRandom(u32 count) {
    clear();

    Real halfSize = m_config.size * 0.5f;

    for (u32 i = 0; i < count && m_particles.size() < m_config.maxParticles; ++i) {
        GalaxyParticle p;
        p.position.x = m_uniformDist(m_rng) * m_config.size;
        p.position.y = m_uniformDist(m_rng) * m_config.size;
        p.velocity.x = (m_uniformDist(m_rng) - 0.5f) * 2.0f;
        p.velocity.y = (m_uniformDist(m_rng) - 0.5f) * 2.0f;
        p.acceleration = {0, 0};
        p.mass = m_config.minMass + m_uniformDist(m_rng) * (m_config.maxMass - m_config.minMass);
        p.type = ParticleType::Star;
        p.temperature = 3000.0f + m_uniformDist(m_rng) * 7000.0f;
        p.active = true;

        m_particles.push_back(p);
    }
}

// ============================================================================
// Particle management
// ============================================================================

u32 GalaxySimulator::addParticle(Vec2 pos, Vec2 vel, Real mass, ParticleType type) {
    if (m_particles.size() >= m_config.maxParticles) {
        return UINT32_MAX;
    }

    GalaxyParticle p;
    p.position = pos;
    p.velocity = vel;
    p.acceleration = {0, 0};
    p.mass = mass;
    p.type = type;
    p.temperature = 5000.0f;
    p.active = true;

    m_particles.push_back(p);
    return static_cast<u32>(m_particles.size() - 1);
}

u32 GalaxySimulator::addBlackHole(Vec2 pos, Vec2 vel, Real mass) {
    if (m_particles.size() >= m_config.maxParticles) {
        return UINT32_MAX;
    }

    GalaxyParticle p;
    p.position = pos;
    p.velocity = vel;
    p.acceleration = {0, 0};
    p.mass = mass;
    p.type = ParticleType::BlackHole;
    p.temperature = 0;
    p.active = true;

    m_particles.push_back(p);
    return static_cast<u32>(m_particles.size() - 1);
}

void GalaxySimulator::removeParticle(u32 index) {
    if (index < m_particles.size()) {
        m_particles[index].active = false;
    }
}

// ============================================================================
// Barnes-Hut Tree
// ============================================================================

void GalaxySimulator::buildTree() {
    // Create root node encompassing all particles
    m_root = std::make_unique<QuadTreeNode>(
        Vec2{m_config.size * 0.5f, m_config.size * 0.5f},
        m_config.size * 0.5f
    );

    m_stats.treeNodes = 1;

    // Insert all active particles
    for (u32 i = 0; i < m_particles.size(); ++i) {
        if (m_particles[i].active) {
            insertParticle(m_root.get(), i);
        }
    }
}

void GalaxySimulator::insertParticle(QuadTreeNode* node, u32 particleIndex) {
    const GalaxyParticle& p = m_particles[particleIndex];

    if (node->isLeaf()) {
        if (node->particleIndex < 0) {
            // Empty leaf - just store the particle
            node->particleIndex = static_cast<i32>(particleIndex);
            node->totalMass = p.mass;
            node->centerOfMass = p.position;
        } else {
            // Leaf with existing particle - subdivide
            i32 existingIndex = node->particleIndex;
            node->particleIndex = -1; // Now internal node

            // Create children
            for (i32 q = 0; q < 4; ++q) {
                node->children[q] = std::make_unique<QuadTreeNode>(
                    node->getChildCenter(q),
                    node->halfSize * 0.5f
                );
                m_stats.treeNodes++;
            }

            // Reinsert existing particle
            if (existingIndex >= 0 && existingIndex < static_cast<i32>(m_particles.size())) {
                const GalaxyParticle& existing = m_particles[existingIndex];
                i32 eq = node->getQuadrant(existing.position);
                insertParticle(node->children[eq].get(), static_cast<u32>(existingIndex));
            }

            // Insert new particle
            i32 q = node->getQuadrant(p.position);
            insertParticle(node->children[q].get(), particleIndex);

            // Update mass and center of mass
            node->totalMass = 0;
            node->centerOfMass = {0, 0};
            for (i32 i = 0; i < 4; ++i) {
                if (node->children[i] && node->children[i]->totalMass > 0) {
                    node->centerOfMass.x += node->children[i]->centerOfMass.x * node->children[i]->totalMass;
                    node->centerOfMass.y += node->children[i]->centerOfMass.y * node->children[i]->totalMass;
                    node->totalMass += node->children[i]->totalMass;
                }
            }
            if (node->totalMass > 0) {
                node->centerOfMass.x /= node->totalMass;
                node->centerOfMass.y /= node->totalMass;
            }
        }
    } else {
        // Internal node - insert into appropriate child
        i32 q = node->getQuadrant(p.position);
        insertParticle(node->children[q].get(), particleIndex);

        // Update mass and center of mass
        node->centerOfMass.x = node->centerOfMass.x * node->totalMass + p.position.x * p.mass;
        node->centerOfMass.y = node->centerOfMass.y * node->totalMass + p.position.y * p.mass;
        node->totalMass += p.mass;
        node->centerOfMass.x /= node->totalMass;
        node->centerOfMass.y /= node->totalMass;
    }
}

Vec2 GalaxySimulator::computeForce(u32 particleIndex) {
    if (!m_root || m_root->totalMass <= 0) {
        return {0, 0};
    }

    const GalaxyParticle& p = m_particles[particleIndex];
    return computeForceFromNode(m_root.get(), p.position, p.mass);
}

Vec2 GalaxySimulator::computeForceFromNode(const QuadTreeNode* node, const Vec2& pos, Real mass) {
    if (!node || node->totalMass <= 0) {
        return {0, 0};
    }

    Real dx = node->centerOfMass.x - pos.x;
    Real dy = node->centerOfMass.y - pos.y;
    Real distSq = dx * dx + dy * dy;
    Real dist = std::sqrt(distSq);

    // If this is a leaf with a single particle, or far enough away, use approximation
    bool isLeaf = node->isLeaf() && node->particleIndex >= 0;
    bool isFarEnough = (node->halfSize * 2.0f / dist) < m_config.theta;

    if (isLeaf || isFarEnough) {
        // Avoid self-interaction
        if (distSq < 0.01f) {
            return {0, 0};
        }

        // Softened gravity: F = G * m1 * m2 / (r^2 + eps^2)
        Real softDistSq = distSq + m_config.softeningLength * m_config.softeningLength;
        Real forceMag = m_config.gravitationalConstant * node->totalMass / softDistSq;

        // Normalize direction
        Real invDist = 1.0f / std::sqrt(softDistSq);
        return {
            forceMag * dx * invDist,
            forceMag * dy * invDist
        };
    } else {
        // Node is too close - recurse into children
        Vec2 force = {0, 0};
        for (i32 i = 0; i < 4; ++i) {
            if (node->children[i]) {
                Vec2 childForce = computeForceFromNode(node->children[i].get(), pos, mass);
                force.x += childForce.x;
                force.y += childForce.y;
            }
        }
        return force;
    }
}

// ============================================================================
// Simulation step
// ============================================================================

void GalaxySimulator::step(Real dt) {
    dt *= m_config.timeScale;

    // Build Barnes-Hut tree
    buildTree();

    // Compute forces and update velocities/positions (Leapfrog)
    integrateLeapfrog(dt);

    m_stats.simulationTime += dt;
}

void GalaxySimulator::stepMultiple(Real dt, u32 steps) {
    for (u32 i = 0; i < steps; ++i) {
        step(dt);
    }
}

void GalaxySimulator::integrateLeapfrog(Real dt) {
    Real halfDt = dt * 0.5f;

    // First half velocity update + full position update
    for (auto& p : m_particles) {
        if (!p.active) continue;

        // v(t + dt/2) = v(t) + a(t) * dt/2
        p.velocity.x += p.acceleration.x * halfDt;
        p.velocity.y += p.acceleration.y * halfDt;

        // x(t + dt) = x(t) + v(t + dt/2) * dt
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;

        // Boundary handling (soft walls)
        Real margin = 50.0f;
        if (p.position.x < margin) p.velocity.x += 0.1f;
        if (p.position.x > m_config.size - margin) p.velocity.x -= 0.1f;
        if (p.position.y < margin) p.velocity.y += 0.1f;
        if (p.position.y > m_config.size - margin) p.velocity.y -= 0.1f;
    }

    // Rebuild tree with new positions
    buildTree();

    // Compute new accelerations
    for (u32 i = 0; i < m_particles.size(); ++i) {
        if (!m_particles[i].active) continue;

        Vec2 force = computeForce(i);
        m_particles[i].acceleration.x = force.x;
        m_particles[i].acceleration.y = force.y;
    }

    // Second half velocity update
    for (auto& p : m_particles) {
        if (!p.active) continue;

        // v(t + dt) = v(t + dt/2) + a(t + dt) * dt/2
        p.velocity.x += p.acceleration.x * halfDt;
        p.velocity.y += p.acceleration.y * halfDt;
    }
}

// ============================================================================
// Statistics
// ============================================================================

void GalaxySimulator::computeStatistics() {
    m_stats.activeParticles = 0;
    m_stats.totalMass = 0;
    m_stats.kineticEnergy = 0;
    m_stats.centerOfMass = {0, 0};
    m_stats.angularMomentum = 0;
    m_stats.maxVelocity = 0;

    for (const auto& p : m_particles) {
        if (!p.active) continue;

        m_stats.activeParticles++;
        m_stats.totalMass += p.mass;

        Real vSq = p.velocity.x * p.velocity.x + p.velocity.y * p.velocity.y;
        m_stats.kineticEnergy += 0.5f * p.mass * vSq;

        Real v = std::sqrt(vSq);
        if (v > m_stats.maxVelocity) {
            m_stats.maxVelocity = v;
        }

        m_stats.centerOfMass.x += p.position.x * p.mass;
        m_stats.centerOfMass.y += p.position.y * p.mass;

        // Angular momentum about center
        m_stats.angularMomentum += p.mass * (p.position.x * p.velocity.y - p.position.y * p.velocity.x);
    }

    if (m_stats.totalMass > 0) {
        m_stats.centerOfMass.x /= m_stats.totalMass;
        m_stats.centerOfMass.y /= m_stats.totalMass;
    }

    // Potential energy would require O(n^2) or using the tree
    m_stats.potentialEnergy = -m_stats.kineticEnergy; // Approximation for virial equilibrium
    m_stats.totalEnergy = m_stats.kineticEnergy + m_stats.potentialEnergy;
}

// ============================================================================
// Data access
// ============================================================================

const Real* GalaxySimulator::positionData() const {
    updatePositionBuffer();
    return m_positionBuffer.data();
}

const Real* GalaxySimulator::velocityData() const {
    updateVelocityBuffer();
    return m_velocityBuffer.data();
}

void GalaxySimulator::updatePositionBuffer() const {
    m_positionBuffer.clear();
    m_positionBuffer.reserve(m_particles.size() * 2);

    for (const auto& p : m_particles) {
        if (p.active) {
            m_positionBuffer.push_back(p.position.x);
            m_positionBuffer.push_back(p.position.y);
        }
    }
}

void GalaxySimulator::updateVelocityBuffer() const {
    m_velocityBuffer.clear();
    m_velocityBuffer.reserve(m_particles.size() * 2);

    for (const auto& p : m_particles) {
        if (p.active) {
            m_velocityBuffer.push_back(p.velocity.x);
            m_velocityBuffer.push_back(p.velocity.y);
        }
    }
}

// ============================================================================
// Helpers
// ============================================================================

Real GalaxySimulator::orbitalVelocity(Real radius, Real enclosedMass) const {
    if (radius < 0.001f) return 0;
    return std::sqrt(m_config.gravitationalConstant * enclosedMass / radius);
}

Real GalaxySimulator::diskDensity(Real r, Real scaleRadius) const {
    return std::exp(-r / scaleRadius);
}

// ============================================================================
// Presets
// ============================================================================

namespace galaxy_presets {

GalaxyConfig milkyWay() {
    GalaxyConfig c;
    c.size = 2000.0f;
    c.gravitationalConstant = 1.0f;
    c.softeningLength = 5.0f;
    c.theta = 0.5f;
    c.blackHoleMass = 2000.0f;
    c.centralBlackHole = true;
    c.minMass = 1.0f;
    c.maxMass = 5.0f;
    return c;
}

GalaxyConfig andromeda() {
    GalaxyConfig c;
    c.size = 2000.0f;
    c.gravitationalConstant = 1.2f;
    c.softeningLength = 4.0f;
    c.theta = 0.5f;
    c.blackHoleMass = 3000.0f;
    c.centralBlackHole = true;
    c.minMass = 1.0f;
    c.maxMass = 8.0f;
    return c;
}

GalaxyConfig dwarf() {
    GalaxyConfig c;
    c.size = 2000.0f;
    c.gravitationalConstant = 0.5f;
    c.softeningLength = 8.0f;
    c.theta = 0.6f;
    c.blackHoleMass = 200.0f;
    c.centralBlackHole = true;
    c.minMass = 0.5f;
    c.maxMass = 2.0f;
    return c;
}

GalaxyConfig collision() {
    GalaxyConfig c;
    c.size = 2000.0f;
    c.gravitationalConstant = 1.0f;
    c.softeningLength = 5.0f;
    c.theta = 0.5f;
    c.blackHoleMass = 1500.0f;
    c.centralBlackHole = true;
    c.minMass = 1.0f;
    c.maxMass = 4.0f;
    c.timeScale = 1.5f;
    return c;
}

GalaxyConfig cluster() {
    GalaxyConfig c;
    c.size = 2000.0f;
    c.gravitationalConstant = 0.3f;
    c.softeningLength = 10.0f;
    c.theta = 0.7f;
    c.blackHoleMass = 100.0f;
    c.centralBlackHole = false;
    c.minMass = 1.0f;
    c.maxMass = 3.0f;
    return c;
}

} // namespace galaxy_presets

} // namespace physics
} // namespace eigenlab
