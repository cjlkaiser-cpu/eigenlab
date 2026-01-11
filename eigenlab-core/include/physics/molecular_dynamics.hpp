#pragma once

#include <vector>
#include <cmath>
#include <random>

namespace eigenlab {
namespace physics {

/**
 * MolecularDynamics - Lennard-Jones molecular dynamics simulation
 *
 * Potential: V(r) = 4ε[(σ/r)¹² - (σ/r)⁶]
 * Force: F(r) = 24ε/r [2(σ/r)¹² - (σ/r)⁶]
 *
 * Features:
 * - Velocity Verlet integration
 * - Cell list spatial hashing for O(N) neighbor search
 * - Periodic boundary conditions
 * - Temperature control (velocity rescaling / Berendsen thermostat)
 * - Phase transitions: solid ↔ liquid ↔ gas
 * - Up to 10,000+ atoms at 60 FPS
 */

enum class MatterPhase {
    SOLID,
    LIQUID,
    GAS,
    UNKNOWN
};

struct MDConfig {
    int numAtoms = 500;
    float boxSize = 50.0f;          // Simulation box size
    float sigma = 1.0f;             // LJ length parameter
    float epsilon = 1.0f;           // LJ energy parameter
    float mass = 1.0f;              // Atomic mass
    float temperature = 1.0f;       // Target temperature (reduced units)
    float dt = 0.005f;              // Time step
    float cutoff = 2.5f;            // Cutoff distance (in units of sigma)
    float thermostatTau = 0.5f;     // Berendsen thermostat coupling
    bool periodicBC = true;         // Periodic boundary conditions
    bool useThermostat = true;      // Enable temperature control
};

struct MDStats {
    float kineticEnergy = 0.0f;
    float potentialEnergy = 0.0f;
    float totalEnergy = 0.0f;
    float temperature = 0.0f;
    float pressure = 0.0f;
    float density = 0.0f;
    MatterPhase phase = MatterPhase::UNKNOWN;
    int numAtoms = 0;
    float simulationTime = 0.0f;
    float meanSquareDisplacement = 0.0f;
};

struct Atom {
    float x, y;           // Position
    float vx, vy;         // Velocity
    float fx, fy;         // Force
    float x0, y0;         // Initial position (for MSD)
    bool active = true;
};

class MolecularDynamics {
public:
    MolecularDynamics();
    MolecularDynamics(const MDConfig& config);
    ~MolecularDynamics() = default;

    // Initialization
    void setConfig(const MDConfig& config);
    void initializeLattice();           // FCC/triangular lattice (solid)
    void initializeRandom();            // Random positions (gas)
    void initializeLiquid();            // Random with moderate density
    void clear();

    // Simulation
    void step(float dt);
    void stepMultiple(int steps, float dt);

    // Temperature control
    void setTemperature(float T);
    void rescaleVelocities(float targetT);
    void applyBerendsenThermostat(float targetT, float tau);

    // Parameters
    void setEpsilon(float eps);
    void setSigma(float sig);
    void setCutoff(float rc);
    void setThermostatEnabled(bool enabled);
    void setThermostatTau(float tau);

    // Getters
    const std::vector<Atom>& atoms() const { return atoms_; }
    const MDStats& stats() const { return stats_; }
    int atomCount() const { return static_cast<int>(atoms_.size()); }
    float boxSize() const { return boxSize_; }
    float getTemperature() const { return stats_.temperature; }
    float getKineticEnergy() const { return stats_.kineticEnergy; }
    float getPotentialEnergy() const { return stats_.potentialEnergy; }
    MatterPhase getPhase() const { return stats_.phase; }

    // Data access for JS
    const float* positionData() const { return positionData_.data(); }
    const float* velocityData() const { return velocityData_.data(); }
    int positionDataSize() const { return static_cast<int>(positionData_.size()); }

    // Statistics
    void computeStatistics();

private:
    MDConfig config_;
    std::vector<Atom> atoms_;
    MDStats stats_;
    float boxSize_;
    float halfBox_;
    float cutoffSq_;
    float simulationTime_;

    // Cell list for neighbor search
    std::vector<std::vector<int>> cells_;
    int cellsPerSide_;
    float cellSize_;

    // Data buffers for JS
    std::vector<float> positionData_;
    std::vector<float> velocityData_;

    // Random number generator
    std::mt19937 rng_;

    // Internal methods
    void computeForces();
    void buildCellList();
    void integrateVerlet(float dt);
    void applyPeriodicBC(Atom& atom);
    float minimumImage(float dx) const;
    void updateDataBuffers();
    MatterPhase detectPhase() const;

    // Lennard-Jones calculations
    inline float ljPotential(float r2) const;
    inline float ljForce(float r2) const;
};

// Presets
MDConfig mdPresetSolid();      // Low temperature, crystalline
MDConfig mdPresetLiquid();     // Medium temperature
MDConfig mdPresetGas();        // High temperature, low density
MDConfig mdPresetMelting();    // Near melting point
MDConfig mdPresetLarge();      // 5000+ atoms

} // namespace physics
} // namespace eigenlab
