/**
 * @file heat_diffusion.hpp
 * @brief High-performance heat diffusion solver using finite differences
 * @version 1.0.0
 *
 * Features:
 * - Explicit and implicit solvers
 * - Conduction and convection modes
 * - Heat sources/sinks with arbitrary shapes
 * - Adiabatic, Dirichlet, and Neumann boundary conditions
 * - Optimized memory layout for cache efficiency
 * - SIMD-friendly data structures
 */

#pragma once

#include "../core/types.hpp"
#include <vector>
#include <functional>

namespace eigenlab {
namespace physics {

// ============================================================================
// Boundary conditions
// ============================================================================

enum class BoundaryCondition {
    Adiabatic,    // ∂T/∂n = 0 (no heat flux)
    Dirichlet,    // T = T_boundary (fixed temperature)
    Neumann,      // ∂T/∂n = q (fixed flux)
    Periodic      // Wrap around
};

// ============================================================================
// Heat source/sink
// ============================================================================

struct HeatSource {
    Vec2 position{0.0f, 0.0f};
    Real radius{20.0f};
    Real temperature{100.0f};
    bool isActive{true};

    enum class Type { Hot, Cold } type{Type::Hot};
    enum class Shape { Circle, Square } shape{Shape::Circle};
};

// ============================================================================
// Grid configuration
// ============================================================================

struct HeatGridConfig {
    // Domain size in world units
    Real width{500.0f};
    Real height{500.0f};

    // Grid resolution
    u32 resolutionX{100};
    u32 resolutionY{100};

    // Physics
    Real thermalDiffusivity{0.25f};  // α in ∂T/∂t = α∇²T
    Real ambientTemperature{20.0f};

    // Convection parameters
    Real convectionStrength{1.0f};
    Real buoyancyFactor{0.05f};      // How much temperature affects vertical velocity

    // Boundary conditions
    BoundaryCondition boundaryTop{BoundaryCondition::Adiabatic};
    BoundaryCondition boundaryBottom{BoundaryCondition::Adiabatic};
    BoundaryCondition boundaryLeft{BoundaryCondition::Adiabatic};
    BoundaryCondition boundaryRight{BoundaryCondition::Adiabatic};

    // Boundary temperatures (for Dirichlet conditions)
    Real boundaryTempTop{20.0f};
    Real boundaryTempBottom{20.0f};
    Real boundaryTempLeft{20.0f};
    Real boundaryTempRight{20.0f};
};

// ============================================================================
// Statistics
// ============================================================================

struct HeatGridStats {
    Real averageTemperature{0.0f};
    Real minTemperature{0.0f};
    Real maxTemperature{0.0f};
    Real totalHeat{0.0f};           // Integrated temperature
    Real heatFluxMagnitude{0.0f};   // Average |∇T|

    // For validation
    Real dtCFL{0.0f};               // Timestep for CFL stability
    u32 iterations{0};
    Real simulationTime{0.0f};
};

// ============================================================================
// Heat Diffusion Solver
// ============================================================================

class HeatDiffusion {
public:
    enum class Mode {
        Conduction,    // Pure diffusion: ∂T/∂t = α∇²T
        Convection     // With advection: ∂T/∂t + v·∇T = α∇²T
    };

    HeatDiffusion();
    explicit HeatDiffusion(const HeatGridConfig& config);

    // Configuration
    void setConfig(const HeatGridConfig& config);
    void resize(u32 resX, u32 resY);
    [[nodiscard]] const HeatGridConfig& config() const { return m_config; }

    // Mode
    void setMode(Mode mode) { m_mode = mode; }
    [[nodiscard]] Mode mode() const { return m_mode; }

    // Heat sources
    void addSource(const HeatSource& source);
    void removeSource(usize index);
    void clearSources();
    void setSourceTemperature(usize index, Real temp);
    void setSourcePosition(usize index, const Vec2& pos);
    [[nodiscard]] const std::vector<HeatSource>& sources() const { return m_sources; }

    // Initial conditions
    void reset();                              // Reset to ambient
    void setUniform(Real temperature);         // Set all cells to T
    void setTemperatureAt(const Vec2& pos, Real temp, Real radius);
    void addGaussianHeat(const Vec2& center, Real amplitude, Real sigma);

    // Simulation
    void step(Real dt);
    void stepMultiple(Real dt, u32 steps);

    // Adaptive timestep based on CFL condition
    [[nodiscard]] Real computeStableDt() const;

    // Grid access
    [[nodiscard]] Real temperatureAt(u32 x, u32 y) const;
    [[nodiscard]] Real temperatureAt(const Vec2& worldPos) const;
    void setTemperatureAt(u32 x, u32 y, Real temp);

    // Velocity field (convection mode)
    [[nodiscard]] Vec2 velocityAt(u32 x, u32 y) const;
    [[nodiscard]] Vec2 velocityAt(const Vec2& worldPos) const;

    // Heat flux (gradient)
    [[nodiscard]] Vec2 heatFluxAt(u32 x, u32 y) const;

    // Statistics
    void computeStatistics();
    [[nodiscard]] const HeatGridStats& stats() const { return m_stats; }

    // Raw data access for rendering
    [[nodiscard]] const Real* temperatureData() const { return m_temperature.data(); }
    [[nodiscard]] const Real* velocityXData() const { return m_velocityX.data(); }
    [[nodiscard]] const Real* velocityYData() const { return m_velocityY.data(); }
    [[nodiscard]] u32 width() const { return m_config.resolutionX; }
    [[nodiscard]] u32 height() const { return m_config.resolutionY; }
    [[nodiscard]] usize cellCount() const { return m_temperature.size(); }

    // Render to RGBA buffer (for canvas ImageData)
    void renderToBuffer(u32* buffer, Real minTemp, Real maxTemp) const;

    // World/Grid coordinate conversion
    [[nodiscard]] Vec2 worldToGrid(const Vec2& world) const;
    [[nodiscard]] Vec2 gridToWorld(u32 x, u32 y) const;

private:
    void allocateBuffers();
    void applySources();
    void applyBoundaryConditions();
    void computeVelocityField();
    void stepConduction(Real dt);
    void stepConvection(Real dt);

    [[nodiscard]] usize idx(u32 x, u32 y) const {
        return static_cast<usize>(y) * m_config.resolutionX + x;
    }

    HeatGridConfig m_config;
    Mode m_mode{Mode::Conduction};

    // Double-buffered temperature grids
    std::vector<Real> m_temperature;
    std::vector<Real> m_temperatureNew;

    // Velocity field for convection
    std::vector<Real> m_velocityX;
    std::vector<Real> m_velocityY;

    // Heat sources
    std::vector<HeatSource> m_sources;

    // Cached values
    Real m_dx{1.0f};  // Cell size X
    Real m_dy{1.0f};  // Cell size Y
    Real m_invDx{1.0f};
    Real m_invDy{1.0f};
    Real m_invDx2{1.0f};
    Real m_invDy2{1.0f};

    HeatGridStats m_stats;
};

// ============================================================================
// Color mapping utilities
// ============================================================================

namespace colormap {
    // Standard thermal colormap (blue -> cyan -> green -> yellow -> red)
    Color thermal(Real t);  // t in [0, 1]

    // Inferno colormap (black -> purple -> red -> yellow -> white)
    Color inferno(Real t);

    // Viridis colormap (dark purple -> blue -> green -> yellow)
    Color viridis(Real t);

    // Grayscale
    Color grayscale(Real t);

    // Map temperature to color with custom range
    Color temperatureToColor(Real temp, Real minTemp, Real maxTemp);
}

} // namespace physics
} // namespace eigenlab
