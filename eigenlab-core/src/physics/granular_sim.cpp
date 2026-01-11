#include "../../include/physics/granular_sim.hpp"
#include <cmath>
#include <algorithm>

namespace eigenlab {
namespace physics {

GranularSimulator::GranularSimulator()
    : numParticles_(0)
    , terrainWidth_(200)
    , gridWidth_(50)
    , gridHeight_(40)
    , cellSize_(2.0f)
    , spawnX_(0), spawnY_(0), spawnRate_(0)
    , spawnAccum_(0), spawning_(false)
    , rng_(std::random_device{}())
{
    setConfig(GranularConfig{});
}

GranularSimulator::GranularSimulator(const GranularConfig& config)
    : numParticles_(0)
    , rng_(std::random_device{}())
    , spawnX_(0), spawnY_(0), spawnRate_(0)
    , spawnAccum_(0), spawning_(false)
{
    setConfig(config);
}

void GranularSimulator::setConfig(const GranularConfig& config) {
    config_ = config;

    // Setup terrain
    terrainWidth_ = static_cast<int>(config.boxWidth * 2);
    terrain_.resize(terrainWidth_, 0.0f);

    // Setup spatial hash
    cellSize_ = config.particleRadius * 4.0f;
    gridWidth_ = static_cast<int>(config.boxWidth / cellSize_) + 1;
    gridHeight_ = static_cast<int>(config.boxHeight / cellSize_) + 1;
    grid_.resize(gridWidth_ * gridHeight_);

    particles_.reserve(config.numParticles);
    positions_.reserve(config.numParticles * 2);
    velocities_.reserve(config.numParticles * 2);

    reset();
}

void GranularSimulator::reset() {
    particles_.clear();
    numParticles_ = 0;
    spawnAccum_ = 0;
    spawning_ = false;

    // Default terrain: flat ground
    std::fill(terrain_.begin(), terrain_.end(), 5.0f);

    updateDataBuffers();
}

void GranularSimulator::clear() {
    particles_.clear();
    numParticles_ = 0;
    std::fill(terrain_.begin(), terrain_.end(), 5.0f);
    updateDataBuffers();
}

// ==================================================
// Simulation
// ==================================================

void GranularSimulator::step() {
    float dt = config_.dt;

    // Handle continuous spawning
    if (spawning_ && spawnRate_ > 0) {
        spawnAccum_ += spawnRate_ * dt;
        while (spawnAccum_ >= 1.0f && numParticles_ < config_.numParticles) {
            spawnParticles(spawnX_, spawnY_, 1, 2.0f);
            spawnAccum_ -= 1.0f;
        }
    }

    // Apply gravity and integrate
    for (auto& p : particles_) {
        if (!p.active) continue;

        p.vy -= config_.gravity * dt;
        p.vx *= config_.damping;
        p.vy *= config_.damping;

        p.x += p.vx * dt;
        p.y += p.vy * dt;
    }

    // Build spatial hash and resolve collisions
    buildSpatialHash();
    resolveCollisions();

    // Terrain collisions
    for (auto& p : particles_) {
        if (!p.active) continue;
        resolveTerrainCollision(p);

        // Box boundaries
        if (p.x < p.radius) {
            p.x = p.radius;
            p.vx = -p.vx * config_.restitution;
        }
        if (p.x > config_.boxWidth - p.radius) {
            p.x = config_.boxWidth - p.radius;
            p.vx = -p.vx * config_.restitution;
        }
        if (p.y > config_.boxHeight - p.radius) {
            p.y = config_.boxHeight - p.radius;
            p.vy = -p.vy * config_.restitution;
        }
    }

    updateDataBuffers();
}

void GranularSimulator::stepMultiple(int steps) {
    for (int i = 0; i < steps; ++i) {
        step();
    }
    computeStatistics();
}

void GranularSimulator::buildSpatialHash() {
    for (auto& cell : grid_) {
        cell.clear();
    }

    for (int i = 0; i < static_cast<int>(particles_.size()); ++i) {
        const auto& p = particles_[i];
        if (!p.active) continue;

        int cx = static_cast<int>(p.x / cellSize_);
        int cy = static_cast<int>(p.y / cellSize_);

        cx = std::max(0, std::min(gridWidth_ - 1, cx));
        cy = std::max(0, std::min(gridHeight_ - 1, cy));

        grid_[cy * gridWidth_ + cx].push_back(i);
    }
}

void GranularSimulator::resolveCollisions() {
    // Check each cell and neighbors
    for (int cy = 0; cy < gridHeight_; ++cy) {
        for (int cx = 0; cx < gridWidth_; ++cx) {
            const auto& cell = grid_[cy * gridWidth_ + cx];

            for (int i : cell) {
                // Check same cell
                for (int j : cell) {
                    if (i >= j) continue;
                    resolveParticleCollision(particles_[i], particles_[j]);
                }

                // Check neighboring cells
                for (int dcy = 0; dcy <= 1; ++dcy) {
                    for (int dcx = -1; dcx <= 1; ++dcx) {
                        if (dcy == 0 && dcx <= 0) continue;

                        int ncx = cx + dcx;
                        int ncy = cy + dcy;

                        if (ncx < 0 || ncx >= gridWidth_) continue;
                        if (ncy >= gridHeight_) continue;

                        const auto& neighborCell = grid_[ncy * gridWidth_ + ncx];
                        for (int j : neighborCell) {
                            resolveParticleCollision(particles_[i], particles_[j]);
                        }
                    }
                }
            }
        }
    }
}

void GranularSimulator::resolveParticleCollision(Particle& p1, Particle& p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    float minDist = p1.radius + p2.radius;

    if (dist < minDist && dist > 0.001f) {
        // Normalize
        float nx = dx / dist;
        float ny = dy / dist;

        // Separate particles
        float overlap = minDist - dist;
        p1.x -= nx * overlap * 0.5f;
        p1.y -= ny * overlap * 0.5f;
        p2.x += nx * overlap * 0.5f;
        p2.y += ny * overlap * 0.5f;

        // Relative velocity
        float dvx = p2.vx - p1.vx;
        float dvy = p2.vy - p1.vy;
        float vn = dvx * nx + dvy * ny;

        // Only resolve if approaching
        if (vn < 0) {
            // Impulse
            float j = -(1.0f + config_.restitution) * vn * 0.5f;

            p1.vx -= j * nx;
            p1.vy -= j * ny;
            p2.vx += j * nx;
            p2.vy += j * ny;

            // Friction
            float normalForce = std::abs(vn);
            applyFriction(p1, -nx, -ny, normalForce);
            applyFriction(p2, nx, ny, normalForce);
        }
    }
}

void GranularSimulator::resolveTerrainCollision(Particle& p) {
    float terrainY = getTerrainHeight(p.x);

    if (p.y - p.radius < terrainY) {
        // Get terrain normal (approximate from gradient)
        float h1 = getTerrainHeight(p.x - 0.5f);
        float h2 = getTerrainHeight(p.x + 0.5f);
        float slope = h2 - h1;

        float nx = -slope;
        float ny = 1.0f;
        float len = std::sqrt(nx * nx + ny * ny);
        nx /= len;
        ny /= len;

        // Push out
        p.y = terrainY + p.radius;

        // Velocity relative to normal
        float vn = p.vx * nx + p.vy * ny;

        if (vn < 0) {
            // Reflect with restitution
            p.vx -= (1.0f + config_.restitution) * vn * nx;
            p.vy -= (1.0f + config_.restitution) * vn * ny;

            // Apply friction
            float normalForce = std::abs(vn) + config_.gravity * config_.dt;
            applyFriction(p, nx, ny, normalForce);
        }
    }
}

void GranularSimulator::applyFriction(Particle& p, float nx, float ny, float normalForce) {
    // Tangent direction
    float tx = -ny;
    float ty = nx;

    // Tangential velocity
    float vt = p.vx * tx + p.vy * ty;

    // Friction force
    float frictionCoef = std::abs(vt) < 0.1f ? config_.staticFriction : config_.dynamicFriction;
    float maxFriction = frictionCoef * normalForce;
    float frictionMag = std::min(std::abs(vt), maxFriction);

    if (std::abs(vt) > 0.001f) {
        float sign = vt > 0 ? -1.0f : 1.0f;
        p.vx += sign * frictionMag * tx;
        p.vy += sign * frictionMag * ty;
    }
}

float GranularSimulator::getTerrainHeight(float x) const {
    // Map x to terrain index
    float t = x / config_.boxWidth * (terrainWidth_ - 1);
    int i0 = static_cast<int>(t);
    int i1 = i0 + 1;

    i0 = std::max(0, std::min(terrainWidth_ - 1, i0));
    i1 = std::max(0, std::min(terrainWidth_ - 1, i1));

    float frac = t - i0;
    return terrain_[i0] * (1.0f - frac) + terrain_[i1] * frac;
}

// ==================================================
// Terrain
// ==================================================

void GranularSimulator::setTerrainHeight(int x, float height) {
    if (x >= 0 && x < terrainWidth_) {
        terrain_[x] = height;
    }
}

void GranularSimulator::clearTerrain() {
    std::fill(terrain_.begin(), terrain_.end(), 5.0f);
}

void GranularSimulator::addRamp(float x1, float y1, float x2, float y2) {
    int i1 = static_cast<int>(x1 / config_.boxWidth * (terrainWidth_ - 1));
    int i2 = static_cast<int>(x2 / config_.boxWidth * (terrainWidth_ - 1));

    i1 = std::max(0, std::min(terrainWidth_ - 1, i1));
    i2 = std::max(0, std::min(terrainWidth_ - 1, i2));

    if (i1 > i2) {
        std::swap(i1, i2);
        std::swap(y1, y2);
    }

    for (int i = i1; i <= i2; ++i) {
        float t = (i2 > i1) ? static_cast<float>(i - i1) / (i2 - i1) : 0.0f;
        float h = y1 * (1.0f - t) + y2 * t;
        terrain_[i] = std::max(terrain_[i], h);
    }
}

void GranularSimulator::addFunnel(float centerX, float topY, float width, float gapWidth) {
    float leftX = centerX - width / 2;
    float rightX = centerX + width / 2;
    float gapLeft = centerX - gapWidth / 2;
    float gapRight = centerX + gapWidth / 2;

    // Left slope
    addRamp(leftX, 5.0f, gapLeft, topY);
    // Right slope
    addRamp(gapRight, topY, rightX, 5.0f);
}

void GranularSimulator::addPile(float centerX, float baseY, float width, float height) {
    // Triangular pile
    addRamp(centerX - width / 2, baseY, centerX, baseY + height);
    addRamp(centerX, baseY + height, centerX + width / 2, baseY);
}

// ==================================================
// Spawning
// ==================================================

void GranularSimulator::spawnParticles(float x, float y, int count, float spread) {
    std::uniform_real_distribution<float> dist(-spread, spread);
    std::uniform_real_distribution<float> velDist(-2.0f, 2.0f);

    for (int i = 0; i < count && numParticles_ < config_.numParticles; ++i) {
        Particle p;
        p.x = x + dist(rng_);
        p.y = y + dist(rng_);
        p.vx = velDist(rng_);
        p.vy = velDist(rng_);
        p.radius = config_.particleRadius;
        p.active = true;

        particles_.push_back(p);
        numParticles_++;
    }
}

void GranularSimulator::spawnStream(float x, float y, float rate) {
    spawnX_ = x;
    spawnY_ = y;
    spawnRate_ = rate;
    spawning_ = rate > 0;
}

// ==================================================
// Parameters
// ==================================================

void GranularSimulator::setGravity(float g) {
    config_.gravity = g;
}

void GranularSimulator::setFriction(float staticF, float dynamicF) {
    config_.staticFriction = staticF;
    config_.dynamicFriction = dynamicF;
}

void GranularSimulator::setRestitution(float r) {
    config_.restitution = r;
}

// ==================================================
// Statistics
// ==================================================

void GranularSimulator::computeStatistics() {
    float ke = 0.0f;
    float sumSpeed = 0.0f;
    int active = 0;
    float maxY = 0.0f;

    for (const auto& p : particles_) {
        if (!p.active) continue;

        float speed = std::sqrt(p.vx * p.vx + p.vy * p.vy);
        ke += speed * speed;
        sumSpeed += speed;

        if (speed > 0.5f) active++;
        if (p.y > maxY) maxY = p.y;
    }

    stats_.numParticles = numParticles_;
    stats_.kineticEnergy = ke * 0.5f;
    stats_.avgSpeed = numParticles_ > 0 ? sumSpeed / numParticles_ : 0.0f;
    stats_.activeParticles = active;
    stats_.pileHeight = maxY;
}

void GranularSimulator::updateDataBuffers() {
    positions_.resize(particles_.size() * 2);
    velocities_.resize(particles_.size() * 2);

    for (size_t i = 0; i < particles_.size(); ++i) {
        positions_[i * 2] = particles_[i].x;
        positions_[i * 2 + 1] = particles_[i].y;
        velocities_[i * 2] = particles_[i].vx;
        velocities_[i * 2 + 1] = particles_[i].vy;
    }
}

// ==================================================
// Presets
// ==================================================

GranularConfig granularPresetSand() {
    GranularConfig config;
    config.numParticles = 3000;
    config.boxWidth = 100.0f;
    config.boxHeight = 80.0f;
    config.particleRadius = 0.35f;
    config.gravity = 25.0f;
    config.staticFriction = 0.7f;
    config.dynamicFriction = 0.5f;
    config.restitution = 0.1f;
    config.damping = 0.99f;
    return config;
}

GranularConfig granularPresetGravel() {
    GranularConfig config;
    config.numParticles = 2000;
    config.boxWidth = 100.0f;
    config.boxHeight = 80.0f;
    config.particleRadius = 0.6f;
    config.gravity = 30.0f;
    config.staticFriction = 0.6f;
    config.dynamicFriction = 0.4f;
    config.restitution = 0.25f;
    config.damping = 0.98f;
    return config;
}

GranularConfig granularPresetRocks() {
    GranularConfig config;
    config.numParticles = 800;
    config.boxWidth = 100.0f;
    config.boxHeight = 80.0f;
    config.particleRadius = 1.2f;
    config.gravity = 35.0f;
    config.staticFriction = 0.5f;
    config.dynamicFriction = 0.3f;
    config.restitution = 0.35f;
    config.damping = 0.97f;
    return config;
}

GranularConfig granularPresetAvalanche() {
    GranularConfig config;
    config.numParticles = 5000;
    config.boxWidth = 120.0f;
    config.boxHeight = 100.0f;
    config.particleRadius = 0.4f;
    config.gravity = 30.0f;
    config.staticFriction = 0.65f;
    config.dynamicFriction = 0.45f;
    config.restitution = 0.15f;
    config.damping = 0.99f;
    return config;
}

} // namespace physics
} // namespace eigenlab
