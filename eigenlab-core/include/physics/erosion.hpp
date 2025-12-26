/**
 * @file erosion.hpp
 * @brief Hydraulic erosion simulation for terrain generation
 *
 * Implements particle-based hydraulic erosion following:
 * - Water droplet simulation
 * - Sediment transport (erosion/deposition)
 * - Thermal erosion (talus angle)
 */

#pragma once

#include "../core/types.hpp"
#include <vector>
#include <random>
#include <cmath>

namespace eigenlab {
namespace physics {

// ============================================================================
// Configuration
// ============================================================================

struct ErosionConfig {
    // Terrain dimensions
    u32 width{256};
    u32 height{256};
    Real cellSize{1.0f};

    // Droplet parameters
    Real inertia{0.05f};           // How much droplet keeps its direction (0-1)
    Real sedimentCapacity{4.0f};   // Max sediment a droplet can carry
    Real minSedimentCapacity{0.01f};
    Real depositSpeed{0.3f};       // Fraction deposited per step
    Real erodeSpeed{0.3f};         // Fraction eroded per step

    // Physics
    Real gravity{4.0f};
    Real evaporateSpeed{0.01f};    // Water evaporation rate
    Real minSlope{0.01f};          // Minimum slope for erosion

    // Droplet lifetime
    u32 maxLifetime{100};
    Real initialWater{1.0f};
    Real initialVelocity{1.0f};

    // Erosion radius
    u32 erosionRadius{3};

    // Thermal erosion (optional)
    Real talusAngle{0.5f};         // Max slope before sliding (tan of angle)
    Real thermalRate{0.1f};        // How fast material slides

    // Simulation
    u32 maxDroplets{100000};
};

// ============================================================================
// Data structures
// ============================================================================

struct WaterDroplet {
    Vec2 position;
    Vec2 direction;
    Real velocity{1.0f};
    Real water{1.0f};
    Real sediment{0.0f};
    u32 lifetime{0};
    bool active{true};
};

struct ErosionStats {
    u32 activeDroplets{0};
    u32 totalDroplets{0};
    Real totalErosion{0};
    Real totalDeposition{0};
    Real minHeight{0};
    Real maxHeight{1};
    Real averageHeight{0.5f};
    u32 iterations{0};
};

// ============================================================================
// Erosion Simulator
// ============================================================================

class ErosionSimulator {
public:
    ErosionSimulator();
    explicit ErosionSimulator(const ErosionConfig& config);

    // Configuration
    void setConfig(const ErosionConfig& config);
    const ErosionConfig& config() const { return m_config; }

    // Terrain initialization
    void initializeFlat(Real height = 0.5f);
    void initializeNoise(Real scale = 1.0f, u32 octaves = 4);
    void initializeMountain(Vec2 center, Real radius, Real height);
    void initializeRidges(u32 count, Real height);
    void initializeFromData(const Real* data, u32 width, u32 height);

    // Manual terrain modification
    void raise(Real x, Real y, Real radius, Real amount);
    void lower(Real x, Real y, Real radius, Real amount);
    void flatten(Real x, Real y, Real radius, Real targetHeight);
    void setHeight(u32 x, u32 y, Real height);

    // Erosion simulation
    void simulateDroplet();
    void simulateDroplets(u32 count);
    void simulateThermalErosion();
    void step();  // One full iteration (droplets + thermal)

    // Statistics
    void computeStatistics();
    const ErosionStats& stats() const { return m_stats; }

    // Accessors
    u32 width() const { return m_config.width; }
    u32 height() const { return m_config.height; }
    u32 cellCount() const { return m_config.width * m_config.height; }

    Real heightAt(u32 x, u32 y) const;
    Real heightAtInterp(Real x, Real y) const;
    Vec2 gradientAt(Real x, Real y) const;
    Vec2 normalAt(u32 x, u32 y) const;

    // Data access for JavaScript
    const Real* heightData() const { return m_heightMap.data(); }
    const Real* waterData() const { return m_waterMap.data(); }
    const Real* sedimentData() const { return m_sedimentMap.data(); }

    // Rendering
    void renderHeightMap(u32* buffer) const;
    void renderNormalMap(u32* buffer) const;
    void renderWaterMap(u32* buffer) const;
    void renderSlopeMap(u32* buffer) const;

    // Parameter setters
    void setInertia(Real i) { m_config.inertia = i; }
    void setErosionSpeed(Real e) { m_config.erodeSpeed = e; }
    void setDepositSpeed(Real d) { m_config.depositSpeed = d; }
    void setEvaporateSpeed(Real e) { m_config.evaporateSpeed = e; }
    void setCapacity(Real c) { m_config.sedimentCapacity = c; }
    void setErosionRadius(u32 r) { m_config.erosionRadius = r; }

private:
    ErosionConfig m_config;
    ErosionStats m_stats;

    std::vector<Real> m_heightMap;
    std::vector<Real> m_waterMap;      // Water accumulation visualization
    std::vector<Real> m_sedimentMap;   // Sediment for visualization

    // Random number generation
    std::mt19937 m_rng;
    std::uniform_real_distribution<Real> m_uniformDist{0.0f, 1.0f};

    // Internal methods
    u32 idx(u32 x, u32 y) const { return y * m_config.width + x; }
    bool inBounds(Real x, Real y) const;
    Real sampleHeight(Real x, Real y) const;
    void erodeAt(Real x, Real y, Real amount);
    void depositAt(Real x, Real y, Real amount);

    // Noise generation
    Real noise2D(Real x, Real y) const;
    Real fbm(Real x, Real y, u32 octaves) const;
};

// ============================================================================
// Presets
// ============================================================================

namespace erosion_presets {
    ErosionConfig gentle();      // Gentle rain erosion
    ErosionConfig river();       // River-like carving
    ErosionConfig canyon();      // Deep canyon formation
    ErosionConfig coastal();     // Coastal erosion
    ErosionConfig volcanic();    // Volcanic terrain
}

} // namespace physics
} // namespace eigenlab
