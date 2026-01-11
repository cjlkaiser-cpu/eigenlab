#ifndef EPIDEMIOLOGY_HPP
#define EPIDEMIOLOGY_HPP

#include <vector>
#include <random>

/**
 * Epidemiology - Agent-based SIR epidemic model
 *
 * Physics/Model:
 * - SIR compartmental model: Susceptible → Infected → Recovered
 * - Spatial agent-based simulation with random movement
 * - Infection transmission based on proximity and contact rate
 * - Customizable quarantine, vaccination, social distancing
 *
 * Applications: Disease spread modeling, public health policy
 */

enum class HealthState {
    Susceptible,   // S - can be infected
    Infected,      // I - currently infectious
    Recovered,     // R - immune (or dead)
    Vaccinated,    // V - protected
    Quarantined    // Q - infected but isolated
};

struct EpidemiologyConfig {
    int numAgents = 500;
    float worldSize = 100.0f;

    // Disease parameters
    float transmissionRate = 0.3f;     // Probability of infection on contact
    float infectionRadius = 2.0f;       // Distance for potential transmission
    float recoveryRate = 0.05f;         // Probability of recovery per step
    float mortalityRate = 0.01f;        // Probability of death if infected
    int incubationPeriod = 0;           // Steps before becoming infectious

    // Initial conditions
    int initialInfected = 5;
    float vaccinationRate = 0.0f;       // Initial vaccination coverage

    // Interventions
    bool enableQuarantine = false;
    float quarantineProbability = 0.5f; // Probability infected person quarantines
    float socialDistancingFactor = 1.0f; // 0.5 = half movement speed

    // Agent movement
    float baseSpeed = 1.0f;
    float dt = 0.1f;
};

struct EpidemiologyStats {
    int totalAgents;
    int susceptible;
    int infected;
    int recovered;
    int vaccinated;
    int quarantined;
    int deaths;
    float r0Estimate;           // Basic reproduction number estimate
    int peakInfected;
    int daysPassed;
    bool epidemicOver;
};

struct Agent {
    float x, y;
    float vx, vy;
    HealthState state;
    int daysInfected;
    int daysSinceExposure;
    bool isAlive;
};

class Epidemiology {
public:
    Epidemiology();
    explicit Epidemiology(const EpidemiologyConfig& config);

    void init();
    void step();
    void step(int numSteps);

    // Interventions
    void setTransmissionRate(float rate);
    void setRecoveryRate(float rate);
    void setVaccinationRate(float rate);
    void enableQuarantine(bool enable);
    void setSocialDistancing(float factor);
    void vaccinateRandom(int count);
    void introduceInfected(int count);

    // Presets
    void presetCovid();          // High R0, moderate mortality
    void presetFlu();            // Moderate R0, low mortality
    void presetMeasles();        // Very high R0, low mortality
    void presetEbola();          // Lower R0, high mortality

    // Data access
    const float* positionData() const { return m_positions.data(); }
    const float* colorData() const { return m_colors.data(); }
    const int* stateData() const { return m_states.data(); }

    int agentCount() const { return static_cast<int>(m_agents.size()); }
    EpidemiologyStats getStats() const;

    // Time series for plotting
    const std::vector<int>& getSHistory() const { return m_sHistory; }
    const std::vector<int>& getIHistory() const { return m_iHistory; }
    const std::vector<int>& getRHistory() const { return m_rHistory; }

private:
    EpidemiologyConfig m_config;
    std::vector<Agent> m_agents;

    // Flattened arrays for JS interop
    std::vector<float> m_positions;   // x,y per agent
    std::vector<float> m_colors;      // r,g,b per agent
    std::vector<int> m_states;        // HealthState per agent

    // Statistics tracking
    std::vector<int> m_sHistory;
    std::vector<int> m_iHistory;
    std::vector<int> m_rHistory;
    int m_peakInfected;
    int m_deaths;
    int m_stepCount;

    std::mt19937 m_rng;

    void initAgents();
    void updateArrays();
    void moveAgents();
    void processInfections();
    void processRecovery();
    void updateStatistics();

    void getAgentColor(const Agent& a, float& r, float& g, float& b) const;
    float distance(const Agent& a1, const Agent& a2) const;
};

#endif // EPIDEMIOLOGY_HPP
