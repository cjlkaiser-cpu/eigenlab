# EigenLab - Contexto para Claude Code

## Descripción del Proyecto

**EigenLab** es un ecosistema de laboratorios virtuales educativos que convierte ecuaciones y conceptos científicos en experiencias visuales, interactivas y sonoras. Cada simulación es autocontenida (HTML/CSS/JS en un solo archivo) y ejecutable directamente en el navegador sin necesidad de build tools.

**Repositorio:** https://github.com/cjlkaiser-cpu/eigenlab

**Filosofía:** Educación científica a través de la experimentación interactiva. Cada laboratorio combina rigor matemático con visualización inmersiva.

## Stack Tecnológico

### Core
- **HTML5** - Archivos autocontenidos, sin bundler
- **CSS3** - Tema oscuro, variables CSS (custom properties)
- **JavaScript ES6+** - Vanilla, sin frameworks
- **Google Fonts: Inter** - Tipografía principal

### Rendering
- **Canvas 2D** - Renderizado principal (mayoría de simulaciones)
- **Three.js** (CDN) - Visualizaciones 3D (Lorenz, órbitas, moléculas)
- **SVG** - Diagramas y gráficos vectoriales

### Interfaces
- **Tailwind CSS** (CDN) - Solo para índices de laboratorio
- **Custom CSS** - Controles y layouts de simulaciones

### Audio (Sound Labs)
- **Web Audio API** - Síntesis de sonido nativa
- **ADSR Envelopes** - Modelado de notas
- **Síntesis FM/Aditiva** - Timbres complejos

## Estructura del Proyecto

```
EigenLab/
├── _portal/
│   ├── index.html              # Landing page unificada
│   ├── knowledge-graph.html    # Mapa de conexiones (D3.js)
│   └── paths/                  # Caminos de aprendizaje interdisciplinarios
├── _templates/                 # Plantillas base
│   ├── simulation-2d.html
│   └── simulation-3d.html
├── Physics/
│   ├── Physics Visual Lab/     # 19 sims - Mecánica, ondas, termodinámica
│   └── Physics Sound Lab/      # 7+ sims - Ondas sonoras, resonancia
├── Chemistry/
│   └── Chemistry Visual Lab/   # 17 sims - Estructura atómica, reacciones
├── Biochemistry/
│   └── Biochem Visual Lab/     # 16 sims - Metabolismo, síntesis proteica
├── Biology/
│   └── Biology Visual Lab/     # 6 sims - Hodgkin-Huxley, Lotka-Volterra
├── Geology/
│   └── Geology Visual Lab/     # 6 sims - Ondas sísmicas, tectónica
├── Mathematics/
│   ├── Math Visual Lab/        # 27 sims - Geometría, topología, caos
│   ├── Math Generative Art Lab/# 7 sims - L-Systems, fractales, autómatas
│   └── Math Sound Lab/         # 8 sims - Sonificación matemática ⭐
│       └── CLAUDE.md           # Documentación técnica detallada
├── Astronomy/
│   ├── Astronomy Visual Lab/   # 6 sims - Órbitas, Hubble, HR diagrams
│   └── Astronomy Sound Lab/    # 3 sims - Pulsares, música de las esferas
├── Computation/
│   └── Computation Lab/        # 6 sims - Algoritmos, estructuras de datos
├── README.md                   # Documentación pública
├── ROADMAP.md                  # Planificación futura
└── CLAUDE.md                   # Este archivo (contexto para Claude)
```

## Laboratorios y Simulaciones

### Resumen por Disciplina

| Disciplina | Labs | Simulaciones | Color | Estado |
|------------|------|--------------|-------|--------|
| **Physics** | Visual (19) + Sound (7+) | 26+ | Verde/Púrpura | ✅ Maduro |
| **Chemistry** | Visual (17) | 17 | Cyan | ✅ Maduro |
| **Biochemistry** | Visual (16) | 16 | Rosa | ✅ Maduro |
| **Biology** | Visual (6) | 6 | Verde esmeralda | ✅ Completo |
| **Geology** | Visual (6) | 6 | Ámbar | ✅ Completo |
| **Mathematics** | Visual (27) + Gen Art (7) + Sound (8) | 42 | Naranja | ⭐ Activo |
| **Astronomy** | Visual (6) + Sound (3) | 9 | Índigo | ✅ Completo |
| **Computation** | Lab (6) | 6 | Azul | ✅ Completo |

**Total:** ~128+ simulaciones

### Math Sound Lab ⭐ (Nueva adición)

El **Math Sound Lab** es el laboratorio más reciente, completado en enero 2026 con **8 simulaciones** de sonificación matemática.

**Simulaciones:**
1. Game of Life Musical - Autómata celular pentatónico
2. Autómatas Celulares 1D Musical - Reglas de Wolfram sonificadas
3. Transformada de Fourier Interactiva - Síntesis aditiva (32 armónicos)
4. Números Primos como Ritmos - Espiral de Ulam + gaps rítmicos
5. Atractor de Lorenz Musical - Caos 3D → pitch/volumen/pan
6. Fractales Sonoros - Mandelbrot/Julia con síntesis FM
7. Cadenas de Markov Generativas - Composición algorítmica
8. L-Systems Musicales - Gramáticas de Lindenmayer fractales

**Documentación detallada:** `Mathematics/Math Sound Lab/CLAUDE.md`

**Características únicas:**
- Web Audio API con lazy initialization (iOS/Safari compatible)
- ADSR envelopes + filtros lowpass/highpass
- Síntesis FM y aditiva
- Escalas pentatónicas, modos griegos, cromática
- Gestión de polifonía (64 voces)
- Dual visualización (canvas 2D + Three.js)
- ~7,631 líneas de código total

## Paleta de Colores

Cada disciplina tiene un color de acento distintivo usado en sus simulaciones:

| Disciplina | Variable CSS | Hex | Uso |
|------------|--------------|-----|-----|
| Physics Visual | `--color-physics-visual` | `#22c55e` | Sliders, highlights |
| Physics Sound | `--color-physics-sound` | `#a855f7` | Audio labs |
| Chemistry | `--color-chemistry` | `#06b6d4` | Reacciones, moléculas |
| Biochemistry | `--color-biochemistry` | `#ec4899` | Metabolismo, ADN |
| Biology | `--color-biology` | `#10b981` | Células, ecosistemas |
| Geology | `--color-geology` | `#d97706` | Rocas, tectónica |
| Mathematics | `--color-mathematics` | `#f97316` | Fractales, caos |
| Astronomy | `--color-astronomy` | `#6366f1` | Órbitas, cosmología |
| Computation | `--color-computation` | `#3b82f6` | Algoritmos, estructuras |

## Métodos Numéricos Implementados

### Integración de EDOs
- **RK4 (Runge-Kutta 4)**: Péndulos, órbitas, atractores caóticos, Lorenz
- **Euler**: Simulaciones simples (partículas, difusión)

### Ecuaciones No Lineales
- **Newton-Raphson**: Raíces de ecuaciones trascendentes
- **Método de la Bisección**: Búsqueda de ceros

### PDEs (Ecuaciones Diferenciales Parciales)
- **Diferencias finitas**: Difusión térmica, ecuación de onda
- **Gray-Scott**: Reacción-difusión (patterns de Turing)

### Probabilidad y Estadística
- **Monte Carlo**: Distribuciones, integrales, simulaciones estocásticas
- **Cadenas de Markov**: Composición generativa, procesos aleatorios

### Álgebra Computacional
- **FFT (Fast Fourier Transform)**: Análisis espectral
- **Criba de Eratóstenes**: Números primos
- **Algoritmos de grafos**: BST, DFS, BFS

### Geometría Fractal
- **Mandelbrot/Julia**: Iteración compleja z² + c
- **L-Systems**: Gramáticas de Lindenmayer
- **Wolfram rules**: Autómatas celulares 1D (256 reglas)

## Patrón de Simulación

Cada simulación sigue una estructura estándar:

```html
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>[Nombre] - EigenLab</title>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg-primary: #030712;
            --bg-secondary: #0f172a;
            --bg-panel: #1e293b;
            --accent: #COLOR_DISCIPLINA;
        }
        /* Layout: flex container con canvas + sidebar */
    </style>
</head>
<body>
    <header class="header">
        <h1>[Título]</h1>
        <nav class="breadcrumb">
            <a href="../../_portal/index.html">EigenLab</a> /
            <a href="../index.html">[Disciplina] Lab</a>
        </nav>
    </header>
    <main class="main">
        <div class="canvas-container">
            <canvas id="canvas"></canvas>
        </div>
        <aside class="controls">
            <!-- Ecuación principal -->
            <!-- Sliders de parámetros -->
            <!-- Botones de control -->
            <!-- Valores calculados -->
        </aside>
    </main>
    <script>
        // === CONFIG ===
        const CONFIG = { /* parámetros globales */ };

        // === CANVAS SETUP ===
        const canvas = document.getElementById('canvas');
        const ctx = canvas.getContext('2d');

        function resizeCanvas() {
            const dpr = window.devicePixelRatio || 1;
            // ... handle DPR scaling
        }

        // === PHYSICS/MATH ===
        function init() { /* inicializar estado */ }
        function update(dt) { /* física/matemática */ }
        function draw() { /* renderizado */ }

        // === LOOP ===
        function loop(time) {
            const dt = (time - lastTime) / 1000;
            update(dt);
            draw();
            requestAnimationFrame(loop);
        }

        // === INIT ===
        init();
        requestAnimationFrame(loop);
    </script>
</body>
</html>
```

## Convenciones de Código

### Nomenclatura
- **Idioma UI**: Español (títulos, labels, breadcrumbs)
- **Idioma código**: Inglés (variables, funciones, comentarios)
- **Archivos**: kebab-case (`potencial-accion.html`, `atractor-lorenz-musical.html`)
- **Variables**: camelCase (`audioContext`, `masterGain`, `currentVelocity`)
- **Constantes**: UPPER_CASE o CONFIG object (`MAX_VOICES`, `CONFIG.volume`)
- **Clases**: PascalCase (`LSystem`, `MarkovChain`, `Particle`)

### Estructura
- **Un HTML por simulación** (autocontenido)
- **Cada laboratorio tiene `index.html`** con grid de previews
- **CSS inline** en `<style>` tag (no archivos separados)
- **JS inline** en `<script>` tag (no archivos separados)

### Canvas
- Siempre manejar `devicePixelRatio` para pantallas Retina
- Usar `requestAnimationFrame` para loops de animación
- Implementar `resizeCanvas()` con event listener

### Audio (Sound Labs)
- Lazy initialization del `AudioContext` (iOS/Safari)
- ADSR envelopes para todas las notas
- Limitar polifonía (64 voces típico)
- Cleanup de osciladores con `.stop(time)`

## Knowledge Graph

Visualización interactiva de **todas las simulaciones** y sus conexiones interdisciplinarias.

**Archivo:** `_portal/knowledge-graph.html`

**Tecnología:** D3.js force-directed graph

**Características:**
- ~110+ nodos (simulaciones + conceptos transversales)
- ~230+ conexiones (intra e inter-disciplina)
- Click en simulación → abre en misma pestaña
- Ctrl+Click → abre en nueva pestaña
- Click en concepto → propaga onda visual + zoom
- Filtros por disciplina con checkboxes
- Búsqueda por nombre con highlight

**Nodos especiales (conceptos transversales):**
- **Métodos numéricos**: RK4, Monte Carlo, FFT
- **Conceptos físicos**: Caos, Energía, Entropía, Ondas, Gravedad
- **Cuántica**: Schrödinger, orbitales, spin
- **Relatividad**: Schwarzschild, Lorentz, E=mc²
- **Biología**: Metabolismo, Dogma Central, Hardy-Weinberg
- **Matemática**: Fractales, Topología, Números primos

**Actualización:** Al agregar nuevas simulaciones, actualizar `graphData.nodes` y `graphData.links`

## Caminos de Aprendizaje

Rutas guiadas interdisciplinarias que conectan simulaciones de diferentes disciplinas bajo un tema común.

**Directorio:** `_portal/paths/`

**Caminos actuales:**

| Camino | Concepto Unificador | Simulaciones | Archivo |
|--------|---------------------|--------------|---------|
| **Entender el Caos** | Sensibilidad a condiciones iniciales | Lorenz, péndulo doble, mandelbrot | `chaos.html` |
| **Oscilaciones Universales** | Todo vibra | Péndulos, ondas, orbitales, fourier | `oscillations.html` |
| **Estados y Transiciones** | Máquinas de estado | Autómatas, Markov, mitosis | `states.html` |
| **Algoritmos en Acción** | Complejidad computacional | Sorting, BST, DFA | `algorithms.html` |
| **Emergencia** | Complejidad de simplicidad | Game of Life, Gray-Scott, Lorenz | `emergence.html` |

**Estructura de cada camino:**
- Introducción conceptual (¿por qué estas sims están conectadas?)
- Lista ordenada de 5-6 simulaciones
- Explicación de qué observar en cada paso
- Preguntas guía para reflexión

**Futuros caminos:** Ver `ROADMAP.md`

## Documentación Modular

Cada laboratorio grande tiene su propio archivo `CLAUDE.md` con documentación técnica detallada:

### Math Sound Lab
**Archivo:** `Mathematics/Math Sound Lab/CLAUDE.md`

**Contenido:**
- Arquitectura de audio (5 patrones completos)
- Desglose de 8 simulaciones
- Escalas musicales y teoría
- Referencias cruzadas entre archivos
- Mejores prácticas de Web Audio API
- ~950 líneas de documentación técnica

### Futuros CLAUDE.md (por crear):
- `Physics/Physics Visual Lab/CLAUDE.md` (19 simulaciones)
- `Chemistry/Chemistry Visual Lab/CLAUDE.md` (17 simulaciones)
- `Mathematics/Math Visual Lab/CLAUDE.md` (27 simulaciones)
- `Biochemistry/Biochem Visual Lab/CLAUDE.md` (16 simulaciones)

**Estrategia:** Cada lab con 5+ simulaciones debería tener su CLAUDE.md detallado. Este archivo maestro hace referencia a ellos.

## Comandos Útiles

```bash
# Navegación
cd /Users/carlos/Projects/EigenLab
open _portal/index.html                    # Abrir portal principal
open _portal/knowledge-graph.html          # Mapa de conexiones
open "Mathematics/Math Sound Lab/index.html"  # Lab específico

# Crear nueva simulación
cp _templates/simulation-2d.html "Physics/Physics Visual Lab/nueva-sim.html"

# Git
git status
git add [archivos]
git commit -m "feat(disciplina): add [nombre]"
git push origin main
```

## Workflow: Agregar Nueva Simulación

### Paso 1: Crear archivo desde plantilla
```bash
cp _templates/simulation-2d.html "[Disciplina]/[Lab]/nombre-simulacion.html"
```

### Paso 2: Configurar simulación
- Cambiar `--accent` al color de la disciplina (ver tabla arriba)
- Actualizar `<title>` y breadcrumbs
- Escribir ecuación principal en sidebar

### Paso 3: Implementar física/matemática
- `init()`: Inicializar estado
- `update(dt)`: Física real (usar RK4 si es EDO)
- `draw()`: Visualización clara

### Paso 4: Añadir preview al index
Editar `[Lab]/index.html` y agregar card con preview animado

### Paso 5: Actualizar portal
Si es un **nuevo laboratorio** (no nueva simulación en lab existente):
- Actualizar `_portal/index.html` con nueva tarjeta
- Incrementar contador de simulaciones

### Paso 6: Knowledge Graph
Editar `_portal/knowledge-graph.html`:
- Agregar nodo a `graphData.nodes`
- Agregar conexiones a `graphData.links`

### Paso 7: Documentación
- Actualizar `README.md` con nueva simulación
- Si aplica, actualizar `ROADMAP.md`
- Si lab tiene CLAUDE.md, actualizar con nueva sim

### Paso 8: Commit
```bash
git add [archivos modificados]
git commit -m "feat([disciplina-lab]): add [nombre simulación]

[Descripción breve de la simulación]
[Características clave]

Co-Authored-By: Claude Sonnet 4.5 <noreply@anthropic.com>"
git push origin main
```

## Modelos Científicos Destacados

### Biología
- **Hodgkin-Huxley**: Potencial de acción neuronal (4 EDOs acopladas)
- **Lotka-Volterra**: Dinámica depredador-presa
- **Hardy-Weinberg**: Genética poblacional + deriva genética
- **Mitosis/Meiosis**: División celular paso a paso
- **Algoritmos genéticos**: Selección natural simulada

### Física
- **Péndulos**: Simple, doble (caos), acoplados (modos normales)
- **Ondas**: Superposición, interferencia, difracción
- **Relatividad especial**: Lorentz, dilatación temporal
- **Termodinámica**: Distribución de Maxwell-Boltzmann

### Astronomía
- **Ley de Hubble**: Expansión del universo + redshift
- **Órbitas relativistas**: Precesión de Schwarzschild
- **Diagrama H-R**: Clasificación estelar
- **Pulsares**: Faros cósmicos sonificados

### Geología
- **Ondas sísmicas**: P, S, superficiales + sismogramas
- **Volcanes**: Escudo, estratovolcán, caldera (VEI scale)
- **Tectónica de placas**: Deriva continental, bordes
- **Ciclo de rocas**: Ígneas, sedimentarias, metamórficas
- **Erosión**: Hídrica, eólica, glaciar, costera

### Matemáticas
- **Atractor de Lorenz**: Sistema caótico 3D
- **Mandelbrot/Julia**: Iteración compleja z² + c
- **Topología**: Banda de Möbius, botella de Klein
- **L-Systems**: Gramáticas generativas (fractales vegetales)
- **Cadenas de Markov**: Procesos estocásticos

### Computación
- **Bubble Sort Race**: Complejidad O(n²) visualizada
- **Binary Search Tree**: Árbol binario de búsqueda
- **Logic Gates**: Sandbox de compuertas lógicas
- **Stack & Heap**: Modelo de memoria
- **Perceptrón**: Red neuronal simple
- **DFA**: Autómata finito determinista

## Recursos y Referencias

### Tecnologías
- **Canvas API**: https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API
- **Web Audio API**: https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API
- **Three.js**: https://threejs.org/docs/
- **D3.js**: https://d3js.org/

### Matemática y Ciencia
- **Wolfram MathWorld**: https://mathworld.wolfram.com/
- **OEIS**: https://oeis.org/ (secuencias numéricas)
- **PhET Interactive Simulations**: https://phet.colorado.edu/

### Git
- **Repositorio**: https://github.com/cjlkaiser-cpu/eigenlab
- **Conventional Commits**: https://www.conventionalcommits.org/

## Estado del Proyecto

**Última actualización:** 2026-01-10

**Estadísticas:**
- **~128+ simulaciones** totales
- **9 disciplinas** científicas
- **14 laboratorios** temáticos
- **110+ nodos** en Knowledge Graph
- **5 caminos** de aprendizaje

**Laboratorio más reciente:** Math Sound Lab (8 simulaciones, enero 2026)

**Commits recientes:**
```
08f26dc docs(math-sound): add comprehensive documentation
22253d6 feat(math-sound): add L-Systems Musicales simulation
00ff40b feat(math-sound): add Cadenas de Markov Generativas
```

**Próximos hitos:** Ver `ROADMAP.md`

---

**Nota para Claude Code:** Este es el archivo maestro de contexto. Para detalles técnicos de laboratorios específicos, consulta sus archivos `CLAUDE.md` individuales (ej: `Mathematics/Math Sound Lab/CLAUDE.md`).
