# EigenLab - Roadmap

Plan de desarrollo y crecimiento del ecosistema de laboratorios virtuales.

---

## Estado Actual (Marzo 2026)

### Inventario Real por Laboratorio

| Lab | Sims | Guías | Cobertura |
|-----|:----:|:-----:|:---------:|
| Physics Visual Lab | 30 | 5 | 17% |
| Physics Sound Lab | 14 | 0 | 0% |
| Chemistry Visual Lab | 19 | ~17 | 89% |
| Biochemistry Visual Lab | 18 | 16 | 89% |
| Biology Visual Lab | 11 | ~8 | 73% |
| Geology Visual Lab | 10 | ~6 | 60% |
| Math Visual Lab | 28 | 27 | 96% |
| Math Generative Art Lab | 9 | 0 | 0% |
| Math Sound Lab | 11 | 0 | 0% |
| Astronomy Visual Lab | 11 | ~7 | 64% |
| Astronomy Sound Lab | 3 | 3 | 100% |
| Computation Lab | 6 | 6 | 100% |
| AI Visual Lab | 20 | 21 | 100% |
| Music Theory Lab | 9 | 0 | 0% |

**Total: ~199 simulaciones · 91 guías · 46% cobertura global**

> Cursos: Chaos Lab (10 atractores + 30 lecciones), Platonic Lab (poliedros 4D + 33 lecciones), Euler Lab (100 problemas, 4 niveles).

---

### Motor C++/WebAssembly (eigenlab-core)

**22 módulos C++ compilados. 7 desplegados como simulación.**

| Módulo | Sim HTML | Estado |
|--------|----------|:------:|
| `FluidSolver` | `fluidos-2d.html` | ✅ |
| `BoidSystem` | `boids-masivo.html` | ✅ |
| `GalaxySimulator` | `formacion-galaxias.html` | ✅ |
| `ErosionSimulator` | `erosion-hidraulica.html` | ✅ |
| `ClothSimulator` | `simulacion-tela.html` | ✅ |
| `SaturnRings` | `anillos-saturno.html` | ✅ |
| `VectorFields` | `campos-vectoriales.html` | ✅ |
| `Mandelbulb` | — | 💤 |
| `MolecularDynamics` | — | 💤 |
| `Epidemiology` | — | 💤 |
| `ProteinFolding` | — | 💤 |
| `AccretionDisk` | — | 💤 |
| `LatticeBotzmann` | — | 💤 |
| `PlasmaParticles` | — | 💤 |
| `GalaxyCollision` | — | 💤 |
| `GranularSim` | — | 💤 |
| `CellularAutomata3D` | — | 💤 |
| `NeuralNetwork` | — | 💤 |
| `HeatDiffusion` | — | 💤 |
| `WaveSolver` | — | 💤 |
| `ParticleSystem` | — | 💤 |
| `Electrophoresis` | — | 💤 |

**Compilacion:**
```bash
cd eigenlab-core && mkdir build && cd build
emcmake cmake .. && emmake make -j4
```

---

## Filosofia

> **Profundidad antes que amplitud.**
> Cada simulacion debe poder responder la pregunta: "¿que pasa si cambio esto?"
> Cada guia debe poder responder: "¿por que importa esto?"
> El ecosistema se nutre a si mismo: las guias revelan gaps, los gaps generan nuevas simulaciones.

---

## FASE ACTIVA I: Consolidacion Pedagogica

La mayor deuda tecnica del proyecto. Laboratorios con alto valor pedagogico y baja cobertura de guias.

### Prioridad 1 — Physics Visual Lab (25 guias pendientes)

El lab mas visitado y mas fundamental del ecosistema. Base de todo lo demas.

Simulaciones sin guia:
- `pendulo-simple.html`, `pendulos-desacoplados.html`
- `ondas.html`, `interferencia.html`, `optica.html`
- `campo-electrico.html`, `circuitos.html`, `relatividad.html`
- `termodinamica.html`, `entropia.html`, `gas-ideal.html`
- `fluidos-2d.html`, `simulacion-tela.html`, `lattice-boltzmann.html`
- `particulas-plasma.html`, `orbitas-kepler.html`
- `proyectil.html`, `mecanica-lagrangiana.html`, `oscilador-forzado.html`
- `efecto-doppler.html`, `efecto-tunel.html`, `colisiones.html`
- `difusion-calor.html`, `sistema-solar.html`, `atractor-multicuerpo.html`

Cada guia debe incluir: ecuacion principal explicada, estados visuales, experimento guiado, conexiones con otras sims, limitaciones del modelo.

### Prioridad 2 — Music Theory Lab (9 guias pendientes)

El laboratorio con mayor densidad teorica por sim (Sistema Armonico Aureo). Sin ninguna guia de entrada, es inaccesible para nuevos usuarios.

Simulaciones: `escala-cromatica-aurea`, `escala-15-notas`, `escala-12-phiW`, `armonizador-aureo`, `armonizador-15-notas`, `armonizador-12-phiW`, `compositor-aureo`, `compositor-15-notas`, `compositor-12-phiW`.

Enfoque sugerido: guia unificada de introduccion a phi como operador musical + guia individual por herramienta (escala, armonizador, compositor).

### Prioridad 3 — Math Sound + Generative Art (20 guias pendientes)

Math Sound Lab (11 sims): conexion entre matematicas y musica. Audiencia amplia.
Math Generative Art Lab (9 sims): arte generativo, muy visual, facil de documentar.

---

## FASE ACTIVA II: WASM Durmientes

15 modulos C++ compilados sin simulacion HTML. Activarlos es alto impacto con bajo coste de implementacion (el motor ya existe).

### Tier A — Desplegar ya (maxima espectacularidad + valor pedagogico)

#### Mandelbulb (Mathematics)
Fractal 3D en tiempo real. Raymarching sobre el conjunto de Mandelbrot en R³.
- Rotacion interactiva, zoom, control de potencia n
- Unico en el ecosistema: el unico fractal volumetrico
- Conexiones: Mandelbrot 2D, Julia, caos, dominio complejo

#### Molecular Dynamics (Chemistry / Biochemistry)
Dinamica molecular con potencial Lennard-Jones.
- Visualizar cristalizacion, fusion, presion, temperatura emergente
- Parametros: temperatura, densidad, tipo de particula
- Conexiones: termodinamica, gases reales, plegamiento de proteinas

#### Epidemiology (Biology / Network Science)
Modelo SIR/SEIR espacial con miles de agentes en grid.
- Visualizar R0, inmunidad de rebano, curvas de contagio en tiempo real
- Conexiones: ecosistema (Lotka-Volterra), redes, probabilidad

### Tier B — Activar con nueva disciplina natural

| Modulo | Disciplina futura | Descripcion |
|--------|-------------------|-------------|
| `AccretionDisk` | Astronomy | Disco de acrecion alrededor de agujero negro |
| `GalaxyCollision` | Astronomy | Dos sistemas gravitacionales, colas de marea |
| `ProteinFolding` | Biochemistry | Minimizacion de energia, estructuras 2D/3D |
| `LatticeBotzmann` | Physics (ya tiene HTML) | Flujo alrededor de obstaculos, turbulencia |
| `PlasmaParticles` | Physics (ya tiene HTML) | Interacciones Coulomb, aurora boreal |
| `GranularSim` | Geology / Physics | Arena, avalanchas, angulo de reposo |
| `HeatDiffusion` | Physics | Difusion en 2D con materiales distintos |
| `WaveSolver` | Physics / Acoustics | Ecuacion de onda 2D con reflexion |
| `CellularAutomata3D` | Mathematics | Game of Life volumetrico |
| `NeuralNetwork` | AI | Red neuronal entrenando en tiempo real (C++) |
| `Electrophoresis` | Biochemistry | Separacion por tamanyo y carga en gel |

---

## Proximas Disciplinas

### Neuroscience Lab

**Por que ahora:** la brecha entre Biology y AI es la mas obvia del ecosistema. La neurociencia la llena con matematicas rigurosas.

**Simulaciones propuestas:**

| Sim | Modelo | Conexiones |
|-----|--------|-----------|
| Red de neuronas Kuramoto | Sincronizacion de osciladores acoplados | Physics (ondas), Biology (neurona) |
| Plasticidad sinaptica | Regla de Hebb, LTP/LTD | AI (backprop), Biochemistry |
| Oscilaciones cerebrales | Ritmos alpha/beta/gamma → EEG sintetico | Physics Sound, Math Fourier |
| Red de Hopfield | Memoria como atractor de energia | AI (autoencoder), Math (caos) |
| Percepcion bayesiana | Ilusiones visuales como inferencia | AI, Computation |
| Modelo de memoria de trabajo | Bump attractor (EDO neural) | Math (EDOs), RK4 |

**Sound Lab:** sonificacion de ritmos EEG — frecuencias cerebrales como drones.

**Curso:** "Del Potencial de Accion al Pensamiento" — 20 lecciones desde Hodgkin-Huxley hasta cognicion.

---

### Climate Science Lab

**Por que ahora:** la fisica mas urgente de nuestro tiempo. Dinamica no lineal con tipping points reales. Conecta 5 disciplinas existentes.

**Simulaciones propuestas:**

| Sim | Modelo | Conexiones |
|-----|--------|-----------|
| Forzamiento radiativo | Curva Keeling interactiva, equilibrio energetico | Chemistry (CO2), Physics (termo) |
| Retroalimentacion hielo-albedo | Tipping points, histéresis climatica | Physics (caos), Geology |
| Ciclos de Milankovitch | Parametros orbitales → glaciaciones | Astronomy (orbitas), Geology (estratigrafia) |
| Celulas de Hadley | Circulacion atmosferica, zona intertropical | Physics (fluidos WASM) |
| Modelo energetico 0D (EBM) | Temperatura global como EDO simple | Math (EDOs), RK4 |
| Termoclina oceanica | Capas de densidad, AMOC simplificado | Physics (fluidos), Geology |

**Curso:** "La Ecuacion del Clima" — desde el efecto invernadero hasta los ciclos de Milankovitch.

---

### Quantum Computing Lab

**Por que ahora:** extension natural de la mecanica cuantica existente en Physics. Puente entre Physics, Math y AI.

**Simulaciones propuestas:**

| Sim | Concepto | Conexiones |
|-----|----------|-----------|
| Esfera de Bloch | Qubit como punto en S², rotaciones unitarias | Physics (cuantica), Math (algebra lineal) |
| Puertas cuanticas | H, CNOT, Toffoli animadas en circuito | Computation (logic gates) |
| Entrelazamiento | Bell states, no-localidad visualizada | Physics (cuantica) |
| Algoritmo de Grover | Busqueda cuantica O(√n) vs O(n) clasico | Computation (complejidad) |
| QFT (Fourier Cuantica) | Transformada de Fourier cuantica | Math Fourier, Computation |
| Interferencia cuantica | Doble rendija cuantica vs clasica | Physics (interferencia) |

**Curso:** "De Bits a Qubits" — 25 lecciones desde algebra lineal hasta el algoritmo de Shor.

---

### Game Theory Lab

**Por que ahora:** matematicas puras que modelan comportamiento emergente. Conecta Biology (evolucion), Computation (algoritmos) y un futuro Economics Lab.

**Simulaciones propuestas:**

| Sim | Modelo | Conexiones |
|-----|--------|-----------|
| Dilema del Prisionero iterado | Cooperacion/defeccion, TFT, ALLC, ALLD | Biology (evolucion), Computation |
| Nash Equilibrium finder | Juegos 2×2, dominancia, equilibrio mixto | Math (probabilidad) |
| Replicator dynamics | Evolucion de estrategias en poblacion | Biology (seleccion natural), EDOs |
| Tragedia de los Comunes | Recursos compartidos, over-exploitation | Biology (ecosistema) |
| Subastas | Vickrey, primer precio, revenue equivalence | Math (probabilidad) |
| Juegos en red | Grafos donde los nodos juegan, cascadas | Computation (grafos) |

**Sound Lab:** estrategias como partituras — cooperacion = consonancia, defeccion = disonancia.

---

### Acoustics Lab

**Por que ahora:** la fisica del sonido merece su propio laboratorio, distinto de Physics Sound Lab (sonificacion) y Math Sound Lab (matematizacion). Complementa Music Theory.

**Simulaciones propuestas:**

| Sim | Modelo | Conexiones |
|-----|--------|-----------|
| Modos de resonancia de salas | Ondas estacionarias 3D, frecuencias propias | Physics (ondas), Math Fourier |
| Sintesis fisica (Karplus-Strong) | Modelo de cuerda/tubo con delays | Music Theory, Physics Sound |
| Psychoacoustica | Bandas criticas, enmascaramiento, pitch | Biology (percepcion) |
| Efecto Doppler 2D | Fuente movil con audio real | Physics (Doppler) |
| Difraccion sonora | Obstaculos, difraccion en aperturas | Physics (difraccion) |
| Patron de Chladni | Vibracion de placa, patrones de arena | Physics (ondas), Math Gen Art |

---

### Network Science Lab

**Por que ahora:** los grafos son el lenguaje universal de la complejidad. Transversal a Biologia, Epidemiologia, Computacion e IA.

**Simulaciones propuestas:**

| Sim | Modelo | Conexiones |
|-----|--------|-----------|
| Modelo Barabasi-Albert | Red scale-free creciendo en tiempo real | Math (probabilidad), Computation |
| Watts-Strogatz | Pequeño mundo, coeficiente de clustering | Biology (ecosistema) |
| Epidemias en redes | SIR sobre grafo (usando modulo WASM) | Biology, Epidemiology |
| Cascadas de fallo | Blackouts, robustez vs ataque dirigido | Computation (algoritmos) |
| Comunidades | Modularidad, algoritmo Louvain | Computation (grafos), AI |
| PageRank visual | Flujo de autoridad en red | Computation, AI |

---

### Engineering Lab

**Por que ahora:** el ecosistema tiene todos los ingredientes (fluidos, estructuras, circuitos, control). Cierra el ciclo STEM→Ingenieria.

**Simulaciones propuestas:**

| Sim | Modelo | Conexiones |
|-----|--------|-----------|
| Cerchas y vigas | Metodo de nudos, diagramas M/V/N | Math (algebra lineal) |
| Control PID | Respuesta al escalon, sintonizacion | Math (EDOs), Physics |
| Circuitos AC | Fasores, potencia reactiva, filtros | Physics (circuitos RLC) |
| Maquinas termicas | Ciclo Carnot/Otto/Diesel animado | Physics (termodinamica) |
| Cinematica de robot | Transformaciones homogeneas, IK | Math (matrices), 3D |
| Analisis de señales | Convolucion, filtros FIR/IIR | Math Sound (Fourier), Acoustics |

---

## Proximos Cursos (formato Academia)

Disciplinas donde la riqueza esta en el recorrido intelectual, no en simulaciones individuales. Se publican en `_portal/academia.html`.

### Topology Course

Continuacion natural de Platonic Lab y Math Visual Lab (Möbius, Klein).

| Modulo | Contenido |
|--------|-----------|
| 1. Superficies | Clasificacion topologica, genero, orientabilidad |
| 2. Homotopia | Deformaciones continuas, grupo fundamental |
| 3. Homologia | Agujeros en dimension n, numeros de Betti |
| 4. TDA | Persistent homology, nubes de puntos |
| 5. Aplicaciones | Proteinas, redes, cosmologia, datos |

**Prerequisito:** Platonic Lab (solidos, 4D).

### Information Theory Course

El curso unificador del ecosistema. La entropia de Shannon conecta termodinamica, biologia, computacion e IA.

| Modulo | Contenido |
|--------|-----------|
| 1. Entropia de Shannon | Informacion, sorpresa, bits |
| 2. Capacidad de canal | Ruido, teorema de Shannon-Hartley |
| 3. Compresion | Huffman, LZ, incompresibilidad |
| 4. Informacion mutua | Dependencia, causalidad de Granger |
| 5. Informacion cuantica | Qubits, entrelazamiento, teleportacion |

**Conexiones:** Physics (entropia termodinamica), Biology (informacion genetica), AI (cross-entropy loss), Quantum Computing.

### Complex Systems Course

El meta-nivel de EigenLab: el estudio de la emergencia misma. Curso transversal a todas las disciplinas.

| Modulo | Contenido |
|--------|-----------|
| 1. Criticidad auto-organizada | Avalanchas de arena, 1/f noise |
| 2. Transiciones de fase | Orden/desorden, universalidad |
| 3. Redes adaptativas | Feedback estructural y dinamico |
| 4. Complejidad computacional | P vs NP, computacion al borde del caos |
| 5. Vida artificial | Autopoiesis, origen de la vida como complejidad |

---

## Features Transversales

### Corto Plazo
- [ ] Sistema de progreso en Caminos de Aprendizaje (localStorage, checkmarks)
- [ ] Botones "Anterior / Siguiente" dentro de cada simulacion cuando viene de un camino
- [ ] Breadcrumb de camino: "Caos: Paso 3/5"
- [ ] Actualizar `_portal/academia.html` con Euler Lab como curso destacado
- [ ] Añadir Euler Lab al Knowledge Graph

### Medio Plazo
- [ ] Integrar Caminos con Knowledge Graph (resaltar nodos del camino activo al filtrarlo)
- [ ] Deep linking en caminos: `?path=chaos&step=3` para compartir progreso
- [ ] Preguntas de reflexion al final de cada paso del camino
- [ ] Service Workers para modo offline (simulaciones JS ya son autocontenidas)
- [ ] PWA instalable en moviles

### Largo Plazo
- [ ] Modo "profesor": anotaciones sobre simulaciones, exportar como PDF
- [ ] Grabacion de sesiones (replay de parametros en el tiempo)
- [ ] URLs con parametros para compartir configuraciones: `?L=1.5&g=9.8&theta=45`
- [ ] Internacionalizacion (ES/EN/PT)
- [ ] Integracion con LMS (exportar como SCORM)
- [ ] WebXR para experiencias inmersivas (moleculas en AR)

---

## Mapa de Nuevas Disciplinas

Conexiones con el ecosistema existente:

```
Neuroscience   ←→  Biology (neurona), AI (redes), Physics Sound, Biochemistry
Climate        ←→  Geology (estratigrafia), Astronomy (orbitas), Chemistry (CO2), Physics
Quantum Comp.  ←→  Physics (cuantica), Math (algebra), AI (quantum ML), Computation
Game Theory    ←→  Biology (evolucion), Computation (algoritmos), Math (probabilidad)
Acoustics      ←→  Physics Sound, Music Theory, Math Fourier, Biology (percepcion)
Network Sci.   ←→  Biology (ecosistema), Computation (grafos), AI, Epidemiology
Engineering    ←→  Physics (fluidos, termo), Math (EDOs), Computation (control)
```

---

## Contribuciones

Formato sugerido para propuestas de nuevas simulaciones:

```markdown
## Nueva Simulacion: [Nombre]

**Disciplina:** Physics/Chemistry/etc.
**Tipo:** Visual/Sound/Data
**Ecuacion principal:** [descripcion o LaTeX]
**Parametros interactivos:** [lista]
**Conexiones cross-disciplina:** [labs relacionados]
**Referencias:** [papers, libros, videos]
```

---

## Changelog

### Marzo 2026

- ✅ **Knowledge Graph actualizado:** 132 → 167 nodos, ~290 → 409 enlaces
- ✅ **14 links rotos corregidos** en el Knowledge Graph (URLs verificadas)
- ✅ **Academia page:** cursos, teoria original y papers en `_portal/academia.html`
- ✅ **Portal reorganizado:** Computation + AI unificados, depth badges en todas las cards
- ✅ **Sculpt Lab y Puzzle Lab** añadidos como cards en Mathematics
- ✅ **Generative Music Lab** duplicado bajo Music para mayor descubribilidad
- ✅ **README actualizado:** conteos reales, 10 disciplinas, 16 labs

### Enero 2026

- ✅ **Music Theory Lab** — nueva disciplina, 9 simulaciones del Sistema Armonico Aureo (phi)
- ✅ **Math Sound Lab** — 8 simulaciones de sonificacion matematica
- ✅ **AI Visual Lab** — 20 simulaciones + 21 guias sobre redes neuronales y LLMs
- ✅ **Platonic Lab** — Curso: 33 lecciones desde Platon hasta politopos 4D
- ✅ **Chaos Lab** — Curso: 10 atractores + 30 lecciones
- ✅ **2 nuevos modulos WASM:** SaturnRings, VectorFields
- ✅ **Knowledge Graph expandido:** ~110 → 132 nodos, ~230 conexiones

### Diciembre 2025

- ✅ **Motor WASM eigenlab-core** — C++17, Emscripten, 7 modulos desplegados
- ✅ **5 simulaciones WASM de alto rendimiento:** fluidos, boids, galaxias, erosion, tela
- ✅ **Math Visual Lab:** 23 → 27 simulaciones (probabilidad, funciones, geometria-3d, EDOs)
- ✅ **12 caminos de aprendizaje** en `_portal/paths/`
- ✅ **Knowledge Graph** — D3.js force-directed, ~110 nodos, filtros, busqueda
- ✅ **91 guias pedagogicas** completadas (68% cobertura en ese momento)
- ✅ **Computation Lab** — 6 simulaciones + 6 guias completas
- ✅ **Geology Visual Lab** — 6 simulaciones completas
- ✅ **Biology Visual Lab** — expandido a 9 simulaciones
- ✅ **Astronomy Visual + Sound Lab** — completados
- ✅ **23 links rotos corregidos** en 18 archivos (nomenclatura)

### Diciembre 2024

- ✅ **Biology Visual Lab** — 6 simulaciones (Hodgkin-Huxley, Lotka-Volterra, Hardy-Weinberg, mitosis, meiosis, evolucion)
- ✅ **Astronomy Visual Lab** — 6 simulaciones (Hubble, agujero negro, lentes, H-R, lunas, estaciones)
- ✅ **Astronomy Sound Lab** — 3 simulaciones (pulsares, LIGO, musica esferas)
- ✅ **Portal unificado** `_portal/index.html` con previews animados

---

*Ultima actualizacion: Marzo 2026*
