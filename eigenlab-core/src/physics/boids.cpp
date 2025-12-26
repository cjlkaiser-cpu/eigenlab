/**
 * @file boids.cpp
 * @brief Implementation of Boids flocking simulation
 *
 * Based on Craig Reynolds' original algorithm with modern optimizations
 */

#include "../../include/physics/boids.hpp"
#include <cmath>
#include <algorithm>

namespace eigenlab {
namespace physics {

// ============================================================================
// Constructor
// ============================================================================

BoidSystem::BoidSystem()
    : m_rng(std::random_device{}())
{
    m_boids.reserve(m_config.maxBoids);
    m_positions.reserve(m_config.maxBoids);
    m_velocities.reserve(m_config.maxBoids);
}

BoidSystem::BoidSystem(const BoidConfig& config)
    : m_config(config)
    , m_rng(std::random_device{}())
{
    m_boids.reserve(config.maxBoids);
    m_positions.reserve(config.maxBoids);
    m_velocities.reserve(config.maxBoids);

    // Calculate grid dimensions
    Real width = config.bounds.max.x - config.bounds.min.x;
    Real height = config.bounds.max.y - config.bounds.min.y;
    m_gridWidth = static_cast<u32>(std::ceil(width / config.spatialGridCellSize));
    m_gridHeight = static_cast<u32>(std::ceil(height / config.spatialGridCellSize));
}

void BoidSystem::setConfig(const BoidConfig& config) {
    m_config = config;

    Real width = config.bounds.max.x - config.bounds.min.x;
    Real height = config.bounds.max.y - config.bounds.min.y;
    m_gridWidth = static_cast<u32>(std::ceil(width / config.spatialGridCellSize));
    m_gridHeight = static_cast<u32>(std::ceil(height / config.spatialGridCellSize));
}

// ============================================================================
// Initialization
// ============================================================================

void BoidSystem::clear() {
    m_boids.clear();
    m_predators.clear();
    m_positions.clear();
    m_velocities.clear();
    m_spatialGrid.clear();
    m_stats = BoidStats{};
}

void BoidSystem::initializeRandom(u32 count) {
    clear();

    Real width = m_config.bounds.max.x - m_config.bounds.min.x;
    Real height = m_config.bounds.max.y - m_config.bounds.min.y;

    for (u32 i = 0; i < count && m_boids.size() < m_config.maxBoids; ++i) {
        Boid b;
        b.position.x = m_config.bounds.min.x + m_uniformDist(m_rng) * width;
        b.position.y = m_config.bounds.min.y + m_uniformDist(m_rng) * height;

        // Random direction, speed between min and max
        Real angle = m_uniformDist(m_rng) * 2.0f * constants::PI;
        Real speed = m_config.minSpeed + m_uniformDist(m_rng) * (m_config.maxSpeed - m_config.minSpeed);
        b.velocity.x = std::cos(angle) * speed;
        b.velocity.y = std::sin(angle) * speed;

        b.acceleration = {0, 0};
        b.hue = m_uniformDist(m_rng) * 60.0f + 180.0f; // Blue-cyan range
        b.species = 0;
        b.active = true;

        m_boids.push_back(b);
    }

    updatePackedArrays();
}

void BoidSystem::initializeCircle(u32 count, Vec2 center, Real radius) {
    clear();

    for (u32 i = 0; i < count && m_boids.size() < m_config.maxBoids; ++i) {
        Boid b;

        // Random position in circle
        Real r = radius * std::sqrt(m_uniformDist(m_rng));
        Real theta = m_uniformDist(m_rng) * 2.0f * constants::PI;
        b.position.x = center.x + r * std::cos(theta);
        b.position.y = center.y + r * std::sin(theta);

        // Tangential velocity (circular motion initially)
        Real speed = m_config.minSpeed + m_uniformDist(m_rng) * (m_config.maxSpeed - m_config.minSpeed);
        b.velocity.x = -std::sin(theta) * speed;
        b.velocity.y = std::cos(theta) * speed;

        b.acceleration = {0, 0};
        b.hue = m_uniformDist(m_rng) * 60.0f + 180.0f;
        b.active = true;

        m_boids.push_back(b);
    }

    updatePackedArrays();
}

void BoidSystem::initializeGrid(u32 count) {
    clear();

    Real width = m_config.bounds.max.x - m_config.bounds.min.x;
    Real height = m_config.bounds.max.y - m_config.bounds.min.y;

    u32 cols = static_cast<u32>(std::sqrt(count * width / height));
    u32 rows = (count + cols - 1) / cols;

    Real spacingX = width / (cols + 1);
    Real spacingY = height / (rows + 1);

    u32 added = 0;
    for (u32 row = 0; row < rows && added < count; ++row) {
        for (u32 col = 0; col < cols && added < count; ++col) {
            if (m_boids.size() >= m_config.maxBoids) break;

            Boid b;
            b.position.x = m_config.bounds.min.x + spacingX * (col + 1);
            b.position.y = m_config.bounds.min.y + spacingY * (row + 1);

            // Random initial velocity
            Real angle = m_uniformDist(m_rng) * 2.0f * constants::PI;
            Real speed = m_config.minSpeed + m_uniformDist(m_rng) * (m_config.maxSpeed - m_config.minSpeed);
            b.velocity.x = std::cos(angle) * speed;
            b.velocity.y = std::sin(angle) * speed;

            b.acceleration = {0, 0};
            b.hue = 200.0f + (static_cast<Real>(row) / rows) * 60.0f;
            b.active = true;

            m_boids.push_back(b);
            ++added;
        }
    }

    updatePackedArrays();
}

// ============================================================================
// Add/Remove entities
// ============================================================================

u32 BoidSystem::addBoid(Vec2 pos, Vec2 vel, u32 species) {
    if (m_boids.size() >= m_config.maxBoids) {
        return UINT32_MAX;
    }

    Boid b;
    b.position = pos;
    b.velocity = vel;
    b.acceleration = {0, 0};
    b.species = species;
    b.hue = m_uniformDist(m_rng) * 60.0f + 180.0f;
    b.active = true;

    m_boids.push_back(b);
    updatePackedArrays();
    return static_cast<u32>(m_boids.size() - 1);
}

u32 BoidSystem::addPredator(Vec2 pos, Vec2 vel) {
    Predator p;
    p.position = pos;
    p.velocity = vel;
    p.acceleration = {0, 0};
    p.kills = 0;
    p.active = true;

    m_predators.push_back(p);
    return static_cast<u32>(m_predators.size() - 1);
}

u32 BoidSystem::addObstacle(Vec2 pos, Real radius) {
    Obstacle o;
    o.position = pos;
    o.radius = radius;
    o.active = true;

    m_obstacles.push_back(o);
    return static_cast<u32>(m_obstacles.size() - 1);
}

u32 BoidSystem::addAttractor(Vec2 pos, Real strength, Real radius) {
    Attractor a;
    a.position = pos;
    a.strength = strength;
    a.radius = radius;
    a.active = true;

    m_attractors.push_back(a);
    return static_cast<u32>(m_attractors.size() - 1);
}

void BoidSystem::removeBoid(u32 index) {
    if (index < m_boids.size()) {
        m_boids[index].active = false;
    }
}

void BoidSystem::removePredator(u32 index) {
    if (index < m_predators.size()) {
        m_predators[index].active = false;
    }
}

void BoidSystem::removeObstacle(u32 index) {
    if (index < m_obstacles.size()) {
        m_obstacles[index].active = false;
    }
}

void BoidSystem::removeAttractor(u32 index) {
    if (index < m_attractors.size()) {
        m_attractors[index].active = false;
    }
}

void BoidSystem::clearPredators() {
    m_predators.clear();
}

void BoidSystem::clearObstacles() {
    m_obstacles.clear();
}

void BoidSystem::clearAttractors() {
    m_attractors.clear();
}

// ============================================================================
// Spatial hashing
// ============================================================================

i64 BoidSystem::cellKey(i32 cx, i32 cy) const {
    return (static_cast<i64>(cy) << 32) | (static_cast<i64>(cx) & 0xFFFFFFFF);
}

void BoidSystem::getCellCoords(const Vec2& pos, i32& cx, i32& cy) const {
    cx = static_cast<i32>((pos.x - m_config.bounds.min.x) / m_config.spatialGridCellSize);
    cy = static_cast<i32>((pos.y - m_config.bounds.min.y) / m_config.spatialGridCellSize);
}

void BoidSystem::buildSpatialGrid() {
    // Clear all cells
    for (auto& pair : m_spatialGrid) {
        pair.second.clear();
    }

    // Insert all active boids
    for (u32 i = 0; i < m_boids.size(); ++i) {
        if (!m_boids[i].active) continue;

        i32 cx, cy;
        getCellCoords(m_boids[i].position, cx, cy);
        m_spatialGrid[cellKey(cx, cy)].add(i);
    }
}

std::vector<u32> BoidSystem::getNeighbors(u32 boidIndex) const {
    std::vector<u32> neighbors;
    const Boid& boid = m_boids[boidIndex];

    i32 cx, cy;
    getCellCoords(boid.position, cx, cy);

    // Check 3x3 grid of cells
    for (i32 dy = -1; dy <= 1; ++dy) {
        for (i32 dx = -1; dx <= 1; ++dx) {
            auto it = m_spatialGrid.find(cellKey(cx + dx, cy + dy));
            if (it == m_spatialGrid.end()) continue;

            for (u32 otherIdx : it->second.boidIndices) {
                if (otherIdx == boidIndex) continue;
                if (!m_boids[otherIdx].active) continue;

                const Boid& other = m_boids[otherIdx];

                // Distance check
                Real dx2 = other.position.x - boid.position.x;
                Real dy2 = other.position.y - boid.position.y;
                Real distSq = dx2 * dx2 + dy2 * dy2;

                if (distSq < m_config.perceptionRadius * m_config.perceptionRadius) {
                    // Field of view check
                    if (isInFieldOfView(boid, other.position)) {
                        neighbors.push_back(otherIdx);
                    }
                }
            }
        }
    }

    return neighbors;
}

bool BoidSystem::isInFieldOfView(const Boid& boid, const Vec2& target) const {
    if (m_config.fieldOfView >= 360.0f) return true;

    Vec2 toTarget = {target.x - boid.position.x, target.y - boid.position.y};
    Real targetLen = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
    if (targetLen < 0.0001f) return true;

    Real velLen = std::sqrt(boid.velocity.x * boid.velocity.x + boid.velocity.y * boid.velocity.y);
    if (velLen < 0.0001f) return true;

    // Dot product gives cos(angle)
    Real dot = (toTarget.x * boid.velocity.x + toTarget.y * boid.velocity.y) / (targetLen * velLen);
    Real halfFOV = (m_config.fieldOfView / 2.0f) * (constants::PI / 180.0f);

    return dot >= std::cos(halfFOV);
}

// ============================================================================
// Steering behaviors
// ============================================================================

Vec2 BoidSystem::computeSeparation(u32 index, const std::vector<u32>& neighbors) {
    Vec2 steer = {0, 0};
    u32 count = 0;
    const Boid& boid = m_boids[index];

    for (u32 otherIdx : neighbors) {
        const Boid& other = m_boids[otherIdx];

        Real dx = boid.position.x - other.position.x;
        Real dy = boid.position.y - other.position.y;
        Real distSq = dx * dx + dy * dy;

        if (distSq < m_config.separationRadius * m_config.separationRadius && distSq > 0) {
            Real dist = std::sqrt(distSq);

            // Weight by inverse distance
            steer.x += (dx / dist) / dist;
            steer.y += (dy / dist) / dist;
            ++count;
        }
    }

    if (count > 0) {
        steer.x /= count;
        steer.y /= count;

        // Steering = desired - velocity
        Real mag = std::sqrt(steer.x * steer.x + steer.y * steer.y);
        if (mag > 0) {
            steer.x = (steer.x / mag) * m_config.maxSpeed - boid.velocity.x;
            steer.y = (steer.y / mag) * m_config.maxSpeed - boid.velocity.y;
            steer = limit(steer, m_config.maxForce);
        }
    }

    return steer;
}

Vec2 BoidSystem::computeAlignment(u32 index, const std::vector<u32>& neighbors) {
    Vec2 avgVel = {0, 0};

    if (neighbors.empty()) return avgVel;

    for (u32 otherIdx : neighbors) {
        avgVel.x += m_boids[otherIdx].velocity.x;
        avgVel.y += m_boids[otherIdx].velocity.y;
    }

    avgVel.x /= neighbors.size();
    avgVel.y /= neighbors.size();

    // Steering = desired - velocity
    const Boid& boid = m_boids[index];
    Vec2 steer = setMagnitude(avgVel, m_config.maxSpeed);
    steer.x -= boid.velocity.x;
    steer.y -= boid.velocity.y;

    return limit(steer, m_config.maxForce);
}

Vec2 BoidSystem::computeCohesion(u32 index, const std::vector<u32>& neighbors) {
    Vec2 center = {0, 0};

    if (neighbors.empty()) return center;

    for (u32 otherIdx : neighbors) {
        center.x += m_boids[otherIdx].position.x;
        center.y += m_boids[otherIdx].position.y;
    }

    center.x /= neighbors.size();
    center.y /= neighbors.size();

    // Seek toward center
    const Boid& boid = m_boids[index];
    Vec2 desired = {center.x - boid.position.x, center.y - boid.position.y};
    desired = setMagnitude(desired, m_config.maxSpeed);

    Vec2 steer = {desired.x - boid.velocity.x, desired.y - boid.velocity.y};
    return limit(steer, m_config.maxForce);
}

Vec2 BoidSystem::computeWallAvoidance(const Boid& boid) {
    Vec2 steer = {0, 0};
    Real margin = m_config.wallMargin;

    // Left wall
    if (boid.position.x < m_config.bounds.min.x + margin) {
        Real strength = (margin - (boid.position.x - m_config.bounds.min.x)) / margin;
        steer.x += m_config.maxSpeed * strength;
    }
    // Right wall
    if (boid.position.x > m_config.bounds.max.x - margin) {
        Real strength = (margin - (m_config.bounds.max.x - boid.position.x)) / margin;
        steer.x -= m_config.maxSpeed * strength;
    }
    // Top wall
    if (boid.position.y < m_config.bounds.min.y + margin) {
        Real strength = (margin - (boid.position.y - m_config.bounds.min.y)) / margin;
        steer.y += m_config.maxSpeed * strength;
    }
    // Bottom wall
    if (boid.position.y > m_config.bounds.max.y - margin) {
        Real strength = (margin - (m_config.bounds.max.y - boid.position.y)) / margin;
        steer.y -= m_config.maxSpeed * strength;
    }

    return limit(steer, m_config.maxForce);
}

Vec2 BoidSystem::computeObstacleAvoidance(const Boid& boid) {
    Vec2 steer = {0, 0};

    for (const auto& obs : m_obstacles) {
        if (!obs.active) continue;

        Real dx = boid.position.x - obs.position.x;
        Real dy = boid.position.y - obs.position.y;
        Real dist = std::sqrt(dx * dx + dy * dy);

        Real avoidRadius = obs.radius + m_config.separationRadius;
        if (dist < avoidRadius && dist > 0) {
            Real strength = (avoidRadius - dist) / avoidRadius;
            steer.x += (dx / dist) * strength * m_config.maxSpeed;
            steer.y += (dy / dist) * strength * m_config.maxSpeed;
        }
    }

    return limit(steer, m_config.maxForce * 2.0f);
}

Vec2 BoidSystem::computeAttractorForce(const Boid& boid) {
    Vec2 force = {0, 0};

    for (const auto& attr : m_attractors) {
        if (!attr.active) continue;

        Real dx = attr.position.x - boid.position.x;
        Real dy = attr.position.y - boid.position.y;
        Real dist = std::sqrt(dx * dx + dy * dy);

        if (dist < attr.radius && dist > 0) {
            Real strength = attr.strength * (1.0f - dist / attr.radius);
            force.x += (dx / dist) * strength;
            force.y += (dy / dist) * strength;
        }
    }

    return limit(force, m_config.maxForce);
}

Vec2 BoidSystem::computePredatorAvoidance(const Boid& boid) {
    Vec2 steer = {0, 0};

    for (const auto& pred : m_predators) {
        if (!pred.active) continue;

        Real dx = boid.position.x - pred.position.x;
        Real dy = boid.position.y - pred.position.y;
        Real dist = std::sqrt(dx * dx + dy * dy);

        if (dist < m_predatorConfig.huntRadius && dist > 0) {
            Real strength = (m_predatorConfig.huntRadius - dist) / m_predatorConfig.huntRadius;
            strength *= 3.0f; // Strong avoidance
            steer.x += (dx / dist) * strength * m_config.maxSpeed;
            steer.y += (dy / dist) * strength * m_config.maxSpeed;
        }
    }

    return limit(steer, m_config.maxForce * 3.0f);
}

Vec2 BoidSystem::computeNoise() {
    Real angle = m_uniformDist(m_rng) * 2.0f * constants::PI;
    return {
        std::cos(angle) * m_config.maxForce * m_config.noiseWeight,
        std::sin(angle) * m_config.maxForce * m_config.noiseWeight
    };
}

// ============================================================================
// Update
// ============================================================================

void BoidSystem::updateBoid(u32 index, Real dt) {
    Boid& boid = m_boids[index];
    if (!boid.active) return;

    // Get neighbors
    std::vector<u32> neighbors = getNeighbors(index);
    boid.neighborCount = static_cast<u32>(neighbors.size());

    // Compute steering forces
    Vec2 separation = computeSeparation(index, neighbors);
    Vec2 alignment = computeAlignment(index, neighbors);
    Vec2 cohesion = computeCohesion(index, neighbors);
    Vec2 wallAvoid = computeWallAvoidance(boid);
    Vec2 obstacleAvoid = computeObstacleAvoidance(boid);
    Vec2 attractorForce = computeAttractorForce(boid);
    Vec2 predatorAvoid = computePredatorAvoidance(boid);
    Vec2 noise = computeNoise();

    // Combine forces
    boid.acceleration = {0, 0};
    boid.acceleration.x += separation.x * m_config.separationWeight;
    boid.acceleration.y += separation.y * m_config.separationWeight;
    boid.acceleration.x += alignment.x * m_config.alignmentWeight;
    boid.acceleration.y += alignment.y * m_config.alignmentWeight;
    boid.acceleration.x += cohesion.x * m_config.cohesionWeight;
    boid.acceleration.y += cohesion.y * m_config.cohesionWeight;
    boid.acceleration.x += wallAvoid.x * m_config.avoidWallsWeight;
    boid.acceleration.y += wallAvoid.y * m_config.avoidWallsWeight;
    boid.acceleration.x += obstacleAvoid.x * 2.0f;
    boid.acceleration.y += obstacleAvoid.y * 2.0f;
    boid.acceleration.x += attractorForce.x;
    boid.acceleration.y += attractorForce.y;
    boid.acceleration.x += predatorAvoid.x;
    boid.acceleration.y += predatorAvoid.y;
    boid.acceleration.x += noise.x;
    boid.acceleration.y += noise.y;

    // Update velocity
    boid.velocity.x += boid.acceleration.x;
    boid.velocity.y += boid.acceleration.y;

    // Clamp speed
    Real speed = std::sqrt(boid.velocity.x * boid.velocity.x + boid.velocity.y * boid.velocity.y);
    if (speed > m_config.maxSpeed) {
        boid.velocity.x = (boid.velocity.x / speed) * m_config.maxSpeed;
        boid.velocity.y = (boid.velocity.y / speed) * m_config.maxSpeed;
    } else if (speed < m_config.minSpeed && speed > 0) {
        boid.velocity.x = (boid.velocity.x / speed) * m_config.minSpeed;
        boid.velocity.y = (boid.velocity.y / speed) * m_config.minSpeed;
    }

    // Update position
    boid.position.x += boid.velocity.x * dt;
    boid.position.y += boid.velocity.y * dt;

    // Handle boundaries
    if (m_config.wrapEdges) {
        wrapPosition(boid.position);
    } else {
        constrainPosition(boid.position, boid.velocity);
    }

    // Update hue based on neighbors (visual effect)
    if (neighbors.size() > 0) {
        boid.hue = 200.0f + std::min(neighbors.size(), static_cast<size_t>(10)) * 8.0f;
    }
}

void BoidSystem::updatePredator(u32 index, Real dt) {
    Predator& pred = m_predators[index];
    if (!pred.active) return;

    // Find nearest boid
    Real nearestDist = m_predatorConfig.huntRadius;
    i32 nearestBoid = -1;

    for (u32 i = 0; i < m_boids.size(); ++i) {
        if (!m_boids[i].active) continue;

        Real dx = m_boids[i].position.x - pred.position.x;
        Real dy = m_boids[i].position.y - pred.position.y;
        Real dist = std::sqrt(dx * dx + dy * dy);

        if (dist < nearestDist) {
            nearestDist = dist;
            nearestBoid = static_cast<i32>(i);
        }
    }

    // Chase nearest boid
    pred.acceleration = {0, 0};
    if (nearestBoid >= 0) {
        Vec2 desired = {
            m_boids[nearestBoid].position.x - pred.position.x,
            m_boids[nearestBoid].position.y - pred.position.y
        };
        desired = setMagnitude(desired, m_predatorConfig.maxSpeed);

        Vec2 steer = {desired.x - pred.velocity.x, desired.y - pred.velocity.y};
        steer = limit(steer, m_predatorConfig.maxForce);

        pred.acceleration.x = steer.x * m_predatorConfig.chaseWeight;
        pred.acceleration.y = steer.y * m_predatorConfig.chaseWeight;

        // Check for kill
        if (nearestDist < m_predatorConfig.killRadius) {
            m_boids[nearestBoid].active = false;
            pred.kills++;
            m_stats.kills++;
        }
    }

    // Update velocity
    pred.velocity.x += pred.acceleration.x;
    pred.velocity.y += pred.acceleration.y;

    Real speed = std::sqrt(pred.velocity.x * pred.velocity.x + pred.velocity.y * pred.velocity.y);
    if (speed > m_predatorConfig.maxSpeed) {
        pred.velocity.x = (pred.velocity.x / speed) * m_predatorConfig.maxSpeed;
        pred.velocity.y = (pred.velocity.y / speed) * m_predatorConfig.maxSpeed;
    }

    // Update position
    pred.position.x += pred.velocity.x * dt;
    pred.position.y += pred.velocity.y * dt;

    // Boundary handling
    if (m_config.wrapEdges) {
        wrapPosition(pred.position);
    } else {
        constrainPosition(pred.position, pred.velocity);
    }
}

void BoidSystem::step(Real dt) {
    // Build spatial hash
    buildSpatialGrid();

    // Update all boids
    for (u32 i = 0; i < m_boids.size(); ++i) {
        updateBoid(i, dt);
    }

    // Update predators
    for (u32 i = 0; i < m_predators.size(); ++i) {
        updatePredator(i, dt);
    }

    // Update packed arrays for JavaScript
    updatePackedArrays();

    m_stats.simulationTime += dt;
}

void BoidSystem::stepMultiple(Real dt, u32 steps) {
    for (u32 i = 0; i < steps; ++i) {
        step(dt);
    }
}

// ============================================================================
// Helper functions
// ============================================================================

Vec2 BoidSystem::limit(Vec2 v, Real maxMag) const {
    Real magSq = v.x * v.x + v.y * v.y;
    if (magSq > maxMag * maxMag) {
        Real mag = std::sqrt(magSq);
        v.x = (v.x / mag) * maxMag;
        v.y = (v.y / mag) * maxMag;
    }
    return v;
}

Vec2 BoidSystem::setMagnitude(Vec2 v, Real mag) const {
    Real currentMag = std::sqrt(v.x * v.x + v.y * v.y);
    if (currentMag > 0.0001f) {
        v.x = (v.x / currentMag) * mag;
        v.y = (v.y / currentMag) * mag;
    }
    return v;
}

void BoidSystem::wrapPosition(Vec2& pos) {
    Real width = m_config.bounds.max.x - m_config.bounds.min.x;
    Real height = m_config.bounds.max.y - m_config.bounds.min.y;

    if (pos.x < m_config.bounds.min.x) pos.x += width;
    if (pos.x > m_config.bounds.max.x) pos.x -= width;
    if (pos.y < m_config.bounds.min.y) pos.y += height;
    if (pos.y > m_config.bounds.max.y) pos.y -= height;
}

void BoidSystem::constrainPosition(Vec2& pos, Vec2& vel) {
    if (pos.x < m_config.bounds.min.x) {
        pos.x = m_config.bounds.min.x;
        vel.x = std::abs(vel.x);
    }
    if (pos.x > m_config.bounds.max.x) {
        pos.x = m_config.bounds.max.x;
        vel.x = -std::abs(vel.x);
    }
    if (pos.y < m_config.bounds.min.y) {
        pos.y = m_config.bounds.min.y;
        vel.y = std::abs(vel.y);
    }
    if (pos.y > m_config.bounds.max.y) {
        pos.y = m_config.bounds.max.y;
        vel.y = -std::abs(vel.y);
    }
}

void BoidSystem::updatePackedArrays() {
    m_positions.clear();
    m_velocities.clear();
    m_positions.reserve(m_boids.size());
    m_velocities.reserve(m_boids.size());

    for (const auto& b : m_boids) {
        if (b.active) {
            m_positions.push_back(b.position);
            m_velocities.push_back(b.velocity);
        }
    }
}

// ============================================================================
// Statistics
// ============================================================================

void BoidSystem::computeStatistics() {
    m_stats.activeBoids = 0;
    m_stats.activePredators = 0;
    m_stats.averageSpeed = 0;
    m_stats.averageNeighbors = 0;
    m_stats.flockCenter = {0, 0};
    m_stats.flockSpread = 0;
    m_stats.polarization = 0;

    Vec2 avgVel = {0, 0};

    for (const auto& b : m_boids) {
        if (!b.active) continue;
        m_stats.activeBoids++;

        Real speed = std::sqrt(b.velocity.x * b.velocity.x + b.velocity.y * b.velocity.y);
        m_stats.averageSpeed += speed;
        m_stats.averageNeighbors += b.neighborCount;

        m_stats.flockCenter.x += b.position.x;
        m_stats.flockCenter.y += b.position.y;

        avgVel.x += b.velocity.x;
        avgVel.y += b.velocity.y;
    }

    if (m_stats.activeBoids > 0) {
        m_stats.averageSpeed /= m_stats.activeBoids;
        m_stats.averageNeighbors /= m_stats.activeBoids;
        m_stats.flockCenter.x /= m_stats.activeBoids;
        m_stats.flockCenter.y /= m_stats.activeBoids;

        avgVel.x /= m_stats.activeBoids;
        avgVel.y /= m_stats.activeBoids;

        // Polarization: magnitude of average velocity / average speed
        Real avgVelMag = std::sqrt(avgVel.x * avgVel.x + avgVel.y * avgVel.y);
        if (m_stats.averageSpeed > 0) {
            m_stats.polarization = avgVelMag / m_stats.averageSpeed;
        }

        // Flock spread (standard deviation)
        Real variance = 0;
        for (const auto& b : m_boids) {
            if (!b.active) continue;
            Real dx = b.position.x - m_stats.flockCenter.x;
            Real dy = b.position.y - m_stats.flockCenter.y;
            variance += dx * dx + dy * dy;
        }
        m_stats.flockSpread = std::sqrt(variance / m_stats.activeBoids);
    }

    for (const auto& p : m_predators) {
        if (p.active) m_stats.activePredators++;
    }
}

// ============================================================================
// Presets
// ============================================================================

namespace boid_presets {

BoidConfig birds() {
    BoidConfig c;
    c.maxSpeed = 5.0f;
    c.minSpeed = 2.0f;
    c.maxForce = 0.15f;
    c.perceptionRadius = 60.0f;
    c.separationRadius = 30.0f;
    c.fieldOfView = 270.0f;
    c.separationWeight = 1.5f;
    c.alignmentWeight = 1.2f;
    c.cohesionWeight = 1.0f;
    c.noiseWeight = 0.1f;
    return c;
}

BoidConfig fish() {
    BoidConfig c;
    c.maxSpeed = 3.5f;
    c.minSpeed = 1.5f;
    c.maxForce = 0.12f;
    c.perceptionRadius = 40.0f;
    c.separationRadius = 20.0f;
    c.fieldOfView = 300.0f;
    c.separationWeight = 2.0f;  // Fish stay very close but maintain distance
    c.alignmentWeight = 1.5f;    // Strong alignment
    c.cohesionWeight = 1.5f;     // Very cohesive
    c.noiseWeight = 0.05f;
    return c;
}

BoidConfig insects() {
    BoidConfig c;
    c.maxSpeed = 7.0f;
    c.minSpeed = 3.0f;
    c.maxForce = 0.25f;
    c.perceptionRadius = 35.0f;
    c.separationRadius = 15.0f;
    c.fieldOfView = 360.0f;
    c.separationWeight = 1.0f;
    c.alignmentWeight = 0.5f;
    c.cohesionWeight = 0.8f;
    c.noiseWeight = 0.4f;  // Very erratic
    return c;
}

BoidConfig slow() {
    BoidConfig c;
    c.maxSpeed = 2.0f;
    c.minSpeed = 0.5f;
    c.maxForce = 0.05f;
    c.perceptionRadius = 80.0f;
    c.separationRadius = 40.0f;
    c.fieldOfView = 360.0f;
    c.separationWeight = 1.0f;
    c.alignmentWeight = 0.8f;
    c.cohesionWeight = 0.6f;
    c.noiseWeight = 0.02f;
    return c;
}

BoidConfig chaos() {
    BoidConfig c;
    c.maxSpeed = 6.0f;
    c.minSpeed = 2.0f;
    c.maxForce = 0.3f;
    c.perceptionRadius = 30.0f;
    c.separationRadius = 20.0f;
    c.fieldOfView = 180.0f;  // Limited vision
    c.separationWeight = 2.0f;
    c.alignmentWeight = 0.3f;
    c.cohesionWeight = 0.2f;
    c.noiseWeight = 0.6f;  // Maximum chaos
    return c;
}

} // namespace boid_presets

} // namespace physics
} // namespace eigenlab
