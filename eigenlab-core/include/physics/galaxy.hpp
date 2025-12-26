/**
 * @file galaxy.hpp
 * @brief N-body gravitational simulation for galaxy formation
 *
 * Implements Barnes-Hut algorithm for O(n log n) complexity
 * Supports galaxy collisions, dark matter halos, and black holes
 */

#pragma once

#include "../core/types.hpp"
#include <vector>
#include <memory>
#include <random>
#include <cmath>
#include <array>

namespace eigenlab {
namespace physics {

// ============================================================================
// Configuration
// ============================================================================

struct GalaxyConfig {
    // Simulation bounds
    Real size{2000.0f};           // Square simulation area

    // Physics
    Real gravitationalConstant{1.0f};
    Real softeningLength{5.0f};   // Prevents singularities at close range
    Real timeScale{1.0f};

    // Barnes-Hut
    Real theta{0.5f};             // Opening angle (0.5-1.0, lower = more accurate)

    // Particle properties
    u32 maxParticles{50000};
    Real minMass{1.0f};
    Real maxMass{10.0f};

    // Black hole
    Real blackHoleMass{1000.0f};
    Real blackHoleRadius{30.0f};
    bool centralBlackHole{true};

    // Visual
    Real velocityScale{1.0f};
};

// ============================================================================
// Particle types
// ============================================================================

enum class ParticleType : u8 {
    Star,
    DarkMatter,
    Gas,
    BlackHole
};

struct GalaxyParticle {
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    Real mass{1.0f};
    ParticleType type{ParticleType::Star};
    Real temperature{5000.0f};    // For color (Kelvin)
    bool active{true};
};

struct GalaxyStats {
    u32 activeParticles{0};
    Real totalMass{0};
    Real totalEnergy{0};          // Kinetic + Potential
    Real kineticEnergy{0};
    Real potentialEnergy{0};
    Vec2 centerOfMass{0, 0};
    Real angularMomentum{0};
    Real maxVelocity{0};
    u32 treeNodes{0};
    Real simulationTime{0};
};

// ============================================================================
// Barnes-Hut Quadtree
// ============================================================================

struct QuadTreeNode {
    // Bounds
    Vec2 center;
    Real halfSize;

    // Mass distribution
    Vec2 centerOfMass{0, 0};
    Real totalMass{0};

    // Children: NW, NE, SW, SE
    std::array<std::unique_ptr<QuadTreeNode>, 4> children;

    // Leaf data
    i32 particleIndex{-1};  // -1 = internal node, >=0 = leaf with particle

    QuadTreeNode(Vec2 c, Real hs) : center(c), halfSize(hs) {}

    bool isLeaf() const {
        return children[0] == nullptr;
    }

    i32 getQuadrant(const Vec2& pos) const {
        i32 quadrant = 0;
        if (pos.x >= center.x) quadrant |= 1;
        if (pos.y >= center.y) quadrant |= 2;
        return quadrant;
    }

    Vec2 getChildCenter(i32 quadrant) const {
        Real quarter = halfSize * 0.5f;
        return {
            center.x + ((quadrant & 1) ? quarter : -quarter),
            center.y + ((quadrant & 2) ? quarter : -quarter)
        };
    }
};

// ============================================================================
// Galaxy Simulator
// ============================================================================

class GalaxySimulator {
public:
    GalaxySimulator();
    explicit GalaxySimulator(const GalaxyConfig& config);

    // Configuration
    void setConfig(const GalaxyConfig& config);
    const GalaxyConfig& config() const { return m_config; }

    // Initialization
    void clear();
    void initializeDiskGalaxy(u32 count, Vec2 center, Real radius, Real rotation = 1.0f);
    void initializeEllipticalGalaxy(u32 count, Vec2 center, Real radius);
    void initializeCollision(u32 countEach, Real separation, Real impactParam);
    void initializeRandom(u32 count);

    // Particle management
    u32 addParticle(Vec2 pos, Vec2 vel, Real mass, ParticleType type = ParticleType::Star);
    u32 addBlackHole(Vec2 pos, Vec2 vel, Real mass);
    void removeParticle(u32 index);

    // Simulation
    void step(Real dt);
    void stepMultiple(Real dt, u32 steps);

    // Statistics
    void computeStatistics();
    const GalaxyStats& stats() const { return m_stats; }

    // Accessors
    u32 particleCount() const { return static_cast<u32>(m_particles.size()); }
    const std::vector<GalaxyParticle>& particles() const { return m_particles; }

    // Data pointers for JavaScript
    const Real* positionData() const;
    const Real* velocityData() const;

    // Parameter setters
    void setGravitationalConstant(Real G) { m_config.gravitationalConstant = G; }
    void setSofteningLength(Real eps) { m_config.softeningLength = eps; }
    void setTheta(Real theta) { m_config.theta = theta; }
    void setTimeScale(Real scale) { m_config.timeScale = scale; }

private:
    GalaxyConfig m_config;
    GalaxyStats m_stats;

    std::vector<GalaxyParticle> m_particles;
    std::unique_ptr<QuadTreeNode> m_root;

    // Packed data for JS
    mutable std::vector<Real> m_positionBuffer;
    mutable std::vector<Real> m_velocityBuffer;

    // Random
    std::mt19937 m_rng;
    std::uniform_real_distribution<Real> m_uniformDist{0.0f, 1.0f};
    std::normal_distribution<Real> m_normalDist{0.0f, 1.0f};

    // Tree methods
    void buildTree();
    void insertParticle(QuadTreeNode* node, u32 particleIndex);
    Vec2 computeForce(u32 particleIndex);
    Vec2 computeForceFromNode(const QuadTreeNode* node, const Vec2& pos, Real mass);

    // Integration
    void integrateLeapfrog(Real dt);
    void integrateVerlet(Real dt);

    void updatePositionBuffer() const;
    void updateVelocityBuffer() const;

    // Helpers
    Real orbitalVelocity(Real radius, Real enclosedMass) const;
    Real diskDensity(Real r, Real scaleRadius) const;
};

// ============================================================================
// Presets
// ============================================================================

namespace galaxy_presets {
    GalaxyConfig milkyWay();
    GalaxyConfig andromeda();
    GalaxyConfig dwarf();
    GalaxyConfig collision();
    GalaxyConfig cluster();
}

} // namespace physics
} // namespace eigenlab
