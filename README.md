# EigenLab

**Simulaciones interactivas donde las ecuaciones cobran vida.**

Ecosistema de laboratorios virtuales que cubre Física, Química, Bioquímica, Biología, Geología, Matemáticas, Astronomía, Computación, IA y Música. Cada simulación convierte conceptos abstractos en experiencias visuales, sonoras e interactivas.

---

## Laboratorios

| Disciplina | Laboratorio | Sims | Descripción |
|------------|-------------|:----:|-------------|
| **Physics** | [Physics Visual Lab](Physics/Physics%20Visual%20Lab/) | 30 | Mecánica, ondas, fluidos, termodinámica, relatividad, WASM |
| **Physics** | [Physics Sound Lab](Physics/Physics%20Sound%20Sound/) | 14 | Metrónomos físicos (7) + música generativa (7) |
| **Chemistry** | [Chemistry Visual Lab](Chemistry/Chemistry%20Visual%20Lab/) | 19 | Estructura atómica, equilibrio, electroquímica, gases |
| **Biochemistry** | [Biochem Visual Lab](Biochemistry/Biochem%20Visual%20Lab/) | 18 | ADN, proteínas, metabolismo, membranas |
| **Biology** | [Biology Visual Lab](Biology/Biology%20Visual%20Lab/) | 11 | Hodgkin-Huxley, Lotka-Volterra, boids masivo, circadianos |
| **Geology** | [Geology Visual Lab](Geology/Geology%20Visual%20Lab/) | 10 | Tectónica, terremotos, volcanes, erosión hidráulica (WASM) |
| **Mathematics** | [Math Visual Lab](Mathematics/Math%20Visual%20Lab/) | 28 | Fractales, caos, topología, campos vectoriales (WASM) |
| **Mathematics** | [Math Generative Art Lab](Mathematics/Math%20Generative%20Art%20Lab/) | 9 | Reacción-difusión, L-Systems, flow fields, Mandelbulb |
| **Mathematics** | [Math Sound Lab](Mathematics/Math%20Sound%20Lab/) | 11 | Sonificación matemática: Lorenz, Fourier, primos, Markov |
| **Mathematics** | [Chaos Lab](Mathematics/Chaos%20Lab/) | curso | 10 atractores extraños + 30 lecciones |
| **Mathematics** | [Platonic Lab](Mathematics/Platonic%20Lab/) | curso | Poliedros, 4D, Conway operators + 33 lecciones |
| **Astronomy** | [Astronomy Visual Lab](Astronomy/Astronomy%20Visual%20Lab/) | 11 | Hubble, agujeros negros, galaxias (WASM), anillos Saturno |
| **Astronomy** | [Astronomy Sound Lab](Astronomy/Astronomy%20Sound%20Lab/) | 3 | Púlsares, ondas gravitacionales, música de las esferas |
| **Computation** | [Computation Lab](Computation/Computation%20Lab/) | 6 | Algoritmos, estructuras de datos, autómatas, perceptrón |
| **AI** | [AI Visual Lab](AI/AI%20Visual%20Lab/) | 20 | Redes neuronales, transformers, difusión, RL, LLMs |
| **Music** | [Music Theory Lab](Music/Music%20Theory%20Lab/) | 9 | Sistema Armónico Áureo: escalas, armonía y composición con φ |

**Total: 16 laboratorios · ~199 simulaciones · 10 disciplinas**

> Incluye 2 cursos estructurados (Chaos Lab, Platonic Lab) y Euler Lab (100 problemas en submodule).
> Portal unificado con Knowledge Graph interactivo (167 nodos) y 12 caminos de aprendizaje.

---

## Estructura del Proyecto

```
EigenLab/
├── _portal/                    # Portal unificado
│   ├── index.html              # Landing page (10 disciplinas)
│   ├── academia.html           # Cursos, teoría y papers académicos
│   ├── knowledge-graph.html    # Mapa D3.js (167 nodos, 409 enlaces)
│   ├── wasm-labs.html          # Catálogo de simulaciones WASM
│   └── paths/                  # 12 caminos de aprendizaje interdisciplinarios
│
├── _templates/                 # Plantillas reutilizables
│   ├── simulation-2d.html      # Base para simulaciones Canvas 2D
│   ├── simulation-3d.html      # Base para simulaciones Three.js
│   └── styles-base.css         # Variables CSS y utilidades
│
├── _wasm/                      # Binarios WebAssembly compartidos
│   ├── eigenlab-core.js        # Glue code Emscripten
│   └── eigenlab-core.wasm      # Motor compilado (~330KB, 22 módulos)
│
├── eigenlab-core/              # Motor C++17 compilado con Emscripten
│   ├── include/                # Headers (.hpp)
│   ├── src/physics/            # 22 módulos: fluidos, boids, N-body...
│   └── CMakeLists.txt          # Build system
│
├── Physics/
│   ├── Physics Visual Lab/     # 30 sims: mecánica, fluidos, relatividad
│   └── Physics Sound Lab/      # 14 sims: metrónomos + generativos
│
├── Chemistry/
│   └── Chemistry Visual Lab/   # 19 sims: atómica, termodinámica
│
├── Biochemistry/
│   └── Biochem Visual Lab/     # 18 sims: ADN, metabolismo, proteínas
│
├── Biology/
│   └── Biology Visual Lab/     # 11 sims: neurona, ecosistema, boids
│
├── Geology/
│   └── Geology Visual Lab/     # 10 sims: tectónica, erosión hidráulica
│
├── Mathematics/
│   ├── Math Visual Lab/        # 28 sims: fractales, caos, topología
│   ├── Math Generative Art Lab/# 9 sims: Gray-Scott, L-Systems, Mandelbulb
│   ├── Math Sound Lab/         # 11 sims: sonificación matemática
│   ├── Chaos Lab/              # Curso: 10 atractores + 30 lecciones
│   ├── Platonic Lab/           # Curso: poliedros, 4D + 33 lecciones
│   └── Euler Lab/              # Curso: 100 problemas, 4 niveles (submodule)
│
├── Astronomy/
│   ├── Astronomy Visual Lab/   # 11 sims: cosmología, galaxias, Saturno
│   └── Astronomy Sound Lab/    # 3 sims: púlsares, LIGO, esferas
│
├── Computation/
│   └── Computation Lab/        # 6 sims: algoritmos, autómatas, perceptrón
│
├── AI/
│   └── AI Visual Lab/          # 20 sims: redes, transformers, difusión
│
├── Music/
│   └── Music Theory Lab/       # 9 sims: Sistema Armónico Áureo (φ)
│
├── README.md                   # Este archivo
└── ROADMAP.md                  # Plan de desarrollo futuro
```

---

## Stack Tecnológico

- **HTML5** - Archivos autocontenidos, sin bundler
- **CSS3** - Tema oscuro, variables CSS, responsive
- **JavaScript ES6+** - Vanilla, sin frameworks
- **Canvas 2D** - Renderizado y animaciones
- **Three.js** - Visualizaciones 3D (química, algunas simulaciones)
- **Web Audio API** - Síntesis de sonido (Physics Sound Lab)
- **C++17 / WebAssembly** - Motor de alto rendimiento para simulaciones intensivas

### Motor WASM (eigenlab-core)

Para simulaciones que requieren miles de cálculos por frame, usamos un motor C++ compilado a WebAssembly:

| Módulo | Algoritmo | Sim desplegada |
|--------|-----------|----------------|
| **FluidSolver** | Navier-Stokes 2D | `fluidos-2d.html` |
| **BoidSystem** | Reynolds + spatial hash | `boids-masivo.html` |
| **GalaxySimulator** | Barnes-Hut N-body | `formacion-galaxias.html` |
| **ErosionSimulator** | Erosión hidráulica | `erosion-hidraulica.html` |
| **ClothSimulator** | Verlet + PBD | `simulacion-tela.html` |
| **SaturnRings** | Keplerian orbits | `anillos-saturno.html` |
| **VectorFields** | RK4 advection | `campos-vectoriales.html` |
| **+ 15 módulos** | Mandelbulb, MD, epidemiología... | pendientes de front-end |

```bash
# Compilar el motor (requiere Emscripten)
cd eigenlab-core && mkdir build && cd build
emcmake cmake .. && emmake make -j4
```

### Métodos Numéricos Implementados

- Runge-Kutta 4 (RK4) para integración de EDOs
- Newton-Raphson para ecuaciones trascendentes
- Diferencias finitas para ecuaciones diferenciales parciales
- Monte Carlo para distribuciones de probabilidad
- Barnes-Hut para simulaciones N-body O(n log n)
- Position-Based Dynamics para restricciones de cuerpos blandos

---

## Características

- **Sin servidor** - Abre cualquier `index.html` directamente en el navegador
- **Autocontenido** - Cada simulación es un único archivo HTML
- **Interactivo** - Controles en tiempo real, drag & drop
- **Responsive** - Funciona en desktop y móvil
- **Tema oscuro** - Diseño consistente en todos los laboratorios
- **Educativo** - Ecuaciones visibles, valores calculados en tiempo real
- **Conectado** - Knowledge Graph muestra relaciones entre simulaciones

---

## Knowledge Graph

Visualización interactiva de todas las simulaciones y sus conexiones conceptuales.

- **167 nodos** - Simulaciones + conceptos transversales (RK4, Caos, Entropía, φ...)
- **409 conexiones** - Intra e inter-disciplina
- **Navegable** - Click abre simulación, Ctrl+Click nueva pestaña
- **Ondas de propagación** - Click en conceptos propaga onda visual

```bash
open _portal/knowledge-graph.html
```

---

## Caminos de Aprendizaje

Rutas guiadas que conectan simulaciones de distintas disciplinas a través de un concepto unificador.

| Camino | Tema | Disciplinas |
|--------|------|-------------|
| **Entender el Caos** | Del orden al desorden | Physics → Math → Biology |
| **Oscilaciones Universales** | Todo vibra | Physics → Chemistry → Biology → Astronomy |
| **Estados y Transiciones** | Máquinas que cambian | Computation → Biology → Chemistry → Geology |
| **Algoritmos en Acción** | Cómo resuelven problemas | Computation → Math |
| **Emergencia** | Lo simple genera lo complejo | Math → Biology |
| **Energía en Transformación** | Conservación y disipación | Physics → Chemistry → Biochemistry |
| **Patrones de la Naturaleza** | Fibonacci, fractales | Math → Biology → Geology |
| **Del Átomo al Cosmos** | Escalas del universo | Chemistry → Physics → Astronomy |
| **Ondas Everywhere** | Propagación | Physics → Math → Astronomy → Music |
| **Retroalimentación** | Feedback loops | Biology → Chemistry → Computation |
| **Información y Complejidad** | Bits a comportamiento | Computation → Math → AI |
| **Simetría y Ruptura** | Orden y desorden | Math → Physics → Chemistry |

Cada camino incluye:
- Introducción al concepto unificador
- Secuencia ordenada de simulaciones
- Explicación de qué observar en cada paso
- Conexiones entre las simulaciones

```bash
open _portal/paths/index.html
```

---

## Uso

### Simulaciones JavaScript (mayoría)
Abre directamente en el navegador:
```bash
open _portal/index.html
open Physics/Physics\ Visual\ Lab/index.html
```

### Simulaciones WASM (alto rendimiento)
Requieren servidor HTTP local (restricción de seguridad del navegador):
```bash
# Iniciar servidor en la raíz de EigenLab
cd EigenLab
python3 -m http.server 8080

# Abrir en navegador
open http://localhost:8080/Physics/Physics%20Visual%20Lab/fluidos-2d.html
```

**Simulaciones WASM desplegadas (7):**
- `fluidos-2d.html` — Navier-Stokes 2D
- `simulacion-tela.html` — Cloth physics (Verlet + PBD)
- `boids-masivo.html` — Flocking (Reynolds, 1000+ agentes)
- `formacion-galaxias.html` — N-body Barnes-Hut
- `erosion-hidraulica.html` — Erosión hidráulica por gotas
- `anillos-saturno.html` — Anillos Keplerianos (10K+ partículas)
- `campos-vectoriales.html` — Advección RK4

> El motor tiene **22 módulos compilados**; 15 pendientes de front-end (Mandelbulb, dinámica molecular, epidemiología espacial, plasma, proteínas...).

---

## Crear Nueva Simulación

Usa las plantillas en `_templates/`:

```bash
# Copiar plantilla 2D
cp _templates/simulation-2d.html Physics/Physics\ Visual\ Lab/nueva-simulacion.html

# Copiar plantilla 3D
cp _templates/simulation-3d.html Chemistry/Chemistry\ Visual\ Lab/nueva-molecula.html
```

Personaliza:
1. Título y breadcrumbs
2. Color de acento (`--accent` en CSS)
3. Ecuación principal
4. Parámetros y controles
5. Lógica de física/renderizado

---

## Paleta de Colores por Disciplina

| Disciplina | Color | Hex |
|------------|-------|-----|
| Physics Visual | Verde | `#22c55e` |
| Physics Sound | Púrpura | `#a855f7` |
| Chemistry | Cyan | `#06b6d4` |
| Biochemistry | Rosa oscuro | `#ec4899` |
| Biology | Esmeralda | `#10b981` |
| Geology | Ámbar | `#d97706` |
| Mathematics | Naranja | `#f97316` |
| Astronomy | Índigo | `#6366f1` |
| Computation | Azul eléctrico | `#3b82f6` |
| AI | Violeta | `#8b5cf6` |
| Music | Rosa | `#f472b6` |

---

## Roadmap

Ver [ROADMAP.md](ROADMAP.md) para el plan de desarrollo y nuevas disciplinas.

---

## Prompt de Reproducción

<details>
<summary>Prompt para recrear EigenLab desde cero</summary>

```markdown
# EigenLab - Simulaciones Interactivas

Crea un ecosistema de laboratorios virtuales educativos donde las ecuaciones cobran vida.
Colección de simulaciones interactivas de Física, Química, Bioquímica, Biología, Matemáticas y Astronomía.

## Stack

- **HTML5** - Archivos autocontenidos, sin bundler ni build step
- **CSS3** - Tema oscuro, variables CSS custom properties, responsive
- **JavaScript ES6+** - Vanilla, sin frameworks (React, Vue, etc.)
- **Canvas 2D** - Renderizado principal para simulaciones
- **Three.js** (CDN) - Visualizaciones 3D opcionales (química molecular, orbifolds)
- **Tailwind CSS** (CDN) - Opcional para índices de laboratorio
- **Web Audio API** - Síntesis de sonido para Sound Labs
- **Google Fonts: Inter** - Tipografía consistente

## Estructura

EigenLab/
├── _portal/
│   └── index.html              # Landing page con previews animados de cada lab
├── _templates/
│   ├── simulation-2d.html      # Plantilla base Canvas 2D
│   ├── simulation-3d.html      # Plantilla base Three.js
│   └── styles-base.css         # Variables CSS compartidas
├── Physics/
│   ├── Physics Visual Lab/     # 19 simulaciones (mecánica, ondas, relatividad)
│   └── Physics Sound Lab/      # 7+ generativos (metrónomos, música generativa)
├── Chemistry/
│   └── Chemistry Visual Lab/   # 17 simulaciones (atómica, termodinámica)
├── Biochemistry/
│   └── Biochem Visual Lab/     # 16 simulaciones (ADN, metabolismo)
├── Biology/
│   └── Biology Visual Lab/     # 6 simulaciones (neurona, ecología, genética)
├── Mathematics/
│   ├── Math Visual Lab/        # 22 simulaciones (fractales, caos, topología)
│   └── Math Generative Art Lab/# 7 simulaciones (Gray-Scott, L-Systems)
├── Astronomy/
│   ├── Astronomy Visual Lab/   # 6 simulaciones (Hubble, H-R, agujeros negros)
│   └── Astronomy Sound Lab/    # 3 simulaciones (púlsares, ondas gravitacionales)
├── README.md
└── ROADMAP.md

## Paleta de Colores por Disciplina

| Disciplina | Variable CSS | Hex |
|------------|--------------|-----|
| Physics Visual | `--color-physics-visual` | `#22c55e` (verde) |
| Physics Sound | `--color-physics-sound` | `#a855f7` (púrpura) |
| Chemistry | `--color-chemistry` | `#06b6d4` (cyan) |
| Biochemistry | `--color-biochemistry` | `#ec4899` (rosa) |
| Biology | `--color-biology` | `#10b981` (esmeralda) |
| Mathematics | `--color-mathematics` | `#f97316` (naranja) |
| Astronomy | `--color-astronomy` | `#6366f1` (índigo) |

## Estructura de Simulación (Template)

Cada simulación es un archivo HTML autocontenido con:

1. **Header**: Título + breadcrumb de navegación
2. **Canvas container**: Área de renderizado (flex: 1)
3. **Panel de controles** (320px width):
   - Ecuación principal (Times New Roman, italic)
   - Sliders de parámetros con valor en tiempo real
   - Botones Play/Pause y Reset
   - Valores calculados (font monospace, color accent)

### Patrón de código JavaScript:

// 1. CONFIG object - parámetros y estado
const CONFIG = { param1: 50, running: false };

// 2. Canvas setup con devicePixelRatio
const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
let dpr = window.devicePixelRatio || 1;
function resizeCanvas() { /* ... */ }

// 3. Event listeners para controles
slider.addEventListener('input', (e) => { /* ... */ });

// 4. Estado de simulación
let state = { x: 0, y: 0, vx: 0, vy: 0 };
function init() { /* inicializar estado */ }

// 5. Física
function update(dt) { /* integración numérica */ }

// 6. Renderizado
function draw() { /* dibujar en canvas */ }

// 7. Loop principal con deltaTime
function loop(time) {
    const dt = Math.min((time - lastTime) / 1000, 0.1);
    update(dt);
    draw();
    requestAnimationFrame(loop);
}

## Métodos Numéricos

- **Runge-Kutta 4 (RK4)**: Integración de EDOs (péndulos, órbitas, atractores)
- **Newton-Raphson**: Ecuaciones trascendentes (anomalía de Kepler)
- **Diferencias finitas**: PDEs (difusión de calor, Gray-Scott)
- **Monte Carlo**: Distribuciones probabilísticas (orbitales atómicos)

## Características Clave

1. **Sin servidor** - Abrir index.html directamente en navegador
2. **Autocontenido** - Cada simulación = 1 archivo HTML (excepto CDNs)
3. **Responsive** - Funciona en desktop y móvil
4. **Tema oscuro** - `--bg-primary: #030712`, `--bg-secondary: #0f172a`
5. **Educativo** - Ecuaciones visibles, valores en tiempo real
6. **Interactivo** - Sliders, drag & drop, presets

## Convenciones

- **Idioma**: Español para UI, inglés para código
- **Naming**: camelCase para variables, kebab-case para archivos HTML
- **Comentarios**: Español para explicar física, inglés para código técnico
- **Archivos**: Un HTML por simulación, index.html por laboratorio
- **CSS**: Variables custom properties, inline en cada archivo
- **JS**: Vanilla ES6+, sin transpilación

## Portal Principal

El portal (`_portal/index.html`) incluye:
- Header con gradiente de todos los colores de disciplina
- Stats: laboratorios, simulaciones, disciplinas
- Grid de cards por disciplina con previews animados en canvas
- Cada preview es una mini-animación representativa del laboratorio

## Crear Nueva Simulación

1. Copiar `_templates/simulation-2d.html`
2. Cambiar `--accent` al color de la disciplina
3. Actualizar breadcrumbs
4. Escribir ecuación principal
5. Definir parámetros y sliders
6. Implementar `update(dt)` con física real
7. Implementar `draw()` con visualización
8. Añadir al index.html del laboratorio

## Sound Labs

Usan Web Audio API:
- `AudioContext` para crear contexto
- `OscillatorNode` para síntesis
- `GainNode` para volumen/ADSR
- `AnalyserNode` para FFT/visualización
- Sintetizar a partir de física (períodos orbitales → frecuencias)
```

</details>

---

## Autor

**Carlos Lorente Kaiser**

- GitHub: [@cjlkaiser-cpu](https://github.com/cjlkaiser-cpu)

---

## Licencia

MIT License - Uso libre para propósitos educativos.
