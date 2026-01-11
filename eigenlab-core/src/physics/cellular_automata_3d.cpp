#include "../../include/physics/cellular_automata_3d.hpp"
#include <cmath>
#include <algorithm>

namespace eigenlab {
namespace physics {

CellularAutomata3D::CellularAutomata3D()
    : aliveCount_(0)
    , aliveCells_(0)
    , generation_(0)
    , rng_(std::random_device{}())
{
    setConfig(CA3DConfig{});
}

CellularAutomata3D::CellularAutomata3D(const CA3DConfig& config)
    : aliveCount_(0)
    , aliveCells_(0)
    , generation_(0)
    , rng_(std::random_device{}())
{
    setConfig(config);
}

void CellularAutomata3D::setConfig(const CA3DConfig& config) {
    config_ = config;

    int size = config.gridSize;
    int totalCells = size * size * size;

    cells_.resize(totalCells, 0);
    cellsNext_.resize(totalCells, 0);
    ages_.resize(totalCells, 0);

    // Reserve space for alive positions (worst case all alive)
    alivePositions_.reserve(totalCells * 3);
    aliveAges_.reserve(totalCells);

    reset();
}

void CellularAutomata3D::reset() {
    generation_ = 0;
    randomize(config_.initialDensity);
}

void CellularAutomata3D::randomize(float density) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    int size = config_.gridSize;
    for (int z = 0; z < size; ++z) {
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                bool alive = dist(rng_) < density;
                cells_[index(x, y, z)] = alive ? 1 : 0;
                ages_[index(x, y, z)] = 0;
            }
        }
    }

    updateAlivePositions();
}

void CellularAutomata3D::clear() {
    std::fill(cells_.begin(), cells_.end(), 0);
    std::fill(ages_.begin(), ages_.end(), 0);
    generation_ = 0;
    updateAlivePositions();
}

// ==================================================
// Indexing
// ==================================================

int CellularAutomata3D::index(int x, int y, int z) const {
    int size = config_.gridSize;
    return z * size * size + y * size + x;
}

int CellularAutomata3D::wrapCoord(int c) const {
    int size = config_.gridSize;
    if (c < 0) return c + size;
    if (c >= size) return c - size;
    return c;
}

int CellularAutomata3D::countNeighbors(int x, int y, int z) const {
    int count = 0;
    int size = config_.gridSize;

    if (config_.mooreNeighborhood) {
        // Moore neighborhood: 26 neighbors
        for (int dz = -1; dz <= 1; ++dz) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0 && dz == 0) continue;

                    int nx = x + dx;
                    int ny = y + dy;
                    int nz = z + dz;

                    if (config_.periodicBoundary) {
                        nx = wrapCoord(nx);
                        ny = wrapCoord(ny);
                        nz = wrapCoord(nz);
                    } else {
                        if (nx < 0 || nx >= size) continue;
                        if (ny < 0 || ny >= size) continue;
                        if (nz < 0 || nz >= size) continue;
                    }

                    if (cells_[index(nx, ny, nz)]) {
                        count++;
                    }
                }
            }
        }
    } else {
        // Von Neumann neighborhood: 6 neighbors
        const int dirs[6][3] = {
            {-1, 0, 0}, {1, 0, 0},
            {0, -1, 0}, {0, 1, 0},
            {0, 0, -1}, {0, 0, 1}
        };

        for (int i = 0; i < 6; ++i) {
            int nx = x + dirs[i][0];
            int ny = y + dirs[i][1];
            int nz = z + dirs[i][2];

            if (config_.periodicBoundary) {
                nx = wrapCoord(nx);
                ny = wrapCoord(ny);
                nz = wrapCoord(nz);
            } else {
                if (nx < 0 || nx >= size) continue;
                if (ny < 0 || ny >= size) continue;
                if (nz < 0 || nz >= size) continue;
            }

            if (cells_[index(nx, ny, nz)]) {
                count++;
            }
        }
    }

    return count;
}

// ==================================================
// Simulation
// ==================================================

void CellularAutomata3D::step() {
    int size = config_.gridSize;
    int births = 0, deaths = 0;

    for (int z = 0; z < size; ++z) {
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                int idx = index(x, y, z);
                int neighbors = countNeighbors(x, y, z);
                bool alive = cells_[idx] != 0;

                bool nextAlive = false;
                if (alive) {
                    // Survival rule
                    if (neighbors >= config_.surviveMin && neighbors <= config_.surviveMax) {
                        nextAlive = true;
                    }
                } else {
                    // Birth rule
                    if (neighbors >= config_.birthMin && neighbors <= config_.birthMax) {
                        nextAlive = true;
                    }
                }

                cellsNext_[idx] = nextAlive ? 1 : 0;

                // Update age
                if (nextAlive) {
                    if (alive) {
                        ages_[idx] = std::min(255, ages_[idx] + 1);
                    } else {
                        ages_[idx] = 1;
                        births++;
                    }
                } else {
                    if (alive) {
                        deaths++;
                    }
                    ages_[idx] = 0;
                }
            }
        }
    }

    // Swap buffers
    std::swap(cells_, cellsNext_);

    generation_++;
    stats_.births = births;
    stats_.deaths = deaths;

    updateAlivePositions();
}

void CellularAutomata3D::stepMultiple(int steps) {
    for (int i = 0; i < steps; ++i) {
        step();
    }
    computeStatistics();
}

// ==================================================
// Cell manipulation
// ==================================================

void CellularAutomata3D::setCell(int x, int y, int z, bool alive) {
    if (x < 0 || x >= config_.gridSize) return;
    if (y < 0 || y >= config_.gridSize) return;
    if (z < 0 || z >= config_.gridSize) return;

    int idx = index(x, y, z);
    cells_[idx] = alive ? 1 : 0;
    ages_[idx] = alive ? 1 : 0;
}

bool CellularAutomata3D::getCell(int x, int y, int z) const {
    if (x < 0 || x >= config_.gridSize) return false;
    if (y < 0 || y >= config_.gridSize) return false;
    if (z < 0 || z >= config_.gridSize) return false;

    return cells_[index(x, y, z)] != 0;
}

void CellularAutomata3D::addPattern(int cx, int cy, int cz, int pattern) {
    // Pattern 0: Small cube
    // Pattern 1: Cross
    // Pattern 2: Diagonal line
    // Pattern 3: Random cluster

    switch (pattern) {
        case 0: // Small cube
            for (int dz = -1; dz <= 1; ++dz) {
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        setCell(cx + dx, cy + dy, cz + dz, true);
                    }
                }
            }
            break;

        case 1: // Cross
            for (int d = -3; d <= 3; ++d) {
                setCell(cx + d, cy, cz, true);
                setCell(cx, cy + d, cz, true);
                setCell(cx, cy, cz + d, true);
            }
            break;

        case 2: // Diagonal line
            for (int d = -3; d <= 3; ++d) {
                setCell(cx + d, cy + d, cz + d, true);
            }
            break;

        case 3: { // Random cluster
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            for (int dz = -3; dz <= 3; ++dz) {
                for (int dy = -3; dy <= 3; ++dy) {
                    for (int dx = -3; dx <= 3; ++dx) {
                        float d = std::sqrt(static_cast<float>(dx*dx + dy*dy + dz*dz));
                        if (dist(rng_) < 0.6f - d * 0.1f) {
                            setCell(cx + dx, cy + dy, cz + dz, true);
                        }
                    }
                }
            }
            break;
        }
    }

    updateAlivePositions();
}

void CellularAutomata3D::setRules(int birthMin, int birthMax, int surviveMin, int surviveMax) {
    config_.birthMin = birthMin;
    config_.birthMax = birthMax;
    config_.surviveMin = surviveMin;
    config_.surviveMax = surviveMax;
}

// ==================================================
// Statistics
// ==================================================

void CellularAutomata3D::computeStatistics() {
    int size = config_.gridSize;
    int total = size * size * size;

    stats_.totalCells = total;
    stats_.aliveCells = aliveCells_;
    stats_.generation = generation_;
    stats_.density = static_cast<float>(aliveCells_) / total;
}

void CellularAutomata3D::updateAlivePositions() {
    alivePositions_.clear();
    aliveAges_.clear();
    aliveCells_ = 0;

    int size = config_.gridSize;
    float scale = 2.0f / size;  // Normalize to [-1, 1]

    for (int z = 0; z < size; ++z) {
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                int idx = index(x, y, z);
                if (cells_[idx]) {
                    alivePositions_.push_back((x + 0.5f) * scale - 1.0f);
                    alivePositions_.push_back((y + 0.5f) * scale - 1.0f);
                    alivePositions_.push_back((z + 0.5f) * scale - 1.0f);
                    aliveAges_.push_back(static_cast<float>(ages_[idx]) / 255.0f);
                    aliveCells_++;
                }
            }
        }
    }

    aliveCount_ = aliveCells_;
}

// ==================================================
// Presets
// ==================================================

CA3DConfig ca3dPresetGameOfLife() {
    CA3DConfig config;
    config.gridSize = 32;
    config.birthMin = 5;
    config.birthMax = 7;
    config.surviveMin = 4;
    config.surviveMax = 6;
    config.initialDensity = 0.25f;
    config.mooreNeighborhood = true;
    return config;
}

CA3DConfig ca3dPresetCrystal() {
    CA3DConfig config;
    config.gridSize = 40;
    config.birthMin = 1;
    config.birthMax = 3;
    config.surviveMin = 1;
    config.surviveMax = 4;
    config.initialDensity = 0.01f;  // Sparse initial state
    config.mooreNeighborhood = false;  // Von Neumann
    return config;
}

CA3DConfig ca3dPresetAmoeba() {
    CA3DConfig config;
    config.gridSize = 30;
    config.birthMin = 9;
    config.birthMax = 21;
    config.surviveMin = 5;
    config.surviveMax = 12;
    config.initialDensity = 0.35f;
    config.mooreNeighborhood = true;
    return config;
}

CA3DConfig ca3dPresetPyramids() {
    CA3DConfig config;
    config.gridSize = 36;
    config.birthMin = 4;
    config.birthMax = 4;
    config.surviveMin = 3;
    config.surviveMax = 5;
    config.initialDensity = 0.15f;
    config.mooreNeighborhood = true;
    return config;
}

} // namespace physics
} // namespace eigenlab
