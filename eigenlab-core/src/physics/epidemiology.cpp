#include "physics/epidemiology.hpp"
#include <cmath>
#include <algorithm>

Epidemiology::Epidemiology()
    : m_rng(42), m_peakInfected(0), m_deaths(0), m_stepCount(0) {
    init();
}

Epidemiology::Epidemiology(const EpidemiologyConfig& config)
    : m_config(config), m_rng(42), m_peakInfected(0), m_deaths(0), m_stepCount(0) {
    init();
}

void Epidemiology::init() {
    m_stepCount = 0;
    m_peakInfected = 0;
    m_deaths = 0;
    m_sHistory.clear();
    m_iHistory.clear();
    m_rHistory.clear();
    initAgents();
    updateArrays();
    updateStatistics();
}

void Epidemiology::initAgents() {
    m_agents.clear();
    m_agents.reserve(m_config.numAgents);

    std::uniform_real_distribution<float> posDist(0, m_config.worldSize);
    std::uniform_real_distribution<float> velDist(-m_config.baseSpeed, m_config.baseSpeed);
    std::uniform_real_distribution<float> prob(0.0f, 1.0f);

    for (int i = 0; i < m_config.numAgents; ++i) {
        Agent a;
        a.x = posDist(m_rng);
        a.y = posDist(m_rng);
        a.vx = velDist(m_rng);
        a.vy = velDist(m_rng);
        a.state = HealthState::Susceptible;
        a.daysInfected = 0;
        a.daysSinceExposure = 0;
        a.isAlive = true;

        // Initial vaccination
        if (prob(m_rng) < m_config.vaccinationRate) {
            a.state = HealthState::Vaccinated;
        }

        m_agents.push_back(a);
    }

    // Introduce initial infected
    int infected = 0;
    while (infected < m_config.initialInfected && infected < m_config.numAgents) {
        int idx = static_cast<int>(prob(m_rng) * m_config.numAgents);
        if (m_agents[idx].state == HealthState::Susceptible) {
            m_agents[idx].state = HealthState::Infected;
            m_agents[idx].daysInfected = 1;
            infected++;
        }
    }
}

void Epidemiology::updateArrays() {
    int n = static_cast<int>(m_agents.size());

    m_positions.resize(n * 2);
    m_colors.resize(n * 3);
    m_states.resize(n);

    for (int i = 0; i < n; ++i) {
        const Agent& a = m_agents[i];

        m_positions[i * 2 + 0] = a.x;
        m_positions[i * 2 + 1] = a.y;

        float r, g, b;
        getAgentColor(a, r, g, b);
        m_colors[i * 3 + 0] = r;
        m_colors[i * 3 + 1] = g;
        m_colors[i * 3 + 2] = b;

        m_states[i] = static_cast<int>(a.state);
    }
}

void Epidemiology::getAgentColor(const Agent& a, float& r, float& g, float& b) const {
    if (!a.isAlive) {
        r = 0.2f; g = 0.2f; b = 0.2f;  // Dead: gray
        return;
    }

    switch (a.state) {
        case HealthState::Susceptible:
            r = 0.2f; g = 0.6f; b = 1.0f;  // Blue
            break;
        case HealthState::Infected:
            r = 1.0f; g = 0.2f; b = 0.2f;  // Red
            break;
        case HealthState::Recovered:
            r = 0.2f; g = 0.9f; b = 0.4f;  // Green
            break;
        case HealthState::Vaccinated:
            r = 0.6f; g = 0.4f; b = 1.0f;  // Purple
            break;
        case HealthState::Quarantined:
            r = 1.0f; g = 0.6f; b = 0.0f;  // Orange
            break;
    }
}

float Epidemiology::distance(const Agent& a1, const Agent& a2) const {
    float dx = a1.x - a2.x;
    float dy = a1.y - a2.y;
    return std::sqrt(dx * dx + dy * dy);
}

void Epidemiology::moveAgents() {
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> prob(0.0f, 1.0f);

    float speedFactor = m_config.socialDistancingFactor;

    for (auto& a : m_agents) {
        if (!a.isAlive) continue;

        // Quarantined agents don't move
        if (a.state == HealthState::Quarantined) continue;

        // Random walk with occasional direction change
        if (prob(m_rng) < 0.1f) {
            float angle = angleDist(m_rng);
            float speed = m_config.baseSpeed * speedFactor;
            a.vx = std::cos(angle) * speed;
            a.vy = std::sin(angle) * speed;
        }

        // Update position
        a.x += a.vx * m_config.dt;
        a.y += a.vy * m_config.dt;

        // Bounce off walls
        if (a.x < 0) { a.x = 0; a.vx *= -1; }
        if (a.x > m_config.worldSize) { a.x = m_config.worldSize; a.vx *= -1; }
        if (a.y < 0) { a.y = 0; a.vy *= -1; }
        if (a.y > m_config.worldSize) { a.y = m_config.worldSize; a.vy *= -1; }
    }
}

void Epidemiology::processInfections() {
    std::uniform_real_distribution<float> prob(0.0f, 1.0f);
    int n = static_cast<int>(m_agents.size());

    // Find all infections this step
    std::vector<int> newInfections;

    for (int i = 0; i < n; ++i) {
        Agent& susceptible = m_agents[i];
        if (!susceptible.isAlive) continue;
        if (susceptible.state != HealthState::Susceptible) continue;

        // Check proximity to all infected
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            const Agent& infected = m_agents[j];
            if (!infected.isAlive) continue;
            if (infected.state != HealthState::Infected) continue;

            float dist = distance(susceptible, infected);
            if (dist < m_config.infectionRadius) {
                // Chance of infection
                if (prob(m_rng) < m_config.transmissionRate) {
                    newInfections.push_back(i);
                    break;  // Can only be infected once
                }
            }
        }
    }

    // Apply infections
    for (int idx : newInfections) {
        m_agents[idx].state = HealthState::Infected;
        m_agents[idx].daysInfected = 0;
        m_agents[idx].daysSinceExposure = 0;
    }
}

void Epidemiology::processRecovery() {
    std::uniform_real_distribution<float> prob(0.0f, 1.0f);

    for (auto& a : m_agents) {
        if (!a.isAlive) continue;

        if (a.state == HealthState::Infected || a.state == HealthState::Quarantined) {
            a.daysInfected++;

            // Check for quarantine (if enabled and just got infected)
            if (m_config.enableQuarantine &&
                a.state == HealthState::Infected &&
                a.daysInfected == 3 &&
                prob(m_rng) < m_config.quarantineProbability) {
                a.state = HealthState::Quarantined;
            }

            // Recovery or death
            if (prob(m_rng) < m_config.recoveryRate) {
                // Check mortality
                if (prob(m_rng) < m_config.mortalityRate) {
                    a.isAlive = false;
                    m_deaths++;
                } else {
                    a.state = HealthState::Recovered;
                }
            }
        }
    }
}

void Epidemiology::updateStatistics() {
    int s = 0, i = 0, r = 0, v = 0, q = 0;

    for (const auto& a : m_agents) {
        if (!a.isAlive) continue;

        switch (a.state) {
            case HealthState::Susceptible: s++; break;
            case HealthState::Infected: i++; break;
            case HealthState::Recovered: r++; break;
            case HealthState::Vaccinated: v++; break;
            case HealthState::Quarantined: q++; break;
        }
    }

    m_sHistory.push_back(s);
    m_iHistory.push_back(i + q);  // Quarantined count as infected for stats
    m_rHistory.push_back(r);

    if (i + q > m_peakInfected) {
        m_peakInfected = i + q;
    }
}

void Epidemiology::step() {
    moveAgents();
    processInfections();
    processRecovery();
    m_stepCount++;

    if (m_stepCount % 10 == 0) {  // Update stats every 10 steps
        updateStatistics();
    }

    updateArrays();
}

void Epidemiology::step(int numSteps) {
    for (int i = 0; i < numSteps; ++i) {
        step();
    }
}

void Epidemiology::setTransmissionRate(float rate) {
    m_config.transmissionRate = rate;
}

void Epidemiology::setRecoveryRate(float rate) {
    m_config.recoveryRate = rate;
}

void Epidemiology::setVaccinationRate(float rate) {
    m_config.vaccinationRate = rate;
}

void Epidemiology::enableQuarantine(bool enable) {
    m_config.enableQuarantine = enable;
}

void Epidemiology::setSocialDistancing(float factor) {
    m_config.socialDistancingFactor = factor;
}

void Epidemiology::vaccinateRandom(int count) {
    std::uniform_real_distribution<float> prob(0.0f, 1.0f);
    int vaccinated = 0;
    int attempts = 0;

    while (vaccinated < count && attempts < m_config.numAgents * 2) {
        int idx = static_cast<int>(prob(m_rng) * m_agents.size());
        if (m_agents[idx].state == HealthState::Susceptible && m_agents[idx].isAlive) {
            m_agents[idx].state = HealthState::Vaccinated;
            vaccinated++;
        }
        attempts++;
    }
}

void Epidemiology::introduceInfected(int count) {
    std::uniform_real_distribution<float> prob(0.0f, 1.0f);
    int infected = 0;
    int attempts = 0;

    while (infected < count && attempts < m_config.numAgents * 2) {
        int idx = static_cast<int>(prob(m_rng) * m_agents.size());
        if (m_agents[idx].state == HealthState::Susceptible && m_agents[idx].isAlive) {
            m_agents[idx].state = HealthState::Infected;
            m_agents[idx].daysInfected = 1;
            infected++;
        }
        attempts++;
    }
}

// Presets
void Epidemiology::presetCovid() {
    m_config.transmissionRate = 0.25f;
    m_config.infectionRadius = 2.5f;
    m_config.recoveryRate = 0.03f;
    m_config.mortalityRate = 0.02f;
    m_config.initialInfected = 3;
    init();
}

void Epidemiology::presetFlu() {
    m_config.transmissionRate = 0.2f;
    m_config.infectionRadius = 2.0f;
    m_config.recoveryRate = 0.1f;
    m_config.mortalityRate = 0.001f;
    m_config.initialInfected = 5;
    init();
}

void Epidemiology::presetMeasles() {
    m_config.transmissionRate = 0.5f;
    m_config.infectionRadius = 3.0f;
    m_config.recoveryRate = 0.07f;
    m_config.mortalityRate = 0.002f;
    m_config.initialInfected = 2;
    init();
}

void Epidemiology::presetEbola() {
    m_config.transmissionRate = 0.15f;
    m_config.infectionRadius = 1.5f;
    m_config.recoveryRate = 0.04f;
    m_config.mortalityRate = 0.5f;
    m_config.initialInfected = 1;
    init();
}

EpidemiologyStats Epidemiology::getStats() const {
    EpidemiologyStats stats = {};

    int s = 0, i = 0, r = 0, v = 0, q = 0;

    for (const auto& a : m_agents) {
        if (!a.isAlive) continue;

        switch (a.state) {
            case HealthState::Susceptible: s++; break;
            case HealthState::Infected: i++; break;
            case HealthState::Recovered: r++; break;
            case HealthState::Vaccinated: v++; break;
            case HealthState::Quarantined: q++; break;
        }
    }

    stats.totalAgents = m_config.numAgents;
    stats.susceptible = s;
    stats.infected = i;
    stats.recovered = r;
    stats.vaccinated = v;
    stats.quarantined = q;
    stats.deaths = m_deaths;
    stats.peakInfected = m_peakInfected;
    stats.daysPassed = m_stepCount / 10;  // Approximate days

    // Estimate R0 from early data
    if (m_iHistory.size() > 5 && m_iHistory[0] > 0) {
        float earlyGrowth = static_cast<float>(m_iHistory[5]) / m_iHistory[0];
        stats.r0Estimate = 1.0f + std::log(earlyGrowth) / 5.0f * (1.0f / m_config.recoveryRate);
    } else {
        stats.r0Estimate = 0.0f;
    }

    stats.epidemicOver = (i + q == 0);

    return stats;
}
