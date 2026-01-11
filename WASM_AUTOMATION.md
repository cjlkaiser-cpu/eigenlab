# EigenLab WASM Automation - Plan de Desarrollo

## Objetivo

Crear 15 simulaciones WASM de alto rendimiento, una por una, con máxima calidad.
Cada simulación incluye: módulo C++, bindings, HTML frontend, indexación en portal.

---

## Estado Actual

### Completadas (7)
- [x] `FluidSolver` - Navier-Stokes 2D (fluidos-2d.html)
- [x] `BoidSystem` - Reynolds flocking (boids-masivo.html)
- [x] `GalaxySimulator` - Barnes-Hut N-body (formacion-galaxias.html)
- [x] `ErosionSimulator` - Erosión hidráulica (erosion-hidraulica.html)
- [x] `ClothSimulator` - Verlet + PBD (simulacion-tela.html)
- [x] `HeatDiffusion` - FTCS Laplacian (difusion-calor-wasm.html)
- [x] `ParticleSystem` - Gas ideal (gas-ideal-wasm.html)

### Pendientes (15 nuevas)
- [ ] 1. `GalaxyCollision` - Colisión de galaxias (50K+ estrellas)
- [ ] 2. `WaveSolver2D` - Ecuación de onda 2D (estanque, interferencia)
- [ ] 3. `MolecularDynamics` - Lennard-Jones (cristalización, fusión)
- [ ] 4. `LatticeBoltzmann` - LBM fluidos (túnel de viento)
- [ ] 5. `Avalanche` - Partículas granulares (arena, rocas)
- [ ] 6. `NeuralNetwork` - Hodgkin-Huxley masivo (1000+ neuronas)
- [ ] 7. `AccretionDisk` - Disco de acreción (agujero negro)
- [ ] 8. `CellularAutomata3D` - Game of Life 3D (128³)
- [ ] 9. `Mandelbulb` - Fractal 3D raymarching
- [ ] 10. `Electrophoresis` - Separación por campo E
- [ ] 11. `PlasmaParticles` - Interacciones Coulomb + B
- [ ] 12. `ProteinFolding` - Minimización energía 2D
- [ ] 13. `Epidemiology` - Propagación SIR espacial
- [ ] 14. `SaturnRings` - Millones de partículas
- [ ] 15. `VectorFields` - Integración de trayectorias

---

## Simulación Actual

**CURRENT_SIM: 1**
**NOMBRE: GalaxyCollision**
**ESTADO: NOT_STARTED**

---

## Especificaciones por Simulación

### 1. GalaxyCollision - Colisión de Galaxias
**Lab destino:** Astronomy Visual Lab
**Archivo HTML:** `colision-galaxias.html`
**Módulo C++:** `galaxy_collision.hpp` / `galaxy_collision.cpp`

**Algoritmo:**
- Barnes-Hut bilateral con 2 centros de masa
- 25K estrellas por galaxia (50K total)
- Parámetros: masa, velocidad de impacto, ángulo
- Visualización: colas de marea, puentes galácticos

**Criterios de completitud:**
- [ ] Header C++ con clase GalaxyCollision
- [ ] Implementación con spatial partitioning
- [ ] Bindings Embind en bindings.cpp
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa (build.sh)
- [ ] HTML con controles interactivos
- [ ] 60 FPS con 20K+ partículas
- [ ] Indexado en wasm-labs.html
- [ ] Commit con mensaje descriptivo

---

### 2. WaveSolver2D - Ondas 2D
**Lab destino:** Physics Visual Lab
**Archivo HTML:** `ondas-2d-wasm.html`
**Módulo C++:** `wave_solver.hpp` / `wave_solver.cpp`

**Algoritmo:**
- PDE: ∂²u/∂t² = c²∇²u
- Grid 512x512
- Diferencias finitas explícitas
- Reflexión en bordes, absorción opcional
- Fuentes puntuales interactivas

**Criterios de completitud:**
- [ ] Header C++ con clase WaveSolver2D
- [ ] Implementación con boundary conditions
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con interacción (click = gota)
- [ ] 60 FPS con 512x512
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 3. MolecularDynamics - Dinámica Molecular
**Lab destino:** Chemistry Visual Lab
**Archivo HTML:** `dinamica-molecular.html`
**Módulo C++:** `molecular_dynamics.hpp` / `molecular_dynamics.cpp`

**Algoritmo:**
- Potencial Lennard-Jones: V(r) = 4ε[(σ/r)¹² - (σ/r)⁶]
- Verlet velocity integration
- Spatial hashing para vecinos
- 5000+ átomos
- Estados: sólido, líquido, gas

**Criterios de completitud:**
- [ ] Header C++ con clase MolecularDynamics
- [ ] Implementación con cell lists
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con control de temperatura
- [ ] Transiciones de fase visibles
- [ ] 60 FPS con 3000+ átomos
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 4. LatticeBoltzmann - Método de Lattice Boltzmann
**Lab destino:** Physics Visual Lab
**Archivo HTML:** `lattice-boltzmann.html`
**Módulo C++:** `lattice_boltzmann.hpp` / `lattice_boltzmann.cpp`

**Algoritmo:**
- D2Q9 lattice
- Collision: BGK operator
- Streaming step
- Bounce-back para obstáculos
- Grid 256x256 o más

**Criterios de completitud:**
- [ ] Header C++ con clase LatticeBoltzmann
- [ ] Implementación D2Q9
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con obstáculos editables
- [ ] Vórtices de Kármán visibles
- [ ] 60 FPS
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 5. Avalanche - Simulación Granular
**Lab destino:** Geology Visual Lab
**Archivo HTML:** `avalancha.html`
**Módulo C++:** `granular_sim.hpp` / `granular_sim.cpp`

**Algoritmo:**
- Position-Based Dynamics
- Fricción estática y dinámica
- 10K+ partículas
- Colisiones partícula-partícula y terreno
- Ángulo de reposo

**Criterios de completitud:**
- [ ] Header C++ con clase GranularSimulator
- [ ] Implementación con friction model
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con terreno editable
- [ ] Comportamiento realista de arena
- [ ] 60 FPS con 5000+ partículas
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 6. NeuralNetwork - Red Neuronal Biológica
**Lab destino:** Biology Visual Lab
**Archivo HTML:** `red-neuronal-wasm.html`
**Módulo C++:** `neural_network.hpp` / `neural_network.cpp`

**Algoritmo:**
- Hodgkin-Huxley para cada neurona
- 1000+ neuronas conectadas
- Sinapsis con delays
- Propagación de spikes
- Plasticidad (STDP opcional)

**Criterios de completitud:**
- [ ] Header C++ con clase BiologicalNeuralNet
- [ ] Implementación HH completa
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con visualización de actividad
- [ ] Patrones de disparo visibles
- [ ] 30+ FPS con 500+ neuronas
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 7. AccretionDisk - Disco de Acreción
**Lab destino:** Astronomy Visual Lab
**Archivo HTML:** `disco-acrecion.html`
**Módulo C++:** `accretion_disk.hpp` / `accretion_disk.cpp`

**Algoritmo:**
- Gravedad pseudo-relativista (Schwarzschild)
- 20K+ partículas en órbita
- Viscosidad efectiva
- Jets polares opcionales
- Lensing gravitacional visual

**Criterios de completitud:**
- [ ] Header C++ con clase AccretionDisk
- [ ] Implementación con metric
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con parámetros del BH
- [ ] Efecto visual de lensing
- [ ] 60 FPS con 10K+ partículas
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 8. CellularAutomata3D - Autómatas 3D
**Lab destino:** Mathematics Visual Lab
**Archivo HTML:** `automata-3d.html`
**Módulo C++:** `cellular_automata_3d.hpp` / `cellular_automata_3d.cpp`

**Algoritmo:**
- Grid cúbico 64³ o 128³
- Reglas tipo Game of Life 3D
- Vecindad de Moore/von Neumann
- Multiple rulesets

**Criterios de completitud:**
- [ ] Header C++ con clase CellularAutomata3D
- [ ] Implementación con double buffering
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con Three.js para render 3D
- [ ] Patrones emergentes visibles
- [ ] 30+ FPS con 64³
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 9. Mandelbulb - Fractal 3D
**Lab destino:** Math Generative Art Lab
**Archivo HTML:** `mandelbulb.html`
**Módulo C++:** `mandelbulb.hpp` / `mandelbulb.cpp`

**Algoritmo:**
- Raymarching con distance estimator
- Triplex algebra
- Potencia n variable (default 8)
- Ambient occlusion
- Normal estimation para shading

**Criterios de completitud:**
- [ ] Header C++ con clase Mandelbulb
- [ ] Implementación raymarching
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con rotación interactiva
- [ ] Render de alta calidad
- [ ] 30+ FPS en 512x512
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 10. Electrophoresis - Electroforesis
**Lab destino:** Biochemistry Visual Lab
**Archivo HTML:** `electroforesis-wasm.html`
**Módulo C++:** `electrophoresis.hpp` / `electrophoresis.cpp`

**Algoritmo:**
- Campo eléctrico uniforme
- Partículas con carga/masa variable
- Difusión + drift
- Gel como medio resistivo
- 10K+ partículas

**Criterios de completitud:**
- [ ] Header C++ con clase Electrophoresis
- [ ] Implementación con mobility
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con control de voltaje
- [ ] Bandas de separación visibles
- [ ] 60 FPS
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 11. PlasmaParticles - Plasma
**Lab destino:** Physics Visual Lab
**Archivo HTML:** `plasma-particulas.html`
**Módulo C++:** `plasma_sim.hpp` / `plasma_sim.cpp`

**Algoritmo:**
- Interacciones Coulomb (N² → N log N con tree)
- Campo magnético externo
- Movimiento ciclotrónico
- 10K+ partículas cargadas

**Criterios de completitud:**
- [ ] Header C++ con clase PlasmaSimulator
- [ ] Implementación con B field
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con control de B
- [ ] Órbitas ciclotrónicas visibles
- [ ] 60 FPS con 5K+ partículas
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 12. ProteinFolding - Plegamiento de Proteínas
**Lab destino:** Biochemistry Visual Lab
**Archivo HTML:** `plegamiento-proteinas.html`
**Módulo C++:** `protein_folding.hpp` / `protein_folding.cpp`

**Algoritmo:**
- Modelo HP (Hydrophobic-Polar) simplificado
- Lattice model o off-lattice
- Minimización de energía (Monte Carlo o gradient)
- Visualización de conformaciones

**Criterios de completitud:**
- [ ] Header C++ con clase ProteinFolder
- [ ] Implementación con energy function
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con secuencia editable
- [ ] Animación de plegamiento
- [ ] Convergencia a mínimo visible
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 13. Epidemiology - Epidemiología Espacial
**Lab destino:** Biology Visual Lab
**Archivo HTML:** `epidemiologia-wasm.html`
**Módulo C++:** `epidemiology.hpp` / `epidemiology.cpp`

**Algoritmo:**
- Modelo SIR/SEIR espacial
- Agentes en grid o continuo
- Contacto por proximidad
- 100K+ agentes
- Parámetros: β, γ, movilidad

**Criterios de completitud:**
- [ ] Header C++ con clase EpidemiologySim
- [ ] Implementación con spatial queries
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con parámetros epidemiológicos
- [ ] Propagación visual de olas
- [ ] 60 FPS con 50K+ agentes
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 14. SaturnRings - Anillos de Saturno
**Lab destino:** Astronomy Visual Lab
**Archivo HTML:** `anillos-saturno.html`
**Módulo C++:** `saturn_rings.hpp` / `saturn_rings.cpp`

**Algoritmo:**
- Millones de partículas en anillo
- Gravedad de Saturno + lunas (resonancias)
- Gaps de Cassini, Encke
- Instanced rendering

**Criterios de completitud:**
- [ ] Header C++ con clase SaturnRings
- [ ] Implementación con ring dynamics
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con vista 3D (Three.js)
- [ ] Estructura de gaps visible
- [ ] 30+ FPS con 100K+ partículas
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

### 15. VectorFields - Campos Vectoriales
**Lab destino:** Mathematics Visual Lab
**Archivo HTML:** `campos-vectoriales-wasm.html`
**Módulo C++:** `vector_fields.hpp` / `vector_fields.cpp`

**Algoritmo:**
- Integración RK4 de miles de trayectorias
- Campos predefinidos (saddle, spiral, etc)
- Campos custom (expresión matemática)
- Flow lines + streamlines

**Criterios de completitud:**
- [ ] Header C++ con clase VectorFieldIntegrator
- [ ] Implementación RK4 batch
- [ ] Bindings Embind
- [ ] CMakeLists.txt actualizado
- [ ] Compilación exitosa
- [ ] HTML con galería de campos
- [ ] 10K+ trayectorias simultáneas
- [ ] 60 FPS
- [ ] Indexado en wasm-labs.html
- [ ] Commit

---

## Archivos a Modificar por Simulación

### C++ (eigenlab-core/)
1. `include/physics/[module].hpp` - Header con clase
2. `src/physics/[module].cpp` - Implementación
3. `src/bindings.cpp` - Agregar bindings Embind
4. `CMakeLists.txt` - Agregar fuentes

### Frontend
5. `[Lab]/[simulation].html` - Interfaz completa

### Portal
6. `_portal/wasm-labs.html` - Agregar card con preview

### Git
7. Commit con formato: `feat(wasm): add [SimName] - [description]`

---

## Patrón de Código C++

```cpp
// include/physics/example.hpp
#pragma once
#include "../core/types.hpp"
#include <vector>

namespace eigenlab {
namespace physics {

class ExampleSim {
public:
    ExampleSim(int width, int height);
    void update(float dt);
    void reset();

    // Getters for JS
    const float* getData() const;
    int getDataSize() const;

    // Setters from JS
    void setParameter(float value);

private:
    int width_, height_;
    std::vector<float> data_;
    // Internal state
};

} // namespace physics
} // namespace eigenlab
```

---

## Patrón de Bindings

```cpp
// En src/bindings.cpp, agregar:

// Helper para datos
val getExampleData(const ExampleSim& sim) {
    return val(typed_memory_view(sim.getDataSize(), sim.getData()));
}

// En EMSCRIPTEN_BINDINGS:
class_<ExampleSim>("ExampleSim")
    .constructor<int, int>()
    .function("update", &ExampleSim::update)
    .function("reset", &ExampleSim::reset)
    .function("setParameter", &ExampleSim::setParameter)
    .function("getData", &getExampleData);
```

---

## Patrón HTML (estructura mínima)

```html
<!DOCTYPE html>
<html lang="es">
<head>
    <title>[Nombre] | EigenLab WASM</title>
    <!-- Estilos estándar EigenLab -->
</head>
<body>
    <header><!-- Breadcrumb + badge WASM --></header>
    <div class="canvas-container">
        <canvas id="canvas"></canvas>
        <div class="hud"><!-- Stats: FPS, partículas, etc --></div>
    </div>
    <aside class="sidebar">
        <!-- Ecuación principal -->
        <!-- Controles (sliders) -->
        <!-- Botones (reset, pause) -->
    </aside>

    <script type="module">
        // 1. Cargar WASM
        import EigenLabCore from '../../_wasm/eigenlab-core.js';

        // 2. Inicializar módulo
        const Module = await EigenLabCore();
        const sim = new Module.ExampleSim(width, height);

        // 3. Loop principal
        function loop() {
            sim.update(dt);
            const data = sim.getData();
            render(data);
            requestAnimationFrame(loop);
        }

        loop();
    </script>
</body>
</html>
```

---

## Comandos de Compilación

```bash
cd /Users/carlos/Projects/EigenLab/eigenlab-core

# Limpiar y recompilar
rm -rf build && mkdir build && cd build
emcmake cmake ..
emmake make -j4

# Copiar a _wasm/
cp eigenlab-core.js eigenlab-core.wasm ../../_wasm/
```

---

## Proceso Ralph Loop

Cada iteración del loop debe:

1. **Leer CURRENT_SIM** para saber cuál simulación trabajar
2. **Verificar estado** - Si NOT_STARTED o IN_PROGRESS, continuar
3. **Implementar paso a paso**:
   - Crear/modificar C++ header
   - Crear/modificar C++ implementation
   - Actualizar bindings.cpp
   - Actualizar CMakeLists.txt
   - Compilar y verificar
   - Crear HTML completo
   - Actualizar wasm-labs.html
   - Hacer commit
4. **Marcar COMPLETED** y actualizar CURRENT_SIM al siguiente
5. **Si todas completadas**, output: `<promise>ALL_WASM_SIMULATIONS_COMPLETE</promise>`

---

## Verificación de Calidad

Cada simulación debe cumplir:
- [ ] Compila sin warnings
- [ ] 60 FPS (o 30 FPS para 3D pesado)
- [ ] Controles responsivos
- [ ] Valores físicos correctos
- [ ] Estilo visual consistente con EigenLab
- [ ] HUD con métricas relevantes
- [ ] Mobile-friendly (touch events)

---

## Progreso Total

**Completadas:** 7/22 (32%)
**Pendientes:** 15

Cuando llegues al final:
```
<promise>ALL_WASM_SIMULATIONS_COMPLETE</promise>
```
