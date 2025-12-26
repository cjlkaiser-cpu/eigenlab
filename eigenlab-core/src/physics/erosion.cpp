/**
 * @file erosion.cpp
 * @brief Implementation of hydraulic erosion simulation
 */

#include "../../include/physics/erosion.hpp"
#include <algorithm>
#include <cmath>

namespace eigenlab {
namespace physics {

// ============================================================================
// Constructor
// ============================================================================

ErosionSimulator::ErosionSimulator()
    : m_rng(std::random_device{}())
{
    m_heightMap.resize(m_config.width * m_config.height, 0.5f);
    m_waterMap.resize(m_config.width * m_config.height, 0.0f);
    m_sedimentMap.resize(m_config.width * m_config.height, 0.0f);
}

ErosionSimulator::ErosionSimulator(const ErosionConfig& config)
    : m_config(config)
    , m_rng(std::random_device{}())
{
    m_heightMap.resize(config.width * config.height, 0.5f);
    m_waterMap.resize(config.width * config.height, 0.0f);
    m_sedimentMap.resize(config.width * config.height, 0.0f);
}

void ErosionSimulator::setConfig(const ErosionConfig& config) {
    m_config = config;
    m_heightMap.resize(config.width * config.height, 0.5f);
    m_waterMap.resize(config.width * config.height, 0.0f);
    m_sedimentMap.resize(config.width * config.height, 0.0f);
}

// ============================================================================
// Terrain Initialization
// ============================================================================

void ErosionSimulator::initializeFlat(Real height) {
    std::fill(m_heightMap.begin(), m_heightMap.end(), height);
    std::fill(m_waterMap.begin(), m_waterMap.end(), 0.0f);
    std::fill(m_sedimentMap.begin(), m_sedimentMap.end(), 0.0f);
    m_stats = ErosionStats{};
}

void ErosionSimulator::initializeNoise(Real scale, u32 octaves) {
    for (u32 y = 0; y < m_config.height; ++y) {
        for (u32 x = 0; x < m_config.width; ++x) {
            Real nx = static_cast<Real>(x) / m_config.width * scale;
            Real ny = static_cast<Real>(y) / m_config.height * scale;
            m_heightMap[idx(x, y)] = fbm(nx, ny, octaves);
        }
    }

    std::fill(m_waterMap.begin(), m_waterMap.end(), 0.0f);
    std::fill(m_sedimentMap.begin(), m_sedimentMap.end(), 0.0f);
    m_stats = ErosionStats{};
}

void ErosionSimulator::initializeMountain(Vec2 center, Real radius, Real height) {
    for (u32 y = 0; y < m_config.height; ++y) {
        for (u32 x = 0; x < m_config.width; ++x) {
            Real dx = x - center.x;
            Real dy = y - center.y;
            Real dist = std::sqrt(dx * dx + dy * dy);

            // Cone shape with noise
            Real h = std::max(0.0f, 1.0f - dist / radius);
            h = h * h * height; // Quadratic falloff

            // Add some noise
            Real noise = fbm(x * 0.05f, y * 0.05f, 3) * 0.1f;
            m_heightMap[idx(x, y)] = h + noise;
        }
    }

    std::fill(m_waterMap.begin(), m_waterMap.end(), 0.0f);
    std::fill(m_sedimentMap.begin(), m_sedimentMap.end(), 0.0f);
    m_stats = ErosionStats{};
}

void ErosionSimulator::initializeRidges(u32 count, Real height) {
    // Create mountain ridges
    for (u32 i = 0; i < count; ++i) {
        // Random ridge line
        Real x1 = m_uniformDist(m_rng) * m_config.width;
        Real y1 = m_uniformDist(m_rng) * m_config.height;
        Real x2 = m_uniformDist(m_rng) * m_config.width;
        Real y2 = m_uniformDist(m_rng) * m_config.height;

        Real ridgeWidth = 20.0f + m_uniformDist(m_rng) * 40.0f;
        Real ridgeHeight = height * (0.5f + m_uniformDist(m_rng) * 0.5f);

        for (u32 y = 0; y < m_config.height; ++y) {
            for (u32 x = 0; x < m_config.width; ++x) {
                // Distance to line segment
                Real dx = x2 - x1;
                Real dy = y2 - y1;
                Real len = std::sqrt(dx * dx + dy * dy);
                if (len < 0.001f) continue;

                Real t = std::max(0.0f, std::min(1.0f,
                    ((x - x1) * dx + (y - y1) * dy) / (len * len)));

                Real projX = x1 + t * dx;
                Real projY = y1 + t * dy;

                Real distToLine = std::sqrt(
                    (x - projX) * (x - projX) +
                    (y - projY) * (y - projY));

                Real contribution = std::max(0.0f, 1.0f - distToLine / ridgeWidth);
                contribution = contribution * contribution * ridgeHeight;

                m_heightMap[idx(x, y)] = std::max(m_heightMap[idx(x, y)], contribution);
            }
        }
    }

    // Add noise
    for (u32 i = 0; i < m_heightMap.size(); ++i) {
        Real x = static_cast<Real>(i % m_config.width);
        Real y = static_cast<Real>(i / m_config.width);
        m_heightMap[i] += fbm(x * 0.03f, y * 0.03f, 4) * 0.15f;
    }

    std::fill(m_waterMap.begin(), m_waterMap.end(), 0.0f);
    std::fill(m_sedimentMap.begin(), m_sedimentMap.end(), 0.0f);
    m_stats = ErosionStats{};
}

void ErosionSimulator::initializeFromData(const Real* data, u32 w, u32 h) {
    m_config.width = w;
    m_config.height = h;
    m_heightMap.assign(data, data + w * h);
    m_waterMap.resize(w * h, 0.0f);
    m_sedimentMap.resize(w * h, 0.0f);
    m_stats = ErosionStats{};
}

// ============================================================================
// Manual Terrain Modification
// ============================================================================

void ErosionSimulator::raise(Real x, Real y, Real radius, Real amount) {
    u32 r = static_cast<u32>(radius);
    u32 cx = static_cast<u32>(x);
    u32 cy = static_cast<u32>(y);

    for (u32 dy = 0; dy <= r * 2; ++dy) {
        for (u32 dx = 0; dx <= r * 2; ++dx) {
            i32 px = static_cast<i32>(cx) - r + dx;
            i32 py = static_cast<i32>(cy) - r + dy;

            if (px >= 0 && px < static_cast<i32>(m_config.width) &&
                py >= 0 && py < static_cast<i32>(m_config.height)) {

                Real dist = std::sqrt(
                    (px - x) * (px - x) +
                    (py - y) * (py - y));

                if (dist < radius) {
                    Real factor = 1.0f - dist / radius;
                    factor = factor * factor; // Smooth falloff
                    m_heightMap[idx(px, py)] += amount * factor;
                }
            }
        }
    }
}

void ErosionSimulator::lower(Real x, Real y, Real radius, Real amount) {
    raise(x, y, radius, -amount);
}

void ErosionSimulator::flatten(Real x, Real y, Real radius, Real targetHeight) {
    u32 r = static_cast<u32>(radius);
    u32 cx = static_cast<u32>(x);
    u32 cy = static_cast<u32>(y);

    for (u32 dy = 0; dy <= r * 2; ++dy) {
        for (u32 dx = 0; dx <= r * 2; ++dx) {
            i32 px = static_cast<i32>(cx) - r + dx;
            i32 py = static_cast<i32>(cy) - r + dy;

            if (px >= 0 && px < static_cast<i32>(m_config.width) &&
                py >= 0 && py < static_cast<i32>(m_config.height)) {

                Real dist = std::sqrt(
                    (px - x) * (px - x) +
                    (py - y) * (py - y));

                if (dist < radius) {
                    Real factor = 1.0f - dist / radius;
                    Real& h = m_heightMap[idx(px, py)];
                    h = h + (targetHeight - h) * factor * 0.5f;
                }
            }
        }
    }
}

void ErosionSimulator::setHeight(u32 x, u32 y, Real height) {
    if (x < m_config.width && y < m_config.height) {
        m_heightMap[idx(x, y)] = height;
    }
}

// ============================================================================
// Height Sampling
// ============================================================================

Real ErosionSimulator::heightAt(u32 x, u32 y) const {
    if (x >= m_config.width) x = m_config.width - 1;
    if (y >= m_config.height) y = m_config.height - 1;
    return m_heightMap[idx(x, y)];
}

Real ErosionSimulator::heightAtInterp(Real x, Real y) const {
    // Bilinear interpolation
    x = std::max(0.0f, std::min(x, static_cast<Real>(m_config.width - 1)));
    y = std::max(0.0f, std::min(y, static_cast<Real>(m_config.height - 1)));

    u32 x0 = static_cast<u32>(x);
    u32 y0 = static_cast<u32>(y);
    u32 x1 = std::min(x0 + 1, m_config.width - 1);
    u32 y1 = std::min(y0 + 1, m_config.height - 1);

    Real fx = x - x0;
    Real fy = y - y0;

    Real h00 = m_heightMap[idx(x0, y0)];
    Real h10 = m_heightMap[idx(x1, y0)];
    Real h01 = m_heightMap[idx(x0, y1)];
    Real h11 = m_heightMap[idx(x1, y1)];

    Real h0 = h00 + (h10 - h00) * fx;
    Real h1 = h01 + (h11 - h01) * fx;

    return h0 + (h1 - h0) * fy;
}

Vec2 ErosionSimulator::gradientAt(Real x, Real y) const {
    // Central differences
    Real h = heightAtInterp(x, y);
    Real hpx = heightAtInterp(x + 1, y);
    Real hmx = heightAtInterp(x - 1, y);
    Real hpy = heightAtInterp(x, y + 1);
    Real hmy = heightAtInterp(x, y - 1);

    return {(hmx - hpx) * 0.5f, (hmy - hpy) * 0.5f};
}

Vec2 ErosionSimulator::normalAt(u32 x, u32 y) const {
    Vec2 grad = gradientAt(static_cast<Real>(x), static_cast<Real>(y));
    // Normal = (-dh/dx, -dh/dy, 1) normalized, but we return 2D
    Real len = std::sqrt(grad.x * grad.x + grad.y * grad.y + 1.0f);
    return {-grad.x / len, -grad.y / len};
}

bool ErosionSimulator::inBounds(Real x, Real y) const {
    return x >= 0 && x < m_config.width - 1 && y >= 0 && y < m_config.height - 1;
}

// ============================================================================
// Erosion Simulation
// ============================================================================

void ErosionSimulator::simulateDroplet() {
    // Random starting position
    Real posX = m_uniformDist(m_rng) * (m_config.width - 2) + 1;
    Real posY = m_uniformDist(m_rng) * (m_config.height - 2) + 1;

    Real dirX = 0;
    Real dirY = 0;
    Real velocity = m_config.initialVelocity;
    Real water = m_config.initialWater;
    Real sediment = 0;

    for (u32 lifetime = 0; lifetime < m_config.maxLifetime; ++lifetime) {
        u32 cellX = static_cast<u32>(posX);
        u32 cellY = static_cast<u32>(posY);

        // Calculate droplet's offset inside the cell
        Real cellOffsetX = posX - cellX;
        Real cellOffsetY = posY - cellY;

        // Calculate droplet's height and direction of flow
        Real height = heightAtInterp(posX, posY);
        Vec2 gradient = gradientAt(posX, posY);

        // Update direction based on inertia
        dirX = dirX * m_config.inertia - gradient.x * (1 - m_config.inertia);
        dirY = dirY * m_config.inertia - gradient.y * (1 - m_config.inertia);

        // Normalize direction
        Real len = std::sqrt(dirX * dirX + dirY * dirY);
        if (len > 0.0001f) {
            dirX /= len;
            dirY /= len;
        } else {
            // Random direction if no gradient
            Real angle = m_uniformDist(m_rng) * 2.0f * constants::PI;
            dirX = std::cos(angle);
            dirY = std::sin(angle);
        }

        // Calculate new position
        Real newPosX = posX + dirX;
        Real newPosY = posY + dirY;

        // Check bounds
        if (!inBounds(newPosX, newPosY)) {
            break;
        }

        // Calculate height difference
        Real newHeight = heightAtInterp(newPosX, newPosY);
        Real deltaHeight = newHeight - height;

        // Calculate sediment capacity
        Real capacity = std::max(
            -deltaHeight * velocity * water * m_config.sedimentCapacity,
            m_config.minSedimentCapacity);

        if (sediment > capacity || deltaHeight > 0) {
            // Deposit sediment
            Real depositAmount = (deltaHeight > 0)
                ? std::min(deltaHeight, sediment)
                : (sediment - capacity) * m_config.depositSpeed;

            sediment -= depositAmount;
            depositAt(posX, posY, depositAmount);
            m_stats.totalDeposition += depositAmount;
        } else {
            // Erode terrain
            Real erodeAmount = std::min(
                (capacity - sediment) * m_config.erodeSpeed,
                -deltaHeight);

            erodeAt(posX, posY, erodeAmount);
            sediment += erodeAmount;
            m_stats.totalErosion += erodeAmount;
        }

        // Update velocity
        velocity = std::sqrt(std::max(0.0f, velocity * velocity + deltaHeight * m_config.gravity));

        // Evaporate water
        water *= (1 - m_config.evaporateSpeed);

        // Track water for visualization
        if (cellX < m_config.width && cellY < m_config.height) {
            m_waterMap[idx(cellX, cellY)] += water * 0.01f;
        }

        // Move to new position
        posX = newPosX;
        posY = newPosY;

        // Stop if water depleted
        if (water < 0.001f) {
            break;
        }
    }

    m_stats.totalDroplets++;
}

void ErosionSimulator::erodeAt(Real x, Real y, Real amount) {
    u32 r = m_config.erosionRadius;
    i32 cx = static_cast<i32>(x);
    i32 cy = static_cast<i32>(y);

    // Calculate weights
    Real totalWeight = 0;
    std::vector<std::pair<u32, Real>> weights;

    for (i32 dy = -static_cast<i32>(r); dy <= static_cast<i32>(r); ++dy) {
        for (i32 dx = -static_cast<i32>(r); dx <= static_cast<i32>(r); ++dx) {
            i32 px = cx + dx;
            i32 py = cy + dy;

            if (px >= 0 && px < static_cast<i32>(m_config.width) &&
                py >= 0 && py < static_cast<i32>(m_config.height)) {

                Real dist = std::sqrt(
                    (px - x) * (px - x) +
                    (py - y) * (py - y));

                if (dist <= r) {
                    Real weight = std::max(0.0f, r - dist);
                    weights.emplace_back(idx(px, py), weight);
                    totalWeight += weight;
                }
            }
        }
    }

    // Apply erosion
    if (totalWeight > 0) {
        for (const auto& [i, weight] : weights) {
            m_heightMap[i] -= amount * weight / totalWeight;
        }
    }
}

void ErosionSimulator::depositAt(Real x, Real y, Real amount) {
    // Bilinear deposition
    u32 x0 = static_cast<u32>(x);
    u32 y0 = static_cast<u32>(y);
    u32 x1 = std::min(x0 + 1, m_config.width - 1);
    u32 y1 = std::min(y0 + 1, m_config.height - 1);

    Real fx = x - x0;
    Real fy = y - y0;

    m_heightMap[idx(x0, y0)] += amount * (1 - fx) * (1 - fy);
    m_heightMap[idx(x1, y0)] += amount * fx * (1 - fy);
    m_heightMap[idx(x0, y1)] += amount * (1 - fx) * fy;
    m_heightMap[idx(x1, y1)] += amount * fx * fy;

    m_sedimentMap[idx(x0, y0)] += amount * 0.5f;
}

void ErosionSimulator::simulateDroplets(u32 count) {
    for (u32 i = 0; i < count; ++i) {
        simulateDroplet();
    }
    m_stats.iterations++;
}

void ErosionSimulator::simulateThermalErosion() {
    // Thermal erosion: material slides down steep slopes
    std::vector<Real> changes(m_heightMap.size(), 0);

    for (u32 y = 1; y < m_config.height - 1; ++y) {
        for (u32 x = 1; x < m_config.width - 1; ++x) {
            Real h = m_heightMap[idx(x, y)];

            // Check 4 neighbors
            const i32 dx[] = {-1, 1, 0, 0};
            const i32 dy[] = {0, 0, -1, 1};

            for (i32 i = 0; i < 4; ++i) {
                u32 nx = x + dx[i];
                u32 ny = y + dy[i];
                Real nh = m_heightMap[idx(nx, ny)];

                Real slope = h - nh;
                if (slope > m_config.talusAngle) {
                    Real transfer = (slope - m_config.talusAngle) * m_config.thermalRate * 0.25f;
                    changes[idx(x, y)] -= transfer;
                    changes[idx(nx, ny)] += transfer;
                }
            }
        }
    }

    // Apply changes
    for (u32 i = 0; i < m_heightMap.size(); ++i) {
        m_heightMap[i] += changes[i];
    }
}

void ErosionSimulator::step() {
    simulateDroplets(100);
    simulateThermalErosion();
}

// ============================================================================
// Statistics
// ============================================================================

void ErosionSimulator::computeStatistics() {
    m_stats.minHeight = 1e30f;
    m_stats.maxHeight = -1e30f;
    m_stats.averageHeight = 0;

    for (const auto& h : m_heightMap) {
        m_stats.minHeight = std::min(m_stats.minHeight, h);
        m_stats.maxHeight = std::max(m_stats.maxHeight, h);
        m_stats.averageHeight += h;
    }

    m_stats.averageHeight /= m_heightMap.size();
}

// ============================================================================
// Rendering
// ============================================================================

void ErosionSimulator::renderHeightMap(u32* buffer) const {
    Real minH = *std::min_element(m_heightMap.begin(), m_heightMap.end());
    Real maxH = *std::max_element(m_heightMap.begin(), m_heightMap.end());
    Real range = maxH - minH;
    if (range < 0.001f) range = 1.0f;

    for (u32 i = 0; i < m_heightMap.size(); ++i) {
        Real t = (m_heightMap[i] - minH) / range;

        // Terrain color gradient
        u32 r, g, b;
        if (t < 0.3f) {
            // Water/lowland (blue-green)
            Real s = t / 0.3f;
            r = static_cast<u32>(30 + s * 50);
            g = static_cast<u32>(80 + s * 70);
            b = static_cast<u32>(120 - s * 40);
        } else if (t < 0.6f) {
            // Grass/forest (green)
            Real s = (t - 0.3f) / 0.3f;
            r = static_cast<u32>(80 + s * 60);
            g = static_cast<u32>(150 - s * 30);
            b = static_cast<u32>(80 - s * 30);
        } else if (t < 0.85f) {
            // Rock/mountain (brown-gray)
            Real s = (t - 0.6f) / 0.25f;
            r = static_cast<u32>(140 + s * 40);
            g = static_cast<u32>(120 + s * 40);
            b = static_cast<u32>(50 + s * 80);
        } else {
            // Snow (white)
            Real s = (t - 0.85f) / 0.15f;
            r = static_cast<u32>(180 + s * 75);
            g = static_cast<u32>(160 + s * 95);
            b = static_cast<u32>(130 + s * 125);
        }

        buffer[i] = (255 << 24) | (b << 16) | (g << 8) | r;
    }
}

void ErosionSimulator::renderWaterMap(u32* buffer) const {
    Real maxW = *std::max_element(m_waterMap.begin(), m_waterMap.end());
    if (maxW < 0.001f) maxW = 1.0f;

    for (u32 i = 0; i < m_waterMap.size(); ++i) {
        Real t = std::min(m_waterMap[i] / maxW, 1.0f);
        t = std::sqrt(t); // Enhance visibility

        u32 r = static_cast<u32>(30 * t);
        u32 g = static_cast<u32>(100 * t);
        u32 b = static_cast<u32>(200 * t + 50);

        buffer[i] = (255 << 24) | (b << 16) | (g << 8) | r;
    }
}

void ErosionSimulator::renderNormalMap(u32* buffer) const {
    for (u32 y = 0; y < m_config.height; ++y) {
        for (u32 x = 0; x < m_config.width; ++x) {
            Vec2 n = normalAt(x, y);

            u32 r = static_cast<u32>((n.x * 0.5f + 0.5f) * 255);
            u32 g = static_cast<u32>((n.y * 0.5f + 0.5f) * 255);
            u32 b = 128; // Z component

            buffer[idx(x, y)] = (255 << 24) | (b << 16) | (g << 8) | r;
        }
    }
}

void ErosionSimulator::renderSlopeMap(u32* buffer) const {
    for (u32 y = 0; y < m_config.height; ++y) {
        for (u32 x = 0; x < m_config.width; ++x) {
            Vec2 grad = gradientAt(static_cast<Real>(x), static_cast<Real>(y));
            Real slope = std::sqrt(grad.x * grad.x + grad.y * grad.y);
            slope = std::min(slope * 2.0f, 1.0f);

            u32 v = static_cast<u32>(slope * 255);
            buffer[idx(x, y)] = (255 << 24) | (v << 16) | (v << 8) | v;
        }
    }
}

// ============================================================================
// Noise Functions
// ============================================================================

Real ErosionSimulator::noise2D(Real x, Real y) const {
    // Simple value noise using hashing
    auto hash = [](i32 x, i32 y) -> Real {
        i32 n = x + y * 57;
        n = (n << 13) ^ n;
        return 1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f;
    };

    i32 xi = static_cast<i32>(std::floor(x));
    i32 yi = static_cast<i32>(std::floor(y));
    Real xf = x - xi;
    Real yf = y - yi;

    // Smoothstep
    xf = xf * xf * (3 - 2 * xf);
    yf = yf * yf * (3 - 2 * yf);

    Real n00 = hash(xi, yi);
    Real n10 = hash(xi + 1, yi);
    Real n01 = hash(xi, yi + 1);
    Real n11 = hash(xi + 1, yi + 1);

    Real n0 = n00 + (n10 - n00) * xf;
    Real n1 = n01 + (n11 - n01) * xf;

    return (n0 + (n1 - n0) * yf) * 0.5f + 0.5f;
}

Real ErosionSimulator::fbm(Real x, Real y, u32 octaves) const {
    Real value = 0;
    Real amplitude = 0.5f;
    Real frequency = 1.0f;
    Real maxValue = 0;

    for (u32 i = 0; i < octaves; ++i) {
        value += noise2D(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }

    return value / maxValue;
}

// ============================================================================
// Presets
// ============================================================================

namespace erosion_presets {

ErosionConfig gentle() {
    ErosionConfig c;
    c.inertia = 0.05f;
    c.sedimentCapacity = 4.0f;
    c.depositSpeed = 0.3f;
    c.erodeSpeed = 0.3f;
    c.evaporateSpeed = 0.01f;
    c.gravity = 4.0f;
    c.erosionRadius = 3;
    return c;
}

ErosionConfig river() {
    ErosionConfig c;
    c.inertia = 0.1f;
    c.sedimentCapacity = 8.0f;
    c.depositSpeed = 0.2f;
    c.erodeSpeed = 0.5f;
    c.evaporateSpeed = 0.005f;
    c.gravity = 6.0f;
    c.erosionRadius = 4;
    c.maxLifetime = 150;
    return c;
}

ErosionConfig canyon() {
    ErosionConfig c;
    c.inertia = 0.02f;
    c.sedimentCapacity = 12.0f;
    c.depositSpeed = 0.1f;
    c.erodeSpeed = 0.7f;
    c.evaporateSpeed = 0.002f;
    c.gravity = 10.0f;
    c.erosionRadius = 5;
    c.maxLifetime = 200;
    return c;
}

ErosionConfig coastal() {
    ErosionConfig c;
    c.inertia = 0.15f;
    c.sedimentCapacity = 3.0f;
    c.depositSpeed = 0.5f;
    c.erodeSpeed = 0.2f;
    c.evaporateSpeed = 0.02f;
    c.gravity = 2.0f;
    c.erosionRadius = 2;
    return c;
}

ErosionConfig volcanic() {
    ErosionConfig c;
    c.inertia = 0.03f;
    c.sedimentCapacity = 6.0f;
    c.depositSpeed = 0.4f;
    c.erodeSpeed = 0.4f;
    c.evaporateSpeed = 0.015f;
    c.gravity = 8.0f;
    c.erosionRadius = 4;
    c.talusAngle = 0.7f;
    c.thermalRate = 0.15f;
    return c;
}

} // namespace erosion_presets

} // namespace physics
} // namespace eigenlab
