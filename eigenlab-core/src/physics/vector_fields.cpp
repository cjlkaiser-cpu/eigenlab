#include "physics/vector_fields.hpp"
#include <cmath>
#include <algorithm>

VectorFields::VectorFields()
    : m_seed(12345), m_simTime(0.0f) {
    init();
}

VectorFields::VectorFields(const VectorFieldConfig& config)
    : m_config(config), m_seed(12345), m_simTime(0.0f) {
    init();
}

float VectorFields::randomFloat() {
    m_seed = m_seed * 1103515245 + 12345;
    return static_cast<float>(m_seed % 10000) / 10000.0f;
}

void VectorFields::init() {
    m_simTime = 0.0f;

    // Initialize particles
    m_particles.clear();
    m_particles.resize(m_config.numParticles);
    for (auto& p : m_particles) {
        spawnParticle(p);
    }

    // Initialize field grid
    int n = m_config.gridWidth * m_config.gridHeight;
    m_fieldData.resize(n * 2);

    updateFieldGrid();
    updateParticles();
}

void VectorFields::spawnParticle(FieldParticle& p) {
    float hw = m_config.domainWidth / 2.0f;
    float hh = m_config.domainHeight / 2.0f;

    p.x = (randomFloat() - 0.5f) * m_config.domainWidth;
    p.y = (randomFloat() - 0.5f) * m_config.domainHeight;
    p.age = randomFloat() * m_config.particleLifetime;  // Stagger ages
    p.brightness = 1.0f;
}

void VectorFields::evaluateFieldAt(float x, float y, float& vx, float& vy) const {
    switch (m_config.fieldType) {
        case FieldType::Uniform:
            fieldUniform(x, y, vx, vy);
            break;
        case FieldType::Rotation:
            fieldRotation(x, y, vx, vy);
            break;
        case FieldType::Source:
            fieldSource(x, y, vx, vy);
            break;
        case FieldType::Sink:
            fieldSink(x, y, vx, vy);
            break;
        case FieldType::Dipole:
            fieldDipole(x, y, vx, vy);
            break;
        case FieldType::Saddle:
            fieldSaddle(x, y, vx, vy);
            break;
        case FieldType::SinCos:
            fieldSinCos(x, y, vx, vy);
            break;
        default:
            fieldRotation(x, y, vx, vy);
            break;
    }

    vx *= m_config.fieldStrength;
    vy *= m_config.fieldStrength;
}

void VectorFields::fieldUniform(float x, float y, float& vx, float& vy) const {
    vx = 1.0f;
    vy = 0.0f;
}

void VectorFields::fieldRotation(float x, float y, float& vx, float& vy) const {
    float dx = x - m_config.centerX;
    float dy = y - m_config.centerY;
    float r = std::sqrt(dx * dx + dy * dy) + 0.1f;

    // Tangential flow: perpendicular to radius
    vx = -dy / r;
    vy = dx / r;
}

void VectorFields::fieldSource(float x, float y, float& vx, float& vy) const {
    float dx = x - m_config.centerX;
    float dy = y - m_config.centerY;
    float r2 = dx * dx + dy * dy + 0.1f;
    float r = std::sqrt(r2);

    // Radial outward
    vx = dx / r;
    vy = dy / r;
}

void VectorFields::fieldSink(float x, float y, float& vx, float& vy) const {
    float dx = x - m_config.centerX;
    float dy = y - m_config.centerY;
    float r2 = dx * dx + dy * dy + 0.1f;
    float r = std::sqrt(r2);

    // Radial inward
    vx = -dx / r;
    vy = -dy / r;
}

void VectorFields::fieldDipole(float x, float y, float& vx, float& vy) const {
    float d = m_config.dipoleDistance / 2.0f;

    // Positive charge at (cx + d, cy)
    float dx1 = x - (m_config.centerX + d);
    float dy1 = y - m_config.centerY;
    float r1_2 = dx1 * dx1 + dy1 * dy1 + 0.1f;
    float r1 = std::sqrt(r1_2);

    // Negative charge at (cx - d, cy)
    float dx2 = x - (m_config.centerX - d);
    float dy2 = y - m_config.centerY;
    float r2_2 = dx2 * dx2 + dy2 * dy2 + 0.1f;
    float r2 = std::sqrt(r2_2);

    // Superposition (positive radiates out, negative attracts)
    vx = dx1 / r1 - dx2 / r2;
    vy = dy1 / r1 - dy2 / r2;

    // Normalize for visualization
    float mag = std::sqrt(vx * vx + vy * vy) + 0.01f;
    vx /= mag;
    vy /= mag;
}

void VectorFields::fieldSaddle(float x, float y, float& vx, float& vy) const {
    float dx = x - m_config.centerX;
    float dy = y - m_config.centerY;

    // f(x,y) = xy -> grad = (y, x)
    vx = dy;
    vy = dx;
}

void VectorFields::fieldSinCos(float x, float y, float& vx, float& vy) const {
    float k = m_config.frequency;

    vx = std::sin(k * y);
    vy = std::cos(k * x);
}

void VectorFields::updateFieldGrid() {
    float dx = m_config.domainWidth / (m_config.gridWidth - 1);
    float dy = m_config.domainHeight / (m_config.gridHeight - 1);
    float hw = m_config.domainWidth / 2.0f;
    float hh = m_config.domainHeight / 2.0f;

    for (int j = 0; j < m_config.gridHeight; ++j) {
        for (int i = 0; i < m_config.gridWidth; ++i) {
            float x = i * dx - hw;
            float y = j * dy - hh;

            float vx, vy;
            evaluateFieldAt(x, y, vx, vy);

            int idx = (j * m_config.gridWidth + i) * 2;
            m_fieldData[idx] = vx;
            m_fieldData[idx + 1] = vy;
        }
    }
}

void VectorFields::advectRK4(FieldParticle& p, float dt) {
    float vx1, vy1, vx2, vy2, vx3, vy3, vx4, vy4;

    // k1
    evaluateFieldAt(p.x, p.y, vx1, vy1);

    // k2
    evaluateFieldAt(p.x + 0.5f * dt * vx1, p.y + 0.5f * dt * vy1, vx2, vy2);

    // k3
    evaluateFieldAt(p.x + 0.5f * dt * vx2, p.y + 0.5f * dt * vy2, vx3, vy3);

    // k4
    evaluateFieldAt(p.x + dt * vx3, p.y + dt * vy3, vx4, vy4);

    // RK4 combination
    p.x += (dt / 6.0f) * (vx1 + 2.0f * vx2 + 2.0f * vx3 + vx4) * m_config.particleSpeed;
    p.y += (dt / 6.0f) * (vy1 + 2.0f * vy2 + 2.0f * vy3 + vy4) * m_config.particleSpeed;
}

void VectorFields::updateParticles() {
    float hw = m_config.domainWidth / 2.0f;
    float hh = m_config.domainHeight / 2.0f;

    for (auto& p : m_particles) {
        p.age += m_config.dt;

        // Respawn if too old or out of bounds
        if (p.age > m_config.particleLifetime ||
            p.x < -hw * 1.1f || p.x > hw * 1.1f ||
            p.y < -hh * 1.1f || p.y > hh * 1.1f) {
            spawnParticle(p);
            p.age = 0.0f;
        }

        // Update brightness based on age (fade in and out)
        float t = p.age / m_config.particleLifetime;
        if (t < 0.1f) {
            p.brightness = t / 0.1f;  // Fade in
        } else if (t > 0.8f) {
            p.brightness = (1.0f - t) / 0.2f;  // Fade out
        } else {
            p.brightness = 1.0f;
        }
    }

    // Update arrays
    int n = static_cast<int>(m_particles.size());
    m_particlePos.resize(n * 2);
    m_particleBright.resize(n);

    for (int i = 0; i < n; ++i) {
        m_particlePos[i * 2] = m_particles[i].x;
        m_particlePos[i * 2 + 1] = m_particles[i].y;
        m_particleBright[i] = m_particles[i].brightness;
    }
}

void VectorFields::step() {
    for (auto& p : m_particles) {
        advectRK4(p, m_config.dt);
    }

    m_simTime += m_config.dt;
    updateParticles();
}

void VectorFields::step(int numSteps) {
    for (int i = 0; i < numSteps; ++i) {
        for (auto& p : m_particles) {
            advectRK4(p, m_config.dt);
        }
        m_simTime += m_config.dt;
    }
    updateParticles();
}

void VectorFields::setFieldType(FieldType type) {
    m_config.fieldType = type;
    updateFieldGrid();
}

void VectorFields::setFieldStrength(float strength) {
    m_config.fieldStrength = strength;
    updateFieldGrid();
}

void VectorFields::setFieldCenter(float x, float y) {
    m_config.centerX = x;
    m_config.centerY = y;
    updateFieldGrid();
}

void VectorFields::setFrequency(float freq) {
    m_config.frequency = freq;
    updateFieldGrid();
}

void VectorFields::setDipoleDistance(float dist) {
    m_config.dipoleDistance = dist;
    updateFieldGrid();
}

void VectorFields::resetParticles() {
    for (auto& p : m_particles) {
        spawnParticle(p);
    }
    updateParticles();
}

void VectorFields::addParticles(int count) {
    for (int i = 0; i < count; ++i) {
        FieldParticle p;
        spawnParticle(p);
        p.age = 0.0f;
        m_particles.push_back(p);
    }
    updateParticles();
}

void VectorFields::setParticleSpeed(float speed) {
    m_config.particleSpeed = speed;
}

void VectorFields::setParticleLifetime(float lifetime) {
    m_config.particleLifetime = lifetime;
}

void VectorFields::setShowVectors(bool show) {
    m_config.showVectors = show;
}

void VectorFields::setShowParticles(bool show) {
    m_config.showParticles = show;
}

void VectorFields::setArrowScale(float scale) {
    m_config.arrowScale = scale;
}

// Presets
void VectorFields::presetRotation() {
    m_config.fieldType = FieldType::Rotation;
    m_config.fieldStrength = 1.0f;
    m_config.centerX = 0.0f;
    m_config.centerY = 0.0f;
    updateFieldGrid();
    resetParticles();
}

void VectorFields::presetSource() {
    m_config.fieldType = FieldType::Source;
    m_config.fieldStrength = 1.0f;
    m_config.centerX = 0.0f;
    m_config.centerY = 0.0f;
    updateFieldGrid();
    resetParticles();
}

void VectorFields::presetSink() {
    m_config.fieldType = FieldType::Sink;
    m_config.fieldStrength = 1.0f;
    m_config.centerX = 0.0f;
    m_config.centerY = 0.0f;
    updateFieldGrid();
    resetParticles();
}

void VectorFields::presetDipole() {
    m_config.fieldType = FieldType::Dipole;
    m_config.fieldStrength = 1.0f;
    m_config.centerX = 0.0f;
    m_config.centerY = 0.0f;
    m_config.dipoleDistance = 3.0f;
    updateFieldGrid();
    resetParticles();
}

void VectorFields::presetSaddle() {
    m_config.fieldType = FieldType::Saddle;
    m_config.fieldStrength = 0.5f;
    m_config.centerX = 0.0f;
    m_config.centerY = 0.0f;
    updateFieldGrid();
    resetParticles();
}

void VectorFields::presetSinCos() {
    m_config.fieldType = FieldType::SinCos;
    m_config.fieldStrength = 1.0f;
    m_config.frequency = 1.0f;
    updateFieldGrid();
    resetParticles();
}

void VectorFields::presetUniform() {
    m_config.fieldType = FieldType::Uniform;
    m_config.fieldStrength = 1.0f;
    updateFieldGrid();
    resetParticles();
}

void VectorFields::presetVortexPair() {
    // This will be handled as a special case with two rotation centers
    m_config.fieldType = FieldType::Dipole;  // Approximate with dipole for now
    m_config.fieldStrength = 1.0f;
    m_config.dipoleDistance = 4.0f;
    updateFieldGrid();
    resetParticles();
}

VectorFieldStats VectorFields::getStats() const {
    VectorFieldStats stats = {};

    stats.particleCount = static_cast<int>(m_particles.size());
    stats.simTime = m_simTime;

    // Compute field statistics
    float maxMag = 0.0f;
    float totalMag = 0.0f;
    int count = 0;

    int n = m_config.gridWidth * m_config.gridHeight;
    for (int i = 0; i < n; ++i) {
        float vx = m_fieldData[i * 2];
        float vy = m_fieldData[i * 2 + 1];
        float mag = std::sqrt(vx * vx + vy * vy);
        maxMag = std::max(maxMag, mag);
        totalMag += mag;
        count++;
    }

    stats.maxMagnitude = maxMag;
    stats.avgMagnitude = count > 0 ? totalMag / count : 0.0f;

    // Approximate divergence at center: div F = dFx/dx + dFy/dy
    float vx1, vy1, vx2, vy2;
    float h = 0.1f;
    evaluateFieldAt(m_config.centerX + h, m_config.centerY, vx1, vy1);
    evaluateFieldAt(m_config.centerX - h, m_config.centerY, vx2, vy2);
    float dFxdx = (vx1 - vx2) / (2.0f * h);

    evaluateFieldAt(m_config.centerX, m_config.centerY + h, vx1, vy1);
    evaluateFieldAt(m_config.centerX, m_config.centerY - h, vx2, vy2);
    float dFydy = (vy1 - vy2) / (2.0f * h);

    stats.divergence = dFxdx + dFydy;

    // Approximate curl at center (2D): curl F = dFy/dx - dFx/dy
    evaluateFieldAt(m_config.centerX + h, m_config.centerY, vx1, vy1);
    evaluateFieldAt(m_config.centerX - h, m_config.centerY, vx2, vy2);
    float dFydx = (vy1 - vy2) / (2.0f * h);

    evaluateFieldAt(m_config.centerX, m_config.centerY + h, vx1, vy1);
    evaluateFieldAt(m_config.centerX, m_config.centerY - h, vx2, vy2);
    float dFxdy = (vx1 - vx2) / (2.0f * h);

    stats.curl = dFydx - dFxdy;

    return stats;
}
