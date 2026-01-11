#pragma once

#include <vector>
#include <cmath>
#include <algorithm>

namespace eigenlab {
namespace physics {

/**
 * WaveSolver2D - Solves 2D wave equation using finite differences
 *
 * PDE: d²u/dt² = c² * (d²u/dx² + d²u/dy²)
 *
 * Uses explicit FTCS (Forward Time Centered Space) scheme:
 * u(t+dt) = 2*u(t) - u(t-dt) + (c*dt/dx)² * Laplacian(u)
 *
 * Stability condition (CFL): c * dt / dx <= 1/sqrt(2)
 *
 * Features:
 * - Interactive point sources (drops)
 * - Multiple boundary conditions (reflective, absorbing, periodic)
 * - Real-time damping control
 * - High-resolution grid (up to 512x512)
 */

enum class WaveBoundary {
    REFLECTIVE,   // Hard boundaries (Dirichlet u=0)
    ABSORBING,    // Open boundaries (Mur ABC)
    PERIODIC      // Wrap around
};

struct WaveConfig {
    int width = 256;
    int height = 256;
    float waveSpeed = 1.0f;       // Wave propagation speed c
    float damping = 0.001f;       // Energy dissipation
    float dx = 1.0f;              // Spatial step
    float dt = 0.5f;              // Time step (auto-adjusted for CFL)
    WaveBoundary boundary = WaveBoundary::REFLECTIVE;
};

class WaveSolver2D {
public:
    WaveSolver2D(const WaveConfig& config);
    WaveSolver2D(int width, int height);
    ~WaveSolver2D() = default;

    // Core simulation
    void step(float dt);
    void reset();

    // Interactions
    void addDrop(float x, float y, float amplitude = 1.0f, float radius = 3.0f);
    void addLine(float x1, float y1, float x2, float y2, float amplitude = 0.5f);
    void addPlaneWave(float angle, float wavelength, float amplitude = 0.5f);

    // Obstacles
    void setObstacle(int x, int y, bool isObstacle = true);
    void addCircleObstacle(float cx, float cy, float radius);
    void addRectObstacle(int x1, int y1, int x2, int y2);
    void clearObstacles();

    // Parameters
    void setWaveSpeed(float c);
    void setDamping(float d);
    void setWaveBoundary(WaveBoundary bc);

    // Getters
    const float* getHeightField() const { return u_.data(); }
    const float* getVelocityField() const { return v_.data(); }
    const unsigned char* getObstacles() const { return obstacles_.data(); }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    int getSize() const { return width_ * height_; }
    float getWaveSpeed() const { return c_; }
    float getDamping() const { return damping_; }
    float getEnergy() const { return totalEnergy_; }
    float getMaxAmplitude() const { return maxAmplitude_; }

private:
    int width_, height_;
    float c_;           // Wave speed
    float damping_;
    float dx_, dt_;
    WaveBoundary boundary_;

    // State buffers (ping-pong)
    std::vector<float> u_;      // Current height
    std::vector<float> u_prev_; // Previous height
    std::vector<float> v_;      // Velocity (for visualization)
    std::vector<unsigned char> obstacles_; // Obstacle mask

    // Metrics
    float totalEnergy_;
    float maxAmplitude_;

    // Internal methods
    void computeLaplacian();
    void applyWaveBoundarys();
    void updateMetrics();

    inline int idx(int x, int y) const {
        return y * width_ + x;
    }

    inline bool isObstacle(int x, int y) const {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) return true;
        return obstacles_[idx(x, y)] != 0;
    }
};

// Factory functions for presets
WaveConfig wavePresetCalm();      // Slow, low damping
WaveConfig wavePresetPond();      // Medium speed, realistic damping
WaveConfig wavePresetRipple();    // Fast propagation
WaveConfig wavePresetSlit();      // Double slit diffraction setup

} // namespace physics
} // namespace eigenlab
