#pragma once
#include "../core/types.hpp"
#include <vector>
#include <cstdint>

namespace eigenlab {
namespace physics {

/**
 * Lattice Boltzmann Method (LBM) for 2D fluid simulation
 *
 * Uses D2Q9 lattice (2D, 9 velocities) with BGK collision operator.
 * Simulates incompressible Navier-Stokes in the low Mach number limit.
 *
 * Algorithm:
 * 1. Collision: f_i -> f_i - (f_i - f_i^eq) / tau
 * 2. Streaming: f_i(x + c_i, t+1) = f_i(x, t)
 * 3. Boundary: Bounce-back for obstacles
 */

// D2Q9 lattice velocities
// Direction indices: 0=rest, 1-4=axial, 5-8=diagonal
//     6  2  5
//      \ | /
//    3 - 0 - 1
//      / | \
//     7  4  8

struct LBMConfig {
    int width = 256;
    int height = 128;
    float tau = 0.6f;           // Relaxation time (viscosity = (tau - 0.5) / 3)
    float inletVelocity = 0.1f; // Inlet flow velocity (in lattice units, should be < 0.3)
    bool useInlet = true;       // Left boundary as inlet
    bool useOutlet = true;      // Right boundary as outlet
};

struct LBMStats {
    float maxVelocity = 0.0f;
    float avgDensity = 1.0f;
    float reynoldsNumber = 0.0f;
    int obstacleCount = 0;
};

class LatticeBoltzmann {
public:
    LatticeBoltzmann();
    explicit LatticeBoltzmann(const LBMConfig& config);
    ~LatticeBoltzmann() = default;

    // Configuration
    void setConfig(const LBMConfig& config);
    void reset();
    void clear();

    // Simulation
    void step();
    void stepMultiple(int steps);

    // Obstacles (solid boundaries)
    void setObstacle(int x, int y, bool solid);
    void clearObstacles();
    void addCircleObstacle(int cx, int cy, int radius);
    void addRectObstacle(int x, int y, int w, int h);
    void addAirfoil(int cx, int cy, int length, float angle);

    // Parameters
    void setTau(float tau);
    void setInletVelocity(float v);
    float getViscosity() const;

    // Data access for JS
    const float* velocityXData() const { return velocityX_.data(); }
    const float* velocityYData() const { return velocityY_.data(); }
    const float* densityData() const { return density_.data(); }
    const float* vorticityData() const { return vorticity_.data(); }
    const uint8_t* obstacleData() const { return obstacles_.data(); }
    int dataSize() const { return width_ * height_; }
    int width() const { return width_; }
    int height() const { return height_; }

    // Statistics
    void computeStatistics();
    const LBMStats& stats() const { return stats_; }

private:
    // D2Q9 lattice constants
    static constexpr int Q = 9;  // Number of velocities
    static constexpr int cx[9] = { 0,  1,  0, -1,  0,  1, -1, -1,  1 };
    static constexpr int cy[9] = { 0,  0,  1,  0, -1,  1,  1, -1, -1 };
    static constexpr float w[9] = {
        4.0f/9.0f,                          // rest
        1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f,  // axial
        1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f  // diagonal
    };

    // Grid dimensions
    int width_, height_;

    // Configuration
    LBMConfig config_;

    // Distribution functions (double buffered)
    std::vector<float> f_;      // Current distributions [height][width][Q]
    std::vector<float> fTemp_;  // Temporary buffer for streaming

    // Macroscopic fields
    std::vector<float> density_;
    std::vector<float> velocityX_;
    std::vector<float> velocityY_;
    std::vector<float> vorticity_;

    // Obstacle mask (1 = solid, 0 = fluid)
    std::vector<uint8_t> obstacles_;

    // Statistics
    LBMStats stats_;

    // Helper functions
    int idx(int x, int y) const { return y * width_ + x; }
    int fidx(int x, int y, int i) const { return (y * width_ + x) * Q + i; }

    void initializeEquilibrium();
    void collide();
    void stream();
    void applyBoundaryConditions();
    void computeMacroscopic();
    void computeVorticity();
    float equilibrium(int i, float rho, float ux, float uy) const;
};

// Preset configurations
LBMConfig lbmPresetWindTunnel();
LBMConfig lbmPresetChannel();
LBMConfig lbmPresetHighReynolds();
LBMConfig lbmPresetLowViscosity();

} // namespace physics
} // namespace eigenlab
