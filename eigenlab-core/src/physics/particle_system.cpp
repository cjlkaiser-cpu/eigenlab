/**
 * @file particle_system.cpp
 * @brief High-performance particle system implementation
 */

#include "../../include/physics/particle_system.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace eigenlab {
namespace physics {

namespace {
    thread_local std::mt19937 g_rng{std::random_device{}()};
}

// ============================================================================
// Spatial Hash Implementation
// ============================================================================

SpatialHash::SpatialHash(Real cellSize)
    : m_cellSize(cellSize)
    , m_invCellSize(1.0f / cellSize)
{
}

void SpatialHash::clear() {
    m_cells.clear();
}

i32 SpatialHash::hashPosition(const Vec2& pos) const {
    auto [cx, cy] = getCellCoords(pos);
    // Simple hash combining
    return cx * 73856093 ^ cy * 19349663;
}

std::pair<i32, i32> SpatialHash::getCellCoords(const Vec2& pos) const {
    return {
        static_cast<i32>(std::floor(pos.x * m_invCellSize)),
        static_cast<i32>(std::floor(pos.y * m_invCellSize))
    };
}

void SpatialHash::insert(u32 particleIndex, const Vec2& position) {
    i32 hash = hashPosition(position);
    m_cells[hash].push_back(particleIndex);
}

void SpatialHash::rebuild(const std::vector<Particle>& particles) {
    clear();
    for (u32 i = 0; i < particles.size(); ++i) {
        if (particles[i].active) {
            insert(i, particles[i].position);
        }
    }
}

void SpatialHash::queryNear(const Vec2& position, Real radius,
                            std::vector<u32>& outIndices) const {
    outIndices.clear();

    i32 cellRadius = static_cast<i32>(std::ceil(radius * m_invCellSize));
    auto [cx, cy] = getCellCoords(position);

    for (i32 dx = -cellRadius; dx <= cellRadius; ++dx) {
        for (i32 dy = -cellRadius; dy <= cellRadius; ++dy) {
            i32 hash = (cx + dx) * 73856093 ^ (cy + dy) * 19349663;
            auto it = m_cells.find(hash);
            if (it != m_cells.end()) {
                for (u32 idx : it->second) {
                    outIndices.push_back(idx);
                }
            }
        }
    }
}

void SpatialHash::getPotentialPairs(std::vector<std::pair<u32, u32>>& outPairs) const {
    outPairs.clear();

    for (const auto& [hash, indices] : m_cells) {
        // Pairs within same cell
        for (usize i = 0; i < indices.size(); ++i) {
            for (usize j = i + 1; j < indices.size(); ++j) {
                outPairs.emplace_back(indices[i], indices[j]);
            }
        }

        // We also need to check neighboring cells to find all pairs
        // This is handled by queryNear in the main collision detection
    }
}

// ============================================================================
// Maxwell-Boltzmann utilities
// ============================================================================

namespace maxwell_boltzmann {

Real sampleSpeed(Real temperature, Real mass) {
    // Sample from Maxwell-Boltzmann speed distribution
    // Using Box-Muller for 2D (chi-2 with 2 degrees of freedom)
    std::normal_distribution<Real> normal{0.0f, 1.0f};

    Real sigma = std::sqrt(constants::BOLTZMANN * temperature / mass);
    Real vx = sigma * normal(g_rng);
    Real vy = sigma * normal(g_rng);

    return std::sqrt(vx * vx + vy * vy);
}

Real pdf(Real v, Real temperature, Real mass) {
    // 2D Maxwell-Boltzmann: f(v) = (m*v)/(kT) * exp(-mv²/(2kT))
    Real kT = constants::BOLTZMANN * temperature;
    Real a = mass / kT;
    return a * v * std::exp(-0.5f * a * v * v);
}

Real mostProbableSpeed(Real temperature, Real mass) {
    // v_p = sqrt(kT/m) for 2D
    return std::sqrt(constants::BOLTZMANN * temperature / mass);
}

Real meanSpeed(Real temperature, Real mass) {
    // <v> = sqrt(π*kT/(2m)) for 2D
    return std::sqrt(constants::PI * constants::BOLTZMANN * temperature / (2.0f * mass));
}

Real rmsSpeed(Real temperature, Real mass) {
    // v_rms = sqrt(2kT/m) for 2D
    return std::sqrt(2.0f * constants::BOLTZMANN * temperature / mass);
}

} // namespace maxwell_boltzmann

// ============================================================================
// Particle System Implementation
// ============================================================================

ParticleSystem::ParticleSystem()
    : m_spatialHash(10.0f)
{
    m_particles.reserve(1000);
    m_collisionPairs.reserve(10000);
    m_nearbyIndices.reserve(100);
}

ParticleSystem::ParticleSystem(const ParticleSystemConfig& config)
    : m_config(config)
    , m_spatialHash(config.particleRadius * config.cellSizeMultiplier)
{
    m_particles.reserve(config.maxParticles);
    m_collisionPairs.reserve(config.maxParticles * 10);
    m_nearbyIndices.reserve(100);
}

void ParticleSystem::setConfig(const ParticleSystemConfig& config) {
    m_config = config;
    m_spatialHash.setCellSize(config.particleRadius * config.cellSizeMultiplier);
}

void ParticleSystem::clear() {
    m_particles.clear();
    m_spatialHash.clear();
    m_stats = ParticleSystemStats{};
}

u32 ParticleSystem::addParticle(const Vec2& position, const Vec2& velocity) {
    if (m_particles.size() >= m_config.maxParticles) {
        return static_cast<u32>(-1);
    }

    Particle p;
    p.position = position;
    p.velocity = velocity;
    p.radius = m_config.particleRadius;
    p.mass = m_config.particleMass;
    p.active = true;

    u32 index = static_cast<u32>(m_particles.size());
    m_particles.push_back(p);
    return index;
}

u32 ParticleSystem::addParticle(const Particle& particle) {
    if (m_particles.size() >= m_config.maxParticles) {
        return static_cast<u32>(-1);
    }

    u32 index = static_cast<u32>(m_particles.size());
    m_particles.push_back(particle);
    return index;
}

void ParticleSystem::removeParticle(u32 index) {
    if (index < m_particles.size()) {
        m_particles[index].active = false;
    }
}

void ParticleSystem::initializeRandom(u32 count) {
    clear();

    std::uniform_real_distribution<Real> xDist{
        m_config.bounds.min.x + m_config.particleRadius * 2,
        m_config.bounds.max.x - m_config.particleRadius * 2
    };
    std::uniform_real_distribution<Real> yDist{
        m_config.bounds.min.y + m_config.particleRadius * 2,
        m_config.bounds.max.y - m_config.particleRadius * 2
    };
    std::uniform_real_distribution<Real> angleDist{0.0f, constants::TWO_PI};
    std::uniform_real_distribution<Real> speedDist{50.0f, 200.0f};

    for (u32 i = 0; i < count && m_particles.size() < m_config.maxParticles; ++i) {
        Vec2 pos{xDist(g_rng), yDist(g_rng)};
        Real angle = angleDist(g_rng);
        Real speed = speedDist(g_rng);
        Vec2 vel{speed * std::cos(angle), speed * std::sin(angle)};

        addParticle(pos, vel);
    }
}

void ParticleSystem::initializeGrid(u32 rows, u32 cols) {
    clear();

    Real dx = (m_config.bounds.max.x - m_config.bounds.min.x) / (cols + 1);
    Real dy = (m_config.bounds.max.y - m_config.bounds.min.y) / (rows + 1);

    std::uniform_real_distribution<Real> angleDist{0.0f, constants::TWO_PI};
    std::uniform_real_distribution<Real> speedDist{50.0f, 150.0f};

    for (u32 r = 0; r < rows; ++r) {
        for (u32 c = 0; c < cols; ++c) {
            Vec2 pos{
                m_config.bounds.min.x + dx * (c + 1),
                m_config.bounds.min.y + dy * (r + 1)
            };
            Real angle = angleDist(g_rng);
            Real speed = speedDist(g_rng);
            Vec2 vel{speed * std::cos(angle), speed * std::sin(angle)};

            addParticle(pos, vel);
        }
    }
}

void ParticleSystem::initializeMaxwellBoltzmann(u32 count, Real temperature) {
    clear();

    std::uniform_real_distribution<Real> xDist{
        m_config.bounds.min.x + m_config.particleRadius * 2,
        m_config.bounds.max.x - m_config.particleRadius * 2
    };
    std::uniform_real_distribution<Real> yDist{
        m_config.bounds.min.y + m_config.particleRadius * 2,
        m_config.bounds.max.y - m_config.particleRadius * 2
    };

    // Maxwell-Boltzmann velocity components (Gaussian)
    Real sigma = std::sqrt(constants::BOLTZMANN * temperature / m_config.molecularMass);
    std::normal_distribution<Real> velocityDist{0.0f, sigma};

    // Scale factor for visualization
    Real visualScale = 1e-9f * 5000.0f;

    for (u32 i = 0; i < count && m_particles.size() < m_config.maxParticles; ++i) {
        Vec2 pos{xDist(g_rng), yDist(g_rng)};
        Vec2 vel{
            velocityDist(g_rng) * visualScale,
            velocityDist(g_rng) * visualScale
        };

        Particle p;
        p.position = pos;
        p.velocity = vel;
        p.radius = m_config.particleRadius;
        p.mass = m_config.particleMass;
        p.temperature = temperature;
        p.active = true;

        m_particles.push_back(p);
    }
}

void ParticleSystem::step(Real dt) {
    // Update spatial hash
    updateSpatialHash();

    // Resolve collisions
    resolveParticleCollisions();
    resolveWallCollisions();

    // Integrate positions
    integratePositions(dt);
}

void ParticleSystem::stepSubdivided(Real dt, u32 substeps) {
    Real subDt = dt / static_cast<Real>(substeps);
    for (u32 i = 0; i < substeps; ++i) {
        step(subDt);
    }
}

void ParticleSystem::updateSpatialHash() {
    m_spatialHash.rebuild(m_particles);
}

void ParticleSystem::integratePositions(Real dt) {
    for (auto& p : m_particles) {
        if (!p.active) continue;

        // Limit velocity
        Real speed = p.velocity.length();
        if (speed > m_config.maxSpeed) {
            p.velocity = p.velocity * (m_config.maxSpeed / speed);
        }

        p.position += p.velocity * dt;
    }
}

void ParticleSystem::resolveParticleCollisions() {
    m_stats.particleCollisions = 0;

    // For each particle, check nearby particles
    for (u32 i = 0; i < m_particles.size(); ++i) {
        if (!m_particles[i].active) continue;

        Particle& p1 = m_particles[i];
        Real queryRadius = p1.radius * 2.0f + m_config.particleRadius;

        m_spatialHash.queryNear(p1.position, queryRadius, m_nearbyIndices);

        for (u32 j : m_nearbyIndices) {
            if (j <= i || !m_particles[j].active) continue;

            if (collideParticles(i, j)) {
                m_stats.particleCollisions++;
            }
        }
    }
}

bool ParticleSystem::collideParticles(u32 i, u32 j) {
    Particle& p1 = m_particles[i];
    Particle& p2 = m_particles[j];

    Vec2 delta = p2.position - p1.position;
    Real distSq = delta.lengthSquared();
    Real minDist = p1.radius + p2.radius;
    Real minDistSq = minDist * minDist;

    if (distSq >= minDistSq || distSq < constants::EPSILON) {
        return false;
    }

    Real dist = std::sqrt(distSq);
    Vec2 normal = delta / dist;

    // Relative velocity
    Vec2 relVel = p1.velocity - p2.velocity;
    Real relVelNormal = relVel.dot(normal);

    // Only resolve if particles are approaching
    if (relVelNormal >= 0) {
        return false;
    }

    // Collision impulse (elastic collision)
    Real e = m_config.restitution;
    Real totalMass = p1.mass + p2.mass;
    Real impulse = -(1.0f + e) * relVelNormal / totalMass;

    // Apply impulse
    p1.velocity += normal * (impulse * p2.mass);
    p2.velocity -= normal * (impulse * p1.mass);

    // Separate particles (position correction)
    Real overlap = minDist - dist;
    Real correction = overlap * 0.5f + 0.01f;
    p1.position -= normal * correction;
    p2.position += normal * correction;

    // Update collision counts
    p1.collisionCount++;
    p2.collisionCount++;

    return true;
}

void ParticleSystem::resolveWallCollisions() {
    m_stats.wallCollisions = 0;
    m_stats.momentumTransferToWalls = 0.0f;

    const AABB& bounds = m_config.bounds;
    Real e = m_config.wallRestitution;

    for (auto& p : m_particles) {
        if (!p.active) continue;

        // Left wall
        if (p.position.x - p.radius < bounds.min.x) {
            Real momentum = std::abs(p.velocity.x) * p.mass * (1.0f + e);
            m_stats.momentumTransferToWalls += momentum;

            p.position.x = bounds.min.x + p.radius;
            p.velocity.x = std::abs(p.velocity.x) * e;
            p.collisionCount++;
            m_stats.wallCollisions++;

            if (m_wallCallback) {
                m_wallCallback({
                    {bounds.min.x, p.position.y},
                    Vec2::unitX() * (-1.0f),
                    momentum,
                    static_cast<u32>(&p - m_particles.data())
                });
            }
        }

        // Right wall
        if (p.position.x + p.radius > bounds.max.x) {
            Real momentum = std::abs(p.velocity.x) * p.mass * (1.0f + e);
            m_stats.momentumTransferToWalls += momentum;

            p.position.x = bounds.max.x - p.radius;
            p.velocity.x = -std::abs(p.velocity.x) * e;
            p.collisionCount++;
            m_stats.wallCollisions++;

            if (m_wallCallback) {
                m_wallCallback({
                    {bounds.max.x, p.position.y},
                    Vec2::unitX(),
                    momentum,
                    static_cast<u32>(&p - m_particles.data())
                });
            }
        }

        // Top wall
        if (p.position.y - p.radius < bounds.min.y) {
            Real momentum = std::abs(p.velocity.y) * p.mass * (1.0f + e);
            m_stats.momentumTransferToWalls += momentum;

            p.position.y = bounds.min.y + p.radius;
            p.velocity.y = std::abs(p.velocity.y) * e;
            p.collisionCount++;
            m_stats.wallCollisions++;

            if (m_wallCallback) {
                m_wallCallback({
                    {p.position.x, bounds.min.y},
                    Vec2::unitY() * (-1.0f),
                    momentum,
                    static_cast<u32>(&p - m_particles.data())
                });
            }
        }

        // Bottom wall
        if (p.position.y + p.radius > bounds.max.y) {
            Real momentum = std::abs(p.velocity.y) * p.mass * (1.0f + e);
            m_stats.momentumTransferToWalls += momentum;

            p.position.y = bounds.max.y - p.radius;
            p.velocity.y = -std::abs(p.velocity.y) * e;
            p.collisionCount++;
            m_stats.wallCollisions++;

            if (m_wallCallback) {
                m_wallCallback({
                    {p.position.x, bounds.max.y},
                    Vec2::unitY(),
                    momentum,
                    static_cast<u32>(&p - m_particles.data())
                });
            }
        }
    }
}

void ParticleSystem::setTemperature(Real T) {
    m_config.temperature = T;
    scaleVelocitiesToTemperature(T);
}

void ParticleSystem::scaleVelocitiesToTemperature(Real targetT) {
    // Calculate current temperature from kinetic energy
    // T = (2/3) * E_avg / k  (for 2D: T = E_avg / k)
    Real totalKE = 0.0f;
    u32 activeCount = 0;

    for (const auto& p : m_particles) {
        if (p.active) {
            totalKE += p.kineticEnergy();
            activeCount++;
        }
    }

    if (activeCount == 0) return;

    Real avgKE = totalKE / activeCount;
    Real currentT = avgKE / constants::BOLTZMANN;

    if (currentT < constants::EPSILON) return;

    // Scale factor
    Real scale = std::sqrt(targetT / currentT);

    for (auto& p : m_particles) {
        if (p.active) {
            p.velocity *= scale;
            p.temperature = targetT;
        }
    }
}

void ParticleSystem::thermalizeParticle(u32 index, Real T) {
    if (index >= m_particles.size()) return;

    Particle& p = m_particles[index];
    if (!p.active) return;

    Real sigma = std::sqrt(constants::BOLTZMANN * T / m_config.molecularMass);
    std::normal_distribution<Real> dist{0.0f, sigma};
    Real visualScale = 1e-9f * 5000.0f;

    p.velocity = Vec2{dist(g_rng) * visualScale, dist(g_rng) * visualScale};
    p.temperature = T;
}

void ParticleSystem::setBounds(const AABB& bounds) {
    m_config.bounds = bounds;
}

void ParticleSystem::setVolume(Real widthPercent) {
    widthPercent = std::clamp(widthPercent, 10.0f, 100.0f);

    Real fullWidth = m_config.bounds.max.x - m_config.bounds.min.x;
    Real newWidth = fullWidth * widthPercent / 100.0f;
    Real centerX = (m_config.bounds.min.x + m_config.bounds.max.x) * 0.5f;

    Real newMinX = centerX - newWidth * 0.5f;
    Real newMaxX = centerX + newWidth * 0.5f;

    m_config.bounds.min.x = newMinX;
    m_config.bounds.max.x = newMaxX;

    // Push particles inside new bounds
    for (auto& p : m_particles) {
        if (!p.active) continue;

        if (p.position.x - p.radius < newMinX) {
            p.position.x = newMinX + p.radius;
        }
        if (p.position.x + p.radius > newMaxX) {
            p.position.x = newMaxX - p.radius;
        }
    }
}

void ParticleSystem::computeStatistics() {
    m_stats.totalKineticEnergy = 0.0f;
    m_stats.averageSpeed = 0.0f;
    m_stats.rmsSpeed = 0.0f;
    m_stats.maxSpeed = 0.0f;

    u32 activeCount = 0;
    Real sumSpeed = 0.0f;
    Real sumSpeedSq = 0.0f;

    for (const auto& p : m_particles) {
        if (!p.active) continue;

        Real speed = p.speed();
        Real ke = p.kineticEnergy();

        m_stats.totalKineticEnergy += ke;
        sumSpeed += speed;
        sumSpeedSq += speed * speed;
        m_stats.maxSpeed = std::max(m_stats.maxSpeed, speed);

        activeCount++;
    }

    if (activeCount > 0) {
        m_stats.averageKineticEnergy = m_stats.totalKineticEnergy / activeCount;
        m_stats.averageSpeed = sumSpeed / activeCount;
        m_stats.rmsSpeed = std::sqrt(sumSpeedSq / activeCount);

        // Temperature from equipartition: E = (d/2) * k * T, where d=2 for 2D
        m_stats.temperature = m_stats.averageKineticEnergy / constants::BOLTZMANN;

        // Pressure from ideal gas law: P = N*k*T / V
        // Or from momentum transfer: P = F/A = dp/dt / perimeter
        Real perimeter = 2.0f * (m_config.bounds.width() + m_config.bounds.height());
        Real area = m_config.bounds.area();

        // Using momentum transfer from wall collisions
        // This is updated during resolveWallCollisions
        m_stats.pressure = m_stats.momentumTransferToWalls / perimeter;
    }
}

void ParticleSystem::computeVelocityHistogram(u32 numBins, Real maxVelocity) {
    m_stats.velocityHistogram.assign(numBins, 0);
    m_stats.histogramBinWidth = maxVelocity / numBins;

    for (const auto& p : m_particles) {
        if (!p.active) continue;

        Real speed = p.speed();
        u32 bin = static_cast<u32>(speed / m_stats.histogramBinWidth);
        bin = std::min(bin, numBins - 1);
        m_stats.velocityHistogram[bin]++;
    }
}

void ParticleSystem::setWallCollisionCallback(WallCollisionCallback callback) {
    m_wallCallback = std::move(callback);
}

const Real* ParticleSystem::positionData() const {
    if (m_particles.empty()) return nullptr;
    return &m_particles[0].position.x;
}

const Real* ParticleSystem::velocityData() const {
    if (m_particles.empty()) return nullptr;
    return &m_particles[0].velocity.x;
}

} // namespace physics
} // namespace eigenlab
