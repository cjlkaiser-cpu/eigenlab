/**
 * @file types.cpp
 * @brief Implementation of core types
 */

#include "../../include/core/types.hpp"
#include <random>
#include <cmath>

namespace eigenlab {

// ============================================================================
// Random number generation
// ============================================================================

namespace {
    thread_local std::mt19937 g_rng{std::random_device{}()};
    thread_local std::uniform_real_distribution<Real> g_uniformDist{0.0f, 1.0f};
    thread_local std::normal_distribution<Real> g_normalDist{0.0f, 1.0f};
}

// ============================================================================
// Vec2 implementation
// ============================================================================

Vec2 Vec2::random(Real minVal, Real maxVal) {
    std::uniform_real_distribution<Real> dist{minVal, maxVal};
    return {dist(g_rng), dist(g_rng)};
}

Vec2 Vec2::randomUnit() {
    Real angle = g_uniformDist(g_rng) * constants::TWO_PI;
    return {std::cos(angle), std::sin(angle)};
}

// ============================================================================
// Color implementation
// ============================================================================

Color Color::fromHSV(Real h, Real s, Real v, Real a) {
    // Normalize h to [0, 360)
    h = std::fmod(h, 360.0f);
    if (h < 0) h += 360.0f;

    Real c = v * s;
    Real x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    Real m = v - c;

    Real r, g, b;
    if (h < 60) {
        r = c; g = x; b = 0;
    } else if (h < 120) {
        r = x; g = c; b = 0;
    } else if (h < 180) {
        r = 0; g = c; b = x;
    } else if (h < 240) {
        r = 0; g = x; b = c;
    } else if (h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }

    return Color(
        static_cast<u32>((r + m) * 255),
        static_cast<u32>((g + m) * 255),
        static_cast<u32>((b + m) * 255),
        static_cast<u32>(a * 255)
    );
}

Color Color::fromTemperature(Real temp, Real minT, Real maxT) {
    // Normalize to [0, 1]
    Real t = (temp - minT) / (maxT - minT);
    t = std::clamp(t, 0.0f, 1.0f);

    // Blue -> Cyan -> Green -> Yellow -> Red
    u32 r, g, b;
    if (t < 0.25f) {
        Real s = t / 0.25f;
        r = 0;
        g = static_cast<u32>(255 * s);
        b = 255;
    } else if (t < 0.5f) {
        Real s = (t - 0.25f) / 0.25f;
        r = 0;
        g = 255;
        b = static_cast<u32>(255 * (1 - s));
    } else if (t < 0.75f) {
        Real s = (t - 0.5f) / 0.25f;
        r = static_cast<u32>(255 * s);
        g = 255;
        b = 0;
    } else {
        Real s = (t - 0.75f) / 0.25f;
        r = 255;
        g = static_cast<u32>(255 * (1 - s));
        b = 0;
    }

    return Color(r, g, b);
}

Color Color::fromSpeed(Real speed, Real maxSpeed) {
    Real t = std::clamp(speed / maxSpeed, 0.0f, 1.0f);

    // Blue (slow) -> Purple -> Red (fast)
    u32 r = static_cast<u32>(100 + t * 155);
    u32 g = static_cast<u32>(150 - t * 100);
    u32 b = static_cast<u32>(255 - t * 200);

    return Color(r, g, b);
}

// ============================================================================
// Math utilities
// ============================================================================

namespace math {

Real randomFloat(Real min, Real max) {
    std::uniform_real_distribution<Real> dist{min, max};
    return dist(g_rng);
}

Real randomGaussian(Real mean, Real stddev) {
    return mean + stddev * g_normalDist(g_rng);
}

} // namespace math

} // namespace eigenlab
