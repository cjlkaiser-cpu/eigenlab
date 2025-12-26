/**
 * @file fluid_solver.hpp
 * @brief Real-time 2D fluid simulation using Stable Fluids method (Jos Stam, 1999)
 * @version 1.0.0
 *
 * Implements the incompressible Navier-Stokes equations:
 *   ∂u/∂t = -(u·∇)u - (1/ρ)∇p + ν∇²u + f
 *   ∇·u = 0
 *
 * Features:
 * - Semi-Lagrangian advection for stability at any timestep
 * - Gauss-Seidel iterative solver for pressure projection
 * - Vorticity confinement for swirling details
 * - Interactive force and density injection
 * - Multiple visualization modes (density, velocity, vorticity, pressure)
 */

#pragma once

#include "../core/types.hpp"
#include <vector>
#include <cmath>

namespace eigenlab {
namespace physics {

// ============================================================================
// Fluid configuration
// ============================================================================

struct FluidConfig {
    u32 resolution{128};           // Grid size (NxN)
    Real viscosity{0.0001f};       // Kinematic viscosity (ν)
    Real diffusion{0.0001f};       // Density diffusion rate
    Real dissipation{0.99f};       // Velocity dissipation (1 = no loss)
    Real densityDissipation{0.98f};// Density fade rate
    Real vorticityStrength{0.5f};  // Vorticity confinement strength
    u32 pressureIterations{20};    // Gauss-Seidel iterations
    u32 diffuseIterations{20};     // Diffusion iterations
    bool enableVorticity{true};    // Enable vorticity confinement
    bool boundaryWalls{true};      // Solid walls or open boundaries
};

// ============================================================================
// Visualization modes
// ============================================================================

enum class FluidVisMode {
    Density,       // Color-mapped density field
    Velocity,      // Velocity magnitude
    Vorticity,     // Curl of velocity (rotation)
    Pressure,      // Pressure field
    Streamlines,   // Flow lines
    Rainbow        // Density with rainbow colors based on direction
};

// ============================================================================
// Fluid statistics
// ============================================================================

struct FluidStats {
    Real maxVelocity{0.0f};
    Real averageVelocity{0.0f};
    Real totalDensity{0.0f};
    Real maxVorticity{0.0f};
    Real kineticEnergy{0.0f};
    u32 iterations{0};
    Real simulationTime{0.0f};
};

// ============================================================================
// 2D Fluid Solver
// ============================================================================

class FluidSolver {
public:
    FluidSolver();
    explicit FluidSolver(const FluidConfig& config);
    ~FluidSolver() = default;

    // Configuration
    void setConfig(const FluidConfig& config);
    void resize(u32 resolution);
    [[nodiscard]] const FluidConfig& config() const { return m_config; }

    // Reset
    void clear();
    void clearVelocity();
    void clearDensity();

    // Simulation step
    void step(Real dt);

    // Force and density injection
    void addDensity(Real x, Real y, Real amount, Real radius = 1.0f);
    void addVelocity(Real x, Real y, Real vx, Real vy, Real radius = 1.0f);
    void addForce(Real x, Real y, Real fx, Real fy, Real radius = 1.0f);

    // Convenience: add density with color
    void addColoredDensity(Real x, Real y, Real r, Real g, Real b, Real amount, Real radius = 1.0f);

    // Impulse from mouse drag
    void applyImpulse(Real x, Real y, Real dx, Real dy, Real strength = 100.0f, Real radius = 10.0f);

    // Grid access (normalized coordinates 0-1)
    [[nodiscard]] Real densityAt(Real x, Real y) const;
    [[nodiscard]] Vec2 velocityAt(Real x, Real y) const;
    [[nodiscard]] Real vorticityAt(Real x, Real y) const;
    [[nodiscard]] Real pressureAt(Real x, Real y) const;

    // Raw data access
    [[nodiscard]] const Real* densityData() const { return m_density.data(); }
    [[nodiscard]] const Real* velocityXData() const { return m_velocityX.data(); }
    [[nodiscard]] const Real* velocityYData() const { return m_velocityY.data(); }
    [[nodiscard]] const Real* vorticityData() const { return m_vorticity.data(); }
    [[nodiscard]] const Real* pressureData() const { return m_pressure.data(); }

    // RGB density (for colored smoke)
    [[nodiscard]] const Real* densityRData() const { return m_densityR.data(); }
    [[nodiscard]] const Real* densityGData() const { return m_densityG.data(); }
    [[nodiscard]] const Real* densityBData() const { return m_densityB.data(); }

    // Rendering
    void renderDensity(u32* buffer) const;
    void renderVelocity(u32* buffer) const;
    void renderVorticity(u32* buffer) const;
    void renderPressure(u32* buffer) const;
    void renderRainbow(u32* buffer) const;

    // Statistics
    void computeStatistics();
    [[nodiscard]] const FluidStats& stats() const { return m_stats; }

    [[nodiscard]] u32 resolution() const { return m_config.resolution; }
    [[nodiscard]] usize cellCount() const { return m_size; }

private:
    // Core solver steps
    void addSource(std::vector<Real>& field, const std::vector<Real>& source, Real dt);
    void diffuse(std::vector<Real>& field, const std::vector<Real>& field0, Real diff, Real dt);
    void advect(std::vector<Real>& field, const std::vector<Real>& field0,
                const std::vector<Real>& vx, const std::vector<Real>& vy, Real dt);
    void project();
    void applyVorticityConfinement(Real dt);
    void setBoundary(std::vector<Real>& field, int b);
    void dissipate(std::vector<Real>& field, Real rate);

    // Helper
    [[nodiscard]] usize idx(u32 x, u32 y) const {
        return static_cast<usize>(y) * (m_config.resolution + 2) + x;
    }

    [[nodiscard]] Real bilinearSample(const std::vector<Real>& field, Real x, Real y) const;

    void allocateBuffers();

    FluidConfig m_config;
    usize m_size{0};  // Total cells including boundaries

    // Velocity field (staggered grid with boundaries)
    std::vector<Real> m_velocityX;
    std::vector<Real> m_velocityY;
    std::vector<Real> m_velocityX0;  // Previous/temp
    std::vector<Real> m_velocityY0;

    // Scalar fields
    std::vector<Real> m_density;
    std::vector<Real> m_density0;
    std::vector<Real> m_pressure;
    std::vector<Real> m_divergence;
    std::vector<Real> m_vorticity;

    // RGB density for colored smoke
    std::vector<Real> m_densityR;
    std::vector<Real> m_densityG;
    std::vector<Real> m_densityB;
    std::vector<Real> m_densityR0;
    std::vector<Real> m_densityG0;
    std::vector<Real> m_densityB0;

    // Source buffers (for accumulating input)
    std::vector<Real> m_sourceVx;
    std::vector<Real> m_sourceVy;
    std::vector<Real> m_sourceDensity;
    std::vector<Real> m_sourceR;
    std::vector<Real> m_sourceG;
    std::vector<Real> m_sourceB;

    FluidStats m_stats;
};

// ============================================================================
// Preset configurations
// ============================================================================

namespace fluid_presets {
    FluidConfig smoke();      // Light, billowy smoke
    FluidConfig water();      // Dense, slower fluid
    FluidConfig fire();       // Rising, dissipating
    FluidConfig ink();        // Diffusive, artistic
    FluidConfig turbulent();  // High vorticity
}

} // namespace physics
} // namespace eigenlab
