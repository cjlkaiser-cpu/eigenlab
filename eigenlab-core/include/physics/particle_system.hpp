/**
 * @file particle_system.hpp
 * @brief High-performance particle system with spatial hashing for O(n) collision detection
 * @version 1.0.0
 *
 * Features:
 * - Spatial hash grid for efficient broad-phase collision detection
 * - Elastic and inelastic collisions between particles
 * - Wall collisions with configurable boundaries
 * - Maxwell-Boltzmann velocity distribution
 * - Thermodynamic property calculations (P, T, E)
 */

#pragma once

#include "../core/types.hpp"
#include <vector>
#include <unordered_map>
#include <functional>

namespace eigenlab {
namespace physics {

// ============================================================================
// Particle
// ============================================================================

struct Particle {
    Vec2 position{0.0f, 0.0f};
    Vec2 velocity{0.0f, 0.0f};
    Real radius{1.0f};
    Real mass{1.0f};
    Real temperature{300.0f};  // For visualization
    u32 collisionCount{0};
    bool active{true};

    [[nodiscard]] Real kineticEnergy() const {
        return 0.5f * mass * velocity.lengthSquared();
    }

    [[nodiscard]] Real speed() const {
        return velocity.length();
    }
};

// ============================================================================
// Spatial Hash Grid - O(1) neighbor lookup
// ============================================================================

class SpatialHash {
public:
    explicit SpatialHash(Real cellSize = 10.0f);

    void clear();
    void insert(u32 particleIndex, const Vec2& position);
    void rebuild(const std::vector<Particle>& particles);

    // Get all particle indices in cells near a position
    void queryNear(const Vec2& position, Real radius,
                   std::vector<u32>& outIndices) const;

    // Get all potential collision pairs (each pair returned once)
    void getPotentialPairs(std::vector<std::pair<u32, u32>>& outPairs) const;

    void setCellSize(Real size) { m_cellSize = size; m_invCellSize = 1.0f / size; }
    [[nodiscard]] Real cellSize() const { return m_cellSize; }

private:
    [[nodiscard]] i32 hashPosition(const Vec2& pos) const;
    [[nodiscard]] std::pair<i32, i32> getCellCoords(const Vec2& pos) const;

    Real m_cellSize;
    Real m_invCellSize;
    std::unordered_map<i32, std::vector<u32>> m_cells;
};

// ============================================================================
// Wall collision callback
// ============================================================================

struct WallCollision {
    Vec2 position;      // Collision point
    Vec2 normal;        // Wall normal
    Real momentum;      // Momentum transfer
    u32 particleIndex;
};

using WallCollisionCallback = std::function<void(const WallCollision&)>;

// ============================================================================
// Particle System Configuration
// ============================================================================

struct ParticleSystemConfig {
    // Domain
    AABB bounds{0.0f, 0.0f, 800.0f, 600.0f};

    // Physics
    Real particleRadius{3.0f};
    Real particleMass{1.0f};
    Real restitution{1.0f};        // 1.0 = perfectly elastic
    Real wallRestitution{1.0f};

    // Temperature (for Maxwell-Boltzmann distribution)
    Real temperature{300.0f};      // Kelvin
    Real molecularMass{4.65e-26f}; // kg (N2 molecule default)

    // Spatial hash
    Real cellSizeMultiplier{2.5f}; // cellSize = radius * multiplier

    // Limits
    u32 maxParticles{10000};
    Real maxSpeed{1000.0f};
};

// ============================================================================
// Statistics
// ============================================================================

struct ParticleSystemStats {
    // Thermodynamics
    Real totalKineticEnergy{0.0f};
    Real averageKineticEnergy{0.0f};
    Real temperature{0.0f};        // From KE: T = 2E/(3Nk)
    Real pressure{0.0f};           // From wall collisions

    // Velocities
    Real averageSpeed{0.0f};
    Real rmsSpeed{0.0f};           // √(⟨v²⟩)
    Real maxSpeed{0.0f};

    // Collisions
    u32 particleCollisions{0};
    u32 wallCollisions{0};
    Real momentumTransferToWalls{0.0f};

    // Histogram data (for Maxwell-Boltzmann)
    std::vector<u32> velocityHistogram;
    Real histogramBinWidth{0.0f};
};

// ============================================================================
// Particle System
// ============================================================================

class ParticleSystem {
public:
    ParticleSystem();
    explicit ParticleSystem(const ParticleSystemConfig& config);

    // Configuration
    void setConfig(const ParticleSystemConfig& config);
    [[nodiscard]] const ParticleSystemConfig& config() const { return m_config; }

    // Particle management
    void clear();
    u32 addParticle(const Vec2& position, const Vec2& velocity);
    u32 addParticle(const Particle& particle);
    void removeParticle(u32 index);

    // Bulk initialization
    void initializeRandom(u32 count);
    void initializeGrid(u32 rows, u32 cols);
    void initializeMaxwellBoltzmann(u32 count, Real temperature);

    // Simulation
    void step(Real dt);
    void stepSubdivided(Real dt, u32 substeps);

    // Temperature control
    void setTemperature(Real T);
    void scaleVelocitiesToTemperature(Real targetT);
    void thermalizeParticle(u32 index, Real T);

    // Domain
    void setBounds(const AABB& bounds);
    void setVolume(Real widthPercent);  // 0-100, maintains center

    // Access
    [[nodiscard]] const std::vector<Particle>& particles() const { return m_particles; }
    [[nodiscard]] std::vector<Particle>& particles() { return m_particles; }
    [[nodiscard]] u32 particleCount() const { return static_cast<u32>(m_particles.size()); }
    [[nodiscard]] const Particle& particle(u32 index) const { return m_particles[index]; }

    // Statistics
    void computeStatistics();
    [[nodiscard]] const ParticleSystemStats& stats() const { return m_stats; }

    // Velocity histogram for Maxwell-Boltzmann visualization
    void computeVelocityHistogram(u32 numBins, Real maxVelocity);

    // Callbacks
    void setWallCollisionCallback(WallCollisionCallback callback);

    // Raw data access for JavaScript (pointer + size)
    [[nodiscard]] const Real* positionData() const;
    [[nodiscard]] const Real* velocityData() const;
    [[nodiscard]] usize dataStride() const { return sizeof(Particle) / sizeof(Real); }

private:
    void resolveParticleCollisions();
    void resolveWallCollisions();
    void integratePositions(Real dt);
    void updateSpatialHash();
    bool collideParticles(u32 i, u32 j);

    ParticleSystemConfig m_config;
    std::vector<Particle> m_particles;
    SpatialHash m_spatialHash;
    ParticleSystemStats m_stats;
    WallCollisionCallback m_wallCallback;

    // Working buffers to avoid allocations
    std::vector<std::pair<u32, u32>> m_collisionPairs;
    std::vector<u32> m_nearbyIndices;
};

// ============================================================================
// Maxwell-Boltzmann utilities
// ============================================================================

namespace maxwell_boltzmann {
    // Generate speed from Maxwell-Boltzmann distribution
    Real sampleSpeed(Real temperature, Real mass);

    // PDF of speed
    Real pdf(Real v, Real temperature, Real mass);

    // Most probable speed
    Real mostProbableSpeed(Real temperature, Real mass);

    // Mean speed
    Real meanSpeed(Real temperature, Real mass);

    // RMS speed
    Real rmsSpeed(Real temperature, Real mass);
}

} // namespace physics
} // namespace eigenlab
