#pragma once
#include <vector>
#include <cstdint>
#include <random>

namespace eigenlab {
namespace physics {

/**
 * Accretion Disk Simulation
 *
 * Simulates matter orbiting and accreting onto a central mass.
 * Models viscous disk dynamics with Keplerian rotation.
 *
 * Features:
 * - Keplerian orbital mechanics
 * - Viscous angular momentum transport
 * - Temperature distribution (T ~ r^(-3/4))
 * - Particle emission and accretion
 * - Relativistic effects near central mass
 */

struct AccretionConfig {
    int numParticles = 5000;
    float centralMass = 1000.0f;      // Central object mass
    float innerRadius = 5.0f;          // Inner edge (ISCO-like)
    float outerRadius = 100.0f;        // Outer edge
    float diskHeight = 0.1f;           // Thickness ratio
    float viscosity = 0.01f;           // Kinematic viscosity
    float accretionRate = 0.1f;        // Mass inflow rate
    float emissionRate = 50.0f;        // Particles per second at outer edge
    float G = 1.0f;                     // Gravitational constant
    float dt = 0.01f;
};

struct AccretionStats {
    int numParticles = 0;
    int particlesAccreted = 0;
    float totalAngularMomentum = 0.0f;
    float avgTemperature = 0.0f;
    float diskMass = 0.0f;
    float luminosity = 0.0f;           // From accretion energy
};

class AccretionDisk {
public:
    AccretionDisk();
    explicit AccretionDisk(const AccretionConfig& config);
    ~AccretionDisk() = default;

    // Configuration
    void setConfig(const AccretionConfig& config);
    void reset();

    // Simulation
    void step();
    void stepMultiple(int steps);

    // Control
    void setCentralMass(float mass);
    void setViscosity(float visc);
    void setEmissionRate(float rate);
    void addParticleBurst(float radius, int count);

    // Data access for JS
    const float* positionData() const { return positions_.data(); }
    const float* velocityData() const { return velocities_.data(); }
    const float* temperatureData() const { return temperatures_.data(); }
    int dataSize() const { return static_cast<int>(positions_.size()); }
    int particleCount() const { return numParticles_; }
    float innerRadius() const { return config_.innerRadius; }
    float outerRadius() const { return config_.outerRadius; }

    // Statistics
    void computeStatistics();
    const AccretionStats& stats() const { return stats_; }

private:
    struct Particle {
        float x, y, z;         // Position
        float vx, vy, vz;      // Velocity
        float r;               // Radial distance (cached)
        float temperature;     // Local temperature
        bool active;
    };

    AccretionConfig config_;
    std::vector<Particle> particles_;
    int numParticles_;
    int accretedCount_;

    // Data buffers for JS (x, y, z for each particle)
    std::vector<float> positions_;
    std::vector<float> velocities_;
    std::vector<float> temperatures_;

    // Statistics
    AccretionStats stats_;
    float emitAccum_;

    // RNG
    std::mt19937 rng_;

    // Helper functions
    float keplerianVelocity(float r) const;
    float diskTemperature(float r) const;
    void emitParticles();
    void updateParticle(Particle& p, float dt);
    void updateDataBuffers();
};

// Presets
AccretionConfig accretionPresetProtostar();
AccretionConfig accretionPresetBlackHole();
AccretionConfig accretionPresetCompact();
AccretionConfig accretionPresetThin();

} // namespace physics
} // namespace eigenlab
