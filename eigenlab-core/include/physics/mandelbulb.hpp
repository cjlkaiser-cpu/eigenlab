#pragma once
#include <vector>
#include <cstdint>

namespace eigenlab {
namespace physics {

/**
 * Mandelbulb Fractal Generator
 *
 * 3D generalization of the Mandelbrot set using triplex algebra.
 * Formula: z^n + c using spherical coordinates
 *
 * z = (r, theta, phi)
 * z^n = r^n * (sin(n*theta)*cos(n*phi), sin(n*theta)*sin(n*phi), cos(n*theta))
 *
 * Features:
 * - Configurable power (n=8 is classic Mandelbulb)
 * - Ray marching distance estimation
 * - Point cloud generation for rendering
 * - Multiple color schemes
 */

struct MandelbulbConfig {
    int resolution = 128;        // Grid resolution
    int maxIterations = 12;      // Max iterations for escape
    float power = 8.0f;          // Power for z^n (8 = classic)
    float bailout = 2.0f;        // Escape radius
    float zoom = 1.5f;           // Zoom level (bounds = [-zoom, zoom])
    float threshold = 0.01f;     // Surface threshold for point cloud
};

struct MandelbulbStats {
    int pointCount = 0;
    int resolution = 0;
    float power = 0.0f;
    int maxIterations = 0;
};

class Mandelbulb {
public:
    Mandelbulb();
    explicit Mandelbulb(const MandelbulbConfig& config);
    ~Mandelbulb() = default;

    // Configuration
    void setConfig(const MandelbulbConfig& config);
    void generate();

    // Parameters
    void setPower(float power);
    void setMaxIterations(int maxIter);
    void setResolution(int res);
    void setZoom(float zoom);

    // Data access
    const float* surfacePoints() const { return surfacePoints_.data(); }
    const float* surfaceNormals() const { return surfaceNormals_.data(); }
    const float* surfaceColors() const { return surfaceColors_.data(); }
    int pointCount() const { return pointCount_; }

    // Distance estimation for ray marching (single point)
    float distanceEstimate(float x, float y, float z) const;

    // Iteration count (for coloring)
    int iterationCount(float x, float y, float z) const;

    // Statistics
    void computeStatistics();
    const MandelbulbStats& stats() const { return stats_; }

private:
    MandelbulbConfig config_;
    MandelbulbStats stats_;

    // Surface points for rendering
    std::vector<float> surfacePoints_;   // x, y, z
    std::vector<float> surfaceNormals_;  // nx, ny, nz
    std::vector<float> surfaceColors_;   // r, g, b
    int pointCount_;

    // Internal methods
    void triplex_pow(float x, float y, float z, float n,
                     float& outX, float& outY, float& outZ) const;
    void computeNormal(float x, float y, float z,
                       float& nx, float& ny, float& nz) const;
};

// Presets
MandelbulbConfig mandelbulbPresetClassic();    // Power 8
MandelbulbConfig mandelbulbPresetSmooth();     // Power 4, smooth
MandelbulbConfig mandelbulbPresetSpiky();      // Power 12
MandelbulbConfig mandelbulbPresetHighDetail(); // High resolution

} // namespace physics
} // namespace eigenlab
