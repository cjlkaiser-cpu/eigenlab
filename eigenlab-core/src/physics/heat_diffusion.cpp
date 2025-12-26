/**
 * @file heat_diffusion.cpp
 * @brief High-performance heat diffusion solver implementation
 */

#include "../../include/physics/heat_diffusion.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace eigenlab {
namespace physics {

// ============================================================================
// Colormap utilities
// ============================================================================

namespace colormap {

Color thermal(Real t) {
    t = std::clamp(t, 0.0f, 1.0f);

    // Blue -> Cyan -> Green -> Yellow -> Red
    u32 r, g, b;
    if (t < 0.25f) {
        Real s = t / 0.25f;
        r = 0;
        g = static_cast<u32>(255 * s);
        b = 255;
    } else if (t < 0.5f) {
        Real s = (t - 0.25f) / 0.25f;
        r = 0;
        g = 255;
        b = static_cast<u32>(255 * (1.0f - s));
    } else if (t < 0.75f) {
        Real s = (t - 0.5f) / 0.25f;
        r = static_cast<u32>(255 * s);
        g = 255;
        b = 0;
    } else {
        Real s = (t - 0.75f) / 0.25f;
        r = 255;
        g = static_cast<u32>(255 * (1.0f - s));
        b = 0;
    }

    return Color(r, g, b);
}

Color inferno(Real t) {
    t = std::clamp(t, 0.0f, 1.0f);

    // Approximation of matplotlib's inferno colormap
    Real r = std::clamp(1.0f * std::pow(t, 0.5f) + 0.3f * t, 0.0f, 1.0f);
    Real g = std::clamp(0.8f * std::pow(t, 2.0f), 0.0f, 1.0f);
    Real b = std::clamp(0.3f * std::sin(t * constants::PI) + 0.2f * (1.0f - t), 0.0f, 1.0f);

    return Color(
        static_cast<u32>(r * 255),
        static_cast<u32>(g * 255),
        static_cast<u32>(b * 255)
    );
}

Color viridis(Real t) {
    t = std::clamp(t, 0.0f, 1.0f);

    // Simplified viridis approximation
    Real r = std::clamp(0.27f + 0.73f * t - 0.5f * std::sin(t * constants::PI), 0.0f, 1.0f);
    Real g = std::clamp(0.0f + 0.9f * t, 0.0f, 1.0f);
    Real b = std::clamp(0.33f + 0.3f * std::sin(t * constants::PI * 0.5f), 0.0f, 1.0f);

    return Color(
        static_cast<u32>(r * 255),
        static_cast<u32>(g * 255),
        static_cast<u32>(b * 255)
    );
}

Color grayscale(Real t) {
    t = std::clamp(t, 0.0f, 1.0f);
    u32 v = static_cast<u32>(t * 255);
    return Color(v, v, v);
}

Color temperatureToColor(Real temp, Real minTemp, Real maxTemp) {
    Real t = (temp - minTemp) / (maxTemp - minTemp);
    return thermal(t);
}

} // namespace colormap

// ============================================================================
// Heat Diffusion Implementation
// ============================================================================

HeatDiffusion::HeatDiffusion() {
    setConfig(HeatGridConfig{});
}

HeatDiffusion::HeatDiffusion(const HeatGridConfig& config) {
    setConfig(config);
}

void HeatDiffusion::setConfig(const HeatGridConfig& config) {
    m_config = config;
    allocateBuffers();
    reset();
}

void HeatDiffusion::resize(u32 resX, u32 resY) {
    m_config.resolutionX = resX;
    m_config.resolutionY = resY;
    allocateBuffers();
    reset();
}

void HeatDiffusion::allocateBuffers() {
    usize size = static_cast<usize>(m_config.resolutionX) * m_config.resolutionY;

    m_temperature.resize(size);
    m_temperatureNew.resize(size);
    m_velocityX.resize(size);
    m_velocityY.resize(size);

    // Calculate cell sizes
    m_dx = m_config.width / m_config.resolutionX;
    m_dy = m_config.height / m_config.resolutionY;
    m_invDx = 1.0f / m_dx;
    m_invDy = 1.0f / m_dy;
    m_invDx2 = 1.0f / (m_dx * m_dx);
    m_invDy2 = 1.0f / (m_dy * m_dy);
}

void HeatDiffusion::reset() {
    std::fill(m_temperature.begin(), m_temperature.end(), m_config.ambientTemperature);
    std::fill(m_temperatureNew.begin(), m_temperatureNew.end(), m_config.ambientTemperature);
    std::fill(m_velocityX.begin(), m_velocityX.end(), 0.0f);
    std::fill(m_velocityY.begin(), m_velocityY.end(), 0.0f);

    m_stats = HeatGridStats{};
}

void HeatDiffusion::setUniform(Real temperature) {
    std::fill(m_temperature.begin(), m_temperature.end(), temperature);
    std::fill(m_temperatureNew.begin(), m_temperatureNew.end(), temperature);
}

void HeatDiffusion::setTemperatureAt(const Vec2& pos, Real temp, Real radius) {
    Vec2 gridPos = worldToGrid(pos);
    i32 cx = static_cast<i32>(gridPos.x);
    i32 cy = static_cast<i32>(gridPos.y);
    i32 r = static_cast<i32>(radius / m_dx);

    i32 nx = static_cast<i32>(m_config.resolutionX);
    i32 ny = static_cast<i32>(m_config.resolutionY);

    for (i32 dy = -r; dy <= r; ++dy) {
        for (i32 dx = -r; dx <= r; ++dx) {
            i32 x = cx + dx;
            i32 y = cy + dy;

            if (x >= 0 && x < nx && y >= 0 && y < ny) {
                Real dist = std::sqrt(static_cast<Real>(dx * dx + dy * dy));
                if (dist <= r) {
                    m_temperature[idx(x, y)] = temp;
                    m_temperatureNew[idx(x, y)] = temp;
                }
            }
        }
    }
}

void HeatDiffusion::addGaussianHeat(const Vec2& center, Real amplitude, Real sigma) {
    Vec2 gridCenter = worldToGrid(center);
    Real gridSigma = sigma / m_dx;

    for (u32 y = 0; y < m_config.resolutionY; ++y) {
        for (u32 x = 0; x < m_config.resolutionX; ++x) {
            Real dx = x - gridCenter.x;
            Real dy = y - gridCenter.y;
            Real distSq = dx * dx + dy * dy;
            Real gaussian = amplitude * std::exp(-distSq / (2.0f * gridSigma * gridSigma));
            m_temperature[idx(x, y)] += gaussian;
        }
    }
}

void HeatDiffusion::addSource(const HeatSource& source) {
    m_sources.push_back(source);
    applySources();
}

void HeatDiffusion::removeSource(usize index) {
    if (index < m_sources.size()) {
        m_sources.erase(m_sources.begin() + index);
    }
}

void HeatDiffusion::clearSources() {
    m_sources.clear();
}

void HeatDiffusion::setSourceTemperature(usize index, Real temp) {
    if (index < m_sources.size()) {
        m_sources[index].temperature = temp;
    }
}

void HeatDiffusion::setSourcePosition(usize index, const Vec2& pos) {
    if (index < m_sources.size()) {
        m_sources[index].position = pos;
    }
}

void HeatDiffusion::applySources() {
    for (const auto& src : m_sources) {
        if (!src.isActive) continue;

        Vec2 gridPos = worldToGrid(src.position);
        i32 cx = static_cast<i32>(gridPos.x);
        i32 cy = static_cast<i32>(gridPos.y);
        i32 r = static_cast<i32>(src.radius / m_dx);

        i32 nx = static_cast<i32>(m_config.resolutionX);
        i32 ny = static_cast<i32>(m_config.resolutionY);

        for (i32 dy = -r; dy <= r; ++dy) {
            for (i32 dx = -r; dx <= r; ++dx) {
                i32 x = cx + dx;
                i32 y = cy + dy;

                if (x >= 0 && x < nx && y >= 0 && y < ny) {
                    bool inside = false;

                    if (src.shape == HeatSource::Shape::Circle) {
                        Real dist = std::sqrt(static_cast<Real>(dx * dx + dy * dy));
                        inside = dist <= r;
                    } else {
                        inside = std::abs(dx) <= r && std::abs(dy) <= r;
                    }

                    if (inside) {
                        m_temperature[idx(x, y)] = src.temperature;
                        m_temperatureNew[idx(x, y)] = src.temperature;
                    }
                }
            }
        }
    }
}

Real HeatDiffusion::computeStableDt() const {
    // CFL condition for explicit finite differences: dt <= dx² / (4α)
    Real dxMin = std::min(m_dx, m_dy);
    return 0.25f * dxMin * dxMin / m_config.thermalDiffusivity;
}

void HeatDiffusion::step(Real dt) {
    if (m_mode == Mode::Conduction) {
        stepConduction(dt);
    } else {
        stepConvection(dt);
    }

    // Swap buffers
    std::swap(m_temperature, m_temperatureNew);

    // Reapply sources (fixed boundary conditions)
    applySources();

    m_stats.iterations++;
    m_stats.simulationTime += dt;
}

void HeatDiffusion::stepMultiple(Real dt, u32 steps) {
    for (u32 i = 0; i < steps; ++i) {
        step(dt);
    }
}

void HeatDiffusion::stepConduction(Real dt) {
    // Pure diffusion: ∂T/∂t = α∇²T
    // Explicit finite difference scheme

    u32 nx = m_config.resolutionX;
    u32 ny = m_config.resolutionY;
    Real alpha = m_config.thermalDiffusivity;

    // Interior points
    for (u32 y = 1; y < ny - 1; ++y) {
        for (u32 x = 1; x < nx - 1; ++x) {
            Real T_center = m_temperature[idx(x, y)];
            Real T_left = m_temperature[idx(x - 1, y)];
            Real T_right = m_temperature[idx(x + 1, y)];
            Real T_up = m_temperature[idx(x, y - 1)];
            Real T_down = m_temperature[idx(x, y + 1)];

            // Laplacian with 5-point stencil
            Real laplacian = (T_left + T_right - 2.0f * T_center) * m_invDx2 +
                            (T_up + T_down - 2.0f * T_center) * m_invDy2;

            m_temperatureNew[idx(x, y)] = T_center + alpha * dt * laplacian;
        }
    }

    applyBoundaryConditions();
}

void HeatDiffusion::stepConvection(Real dt) {
    // Compute velocity field first
    computeVelocityField();

    u32 nx = m_config.resolutionX;
    u32 ny = m_config.resolutionY;
    Real alpha = m_config.thermalDiffusivity;

    // Advection-Diffusion: ∂T/∂t + v·∇T = α∇²T
    for (u32 y = 1; y < ny - 1; ++y) {
        for (u32 x = 1; x < nx - 1; ++x) {
            Real T_center = m_temperature[idx(x, y)];
            Real T_left = m_temperature[idx(x - 1, y)];
            Real T_right = m_temperature[idx(x + 1, y)];
            Real T_up = m_temperature[idx(x, y - 1)];
            Real T_down = m_temperature[idx(x, y + 1)];

            // Laplacian
            Real laplacian = (T_left + T_right - 2.0f * T_center) * m_invDx2 +
                            (T_up + T_down - 2.0f * T_center) * m_invDy2;

            // Gradients (central differences)
            Real dTdx = (T_right - T_left) * 0.5f * m_invDx;
            Real dTdy = (T_down - T_up) * 0.5f * m_invDy;

            // Velocity at this point
            Real vx = m_velocityX[idx(x, y)];
            Real vy = m_velocityY[idx(x, y)];

            // Advection term: -v·∇T
            Real advection = -(vx * dTdx + vy * dTdy);

            m_temperatureNew[idx(x, y)] = T_center + dt * (alpha * laplacian + advection);
        }
    }

    applyBoundaryConditions();
}

void HeatDiffusion::computeVelocityField() {
    // Simple buoyancy-driven convection: hot air rises
    // v_y proportional to -∂T/∂y (negative because y increases downward in screen coords)
    // v_x based on continuity/circulation

    u32 nx = m_config.resolutionX;
    u32 ny = m_config.resolutionY;
    Real strength = m_config.convectionStrength;
    Real buoyancy = m_config.buoyancyFactor;

    for (u32 y = 1; y < ny - 1; ++y) {
        for (u32 x = 1; x < nx - 1; ++x) {
            // Temperature gradients
            Real dTdy = (m_temperature[idx(x, y + 1)] - m_temperature[idx(x, y - 1)]) * 0.5f * m_invDy;
            Real dTdx = (m_temperature[idx(x + 1, y)] - m_temperature[idx(x - 1, y)]) * 0.5f * m_invDx;

            // Vertical velocity (hot rises, cold sinks)
            // Negative dTdy because screen coords have y increasing downward
            m_velocityY[idx(x, y)] = -dTdy * strength * buoyancy;

            // Horizontal velocity (circulation)
            m_velocityX[idx(x, y)] = dTdx * strength * buoyancy * 0.4f;
        }
    }

    // Zero velocity at boundaries
    for (u32 y = 0; y < ny; ++y) {
        m_velocityX[idx(0, y)] = 0;
        m_velocityX[idx(nx - 1, y)] = 0;
        m_velocityY[idx(0, y)] = 0;
        m_velocityY[idx(nx - 1, y)] = 0;
    }
    for (u32 x = 0; x < nx; ++x) {
        m_velocityX[idx(x, 0)] = 0;
        m_velocityX[idx(x, ny - 1)] = 0;
        m_velocityY[idx(x, 0)] = 0;
        m_velocityY[idx(x, ny - 1)] = 0;
    }
}

void HeatDiffusion::applyBoundaryConditions() {
    u32 nx = m_config.resolutionX;
    u32 ny = m_config.resolutionY;

    // Left boundary (x = 0)
    for (u32 y = 0; y < ny; ++y) {
        switch (m_config.boundaryLeft) {
            case BoundaryCondition::Adiabatic:
                m_temperatureNew[idx(0, y)] = m_temperatureNew[idx(1, y)];
                break;
            case BoundaryCondition::Dirichlet:
                m_temperatureNew[idx(0, y)] = m_config.boundaryTempLeft;
                break;
            case BoundaryCondition::Neumann:
                // Implement if needed
                m_temperatureNew[idx(0, y)] = m_temperatureNew[idx(1, y)];
                break;
            case BoundaryCondition::Periodic:
                m_temperatureNew[idx(0, y)] = m_temperatureNew[idx(nx - 2, y)];
                break;
        }
    }

    // Right boundary (x = nx-1)
    for (u32 y = 0; y < ny; ++y) {
        switch (m_config.boundaryRight) {
            case BoundaryCondition::Adiabatic:
                m_temperatureNew[idx(nx - 1, y)] = m_temperatureNew[idx(nx - 2, y)];
                break;
            case BoundaryCondition::Dirichlet:
                m_temperatureNew[idx(nx - 1, y)] = m_config.boundaryTempRight;
                break;
            case BoundaryCondition::Neumann:
                m_temperatureNew[idx(nx - 1, y)] = m_temperatureNew[idx(nx - 2, y)];
                break;
            case BoundaryCondition::Periodic:
                m_temperatureNew[idx(nx - 1, y)] = m_temperatureNew[idx(1, y)];
                break;
        }
    }

    // Top boundary (y = 0)
    for (u32 x = 0; x < nx; ++x) {
        switch (m_config.boundaryTop) {
            case BoundaryCondition::Adiabatic:
                m_temperatureNew[idx(x, 0)] = m_temperatureNew[idx(x, 1)];
                break;
            case BoundaryCondition::Dirichlet:
                m_temperatureNew[idx(x, 0)] = m_config.boundaryTempTop;
                break;
            case BoundaryCondition::Neumann:
                m_temperatureNew[idx(x, 0)] = m_temperatureNew[idx(x, 1)];
                break;
            case BoundaryCondition::Periodic:
                m_temperatureNew[idx(x, 0)] = m_temperatureNew[idx(x, ny - 2)];
                break;
        }
    }

    // Bottom boundary (y = ny-1)
    for (u32 x = 0; x < nx; ++x) {
        switch (m_config.boundaryBottom) {
            case BoundaryCondition::Adiabatic:
                m_temperatureNew[idx(x, ny - 1)] = m_temperatureNew[idx(x, ny - 2)];
                break;
            case BoundaryCondition::Dirichlet:
                m_temperatureNew[idx(x, ny - 1)] = m_config.boundaryTempBottom;
                break;
            case BoundaryCondition::Neumann:
                m_temperatureNew[idx(x, ny - 1)] = m_temperatureNew[idx(x, ny - 2)];
                break;
            case BoundaryCondition::Periodic:
                m_temperatureNew[idx(x, ny - 1)] = m_temperatureNew[idx(x, 1)];
                break;
        }
    }
}

Real HeatDiffusion::temperatureAt(u32 x, u32 y) const {
    if (x >= m_config.resolutionX || y >= m_config.resolutionY) {
        return m_config.ambientTemperature;
    }
    return m_temperature[idx(x, y)];
}

Real HeatDiffusion::temperatureAt(const Vec2& worldPos) const {
    Vec2 gridPos = worldToGrid(worldPos);
    u32 x = static_cast<u32>(std::clamp(gridPos.x, 0.0f, static_cast<Real>(m_config.resolutionX - 1)));
    u32 y = static_cast<u32>(std::clamp(gridPos.y, 0.0f, static_cast<Real>(m_config.resolutionY - 1)));
    return temperatureAt(x, y);
}

void HeatDiffusion::setTemperatureAt(u32 x, u32 y, Real temp) {
    if (x < m_config.resolutionX && y < m_config.resolutionY) {
        m_temperature[idx(x, y)] = temp;
        m_temperatureNew[idx(x, y)] = temp;
    }
}

Vec2 HeatDiffusion::velocityAt(u32 x, u32 y) const {
    if (x >= m_config.resolutionX || y >= m_config.resolutionY) {
        return Vec2::zero();
    }
    return {m_velocityX[idx(x, y)], m_velocityY[idx(x, y)]};
}

Vec2 HeatDiffusion::velocityAt(const Vec2& worldPos) const {
    Vec2 gridPos = worldToGrid(worldPos);
    u32 x = static_cast<u32>(std::clamp(gridPos.x, 0.0f, static_cast<Real>(m_config.resolutionX - 1)));
    u32 y = static_cast<u32>(std::clamp(gridPos.y, 0.0f, static_cast<Real>(m_config.resolutionY - 1)));
    return velocityAt(x, y);
}

Vec2 HeatDiffusion::heatFluxAt(u32 x, u32 y) const {
    if (x == 0 || x >= m_config.resolutionX - 1 ||
        y == 0 || y >= m_config.resolutionY - 1) {
        return Vec2::zero();
    }

    // Heat flux q = -k * ∇T (Fourier's law)
    // Here we just return the gradient
    Real dTdx = (m_temperature[idx(x + 1, y)] - m_temperature[idx(x - 1, y)]) * 0.5f * m_invDx;
    Real dTdy = (m_temperature[idx(x, y + 1)] - m_temperature[idx(x, y - 1)]) * 0.5f * m_invDy;

    // Return negative gradient (heat flows from hot to cold)
    return {-dTdx, -dTdy};
}

void HeatDiffusion::computeStatistics() {
    Real sum = 0.0f;
    Real minT = constants::INF;
    Real maxT = -constants::INF;
    Real fluxSum = 0.0f;

    u32 nx = m_config.resolutionX;
    u32 ny = m_config.resolutionY;

    for (u32 y = 0; y < ny; ++y) {
        for (u32 x = 0; x < nx; ++x) {
            Real T = m_temperature[idx(x, y)];
            sum += T;
            minT = std::min(minT, T);
            maxT = std::max(maxT, T);

            if (x > 0 && x < nx - 1 && y > 0 && y < ny - 1) {
                Vec2 flux = heatFluxAt(x, y);
                fluxSum += flux.length();
            }
        }
    }

    usize count = m_temperature.size();
    m_stats.averageTemperature = sum / count;
    m_stats.minTemperature = minT;
    m_stats.maxTemperature = maxT;
    m_stats.totalHeat = sum;
    m_stats.heatFluxMagnitude = fluxSum / ((nx - 2) * (ny - 2));
    m_stats.dtCFL = computeStableDt();
}

void HeatDiffusion::renderToBuffer(u32* buffer, Real minTemp, Real maxTemp) const {
    u32 nx = m_config.resolutionX;
    u32 ny = m_config.resolutionY;
    Real range = maxTemp - minTemp;

    if (range < constants::EPSILON) {
        range = 1.0f;
    }

    for (u32 y = 0; y < ny; ++y) {
        for (u32 x = 0; x < nx; ++x) {
            Real T = m_temperature[idx(x, y)];
            Real t = (T - minTemp) / range;
            Color c = colormap::thermal(t);
            buffer[y * nx + x] = c.value;
        }
    }
}

Vec2 HeatDiffusion::worldToGrid(const Vec2& world) const {
    return {
        world.x / m_dx,
        world.y / m_dy
    };
}

Vec2 HeatDiffusion::gridToWorld(u32 x, u32 y) const {
    return {
        (x + 0.5f) * m_dx,
        (y + 0.5f) * m_dy
    };
}

} // namespace physics
} // namespace eigenlab
