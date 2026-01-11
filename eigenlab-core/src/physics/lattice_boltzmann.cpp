#include "../../include/physics/lattice_boltzmann.hpp"
#include <cmath>
#include <algorithm>

namespace eigenlab {
namespace physics {

// Static member definitions
constexpr int LatticeBoltzmann::cx[9];
constexpr int LatticeBoltzmann::cy[9];
constexpr float LatticeBoltzmann::w[9];

// ==================================================
// Constructor & Configuration
// ==================================================

LatticeBoltzmann::LatticeBoltzmann()
    : width_(256)
    , height_(128)
{
    setConfig(LBMConfig{});
}

LatticeBoltzmann::LatticeBoltzmann(const LBMConfig& config)
    : width_(config.width)
    , height_(config.height)
{
    setConfig(config);
}

void LatticeBoltzmann::setConfig(const LBMConfig& config) {
    config_ = config;
    width_ = config.width;
    height_ = config.height;

    // Allocate arrays
    int size = width_ * height_;
    f_.resize(size * Q);
    fTemp_.resize(size * Q);
    density_.resize(size);
    velocityX_.resize(size);
    velocityY_.resize(size);
    vorticity_.resize(size);
    obstacles_.resize(size, 0);

    reset();
}

void LatticeBoltzmann::reset() {
    initializeEquilibrium();
    computeMacroscopic();
    computeVorticity();
}

void LatticeBoltzmann::clear() {
    std::fill(obstacles_.begin(), obstacles_.end(), 0);
    stats_.obstacleCount = 0;
    reset();
}

// ==================================================
// Initialization
// ==================================================

void LatticeBoltzmann::initializeEquilibrium() {
    // Initialize with equilibrium distribution at rest (or with inlet velocity)
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            float rho = 1.0f;
            float ux = config_.useInlet ? config_.inletVelocity : 0.0f;
            float uy = 0.0f;

            for (int i = 0; i < Q; ++i) {
                f_[fidx(x, y, i)] = equilibrium(i, rho, ux, uy);
            }
        }
    }
}

float LatticeBoltzmann::equilibrium(int i, float rho, float ux, float uy) const {
    // Equilibrium distribution function
    // f_i^eq = w_i * rho * (1 + 3*c_i.u + 4.5*(c_i.u)^2 - 1.5*u^2)
    float cu = cx[i] * ux + cy[i] * uy;  // c_i . u
    float u2 = ux * ux + uy * uy;         // |u|^2
    return w[i] * rho * (1.0f + 3.0f * cu + 4.5f * cu * cu - 1.5f * u2);
}

// ==================================================
// Simulation Steps
// ==================================================

void LatticeBoltzmann::step() {
    collide();
    stream();
    applyBoundaryConditions();
    computeMacroscopic();
}

void LatticeBoltzmann::stepMultiple(int steps) {
    for (int i = 0; i < steps; ++i) {
        step();
    }
    computeVorticity();
}

void LatticeBoltzmann::collide() {
    // BGK collision operator
    // f_i = f_i - (f_i - f_i^eq) / tau

    float invTau = 1.0f / config_.tau;

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            if (obstacles_[idx(x, y)]) continue;

            // Compute macroscopic quantities at this node
            float rho = 0.0f;
            float ux = 0.0f;
            float uy = 0.0f;

            for (int i = 0; i < Q; ++i) {
                float fi = f_[fidx(x, y, i)];
                rho += fi;
                ux += cx[i] * fi;
                uy += cy[i] * fi;
            }

            if (rho > 0.0f) {
                ux /= rho;
                uy /= rho;
            }

            // Apply collision
            for (int i = 0; i < Q; ++i) {
                float feq = equilibrium(i, rho, ux, uy);
                f_[fidx(x, y, i)] -= invTau * (f_[fidx(x, y, i)] - feq);
            }
        }
    }
}

void LatticeBoltzmann::stream() {
    // Stream: f_i(x + c_i, t+1) = f_i(x, t)
    // Copy to temp buffer first

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            for (int i = 0; i < Q; ++i) {
                int xn = x + cx[i];
                int yn = y + cy[i];

                // Periodic boundaries in y (for channel flow)
                if (yn < 0) yn = height_ - 1;
                if (yn >= height_) yn = 0;

                // Handle x boundaries
                if (xn >= 0 && xn < width_) {
                    fTemp_[fidx(xn, yn, i)] = f_[fidx(x, y, i)];
                }
            }
        }
    }

    // Swap buffers
    std::swap(f_, fTemp_);
}

void LatticeBoltzmann::applyBoundaryConditions() {
    // Bounce-back for obstacles
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            if (!obstacles_[idx(x, y)]) continue;

            // For each direction, bounce back to opposite
            // opposite[i] gives the index of -c_i
            static const int opposite[9] = { 0, 3, 4, 1, 2, 7, 8, 5, 6 };

            for (int i = 1; i < Q; ++i) {
                int xn = x - cx[i];
                int yn = y - cy[i];

                if (xn >= 0 && xn < width_ && yn >= 0 && yn < height_) {
                    if (!obstacles_[idx(xn, yn)]) {
                        f_[fidx(xn, yn, opposite[i])] = f_[fidx(x, y, i)];
                    }
                }
            }
        }
    }

    // Inlet boundary (left, x=0) - Zou/He velocity BC
    if (config_.useInlet) {
        float ux = config_.inletVelocity;
        for (int y = 1; y < height_ - 1; ++y) {
            if (obstacles_[idx(0, y)]) continue;

            // Zou/He inlet: known velocity, compute density
            float f2 = f_[fidx(0, y, 2)];
            float f3 = f_[fidx(0, y, 3)];
            float f4 = f_[fidx(0, y, 4)];
            float f6 = f_[fidx(0, y, 6)];
            float f7 = f_[fidx(0, y, 7)];

            float rho = (f_[fidx(0, y, 0)] + f2 + f4 + 2.0f * (f3 + f6 + f7)) / (1.0f - ux);

            // Unknown distributions: f1, f5, f8
            f_[fidx(0, y, 1)] = f3 + (2.0f / 3.0f) * rho * ux;
            f_[fidx(0, y, 5)] = f7 + 0.5f * (f4 - f2) + (1.0f / 6.0f) * rho * ux;
            f_[fidx(0, y, 8)] = f6 + 0.5f * (f2 - f4) + (1.0f / 6.0f) * rho * ux;
        }
    }

    // Outlet boundary (right, x=width-1) - extrapolation/copy
    if (config_.useOutlet) {
        int x = width_ - 1;
        for (int y = 0; y < height_; ++y) {
            if (obstacles_[idx(x, y)]) continue;

            // Copy from second-to-last column
            for (int i = 0; i < Q; ++i) {
                f_[fidx(x, y, i)] = f_[fidx(x - 1, y, i)];
            }
        }
    }

    // Top and bottom walls - bounce-back
    for (int x = 0; x < width_; ++x) {
        // Bottom (y=0)
        f_[fidx(x, 0, 2)] = f_[fidx(x, 0, 4)];
        f_[fidx(x, 0, 5)] = f_[fidx(x, 0, 7)];
        f_[fidx(x, 0, 6)] = f_[fidx(x, 0, 8)];

        // Top (y=height-1)
        int y = height_ - 1;
        f_[fidx(x, y, 4)] = f_[fidx(x, y, 2)];
        f_[fidx(x, y, 7)] = f_[fidx(x, y, 5)];
        f_[fidx(x, y, 8)] = f_[fidx(x, y, 6)];
    }
}

void LatticeBoltzmann::computeMacroscopic() {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            int i0 = idx(x, y);

            if (obstacles_[i0]) {
                density_[i0] = 0.0f;
                velocityX_[i0] = 0.0f;
                velocityY_[i0] = 0.0f;
                continue;
            }

            float rho = 0.0f;
            float ux = 0.0f;
            float uy = 0.0f;

            for (int i = 0; i < Q; ++i) {
                float fi = f_[fidx(x, y, i)];
                rho += fi;
                ux += cx[i] * fi;
                uy += cy[i] * fi;
            }

            if (rho > 0.0f) {
                ux /= rho;
                uy /= rho;
            }

            density_[i0] = rho;
            velocityX_[i0] = ux;
            velocityY_[i0] = uy;
        }
    }
}

void LatticeBoltzmann::computeVorticity() {
    // Vorticity = duy/dx - dux/dy (curl in 2D)
    for (int y = 1; y < height_ - 1; ++y) {
        for (int x = 1; x < width_ - 1; ++x) {
            float duy_dx = (velocityY_[idx(x + 1, y)] - velocityY_[idx(x - 1, y)]) * 0.5f;
            float dux_dy = (velocityX_[idx(x, y + 1)] - velocityX_[idx(x, y - 1)]) * 0.5f;
            vorticity_[idx(x, y)] = duy_dx - dux_dy;
        }
    }

    // Boundary vorticity = 0
    for (int x = 0; x < width_; ++x) {
        vorticity_[idx(x, 0)] = 0.0f;
        vorticity_[idx(x, height_ - 1)] = 0.0f;
    }
    for (int y = 0; y < height_; ++y) {
        vorticity_[idx(0, y)] = 0.0f;
        vorticity_[idx(width_ - 1, y)] = 0.0f;
    }
}

// ==================================================
// Obstacles
// ==================================================

void LatticeBoltzmann::setObstacle(int x, int y, bool solid) {
    if (x >= 0 && x < width_ && y >= 0 && y < height_) {
        uint8_t prev = obstacles_[idx(x, y)];
        obstacles_[idx(x, y)] = solid ? 1 : 0;

        if (solid && !prev) stats_.obstacleCount++;
        else if (!solid && prev) stats_.obstacleCount--;
    }
}

void LatticeBoltzmann::clearObstacles() {
    std::fill(obstacles_.begin(), obstacles_.end(), 0);
    stats_.obstacleCount = 0;
}

void LatticeBoltzmann::addCircleObstacle(int cx, int cy, int radius) {
    int r2 = radius * radius;
    for (int y = cy - radius; y <= cy + radius; ++y) {
        for (int x = cx - radius; x <= cx + radius; ++x) {
            int dx = x - cx;
            int dy = y - cy;
            if (dx * dx + dy * dy <= r2) {
                setObstacle(x, y, true);
            }
        }
    }
}

void LatticeBoltzmann::addRectObstacle(int x0, int y0, int w, int h) {
    for (int y = y0; y < y0 + h; ++y) {
        for (int x = x0; x < x0 + w; ++x) {
            setObstacle(x, y, true);
        }
    }
}

void LatticeBoltzmann::addAirfoil(int cx, int cy, int length, float angle) {
    // Simple symmetric airfoil (NACA 0012-like)
    float cosA = std::cos(angle);
    float sinA = std::sin(angle);

    for (int i = -length / 2; i <= length / 2; ++i) {
        // NACA 0012 thickness distribution (simplified)
        float t = static_cast<float>(i + length / 2) / length;
        float thickness = 0.12f * length * (
            0.2969f * std::sqrt(t)
            - 0.126f * t
            - 0.3516f * t * t
            + 0.2843f * t * t * t
            - 0.1015f * t * t * t * t
        );

        int halfT = static_cast<int>(thickness * 0.5f);

        for (int j = -halfT; j <= halfT; ++j) {
            int x = cx + static_cast<int>(i * cosA - j * sinA);
            int y = cy + static_cast<int>(i * sinA + j * cosA);
            setObstacle(x, y, true);
        }
    }
}

// ==================================================
// Parameters
// ==================================================

void LatticeBoltzmann::setTau(float tau) {
    config_.tau = std::max(0.51f, tau);  // Must be > 0.5 for stability
}

void LatticeBoltzmann::setInletVelocity(float v) {
    config_.inletVelocity = std::min(0.3f, std::max(0.0f, v));  // Keep < 0.3 for stability
}

float LatticeBoltzmann::getViscosity() const {
    // kinematic viscosity = (tau - 0.5) / 3 (in lattice units)
    return (config_.tau - 0.5f) / 3.0f;
}

// ==================================================
// Statistics
// ==================================================

void LatticeBoltzmann::computeStatistics() {
    float maxV = 0.0f;
    float sumRho = 0.0f;
    int count = 0;

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            int i0 = idx(x, y);
            if (obstacles_[i0]) continue;

            float vx = velocityX_[i0];
            float vy = velocityY_[i0];
            float v = std::sqrt(vx * vx + vy * vy);

            if (v > maxV) maxV = v;
            sumRho += density_[i0];
            count++;
        }
    }

    stats_.maxVelocity = maxV;
    stats_.avgDensity = count > 0 ? sumRho / count : 1.0f;

    // Reynolds number: Re = u * L / nu
    // Using inlet velocity and channel height as characteristic scales
    float nu = getViscosity();
    if (nu > 0.0001f) {
        stats_.reynoldsNumber = config_.inletVelocity * height_ / nu;
    }
}

// ==================================================
// Presets
// ==================================================

LBMConfig lbmPresetWindTunnel() {
    LBMConfig config;
    config.width = 400;
    config.height = 150;
    config.tau = 0.6f;
    config.inletVelocity = 0.1f;
    config.useInlet = true;
    config.useOutlet = true;
    return config;
}

LBMConfig lbmPresetChannel() {
    LBMConfig config;
    config.width = 300;
    config.height = 100;
    config.tau = 0.55f;
    config.inletVelocity = 0.08f;
    config.useInlet = true;
    config.useOutlet = true;
    return config;
}

LBMConfig lbmPresetHighReynolds() {
    LBMConfig config;
    config.width = 500;
    config.height = 200;
    config.tau = 0.52f;   // Lower tau = lower viscosity = higher Re
    config.inletVelocity = 0.15f;
    config.useInlet = true;
    config.useOutlet = true;
    return config;
}

LBMConfig lbmPresetLowViscosity() {
    LBMConfig config;
    config.width = 350;
    config.height = 120;
    config.tau = 0.51f;   // Very low viscosity (near stability limit)
    config.inletVelocity = 0.05f;
    config.useInlet = true;
    config.useOutlet = true;
    return config;
}

} // namespace physics
} // namespace eigenlab
