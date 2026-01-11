# EigenLab - Roadmap

Plan de desarrollo y crecimiento del ecosistema de laboratorios virtuales.

---

## Estado Actual

### Motor C++/WebAssembly (eigenlab-core)

**Nueva arquitectura de alto rendimiento** para simulaciones que requieren miles de calculos por frame.

| Componente | Descripcion |
|------------|-------------|
| `eigenlab-core/` | Codigo fuente C++17 |
| `eigenlab-core.wasm` | Binario compilado (158KB) |
| `eigenlab-core.js` | Glue code Emscripten |

**Modulos implementados:**

| Modulo | Algoritmo | Rendimiento |
|--------|-----------|-------------|
| `FluidSolver` | Navier-Stokes 2D, proyeccion de presion | 256x256 @ 60fps |
| `BoidSystem` | Reynolds flocking + spatial hash O(n) | 1000+ agentes @ 60fps |
| `GalaxySimulator` | N-body Barnes-Hut O(n log n) | 5000+ particulas @ 30fps |
| `ErosionSimulator` | Erosion hidraulica por gotas | 256x256 terreno |
| `ClothSimulator` | Verlet + PBD constraints | 1200 particulas @ 60fps |

**Compilacion:**
```bash
cd eigenlab-core
mkdir build && cd build
emcmake cmake ..
emmake make -j4
```

### Laboratorios Existentes

| Lab | Simulaciones | Con Guía | Estado |
|-----|:------------:|:--------:|--------|
| Physics Visual Lab | 27 | 5 | 🔴 22 guías pendientes |
| Physics Sound Lab | 4 | 0 | 🔴 4 guías pendientes |
| Chemistry Visual Lab | 18 | 17 | ✅ Casi completo (94%) |
| Biochem Visual Lab | 16 | 16 | ✅ Completo |
| Math Visual Lab | 27 | 27 | ✅ Completo |
| Math Generative Art Lab | 7 | 0 | 🔴 7 guías pendientes |
| Astronomy Visual Lab | 8 | 7 | ✅ Casi completo (87%) |
| Astronomy Sound Lab | 3 | 3 | ✅ Completo |
| Biology Visual Lab | 9 | 8 | ✅ Casi completo (88%) |
| Geology Visual Lab | 7 | 6 | ✅ Casi completo (85%) |
| Computation Lab | 6 | 2 | 🔴 4 guías pendientes |

**Total: 132 simulaciones | 91 con guía (68%)**

> Última actualización: 27 Dic 2025

---

## Ideas de Crecimiento por Área

### Physics

#### Visual Lab - Expansión
| Categoría | Simulación | Descripción |
|-----------|------------|-------------|
| Cuántica | Doble Rendija | Patrón de interferencia, dualidad onda-partícula |
| Cuántica | Efecto Túnel | Probabilidad de tunelaje vs barrera de potencial |
| Cuántica | Pozo de Potencial | Niveles de energía cuantizados |
| Fluidos | Navier-Stokes 2D | Flujo laminar y turbulento |
| Fluidos | Vórtices de Karman | Calle de vórtices detrás de obstáculos |
| Electromagnetismo | Ondas EM 3D | Propagación de campos E y B acoplados |
| Electromagnetismo | Líneas de Campo Magnético | Imanes, solenoides, bobinas |
| Relatividad | Diagrama de Minkowski | Conos de luz, líneas de mundo |
| Relatividad | Paradoja de los Gemelos | Visualización del envejecimiento diferencial |

#### Sound Lab - Expansión
| Tipo | Simulación | Descripción |
|------|------------|-------------|
| Acústica | Modos de Vibración | Patrones de Chladni sonificados |
| Acústica | Resonancia de Salas | Reverberación y ondas estacionarias |
| Síntesis | FM Synthesis | Modulación de frecuencia interactiva |
| Síntesis | Síntesis Granular | Física de partículas → granos de audio |
| Doppler | Efecto Doppler Sonoro | Fuente móvil con audio real |
| Cuerdas | Modos Armónicos | Cuerda vibrante → sonido |

#### Data Lab (Nuevo)
| Tipo | Contenido |
|------|-----------|
| Datasets | Mediciones reales de experimentos clásicos |
| Comparación | Simulación vs datos reales |
| Análisis | Herramientas de ajuste de curvas |

---

### Chemistry

#### Visual Lab - Expansión
| Categoría | Simulación | Descripción |
|-----------|------------|-------------|
| Espectroscopía | IR Interactivo | Modos vibracionales → espectro |
| Espectroscopía | UV-Vis | Transiciones electrónicas |
| Espectroscopía | RMN básico | Desplazamiento químico |
| Cristalografía | Celdas Unitarias 3D | 14 redes de Bravais |
| Cristalografía | Difracción de Rayos X | Ley de Bragg visualizada |
| Cinética | Michaelis-Menten | Cinética enzimática detallada |
| Orgánica | Mecanismos SN1/SN2 | Animación paso a paso |
| Orgánica | Conformaciones | Rotación de enlaces, energía torsional |

#### Sound Lab (Nuevo)
| Tipo | Simulación | Descripción |
|------|------------|-------------|
| Sonificación | Espectros IR → Audio | Cada pico = frecuencia |
| Música Molecular | Vibraciones Moleculares | Modos normales como acordes |
| Reacciones | Sonificación de Cinética | Velocidad de reacción → pitch |

#### AR Lab (Futuro)
| Tipo | Descripción |
|------|-------------|
| WebXR | Moléculas en realidad aumentada |
| Marker-based | Tarjetas físicas → modelos 3D |

---

### Biochemistry

#### Visual Lab - Expansión
| Categoría | Simulación | Descripción |
|-----------|------------|-------------|
| Enzimas | Docking Molecular | Interacción enzima-sustrato simplificada |
| Técnicas | Electroforesis | Separación por tamaño/carga |
| Técnicas | PCR Animado | Ciclos de amplificación |
| Técnicas | Western Blot | Proceso paso a paso |
| Genética | CRISPR Simplificado | Edición génica visualizada |
| Membrana | Transporte Activo | Bombas iónicas, gradientes |
| Señalización | Cascadas de Señales | Receptores, segundos mensajeros |

#### Sound Lab (Nuevo)
| Tipo | Simulación | Descripción |
|------|------------|-------------|
| ADN Musical | Secuencia → Melodía | A=Do, T=Re, G=Mi, C=Fa |
| Proteínas | Estructura → Sonido | α-hélice y β-lámina como patrones rítmicos |
| Metabolismo | Flujo Metabólico | Actividad de rutas → texturas sonoras |

---

### Mathematics

#### Visual Lab - Expansión (Top 7 Simulaciones)
| # | Simulación | Categoría | Ecuación/Concepto | Descripción |
|---|------------|-----------|-------------------|-------------|
| 1 | Disco de Poincaré | Geometría | Métrica hiperbólica | Universo infinito en círculo finito. Teselaciones tipo Escher. |
| 2 | Domain Coloring | Análisis Complejo | f: ℂ → ℂ | Visualizar funciones complejas con color. Ceros, polos, singularidades. |
| 3 | Autómatas Elementales | Caos Discreto | Rule 30, 110, 184 | Wolfram. Determinismo → caos en 1D. Complementa Game of Life. |
| 4 | Problema de 3 Cuerpos | Dinámica | F = Gm₁m₂/r² | Caos gravitacional. Completa trilogía (Lorenz, Péndulo). |
| 5 | Banda de Möbius | Topología | Superficie no orientable | Una cara, un borde. Preparación para Klein. |
| 6 | Nudos Matemáticos | Topología | Invariantes, coloración | Equivalencia topológica. ¿Se puede desatar? |
| 7 | Grupos de Simetría | Álgebra | Wallpaper groups (17) | Todas las simetrías posibles del plano. Arte islámico explicado. |

**Menciones honorables:** Grafos (Dijkstra, BFS), Redes complejas (scale-free), Botella de Klein.

#### Generative Art Lab (Top 7 - Nuevo)
| # | Simulación | Técnica | Descripción |
|---|------------|---------|-------------|
| 1 | Reacción-Difusión | Gray-Scott (EDP) | Patrones de Turing vivos. Coral, huellas, cebras. La joya de la emergencia. |
| 2 | L-Systems | Gramáticas formales | Un motor → árboles, helechos, curva de Hilbert, dragón. Usuario edita reglas. |
| 3 | Flow Fields | Perlin Noise + partículas | Estética Van Gogh. Miles de trazos siguiendo un campo suave. |
| 4 | Strange Attractors | Sistemas dinámicos | Galería: Rössler, Aizawa, Thomas, Halvorsen. Más allá de Lorenz. |
| 5 | Phyllotaxis | Ángulo áureo (137.5°) | Espirales de Fibonacci en girasoles, piñas, suculentas. |
| 6 | Fractales IFS | Funciones iteradas | Helecho de Barnsley, Sierpinski por IFS, árboles fractales. |
| 7 | Teselaciones Aperiódicas | Penrose, Wang tiles | Cubrir el infinito sin repetirse. Cuasicristales. |

#### Sound Lab (Nuevo)
| Tipo | Simulación | Descripción |
|------|------------|-------------|
| Secuencias | Primos → Ritmo | Números primos como pulsos |
| Secuencias | Fibonacci → Melodía | Proporción áurea en música |
| Caos | Atractor de Lorenz → Audio | Coordenadas XYZ → parámetros de síntesis |
| Fractales | Mandelbrot Sonificado | Profundidad de iteración → pitch |

---

## Nuevas Disciplinas Potenciales

### Biology ✅ VISUAL LAB COMPLETADO
```
Biology/
├── Biology Visual Lab/          # ✅ 6 simulaciones
│   ├── index.html               # ✅ Landing con previews
│   ├── neurona.html             # ✅ Potencial de acción (Hodgkin-Huxley)
│   ├── ecosistema.html          # ✅ Depredador-presa (Lotka-Volterra)
│   ├── genetica-poblacional.html # ✅ Hardy-Weinberg, deriva genética
│   ├── mitosis.html             # ✅ Fases del ciclo celular
│   ├── meiosis.html             # ✅ Recombinación, crossing-over
│   └── evolucion.html           # ✅ Selección natural simulada
└── Biology Sound Lab/
    └── ritmos-circadianos.html  # Ciclos biológicos → música (pendiente)
```

### Astronomy ✅ COMPLETADO
```
Astronomy/
├── Astronomy Visual Lab/         # ✅ 6 simulaciones
│   ├── expansion-universo.html   # ✅ Ley de Hubble
│   ├── agujero-negro.html        # ✅ Órbitas, horizonte de eventos
│   ├── lentes-gravitacionales.html # ✅ Deflexión de luz
│   ├── diagrama-hr.html          # ✅ Hertzsprung-Russell interactivo
│   ├── fases-lunares.html        # ✅ Geometría Sol-Tierra-Luna
│   └── estaciones.html           # ✅ Inclinación axial
└── Astronomy Sound Lab/          # ✅ 3 simulaciones
    ├── pulsares.html             # ✅ Pulsos de radio → ritmo
    ├── ondas-gravitacionales.html # ✅ Datos LIGO sonificados
    └── musica-esferas-moderna.html # ✅ Exoplanetas cantando
```

### Geology ✅ VISUAL LAB COMPLETADO
```
Geology/
└── Geology Visual Lab/           # ✅ 6 simulaciones
    ├── index.html                # ✅ Landing con previews
    ├── terremotos.html           # ✅ Ondas P, S, superficiales, sismograma
    ├── volcanes.html             # ✅ Tipos de erupciones, VEI
    ├── tectonica-placas.html     # ✅ Deriva continental, bordes de placas
    ├── ciclo-rocas.html          # ✅ Ígneas, sedimentarias, metamórficas
    ├── erosion.html              # ✅ Hídrica, eólica, glaciar, costera
    └── estratigrafia.html        # ✅ Capas geológicas, fósiles, datación
```

### Engineering
```
Engineering/
├── Engineering Visual Lab/
│   ├── estructuras.html          # Análisis de cerchas
│   ├── vigas.html                # Diagramas de momento/cortante
│   ├── circuitos.html            # Ley de Ohm, Kirchhoff
│   ├── control-pid.html          # Controladores, respuesta
│   ├── maquinas-termicas.html    # Ciclos Carnot, Otto, Diesel
│   └── robotica-cinematica.html  # Brazos articulados
└── Engineering Sound Lab/
    └── feedback-audio.html       # Control PID aplicado a síntesis
```

### Music Theory
```
Music Theory/
├── Music Theory Visual Lab/
│   ├── circulo-quintas.html      # Relaciones tonales
│   ├── progresiones.html         # Análisis armónico
│   ├── contrapunto.html          # Reglas de voice leading
│   ├── formas-musicales.html     # Sonata, rondó, variaciones
│   └── analisis-schenkerian.html # Reducción de Schenker
└── Music Theory Sound Lab/        # Ya parcialmente en Physics Sound Lab
    ├── temperamentos.html        # Comparación de afinaciones
    ├── escalas-mundo.html        # Modos, ragas, maqams
    └── microtonal.html           # Divisiones del octavo
```

---

## Features Transversales

### Corto Plazo
- [ ] Teclas de atajo consistentes (Space=play, R=reset)
- [ ] Export de estado como JSON
- [ ] URLs con parámetros para compartir configuraciones

### Medio Plazo
- [ ] Internacionalización (ES/EN/PT)
- [ ] Service Workers para modo offline
- [ ] PWA instalable en móviles
- [ ] Presets compartibles entre simulaciones similares

### Largo Plazo
- [ ] Modo "profesor" con anotaciones
- [ ] Grabación de sesiones (replay)
- [ ] Integración con LMS (exportar como SCORM)
- [ ] WebXR para experiencias inmersivas

---

## Prioridades: Consolidación antes que Expansion

> "Profundidad antes que amplitud"
> — Ver MANIFESTO.md

---

### FASE ACTUAL: Consolidacion Pedagogica

#### 1. Guias Pedagogicas para Todos los Labs

Cada simulacion debe tener una guia en `guides/` con:
- Que es y por que importa
- Ecuacion/modelo con explicacion
- Estados visuales (leyenda de colores)
- Experimento guiado paso a paso
- Conexiones con otras simulaciones
- Limitaciones del modelo

**Estado actual (27 Dic 2025):**
| Lab | Guías | Sims | Estado |
|-----|:-----:|:----:|--------|
| Math Visual Lab | 27/27 | 27 | ✅ Completo |
| Biochem Visual Lab | 16/16 | 16 | ✅ Completo |
| Astronomy Sound Lab | 3/3 | 3 | ✅ Completo |
| Chemistry Visual Lab | 17/18 | 18 | ✅ Casi completo (94%) |
| Biology Visual Lab | 8/9 | 9 | ✅ Casi completo (88%) |
| Astronomy Visual Lab | 7/8 | 8 | ✅ Casi completo (87%) |
| Geology Visual Lab | 6/7 | 7 | ✅ Casi completo (85%) |
| Computation Lab | 2/6 | 6 | 🔴 4 pendientes |
| Physics Visual Lab | 5/27 | 27 | 🔴 22 pendientes |
| Physics Sound Lab | 0/4 | 4 | 🔴 4 pendientes |
| Math Generative Art Lab | 0/7 | 7 | 🔴 7 pendientes |

**Total: 91 guías completadas** (de 132 simulaciones = 68%)

**Próxima prioridad:**
1. Physics Visual Lab (22 sin guía) - las más visitadas, base de todo
2. Math Generative Art Lab (7 sin guía) - arte generativo
3. Physics Sound Lab (4 sin guía) - generativos musicales
4. Computation Lab (4 sin guía) - algoritmos

---

### NUEVA FASE: Espiral Virtuosa de Auto-Evolución

> El ecosistema se nutre a sí mismo: las guías revelan gaps, los gaps generan nuevas simulaciones, las simulaciones generan nuevas guías.

#### Diagrama del Sistema

```
┌─────────────────────────────────────────────────────────┐
│                                                         │
▼                                                         │
┌──────────┐     ┌──────────────┐     ┌───────────────┐   │
│  GUÍAS   │────▶│  CONEXIONES  │────▶│ SIMULACIONES  │───┤
│ (91 ya)  │     │ INTER-DISC   │     │    NUEVAS     │   │
└────┬─────┘     └──────┬───────┘     └───────────────┘   │
     │                  │                                  │
     │                  ▼                                  │
     │          ┌───────────────┐                         │
     │          │    DEMANDA    │ ← ¿qué falta?           │
     │          │   DETECTADA   │                         │
     │          └───────────────┘                         │
     │                                                     │
     ▼                                                     │
┌──────────────┐     ┌──────────────┐                     │
│  MEJORAS A   │     │  CAMINOS DE  │◀────────────────────┘
│ SIMULACIONES │     │ APRENDIZAJE  │
└──────────────┘     └──────────────┘
```

#### Flujo de Trabajo

1. **Leer guías existentes** → Identificar:
   - Sugerencias de mejora para la simulación
   - Conexiones a simulaciones que no existen
   - Conceptos que merecen su propia simulación

2. **Mapa de Demanda** → Analizar:
   - ¿Cuántas guías referencian una simulación inexistente?
   - ¿Qué conceptos son "hubs" que conectan múltiples disciplinas?
   - Priorizar por frecuencia + valor pedagógico

3. **Crear simulaciones prioritarias** → Las más demandadas primero

4. **Crear guía para la nueva simulación** → Ciclo se repite

#### ACCIÓN INMEDIATA: Mapa de Demanda

**Objetivo:** Parsear las guías para extraer:

| Dato | Uso |
|------|-----|
| Links a simulaciones existentes | Validar conexiones |
| Links a simulaciones inexistentes | Detectar demanda |
| Conceptos mencionados sin link | Oportunidades |
| Mejoras sugeridas en experimentos | Backlog de features |

**Resultado del análisis (Diciembre 2025):**

#### SIMULACIONES MÁS DEMANDADAS

✅ **Todas completadas** - Las simulaciones más solicitadas han sido implementadas.

#### COMPLETADAS RECIENTEMENTE ✅

| Simulación | Lab | Fecha |
|------------|-----|-------|
| `nucleosintesis.html` | Astronomy Visual | Dic 2025 |
| `vision.html` | Biology Visual | Dic 2025 |
| `lipidos.html` | Biochem Visual | Dic 2025 |
| `efecto-tunel.html` | Physics Visual | Dic 2025 |
| `circuitos.html` | Physics Visual | Dic 2025 |
| `entropia.html` | Physics Visual | Dic 2025 |
| `fluidos-2d.html` | Physics Visual | Dic 2025 (WASM) |
| `formacion-galaxias.html` | Astronomy Visual | Dic 2025 (WASM) |
| `boids-masivo.html` | Biology Visual | Dic 2025 (WASM) |
| `erosion-hidraulica.html` | Geology Visual | Dic 2025 (WASM) |
| `simulacion-tela.html` | Physics Visual | Dic 2025 (WASM) |
| `probabilidad.html` | Math Visual | 21 Dic 2025 |
| `funciones.html` | Math Visual | 21 Dic 2025 |
| `geometria-3d.html` | Math Visual | 21 Dic 2025 |
| `ecuaciones-diferenciales.html` | Math Visual | 21 Dic 2025 |
| `termodinamica.html` | Physics Visual | Dic 2025 |
| `ondas.html` | Physics Visual | Dic 2025 |
| `trigonometria.html` | Math Visual | Dic 2025 |
| `ritmos-circadianos.html` | Biology Visual | Dic 2025 |
| `enlaces.html` | Chemistry Visual | Dic 2025 |
| `cinetica-colisiones.html` | Chemistry Visual | Dic 2025 |
| `double-pendulum.html` | Math Visual | Dic 2025 |
| `three-body.html` | Math Visual | Dic 2025 |

#### SIMULACIONES MÁS CONECTADAS (hubs existentes)

| Simulación | Conexiones Entrantes | Rol |
|------------|:--------------------:|-----|
| `le-chatelier.html` | 9 | Hub de equilibrio químico |
| `agujero-negro.html` | 5 | Hub de relatividad |
| `gases-reales.html` | 5 | Hub de termodinámica |
| `diagrama-fases.html` | 4 | Hub de estados de materia |
| `perceptron-playground.html` | 4 | Hub de machine learning |
| `lorenz-attractor.html` | 3 | Hub de caos |
| `neurona.html` | 3 | Hub de bioelectricidad |
| `finite-automata.html` | 3 | Hub de computación |

#### LINKS CORREGIDOS ✅

23 links rotos corregidos en 18 archivos (errores de nomenclatura).

---

#### 2. Experimentos Guiados

Dentro de cada guia, incluir secciones "Experimenta" que lleven al usuario a:
- Formular una hipotesis
- Manipular variables especificas
- Observar un fenomeno concreto
- Conectar con la teoria

**Ejemplos de experimentos guiados:**

| Simulacion | Experimento | Descubrimiento |
|------------|-------------|----------------|
| Pendulo Doble | Cambiar angulo inicial de 0.1° | Sensibilidad a condiciones iniciales |
| Lotka-Volterra | Eliminar depredadores | Explosion de presas, luego colapso |
| Bubble Sort | Comparar con array casi-ordenado | O(n) en mejor caso |
| Atractor de Lorenz | Variar σ cerca de 10 | Transicion al caos |
| Hodgkin-Huxley | Reducir Na+ | Bloqueo del potencial de accion |

#### 3. Conexiones Entre Simulaciones

Crear enlaces explicitos entre simulaciones que comparten patrones:

**Patron: Caos Deterministico**
```
Physics/pendulo-doble.html
    ↔ Math/lorenz.html
    ↔ Biology/ecosistema.html
    ↔ Math Generative/gray-scott.html
```

**Patron: Retroalimentacion**
```
Biology/ecosistema.html (depredador-presa)
    ↔ Chemistry/equilibrio-quimico.html
    ↔ Physics/oscilador-amortiguado.html
```

**Patron: Estados Discretos**
```
Computation/finite-automata.html
    ↔ Biology/mitosis.html (fases)
    ↔ Chemistry/estados-materia.html
```

**Implementacion:**
- Seccion "Conexiones" en cada guia
- Badges en simulaciones: "Ver tambien: [patron] en [disciplina]"
- Pagina especial: `_portal/patterns.html` con mapa de patrones

---

### FASE ACTUAL: Caminos de Aprendizaje

**Estado:** 12 caminos implementados en `_portal/paths/`

| # | Camino | Archivo | Simulaciones |
|---|--------|---------|--------------|
| 1 | Caos y Atractores | chaos.html | 5 |
| 2 | Oscilaciones y Ondas | oscillations.html | 5 |
| 3 | Estados y Transiciones | states.html | 5 |
| 4 | Algoritmos y Optimización | algorithms.html | 5 |
| 5 | Emergencia y Autoorganización | emergence.html | 5 |
| 6 | Energía en Transformación | energy.html | 5 |
| 7 | Patrones de la Naturaleza | patterns.html | 5 |
| 8 | Del Átomo al Cosmos | scales.html | 5 |
| 9 | Ondas Everywhere | waves.html | 5 |
| 10 | Retroalimentación | feedback.html | 5 |
| 11 | Información y Complejidad | information.html | 5 |
| 12 | Simetría y Ruptura | symmetry.html | 5 |

Rutas sugeridas que cruzan disciplinas, unificadas por un concepto:

#### Ruta 1: "Entender el Caos"
> Del orden al desorden: por que sistemas simples producen comportamiento impredecible

| Paso | Simulacion | Concepto |
|------|------------|----------|
| 1 | Physics/pendulo-simple.html | Sistema predecible |
| 2 | Physics/pendulo-doble.html | Sensibilidad a condiciones iniciales |
| 3 | Math/lorenz.html | Atractor extrano, trayectorias que nunca se repiten |
| 4 | Biology/ecosistema.html | Caos en poblaciones |
| 5 | Math Generative/gray-scott.html | Patrones emergentes del caos |

#### Ruta 2: "Oscilaciones Universales"
> Todo vibra: de atomos a galaxias

| Paso | Simulacion | Concepto |
|------|------------|----------|
| 1 | Physics/pendulo-simple.html | Oscilacion mecanica |
| 2 | Physics/ondas.html | Propagacion de oscilaciones |
| 3 | Chemistry/orbitales.html | Funciones de onda |
| 4 | Biology/neurona.html | Potencial de accion como oscilacion |
| 5 | Astronomy/pulsares.html | Oscilaciones cosmicas |

#### Ruta 3: "Estados y Transiciones"
> Maquinas que cambian: de bits a celulas

| Paso | Simulacion | Concepto |
|------|------------|----------|
| 1 | Computation/finite-automata.html | Estados discretos, reglas de transicion |
| 2 | Computation/logic-gates.html | Compuertas como maquinas de estado |
| 3 | Biology/mitosis.html | Fases del ciclo celular |
| 4 | Chemistry/estados-materia.html | Transiciones de fase |
| 5 | Geology/ciclo-rocas.html | Estados geologicos |

#### Ruta 4: "Algoritmos en Accion"
> Como resuelven problemas las maquinas

| Paso | Simulacion | Concepto |
|------|------------|----------|
| 1 | Computation/bubble-sort.html | Ordenamiento ingenuo O(n²) |
| 2 | Computation/binary-search-tree.html | Busqueda eficiente O(log n) |
| 3 | Math/dijkstra.html (futuro) | Caminos optimos en grafos |
| 4 | Computation/perceptron.html | Aprendizaje como optimizacion |

#### Ruta 5: "Emergencia"
> Como lo simple genera lo complejo

| Paso | Simulacion | Concepto |
|------|------------|----------|
| 1 | Math/game-of-life.html | Reglas simples, patrones complejos |
| 2 | Math Generative/gray-scott.html | Reaccion-difusion |
| 3 | Biology/evolucion.html | Seleccion natural |
| 4 | Math Generative/l-systems.html | Gramaticas → arboles |

**Implementacion actual:**
- ✅ Pagina `_portal/paths/index.html` con las 5 rutas
- ✅ Cada ruta tiene su página dedicada con explicaciones
- ✅ Links directos a cada simulación del camino

---

### MEJORAS FUTURAS: Caminos de Aprendizaje

#### Interactividad y Progreso
- [ ] **Sistema de progreso** - localStorage para marcar simulaciones completadas
- [ ] **Checkmarks visuales** - Indicador de "visitado" en cada paso
- [ ] **Porcentaje de avance** - Barra de progreso por camino
- [ ] **Certificado de completado** - Badge descargable al terminar un camino

#### Navegación Mejorada
- [ ] **Botón "Siguiente"** - En cada simulación, link al siguiente paso del camino activo
- [ ] **Breadcrumb de camino** - Mostrar posición actual: "Caos: Paso 3/5"
- [ ] **Mini-mapa flotante** - Widget que muestra el camino completo
- [ ] **Deep linking** - URLs como `?path=chaos&step=3` para compartir progreso

#### Contenido Expandido
- [ ] **Preguntas de reflexión** - Al final de cada paso: "¿Qué observaste?"
- [ ] **Comparaciones guiadas** - "Abre estas dos simulaciones lado a lado"
- [ ] **Datos curiosos** - Contexto histórico, aplicaciones reales
- [ ] **Videos cortos** - Explicaciones de 1-2 minutos por concepto
- [ ] **Quizzes opcionales** - Verificar comprensión sin ser intrusivo

#### Nuevos Caminos (Diciembre 2025) ✅
| Camino | Concepto | Estado |
|--------|----------|--------|
| **Energía en Transformación** | Conservación y disipación | ✅ Implementado |
| **Patrones de la Naturaleza** | Fibonacci, fractales | ✅ Implementado |
| **Del Átomo al Cosmos** | Escalas del universo | ✅ Implementado |
| **Ondas Everywhere** | Propagación | ✅ Implementado |
| **Retroalimentación** | Feedback loops | ✅ Implementado |
| **Información y Complejidad** | Bits a comportamiento | ✅ Implementado |
| **Simetría y Ruptura** | Orden y desorden | ✅ Implementado |

#### Personalización
- [ ] **Caminos por nivel** - Básico / Intermedio / Avanzado
- [ ] **Caminos por tiempo** - "15 min", "1 hora", "Exploración profunda"
- [ ] **Caminos por interés** - "Quiero entender X" → sugerencia automática
- [ ] **Constructor de caminos** - Usuario crea su propia ruta

#### Integración con Knowledge Graph
- [ ] **Visualizar camino en el grafo** - Resaltar nodos del camino activo
- [ ] **Sugerir caminos desde grafo** - Click en concepto → "Explorar camino relacionado"
- [ ] **Caminos emergentes** - IA sugiere rutas basadas en patrones de uso

#### Gamificación (opcional, con cuidado)
- [ ] **Logros desbloqueables** - "Explorador del Caos", "Maestro de Ondas"
- [ ] **Estadísticas personales** - Tiempo en simulaciones, conceptos explorados
- [ ] **Modo descubrimiento** - Camino oculto que se revela al explorar

---

### FASES FUTURAS (Post-Consolidacion)

#### Fase 2: Expansion Vertical
1. Añadir Sound Lab a Chemistry y Biochemistry
2. Expandir Physics Sound Lab con más generativos
3. Añadir simulaciones 3D donde tenga sentido

#### Fase 3: Expansion Horizontal
1. ~~Lanzar Biology Visual Lab~~ ✅ Completado (6 simulaciones)
2. ~~Lanzar Astronomy Visual Lab~~ ✅ Completado (Visual + Sound Lab)
3. ~~Lanzar Geology Visual Lab~~ ✅ Completado (6 simulaciones)
4. Engineering Visual Lab
5. Music Theory Visual Lab

#### Fase 4: Integracion Avanzada
1. Sistema de ejercicios con feedback
2. Modo profesor con anotaciones
3. Grabacion de sesiones (replay)

---

## Simulaciones Pendientes (según conexiones de guías)

> **Estrategia:** Completar todas las guías primero. Las conexiones interdisciplinarias revelarán qué simulaciones faltan. Luego priorizamos las más referenciadas.

### Completadas recientemente

| Simulación | Lab | Estado |
|------------|-----|--------|
| `trigonometria.html` | Math Visual | ✅ Dic 2025 - Círculo unitario, ondas |
| `ritmos-circadianos.html` | Biology Visual | ✅ Dic 2025 - Reloj biológico, genes reloj |

### Por descubrir

Cuando terminemos las guías de:
- [ ] Physics Visual Lab (19 sims)
- [ ] Math Visual Lab (22 sims)
- [ ] Chemistry Visual Lab (17 sims)
- [ ] Biochem Visual Lab (16 sims)

...el mapa de conexiones mostrará más gaps. Entonces priorizaremos por:
1. **Frecuencia** — cuántas guías referencian la simulación faltante
2. **Centralidad** — si es un "hub" que conecta múltiples disciplinas
3. **Valor pedagógico** — si explica un concepto fundamental

---

## Contribuciones

Ideas para nuevas simulaciones o mejoras son bienvenidas.

Formato sugerido para propuestas:
```markdown
## Nueva Simulación: [Nombre]

**Disciplina:** Physics/Chemistry/etc.
**Tipo:** Visual/Sound/Data
**Ecuación principal:** [LaTeX o descripción]
**Parámetros interactivos:** [lista]
**Referencias:** [papers, libros, videos]
```

---

*Última actualización: 27 Diciembre 2025*

---

## Changelog

### 27 Diciembre 2025 - Inventario Actualizado

- **Inventario completo regenerado:**
  - 132 simulaciones totales (excluidos papers, tutoriales y documentos auxiliares)
  - 91 guías pedagógicas (68% cobertura)
  - CSV y Excel actualizados: `eigenlab_simulaciones.csv`, `eigenlab_simulaciones.xlsx`

- **Simulaciones demandadas completadas:**
  - `nucleosintesis.html`, `vision.html`, `lipidos.html`
  - `efecto-tunel.html`, `circuitos.html`, `entropia.html`

- **Próximas prioridades (41 guías pendientes):**
  - Physics Visual Lab: 22 guías pendientes
  - Math Generative Art Lab: 7 guías pendientes
  - Physics Sound Lab: 4 guías pendientes
  - Computation Lab: 4 guías pendientes

---

### 23 Diciembre 2025 - Motor WASM de Alto Rendimiento

- **eigenlab-core: Motor C++/WebAssembly**
  - Arquitectura modular en C++17 con Emscripten
  - Binario optimizado: 158KB (-O3, LTO, -ffast-math)
  - Embind para interoperabilidad JS/C++ sin overhead

- **5 Nuevas Simulaciones de Alto Rendimiento:**

  | Simulacion | Lab | Motor | Guia |
  |------------|-----|-------|------|
  | `fluidos-2d.html` | Physics Visual | FluidSolver (Navier-Stokes) | fluidos-2d.md |
  | `boids-masivo.html` | Biology Visual | BoidSystem (Reynolds) | boids-masivo.md |
  | `formacion-galaxias.html` | Astronomy Visual | GalaxySimulator (Barnes-Hut) | formacion-galaxias.md |
  | `erosion-hidraulica.html` | Geology Visual | ErosionSimulator | erosion-hidraulica.md |
  | `simulacion-tela.html` | Physics Visual | ClothSimulator (Verlet+PBD) | simulacion-tela.md |
 Con un motor C++/WASM podrías hacer simulaciones que hoy serían imposibles en JS puro. Ideas por disciplina:

  Physics

  | Simulación                     | Por qué necesita C++                         | Visualización                     |
  |--------------------------------|----------------------------------------------|-----------------------------------|
  | Fluidos 2D (Navier-Stokes)     | Solver de presión iterativo, miles de celdas | Humo, agua, vórtices              |
  | Tela/Softbody                  | Red de 10K+ resortes, colisiones             | Bandera al viento, gelatina       |
  | Electrodinámica de partículas  | N² interacciones Coulomb + campos B          | Plasma, aurora boreal             |
  | Lattice Boltzmann              | Autómata celular masivo                      | Flujo alrededor de obstáculos     |
  | Ondas en 2D (ecuación de onda) | PDE en grid grande                           | Estanque con gotas, interferencia |
  | Cuerpo rígido con fricción     | Solver de restricciones iterativo            | Dominós, jenga, pinball           |

  Chemistry / Biochemistry

  | Simulación                      | Por qué necesita C++          | Visualización                  |
  |---------------------------------|-------------------------------|--------------------------------|
  | Dinámica molecular simplificada | Lennard-Jones N²              | Cristalización, fusión         |
  | Reacción-difusión avanzada      | Gray-Scott en alta res        | Patrones Turing, morphogenesis |
  | Plegamiento de proteínas (toy)  | Minimización energía 2D/3D    | Cadena buscando mínimo         |
  | Electroforesis                  | Miles de partículas + campo E | Separación por tamaño          |

  Biology

  | Simulación                            | Por qué necesita C++               | Visualización               |
  |---------------------------------------|------------------------------------|-----------------------------|
  | Enjambre (boids avanzado)             | 10K+ agentes con reglas            | Peces, pájaros, multitudes  |
  | Crecimiento de plantas (L-systems 3D) | Recursión profunda + renderizado   | Árboles fractales realistas |
  | Epidemiología espacial                | Millones de agentes en grid        | Propagación de enfermedades |
  | Red neuronal biológica                | Hodgkin-Huxley para 1000+ neuronas | Actividad cerebral          |

  Mathematics

  | Simulación                            | Por qué necesita C++                 | Visualización              |
  |---------------------------------------|--------------------------------------|----------------------------|
  | Fractales 3D (Mandelbulb)             | Raymarching, millones de iteraciones | Rotación interactiva       |
  | Autómatas celulares 3D                | Grid cúbico grande                   | "Game of Life" volumétrico |
  | Campos vectoriales interactivos       | Integración de miles de trayectorias | Líneas de flujo            |
  | Atractor de Lorenz (10K trayectorias) | RK4 masivo paralelo                  | Nube de puntos caótica     |

  Astronomy

  | Simulación            | Por qué necesita C++              | Visualización      |
  |-----------------------|-----------------------------------|--------------------|
  | Formación de galaxias HECHA | N-body con 50K+ estrellas         | Espiral emergente  |
  | Colisión de galaxias  | Dos sistemas gravitacionales      | Colas de marea     |
  | Anillos de Saturno    | Millones de partículas + gravedad | Estructura de gaps |
  | Disco de acreción     | Fluido + gravedad relativista     | Agujero negro      |

  Geology

  | Simulación              | Por qué necesita C++            | Visualización              |
  |-------------------------|---------------------------------|----------------------------|
  | Erosión hidráulica HECHA     | Simulación de agua en heightmap | Formación de ríos, cañones |
  | Tectónica con conveción | Fluido viscoso + placas         | Manto convectivo           |
  | Avalancha/derrumbe      | Partículas granulares           | Arena, rocas cayendo       |

 

- **Algoritmos implementados:**
  - Navier-Stokes con proyeccion de presion y vorticidad
  - Spatial hashing O(n) para deteccion de vecinos
  - Barnes-Hut quadtree O(n log n) para N-body
  - Erosion hidraulica basada en particulas con sedimento
  - Position-Based Dynamics para restricciones de tela

- **Infraestructura:**
  - `eigenlab-core/include/` - Headers C++
  - `eigenlab-core/src/` - Implementaciones
  - `eigenlab-core/CMakeLists.txt` - Build system
  - `_wasm/` - Binarios distribuidos

### 21 Diciembre 2025 (Actualización 2)
- ✅ **Math Visual Lab expandido: 23 → 27 simulaciones**
  - `probabilidad.html` - 4 distribuciones (uniforme, normal, binomial, Poisson) + Box-Muller
  - `funciones.html` - Explorador con transformaciones a·f(b(x-h))+k, derivadas/integrales
  - `geometria-3d.html` - 9 sólidos 3D (cubo, esfera, 5 Platónicos) + Three.js + Euler V-A+C=2
  - `ecuaciones-diferenciales.html` - 6 EDOs + campos de pendientes + Euler vs RK4
- ✅ **4 nuevas guías pedagógicas** en `guides/`
- ✅ **Portal actualizado**: 117+ simulaciones
- ✅ **Knowledge Graph**: 6 nuevos nodos + 15 conexiones
- 📊 **Totales**: 115 simulaciones, 89 guías (77%)

### 21 Diciembre 2025
- 📊 **Inventario completo del ecosistema**
  - 111 simulaciones totales (antes se reportaban ~117)
  - 85 guías pedagógicas (77% cobertura)
  - Excel generado: `eigenlab_simulaciones.xlsx`
- ✅ **Simulaciones confirmadas como existentes** (estaban en demanda):
  - `termodinamica.html`, `ondas.html` (Physics)
  - `enlaces.html`, `cinetica-colisiones.html` (Chemistry)
  - `double-pendulum.html`, `three-body.html` (Math)
- 🔴 **Labs que necesitan guías:**
  - Physics Visual Lab: 18/20 sin guía
  - Math Generative Art Lab: 7/7 sin guía

### Diciembre 2025 (Actualización 2)
- ✅ **Nuevas Simulaciones con Guías**
  - `trigonometria.html` (Math Visual Lab) - Círculo unitario, funciones trigonométricas, ondas
  - `ritmos-circadianos.html` (Biology Visual Lab) - Reloj biológico, genes reloj, jet lag
- ✅ **82 Guías Pedagógicas Completadas** (antes 80)
- ✅ **Math Visual Lab: 23 simulaciones** (antes 22)
- ✅ **Biology Visual Lab: 7 simulaciones** (antes 6)

### Diciembre 2025 (Actualización)
- ✅ **80 Guías Pedagógicas Completadas**
  - Chemistry Visual Lab: 16 guías
  - Math Visual Lab: 22 guías
  - Biochem Visual Lab: 15 guías
  - Biology Visual Lab: 6 guías
  - Geology Visual Lab: 6 guías
  - Astronomy Visual Lab: 6 guías
  - Astronomy Sound Lab: 3 guías
  - Computation Lab: 6 guías
- ✅ **Links a Guías en Simulaciones** - Todas las 77 simulaciones con guías tienen link en sidebar
- ✅ **Espiral Virtuosa** - Nuevo framework de auto-evolución documentado en ROADMAP
- ✅ **Mapa de Demanda** - Análisis de 80 guías completado
  - 22 simulaciones/guías demandadas identificadas
  - 8 hubs de conexión mapeados
  - 23 links rotos corregidos en 18 archivos

### Diciembre 2025
- ✅ **Caminos de Aprendizaje Expandidos** - 12 rutas interdisciplinarias (`_portal/paths/`)
  - 7 nuevos caminos: Energía, Patrones, Escalas, Ondas, Feedback, Información, Simetría
  - Cada camino con 5 simulaciones y conexiones explicadas
  - Previews animados en el índice de caminos
- ✅ **Caminos de Aprendizaje** - 5 rutas originales
  - Entender el Caos (Physics → Math → Biology)
  - Oscilaciones Universales (Physics → Chemistry → Biology → Astronomy)
  - Estados y Transiciones (Computation → Biology → Chemistry → Geology)
  - Algoritmos en Acción (Computation → Math)
  - Emergencia (Math → Biology)
- ✅ **Knowledge Graph** - Mapa interactivo de conexiones (`_portal/knowledge-graph.html`)
  - ~110 nodos (simulaciones + conceptos transversales)
  - ~230 conexiones intra e inter-disciplina
  - Navegación: click abre simulación, Ctrl+click nueva pestaña
  - Ondas de propagación visual al hacer click en conceptos
  - Filtros por disciplina, búsqueda, zoom
  - Tecnología: D3.js force-directed graph
- ✅ **Guías pedagógicas** - Geology, Biology, Biochemistry, Astronomy
- ✅ **Computation Lab** - 6 simulaciones + guias pedagogicas
  - Bubble Sort Race (ordenamiento con visualizacion)
  - Binary Search Tree (insercion, busqueda, eliminacion)
  - Logic Gates Sandbox (AND, OR, NOT, XOR, NAND, NOR)
  - Stack & Heap Visualizer (modelo de memoria)
  - Perceptron Playground (clasificador lineal)
  - Finite Automata (DFA interactivo)
  - 6 guias pedagogicas completas en `guides/`
- ✅ **Portal actualizado** - Nuevo header con manifiesto
- ✅ **MANIFESTO.md** - Filosofia pedagogica documentada
- ✅ **Geology Visual Lab** - 6 simulaciones completadas
  - Terremotos (ondas sísmicas P, S, superficiales + sismograma)
  - Volcanes (escudo, estratovolcán, caldera + índice VEI)
  - Tectónica de Placas (mapa mundial, bordes, cortes transversales)
  - Ciclo de Rocas (ígneas, sedimentarias, metamórficas interactivo)
  - Erosión (hídrica, eólica, glaciar, costera con heightmap)
  - Estratigrafía (columna estratigráfica, fósiles, datación)

### Diciembre 2024
- ✅ **Biology Visual Lab** - 6 simulaciones completadas
  - Potencial de Acción (modelo Hodgkin-Huxley)
  - Ecosistema (Lotka-Volterra depredador-presa)
  - Genética Poblacional (Hardy-Weinberg, deriva genética)
  - Mitosis (división celular con fases animadas)
  - Meiosis (crossing-over, segregación independiente)
  - Selección Natural (algoritmo genético visual)
- ✅ **Astronomy Visual Lab** - 6 simulaciones completadas
  - Expansión del Universo (Ley de Hubble)
  - Agujero Negro (órbitas relativistas, spacetime grid)
  - Lentes Gravitacionales (anillo de Einstein)
  - Diagrama H-R (clasificación estelar)
  - Fases Lunares (geometría Sol-Tierra-Luna)
  - Estaciones (inclinación axial)
- ✅ **Astronomy Sound Lab** - 3 simulaciones completadas
  - Púlsares (pulsos de radio → ritmo, Joy Division style)
  - Ondas Gravitacionales (chirp de fusión, GW150914)
  - Música de las Esferas (TRAPPIST-1, Kepler-90)
