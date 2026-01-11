#ifndef VECTOR_FIELDS_HPP
#define VECTOR_FIELDS_HPP

#include <vector>
#include <cmath>
#include <functional>

/**
 * VectorFields - Interactive visualization of 2D vector fields
 *
 * Physics/Math:
 * - Field types: Flow, Rotation, Sink/Source, Dipole, Curl, Gradient
 * - Particle advection using RK4 integration
 * - Field arithmetic (add, combine)
 * - Divergence and curl computation
 *
 * Applications: Fluid visualization, EM fields, mathematical education
 */

enum class FieldType {
    Uniform,       // Constant direction flow
    Rotation,      // Circular rotation (vortex)
    Source,        // Radial outward flow
    Sink,          // Radial inward flow
    Dipole,        // Two opposite sources
    Saddle,        // Hyperbolic flow
    SinCos,        // Trigonometric pattern
    Gradient,      // Scalar field gradient
    Custom         // User-defined
};

struct VectorFieldConfig {
    int gridWidth = 40;
    int gridHeight = 30;
    int numParticles = 500;
    float domainWidth = 10.0f;
    float domainHeight = 7.5f;

    // Field parameters
    FieldType fieldType = FieldType::Rotation;
    float fieldStrength = 1.0f;
    float centerX = 0.0f;
    float centerY = 0.0f;

    // For dipole
    float dipoleDistance = 2.0f;

    // For sincos
    float frequency = 1.0f;

    // Particle advection
    float dt = 0.02f;
    float particleLifetime = 5.0f;
    float particleSpeed = 1.0f;

    // Visualization
    bool showVectors = true;
    bool showParticles = true;
    bool showStreamlines = false;
    float arrowScale = 0.3f;
};

struct VectorFieldStats {
    int particleCount;
    float maxMagnitude;
    float avgMagnitude;
    float divergence;   // Approx at center
    float curl;         // Approx at center
    float simTime;
};

struct FieldParticle {
    float x, y;        // Position
    float age;         // Time alive
    float brightness;  // For fading
};

class VectorFields {
public:
    VectorFields();
    explicit VectorFields(const VectorFieldConfig& config);

    void init();
    void step();
    void step(int numSteps);

    // Field configuration
    void setFieldType(FieldType type);
    void setFieldStrength(float strength);
    void setFieldCenter(float x, float y);
    void setFrequency(float freq);
    void setDipoleDistance(float dist);

    // Particle controls
    void resetParticles();
    void addParticles(int count);
    void setParticleSpeed(float speed);
    void setParticleLifetime(float lifetime);

    // Visualization
    void setShowVectors(bool show);
    void setShowParticles(bool show);
    void setArrowScale(float scale);

    // Field presets
    void presetRotation();       // Circular vortex
    void presetSource();         // Radial source
    void presetSink();           // Radial sink
    void presetDipole();         // Electric dipole
    void presetSaddle();         // Saddle point
    void presetSinCos();         // Trigonometric
    void presetUniform();        // Uniform flow
    void presetVortexPair();     // Two counter-rotating vortices

    // Data access
    const float* vectorFieldData() const { return m_fieldData.data(); }
    const float* particlePositionData() const { return m_particlePos.data(); }
    const float* particleBrightnessData() const { return m_particleBright.data(); }

    int gridWidth() const { return m_config.gridWidth; }
    int gridHeight() const { return m_config.gridHeight; }
    int gridSize() const { return m_config.gridWidth * m_config.gridHeight; }
    int particleCount() const { return static_cast<int>(m_particles.size()); }

    VectorFieldStats getStats() const;

private:
    VectorFieldConfig m_config;
    std::vector<FieldParticle> m_particles;

    // Flattened arrays for JS
    std::vector<float> m_fieldData;      // vx, vy per grid point
    std::vector<float> m_particlePos;    // x, y per particle
    std::vector<float> m_particleBright; // brightness per particle

    float m_simTime;

    // Field evaluation
    void evaluateFieldAt(float x, float y, float& vx, float& vy) const;
    void updateFieldGrid();
    void updateParticles();
    void spawnParticle(FieldParticle& p);

    // Field functions by type
    void fieldUniform(float x, float y, float& vx, float& vy) const;
    void fieldRotation(float x, float y, float& vx, float& vy) const;
    void fieldSource(float x, float y, float& vx, float& vy) const;
    void fieldSink(float x, float y, float& vx, float& vy) const;
    void fieldDipole(float x, float y, float& vx, float& vy) const;
    void fieldSaddle(float x, float y, float& vx, float& vy) const;
    void fieldSinCos(float x, float y, float& vx, float& vy) const;

    // RK4 integration
    void advectRK4(FieldParticle& p, float dt);

    // Random number generation
    float randomFloat();
    unsigned int m_seed;
};

#endif // VECTOR_FIELDS_HPP
