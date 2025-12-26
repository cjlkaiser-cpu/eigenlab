#pragma once

#include "core/types.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace eigenlab {
namespace physics {

/**
 * Configuration for cloth/softbody simulation
 */
struct ClothConfig {
    // Grid dimensions
    u32 width = 40;          // Number of particles horizontally
    u32 height = 30;         // Number of particles vertically
    f32 spacing = 10.0f;     // Rest distance between particles

    // Physics
    f32 gravity = 980.0f;    // Gravity acceleration (cm/s²)
    f32 damping = 0.99f;     // Velocity damping per frame
    f32 stiffness = 0.5f;    // Constraint stiffness (0-1)
    u32 iterations = 8;      // Constraint solver iterations

    // Wind
    f32 windX = 0.0f;        // Wind force X
    f32 windY = 0.0f;        // Wind force Y
    f32 windTurbulence = 0.0f; // Wind randomness

    // Springs
    f32 structuralStiffness = 1.0f;  // Direct neighbor springs
    f32 shearStiffness = 0.8f;       // Diagonal springs
    f32 bendStiffness = 0.3f;        // Skip-one springs for bending

    // Collision
    f32 sphereX = 400.0f;    // Collision sphere center X
    f32 sphereY = 400.0f;    // Collision sphere center Y
    f32 sphereRadius = 80.0f; // Collision sphere radius
    bool sphereEnabled = false;

    // Rendering hints
    bool showStructural = true;
    bool showShear = false;
    bool showBend = false;
    bool showParticles = true;

    // Interaction
    f32 tearThreshold = 0.0f; // 0 = no tearing, > 0 = tear when stretched this much
};

/**
 * A single particle in the cloth
 */
struct ClothParticle {
    f32 x, y;           // Current position
    f32 prevX, prevY;   // Previous position (for Verlet)
    f32 accX, accY;     // Accumulated acceleration
    f32 mass;           // Particle mass (0 = pinned/infinite mass)
    bool pinned;        // Is this particle fixed in place?
    u32 row, col;       // Grid position

    ClothParticle() : x(0), y(0), prevX(0), prevY(0),
                      accX(0), accY(0), mass(1.0f), pinned(false),
                      row(0), col(0) {}
};

/**
 * Spring constraint types
 */
enum class SpringType : u8 {
    Structural = 0,  // Direct neighbors (horizontal/vertical)
    Shear = 1,       // Diagonal neighbors
    Bend = 2         // Skip-one for bending resistance
};

/**
 * A distance constraint between two particles
 */
struct Spring {
    u32 p1, p2;         // Particle indices
    f32 restLength;     // Rest distance
    f32 stiffness;      // Spring stiffness
    SpringType type;    // Type of spring
    bool active;        // Is this spring still intact?

    Spring() : p1(0), p2(0), restLength(0), stiffness(1.0f),
               type(SpringType::Structural), active(true) {}

    Spring(u32 a, u32 b, f32 len, f32 stiff, SpringType t)
        : p1(a), p2(b), restLength(len), stiffness(stiff), type(t), active(true) {}
};

/**
 * Cloth simulation using Position-Based Dynamics (PBD)
 *
 * The cloth is represented as a grid of particles connected by springs.
 * We use Verlet integration for stability and iteratively solve
 * distance constraints.
 *
 * Spring types:
 * - Structural: Connect direct neighbors, resist stretching
 * - Shear: Connect diagonal neighbors, resist shearing
 * - Bend: Connect particles 2 apart, resist bending
 */
class ClothSimulator {
public:
    ClothSimulator();
    ~ClothSimulator() = default;

    /**
     * Initialize the cloth grid
     * @param config Simulation parameters
     */
    void initialize(const ClothConfig& config);

    /**
     * Perform one simulation step
     * @param dt Time step in seconds
     */
    void update(f32 dt);

    /**
     * Apply an external force to all particles
     */
    void applyForce(f32 fx, f32 fy);

    /**
     * Apply force at a specific point with radius
     */
    void applyForceAt(f32 x, f32 y, f32 fx, f32 fy, f32 radius);

    /**
     * Pin/unpin a particle at grid position
     */
    void setPinned(u32 col, u32 row, bool pinned);

    /**
     * Pin/unpin a particle by index
     */
    void setPinnedIndex(u32 index, bool pinned);

    /**
     * Move a pinned particle to new position
     */
    void movePinned(u32 index, f32 x, f32 y);

    /**
     * Grab nearest particle to position
     * @return Particle index or -1 if none found
     */
    i32 grabNearest(f32 x, f32 y, f32 maxDist);

    /**
     * Move a grabbed particle
     */
    void moveGrabbed(u32 index, f32 x, f32 y);

    /**
     * Release a grabbed particle
     */
    void releaseGrabbed(u32 index);

    /**
     * Reset to initial configuration
     */
    void reset();

    /**
     * Set preset configuration
     */
    void setPreset(const std::string& name);

    // Configuration
    void setGravity(f32 g) { config_.gravity = g; }
    void setDamping(f32 d) { config_.damping = d; }
    void setStiffness(f32 s) { config_.stiffness = s; }
    void setIterations(u32 n) { config_.iterations = n; }
    void setWind(f32 wx, f32 wy) { config_.windX = wx; config_.windY = wy; }
    void setWindTurbulence(f32 t) { config_.windTurbulence = t; }
    void setSphereCollision(f32 x, f32 y, f32 r, bool enabled);
    void setTearThreshold(f32 t) { config_.tearThreshold = t; }

    // Getters
    const ClothConfig& getConfig() const { return config_; }
    usize getParticleCount() const { return particles_.size(); }
    usize getSpringCount() const { return springs_.size(); }
    usize getActiveSpringCount() const;

    const ClothParticle& getParticle(u32 index) const { return particles_[index]; }
    const Spring& getSpring(u32 index) const { return springs_[index]; }

    // Data access for rendering
    const std::vector<ClothParticle>& getParticles() const { return particles_; }
    const std::vector<Spring>& getSprings() const { return springs_; }

    // Get particle index from grid position
    u32 getIndex(u32 col, u32 row) const { return row * config_.width + col; }

private:
    ClothConfig config_;
    ClothConfig initialConfig_;
    std::vector<ClothParticle> particles_;
    std::vector<ClothParticle> initialParticles_;
    std::vector<Spring> springs_;

    // Random number generator state
    u32 randState_;

    // Create the spring network
    void createSprings();

    // Add a spring between two particles
    void addSpring(u32 p1, u32 p2, SpringType type, f32 stiffness);

    // Verlet integration step
    void integrate(f32 dt);

    // Apply external forces
    void applyExternalForces(f32 dt);

    // Solve distance constraints
    void solveConstraints();

    // Solve a single spring constraint
    void solveSpring(Spring& spring);

    // Handle collisions with sphere
    void handleSphereCollision();

    // Handle boundary collisions
    void handleBoundaryCollisions();

    // Check and handle tearing
    void checkTearing();

    // Random number generation
    f32 randomFloat();
};

// Preset configurations
namespace cloth_presets {
    ClothConfig silk();      // Light, flowing fabric
    ClothConfig denim();     // Heavy, stiff fabric
    ClothConfig rubber();    // Stretchy material
    ClothConfig flag();      // For flag simulation with wind
    ClothConfig net();       // Fishing net / hammock
    ClothConfig cobweb();    // Fragile spider web
}

} // namespace physics
} // namespace eigenlab
