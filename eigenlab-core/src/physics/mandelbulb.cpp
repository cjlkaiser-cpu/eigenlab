#include "../../include/physics/mandelbulb.hpp"
#include <cmath>
#include <algorithm>

namespace eigenlab {
namespace physics {

Mandelbulb::Mandelbulb()
    : pointCount_(0)
{
    setConfig(MandelbulbConfig{});
}

Mandelbulb::Mandelbulb(const MandelbulbConfig& config)
    : pointCount_(0)
{
    setConfig(config);
}

void Mandelbulb::setConfig(const MandelbulbConfig& config) {
    config_ = config;

    // Reserve space for potential surface points
    int maxPoints = config.resolution * config.resolution * config.resolution / 8;
    surfacePoints_.reserve(maxPoints * 3);
    surfaceNormals_.reserve(maxPoints * 3);
    surfaceColors_.reserve(maxPoints * 3);

    generate();
}

// ==================================================
// Triplex Power (Spherical Coordinates)
// ==================================================

void Mandelbulb::triplex_pow(float x, float y, float z, float n,
                              float& outX, float& outY, float& outZ) const {
    // Convert to spherical
    float r = std::sqrt(x*x + y*y + z*z);
    if (r < 1e-10f) {
        outX = outY = outZ = 0.0f;
        return;
    }

    float theta = std::acos(z / r);
    float phi = std::atan2(y, x);

    // Apply power
    float rn = std::pow(r, n);
    float nt = n * theta;
    float np = n * phi;

    // Convert back to Cartesian
    outX = rn * std::sin(nt) * std::cos(np);
    outY = rn * std::sin(nt) * std::sin(np);
    outZ = rn * std::cos(nt);
}

// ==================================================
// Distance Estimation
// ==================================================

float Mandelbulb::distanceEstimate(float cx, float cy, float cz) const {
    float x = 0, y = 0, z = 0;
    float dr = 1.0f;  // Running derivative
    float r = 0;

    for (int i = 0; i < config_.maxIterations; ++i) {
        r = std::sqrt(x*x + y*y + z*z);

        if (r > config_.bailout) break;

        // Compute derivative
        float theta = std::acos(z / r);
        float phi = std::atan2(y, x);
        dr = std::pow(r, config_.power - 1.0f) * config_.power * dr + 1.0f;

        // z = z^n + c
        float nx, ny, nz;
        triplex_pow(x, y, z, config_.power, nx, ny, nz);
        x = nx + cx;
        y = ny + cy;
        z = nz + cz;
    }

    // Distance estimation formula
    return 0.5f * std::log(r) * r / dr;
}

// ==================================================
// Iteration Count (for coloring)
// ==================================================

int Mandelbulb::iterationCount(float cx, float cy, float cz) const {
    float x = 0, y = 0, z = 0;

    for (int i = 0; i < config_.maxIterations; ++i) {
        float r = std::sqrt(x*x + y*y + z*z);
        if (r > config_.bailout) return i;

        float nx, ny, nz;
        triplex_pow(x, y, z, config_.power, nx, ny, nz);
        x = nx + cx;
        y = ny + cy;
        z = nz + cz;
    }

    return config_.maxIterations;
}

// ==================================================
// Normal Computation
// ==================================================

void Mandelbulb::computeNormal(float x, float y, float z,
                                float& nx, float& ny, float& nz) const {
    const float eps = 0.0001f;

    // Gradient of distance field
    float dx = distanceEstimate(x + eps, y, z) - distanceEstimate(x - eps, y, z);
    float dy = distanceEstimate(x, y + eps, z) - distanceEstimate(x, y - eps, z);
    float dz = distanceEstimate(x, y, z + eps) - distanceEstimate(x, y, z - eps);

    // Normalize
    float len = std::sqrt(dx*dx + dy*dy + dz*dz);
    if (len > 1e-10f) {
        nx = dx / len;
        ny = dy / len;
        nz = dz / len;
    } else {
        nx = ny = 0;
        nz = 1;
    }
}

// ==================================================
// Surface Generation
// ==================================================

void Mandelbulb::generate() {
    surfacePoints_.clear();
    surfaceNormals_.clear();
    surfaceColors_.clear();
    pointCount_ = 0;

    int res = config_.resolution;
    float zoom = config_.zoom;
    float step = 2.0f * zoom / res;

    // Sample the volume and find surface points
    for (int iz = 0; iz < res; ++iz) {
        float z = -zoom + iz * step;

        for (int iy = 0; iy < res; ++iy) {
            float y = -zoom + iy * step;

            for (int ix = 0; ix < res; ++ix) {
                float x = -zoom + ix * step;

                // Distance estimation
                float de = distanceEstimate(x, y, z);

                // Check if near surface
                if (de < config_.threshold && de > 0) {
                    // Add surface point
                    surfacePoints_.push_back(x);
                    surfacePoints_.push_back(y);
                    surfacePoints_.push_back(z);

                    // Compute normal
                    float nx, ny, nz;
                    computeNormal(x, y, z, nx, ny, nz);
                    surfaceNormals_.push_back(nx);
                    surfaceNormals_.push_back(ny);
                    surfaceNormals_.push_back(nz);

                    // Color based on iteration count
                    int iter = iterationCount(x, y, z);
                    float t = static_cast<float>(iter) / config_.maxIterations;

                    // Purple to cyan gradient
                    float r = 0.5f + 0.5f * std::sin(t * 6.28f);
                    float g = 0.3f + 0.5f * std::sin(t * 6.28f + 2.0f);
                    float b = 0.8f + 0.2f * std::sin(t * 6.28f + 4.0f);

                    surfaceColors_.push_back(r);
                    surfaceColors_.push_back(g);
                    surfaceColors_.push_back(b);

                    pointCount_++;
                }
            }
        }
    }

    computeStatistics();
}

// ==================================================
// Parameter Setters
// ==================================================

void Mandelbulb::setPower(float power) {
    config_.power = power;
}

void Mandelbulb::setMaxIterations(int maxIter) {
    config_.maxIterations = maxIter;
}

void Mandelbulb::setResolution(int res) {
    config_.resolution = res;
}

void Mandelbulb::setZoom(float zoom) {
    config_.zoom = zoom;
}

// ==================================================
// Statistics
// ==================================================

void Mandelbulb::computeStatistics() {
    stats_.pointCount = pointCount_;
    stats_.resolution = config_.resolution;
    stats_.power = config_.power;
    stats_.maxIterations = config_.maxIterations;
}

// ==================================================
// Presets
// ==================================================

MandelbulbConfig mandelbulbPresetClassic() {
    MandelbulbConfig config;
    config.resolution = 100;
    config.maxIterations = 12;
    config.power = 8.0f;
    config.bailout = 2.0f;
    config.zoom = 1.3f;
    config.threshold = 0.015f;
    return config;
}

MandelbulbConfig mandelbulbPresetSmooth() {
    MandelbulbConfig config;
    config.resolution = 80;
    config.maxIterations = 10;
    config.power = 4.0f;
    config.bailout = 2.0f;
    config.zoom = 1.5f;
    config.threshold = 0.02f;
    return config;
}

MandelbulbConfig mandelbulbPresetSpiky() {
    MandelbulbConfig config;
    config.resolution = 90;
    config.maxIterations = 15;
    config.power = 12.0f;
    config.bailout = 2.0f;
    config.zoom = 1.2f;
    config.threshold = 0.012f;
    return config;
}

MandelbulbConfig mandelbulbPresetHighDetail() {
    MandelbulbConfig config;
    config.resolution = 150;
    config.maxIterations = 16;
    config.power = 8.0f;
    config.bailout = 2.0f;
    config.zoom = 1.25f;
    config.threshold = 0.008f;
    return config;
}

} // namespace physics
} // namespace eigenlab
