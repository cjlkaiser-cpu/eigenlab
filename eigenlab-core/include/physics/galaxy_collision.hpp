/**
 * @file galaxy_collision.hpp
 * @brief Specialized simulation for galaxy collision and merger events
 *
 * Extends GalaxySimulator with collision-specific features:
 * - Individual tracking of each galaxy
 * - Tidal tail detection
 * - Collision metrics (separation, binding energy, etc.)
 * - Famous collision presets (Antennae, Mice, Milkdromeda)
 */

#pragma once

#include "../core/types.hpp"
#include "galaxy.hpp"
#include <vector>
#include <array>

namespace eigenlab {
namespace physics {

// ============================================================================
// Configuration
// ============================================================================

struct CollisionConfig {
    // Simulation bounds
    Real size{3000.0f};              // Larger area for collision dynamics

    // Physics
    Real gravitationalConstant{1.0f};
    Real softeningLength{5.0f};
    Real timeScale{1.0f};

    // Barnes-Hut
    Real theta{0.5f};

    // Galaxy 1 parameters
    u32 galaxy1Stars{10000};
    Real galaxy1Mass{2000.0f};       // Central black hole mass
    Real galaxy1Radius{300.0f};
    Real galaxy1Rotation{1.0f};      // 1.0 = CCW, -1.0 = CW
    Real galaxy1Angle{0.0f};         // Disk inclination angle

    // Galaxy 2 parameters
    u32 galaxy2Stars{10000};
    Real galaxy2Mass{1500.0f};
    Real galaxy2Radius{250.0f};
    Real galaxy2Rotation{-1.0f};     // Opposite rotation for interesting dynamics
    Real galaxy2Angle{30.0f};        // Different inclination

    // Collision parameters
    Real separation{800.0f};         // Initial separation
    Real impactParameter{100.0f};    // Perpendicular offset (b parameter)
    Real approachVelocity{0.8f};     // Initial relative velocity

    // Visualization
    bool showTrajectories{true};
    bool showTidalTails{true};
    u32 trajectoryLength{500};       // Points to keep in trajectory history
};

// ============================================================================
// Galaxy tracking data
// ============================================================================

struct GalaxyTracker {
    Vec2 centerOfMass{0, 0};
    Vec2 velocity{0, 0};
    Real totalMass{0};
    Real boundMass{0};               // Mass still bound to this galaxy
    Real tidalRadius{0};             // Current tidal/Jacobi radius
    u32 particleCount{0};
    u32 startIndex{0};               // First particle index
    u32 endIndex{0};                 // Last particle index + 1

    // Trajectory history
    std::vector<Vec2> trajectory;

    // Black hole tracking
    u32 blackHoleIndex{0};
    Vec2 blackHolePos{0, 0};
    Vec2 blackHoleVel{0, 0};
};

struct CollisionMetrics {
    // Separation
    Real separation{0};              // Distance between galaxy centers
    Real closestApproach{1e10f};     // Minimum separation achieved
    Real separationVelocity{0};      // Rate of change of separation

    // Energetics
    Real totalEnergy{0};
    Real kineticEnergy{0};
    Real potentialEnergy{0};
    Real bindingEnergy{0};           // Energy to unbind the pair

    // Tidal effects
    Real tidalStrength{0};           // F_tidal / F_self
    Real massTransfer{0};            // Mass exchanged between galaxies

    // Phase
    bool isApproaching{true};
    bool hasMerged{false};           // Single system formed
    u32 passageCount{0};             // Number of close passages

    // Timing
    Real simulationTime{0};
    Real timeToMerger{-1};           // Estimated time to merge
};

// ============================================================================
// Collision presets
// ============================================================================

enum class CollisionPreset {
    Milkdromeda,     // Milky Way + Andromeda (our future!)
    Antennae,        // NGC 4038/4039 - classic collision
    Mice,            // NGC 4676 - long tidal tails
    Whirlpool,       // M51 - minor merger
    Cartwheel,       // Head-on collision
    Custom
};

// ============================================================================
// GalaxyCollision Simulator
// ============================================================================

class GalaxyCollision {
public:
    GalaxyCollision();
    explicit GalaxyCollision(const CollisionConfig& config);

    // Configuration
    void setConfig(const CollisionConfig& config);
    const CollisionConfig& config() const { return m_config; }

    // Initialization
    void initialize();
    void initializePreset(CollisionPreset preset);
    void reset();

    // Simulation
    void step(Real dt);
    void stepMultiple(Real dt, u32 steps);

    // Metrics
    void updateMetrics();
    const CollisionMetrics& metrics() const { return m_metrics; }
    const GalaxyTracker& galaxy1() const { return m_galaxy1; }
    const GalaxyTracker& galaxy2() const { return m_galaxy2; }

    // Accessors
    u32 particleCount() const;
    u32 activeParticleCount() const;

    // Data for JavaScript
    const Real* positionData() const;
    const Real* velocityData() const;
    const Real* temperatureData() const;
    u32 positionDataSize() const;

    // Trajectory data
    const Real* trajectory1Data() const;
    const Real* trajectory2Data() const;
    u32 trajectorySize() const;

    // Parameter setters (runtime adjustable)
    void setGravitationalConstant(Real G);
    void setSofteningLength(Real eps);
    void setTheta(Real theta);
    void setTimeScale(Real scale);

    // Galaxy-specific setters
    void setGalaxy1Mass(Real mass);
    void setGalaxy2Mass(Real mass);
    void setApproachVelocity(Real vel);
    void setImpactParameter(Real b);

    // Preset configurations (public for namespace functions)
    CollisionConfig presetMilkdromeda() const;
    CollisionConfig presetAntennae() const;
    CollisionConfig presetMice() const;
    CollisionConfig presetWhirlpool() const;
    CollisionConfig presetCartwheel() const;

private:
    CollisionConfig m_config;
    CollisionMetrics m_metrics;

    // Internal galaxy simulator
    GalaxySimulator m_simulator;

    // Galaxy trackers
    GalaxyTracker m_galaxy1;
    GalaxyTracker m_galaxy2;

    // Cached data buffers
    mutable std::vector<Real> m_positionBuffer;
    mutable std::vector<Real> m_velocityBuffer;
    mutable std::vector<Real> m_temperatureBuffer;
    mutable std::vector<Real> m_trajectory1Buffer;
    mutable std::vector<Real> m_trajectory2Buffer;

    // Internal methods
    void setupGalaxy1();
    void setupGalaxy2();
    void updateGalaxyTrackers();
    void updateTrajectories();
    void detectMerger();
    void computeTidalStrength();
    void updateDataBuffers() const;
};

// ============================================================================
// Preset factory functions
// ============================================================================

namespace collision_presets {
    CollisionConfig milkdromeda();
    CollisionConfig antennae();
    CollisionConfig mice();
    CollisionConfig whirlpool();
    CollisionConfig cartwheel();
}

} // namespace physics
} // namespace eigenlab
