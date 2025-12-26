/**
 * @file boids.hpp
 * @brief High-performance Boids flocking simulation
 *
 * Implements Craig Reynolds' Boids algorithm (1987) with:
 * - Spatial hash grid for O(n) neighbor lookup
 * - SIMD-friendly data layout
 * - Support for predators, obstacles, and attractors
 * - Multiple species with different behaviors
 */

#pragma once

#include "../core/types.hpp"
#include <vector>
#include <array>
#include <random>
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace eigenlab {
namespace physics {

// ============================================================================
// Configuration
// ============================================================================

struct BoidConfig {
    // Simulation bounds
    AABB bounds{{0, 0}, {800, 600}};

    // Boid properties
    Real maxSpeed{4.0f};
    Real minSpeed{2.0f};
    Real maxForce{0.15f};          // Steering force limit

    // Perception
    Real perceptionRadius{50.0f};  // How far boids can see
    Real separationRadius{25.0f};  // Personal space
    Real fieldOfView{270.0f};      // Degrees (360 = see all around)

    // Behavior weights
    Real separationWeight{1.5f};   // Avoid crowding
    Real alignmentWeight{1.0f};    // Match velocity with neighbors
    Real cohesionWeight{1.0f};     // Move toward center of flock

    // Additional behaviors
    Real avoidWallsWeight{2.0f};   // Wall avoidance strength
    Real wallMargin{50.0f};        // Start avoiding at this distance
    Real noiseWeight{0.1f};        // Random wandering

    // Performance
    u32 maxBoids{10000};
    Real spatialGridCellSize{50.0f};  // Should match perceptionRadius

    // Wrap around edges instead of bouncing
    bool wrapEdges{false};
};

struct PredatorConfig {
    Real maxSpeed{6.0f};
    Real maxForce{0.2f};
    Real perceptionRadius{100.0f};
    Real huntRadius{200.0f};       // Start chasing prey
    Real killRadius{15.0f};        // Close enough to catch
    Real separationWeight{2.0f};
    Real chaseWeight{1.5f};
};

// ============================================================================
// Data structures
// ============================================================================

struct Boid {
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    u32 species{0};         // For multi-species flocking
    Real hue{0.0f};         // Color (0-360)
    bool active{true};

    // Cached neighbor info (updated each frame)
    u32 neighborCount{0};
    Vec2 flockCenter{0, 0};
    Vec2 flockVelocity{0, 0};
};

struct Predator {
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    u32 kills{0};
    bool active{true};
};

struct Obstacle {
    Vec2 position;
    Real radius;
    bool active{true};
};

struct Attractor {
    Vec2 position;
    Real strength;       // Positive = attract, negative = repel
    Real radius;         // Influence radius
    bool active{true};
};

struct BoidStats {
    u32 activeBoids{0};
    u32 activePredators{0};
    Real averageSpeed{0};
    Real averageNeighbors{0};
    Vec2 flockCenter{0, 0};
    Real flockSpread{0};         // Standard deviation from center
    Real polarization{0};         // How aligned the flock is (0-1)
    u32 kills{0};
    Real simulationTime{0};
};

// Spatial hash cell
struct SpatialCell {
    std::vector<u32> boidIndices;

    void clear() { boidIndices.clear(); }
    void add(u32 idx) { boidIndices.push_back(idx); }
};

// ============================================================================
// Boid System
// ============================================================================

class BoidSystem {
public:
    BoidSystem();
    explicit BoidSystem(const BoidConfig& config);

    // Configuration
    void setConfig(const BoidConfig& config);
    const BoidConfig& config() const { return m_config; }

    // Initialization
    void clear();
    void initializeRandom(u32 count);
    void initializeCircle(u32 count, Vec2 center, Real radius);
    void initializeGrid(u32 count);

    // Add entities
    u32 addBoid(Vec2 pos, Vec2 vel, u32 species = 0);
    u32 addPredator(Vec2 pos, Vec2 vel);
    u32 addObstacle(Vec2 pos, Real radius);
    u32 addAttractor(Vec2 pos, Real strength, Real radius);

    // Remove entities
    void removeBoid(u32 index);
    void removePredator(u32 index);
    void removeObstacle(u32 index);
    void removeAttractor(u32 index);
    void clearPredators();
    void clearObstacles();
    void clearAttractors();

    // Simulation
    void step(Real dt);
    void stepMultiple(Real dt, u32 steps);

    // Statistics
    void computeStatistics();
    const BoidStats& stats() const { return m_stats; }

    // Accessors
    u32 boidCount() const { return static_cast<u32>(m_boids.size()); }
    u32 predatorCount() const { return static_cast<u32>(m_predators.size()); }
    u32 obstacleCount() const { return static_cast<u32>(m_obstacles.size()); }
    u32 attractorCount() const { return static_cast<u32>(m_attractors.size()); }

    const std::vector<Boid>& boids() const { return m_boids; }
    const std::vector<Predator>& predators() const { return m_predators; }
    const std::vector<Obstacle>& obstacles() const { return m_obstacles; }
    const std::vector<Attractor>& attractors() const { return m_attractors; }

    // Data pointers for JavaScript
    const Real* positionData() const {
        return reinterpret_cast<const Real*>(m_positions.data());
    }
    const Real* velocityData() const {
        return reinterpret_cast<const Real*>(m_velocities.data());
    }

    // Parameter setters (for real-time control)
    void setSeparationWeight(Real w) { m_config.separationWeight = w; }
    void setAlignmentWeight(Real w) { m_config.alignmentWeight = w; }
    void setCohesionWeight(Real w) { m_config.cohesionWeight = w; }
    void setPerceptionRadius(Real r) { m_config.perceptionRadius = r; }
    void setMaxSpeed(Real s) { m_config.maxSpeed = s; }
    void setMaxForce(Real f) { m_config.maxForce = f; }

private:
    BoidConfig m_config;
    PredatorConfig m_predatorConfig;
    BoidStats m_stats;

    std::vector<Boid> m_boids;
    std::vector<Predator> m_predators;
    std::vector<Obstacle> m_obstacles;
    std::vector<Attractor> m_attractors;

    // Packed arrays for efficient data transfer
    std::vector<Vec2> m_positions;
    std::vector<Vec2> m_velocities;

    // Spatial hash grid
    std::unordered_map<i64, SpatialCell> m_spatialGrid;
    u32 m_gridWidth{0};
    u32 m_gridHeight{0};

    // Random number generation
    std::mt19937 m_rng;
    std::uniform_real_distribution<Real> m_uniformDist{0.0f, 1.0f};
    std::normal_distribution<Real> m_normalDist{0.0f, 1.0f};

    // Internal methods
    void buildSpatialGrid();
    i64 cellKey(i32 cx, i32 cy) const;
    void getCellCoords(const Vec2& pos, i32& cx, i32& cy) const;
    std::vector<u32> getNeighbors(u32 boidIndex) const;

    void updateBoid(u32 index, Real dt);
    void updatePredator(u32 index, Real dt);

    Vec2 computeSeparation(u32 index, const std::vector<u32>& neighbors);
    Vec2 computeAlignment(u32 index, const std::vector<u32>& neighbors);
    Vec2 computeCohesion(u32 index, const std::vector<u32>& neighbors);
    Vec2 computeWallAvoidance(const Boid& boid);
    Vec2 computeObstacleAvoidance(const Boid& boid);
    Vec2 computeAttractorForce(const Boid& boid);
    Vec2 computePredatorAvoidance(const Boid& boid);
    Vec2 computeNoise();

    Vec2 limit(Vec2 v, Real maxMag) const;
    Vec2 setMagnitude(Vec2 v, Real mag) const;
    bool isInFieldOfView(const Boid& boid, const Vec2& target) const;
    void wrapPosition(Vec2& pos);
    void constrainPosition(Vec2& pos, Vec2& vel);

    void updatePackedArrays();
};

// ============================================================================
// Presets
// ============================================================================

namespace boid_presets {
    BoidConfig birds();       // Classic bird flocking
    BoidConfig fish();        // Dense schooling fish
    BoidConfig insects();     // Fast, erratic movement
    BoidConfig slow();        // Slow, smooth movement (jellyfish-like)
    BoidConfig chaos();       // High noise, low cohesion
}

} // namespace physics
} // namespace eigenlab
