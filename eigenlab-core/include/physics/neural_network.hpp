#pragma once
#include <vector>
#include <cstdint>
#include <random>

namespace eigenlab {
namespace physics {

/**
 * Neural Network - Hodgkin-Huxley Model
 *
 * Simulates a network of biologically realistic neurons.
 * Each neuron follows the Hodgkin-Huxley equations for
 * action potential generation.
 *
 * Features:
 * - HH ion channels (Na+, K+, leak)
 * - Synaptic connections with plasticity
 * - Spatial organization (2D grid)
 * - Spike detection and recording
 */

struct NeuralConfig {
    int numNeurons = 100;
    int gridWidth = 10;
    int gridHeight = 10;

    // Hodgkin-Huxley parameters (mV, ms, mS/cm^2)
    float C_m = 1.0f;           // Membrane capacitance
    float g_Na = 120.0f;        // Sodium conductance
    float g_K = 36.0f;          // Potassium conductance
    float g_L = 0.3f;           // Leak conductance
    float E_Na = 50.0f;         // Sodium reversal potential
    float E_K = -77.0f;         // Potassium reversal potential
    float E_L = -54.4f;         // Leak reversal potential

    // Network parameters
    float connectionProb = 0.1f; // Connection probability
    float synapseStrength = 0.5f;// Synaptic weight
    float synapseDecay = 5.0f;   // Synaptic current decay (ms)

    // Stimulation
    float stimCurrent = 10.0f;   // External current (uA/cm^2)

    float dt = 0.05f;           // Time step (ms)
};

struct NeuralStats {
    int numNeurons = 0;
    int numSpikes = 0;
    float avgPotential = 0.0f;
    float avgFiringRate = 0.0f;   // Hz
    int activeNeurons = 0;
    float simTime = 0.0f;         // ms
};

class NeuralNetwork {
public:
    NeuralNetwork();
    explicit NeuralNetwork(const NeuralConfig& config);
    ~NeuralNetwork() = default;

    // Configuration
    void setConfig(const NeuralConfig& config);
    void reset();

    // Simulation
    void step();
    void stepMultiple(int steps);

    // Stimulation
    void stimulateNeuron(int index, float current);
    void stimulateArea(float x, float y, float radius, float current);
    void stimulateRandom(int count, float current);
    void clearStimulation();

    // Parameters
    void setSynapseStrength(float strength);
    void setStimCurrent(float current);

    // Data access for JS
    const float* potentialData() const { return potentials_.data(); }
    const float* gateNData() const { return gateN_.data(); }
    const float* gateMData() const { return gateM_.data(); }
    const float* gateHData() const { return gateH_.data(); }
    const uint8_t* spikeData() const { return spikes_.data(); }
    int dataSize() const { return static_cast<int>(potentials_.size()); }
    int gridWidth() const { return config_.gridWidth; }
    int gridHeight() const { return config_.gridHeight; }

    // Statistics
    void computeStatistics();
    const NeuralStats& stats() const { return stats_; }

private:
    struct Neuron {
        float V;      // Membrane potential
        float m, h, n; // Gating variables
        float I_ext;   // External current
        float I_syn;   // Synaptic current
        bool spiked;
        float lastSpike; // Time of last spike
    };

    struct Synapse {
        int from;
        int to;
        float weight;
    };

    NeuralConfig config_;
    std::vector<Neuron> neurons_;
    std::vector<Synapse> synapses_;

    // Data buffers for JS
    std::vector<float> potentials_;
    std::vector<float> gateN_;
    std::vector<float> gateM_;
    std::vector<float> gateH_;
    std::vector<uint8_t> spikes_;

    // Statistics
    NeuralStats stats_;
    float simTime_;
    int totalSpikes_;

    // RNG
    std::mt19937 rng_;

    // Helper functions
    void buildNetwork();
    void updateNeuron(Neuron& n, float dt);
    void propagateSpikes();

    // Hodgkin-Huxley rate functions
    static float alphaN(float V);
    static float betaN(float V);
    static float alphaM(float V);
    static float betaM(float V);
    static float alphaH(float V);
    static float betaH(float V);

    void updateDataBuffers();
};

// Presets
NeuralConfig neuralPresetSmall();
NeuralConfig neuralPresetMedium();
NeuralConfig neuralPresetLarge();
NeuralConfig neuralPresetFast();

} // namespace physics
} // namespace eigenlab
