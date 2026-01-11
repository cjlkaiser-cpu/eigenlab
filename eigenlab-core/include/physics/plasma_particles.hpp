#ifndef PLASMA_PARTICLES_HPP
#define PLASMA_PARTICLES_HPP

#include <vector>
#include <cstdint>
#include <random>

/**
 * PlasmaParticles - Charged particle dynamics in electromagnetic fields
 *
 * Physics:
 * - Lorentz force: F = q(E + v × B)
 * - Particle-particle Coulomb interactions (optional)
 * - Magnetic mirror effects
 * - Cyclotron motion and gyration
 *
 * Applications: Tokamak simulation, aurora borealis, particle accelerators
 */

struct PlasmaConfig {
    int numParticles = 500;
    float domainSize = 10.0f;          // Cubic domain side length

    // Electromagnetic fields
    float magneticFieldStrength = 1.0f; // Tesla
    float electricFieldStrength = 0.5f; // V/m
    float magneticFieldAngle = 0.0f;    // Field direction angle (radians)

    // Particle properties
    float electronCharge = -1.0f;       // Normalized charge
    float ionCharge = 1.0f;
    float electronMass = 1.0f;          // Normalized mass
    float ionMass = 1836.0f;            // Proton/electron mass ratio
    float electronRatio = 0.7f;         // Fraction of electrons vs ions

    // Physics settings
    float temperature = 1000.0f;        // Kelvin (affects initial velocities)
    bool enableCoulomb = false;         // Particle-particle interactions
    float coulombStrength = 0.01f;      // Coulomb interaction scale
    bool enableMagneticMirror = false;  // Magnetic mirror confinement
    float mirrorRatio = 2.0f;           // B_max / B_min for mirror

    // Simulation
    float dt = 0.01f;
    bool periodicBoundary = true;
};

struct PlasmaStats {
    float totalKineticEnergy;
    float avgSpeed;
    float maxSpeed;
    float electronTemperature;
    float ionTemperature;
    float gyrationRadius;
    int particleCount;
    int electronCount;
    int ionCount;
};

enum class PlasmaParticleType {
    Electron,
    Ion
};

struct PlasmaParticle {
    float x, y, z;
    float vx, vy, vz;
    float charge;
    float mass;
    PlasmaParticleType type;
    float energy;
};

class PlasmaParticles {
public:
    PlasmaParticles();
    explicit PlasmaParticles(const PlasmaConfig& config);

    void init();
    void step();
    void step(int numSteps);

    // Configuration
    void setMagneticField(float strength, float angle);
    void setElectricField(float strength);
    void setTemperature(float temp);
    void enableCoulombInteractions(bool enable);
    void enableMagneticMirror(bool enable, float ratio);

    // Presets
    void presetTokamak();           // Toroidal confinement
    void presetAurora();            // Magnetic field lines toward Earth
    void presetCyclotron();         // Uniform B, spiral motion
    void presetMagneticBottle();    // Mirror confinement

    // Data access
    const float* positionData() const { return m_positions.data(); }
    const float* velocityData() const { return m_velocities.data(); }
    const float* colorData() const { return m_colors.data(); }
    const float* chargeData() const { return m_charges.data(); }

    int particleCount() const { return static_cast<int>(m_particles.size()); }
    PlasmaStats getStats() const;

    // Field visualization (for debug/display)
    void sampleMagneticField(float x, float y, float z, float& bx, float& by, float& bz) const;
    void sampleElectricField(float x, float y, float z, float& ex, float& ey, float& ez) const;

private:
    PlasmaConfig m_config;
    std::vector<PlasmaParticle> m_particles;

    // Flattened arrays for JS interop
    std::vector<float> m_positions;   // x,y,z per particle
    std::vector<float> m_velocities;  // vx,vy,vz per particle
    std::vector<float> m_colors;      // r,g,b per particle
    std::vector<float> m_charges;     // charge per particle

    std::mt19937 m_rng;
    int m_stepCount;

    void initParticles();
    void updateArrays();

    // Force calculations
    void computeLorentzForce(const PlasmaParticle& p, float& fx, float& fy, float& fz) const;
    void computeCoulombForce(int idx, float& fx, float& fy, float& fz) const;

    // Field functions (can be spatially varying)
    void getMagneticField(float x, float y, float z, float& bx, float& by, float& bz) const;
    void getElectricField(float x, float y, float z, float& ex, float& ey, float& ez) const;

    // Boundary handling
    void applyBoundary(PlasmaParticle& p);

    // Color mapping
    void computeParticleColor(const PlasmaParticle& p, float& r, float& g, float& b) const;
};

#endif // PLASMA_PARTICLES_HPP
