/**
 * @file fluid_solver.cpp
 * @brief Implementation of real-time 2D fluid simulation
 *
 * Based on Jos Stam's "Stable Fluids" (SIGGRAPH 1999)
 * With vorticity confinement from Fedkiw et al.
 */

#include "../../include/physics/fluid_solver.hpp"
#include <algorithm>
#include <cmath>

namespace eigenlab {
namespace physics {

// ============================================================================
// Constructor / Configuration
// ============================================================================

FluidSolver::FluidSolver() {
    setConfig(FluidConfig{});
}

FluidSolver::FluidSolver(const FluidConfig& config) {
    setConfig(config);
}

void FluidSolver::setConfig(const FluidConfig& config) {
    m_config = config;
    allocateBuffers();
    clear();
}

void FluidSolver::resize(u32 resolution) {
    m_config.resolution = resolution;
    allocateBuffers();
    clear();
}

void FluidSolver::allocateBuffers() {
    // Grid size + 2 for boundary cells
    u32 n = m_config.resolution + 2;
    m_size = static_cast<usize>(n) * n;

    m_velocityX.resize(m_size, 0.0f);
    m_velocityY.resize(m_size, 0.0f);
    m_velocityX0.resize(m_size, 0.0f);
    m_velocityY0.resize(m_size, 0.0f);

    m_density.resize(m_size, 0.0f);
    m_density0.resize(m_size, 0.0f);
    m_pressure.resize(m_size, 0.0f);
    m_divergence.resize(m_size, 0.0f);
    m_vorticity.resize(m_size, 0.0f);

    m_densityR.resize(m_size, 0.0f);
    m_densityG.resize(m_size, 0.0f);
    m_densityB.resize(m_size, 0.0f);
    m_densityR0.resize(m_size, 0.0f);
    m_densityG0.resize(m_size, 0.0f);
    m_densityB0.resize(m_size, 0.0f);

    m_sourceVx.resize(m_size, 0.0f);
    m_sourceVy.resize(m_size, 0.0f);
    m_sourceDensity.resize(m_size, 0.0f);
    m_sourceR.resize(m_size, 0.0f);
    m_sourceG.resize(m_size, 0.0f);
    m_sourceB.resize(m_size, 0.0f);
}

void FluidSolver::clear() {
    std::fill(m_velocityX.begin(), m_velocityX.end(), 0.0f);
    std::fill(m_velocityY.begin(), m_velocityY.end(), 0.0f);
    std::fill(m_velocityX0.begin(), m_velocityX0.end(), 0.0f);
    std::fill(m_velocityY0.begin(), m_velocityY0.end(), 0.0f);
    std::fill(m_density.begin(), m_density.end(), 0.0f);
    std::fill(m_density0.begin(), m_density0.end(), 0.0f);
    std::fill(m_pressure.begin(), m_pressure.end(), 0.0f);
    std::fill(m_densityR.begin(), m_densityR.end(), 0.0f);
    std::fill(m_densityG.begin(), m_densityG.end(), 0.0f);
    std::fill(m_densityB.begin(), m_densityB.end(), 0.0f);
    std::fill(m_sourceVx.begin(), m_sourceVx.end(), 0.0f);
    std::fill(m_sourceVy.begin(), m_sourceVy.end(), 0.0f);
    std::fill(m_sourceDensity.begin(), m_sourceDensity.end(), 0.0f);
    std::fill(m_sourceR.begin(), m_sourceR.end(), 0.0f);
    std::fill(m_sourceG.begin(), m_sourceG.end(), 0.0f);
    std::fill(m_sourceB.begin(), m_sourceB.end(), 0.0f);
    m_stats = FluidStats{};
}

void FluidSolver::clearVelocity() {
    std::fill(m_velocityX.begin(), m_velocityX.end(), 0.0f);
    std::fill(m_velocityY.begin(), m_velocityY.end(), 0.0f);
}

void FluidSolver::clearDensity() {
    std::fill(m_density.begin(), m_density.end(), 0.0f);
    std::fill(m_densityR.begin(), m_densityR.end(), 0.0f);
    std::fill(m_densityG.begin(), m_densityG.end(), 0.0f);
    std::fill(m_densityB.begin(), m_densityB.end(), 0.0f);
}


// ============================================================================
// Simulation Step
// ============================================================================

void FluidSolver::step(Real dt) {
    u32 N = m_config.resolution;

    // Add accumulated sources
    addSource(m_velocityX, m_sourceVx, dt);
    addSource(m_velocityY, m_sourceVy, dt);
    addSource(m_density, m_sourceDensity, dt);
    addSource(m_densityR, m_sourceR, dt);
    addSource(m_densityG, m_sourceG, dt);
    addSource(m_densityB, m_sourceB, dt);

    // Clear source buffers
    std::fill(m_sourceVx.begin(), m_sourceVx.end(), 0.0f);
    std::fill(m_sourceVy.begin(), m_sourceVy.end(), 0.0f);
    std::fill(m_sourceDensity.begin(), m_sourceDensity.end(), 0.0f);
    std::fill(m_sourceR.begin(), m_sourceR.end(), 0.0f);
    std::fill(m_sourceG.begin(), m_sourceG.end(), 0.0f);
    std::fill(m_sourceB.begin(), m_sourceB.end(), 0.0f);

    // === VELOCITY STEP ===

    // Diffuse velocity
    if (m_config.viscosity > 0) {
        std::swap(m_velocityX, m_velocityX0);
        std::swap(m_velocityY, m_velocityY0);
        diffuse(m_velocityX, m_velocityX0, m_config.viscosity, dt);
        diffuse(m_velocityY, m_velocityY0, m_config.viscosity, dt);
    }

    // Make velocity field divergence-free
    project();

    // Advect velocity
    std::swap(m_velocityX, m_velocityX0);
    std::swap(m_velocityY, m_velocityY0);
    advect(m_velocityX, m_velocityX0, m_velocityX0, m_velocityY0, dt);
    advect(m_velocityY, m_velocityY0, m_velocityX0, m_velocityY0, dt);

    // Project again
    project();

    // Apply vorticity confinement
    if (m_config.enableVorticity && m_config.vorticityStrength > 0) {
        applyVorticityConfinement(dt);
    }

    // Dissipate velocity
    dissipate(m_velocityX, m_config.dissipation);
    dissipate(m_velocityY, m_config.dissipation);

    // === DENSITY STEP ===

    // Diffuse density
    if (m_config.diffusion > 0) {
        std::swap(m_density, m_density0);
        diffuse(m_density, m_density0, m_config.diffusion, dt);

        std::swap(m_densityR, m_densityR0);
        std::swap(m_densityG, m_densityG0);
        std::swap(m_densityB, m_densityB0);
        diffuse(m_densityR, m_densityR0, m_config.diffusion, dt);
        diffuse(m_densityG, m_densityG0, m_config.diffusion, dt);
        diffuse(m_densityB, m_densityB0, m_config.diffusion, dt);
    }

    // Advect density
    std::swap(m_density, m_density0);
    advect(m_density, m_density0, m_velocityX, m_velocityY, dt);

    std::swap(m_densityR, m_densityR0);
    std::swap(m_densityG, m_densityG0);
    std::swap(m_densityB, m_densityB0);
    advect(m_densityR, m_densityR0, m_velocityX, m_velocityY, dt);
    advect(m_densityG, m_densityG0, m_velocityX, m_velocityY, dt);
    advect(m_densityB, m_densityB0, m_velocityX, m_velocityY, dt);

    // Dissipate density
    dissipate(m_density, m_config.densityDissipation);
    dissipate(m_densityR, m_config.densityDissipation);
    dissipate(m_densityG, m_config.densityDissipation);
    dissipate(m_densityB, m_config.densityDissipation);

    m_stats.iterations++;
    m_stats.simulationTime += dt;
}

// ============================================================================
// Core Solver Components
// ============================================================================

void FluidSolver::addSource(std::vector<Real>& field, const std::vector<Real>& source, Real dt) {
    for (usize i = 0; i < m_size; ++i) {
        field[i] += dt * source[i];
    }
}

void FluidSolver::diffuse(std::vector<Real>& field, const std::vector<Real>& field0, Real diff, Real dt) {
    u32 N = m_config.resolution;
    Real a = dt * diff * N * N;
    Real c = 1.0f + 4.0f * a;

    // Gauss-Seidel relaxation
    for (u32 k = 0; k < m_config.diffuseIterations; ++k) {
        for (u32 j = 1; j <= N; ++j) {
            for (u32 i = 1; i <= N; ++i) {
                field[idx(i, j)] = (field0[idx(i, j)] +
                    a * (field[idx(i-1, j)] + field[idx(i+1, j)] +
                         field[idx(i, j-1)] + field[idx(i, j+1)])) / c;
            }
        }
        setBoundary(field, 0);
    }
}

void FluidSolver::advect(std::vector<Real>& field, const std::vector<Real>& field0,
                         const std::vector<Real>& vx, const std::vector<Real>& vy, Real dt) {
    u32 N = m_config.resolution;
    Real dt0 = dt * N;

    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            // Trace back
            Real x = i - dt0 * vx[idx(i, j)];
            Real y = j - dt0 * vy[idx(i, j)];

            // Clamp to grid
            x = std::max(0.5f, std::min(N + 0.5f, x));
            y = std::max(0.5f, std::min(N + 0.5f, y));

            // Bilinear interpolation
            field[idx(i, j)] = bilinearSample(field0, x, y);
        }
    }
    setBoundary(field, 0);
}

void FluidSolver::project() {
    u32 N = m_config.resolution;
    Real h = 1.0f / N;

    // Calculate divergence
    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            m_divergence[idx(i, j)] = -0.5f * h * (
                m_velocityX[idx(i+1, j)] - m_velocityX[idx(i-1, j)] +
                m_velocityY[idx(i, j+1)] - m_velocityY[idx(i, j-1)]
            );
            m_pressure[idx(i, j)] = 0.0f;
        }
    }
    setBoundary(m_divergence, 0);
    setBoundary(m_pressure, 0);

    // Solve pressure Poisson equation
    for (u32 k = 0; k < m_config.pressureIterations; ++k) {
        for (u32 j = 1; j <= N; ++j) {
            for (u32 i = 1; i <= N; ++i) {
                m_pressure[idx(i, j)] = (m_divergence[idx(i, j)] +
                    m_pressure[idx(i-1, j)] + m_pressure[idx(i+1, j)] +
                    m_pressure[idx(i, j-1)] + m_pressure[idx(i, j+1)]) * 0.25f;
            }
        }
        setBoundary(m_pressure, 0);
    }

    // Subtract pressure gradient from velocity
    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            m_velocityX[idx(i, j)] -= 0.5f * N * (m_pressure[idx(i+1, j)] - m_pressure[idx(i-1, j)]);
            m_velocityY[idx(i, j)] -= 0.5f * N * (m_pressure[idx(i, j+1)] - m_pressure[idx(i, j-1)]);
        }
    }
    setBoundary(m_velocityX, 1);
    setBoundary(m_velocityY, 2);
}

void FluidSolver::applyVorticityConfinement(Real dt) {
    u32 N = m_config.resolution;
    Real eps = m_config.vorticityStrength;

    // Calculate vorticity (curl of velocity)
    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            Real dvydx = (m_velocityY[idx(i+1, j)] - m_velocityY[idx(i-1, j)]) * 0.5f;
            Real dvxdy = (m_velocityX[idx(i, j+1)] - m_velocityX[idx(i, j-1)]) * 0.5f;
            m_vorticity[idx(i, j)] = dvydx - dvxdy;
        }
    }

    // Calculate vorticity confinement force
    for (u32 j = 2; j < N; ++j) {
        for (u32 i = 2; i < N; ++i) {
            // Gradient of vorticity magnitude
            Real dw_dx = (std::abs(m_vorticity[idx(i+1, j)]) - std::abs(m_vorticity[idx(i-1, j)])) * 0.5f;
            Real dw_dy = (std::abs(m_vorticity[idx(i, j+1)]) - std::abs(m_vorticity[idx(i, j-1)])) * 0.5f;

            Real len = std::sqrt(dw_dx * dw_dx + dw_dy * dw_dy) + 1e-5f;

            // Normalized gradient
            Real nx = dw_dx / len;
            Real ny = dw_dy / len;

            // Vorticity confinement force
            Real w = m_vorticity[idx(i, j)];
            m_velocityX[idx(i, j)] += dt * eps * ny * w;
            m_velocityY[idx(i, j)] -= dt * eps * nx * w;
        }
    }
}

void FluidSolver::setBoundary(std::vector<Real>& field, int b) {
    u32 N = m_config.resolution;

    if (!m_config.boundaryWalls) {
        // Open boundaries - copy from interior
        for (u32 i = 1; i <= N; ++i) {
            field[idx(0, i)] = field[idx(1, i)];
            field[idx(N+1, i)] = field[idx(N, i)];
            field[idx(i, 0)] = field[idx(i, 1)];
            field[idx(i, N+1)] = field[idx(i, N)];
        }
    } else {
        // Solid walls - reflect velocity
        for (u32 i = 1; i <= N; ++i) {
            field[idx(0, i)]   = (b == 1) ? -field[idx(1, i)] : field[idx(1, i)];
            field[idx(N+1, i)] = (b == 1) ? -field[idx(N, i)] : field[idx(N, i)];
            field[idx(i, 0)]   = (b == 2) ? -field[idx(i, 1)] : field[idx(i, 1)];
            field[idx(i, N+1)] = (b == 2) ? -field[idx(i, N)] : field[idx(i, N)];
        }
    }

    // Corners
    field[idx(0, 0)] = 0.5f * (field[idx(1, 0)] + field[idx(0, 1)]);
    field[idx(0, N+1)] = 0.5f * (field[idx(1, N+1)] + field[idx(0, N)]);
    field[idx(N+1, 0)] = 0.5f * (field[idx(N, 0)] + field[idx(N+1, 1)]);
    field[idx(N+1, N+1)] = 0.5f * (field[idx(N, N+1)] + field[idx(N+1, N)]);
}

void FluidSolver::dissipate(std::vector<Real>& field, Real rate) {
    for (auto& v : field) {
        v *= rate;
    }
}

Real FluidSolver::bilinearSample(const std::vector<Real>& field, Real x, Real y) const {
    u32 i0 = static_cast<u32>(x);
    u32 j0 = static_cast<u32>(y);
    u32 i1 = i0 + 1;
    u32 j1 = j0 + 1;

    Real s1 = x - i0;
    Real s0 = 1.0f - s1;
    Real t1 = y - j0;
    Real t0 = 1.0f - t1;

    return s0 * (t0 * field[idx(i0, j0)] + t1 * field[idx(i0, j1)]) +
           s1 * (t0 * field[idx(i1, j0)] + t1 * field[idx(i1, j1)]);
}

// ============================================================================
// Input Methods
// ============================================================================

void FluidSolver::addDensity(Real x, Real y, Real amount, Real radius) {
    u32 N = m_config.resolution;
    u32 cx = static_cast<u32>(x * N) + 1;
    u32 cy = static_cast<u32>(y * N) + 1;
    u32 r = static_cast<u32>(radius);

    for (i32 dy = -static_cast<i32>(r); dy <= static_cast<i32>(r); ++dy) {
        for (i32 dx = -static_cast<i32>(r); dx <= static_cast<i32>(r); ++dx) {
            u32 px = cx + dx;
            u32 py = cy + dy;
            if (px >= 1 && px <= N && py >= 1 && py <= N) {
                Real dist = std::sqrt(static_cast<Real>(dx*dx + dy*dy));
                if (dist <= radius) {
                    Real falloff = 1.0f - dist / radius;
                    m_sourceDensity[idx(px, py)] += amount * falloff;
                }
            }
        }
    }
}

void FluidSolver::addVelocity(Real x, Real y, Real vx, Real vy, Real radius) {
    u32 N = m_config.resolution;
    u32 cx = static_cast<u32>(x * N) + 1;
    u32 cy = static_cast<u32>(y * N) + 1;
    u32 r = static_cast<u32>(radius);

    for (i32 dy = -static_cast<i32>(r); dy <= static_cast<i32>(r); ++dy) {
        for (i32 dx = -static_cast<i32>(r); dx <= static_cast<i32>(r); ++dx) {
            u32 px = cx + dx;
            u32 py = cy + dy;
            if (px >= 1 && px <= N && py >= 1 && py <= N) {
                Real dist = std::sqrt(static_cast<Real>(dx*dx + dy*dy));
                if (dist <= radius) {
                    Real falloff = 1.0f - dist / radius;
                    m_sourceVx[idx(px, py)] += vx * falloff;
                    m_sourceVy[idx(px, py)] += vy * falloff;
                }
            }
        }
    }
}

void FluidSolver::addForce(Real x, Real y, Real fx, Real fy, Real radius) {
    addVelocity(x, y, fx, fy, radius);
}

void FluidSolver::addColoredDensity(Real x, Real y, Real r, Real g, Real b, Real amount, Real radius) {
    u32 N = m_config.resolution;
    u32 cx = static_cast<u32>(x * N) + 1;
    u32 cy = static_cast<u32>(y * N) + 1;
    u32 rad = static_cast<u32>(radius);

    for (i32 dy = -static_cast<i32>(rad); dy <= static_cast<i32>(rad); ++dy) {
        for (i32 dx = -static_cast<i32>(rad); dx <= static_cast<i32>(rad); ++dx) {
            u32 px = cx + dx;
            u32 py = cy + dy;
            if (px >= 1 && px <= N && py >= 1 && py <= N) {
                Real dist = std::sqrt(static_cast<Real>(dx*dx + dy*dy));
                if (dist <= radius) {
                    Real falloff = 1.0f - dist / radius;
                    Real amt = amount * falloff;
                    m_sourceR[idx(px, py)] += r * amt;
                    m_sourceG[idx(px, py)] += g * amt;
                    m_sourceB[idx(px, py)] += b * amt;
                    m_sourceDensity[idx(px, py)] += amt;
                }
            }
        }
    }
}

void FluidSolver::applyImpulse(Real x, Real y, Real dx, Real dy, Real strength, Real radius) {
    addVelocity(x, y, dx * strength, dy * strength, radius);
    addDensity(x, y, strength * 0.5f, radius);
}

// ============================================================================
// Field Access
// ============================================================================

Real FluidSolver::densityAt(Real x, Real y) const {
    u32 N = m_config.resolution;
    Real gx = x * N + 1;
    Real gy = y * N + 1;
    return bilinearSample(m_density, gx, gy);
}

Vec2 FluidSolver::velocityAt(Real x, Real y) const {
    u32 N = m_config.resolution;
    Real gx = x * N + 1;
    Real gy = y * N + 1;
    return {
        bilinearSample(m_velocityX, gx, gy),
        bilinearSample(m_velocityY, gx, gy)
    };
}

Real FluidSolver::vorticityAt(Real x, Real y) const {
    u32 N = m_config.resolution;
    Real gx = x * N + 1;
    Real gy = y * N + 1;
    return bilinearSample(m_vorticity, gx, gy);
}

Real FluidSolver::pressureAt(Real x, Real y) const {
    u32 N = m_config.resolution;
    Real gx = x * N + 1;
    Real gy = y * N + 1;
    return bilinearSample(m_pressure, gx, gy);
}

// ============================================================================
// Rendering
// ============================================================================

void FluidSolver::renderDensity(u32* buffer) const {
    u32 N = m_config.resolution;

    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            Real d = std::min(m_density[idx(i, j)], 1.0f);
            u32 v = static_cast<u32>(d * 255);
            buffer[(j-1) * N + (i-1)] = (255 << 24) | (v << 16) | (v << 8) | v;
        }
    }
}

void FluidSolver::renderVelocity(u32* buffer) const {
    u32 N = m_config.resolution;

    Real maxV = 0.01f;
    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            Real vx = m_velocityX[idx(i, j)];
            Real vy = m_velocityY[idx(i, j)];
            Real mag = std::sqrt(vx*vx + vy*vy);
            maxV = std::max(maxV, mag);
        }
    }

    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            Real vx = m_velocityX[idx(i, j)];
            Real vy = m_velocityY[idx(i, j)];
            Real mag = std::sqrt(vx*vx + vy*vy) / maxV;
            Real angle = std::atan2(vy, vx);

            // HSV to RGB (hue = angle, saturation = 1, value = magnitude)
            Real h = (angle + constants::PI) / constants::TWO_PI * 360.0f;
            Real s = 1.0f;
            Real v = mag;

            Real c = v * s;
            Real x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
            Real m = v - c;

            Real r, g, b;
            if (h < 60) { r = c; g = x; b = 0; }
            else if (h < 120) { r = x; g = c; b = 0; }
            else if (h < 180) { r = 0; g = c; b = x; }
            else if (h < 240) { r = 0; g = x; b = c; }
            else if (h < 300) { r = x; g = 0; b = c; }
            else { r = c; g = 0; b = x; }

            u32 ri = static_cast<u32>((r + m) * 255);
            u32 gi = static_cast<u32>((g + m) * 255);
            u32 bi = static_cast<u32>((b + m) * 255);

            buffer[(j-1) * N + (i-1)] = (255 << 24) | (bi << 16) | (gi << 8) | ri;
        }
    }
}

void FluidSolver::renderVorticity(u32* buffer) const {
    u32 N = m_config.resolution;

    Real maxW = 0.01f;
    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            maxW = std::max(maxW, std::abs(m_vorticity[idx(i, j)]));
        }
    }

    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            Real w = m_vorticity[idx(i, j)] / maxW;

            // Blue = negative (clockwise), Red = positive (counter-clockwise)
            u32 r = w > 0 ? static_cast<u32>(w * 255) : 0;
            u32 b = w < 0 ? static_cast<u32>(-w * 255) : 0;

            buffer[(j-1) * N + (i-1)] = (255 << 24) | (b << 16) | (0 << 8) | r;
        }
    }
}

void FluidSolver::renderPressure(u32* buffer) const {
    u32 N = m_config.resolution;

    Real minP = 0, maxP = 0;
    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            Real p = m_pressure[idx(i, j)];
            minP = std::min(minP, p);
            maxP = std::max(maxP, p);
        }
    }
    Real range = std::max(maxP - minP, 0.001f);

    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            Real p = (m_pressure[idx(i, j)] - minP) / range;

            // Green-Yellow-Red gradient
            u32 r = static_cast<u32>(std::min(p * 2.0f, 1.0f) * 255);
            u32 g = static_cast<u32>(std::max(1.0f - (p - 0.5f) * 2.0f, 0.0f) * 255);

            buffer[(j-1) * N + (i-1)] = (255 << 24) | (0 << 16) | (g << 8) | r;
        }
    }
}

void FluidSolver::renderRainbow(u32* buffer) const {
    u32 N = m_config.resolution;

    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            Real rr = std::min(m_densityR[idx(i, j)], 1.0f);
            Real gg = std::min(m_densityG[idx(i, j)], 1.0f);
            Real bb = std::min(m_densityB[idx(i, j)], 1.0f);

            u32 ri = static_cast<u32>(rr * 255);
            u32 gi = static_cast<u32>(gg * 255);
            u32 bi = static_cast<u32>(bb * 255);

            buffer[(j-1) * N + (i-1)] = (255 << 24) | (bi << 16) | (gi << 8) | ri;
        }
    }
}

// ============================================================================
// Statistics
// ============================================================================

void FluidSolver::computeStatistics() {
    u32 N = m_config.resolution;

    m_stats.maxVelocity = 0;
    m_stats.averageVelocity = 0;
    m_stats.totalDensity = 0;
    m_stats.maxVorticity = 0;
    m_stats.kineticEnergy = 0;

    Real sumV = 0;
    u32 count = 0;

    for (u32 j = 1; j <= N; ++j) {
        for (u32 i = 1; i <= N; ++i) {
            Real vx = m_velocityX[idx(i, j)];
            Real vy = m_velocityY[idx(i, j)];
            Real mag = std::sqrt(vx*vx + vy*vy);

            m_stats.maxVelocity = std::max(m_stats.maxVelocity, mag);
            sumV += mag;
            m_stats.totalDensity += m_density[idx(i, j)];
            m_stats.maxVorticity = std::max(m_stats.maxVorticity, std::abs(m_vorticity[idx(i, j)]));
            m_stats.kineticEnergy += 0.5f * (vx*vx + vy*vy);
            count++;
        }
    }

    m_stats.averageVelocity = sumV / count;
}

// ============================================================================
// Presets
// ============================================================================

namespace fluid_presets {

FluidConfig smoke() {
    FluidConfig c;
    c.resolution = 128;
    c.viscosity = 0.0f;
    c.diffusion = 0.00001f;
    c.dissipation = 0.995f;
    c.densityDissipation = 0.97f;
    c.vorticityStrength = 0.3f;
    c.pressureIterations = 20;
    return c;
}

FluidConfig water() {
    FluidConfig c;
    c.resolution = 128;
    c.viscosity = 0.001f;
    c.diffusion = 0.0f;
    c.dissipation = 0.999f;
    c.densityDissipation = 0.999f;
    c.vorticityStrength = 0.1f;
    c.pressureIterations = 40;
    return c;
}

FluidConfig fire() {
    FluidConfig c;
    c.resolution = 128;
    c.viscosity = 0.0f;
    c.diffusion = 0.0001f;
    c.dissipation = 0.98f;
    c.densityDissipation = 0.92f;
    c.vorticityStrength = 0.8f;
    c.pressureIterations = 20;
    return c;
}

FluidConfig ink() {
    FluidConfig c;
    c.resolution = 128;
    c.viscosity = 0.0001f;
    c.diffusion = 0.0002f;
    c.dissipation = 0.998f;
    c.densityDissipation = 0.995f;
    c.vorticityStrength = 0.2f;
    c.pressureIterations = 30;
    return c;
}

FluidConfig turbulent() {
    FluidConfig c;
    c.resolution = 128;
    c.viscosity = 0.0f;
    c.diffusion = 0.0f;
    c.dissipation = 0.999f;
    c.densityDissipation = 0.99f;
    c.vorticityStrength = 1.5f;
    c.pressureIterations = 20;
    return c;
}

} // namespace fluid_presets

} // namespace physics
} // namespace eigenlab
