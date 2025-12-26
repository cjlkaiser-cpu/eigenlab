/**
 * @file bindings.cpp
 * @brief Emscripten bindings for EigenLab physics engine
 *
 * Exposes C++ classes to JavaScript via Embind
 */

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "../include/core/types.hpp"
#include "../include/physics/particle_system.hpp"
#include "../include/physics/heat_diffusion.hpp"
#include "../include/physics/fluid_solver.hpp"
#include "../include/physics/boids.hpp"
#include "../include/physics/galaxy.hpp"
#include "../include/physics/erosion.hpp"
#include "../include/physics/softbody.hpp"

using namespace emscripten;
using namespace eigenlab;
using namespace eigenlab::physics;

// ============================================================================
// Helper functions for JavaScript interop
// ============================================================================

// Get particle positions as Float32Array
val getParticlePositions(const ParticleSystem& ps) {
    const auto& particles = ps.particles();
    std::vector<float> positions;
    positions.reserve(particles.size() * 2);

    for (const auto& p : particles) {
        if (p.active) {
            positions.push_back(p.position.x);
            positions.push_back(p.position.y);
        }
    }

    return val(typed_memory_view(positions.size(), positions.data()));
}

// Get particle velocities as Float32Array
val getParticleVelocities(const ParticleSystem& ps) {
    const auto& particles = ps.particles();
    std::vector<float> velocities;
    velocities.reserve(particles.size() * 2);

    for (const auto& p : particles) {
        if (p.active) {
            velocities.push_back(p.velocity.x);
            velocities.push_back(p.velocity.y);
        }
    }

    return val(typed_memory_view(velocities.size(), velocities.data()));
}

// Get particle data packed: [x, y, vx, vy, radius, speed]
val getParticleData(const ParticleSystem& ps) {
    const auto& particles = ps.particles();
    std::vector<float> data;
    data.reserve(particles.size() * 6);

    for (const auto& p : particles) {
        if (p.active) {
            data.push_back(p.position.x);
            data.push_back(p.position.y);
            data.push_back(p.velocity.x);
            data.push_back(p.velocity.y);
            data.push_back(p.radius);
            data.push_back(p.speed());
        }
    }

    return val(typed_memory_view(data.size(), data.data()));
}

// Get velocity histogram as Uint32Array
val getVelocityHistogram(const ParticleSystem& ps) {
    const auto& hist = ps.stats().velocityHistogram;
    return val(typed_memory_view(hist.size(), hist.data()));
}

// Get temperature grid as Float32Array
val getTemperatureGrid(const HeatDiffusion& hd) {
    return val(typed_memory_view(hd.cellCount(), hd.temperatureData()));
}

// Get velocity field X component
val getVelocityFieldX(const HeatDiffusion& hd) {
    return val(typed_memory_view(hd.cellCount(), hd.velocityXData()));
}

// Get velocity field Y component
val getVelocityFieldY(const HeatDiffusion& hd) {
    return val(typed_memory_view(hd.cellCount(), hd.velocityYData()));
}

// Render heat map to Uint32Array (RGBA packed)
val renderHeatMap(HeatDiffusion& hd, float minTemp, float maxTemp) {
    std::vector<uint32_t> buffer(hd.cellCount());
    hd.renderToBuffer(buffer.data(), minTemp, maxTemp);
    return val(typed_memory_view(buffer.size(), buffer.data()));
}

// ============================================================================
// Fluid Solver helpers
// ============================================================================

// Get fluid density as Float32Array
val getFluidDensity(const FluidSolver& fs) {
    u32 n = fs.resolution();
    return val(typed_memory_view(n * n, fs.densityData() + n + 3)); // Skip boundary
}

// Get fluid velocity X as Float32Array
val getFluidVelocityX(const FluidSolver& fs) {
    u32 n = fs.resolution();
    return val(typed_memory_view(n * n, fs.velocityXData() + n + 3));
}

// Get fluid velocity Y as Float32Array
val getFluidVelocityY(const FluidSolver& fs) {
    u32 n = fs.resolution();
    return val(typed_memory_view(n * n, fs.velocityYData() + n + 3));
}

// Get RGB density for colored smoke
val getFluidDensityR(const FluidSolver& fs) {
    u32 n = fs.resolution();
    return val(typed_memory_view(n * n, fs.densityRData() + n + 3));
}

val getFluidDensityG(const FluidSolver& fs) {
    u32 n = fs.resolution();
    return val(typed_memory_view(n * n, fs.densityGData() + n + 3));
}

val getFluidDensityB(const FluidSolver& fs) {
    u32 n = fs.resolution();
    return val(typed_memory_view(n * n, fs.densityBData() + n + 3));
}

// Render fluid to buffer
val renderFluidDensity(FluidSolver& fs) {
    u32 n = fs.resolution();
    std::vector<uint32_t> buffer(n * n);
    fs.renderDensity(buffer.data());
    return val(typed_memory_view(buffer.size(), buffer.data()));
}

val renderFluidVelocity(FluidSolver& fs) {
    u32 n = fs.resolution();
    std::vector<uint32_t> buffer(n * n);
    fs.renderVelocity(buffer.data());
    return val(typed_memory_view(buffer.size(), buffer.data()));
}

val renderFluidVorticity(FluidSolver& fs) {
    u32 n = fs.resolution();
    std::vector<uint32_t> buffer(n * n);
    fs.renderVorticity(buffer.data());
    return val(typed_memory_view(buffer.size(), buffer.data()));
}

val renderFluidRainbow(FluidSolver& fs) {
    u32 n = fs.resolution();
    std::vector<uint32_t> buffer(n * n);
    fs.renderRainbow(buffer.data());
    return val(typed_memory_view(buffer.size(), buffer.data()));
}

// ============================================================================
// Boid System helpers
// ============================================================================

// Get boid positions as Float32Array [x0, y0, x1, y1, ...]
val getBoidPositions(const BoidSystem& bs) {
    const auto& boids = bs.boids();
    std::vector<float> positions;
    positions.reserve(boids.size() * 2);

    for (const auto& b : boids) {
        if (b.active) {
            positions.push_back(b.position.x);
            positions.push_back(b.position.y);
        }
    }

    return val(typed_memory_view(positions.size(), positions.data()));
}

// Get boid velocities as Float32Array
val getBoidVelocities(const BoidSystem& bs) {
    const auto& boids = bs.boids();
    std::vector<float> velocities;
    velocities.reserve(boids.size() * 2);

    for (const auto& b : boids) {
        if (b.active) {
            velocities.push_back(b.velocity.x);
            velocities.push_back(b.velocity.y);
        }
    }

    return val(typed_memory_view(velocities.size(), velocities.data()));
}

// Get packed boid data: [x, y, vx, vy, hue, neighbors]
val getBoidData(const BoidSystem& bs) {
    const auto& boids = bs.boids();
    std::vector<float> data;
    data.reserve(boids.size() * 6);

    for (const auto& b : boids) {
        if (b.active) {
            data.push_back(b.position.x);
            data.push_back(b.position.y);
            data.push_back(b.velocity.x);
            data.push_back(b.velocity.y);
            data.push_back(b.hue);
            data.push_back(static_cast<float>(b.neighborCount));
        }
    }

    return val(typed_memory_view(data.size(), data.data()));
}

// Get predator data: [x, y, vx, vy, kills]
val getPredatorData(const BoidSystem& bs) {
    const auto& predators = bs.predators();
    std::vector<float> data;
    data.reserve(predators.size() * 5);

    for (const auto& p : predators) {
        if (p.active) {
            data.push_back(p.position.x);
            data.push_back(p.position.y);
            data.push_back(p.velocity.x);
            data.push_back(p.velocity.y);
            data.push_back(static_cast<float>(p.kills));
        }
    }

    return val(typed_memory_view(data.size(), data.data()));
}

// Get obstacle data: [x, y, radius]
val getObstacleData(const BoidSystem& bs) {
    const auto& obstacles = bs.obstacles();
    std::vector<float> data;
    data.reserve(obstacles.size() * 3);

    for (const auto& o : obstacles) {
        if (o.active) {
            data.push_back(o.position.x);
            data.push_back(o.position.y);
            data.push_back(o.radius);
        }
    }

    return val(typed_memory_view(data.size(), data.data()));
}

// ============================================================================
// Galaxy Simulator helpers
// ============================================================================

// Get galaxy particle data: [x, y, vx, vy, mass, type, temperature]
val getGalaxyData(const GalaxySimulator& gs) {
    const auto& particles = gs.particles();
    std::vector<float> data;
    data.reserve(particles.size() * 7);

    for (const auto& p : particles) {
        if (p.active) {
            data.push_back(p.position.x);
            data.push_back(p.position.y);
            data.push_back(p.velocity.x);
            data.push_back(p.velocity.y);
            data.push_back(p.mass);
            data.push_back(static_cast<float>(p.type));
            data.push_back(p.temperature);
        }
    }

    return val(typed_memory_view(data.size(), data.data()));
}

// Get only positions for fast rendering
val getGalaxyPositions(const GalaxySimulator& gs) {
    const auto& particles = gs.particles();
    std::vector<float> positions;
    positions.reserve(particles.size() * 2);

    for (const auto& p : particles) {
        if (p.active) {
            positions.push_back(p.position.x);
            positions.push_back(p.position.y);
        }
    }

    return val(typed_memory_view(positions.size(), positions.data()));
}

// ============================================================================
// Erosion Simulator helpers
// ============================================================================

val getErosionHeightMap(const ErosionSimulator& es) {
    return val(typed_memory_view(es.cellCount(), es.heightData()));
}

val getErosionWaterMap(const ErosionSimulator& es) {
    return val(typed_memory_view(es.cellCount(), es.waterData()));
}

val renderErosionTerrain(ErosionSimulator& es) {
    std::vector<uint32_t> buffer(es.cellCount());
    es.renderHeightMap(buffer.data());
    return val(typed_memory_view(buffer.size(), buffer.data()));
}

val renderErosionWater(ErosionSimulator& es) {
    std::vector<uint32_t> buffer(es.cellCount());
    es.renderWaterMap(buffer.data());
    return val(typed_memory_view(buffer.size(), buffer.data()));
}

val renderErosionSlope(ErosionSimulator& es) {
    std::vector<uint32_t> buffer(es.cellCount());
    es.renderSlopeMap(buffer.data());
    return val(typed_memory_view(buffer.size(), buffer.data()));
}

// ============================================================================
// Cloth Simulator helpers
// ============================================================================

// Get particle data: [x, y, prevX, prevY, pinned]
val getClothParticleData(const ClothSimulator& cs) {
    const auto& particles = cs.getParticles();
    std::vector<float> data;
    data.reserve(particles.size() * 5);

    for (const auto& p : particles) {
        data.push_back(p.x);
        data.push_back(p.y);
        data.push_back(p.prevX);
        data.push_back(p.prevY);
        data.push_back(p.pinned ? 1.0f : 0.0f);
    }

    return val(typed_memory_view(data.size(), data.data()));
}

// Get only positions for fast rendering: [x0, y0, x1, y1, ...]
val getClothPositions(const ClothSimulator& cs) {
    const auto& particles = cs.getParticles();
    std::vector<float> positions;
    positions.reserve(particles.size() * 2);

    for (const auto& p : particles) {
        positions.push_back(p.x);
        positions.push_back(p.y);
    }

    return val(typed_memory_view(positions.size(), positions.data()));
}

// Get spring data: [p1, p2, restLength, type, active]
val getClothSpringData(const ClothSimulator& cs) {
    const auto& springs = cs.getSprings();
    std::vector<float> data;
    data.reserve(springs.size() * 5);

    for (const auto& s : springs) {
        data.push_back(static_cast<float>(s.p1));
        data.push_back(static_cast<float>(s.p2));
        data.push_back(s.restLength);
        data.push_back(static_cast<float>(s.type));
        data.push_back(s.active ? 1.0f : 0.0f);
    }

    return val(typed_memory_view(data.size(), data.data()));
}

// Get active structural springs for rendering
val getClothStructuralSprings(const ClothSimulator& cs) {
    const auto& springs = cs.getSprings();
    std::vector<u32> indices;
    indices.reserve(springs.size() * 2);

    for (const auto& s : springs) {
        if (s.active && s.type == SpringType::Structural) {
            indices.push_back(s.p1);
            indices.push_back(s.p2);
        }
    }

    return val(typed_memory_view(indices.size(), indices.data()));
}

// Get all active springs by type
val getClothSpringsByType(const ClothSimulator& cs, u8 type) {
    const auto& springs = cs.getSprings();
    std::vector<u32> indices;
    indices.reserve(springs.size() * 2);

    for (const auto& s : springs) {
        if (s.active && static_cast<u8>(s.type) == type) {
            indices.push_back(s.p1);
            indices.push_back(s.p2);
        }
    }

    return val(typed_memory_view(indices.size(), indices.data()));
}

// ============================================================================
// Embind definitions
// ============================================================================

EMSCRIPTEN_BINDINGS(eigenlab_core) {

    // Vec2
    value_object<Vec2>("Vec2")
        .field("x", &Vec2::x)
        .field("y", &Vec2::y);

    // AABB
    value_object<AABB>("AABB")
        .field("min", &AABB::min)
        .field("max", &AABB::max);

    // ParticleSystemConfig
    value_object<ParticleSystemConfig>("ParticleSystemConfig")
        .field("bounds", &ParticleSystemConfig::bounds)
        .field("particleRadius", &ParticleSystemConfig::particleRadius)
        .field("particleMass", &ParticleSystemConfig::particleMass)
        .field("restitution", &ParticleSystemConfig::restitution)
        .field("wallRestitution", &ParticleSystemConfig::wallRestitution)
        .field("temperature", &ParticleSystemConfig::temperature)
        .field("molecularMass", &ParticleSystemConfig::molecularMass)
        .field("maxParticles", &ParticleSystemConfig::maxParticles)
        .field("maxSpeed", &ParticleSystemConfig::maxSpeed);

    // ParticleSystemStats
    value_object<ParticleSystemStats>("ParticleSystemStats")
        .field("totalKineticEnergy", &ParticleSystemStats::totalKineticEnergy)
        .field("averageKineticEnergy", &ParticleSystemStats::averageKineticEnergy)
        .field("temperature", &ParticleSystemStats::temperature)
        .field("pressure", &ParticleSystemStats::pressure)
        .field("averageSpeed", &ParticleSystemStats::averageSpeed)
        .field("rmsSpeed", &ParticleSystemStats::rmsSpeed)
        .field("maxSpeed", &ParticleSystemStats::maxSpeed)
        .field("particleCollisions", &ParticleSystemStats::particleCollisions)
        .field("wallCollisions", &ParticleSystemStats::wallCollisions);

    // ParticleSystem
    class_<ParticleSystem>("ParticleSystem")
        .constructor<>()
        .constructor<const ParticleSystemConfig&>()
        .function("setConfig", &ParticleSystem::setConfig)
        .function("clear", &ParticleSystem::clear)
        .function("particleCount", &ParticleSystem::particleCount)
        .function("initializeRandom", &ParticleSystem::initializeRandom)
        .function("initializeGrid", &ParticleSystem::initializeGrid)
        .function("initializeMaxwellBoltzmann", &ParticleSystem::initializeMaxwellBoltzmann)
        .function("step", &ParticleSystem::step)
        .function("stepSubdivided", &ParticleSystem::stepSubdivided)
        .function("setTemperature", &ParticleSystem::setTemperature)
        .function("scaleVelocitiesToTemperature", &ParticleSystem::scaleVelocitiesToTemperature)
        .function("setBounds", &ParticleSystem::setBounds)
        .function("setVolume", &ParticleSystem::setVolume)
        .function("computeStatistics", &ParticleSystem::computeStatistics)
        .function("computeVelocityHistogram", &ParticleSystem::computeVelocityHistogram)
        .function("stats", &ParticleSystem::stats);

    // HeatGridConfig
    value_object<HeatGridConfig>("HeatGridConfig")
        .field("width", &HeatGridConfig::width)
        .field("height", &HeatGridConfig::height)
        .field("resolutionX", &HeatGridConfig::resolutionX)
        .field("resolutionY", &HeatGridConfig::resolutionY)
        .field("thermalDiffusivity", &HeatGridConfig::thermalDiffusivity)
        .field("ambientTemperature", &HeatGridConfig::ambientTemperature)
        .field("convectionStrength", &HeatGridConfig::convectionStrength)
        .field("buoyancyFactor", &HeatGridConfig::buoyancyFactor);

    // HeatGridStats
    value_object<HeatGridStats>("HeatGridStats")
        .field("averageTemperature", &HeatGridStats::averageTemperature)
        .field("minTemperature", &HeatGridStats::minTemperature)
        .field("maxTemperature", &HeatGridStats::maxTemperature)
        .field("totalHeat", &HeatGridStats::totalHeat)
        .field("heatFluxMagnitude", &HeatGridStats::heatFluxMagnitude)
        .field("dtCFL", &HeatGridStats::dtCFL)
        .field("iterations", &HeatGridStats::iterations)
        .field("simulationTime", &HeatGridStats::simulationTime);

    // HeatSource
    value_object<HeatSource>("HeatSource")
        .field("position", &HeatSource::position)
        .field("radius", &HeatSource::radius)
        .field("temperature", &HeatSource::temperature)
        .field("isActive", &HeatSource::isActive);

    // HeatDiffusion Mode enum
    enum_<HeatDiffusion::Mode>("HeatDiffusionMode")
        .value("Conduction", HeatDiffusion::Mode::Conduction)
        .value("Convection", HeatDiffusion::Mode::Convection);

    // HeatDiffusion
    class_<HeatDiffusion>("HeatDiffusion")
        .constructor<>()
        .constructor<const HeatGridConfig&>()
        .function("setConfig", &HeatDiffusion::setConfig)
        .function("resize", &HeatDiffusion::resize)
        .function("setMode", &HeatDiffusion::setMode)
        .function("mode", &HeatDiffusion::mode)
        .function("addSource", &HeatDiffusion::addSource)
        .function("removeSource", &HeatDiffusion::removeSource)
        .function("clearSources", &HeatDiffusion::clearSources)
        .function("setSourceTemperature", &HeatDiffusion::setSourceTemperature)
        .function("reset", &HeatDiffusion::reset)
        .function("setUniform", &HeatDiffusion::setUniform)
        .function("step", &HeatDiffusion::step)
        .function("stepMultiple", &HeatDiffusion::stepMultiple)
        .function("computeStableDt", &HeatDiffusion::computeStableDt)
        .function("temperatureAtGrid", select_overload<Real(u32, u32) const>(&HeatDiffusion::temperatureAt))
        .function("setTemperatureAt", select_overload<void(u32, u32, Real)>(&HeatDiffusion::setTemperatureAt))
        .function("computeStatistics", &HeatDiffusion::computeStatistics)
        .function("stats", &HeatDiffusion::stats)
        .function("width", &HeatDiffusion::width)
        .function("height", &HeatDiffusion::height)
        .function("cellCount", &HeatDiffusion::cellCount);

    // Helper functions
    function("getParticlePositions", &getParticlePositions);
    function("getParticleVelocities", &getParticleVelocities);
    function("getParticleData", &getParticleData);
    function("getVelocityHistogram", &getVelocityHistogram);
    function("getTemperatureGrid", &getTemperatureGrid);
    function("getVelocityFieldX", &getVelocityFieldX);
    function("getVelocityFieldY", &getVelocityFieldY);
    function("renderHeatMap", &renderHeatMap);

    // ========================================================================
    // FluidSolver
    // ========================================================================

    // FluidConfig
    value_object<FluidConfig>("FluidConfig")
        .field("resolution", &FluidConfig::resolution)
        .field("viscosity", &FluidConfig::viscosity)
        .field("diffusion", &FluidConfig::diffusion)
        .field("dissipation", &FluidConfig::dissipation)
        .field("densityDissipation", &FluidConfig::densityDissipation)
        .field("vorticityStrength", &FluidConfig::vorticityStrength)
        .field("pressureIterations", &FluidConfig::pressureIterations)
        .field("diffuseIterations", &FluidConfig::diffuseIterations)
        .field("enableVorticity", &FluidConfig::enableVorticity)
        .field("boundaryWalls", &FluidConfig::boundaryWalls);

    // FluidStats
    value_object<FluidStats>("FluidStats")
        .field("maxVelocity", &FluidStats::maxVelocity)
        .field("averageVelocity", &FluidStats::averageVelocity)
        .field("totalDensity", &FluidStats::totalDensity)
        .field("maxVorticity", &FluidStats::maxVorticity)
        .field("kineticEnergy", &FluidStats::kineticEnergy)
        .field("iterations", &FluidStats::iterations)
        .field("simulationTime", &FluidStats::simulationTime);

    // FluidSolver class
    class_<FluidSolver>("FluidSolver")
        .constructor<>()
        .constructor<const FluidConfig&>()
        .function("setConfig", &FluidSolver::setConfig)
        .function("resize", &FluidSolver::resize)
        .function("clear", &FluidSolver::clear)
        .function("clearVelocity", &FluidSolver::clearVelocity)
        .function("clearDensity", &FluidSolver::clearDensity)
        .function("step", &FluidSolver::step)
        .function("addDensity", &FluidSolver::addDensity)
        .function("addVelocity", &FluidSolver::addVelocity)
        .function("addForce", &FluidSolver::addForce)
        .function("addColoredDensity", &FluidSolver::addColoredDensity)
        .function("applyImpulse", &FluidSolver::applyImpulse)
        .function("computeStatistics", &FluidSolver::computeStatistics)
        .function("stats", &FluidSolver::stats)
        .function("resolution", &FluidSolver::resolution)
        .function("cellCount", &FluidSolver::cellCount);

    // Fluid helper functions
    function("getFluidDensity", &getFluidDensity);
    function("getFluidVelocityX", &getFluidVelocityX);
    function("getFluidVelocityY", &getFluidVelocityY);
    function("getFluidDensityR", &getFluidDensityR);
    function("getFluidDensityG", &getFluidDensityG);
    function("getFluidDensityB", &getFluidDensityB);
    function("renderFluidDensity", &renderFluidDensity);
    function("renderFluidVelocity", &renderFluidVelocity);
    function("renderFluidVorticity", &renderFluidVorticity);
    function("renderFluidRainbow", &renderFluidRainbow);

    // Fluid presets
    function("fluidPresetSmoke", &fluid_presets::smoke);
    function("fluidPresetWater", &fluid_presets::water);
    function("fluidPresetFire", &fluid_presets::fire);
    function("fluidPresetInk", &fluid_presets::ink);
    function("fluidPresetTurbulent", &fluid_presets::turbulent);

    // ========================================================================
    // BoidSystem
    // ========================================================================

    // BoidConfig
    value_object<BoidConfig>("BoidConfig")
        .field("bounds", &BoidConfig::bounds)
        .field("maxSpeed", &BoidConfig::maxSpeed)
        .field("minSpeed", &BoidConfig::minSpeed)
        .field("maxForce", &BoidConfig::maxForce)
        .field("perceptionRadius", &BoidConfig::perceptionRadius)
        .field("separationRadius", &BoidConfig::separationRadius)
        .field("fieldOfView", &BoidConfig::fieldOfView)
        .field("separationWeight", &BoidConfig::separationWeight)
        .field("alignmentWeight", &BoidConfig::alignmentWeight)
        .field("cohesionWeight", &BoidConfig::cohesionWeight)
        .field("avoidWallsWeight", &BoidConfig::avoidWallsWeight)
        .field("wallMargin", &BoidConfig::wallMargin)
        .field("noiseWeight", &BoidConfig::noiseWeight)
        .field("maxBoids", &BoidConfig::maxBoids)
        .field("spatialGridCellSize", &BoidConfig::spatialGridCellSize)
        .field("wrapEdges", &BoidConfig::wrapEdges);

    // BoidStats
    value_object<BoidStats>("BoidStats")
        .field("activeBoids", &BoidStats::activeBoids)
        .field("activePredators", &BoidStats::activePredators)
        .field("averageSpeed", &BoidStats::averageSpeed)
        .field("averageNeighbors", &BoidStats::averageNeighbors)
        .field("flockCenter", &BoidStats::flockCenter)
        .field("flockSpread", &BoidStats::flockSpread)
        .field("polarization", &BoidStats::polarization)
        .field("kills", &BoidStats::kills)
        .field("simulationTime", &BoidStats::simulationTime);

    // BoidSystem class
    class_<BoidSystem>("BoidSystem")
        .constructor<>()
        .constructor<const BoidConfig&>()
        .function("setConfig", &BoidSystem::setConfig)
        .function("clear", &BoidSystem::clear)
        .function("initializeRandom", &BoidSystem::initializeRandom)
        .function("initializeCircle", &BoidSystem::initializeCircle)
        .function("initializeGrid", &BoidSystem::initializeGrid)
        .function("addBoid", &BoidSystem::addBoid)
        .function("addPredator", &BoidSystem::addPredator)
        .function("addObstacle", &BoidSystem::addObstacle)
        .function("addAttractor", &BoidSystem::addAttractor)
        .function("removeBoid", &BoidSystem::removeBoid)
        .function("removePredator", &BoidSystem::removePredator)
        .function("removeObstacle", &BoidSystem::removeObstacle)
        .function("removeAttractor", &BoidSystem::removeAttractor)
        .function("clearPredators", &BoidSystem::clearPredators)
        .function("clearObstacles", &BoidSystem::clearObstacles)
        .function("clearAttractors", &BoidSystem::clearAttractors)
        .function("step", &BoidSystem::step)
        .function("stepMultiple", &BoidSystem::stepMultiple)
        .function("computeStatistics", &BoidSystem::computeStatistics)
        .function("stats", &BoidSystem::stats)
        .function("boidCount", &BoidSystem::boidCount)
        .function("predatorCount", &BoidSystem::predatorCount)
        .function("obstacleCount", &BoidSystem::obstacleCount)
        .function("attractorCount", &BoidSystem::attractorCount)
        .function("setSeparationWeight", &BoidSystem::setSeparationWeight)
        .function("setAlignmentWeight", &BoidSystem::setAlignmentWeight)
        .function("setCohesionWeight", &BoidSystem::setCohesionWeight)
        .function("setPerceptionRadius", &BoidSystem::setPerceptionRadius)
        .function("setMaxSpeed", &BoidSystem::setMaxSpeed)
        .function("setMaxForce", &BoidSystem::setMaxForce);

    // Boid helper functions
    function("getBoidPositions", &getBoidPositions);
    function("getBoidVelocities", &getBoidVelocities);
    function("getBoidData", &getBoidData);
    function("getPredatorData", &getPredatorData);
    function("getObstacleData", &getObstacleData);

    // Boid presets
    function("boidPresetBirds", &boid_presets::birds);
    function("boidPresetFish", &boid_presets::fish);
    function("boidPresetInsects", &boid_presets::insects);
    function("boidPresetSlow", &boid_presets::slow);
    function("boidPresetChaos", &boid_presets::chaos);

    // ========================================================================
    // GalaxySimulator
    // ========================================================================

    // ParticleType enum
    enum_<ParticleType>("ParticleType")
        .value("Star", ParticleType::Star)
        .value("DarkMatter", ParticleType::DarkMatter)
        .value("Gas", ParticleType::Gas)
        .value("BlackHole", ParticleType::BlackHole);

    // GalaxyConfig
    value_object<GalaxyConfig>("GalaxyConfig")
        .field("size", &GalaxyConfig::size)
        .field("gravitationalConstant", &GalaxyConfig::gravitationalConstant)
        .field("softeningLength", &GalaxyConfig::softeningLength)
        .field("timeScale", &GalaxyConfig::timeScale)
        .field("theta", &GalaxyConfig::theta)
        .field("maxParticles", &GalaxyConfig::maxParticles)
        .field("minMass", &GalaxyConfig::minMass)
        .field("maxMass", &GalaxyConfig::maxMass)
        .field("blackHoleMass", &GalaxyConfig::blackHoleMass)
        .field("blackHoleRadius", &GalaxyConfig::blackHoleRadius)
        .field("centralBlackHole", &GalaxyConfig::centralBlackHole)
        .field("velocityScale", &GalaxyConfig::velocityScale);

    // GalaxyStats
    value_object<GalaxyStats>("GalaxyStats")
        .field("activeParticles", &GalaxyStats::activeParticles)
        .field("totalMass", &GalaxyStats::totalMass)
        .field("totalEnergy", &GalaxyStats::totalEnergy)
        .field("kineticEnergy", &GalaxyStats::kineticEnergy)
        .field("potentialEnergy", &GalaxyStats::potentialEnergy)
        .field("centerOfMass", &GalaxyStats::centerOfMass)
        .field("angularMomentum", &GalaxyStats::angularMomentum)
        .field("maxVelocity", &GalaxyStats::maxVelocity)
        .field("treeNodes", &GalaxyStats::treeNodes)
        .field("simulationTime", &GalaxyStats::simulationTime);

    // GalaxySimulator class
    class_<GalaxySimulator>("GalaxySimulator")
        .constructor<>()
        .constructor<const GalaxyConfig&>()
        .function("setConfig", &GalaxySimulator::setConfig)
        .function("clear", &GalaxySimulator::clear)
        .function("initializeDiskGalaxy", &GalaxySimulator::initializeDiskGalaxy)
        .function("initializeEllipticalGalaxy", &GalaxySimulator::initializeEllipticalGalaxy)
        .function("initializeCollision", &GalaxySimulator::initializeCollision)
        .function("initializeRandom", &GalaxySimulator::initializeRandom)
        .function("addParticle", &GalaxySimulator::addParticle)
        .function("addBlackHole", &GalaxySimulator::addBlackHole)
        .function("removeParticle", &GalaxySimulator::removeParticle)
        .function("step", &GalaxySimulator::step)
        .function("stepMultiple", &GalaxySimulator::stepMultiple)
        .function("computeStatistics", &GalaxySimulator::computeStatistics)
        .function("stats", &GalaxySimulator::stats)
        .function("particleCount", &GalaxySimulator::particleCount)
        .function("setGravitationalConstant", &GalaxySimulator::setGravitationalConstant)
        .function("setSofteningLength", &GalaxySimulator::setSofteningLength)
        .function("setTheta", &GalaxySimulator::setTheta)
        .function("setTimeScale", &GalaxySimulator::setTimeScale);

    // Galaxy helper functions
    function("getGalaxyData", &getGalaxyData);
    function("getGalaxyPositions", &getGalaxyPositions);

    // Galaxy presets
    function("galaxyPresetMilkyWay", &galaxy_presets::milkyWay);
    function("galaxyPresetAndromeda", &galaxy_presets::andromeda);
    function("galaxyPresetDwarf", &galaxy_presets::dwarf);
    function("galaxyPresetCollision", &galaxy_presets::collision);
    function("galaxyPresetCluster", &galaxy_presets::cluster);

    // ========================================================================
    // ErosionSimulator
    // ========================================================================

    // ErosionConfig
    value_object<ErosionConfig>("ErosionConfig")
        .field("width", &ErosionConfig::width)
        .field("height", &ErosionConfig::height)
        .field("cellSize", &ErosionConfig::cellSize)
        .field("inertia", &ErosionConfig::inertia)
        .field("sedimentCapacity", &ErosionConfig::sedimentCapacity)
        .field("minSedimentCapacity", &ErosionConfig::minSedimentCapacity)
        .field("depositSpeed", &ErosionConfig::depositSpeed)
        .field("erodeSpeed", &ErosionConfig::erodeSpeed)
        .field("gravity", &ErosionConfig::gravity)
        .field("evaporateSpeed", &ErosionConfig::evaporateSpeed)
        .field("minSlope", &ErosionConfig::minSlope)
        .field("maxLifetime", &ErosionConfig::maxLifetime)
        .field("initialWater", &ErosionConfig::initialWater)
        .field("initialVelocity", &ErosionConfig::initialVelocity)
        .field("erosionRadius", &ErosionConfig::erosionRadius)
        .field("talusAngle", &ErosionConfig::talusAngle)
        .field("thermalRate", &ErosionConfig::thermalRate)
        .field("maxDroplets", &ErosionConfig::maxDroplets);

    // ErosionStats
    value_object<ErosionStats>("ErosionStats")
        .field("activeDroplets", &ErosionStats::activeDroplets)
        .field("totalDroplets", &ErosionStats::totalDroplets)
        .field("totalErosion", &ErosionStats::totalErosion)
        .field("totalDeposition", &ErosionStats::totalDeposition)
        .field("minHeight", &ErosionStats::minHeight)
        .field("maxHeight", &ErosionStats::maxHeight)
        .field("averageHeight", &ErosionStats::averageHeight)
        .field("iterations", &ErosionStats::iterations);

    // ErosionSimulator class
    class_<ErosionSimulator>("ErosionSimulator")
        .constructor<>()
        .constructor<const ErosionConfig&>()
        .function("setConfig", &ErosionSimulator::setConfig)
        .function("initializeFlat", &ErosionSimulator::initializeFlat)
        .function("initializeNoise", &ErosionSimulator::initializeNoise)
        .function("initializeMountain", &ErosionSimulator::initializeMountain)
        .function("initializeRidges", &ErosionSimulator::initializeRidges)
        .function("raise", &ErosionSimulator::raise)
        .function("lower", &ErosionSimulator::lower)
        .function("flatten", &ErosionSimulator::flatten)
        .function("setHeight", &ErosionSimulator::setHeight)
        .function("simulateDroplet", &ErosionSimulator::simulateDroplet)
        .function("simulateDroplets", &ErosionSimulator::simulateDroplets)
        .function("simulateThermalErosion", &ErosionSimulator::simulateThermalErosion)
        .function("step", &ErosionSimulator::step)
        .function("computeStatistics", &ErosionSimulator::computeStatistics)
        .function("stats", &ErosionSimulator::stats)
        .function("width", &ErosionSimulator::width)
        .function("height", &ErosionSimulator::height)
        .function("cellCount", &ErosionSimulator::cellCount)
        .function("heightAt", &ErosionSimulator::heightAt)
        .function("heightAtInterp", &ErosionSimulator::heightAtInterp)
        .function("setInertia", &ErosionSimulator::setInertia)
        .function("setErosionSpeed", &ErosionSimulator::setErosionSpeed)
        .function("setDepositSpeed", &ErosionSimulator::setDepositSpeed)
        .function("setEvaporateSpeed", &ErosionSimulator::setEvaporateSpeed)
        .function("setCapacity", &ErosionSimulator::setCapacity)
        .function("setErosionRadius", &ErosionSimulator::setErosionRadius);

    // Erosion helper functions
    function("getErosionHeightMap", &getErosionHeightMap);
    function("getErosionWaterMap", &getErosionWaterMap);
    function("renderErosionTerrain", &renderErosionTerrain);
    function("renderErosionWater", &renderErosionWater);
    function("renderErosionSlope", &renderErosionSlope);

    // Erosion presets
    function("erosionPresetGentle", &erosion_presets::gentle);
    function("erosionPresetRiver", &erosion_presets::river);
    function("erosionPresetCanyon", &erosion_presets::canyon);
    function("erosionPresetCoastal", &erosion_presets::coastal);
    function("erosionPresetVolcanic", &erosion_presets::volcanic);

    // ========================================================================
    // ClothSimulator (Softbody/Cloth)
    // ========================================================================

    // SpringType enum
    enum_<SpringType>("SpringType")
        .value("Structural", SpringType::Structural)
        .value("Shear", SpringType::Shear)
        .value("Bend", SpringType::Bend);

    // ClothConfig
    value_object<ClothConfig>("ClothConfig")
        .field("width", &ClothConfig::width)
        .field("height", &ClothConfig::height)
        .field("spacing", &ClothConfig::spacing)
        .field("gravity", &ClothConfig::gravity)
        .field("damping", &ClothConfig::damping)
        .field("stiffness", &ClothConfig::stiffness)
        .field("iterations", &ClothConfig::iterations)
        .field("windX", &ClothConfig::windX)
        .field("windY", &ClothConfig::windY)
        .field("windTurbulence", &ClothConfig::windTurbulence)
        .field("structuralStiffness", &ClothConfig::structuralStiffness)
        .field("shearStiffness", &ClothConfig::shearStiffness)
        .field("bendStiffness", &ClothConfig::bendStiffness)
        .field("sphereX", &ClothConfig::sphereX)
        .field("sphereY", &ClothConfig::sphereY)
        .field("sphereRadius", &ClothConfig::sphereRadius)
        .field("sphereEnabled", &ClothConfig::sphereEnabled)
        .field("tearThreshold", &ClothConfig::tearThreshold)
        .field("showStructural", &ClothConfig::showStructural)
        .field("showShear", &ClothConfig::showShear)
        .field("showBend", &ClothConfig::showBend)
        .field("showParticles", &ClothConfig::showParticles);

    // ClothSimulator class
    class_<ClothSimulator>("ClothSimulator")
        .constructor<>()
        .function("initialize", &ClothSimulator::initialize)
        .function("update", &ClothSimulator::update)
        .function("applyForce", &ClothSimulator::applyForce)
        .function("applyForceAt", &ClothSimulator::applyForceAt)
        .function("setPinned", &ClothSimulator::setPinned)
        .function("setPinnedIndex", &ClothSimulator::setPinnedIndex)
        .function("movePinned", &ClothSimulator::movePinned)
        .function("grabNearest", &ClothSimulator::grabNearest)
        .function("moveGrabbed", &ClothSimulator::moveGrabbed)
        .function("releaseGrabbed", &ClothSimulator::releaseGrabbed)
        .function("reset", &ClothSimulator::reset)
        .function("setPreset", &ClothSimulator::setPreset)
        .function("setGravity", &ClothSimulator::setGravity)
        .function("setDamping", &ClothSimulator::setDamping)
        .function("setStiffness", &ClothSimulator::setStiffness)
        .function("setIterations", &ClothSimulator::setIterations)
        .function("setWind", &ClothSimulator::setWind)
        .function("setWindTurbulence", &ClothSimulator::setWindTurbulence)
        .function("setSphereCollision", &ClothSimulator::setSphereCollision)
        .function("setTearThreshold", &ClothSimulator::setTearThreshold)
        .function("getConfig", &ClothSimulator::getConfig)
        .function("getParticleCount", &ClothSimulator::getParticleCount)
        .function("getSpringCount", &ClothSimulator::getSpringCount)
        .function("getActiveSpringCount", &ClothSimulator::getActiveSpringCount)
        .function("getIndex", &ClothSimulator::getIndex);

    // Cloth helper functions
    function("getClothParticleData", &getClothParticleData);
    function("getClothPositions", &getClothPositions);
    function("getClothSpringData", &getClothSpringData);
    function("getClothStructuralSprings", &getClothStructuralSprings);
    function("getClothSpringsByType", &getClothSpringsByType);

    // Cloth presets
    function("clothPresetSilk", &cloth_presets::silk);
    function("clothPresetDenim", &cloth_presets::denim);
    function("clothPresetRubber", &cloth_presets::rubber);
    function("clothPresetFlag", &cloth_presets::flag);
    function("clothPresetNet", &cloth_presets::net);
    function("clothPresetCobweb", &cloth_presets::cobweb);

    // Constants
    constant("BOLTZMANN", constants::BOLTZMANN);
    constant("PI", constants::PI);
}
