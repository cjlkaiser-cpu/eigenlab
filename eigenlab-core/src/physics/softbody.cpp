#include "physics/softbody.hpp"
#include <cmath>
#include <algorithm>

namespace eigenlab {
namespace physics {

ClothSimulator::ClothSimulator()
    : randState_(12345) {
}

void ClothSimulator::initialize(const ClothConfig& config) {
    config_ = config;
    initialConfig_ = config;

    const u32 numParticles = config_.width * config_.height;
    particles_.resize(numParticles);
    springs_.clear();

    // Calculate starting position to center the cloth
    const f32 totalWidth = (config_.width - 1) * config_.spacing;
    const f32 startX = 100.0f; // Left margin

    // Initialize particles in a grid
    for (u32 row = 0; row < config_.height; ++row) {
        for (u32 col = 0; col < config_.width; ++col) {
            const u32 idx = getIndex(col, row);
            ClothParticle& p = particles_[idx];

            p.x = startX + col * config_.spacing;
            p.y = 50.0f + row * config_.spacing;
            p.prevX = p.x;
            p.prevY = p.y;
            p.accX = 0.0f;
            p.accY = 0.0f;
            p.mass = 1.0f;
            p.pinned = false;
            p.row = row;
            p.col = col;
        }
    }

    // Pin the top row by default
    for (u32 col = 0; col < config_.width; ++col) {
        // Pin every few particles on the top row
        if (col % 5 == 0 || col == config_.width - 1) {
            particles_[getIndex(col, 0)].pinned = true;
            particles_[getIndex(col, 0)].mass = 0.0f; // Infinite mass
        }
    }

    // Create spring connections
    createSprings();

    // Store initial state for reset
    initialParticles_ = particles_;
}

void ClothSimulator::createSprings() {
    springs_.clear();

    for (u32 row = 0; row < config_.height; ++row) {
        for (u32 col = 0; col < config_.width; ++col) {
            const u32 idx = getIndex(col, row);

            // Structural springs (horizontal and vertical)
            if (col < config_.width - 1) {
                addSpring(idx, getIndex(col + 1, row),
                         SpringType::Structural, config_.structuralStiffness);
            }
            if (row < config_.height - 1) {
                addSpring(idx, getIndex(col, row + 1),
                         SpringType::Structural, config_.structuralStiffness);
            }

            // Shear springs (diagonals)
            if (col < config_.width - 1 && row < config_.height - 1) {
                addSpring(idx, getIndex(col + 1, row + 1),
                         SpringType::Shear, config_.shearStiffness);
            }
            if (col > 0 && row < config_.height - 1) {
                addSpring(idx, getIndex(col - 1, row + 1),
                         SpringType::Shear, config_.shearStiffness);
            }

            // Bend springs (skip one particle)
            if (col < config_.width - 2) {
                addSpring(idx, getIndex(col + 2, row),
                         SpringType::Bend, config_.bendStiffness);
            }
            if (row < config_.height - 2) {
                addSpring(idx, getIndex(col, row + 2),
                         SpringType::Bend, config_.bendStiffness);
            }
        }
    }
}

void ClothSimulator::addSpring(u32 p1, u32 p2, SpringType type, f32 stiffness) {
    const ClothParticle& a = particles_[p1];
    const ClothParticle& b = particles_[p2];

    const f32 dx = b.x - a.x;
    const f32 dy = b.y - a.y;
    const f32 restLength = std::sqrt(dx * dx + dy * dy);

    springs_.emplace_back(p1, p2, restLength, stiffness, type);
}

void ClothSimulator::update(f32 dt) {
    // Substeps for stability
    const f32 subDt = dt / 2.0f;
    for (int substep = 0; substep < 2; ++substep) {
        // Apply external forces (gravity, wind)
        applyExternalForces(subDt);

        // Verlet integration
        integrate(subDt);

        // Constraint solving iterations
        for (u32 iter = 0; iter < config_.iterations; ++iter) {
            solveConstraints();
        }

        // Collision handling
        if (config_.sphereEnabled) {
            handleSphereCollision();
        }
        handleBoundaryCollisions();

        // Check for tearing
        if (config_.tearThreshold > 0.0f) {
            checkTearing();
        }
    }
}

void ClothSimulator::applyExternalForces(f32 dt) {
    for (auto& p : particles_) {
        if (p.pinned) continue;

        // Reset acceleration
        p.accX = 0.0f;
        p.accY = 0.0f;

        // Gravity
        p.accY += config_.gravity;

        // Wind force with turbulence
        f32 windX = config_.windX;
        f32 windY = config_.windY;

        if (config_.windTurbulence > 0.0f) {
            windX += (randomFloat() - 0.5f) * 2.0f * config_.windTurbulence;
            windY += (randomFloat() - 0.5f) * 2.0f * config_.windTurbulence;
        }

        p.accX += windX;
        p.accY += windY;
    }
}

void ClothSimulator::integrate(f32 dt) {
    const f32 dt2 = dt * dt;

    for (auto& p : particles_) {
        if (p.pinned) continue;

        // Verlet integration: x_new = 2x - x_old + a*dt²
        const f32 newX = p.x + (p.x - p.prevX) * config_.damping + p.accX * dt2;
        const f32 newY = p.y + (p.y - p.prevY) * config_.damping + p.accY * dt2;

        p.prevX = p.x;
        p.prevY = p.y;
        p.x = newX;
        p.y = newY;
    }
}

void ClothSimulator::solveConstraints() {
    for (auto& spring : springs_) {
        if (!spring.active) continue;
        solveSpring(spring);
    }
}

void ClothSimulator::solveSpring(Spring& spring) {
    ClothParticle& p1 = particles_[spring.p1];
    ClothParticle& p2 = particles_[spring.p2];

    const f32 dx = p2.x - p1.x;
    const f32 dy = p2.y - p1.y;
    const f32 dist = std::sqrt(dx * dx + dy * dy);

    if (dist < 0.0001f) return; // Avoid division by zero

    // Calculate how much we need to correct
    const f32 diff = (dist - spring.restLength) / dist;
    const f32 correctionX = dx * diff * 0.5f * spring.stiffness * config_.stiffness;
    const f32 correctionY = dy * diff * 0.5f * spring.stiffness * config_.stiffness;

    // Apply corrections based on mass
    if (!p1.pinned && !p2.pinned) {
        // Both free: split correction equally
        p1.x += correctionX;
        p1.y += correctionY;
        p2.x -= correctionX;
        p2.y -= correctionY;
    } else if (!p1.pinned) {
        // p2 is pinned: p1 takes full correction
        p1.x += correctionX * 2.0f;
        p1.y += correctionY * 2.0f;
    } else if (!p2.pinned) {
        // p1 is pinned: p2 takes full correction
        p2.x -= correctionX * 2.0f;
        p2.y -= correctionY * 2.0f;
    }
}

void ClothSimulator::handleSphereCollision() {
    for (auto& p : particles_) {
        if (p.pinned) continue;

        const f32 dx = p.x - config_.sphereX;
        const f32 dy = p.y - config_.sphereY;
        const f32 dist = std::sqrt(dx * dx + dy * dy);

        if (dist < config_.sphereRadius) {
            // Push particle out of sphere
            const f32 pushDist = config_.sphereRadius - dist;
            if (dist > 0.0001f) {
                p.x += (dx / dist) * pushDist * 1.1f;
                p.y += (dy / dist) * pushDist * 1.1f;
            } else {
                p.y -= pushDist * 1.1f;
            }
        }
    }
}

void ClothSimulator::handleBoundaryCollisions() {
    // Assume canvas is 800x600 or similar
    const f32 minX = 5.0f;
    const f32 maxX = 795.0f;
    const f32 minY = 5.0f;
    const f32 maxY = 595.0f;

    for (auto& p : particles_) {
        if (p.pinned) continue;

        if (p.x < minX) {
            p.x = minX;
            p.prevX = p.x;
        }
        if (p.x > maxX) {
            p.x = maxX;
            p.prevX = p.x;
        }
        if (p.y < minY) {
            p.y = minY;
            p.prevY = p.y;
        }
        if (p.y > maxY) {
            p.y = maxY;
            p.prevY = p.y;
        }
    }
}

void ClothSimulator::checkTearing() {
    for (auto& spring : springs_) {
        if (!spring.active) continue;

        const ClothParticle& p1 = particles_[spring.p1];
        const ClothParticle& p2 = particles_[spring.p2];

        const f32 dx = p2.x - p1.x;
        const f32 dy = p2.y - p1.y;
        const f32 dist = std::sqrt(dx * dx + dy * dy);

        // Tear if stretched beyond threshold
        if (dist > spring.restLength * (1.0f + config_.tearThreshold)) {
            spring.active = false;
        }
    }
}

void ClothSimulator::applyForce(f32 fx, f32 fy) {
    for (auto& p : particles_) {
        if (!p.pinned) {
            p.accX += fx;
            p.accY += fy;
        }
    }
}

void ClothSimulator::applyForceAt(f32 x, f32 y, f32 fx, f32 fy, f32 radius) {
    const f32 radius2 = radius * radius;

    for (auto& p : particles_) {
        if (p.pinned) continue;

        const f32 dx = p.x - x;
        const f32 dy = p.y - y;
        const f32 dist2 = dx * dx + dy * dy;

        if (dist2 < radius2) {
            // Force falls off with distance
            const f32 factor = 1.0f - std::sqrt(dist2) / radius;
            p.x += fx * factor;
            p.y += fy * factor;
        }
    }
}

void ClothSimulator::setPinned(u32 col, u32 row, bool pinned) {
    if (col >= config_.width || row >= config_.height) return;

    const u32 idx = getIndex(col, row);
    particles_[idx].pinned = pinned;
    particles_[idx].mass = pinned ? 0.0f : 1.0f;
}

void ClothSimulator::setPinnedIndex(u32 index, bool pinned) {
    if (index >= particles_.size()) return;

    particles_[index].pinned = pinned;
    particles_[index].mass = pinned ? 0.0f : 1.0f;
}

void ClothSimulator::movePinned(u32 index, f32 x, f32 y) {
    if (index >= particles_.size()) return;

    ClothParticle& p = particles_[index];
    if (p.pinned) {
        p.x = x;
        p.y = y;
        p.prevX = x;
        p.prevY = y;
    }
}

i32 ClothSimulator::grabNearest(f32 x, f32 y, f32 maxDist) {
    i32 nearest = -1;
    f32 minDist2 = maxDist * maxDist;

    for (usize i = 0; i < particles_.size(); ++i) {
        const ClothParticle& p = particles_[i];
        const f32 dx = p.x - x;
        const f32 dy = p.y - y;
        const f32 dist2 = dx * dx + dy * dy;

        if (dist2 < minDist2) {
            minDist2 = dist2;
            nearest = static_cast<i32>(i);
        }
    }

    return nearest;
}

void ClothSimulator::moveGrabbed(u32 index, f32 x, f32 y) {
    if (index >= particles_.size()) return;

    ClothParticle& p = particles_[index];
    p.x = x;
    p.y = y;
}

void ClothSimulator::releaseGrabbed(u32 index) {
    if (index >= particles_.size()) return;
    // Reset velocity by setting previous position to current
    ClothParticle& p = particles_[index];
    p.prevX = p.x;
    p.prevY = p.y;
}

void ClothSimulator::reset() {
    config_ = initialConfig_;
    particles_ = initialParticles_;

    // Reactivate all springs
    for (auto& spring : springs_) {
        spring.active = true;
    }
}

void ClothSimulator::setPreset(const std::string& name) {
    ClothConfig newConfig;

    if (name == "silk") {
        newConfig = cloth_presets::silk();
    } else if (name == "denim") {
        newConfig = cloth_presets::denim();
    } else if (name == "rubber") {
        newConfig = cloth_presets::rubber();
    } else if (name == "flag") {
        newConfig = cloth_presets::flag();
    } else if (name == "net") {
        newConfig = cloth_presets::net();
    } else if (name == "cobweb") {
        newConfig = cloth_presets::cobweb();
    } else {
        return; // Unknown preset
    }

    // Preserve grid dimensions
    newConfig.width = config_.width;
    newConfig.height = config_.height;
    newConfig.spacing = config_.spacing;

    initialize(newConfig);
}

void ClothSimulator::setSphereCollision(f32 x, f32 y, f32 r, bool enabled) {
    config_.sphereX = x;
    config_.sphereY = y;
    config_.sphereRadius = r;
    config_.sphereEnabled = enabled;
}

usize ClothSimulator::getActiveSpringCount() const {
    usize count = 0;
    for (const auto& s : springs_) {
        if (s.active) ++count;
    }
    return count;
}

f32 ClothSimulator::randomFloat() {
    // Simple LCG random
    randState_ = randState_ * 1103515245 + 12345;
    return static_cast<f32>((randState_ >> 16) & 0x7FFF) / 32767.0f;
}

// ============================================================
// Presets
// ============================================================

namespace cloth_presets {

ClothConfig silk() {
    ClothConfig cfg;
    cfg.gravity = 600.0f;        // Light weight
    cfg.damping = 0.97f;         // High damping for smoothness
    cfg.stiffness = 0.3f;        // Low stiffness = flowy
    cfg.iterations = 6;
    cfg.structuralStiffness = 0.8f;
    cfg.shearStiffness = 0.5f;
    cfg.bendStiffness = 0.1f;    // Very flexible
    cfg.windX = 50.0f;
    cfg.windY = -20.0f;
    cfg.windTurbulence = 30.0f;
    return cfg;
}

ClothConfig denim() {
    ClothConfig cfg;
    cfg.gravity = 1200.0f;       // Heavy
    cfg.damping = 0.995f;        // Low energy loss
    cfg.stiffness = 0.9f;        // Very stiff
    cfg.iterations = 12;
    cfg.structuralStiffness = 1.0f;
    cfg.shearStiffness = 0.95f;
    cfg.bendStiffness = 0.8f;    // Resists bending
    return cfg;
}

ClothConfig rubber() {
    ClothConfig cfg;
    cfg.gravity = 800.0f;
    cfg.damping = 0.98f;
    cfg.stiffness = 0.2f;        // Very stretchy
    cfg.iterations = 4;          // Few iterations = more stretch
    cfg.structuralStiffness = 0.3f;
    cfg.shearStiffness = 0.2f;
    cfg.bendStiffness = 0.1f;
    return cfg;
}

ClothConfig flag() {
    ClothConfig cfg;
    cfg.gravity = 200.0f;        // Light
    cfg.damping = 0.98f;
    cfg.stiffness = 0.6f;
    cfg.iterations = 8;
    cfg.structuralStiffness = 1.0f;
    cfg.shearStiffness = 0.7f;
    cfg.bendStiffness = 0.2f;
    cfg.windX = 200.0f;          // Strong wind
    cfg.windY = 0.0f;
    cfg.windTurbulence = 80.0f;  // Gusty
    return cfg;
}

ClothConfig net() {
    ClothConfig cfg;
    cfg.gravity = 400.0f;
    cfg.damping = 0.99f;
    cfg.stiffness = 0.7f;
    cfg.iterations = 10;
    cfg.structuralStiffness = 1.0f;
    cfg.shearStiffness = 0.1f;   // No shear resistance
    cfg.bendStiffness = 0.05f;   // No bend resistance
    cfg.showShear = false;
    return cfg;
}

ClothConfig cobweb() {
    ClothConfig cfg;
    cfg.gravity = 100.0f;        // Very light
    cfg.damping = 0.96f;
    cfg.stiffness = 0.5f;
    cfg.iterations = 6;
    cfg.structuralStiffness = 0.6f;
    cfg.shearStiffness = 0.4f;
    cfg.bendStiffness = 0.1f;
    cfg.tearThreshold = 0.5f;    // Tears easily
    cfg.windTurbulence = 10.0f;
    return cfg;
}

} // namespace cloth_presets

} // namespace physics
} // namespace eigenlab
