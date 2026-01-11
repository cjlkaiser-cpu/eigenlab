#include "../../include/physics/electrophoresis.hpp"
#include <cmath>
#include <algorithm>

namespace eigenlab {
namespace physics {

Electrophoresis::Electrophoresis()
    : moleculeCount_(0)
    , simulationTime_(0.0f)
    , rng_(std::random_device{}())
{
    setConfig(ElectrophoresisConfig{});
}

Electrophoresis::Electrophoresis(const ElectrophoresisConfig& config)
    : moleculeCount_(0)
    , simulationTime_(0.0f)
    , rng_(std::random_device{}())
{
    setConfig(config);
}

void Electrophoresis::setConfig(const ElectrophoresisConfig& config) {
    config_ = config;

    int maxMolecules = config.numLanes * config.particlesPerLane * 2;
    molecules_.reserve(maxMolecules);
    positions_.reserve(maxMolecules * 2);
    colors_.reserve(maxMolecules * 3);

    reset();
}

void Electrophoresis::reset() {
    molecules_.clear();
    moleculeCount_ = 0;
    simulationTime_ = 0.0f;
    updatePositionBuffer();
}

void Electrophoresis::clear() {
    reset();
}

// ==================================================
// Lane positioning
// ==================================================

float Electrophoresis::laneXPosition(int lane) const {
    float laneWidth = config_.gelWidth / config_.numLanes;
    return -config_.gelWidth / 2 + laneWidth * (lane + 0.5f);
}

// ==================================================
// Drag coefficient (larger molecules = more drag)
// ==================================================

float Electrophoresis::computeDragCoefficient(float size) const {
    // Simplified Ogston model: gamma ~ size^0.6 for gels
    // Also depends on gel concentration
    float baseGamma = 0.1f;
    float sizeEffect = std::pow(size / 100.0f, 0.6f);
    float gelEffect = std::pow(config_.gelConcentration, 0.8f);
    float tempEffect = 1.0f / (1.0f + 0.02f * (config_.temperature - 25.0f));

    return baseGamma * sizeEffect * gelEffect * tempEffect;
}

// ==================================================
// Sample loading
// ==================================================

void Electrophoresis::loadSample(int lane, const std::vector<MoleculeType>& types) {
    if (lane < 0 || lane >= config_.numLanes) return;

    float laneX = laneXPosition(lane);
    float wellY = config_.gelHeight / 2 - config_.wellDepth / 2;
    float laneWidth = config_.gelWidth / config_.numLanes;

    std::uniform_real_distribution<float> distX(-laneWidth * 0.3f, laneWidth * 0.3f);
    std::uniform_real_distribution<float> distY(-config_.wellDepth * 0.4f, config_.wellDepth * 0.4f);

    for (const auto& type : types) {
        int count = static_cast<int>(config_.particlesPerLane * type.concentration);

        for (int i = 0; i < count; ++i) {
            Molecule mol;
            mol.x = laneX + distX(rng_);
            mol.y = wellY + distY(rng_);
            mol.vx = 0;
            mol.vy = 0;
            mol.size = type.size;
            mol.charge = type.charge;
            mol.lane = lane;
            mol.gamma = computeDragCoefficient(type.size);
            mol.colorR = type.colorR;
            mol.colorG = type.colorG;
            mol.colorB = type.colorB;
            mol.active = true;

            molecules_.push_back(mol);
            moleculeCount_++;
        }
    }

    updatePositionBuffer();
}

void Electrophoresis::loadDNALadder(int lane) {
    // Standard DNA ladder sizes (bp)
    std::vector<MoleculeType> ladder = {
        {100.0f,  -1.0f, 0.15f, 255, 100, 100},   // 100 bp
        {200.0f,  -1.0f, 0.15f, 255, 120, 80},    // 200 bp
        {300.0f,  -1.0f, 0.15f, 255, 140, 60},    // 300 bp
        {500.0f,  -1.0f, 0.2f,  255, 160, 40},    // 500 bp (brighter)
        {750.0f,  -1.0f, 0.15f, 255, 180, 20},    // 750 bp
        {1000.0f, -1.0f, 0.2f,  255, 200, 0},     // 1000 bp (brighter)
        {1500.0f, -1.0f, 0.15f, 200, 180, 0},     // 1500 bp
        {2000.0f, -1.0f, 0.15f, 180, 160, 0},     // 2000 bp
        {3000.0f, -1.0f, 0.15f, 160, 140, 0},     // 3000 bp
    };

    loadSample(lane, ladder);
}

void Electrophoresis::loadProteinLadder(int lane) {
    // Standard protein ladder (kDa)
    std::vector<MoleculeType> ladder = {
        {10.0f,   -2.0f, 0.15f, 100, 150, 255},   // 10 kDa
        {15.0f,   -2.0f, 0.15f, 100, 170, 255},   // 15 kDa
        {25.0f,   -2.0f, 0.2f,  100, 190, 255},   // 25 kDa
        {35.0f,   -2.0f, 0.15f, 100, 210, 255},   // 35 kDa
        {50.0f,   -2.0f, 0.2f,  80, 200, 255},    // 50 kDa
        {75.0f,   -2.0f, 0.15f, 60, 180, 255},    // 75 kDa
        {100.0f,  -2.0f, 0.2f,  40, 160, 255},    // 100 kDa
        {150.0f,  -2.0f, 0.15f, 20, 140, 255},    // 150 kDa
        {250.0f,  -2.0f, 0.15f, 0, 120, 255},     // 250 kDa
    };

    loadSample(lane, ladder);
}

void Electrophoresis::loadRandomSample(int lane, int numBands) {
    std::uniform_real_distribution<float> sizeDist(100.0f, 3000.0f);
    std::uniform_real_distribution<float> concDist(0.1f, 0.25f);

    std::vector<MoleculeType> sample;
    for (int i = 0; i < numBands; ++i) {
        float size = sizeDist(rng_);
        float t = (size - 100.0f) / 2900.0f;

        MoleculeType type;
        type.size = size;
        type.charge = -1.0f;
        type.concentration = concDist(rng_);
        type.colorR = static_cast<int>(100 + t * 155);
        type.colorG = static_cast<int>(255 - t * 100);
        type.colorB = static_cast<int>(100 - t * 100);

        sample.push_back(type);
    }

    loadSample(lane, sample);
}

// ==================================================
// Simulation
// ==================================================

void Electrophoresis::step() {
    // Electric field (pointing down, from wells to bottom)
    float E = config_.voltage / (config_.gelHeight * 100.0f); // V/cm to V/unit

    for (auto& mol : molecules_) {
        if (!mol.active) continue;

        // Electric force: F = qE
        float Fy = mol.charge * E;

        // Update velocity (terminal velocity quickly reached)
        // At terminal: qE = gamma * v
        // v_terminal = qE / gamma
        float vy_terminal = Fy / mol.gamma;

        // Approach terminal velocity with damping
        mol.vy = mol.vy * 0.9f + vy_terminal * 0.1f;

        // Small random diffusion
        std::uniform_real_distribution<float> noise(-0.001f, 0.001f);
        mol.vx = noise(rng_);

        // Update position
        mol.x += mol.vx * config_.dt;
        mol.y += mol.vy * config_.dt;

        // Lane boundaries
        float laneWidth = config_.gelWidth / config_.numLanes;
        float laneCenter = laneXPosition(mol.lane);
        float laneMin = laneCenter - laneWidth * 0.45f;
        float laneMax = laneCenter + laneWidth * 0.45f;

        if (mol.x < laneMin) { mol.x = laneMin; mol.vx = 0; }
        if (mol.x > laneMax) { mol.x = laneMax; mol.vx = 0; }

        // Gel boundaries
        float gelBottom = -config_.gelHeight / 2;
        float gelTop = config_.gelHeight / 2;

        if (mol.y < gelBottom) {
            mol.y = gelBottom;
            mol.vy = 0;
        }
        if (mol.y > gelTop) {
            mol.y = gelTop;
            mol.vy = 0;
        }
    }

    simulationTime_ += config_.dt;
    updatePositionBuffer();
}

void Electrophoresis::stepMultiple(int steps) {
    for (int i = 0; i < steps; ++i) {
        step();
    }
}

void Electrophoresis::run(float duration) {
    int steps = static_cast<int>(duration / config_.dt);
    stepMultiple(steps);
}

// ==================================================
// Controls
// ==================================================

void Electrophoresis::setVoltage(float voltage) {
    config_.voltage = std::max(0.0f, std::min(300.0f, voltage));
}

void Electrophoresis::setGelConcentration(float conc) {
    config_.gelConcentration = std::max(0.5f, std::min(3.0f, conc));

    // Recalculate drag coefficients
    for (auto& mol : molecules_) {
        mol.gamma = computeDragCoefficient(mol.size);
    }
}

void Electrophoresis::setTemperature(float temp) {
    config_.temperature = std::max(4.0f, std::min(40.0f, temp));

    // Recalculate drag coefficients
    for (auto& mol : molecules_) {
        mol.gamma = computeDragCoefficient(mol.size);
    }
}

// ==================================================
// Statistics
// ==================================================

void Electrophoresis::computeStatistics() {
    stats_.totalMolecules = moleculeCount_;
    stats_.simulationTime = simulationTime_;
    stats_.currentVoltage = config_.voltage;

    float maxMigration = 0;
    float totalVelocity = 0;
    int count = 0;

    float wellY = config_.gelHeight / 2 - config_.wellDepth / 2;

    for (const auto& mol : molecules_) {
        if (!mol.active) continue;

        float migration = wellY - mol.y;
        if (migration > maxMigration) {
            maxMigration = migration;
        }

        totalVelocity += std::abs(mol.vy);
        count++;
    }

    stats_.maxMigration = maxMigration;
    stats_.avgVelocity = count > 0 ? totalVelocity / count : 0;
}

// ==================================================
// Buffer update
// ==================================================

void Electrophoresis::updatePositionBuffer() {
    positions_.clear();
    colors_.clear();

    for (const auto& mol : molecules_) {
        if (!mol.active) continue;

        positions_.push_back(mol.x);
        positions_.push_back(mol.y);

        colors_.push_back(mol.colorR / 255.0f);
        colors_.push_back(mol.colorG / 255.0f);
        colors_.push_back(mol.colorB / 255.0f);
    }
}

// ==================================================
// Presets
// ==================================================

ElectrophoresisConfig electrophoresisPresetDNA() {
    ElectrophoresisConfig config;
    config.numLanes = 6;
    config.particlesPerLane = 300;
    config.gelWidth = 2.0f;
    config.gelHeight = 4.0f;
    config.wellDepth = 0.3f;
    config.voltage = 100.0f;
    config.gelConcentration = 1.0f;  // 1% agarose
    config.temperature = 25.0f;
    config.dt = 0.001f;
    return config;
}

ElectrophoresisConfig electrophoresisPresetProtein() {
    ElectrophoresisConfig config;
    config.numLanes = 8;
    config.particlesPerLane = 250;
    config.gelWidth = 2.5f;
    config.gelHeight = 5.0f;
    config.wellDepth = 0.25f;
    config.voltage = 150.0f;
    config.gelConcentration = 1.5f;  // Higher for proteins
    config.temperature = 4.0f;       // Run cold
    config.dt = 0.001f;
    return config;
}

ElectrophoresisConfig electrophoresisPresetHighRes() {
    ElectrophoresisConfig config;
    config.numLanes = 4;
    config.particlesPerLane = 400;
    config.gelWidth = 1.5f;
    config.gelHeight = 6.0f;
    config.wellDepth = 0.2f;
    config.voltage = 80.0f;          // Lower voltage, better resolution
    config.gelConcentration = 2.0f;  // Higher concentration
    config.temperature = 20.0f;
    config.dt = 0.001f;
    return config;
}

} // namespace physics
} // namespace eigenlab
