# EigenLab

**Simulaciones interactivas donde las ecuaciones cobran vida.**

Colección de laboratorios virtuales que cubren Física, Química, Bioquímica, Biología, Geología, Matemáticas, Astronomía y Computación. Cada simulación convierte conceptos abstractos en experiencias visuales, sonoras e interactivas.

---

## Laboratorios

| Disciplina | Laboratorio | Simulaciones | Descripción |
|------------|-------------|--------------|-------------|
| **Physics** | [Physics Visual Lab](Physics/Physics%20Visual%20Lab/) | 22 | Mecánica, ondas, fluidos, termodinámica, relatividad |
| **Physics** | [Physics Sound Lab](Physics/Physics%20Sound%20Lab/) | 7+ | Metrónomos físicos y música generativa |
| **Chemistry** | [Chemistry Visual Lab](Chemistry/Chemistry%20Visual%20Lab/) | 18 | Estructura atómica, termodinámica, electroquímica |
| **Biochemistry** | [Biochem Visual Lab](Biochemistry/Biochem%20Visual%20Lab/) | 15 | ADN, proteínas, metabolismo celular |
| **Biology** | [Biology Visual Lab](Biology/Biology%20Visual%20Lab/) | 8 | Neurociencia, ecología, genética, boids |
| **Geology** | [Geology Visual Lab](Geology/Geology%20Visual%20Lab/) | 7 | Tectónica, terremotos, volcanes, erosión hidráulica |
| **Mathematics** | [Math Visual Lab](Mathematics/Math%20Visual%20Lab/) | 27 | Fractales, caos, teoría de números, topología |
| **Mathematics** | [Math Generative Art Lab](Mathematics/Math%20Generative%20Art%20Lab/) | 7 | Reacción-difusión, L-Systems, flow fields |
| **Astronomy** | [Astronomy Visual Lab](Astronomy/Astronomy%20Visual%20Lab/) | 7 | Hubble, agujeros negros, formación de galaxias |
| **Astronomy** | [Astronomy Sound Lab](Astronomy/Astronomy%20Sound%20Lab/) | 3 | Púlsares, ondas gravitacionales, música de las esferas |
| **Computation** | [Computation Lab](Computation/Computation%20Lab/) | 6 | Algoritmos, estructuras de datos, redes neuronales, autómatas |

**Total: 11 laboratorios · 120 simulaciones · 8 disciplinas**

---

## Estructura del Proyecto

```
EigenLab/
├── _portal/                    # Portal unificado
│   ├── index.html              # Landing page
│   ├── knowledge-graph.html    # Mapa de conexiones interactivo
│   └── paths/                  # Caminos de aprendizaje
│
├── _templates/                 # Plantillas reutilizables
│   ├── simulation-2d.html      # Base para simulaciones Canvas 2D
│   ├── simulation-3d.html      # Base para simulaciones Three.js
│   └── styles-base.css         # Variables CSS y utilidades
│
├── _wasm/                      # Binarios WebAssembly compartidos
│   ├── eigenlab-core.js        # Glue code Emscripten
│   └── eigenlab-core.wasm      # Motor compilado (158KB)
│
├── eigenlab-core/              # Código fuente C++ del motor
│   ├── include/                # Headers (.hpp)
│   ├── src/                    # Implementaciones (.cpp)
│   └── CMakeLists.txt          # Build system
│
├── Physics/
│   ├── Physics Visual Lab/     # Simulaciones visuales de física
│   └── Physics Sound Lab/      # Sonificación y música generativa
│
├── Chemistry/
│   └── Chemistry Visual Lab/   # Simulaciones de química
│
├── Biochemistry/
│   └── Biochem Visual Lab/     # Simulaciones de bioquímica
│
├── Biology/
│   └── Biology Visual Lab/     # Simulaciones de biología
│
├── Geology/
│   └── Geology Visual Lab/     # Simulaciones de geología
│
├── Mathematics/
│   ├── Math Visual Lab/        # Visualizaciones matemáticas
│   └── Math Generative Art Lab/# Arte generativo matemático
│
├── Astronomy/
│   ├── Astronomy Visual Lab/   # Cosmología y astrofísica
│   └── Astronomy Sound Lab/    # Sonificación del cosmos
│
├── Computation/
│   └── Computation Lab/        # Algoritmos y ciencias de la computación
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

| Módulo | Algoritmo | Rendimiento |
|--------|-----------|-------------|
| **FluidSolver** | Navier-Stokes 2D | 256×256 celdas @ 60fps |
| **BoidSystem** | Reynolds + spatial hash | 1000+ agentes @ 60fps |
| **GalaxySimulator** | Barnes-Hut N-body | 5000+ partículas @ 30fps |
| **ErosionSimulator** | Erosión hidráulica | 256×256 terreno |
| **ClothSimulator** | Verlet + PBD | 1200 partículas @ 60fps |

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

- **~110 nodos** - Simulaciones + conceptos transversales (RK4, Caos, Energía...)
- **~230 conexiones** - Intra e inter-disciplina
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

**Simulaciones WASM:**
- `fluidos-2d.html` - Navier-Stokes
- `simulacion-tela.html` - Cloth physics
- `boids-masivo.html` - Flocking
- `formacion-galaxias.html` - N-body
- `erosion-hidraulica.html` - Terrain erosion

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
| Biochemistry | Rosa | `#ec4899` |
| Biology | Esmeralda | `#10b981` |
| Geology | Ámbar | `#d97706` |
| Mathematics | Naranja | `#f97316` |
| Astronomy | Índigo | `#6366f1` |
| Computation | Electric Blue | `#3b82f6` |

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
