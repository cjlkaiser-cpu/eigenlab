/**
 * @file types.hpp
 * @brief Core types and constants for EigenLab physics engine
 * @version 1.0.0
 * @author EigenLab Team
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <cmath>
#include <algorithm>
#include <vector>
#include <array>

namespace eigenlab {

// ============================================================================
// Type aliases
// ============================================================================

using f32 = float;
using f64 = double;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using usize = size_t;

// Default precision for simulations
using Real = f32;

// ============================================================================
// Constants
// ============================================================================

namespace constants {
    constexpr Real PI = 3.14159265358979323846f;
    constexpr Real TWO_PI = 2.0f * PI;
    constexpr Real HALF_PI = 0.5f * PI;
    constexpr Real E = 2.71828182845904523536f;

    // Physics constants (SI units)
    constexpr Real BOLTZMANN = 1.380649e-23f;      // J/K
    constexpr Real AVOGADRO = 6.02214076e23f;     // mol^-1
    constexpr Real GAS_CONSTANT = 8.314462618f;    // J/(mol·K)

    // Numerical stability
    constexpr Real EPSILON = 1e-7f;
    constexpr Real INF = 1e30f;
}

// ============================================================================
// Vec2 - 2D Vector with SIMD-friendly layout
// ============================================================================

struct alignas(8) Vec2 {
    Real x{0.0f};
    Real y{0.0f};

    constexpr Vec2() = default;
    constexpr Vec2(Real x_, Real y_) : x(x_), y(y_) {}
    constexpr explicit Vec2(Real v) : x(v), y(v) {}

    // Arithmetic operators
    constexpr Vec2 operator+(const Vec2& v) const { return {x + v.x, y + v.y}; }
    constexpr Vec2 operator-(const Vec2& v) const { return {x - v.x, y - v.y}; }
    constexpr Vec2 operator*(Real s) const { return {x * s, y * s}; }
    constexpr Vec2 operator/(Real s) const { return {x / s, y / s}; }
    constexpr Vec2 operator-() const { return {-x, -y}; }

    // Compound assignment
    Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; }
    Vec2& operator-=(const Vec2& v) { x -= v.x; y -= v.y; return *this; }
    Vec2& operator*=(Real s) { x *= s; y *= s; return *this; }
    Vec2& operator/=(Real s) { x /= s; y /= s; return *this; }

    // Vector operations
    [[nodiscard]] constexpr Real dot(const Vec2& v) const {
        return x * v.x + y * v.y;
    }

    [[nodiscard]] constexpr Real cross(const Vec2& v) const {
        return x * v.y - y * v.x;
    }

    [[nodiscard]] Real length() const {
        return std::sqrt(x * x + y * y);
    }

    [[nodiscard]] constexpr Real lengthSquared() const {
        return x * x + y * y;
    }

    [[nodiscard]] Vec2 normalized() const {
        Real len = length();
        if (len > constants::EPSILON) {
            return *this / len;
        }
        return {0.0f, 0.0f};
    }

    void normalize() {
        Real len = length();
        if (len > constants::EPSILON) {
            x /= len;
            y /= len;
        }
    }

    [[nodiscard]] constexpr Vec2 perpendicular() const {
        return {-y, x};
    }

    [[nodiscard]] Vec2 rotated(Real angle) const {
        Real c = std::cos(angle);
        Real s = std::sin(angle);
        return {x * c - y * s, x * s + y * c};
    }

    // Reflection around normal n (must be normalized)
    [[nodiscard]] constexpr Vec2 reflect(const Vec2& n) const {
        return *this - n * (2.0f * dot(n));
    }

    // Static constructors
    static constexpr Vec2 zero() { return {0.0f, 0.0f}; }
    static constexpr Vec2 one() { return {1.0f, 1.0f}; }
    static constexpr Vec2 unitX() { return {1.0f, 0.0f}; }
    static constexpr Vec2 unitY() { return {0.0f, 1.0f}; }

    static Vec2 fromAngle(Real angle) {
        return {std::cos(angle), std::sin(angle)};
    }

    static Vec2 random(Real minVal = 0.0f, Real maxVal = 1.0f);
    static Vec2 randomUnit();
};

inline constexpr Vec2 operator*(Real s, const Vec2& v) { return v * s; }

// ============================================================================
// AABB - Axis-Aligned Bounding Box
// ============================================================================

struct AABB {
    Vec2 min{0.0f, 0.0f};
    Vec2 max{0.0f, 0.0f};

    constexpr AABB() = default;
    constexpr AABB(const Vec2& min_, const Vec2& max_) : min(min_), max(max_) {}
    constexpr AABB(Real x, Real y, Real w, Real h)
        : min(x, y), max(x + w, y + h) {}

    [[nodiscard]] constexpr Vec2 center() const {
        return (min + max) * 0.5f;
    }

    [[nodiscard]] constexpr Vec2 size() const {
        return max - min;
    }

    [[nodiscard]] constexpr Real width() const { return max.x - min.x; }
    [[nodiscard]] constexpr Real height() const { return max.y - min.y; }
    [[nodiscard]] constexpr Real area() const { return width() * height(); }

    [[nodiscard]] constexpr bool contains(const Vec2& p) const {
        return p.x >= min.x && p.x <= max.x &&
               p.y >= min.y && p.y <= max.y;
    }

    [[nodiscard]] constexpr bool intersects(const AABB& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y;
    }

    void expand(const Vec2& point) {
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
    }
};

// ============================================================================
// Color - RGBA color with utility functions
// ============================================================================

struct Color {
    u32 value{0xFF000000}; // ABGR format for canvas ImageData

    constexpr Color() = default;
    constexpr explicit Color(u32 v) : value(v) {}
    constexpr Color(u32 r, u32 g, u32 b, u32 a = 255)
        : value((a << 24) | (b << 16) | (g << 8) | r) {}

    [[nodiscard]] constexpr u32 r() const { return value & 0xFF; }
    [[nodiscard]] constexpr u32 g() const { return (value >> 8) & 0xFF; }
    [[nodiscard]] constexpr u32 b() const { return (value >> 16) & 0xFF; }
    [[nodiscard]] constexpr u32 a() const { return (value >> 24) & 0xFF; }

    // Interpolate between two colors
    [[nodiscard]] static Color lerp(const Color& a, const Color& b, Real t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return Color(
            static_cast<u32>(a.r() + (b.r() - a.r()) * t),
            static_cast<u32>(a.g() + (b.g() - a.g()) * t),
            static_cast<u32>(a.b() + (b.b() - a.b()) * t),
            static_cast<u32>(a.a() + (b.a() - a.a()) * t)
        );
    }

    // HSV to RGB conversion
    [[nodiscard]] static Color fromHSV(Real h, Real s, Real v, Real a = 1.0f);

    // Temperature to color (cold=blue to hot=red)
    [[nodiscard]] static Color fromTemperature(Real t, Real minT = 0.0f, Real maxT = 100.0f);

    // Speed to color (slow=blue to fast=red)
    [[nodiscard]] static Color fromSpeed(Real speed, Real maxSpeed);

    // Predefined colors
    static constexpr Color black() { return Color(0, 0, 0); }
    static constexpr Color white() { return Color(255, 255, 255); }
    static constexpr Color red() { return Color(255, 0, 0); }
    static constexpr Color green() { return Color(0, 255, 0); }
    static constexpr Color blue() { return Color(0, 0, 255); }
    static constexpr Color cyan() { return Color(0, 255, 255); }
    static constexpr Color yellow() { return Color(255, 255, 0); }
    static constexpr Color magenta() { return Color(255, 0, 255); }
};

// ============================================================================
// Utility functions
// ============================================================================

namespace math {
    template<typename T>
    constexpr T clamp(T value, T minVal, T maxVal) {
        return std::max(minVal, std::min(maxVal, value));
    }

    template<typename T>
    constexpr T lerp(T a, T b, Real t) {
        return a + (b - a) * t;
    }

    template<typename T>
    constexpr T smoothstep(T edge0, T edge1, T x) {
        T t = clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
        return t * t * (T(3) - T(2) * t);
    }

    inline Real randomFloat(Real min = 0.0f, Real max = 1.0f);
    inline Real randomGaussian(Real mean = 0.0f, Real stddev = 1.0f);
}

} // namespace eigenlab
