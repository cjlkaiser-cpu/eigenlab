#pragma once
#include <vector>
#include <cstdint>
#include <random>

namespace eigenlab {
namespace physics {

/**
 * 3D Cellular Automata
 *
 * Three-dimensional extension of cellular automata rules.
 * Supports various rule sets including 3D Game of Life variants.
 *
 * Features:
 * - Configurable birth/survival rules
 * - Multiple neighborhood types (Moore, von Neumann)
 * - Periodic or fixed boundary conditions
 * - Color by age or neighborhood count
 */

struct CA3DConfig {
    int gridSize = 32;              // Grid dimensions (cubic)
    int birthMin = 5;               // Minimum neighbors for birth
    int birthMax = 7;               // Maximum neighbors for birth
    int surviveMin = 4;             // Minimum neighbors to survive
    int surviveMax = 6;             // Maximum neighbors to survive
    bool periodicBoundary = true;   // Wrap around edges
    bool mooreNeighborhood = true;  // Moore (26) vs von Neumann (6)
    float initialDensity = 0.2f;    // Initial random fill density
};

struct CA3DStats {
    int totalCells = 0;
    int aliveCells = 0;
    int generation = 0;
    float density = 0.0f;
    int births = 0;
    int deaths = 0;
};

class CellularAutomata3D {
public:
    CellularAutomata3D();
    explicit CellularAutomata3D(const CA3DConfig& config);
    ~CellularAutomata3D() = default;

    // Configuration
    void setConfig(const CA3DConfig& config);
    void reset();
    void randomize(float density);
    void clear();

    // Simulation
    void step();
    void stepMultiple(int steps);

    // Cell manipulation
    void setCell(int x, int y, int z, bool alive);
    bool getCell(int x, int y, int z) const;
    void addPattern(int cx, int cy, int cz, int pattern);

    // Rules
    void setRules(int birthMin, int birthMax, int surviveMin, int surviveMax);

    // Data access for JS
    const uint8_t* cellData() const { return cells_.data(); }
    const uint8_t* ageData() const { return ages_.data(); }
    int dataSize() const { return static_cast<int>(cells_.size()); }
    int gridSize() const { return config_.gridSize; }
    int aliveCellCount() const { return aliveCells_; }

    // Get alive cell positions for rendering
    const float* alivePositions() const { return alivePositions_.data(); }
    const float* aliveAges() const { return aliveAges_.data(); }
    int aliveCount() const { return aliveCount_; }

    // Statistics
    void computeStatistics();
    const CA3DStats& stats() const { return stats_; }

private:
    CA3DConfig config_;

    // Double buffer for cells
    std::vector<uint8_t> cells_;
    std::vector<uint8_t> cellsNext_;
    std::vector<uint8_t> ages_;

    // Alive cell positions for efficient rendering
    std::vector<float> alivePositions_;  // x, y, z for each alive cell
    std::vector<float> aliveAges_;
    int aliveCount_;
    int aliveCells_;

    // Statistics
    CA3DStats stats_;
    int generation_;

    // RNG
    std::mt19937 rng_;

    // Helper functions
    int index(int x, int y, int z) const;
    int countNeighbors(int x, int y, int z) const;
    int wrapCoord(int c) const;
    void updateAlivePositions();
};

// Presets (different rule sets)
CA3DConfig ca3dPresetGameOfLife();      // 3D Game of Life variant
CA3DConfig ca3dPresetCrystal();         // Crystal growth
CA3DConfig ca3dPresetAmoeba();          // Amoeba-like growth
CA3DConfig ca3dPresetPyramids();        // Pyramidal structures

} // namespace physics
} // namespace eigenlab
