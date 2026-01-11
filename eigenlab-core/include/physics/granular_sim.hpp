#pragma once
#include <vector>
#include <cstdint>
#include <random>

namespace eigenlab {
namespace physics {

/**
 * Granular Simulation - Sand/Rock Avalanche
 *
 * Position-Based Dynamics with friction model.
 * Simulates granular materials like sand, gravel, rocks.
 *
 * Features:
 * - Static and dynamic friction
 * - Particle-particle collisions
 * - Particle-terrain collisions
 * - Angle of repose emergence
 */

struct GranularConfig {
    int numParticles = 3000;
    float boxWidth = 100.0f;
    float boxHeight = 80.0f;
    float particleRadius = 0.4f;
    float gravity = 20.0f;
    float staticFriction = 0.6f;    // tan(angle of repose) ~ 0.6 for sand
    float dynamicFriction = 0.4f;
    float restitution = 0.2f;       // Low bounce for granular
    float damping = 0.98f;
    float dt = 0.016f;
};

struct GranularStats {
    int numParticles = 0;
    float kineticEnergy = 0.0f;
    float avgSpeed = 0.0f;
    int activeParticles = 0;        // Particles still moving
    float pileHeight = 0.0f;
};

class GranularSimulator {
public:
    GranularSimulator();
    explicit GranularSimulator(const GranularConfig& config);
    ~GranularSimulator() = default;

    // Configuration
    void setConfig(const GranularConfig& config);
    void reset();
    void clear();

    // Simulation
    void step();
    void stepMultiple(int steps);

    // Terrain
    void setTerrainHeight(int x, float height);
    void clearTerrain();
    void addRamp(float x1, float y1, float x2, float y2);
    void addFunnel(float centerX, float topY, float width, float gapWidth);
    void addPile(float centerX, float baseY, float width, float height);

    // Spawning
    void spawnParticles(float x, float y, int count, float spread);
    void spawnStream(float x, float y, float rate);  // Continuous spawn

    // Parameters
    void setGravity(float g);
    void setFriction(float staticF, float dynamicF);
    void setRestitution(float r);

    // Data access for JS
    const float* positionData() const { return positions_.data(); }
    const float* velocityData() const { return velocities_.data(); }
    const float* terrainData() const { return terrain_.data(); }
    int positionDataSize() const { return static_cast<int>(positions_.size()); }
    int particleCount() const { return numParticles_; }
    int terrainWidth() const { return terrainWidth_; }
    float boxWidth() const { return config_.boxWidth; }
    float boxHeight() const { return config_.boxHeight; }

    // Statistics
    void computeStatistics();
    const GranularStats& stats() const { return stats_; }

private:
    struct Particle {
        float x, y;
        float vx, vy;
        float radius;
        bool active;
    };

    GranularConfig config_;
    std::vector<Particle> particles_;
    int numParticles_;

    // Terrain heightmap
    std::vector<float> terrain_;
    int terrainWidth_;

    // Data buffers for JS
    std::vector<float> positions_;
    std::vector<float> velocities_;

    // Spatial hash for collision detection
    std::vector<std::vector<int>> grid_;
    int gridWidth_, gridHeight_;
    float cellSize_;

    // Statistics
    GranularStats stats_;

    // RNG for spawning
    std::mt19937 rng_;

    // Spawn stream state
    float spawnX_, spawnY_, spawnRate_;
    float spawnAccum_;
    bool spawning_;

    // Helper functions
    void buildSpatialHash();
    void resolveCollisions();
    void resolveTerrainCollision(Particle& p);
    void resolveParticleCollision(Particle& p1, Particle& p2);
    void applyFriction(Particle& p, float nx, float ny, float normalForce);
    float getTerrainHeight(float x) const;
    void updateDataBuffers();
};

// Presets
GranularConfig granularPresetSand();
GranularConfig granularPresetGravel();
GranularConfig granularPresetRocks();
GranularConfig granularPresetAvalanche();

} // namespace physics
} // namespace eigenlab
