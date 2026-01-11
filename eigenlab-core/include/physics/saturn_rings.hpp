#ifndef SATURN_RINGS_HPP
#define SATURN_RINGS_HPP

#include <vector>
#include <random>

/**
 * SaturnRings - Simulation of Saturn's ring particles
 *
 * Physics:
 * - Keplerian orbital mechanics: v = sqrt(GM/r)
 * - Ring gaps (Cassini Division, Encke Gap)
 * - Particle-particle collisions (optional)
 * - Shepherd moon perturbations (optional)
 * - Color variation based on composition (ice/rock)
 *
 * Applications: Planetary science, orbital mechanics visualization
 */

struct SaturnRingsConfig {
    int numParticles = 5000;

    // Saturn parameters (normalized units)
    float saturnMass = 1.0f;              // Normalized mass
    float saturnRadius = 1.0f;            // Normalized radius
    float gravitationalConstant = 1.0f;   // G in normalized units

    // Ring geometry (in Saturn radii)
    float innerRingRadius = 1.2f;         // D ring inner edge
    float outerRingRadius = 2.3f;         // A ring outer edge
    float ringThickness = 0.05f;          // Vertical spread

    // Ring gaps
    bool enableCassiniDivision = true;    // Gap between A and B rings
    bool enableEnckGap = false;           // Gap in A ring

    // Physics options
    bool enableCollisions = false;        // Particle collisions
    bool enableShepherdMoons = false;     // Mimas, Enceladus perturbations
    float collisionDamping = 0.8f;

    // Simulation
    float dt = 0.001f;
    bool use3D = true;                    // 3D view or top-down
};

struct SaturnRingsStats {
    int particleCount;
    float avgOrbitalVelocity;
    float avgRadius;
    float minRadius;
    float maxRadius;
    float totalEnergy;
    float totalAngularMomentum;
    int collisionCount;
};

struct RingParticle {
    float x, y, z;          // Position
    float vx, vy, vz;       // Velocity
    float radius;           // Orbital radius
    float size;             // Particle size for rendering
    float brightness;       // Color intensity
    bool isIce;             // Ice (bright) vs rock (dark)
};

class SaturnRings {
public:
    SaturnRings();
    explicit SaturnRings(const SaturnRingsConfig& config);

    void init();
    void step();
    void step(int numSteps);

    // Configuration
    void setGravitationalConstant(float g);
    void enableCollisions(bool enable);
    void enableShepherdMoons(bool enable);
    void setRingBounds(float inner, float outer);

    // View controls
    void setViewAngle(float angle);  // Tilt angle
    void setRotation(float angle);   // Rotation around z

    // Presets
    void presetRealistic();          // Realistic Saturn proportions
    void presetDense();              // Dense particle field
    void presetWideRings();          // Extended ring system
    void presetThinRings();          // Just the main rings

    // Data access
    const float* positionData() const { return m_positions.data(); }
    const float* colorData() const { return m_colors.data(); }
    const float* sizeData() const { return m_sizes.data(); }

    int particleCount() const { return static_cast<int>(m_particles.size()); }
    SaturnRingsStats getStats() const;

private:
    SaturnRingsConfig m_config;
    std::vector<RingParticle> m_particles;

    // Flattened arrays for JS interop
    std::vector<float> m_positions;   // x,y,z per particle
    std::vector<float> m_colors;      // r,g,b per particle
    std::vector<float> m_sizes;       // size per particle

    float m_viewAngle;
    float m_rotation;
    int m_collisionCount;

    std::mt19937 m_rng;

    void initParticles();
    void updateArrays();

    // Physics
    void updateOrbits();
    void applyGravity(RingParticle& p);
    void handleCollisions();

    // Helpers
    float orbitalVelocity(float r) const;
    bool isInGap(float r) const;
    void computeParticleColor(const RingParticle& p, float& r, float& g, float& b) const;
};

#endif // SATURN_RINGS_HPP
