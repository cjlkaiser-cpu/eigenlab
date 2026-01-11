#pragma once
#include <vector>
#include <random>

namespace eigenlab {
namespace physics {

/**
 * Gel Electrophoresis Simulation
 *
 * Separates charged molecules (DNA/proteins) by size in an electric field.
 * Smaller molecules move faster through the gel matrix.
 *
 * Physics:
 * - Electric force: F = qE
 * - Drag force: F = -gamma * v (Stokes drag in porous gel)
 * - Terminal velocity: v = qE / gamma
 * - gamma proportional to molecular size
 *
 * Features:
 * - Multiple sample lanes
 * - Size markers (molecular weight standards)
 * - Voltage/field strength control
 * - Gel concentration effect on resolution
 */

struct MoleculeType {
    float size;           // Base pairs (DNA) or kDa (protein)
    float charge;         // Net charge
    float concentration;  // Relative amount
    int colorR, colorG, colorB;
};

struct ElectrophoresisConfig {
    int numLanes = 6;              // Number of sample lanes
    int particlesPerLane = 200;    // Molecules per lane
    float gelWidth = 2.0f;         // Physical width
    float gelHeight = 4.0f;        // Physical height
    float wellDepth = 0.3f;        // Loading well depth
    float voltage = 100.0f;        // Voltage (V)
    float gelConcentration = 1.0f; // Agarose % (affects drag)
    float temperature = 25.0f;     // Temperature (C)
    float dt = 0.001f;             // Time step
};

struct ElectrophoresisStats {
    int totalMolecules = 0;
    float simulationTime = 0.0f;
    float currentVoltage = 0.0f;
    float maxMigration = 0.0f;
    float avgVelocity = 0.0f;
};

struct Molecule {
    float x, y;           // Position
    float vx, vy;         // Velocity
    float size;           // Molecular size (bp or kDa)
    float charge;         // Net charge
    int lane;             // Which lane
    float gamma;          // Drag coefficient
    int colorR, colorG, colorB;
    bool active;
};

class Electrophoresis {
public:
    Electrophoresis();
    explicit Electrophoresis(const ElectrophoresisConfig& config);
    ~Electrophoresis() = default;

    // Configuration
    void setConfig(const ElectrophoresisConfig& config);
    void reset();
    void clear();

    // Sample loading
    void loadSample(int lane, const std::vector<MoleculeType>& types);
    void loadDNALadder(int lane);           // Standard size markers
    void loadProteinLadder(int lane);       // Protein MW markers
    void loadRandomSample(int lane, int numBands);

    // Simulation
    void step();
    void stepMultiple(int steps);
    void run(float duration);

    // Controls
    void setVoltage(float voltage);
    void setGelConcentration(float conc);
    void setTemperature(float temp);

    // Data access
    const float* positionData() const { return positions_.data(); }
    const float* colorData() const { return colors_.data(); }
    int dataSize() const { return static_cast<int>(positions_.size()); }
    int moleculeCount() const { return moleculeCount_; }
    int numLanes() const { return config_.numLanes; }
    float gelWidth() const { return config_.gelWidth; }
    float gelHeight() const { return config_.gelHeight; }

    // Statistics
    void computeStatistics();
    const ElectrophoresisStats& stats() const { return stats_; }

private:
    ElectrophoresisConfig config_;
    ElectrophoresisStats stats_;

    std::vector<Molecule> molecules_;
    std::vector<float> positions_;  // x, y pairs
    std::vector<float> colors_;     // r, g, b triplets
    int moleculeCount_;

    float simulationTime_;
    std::mt19937 rng_;

    // Helper methods
    float computeDragCoefficient(float size) const;
    float laneXPosition(int lane) const;
    void updatePositionBuffer();
};

// Presets
ElectrophoresisConfig electrophoresisPresetDNA();      // DNA gel
ElectrophoresisConfig electrophoresisPresetProtein();  // SDS-PAGE
ElectrophoresisConfig electrophoresisPresetHighRes();  // High resolution

} // namespace physics
} // namespace eigenlab
