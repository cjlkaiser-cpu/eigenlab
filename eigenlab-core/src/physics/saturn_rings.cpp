#include "physics/saturn_rings.hpp"
#include <cmath>
#include <algorithm>

SaturnRings::SaturnRings()
    : m_rng(42), m_viewAngle(0.4f), m_rotation(0.0f), m_collisionCount(0) {
    init();
}

SaturnRings::SaturnRings(const SaturnRingsConfig& config)
    : m_config(config), m_rng(42), m_viewAngle(0.4f), m_rotation(0.0f), m_collisionCount(0) {
    init();
}

void SaturnRings::init() {
    m_collisionCount = 0;
    initParticles();
    updateArrays();
}

float SaturnRings::orbitalVelocity(float r) const {
    // v = sqrt(GM/r) for circular Keplerian orbit
    return std::sqrt(m_config.gravitationalConstant * m_config.saturnMass / r);
}

bool SaturnRings::isInGap(float r) const {
    float saturnR = m_config.saturnRadius;

    // Cassini Division: 1.95 - 2.02 Saturn radii
    if (m_config.enableCassiniDivision) {
        if (r > 1.95f * saturnR && r < 2.02f * saturnR) {
            return true;
        }
    }

    // Encke Gap: around 2.21 Saturn radii
    if (m_config.enableEnckGap) {
        if (r > 2.20f * saturnR && r < 2.22f * saturnR) {
            return true;
        }
    }

    return false;
}

void SaturnRings::initParticles() {
    m_particles.clear();
    m_particles.reserve(m_config.numParticles);

    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> prob(0.0f, 1.0f);
    std::normal_distribution<float> verticalDist(0.0f, m_config.ringThickness);

    float innerR = m_config.innerRingRadius * m_config.saturnRadius;
    float outerR = m_config.outerRingRadius * m_config.saturnRadius;

    int created = 0;
    int attempts = 0;
    int maxAttempts = m_config.numParticles * 10;

    while (created < m_config.numParticles && attempts < maxAttempts) {
        attempts++;

        // Sample radius with density variation (more particles in main rings)
        float r;
        float u = prob(m_rng);

        // Ring density profile: B ring (1.52-1.95) is densest
        if (u < 0.1f) {
            // D ring (sparse)
            r = innerR + prob(m_rng) * (1.52f * m_config.saturnRadius - innerR);
        } else if (u < 0.2f) {
            // C ring
            r = 1.52f * m_config.saturnRadius + prob(m_rng) * 0.2f * m_config.saturnRadius;
        } else if (u < 0.6f) {
            // B ring (dense)
            r = 1.72f * m_config.saturnRadius + prob(m_rng) * 0.23f * m_config.saturnRadius;
        } else {
            // A ring
            r = 2.02f * m_config.saturnRadius + prob(m_rng) * (outerR - 2.02f * m_config.saturnRadius);
        }

        // Skip gaps
        if (isInGap(r)) continue;

        // Check bounds
        if (r < innerR || r > outerR) continue;

        RingParticle p;

        // Random angle in orbital plane
        float angle = angleDist(m_rng);

        // Position
        p.x = r * std::cos(angle);
        p.y = r * std::sin(angle);
        p.z = verticalDist(m_rng);  // Slight vertical spread

        p.radius = r;

        // Keplerian velocity (circular orbit, counterclockwise)
        float v = orbitalVelocity(r);
        p.vx = -v * std::sin(angle);
        p.vy = v * std::cos(angle);
        p.vz = 0.0f;

        // Visual properties
        p.size = 0.01f + prob(m_rng) * 0.02f;
        p.brightness = 0.5f + prob(m_rng) * 0.5f;

        // Composition: mostly ice, some rock
        p.isIce = prob(m_rng) < 0.8f;

        m_particles.push_back(p);
        created++;
    }
}

void SaturnRings::updateArrays() {
    int n = static_cast<int>(m_particles.size());

    m_positions.resize(n * 3);
    m_colors.resize(n * 3);
    m_sizes.resize(n);

    for (int i = 0; i < n; ++i) {
        const RingParticle& p = m_particles[i];

        // Apply view rotation
        float cosR = std::cos(m_rotation);
        float sinR = std::sin(m_rotation);
        float x = p.x * cosR - p.y * sinR;
        float y = p.x * sinR + p.y * cosR;
        float z = p.z;

        // Apply tilt angle (view from above vs edge-on)
        float cosT = std::cos(m_viewAngle);
        float sinT = std::sin(m_viewAngle);
        float y2 = y * cosT - z * sinT;
        float z2 = y * sinT + z * cosT;

        m_positions[i * 3 + 0] = x;
        m_positions[i * 3 + 1] = y2;
        m_positions[i * 3 + 2] = z2;

        float r, g, b;
        computeParticleColor(p, r, g, b);
        m_colors[i * 3 + 0] = r;
        m_colors[i * 3 + 1] = g;
        m_colors[i * 3 + 2] = b;

        m_sizes[i] = p.size;
    }
}

void SaturnRings::computeParticleColor(const RingParticle& p,
                                        float& r, float& g, float& b) const {
    // Rings are mostly icy (bright) with some rocky (dark) particles
    float brightness = p.brightness;

    // Distance-based dimming (outer rings slightly darker)
    float distFactor = 1.0f - 0.2f * (p.radius - m_config.innerRingRadius) /
                                     (m_config.outerRingRadius - m_config.innerRingRadius);
    brightness *= distFactor;

    if (p.isIce) {
        // Ice: pale cream/white
        r = 0.9f * brightness;
        g = 0.85f * brightness;
        b = 0.8f * brightness;
    } else {
        // Rock: brownish gray
        r = 0.6f * brightness;
        g = 0.5f * brightness;
        b = 0.4f * brightness;
    }
}

void SaturnRings::applyGravity(RingParticle& p) {
    float r2 = p.x * p.x + p.y * p.y + p.z * p.z;
    float r = std::sqrt(r2);

    if (r < 0.1f * m_config.saturnRadius) {
        // Inside Saturn - remove particle
        return;
    }

    // Gravitational acceleration: a = -GM/r^2 * r_hat
    float a = m_config.gravitationalConstant * m_config.saturnMass / r2;

    p.vx -= a * p.x / r * m_config.dt;
    p.vy -= a * p.y / r * m_config.dt;
    p.vz -= a * p.z / r * m_config.dt;

    // Optional: shepherd moon perturbations (simplified)
    if (m_config.enableShepherdMoons) {
        // Mimas resonance (at ~3.08 Saturn radii, 2:1 resonance with B ring edge)
        float mimasR = 3.08f * m_config.saturnRadius;
        float mimasAngle = m_rotation * 0.5f;  // Slower than ring particles
        float mimasX = mimasR * std::cos(mimasAngle);
        float mimasY = mimasR * std::sin(mimasAngle);

        float dx = p.x - mimasX;
        float dy = p.y - mimasY;
        float dz = p.z;
        float moonDist2 = dx * dx + dy * dy + dz * dz;

        if (moonDist2 > 0.01f) {
            float moonMass = 0.00001f * m_config.saturnMass;  // Very small
            float moonA = m_config.gravitationalConstant * moonMass / moonDist2;
            float moonDist = std::sqrt(moonDist2);

            p.vx -= moonA * dx / moonDist * m_config.dt;
            p.vy -= moonA * dy / moonDist * m_config.dt;
            p.vz -= moonA * dz / moonDist * m_config.dt;
        }
    }
}

void SaturnRings::handleCollisions() {
    if (!m_config.enableCollisions) return;

    int n = static_cast<int>(m_particles.size());
    float collisionR = 0.05f * m_config.saturnRadius;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            RingParticle& p1 = m_particles[i];
            RingParticle& p2 = m_particles[j];

            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float dz = p2.z - p1.z;
            float dist2 = dx * dx + dy * dy + dz * dz;

            if (dist2 < collisionR * collisionR && dist2 > 0.0001f) {
                float dist = std::sqrt(dist2);

                // Elastic collision with damping
                float dvx = p2.vx - p1.vx;
                float dvy = p2.vy - p1.vy;
                float dvz = p2.vz - p1.vz;

                float dvDotDr = dvx * dx + dvy * dy + dvz * dz;
                float factor = m_config.collisionDamping * dvDotDr / dist2;

                p1.vx += factor * dx;
                p1.vy += factor * dy;
                p1.vz += factor * dz;

                p2.vx -= factor * dx;
                p2.vy -= factor * dy;
                p2.vz -= factor * dz;

                m_collisionCount++;
            }
        }
    }
}

void SaturnRings::updateOrbits() {
    for (auto& p : m_particles) {
        applyGravity(p);

        // Update positions
        p.x += p.vx * m_config.dt;
        p.y += p.vy * m_config.dt;
        p.z += p.vz * m_config.dt;

        // Update orbital radius
        p.radius = std::sqrt(p.x * p.x + p.y * p.y);

        // Vertical damping (particles settle back to disk plane)
        p.vz *= 0.999f;
        p.z *= 0.999f;
    }
}

void SaturnRings::step() {
    updateOrbits();
    handleCollisions();
    m_rotation += m_config.dt * 0.5f;  // Slow rotation for visualization
    updateArrays();
}

void SaturnRings::step(int numSteps) {
    for (int i = 0; i < numSteps; ++i) {
        updateOrbits();
        handleCollisions();
    }
    m_rotation += m_config.dt * 0.5f * numSteps;
    updateArrays();
}

void SaturnRings::setGravitationalConstant(float g) {
    m_config.gravitationalConstant = g;
}

void SaturnRings::enableCollisions(bool enable) {
    m_config.enableCollisions = enable;
}

void SaturnRings::enableShepherdMoons(bool enable) {
    m_config.enableShepherdMoons = enable;
}

void SaturnRings::setRingBounds(float inner, float outer) {
    m_config.innerRingRadius = inner;
    m_config.outerRingRadius = outer;
    init();
}

void SaturnRings::setViewAngle(float angle) {
    m_viewAngle = angle;
    updateArrays();
}

void SaturnRings::setRotation(float angle) {
    m_rotation = angle;
    updateArrays();
}

// Presets
void SaturnRings::presetRealistic() {
    m_config.numParticles = 5000;
    m_config.innerRingRadius = 1.2f;
    m_config.outerRingRadius = 2.3f;
    m_config.enableCassiniDivision = true;
    m_config.enableEnckGap = true;
    m_config.ringThickness = 0.02f;
    init();
}

void SaturnRings::presetDense() {
    m_config.numParticles = 10000;
    m_config.innerRingRadius = 1.5f;
    m_config.outerRingRadius = 2.5f;
    m_config.enableCassiniDivision = true;
    m_config.ringThickness = 0.03f;
    init();
}

void SaturnRings::presetWideRings() {
    m_config.numParticles = 6000;
    m_config.innerRingRadius = 1.1f;
    m_config.outerRingRadius = 3.0f;
    m_config.enableCassiniDivision = false;
    m_config.ringThickness = 0.05f;
    init();
}

void SaturnRings::presetThinRings() {
    m_config.numParticles = 3000;
    m_config.innerRingRadius = 1.7f;
    m_config.outerRingRadius = 2.0f;
    m_config.enableCassiniDivision = false;
    m_config.ringThickness = 0.01f;
    init();
}

SaturnRingsStats SaturnRings::getStats() const {
    SaturnRingsStats stats = {};

    if (m_particles.empty()) return stats;

    stats.particleCount = static_cast<int>(m_particles.size());

    float totalV = 0.0f, totalR = 0.0f;
    float minR = 1e10f, maxR = 0.0f;
    float totalE = 0.0f, totalL = 0.0f;

    for (const auto& p : m_particles) {
        float v2 = p.vx * p.vx + p.vy * p.vy + p.vz * p.vz;
        float v = std::sqrt(v2);
        float r = p.radius;

        totalV += v;
        totalR += r;
        minR = std::min(minR, r);
        maxR = std::max(maxR, r);

        // Energy: KE + PE = 0.5*v^2 - GM/r
        float ke = 0.5f * v2;
        float pe = -m_config.gravitationalConstant * m_config.saturnMass / r;
        totalE += ke + pe;

        // Angular momentum: L = r * v_tangential
        float L = r * std::sqrt(p.vx * p.vx + p.vy * p.vy);
        totalL += L;
    }

    stats.avgOrbitalVelocity = totalV / stats.particleCount;
    stats.avgRadius = totalR / stats.particleCount;
    stats.minRadius = minR;
    stats.maxRadius = maxR;
    stats.totalEnergy = totalE;
    stats.totalAngularMomentum = totalL;
    stats.collisionCount = m_collisionCount;

    return stats;
}
