#include "../../include/physics/wave_solver.hpp"
#include <cstring>
#include <algorithm>

namespace eigenlab {
namespace physics {

// ==================================================
// Constructor & Initialization
// ==================================================

WaveSolver2D::WaveSolver2D(const WaveConfig& config)
    : width_(config.width)
    , height_(config.height)
    , c_(config.waveSpeed)
    , damping_(config.damping)
    , dx_(config.dx)
    , dt_(config.dt)
    , boundary_(config.boundary)
    , totalEnergy_(0.0f)
    , maxAmplitude_(0.0f)
{
    // CFL stability check and adjustment
    float maxDt = dx_ / (c_ * std::sqrt(2.0f)) * 0.9f;
    if (dt_ > maxDt) {
        dt_ = maxDt;
    }

    // Allocate buffers
    int size = width_ * height_;
    u_.resize(size, 0.0f);
    u_prev_.resize(size, 0.0f);
    v_.resize(size, 0.0f);
    obstacles_.resize(size, 0);
}

WaveSolver2D::WaveSolver2D(int width, int height)
    : WaveSolver2D(WaveConfig{width, height})
{
}

// ==================================================
// Core Simulation
// ==================================================

void WaveSolver2D::step(float dt) {
    // Use internal dt for stability
    float actualDt = dt_;

    // Courant number squared
    float c2 = (c_ * actualDt / dx_) * (c_ * actualDt / dx_);

    // Damping factor
    float dampFactor = 1.0f - damping_;

    // Compute new wave field using finite differences
    // u_next = 2*u - u_prev + c² * Laplacian(u)

    std::vector<float> u_next(width_ * height_);

    for (int y = 1; y < height_ - 1; ++y) {
        for (int x = 1; x < width_ - 1; ++x) {
            int i = idx(x, y);

            // Skip obstacles
            if (obstacles_[i]) {
                u_next[i] = 0.0f;
                continue;
            }

            // 5-point Laplacian stencil
            float laplacian = 0.0f;
            int neighbors = 0;

            // Right
            if (!isObstacle(x + 1, y)) {
                laplacian += u_[idx(x + 1, y)];
                neighbors++;
            }
            // Left
            if (!isObstacle(x - 1, y)) {
                laplacian += u_[idx(x - 1, y)];
                neighbors++;
            }
            // Down
            if (!isObstacle(x, y + 1)) {
                laplacian += u_[idx(x, y + 1)];
                neighbors++;
            }
            // Up
            if (!isObstacle(x, y - 1)) {
                laplacian += u_[idx(x, y - 1)];
                neighbors++;
            }

            // Adjust for missing neighbors (boundary handling)
            if (neighbors < 4) {
                laplacian += (4 - neighbors) * u_[i];
            }

            laplacian -= 4.0f * u_[i];

            // Update equation with damping
            u_next[i] = dampFactor * (2.0f * u_[i] - u_prev_[i] + c2 * laplacian);
        }
    }

    // Apply boundary conditions
    switch (boundary_) {
        case WaveBoundary::REFLECTIVE:
            // Dirichlet: u = 0 at boundaries (already initialized to 0)
            for (int x = 0; x < width_; ++x) {
                u_next[idx(x, 0)] = 0.0f;
                u_next[idx(x, height_ - 1)] = 0.0f;
            }
            for (int y = 0; y < height_; ++y) {
                u_next[idx(0, y)] = 0.0f;
                u_next[idx(width_ - 1, y)] = 0.0f;
            }
            break;

        case WaveBoundary::ABSORBING:
            // Mur's first-order ABC
            {
                float r = (c_ * actualDt - dx_) / (c_ * actualDt + dx_);

                // Left and right boundaries
                for (int y = 1; y < height_ - 1; ++y) {
                    // Left
                    u_next[idx(0, y)] = u_[idx(1, y)] + r * (u_next[idx(1, y)] - u_[idx(0, y)]);
                    // Right
                    u_next[idx(width_ - 1, y)] = u_[idx(width_ - 2, y)] +
                        r * (u_next[idx(width_ - 2, y)] - u_[idx(width_ - 1, y)]);
                }

                // Top and bottom boundaries
                for (int x = 1; x < width_ - 1; ++x) {
                    // Top
                    u_next[idx(x, 0)] = u_[idx(x, 1)] + r * (u_next[idx(x, 1)] - u_[idx(x, 0)]);
                    // Bottom
                    u_next[idx(x, height_ - 1)] = u_[idx(x, height_ - 2)] +
                        r * (u_next[idx(x, height_ - 2)] - u_[idx(x, height_ - 1)]);
                }
            }
            break;

        case WaveBoundary::PERIODIC:
            // Wrap around
            for (int x = 0; x < width_; ++x) {
                u_next[idx(x, 0)] = u_next[idx(x, height_ - 2)];
                u_next[idx(x, height_ - 1)] = u_next[idx(x, 1)];
            }
            for (int y = 0; y < height_; ++y) {
                u_next[idx(0, y)] = u_next[idx(width_ - 2, y)];
                u_next[idx(width_ - 1, y)] = u_next[idx(1, y)];
            }
            break;
    }

    // Calculate velocity field for visualization
    for (int i = 0; i < width_ * height_; ++i) {
        v_[i] = (u_next[i] - u_prev_[i]) / (2.0f * actualDt);
    }

    // Swap buffers
    u_prev_ = u_;
    u_ = u_next;

    // Update metrics
    updateMetrics();
}

void WaveSolver2D::reset() {
    std::fill(u_.begin(), u_.end(), 0.0f);
    std::fill(u_prev_.begin(), u_prev_.end(), 0.0f);
    std::fill(v_.begin(), v_.end(), 0.0f);
    totalEnergy_ = 0.0f;
    maxAmplitude_ = 0.0f;
}

// ==================================================
// Interactions
// ==================================================

void WaveSolver2D::addDrop(float x, float y, float amplitude, float radius) {
    int cx = static_cast<int>(x);
    int cy = static_cast<int>(y);
    int r = static_cast<int>(radius);

    for (int dy = -r * 2; dy <= r * 2; ++dy) {
        for (int dx = -r * 2; dx <= r * 2; ++dx) {
            int px = cx + dx;
            int py = cy + dy;

            if (px < 0 || px >= width_ || py < 0 || py >= height_) continue;
            if (obstacles_[idx(px, py)]) continue;

            float dist = std::sqrt(static_cast<float>(dx * dx + dy * dy));

            // Gaussian profile for smooth drop
            if (dist < radius * 2) {
                float factor = std::exp(-(dist * dist) / (2.0f * radius * radius));
                u_[idx(px, py)] += amplitude * factor;
                u_prev_[idx(px, py)] += amplitude * factor;
            }
        }
    }
}

void WaveSolver2D::addLine(float x1, float y1, float x2, float y2, float amplitude) {
    // Bresenham-style line with wave source
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = std::sqrt(dx * dx + dy * dy);

    if (length < 1.0f) return;

    int steps = static_cast<int>(length);
    dx /= length;
    dy /= length;

    for (int i = 0; i <= steps; ++i) {
        int px = static_cast<int>(x1 + dx * i);
        int py = static_cast<int>(y1 + dy * i);

        if (px >= 0 && px < width_ && py >= 0 && py < height_) {
            if (!obstacles_[idx(px, py)]) {
                u_[idx(px, py)] += amplitude;
                u_prev_[idx(px, py)] += amplitude;
            }
        }
    }
}

void WaveSolver2D::addPlaneWave(float angle, float wavelength, float amplitude) {
    float kx = std::cos(angle) * 2.0f * M_PI / wavelength;
    float ky = std::sin(angle) * 2.0f * M_PI / wavelength;

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            if (obstacles_[idx(x, y)]) continue;

            float phase = kx * x + ky * y;
            u_[idx(x, y)] = amplitude * std::sin(phase);
            u_prev_[idx(x, y)] = amplitude * std::sin(phase - c_ * dt_);
        }
    }
}

// ==================================================
// Obstacles
// ==================================================

void WaveSolver2D::setObstacle(int x, int y, bool isObstacle) {
    if (x >= 0 && x < width_ && y >= 0 && y < height_) {
        obstacles_[idx(x, y)] = isObstacle ? 1 : 0;
        if (isObstacle) {
            u_[idx(x, y)] = 0.0f;
            u_prev_[idx(x, y)] = 0.0f;
        }
    }
}

void WaveSolver2D::addCircleObstacle(float cx, float cy, float radius) {
    int r = static_cast<int>(radius);
    int icx = static_cast<int>(cx);
    int icy = static_cast<int>(cy);

    for (int dy = -r; dy <= r; ++dy) {
        for (int dx = -r; dx <= r; ++dx) {
            if (dx * dx + dy * dy <= r * r) {
                setObstacle(icx + dx, icy + dy, true);
            }
        }
    }
}

void WaveSolver2D::addRectObstacle(int x1, int y1, int x2, int y2) {
    int minX = std::min(x1, x2);
    int maxX = std::max(x1, x2);
    int minY = std::min(y1, y2);
    int maxY = std::max(y1, y2);

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            setObstacle(x, y, true);
        }
    }
}

void WaveSolver2D::clearObstacles() {
    std::fill(obstacles_.begin(), obstacles_.end(), 0);
}

// ==================================================
// Parameters
// ==================================================

void WaveSolver2D::setWaveSpeed(float c) {
    c_ = std::max(0.1f, c);

    // Recalculate dt for stability
    float maxDt = dx_ / (c_ * std::sqrt(2.0f)) * 0.9f;
    dt_ = std::min(dt_, maxDt);
}

void WaveSolver2D::setDamping(float d) {
    damping_ = std::clamp(d, 0.0f, 0.1f);
}

void WaveSolver2D::setWaveBoundary(WaveBoundary bc) {
    boundary_ = bc;
}

// ==================================================
// Metrics
// ==================================================

void WaveSolver2D::updateMetrics() {
    totalEnergy_ = 0.0f;
    maxAmplitude_ = 0.0f;

    for (int i = 0; i < width_ * height_; ++i) {
        if (!obstacles_[i]) {
            float h = u_[i];
            float vel = v_[i];

            // Total energy = kinetic + potential
            totalEnergy_ += 0.5f * (vel * vel + c_ * c_ * h * h);

            maxAmplitude_ = std::max(maxAmplitude_, std::abs(h));
        }
    }

    // Normalize by grid size
    totalEnergy_ /= (width_ * height_);
}

// ==================================================
// Presets
// ==================================================

WaveConfig wavePresetCalm() {
    WaveConfig config;
    config.width = 256;
    config.height = 256;
    config.waveSpeed = 0.5f;
    config.damping = 0.0005f;
    config.boundary = WaveBoundary::REFLECTIVE;
    return config;
}

WaveConfig wavePresetPond() {
    WaveConfig config;
    config.width = 384;
    config.height = 384;
    config.waveSpeed = 1.0f;
    config.damping = 0.002f;
    config.boundary = WaveBoundary::ABSORBING;
    return config;
}

WaveConfig wavePresetRipple() {
    WaveConfig config;
    config.width = 512;
    config.height = 512;
    config.waveSpeed = 2.0f;
    config.damping = 0.001f;
    config.boundary = WaveBoundary::REFLECTIVE;
    return config;
}

WaveConfig wavePresetSlit() {
    WaveConfig config;
    config.width = 400;
    config.height = 300;
    config.waveSpeed = 1.5f;
    config.damping = 0.0008f;
    config.boundary = WaveBoundary::ABSORBING;
    return config;
}

} // namespace physics
} // namespace eigenlab
