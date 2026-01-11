#include "../../include/physics/neural_network.hpp"
#include <cmath>
#include <algorithm>

namespace eigenlab {
namespace physics {

NeuralNetwork::NeuralNetwork()
    : simTime_(0)
    , totalSpikes_(0)
    , rng_(std::random_device{}())
{
    setConfig(NeuralConfig{});
}

NeuralNetwork::NeuralNetwork(const NeuralConfig& config)
    : simTime_(0)
    , totalSpikes_(0)
    , rng_(std::random_device{}())
{
    setConfig(config);
}

void NeuralNetwork::setConfig(const NeuralConfig& config) {
    config_ = config;

    int n = config.numNeurons;
    neurons_.resize(n);
    potentials_.resize(n);
    gateN_.resize(n);
    gateM_.resize(n);
    gateH_.resize(n);
    spikes_.resize(n);

    reset();
}

void NeuralNetwork::reset() {
    simTime_ = 0;
    totalSpikes_ = 0;

    // Initialize neurons at resting potential
    float V0 = config_.E_L;  // Resting potential ~-65 mV

    // Steady-state gating variables at rest
    float m0 = alphaM(V0) / (alphaM(V0) + betaM(V0));
    float h0 = alphaH(V0) / (alphaH(V0) + betaH(V0));
    float n0 = alphaN(V0) / (alphaN(V0) + betaN(V0));

    for (auto& neuron : neurons_) {
        neuron.V = V0;
        neuron.m = m0;
        neuron.h = h0;
        neuron.n = n0;
        neuron.I_ext = 0;
        neuron.I_syn = 0;
        neuron.spiked = false;
        neuron.lastSpike = -1000;
    }

    buildNetwork();
    updateDataBuffers();
}

void NeuralNetwork::buildNetwork() {
    synapses_.clear();
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::uniform_real_distribution<float> weightDist(0.5f, 1.5f);

    int n = static_cast<int>(neurons_.size());

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            // Distance-dependent connection probability
            int xi = i % config_.gridWidth;
            int yi = i / config_.gridWidth;
            int xj = j % config_.gridWidth;
            int yj = j / config_.gridWidth;

            float dx = static_cast<float>(xj - xi);
            float dy = static_cast<float>(yj - yi);
            float distance = std::sqrt(dx * dx + dy * dy);

            // Probability decreases with distance
            float prob = config_.connectionProb * std::exp(-distance * 0.3f);

            if (dist(rng_) < prob) {
                Synapse syn;
                syn.from = i;
                syn.to = j;
                syn.weight = config_.synapseStrength * weightDist(rng_);
                synapses_.push_back(syn);
            }
        }
    }
}

// ==================================================
// Hodgkin-Huxley Rate Functions
// ==================================================

float NeuralNetwork::alphaN(float V) {
    float dV = V + 55.0f;
    if (std::abs(dV) < 0.001f) {
        return 0.1f;  // L'Hopital limit
    }
    return 0.01f * dV / (1.0f - std::exp(-dV / 10.0f));
}

float NeuralNetwork::betaN(float V) {
    return 0.125f * std::exp(-(V + 65.0f) / 80.0f);
}

float NeuralNetwork::alphaM(float V) {
    float dV = V + 40.0f;
    if (std::abs(dV) < 0.001f) {
        return 1.0f;  // L'Hopital limit
    }
    return 0.1f * dV / (1.0f - std::exp(-dV / 10.0f));
}

float NeuralNetwork::betaM(float V) {
    return 4.0f * std::exp(-(V + 65.0f) / 18.0f);
}

float NeuralNetwork::alphaH(float V) {
    return 0.07f * std::exp(-(V + 65.0f) / 20.0f);
}

float NeuralNetwork::betaH(float V) {
    return 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f));
}

// ==================================================
// Simulation
// ==================================================

void NeuralNetwork::updateNeuron(Neuron& n, float dt) {
    // Current Hodgkin-Huxley currents
    float I_Na = config_.g_Na * n.m * n.m * n.m * n.h * (n.V - config_.E_Na);
    float I_K = config_.g_K * n.n * n.n * n.n * n.n * (n.V - config_.E_K);
    float I_L = config_.g_L * (n.V - config_.E_L);

    // Total current
    float I_total = n.I_ext + n.I_syn - I_Na - I_K - I_L;

    // Update membrane potential
    float dV = I_total / config_.C_m;
    n.V += dV * dt;

    // Update gating variables (forward Euler)
    float dn = alphaN(n.V) * (1.0f - n.n) - betaN(n.V) * n.n;
    float dm = alphaM(n.V) * (1.0f - n.m) - betaM(n.V) * n.m;
    float dh = alphaH(n.V) * (1.0f - n.h) - betaH(n.V) * n.h;

    n.n += dn * dt;
    n.m += dm * dt;
    n.h += dh * dt;

    // Clamp gating variables
    n.n = std::max(0.0f, std::min(1.0f, n.n));
    n.m = std::max(0.0f, std::min(1.0f, n.m));
    n.h = std::max(0.0f, std::min(1.0f, n.h));

    // Spike detection (threshold crossing)
    bool wasAbove = n.spiked;
    bool isAbove = n.V > 0.0f;  // Threshold at 0 mV

    if (!wasAbove && isAbove) {
        n.spiked = true;
        n.lastSpike = simTime_;
        totalSpikes_++;
    } else if (n.V < -20.0f) {
        n.spiked = false;
    }

    // Decay synaptic current
    n.I_syn *= std::exp(-dt / config_.synapseDecay);
}

void NeuralNetwork::propagateSpikes() {
    for (const auto& syn : synapses_) {
        Neuron& pre = neurons_[syn.from];
        Neuron& post = neurons_[syn.to];

        // If presynaptic neuron just spiked, add current to postsynaptic
        if (pre.spiked && (simTime_ - pre.lastSpike) < config_.dt * 2) {
            post.I_syn += syn.weight;
        }
    }
}

void NeuralNetwork::step() {
    float dt = config_.dt;

    // Propagate spikes through synapses
    propagateSpikes();

    // Update all neurons
    for (auto& neuron : neurons_) {
        updateNeuron(neuron, dt);
    }

    simTime_ += dt;
    updateDataBuffers();
}

void NeuralNetwork::stepMultiple(int steps) {
    for (int i = 0; i < steps; ++i) {
        step();
    }
    computeStatistics();
}

// ==================================================
// Stimulation
// ==================================================

void NeuralNetwork::stimulateNeuron(int index, float current) {
    if (index >= 0 && index < static_cast<int>(neurons_.size())) {
        neurons_[index].I_ext = current;
    }
}

void NeuralNetwork::stimulateArea(float x, float y, float radius, float current) {
    int gw = config_.gridWidth;
    int gh = config_.gridHeight;

    for (int i = 0; i < static_cast<int>(neurons_.size()); ++i) {
        float nx = static_cast<float>(i % gw) / static_cast<float>(gw);
        float ny = static_cast<float>(i / gw) / static_cast<float>(gh);

        float dx = nx - x;
        float dy = ny - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < radius) {
            neurons_[i].I_ext = current * (1.0f - dist / radius);
        }
    }
}

void NeuralNetwork::stimulateRandom(int count, float current) {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(neurons_.size()) - 1);

    for (int i = 0; i < count; ++i) {
        int idx = dist(rng_);
        neurons_[idx].I_ext = current;
    }
}

void NeuralNetwork::clearStimulation() {
    for (auto& neuron : neurons_) {
        neuron.I_ext = 0;
    }
}

// ==================================================
// Parameters
// ==================================================

void NeuralNetwork::setSynapseStrength(float strength) {
    config_.synapseStrength = strength;

    // Update existing synapses
    std::uniform_real_distribution<float> weightDist(0.5f, 1.5f);
    for (auto& syn : synapses_) {
        syn.weight = strength * weightDist(rng_);
    }
}

void NeuralNetwork::setStimCurrent(float current) {
    config_.stimCurrent = current;
}

// ==================================================
// Statistics
// ==================================================

void NeuralNetwork::computeStatistics() {
    float sumV = 0;
    int active = 0;
    int recentSpikes = 0;

    for (const auto& neuron : neurons_) {
        sumV += neuron.V;

        if (neuron.V > -50.0f) {
            active++;
        }

        // Count spikes in last 100 ms
        if (simTime_ - neuron.lastSpike < 100.0f) {
            recentSpikes++;
        }
    }

    int n = static_cast<int>(neurons_.size());
    stats_.numNeurons = n;
    stats_.numSpikes = totalSpikes_;
    stats_.avgPotential = sumV / n;
    stats_.activeNeurons = active;
    stats_.simTime = simTime_;

    // Firing rate in Hz (spikes per neuron per second)
    float windowMs = 100.0f;
    stats_.avgFiringRate = (recentSpikes / static_cast<float>(n)) * (1000.0f / windowMs);
}

void NeuralNetwork::updateDataBuffers() {
    for (size_t i = 0; i < neurons_.size(); ++i) {
        potentials_[i] = neurons_[i].V;
        gateN_[i] = neurons_[i].n;
        gateM_[i] = neurons_[i].m;
        gateH_[i] = neurons_[i].h;
        spikes_[i] = neurons_[i].spiked ? 1 : 0;
    }
}

// ==================================================
// Presets
// ==================================================

NeuralConfig neuralPresetSmall() {
    NeuralConfig config;
    config.numNeurons = 64;
    config.gridWidth = 8;
    config.gridHeight = 8;
    config.connectionProb = 0.15f;
    config.synapseStrength = 0.8f;
    config.dt = 0.05f;
    return config;
}

NeuralConfig neuralPresetMedium() {
    NeuralConfig config;
    config.numNeurons = 256;
    config.gridWidth = 16;
    config.gridHeight = 16;
    config.connectionProb = 0.08f;
    config.synapseStrength = 0.6f;
    config.dt = 0.05f;
    return config;
}

NeuralConfig neuralPresetLarge() {
    NeuralConfig config;
    config.numNeurons = 625;
    config.gridWidth = 25;
    config.gridHeight = 25;
    config.connectionProb = 0.05f;
    config.synapseStrength = 0.5f;
    config.dt = 0.05f;
    return config;
}

NeuralConfig neuralPresetFast() {
    NeuralConfig config;
    config.numNeurons = 100;
    config.gridWidth = 10;
    config.gridHeight = 10;
    config.connectionProb = 0.2f;
    config.synapseStrength = 1.0f;
    config.g_Na = 150.0f;  // Faster dynamics
    config.dt = 0.1f;
    return config;
}

} // namespace physics
} // namespace eigenlab
