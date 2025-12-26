# EigenLab - Contexto para Claude Code

## Descripción del Proyecto

EigenLab es un ecosistema de laboratorios virtuales educativos. Cada simulación convierte ecuaciones y conceptos científicos en experiencias visuales e interactivas.

## Stack Tecnológico

- **HTML5** - Archivos autocontenidos, sin bundler
- **CSS3** - Tema oscuro, variables CSS custom properties
- **JavaScript ES6+** - Vanilla, sin frameworks
- **Canvas 2D** - Renderizado principal
- **Three.js** (CDN) - Visualizaciones 3D opcionales
- **Tailwind CSS** (CDN) - Solo para índices de laboratorio
- **Web Audio API** - Síntesis de sonido (Sound Labs)
- **Google Fonts: Inter** - Tipografía

## Estructura

```
EigenLab/
├── _portal/
│   ├── index.html              # Landing page unificada
│   ├── knowledge-graph.html    # Mapa de conexiones (D3.js)
│   └── paths/                  # Caminos de aprendizaje
├── _templates/                 # Plantillas base
│   ├── simulation-2d.html
│   └── simulation-3d.html
├── Physics/
│   ├── Physics Visual Lab/     # 19 sims
│   └── Physics Sound Lab/      # 7+ sims
├── Chemistry/
│   └── Chemistry Visual Lab/   # 17 sims
├── Biochemistry/
│   └── Biochem Visual Lab/     # 16 sims
├── Biology/
│   └── Biology Visual Lab/     # 6 sims
├── Geology/
│   └── Geology Visual Lab/     # 6 sims
├── Mathematics/
│   ├── Math Visual Lab/        # 27 sims
│   └── Math Generative Art Lab/# 7 sims
├── Astronomy/
│   ├── Astronomy Visual Lab/   # 6 sims
│   └── Astronomy Sound Lab/    # 3 sims
├── Computation/
│   └── Computation Lab/        # 6 sims
├── README.md
├── ROADMAP.md
└── CLAUDE.md
```

## Paleta de Colores

| Disciplina | Variable CSS | Hex |
|------------|--------------|-----|
| Physics Visual | `--color-physics-visual` | `#22c55e` |
| Physics Sound | `--color-physics-sound` | `#a855f7` |
| Chemistry | `--color-chemistry` | `#06b6d4` |
| Biochemistry | `--color-biochemistry` | `#ec4899` |
| Biology | `--color-biology` | `#10b981` |
| Geology | `--color-geology` | `#d97706` |
| Mathematics | `--color-mathematics` | `#f97316` |
| Astronomy | `--color-astronomy` | `#6366f1` |
| Computation | `--color-computation` | `#3b82f6` |

## Patrón de Simulación

Cada simulación sigue esta estructura:

```html
<!DOCTYPE html>
<html lang="es">
<head>
    <!-- Meta, title, Google Fonts -->
    <style>
        :root {
            --bg-primary: #030712;
            --bg-secondary: #0f172a;
            --accent: #COLOR_DISCIPLINA;
        }
        /* Layout: flex container con canvas + sidebar */
    </style>
</head>
<body>
    <header><!-- Breadcrumb --></header>
    <main>
        <div id="canvas-container">
            <canvas id="canvas"></canvas>
        </div>
        <aside class="controls">
            <!-- Ecuación, sliders, botones, valores -->
        </aside>
    </main>
    <script>
        // CONFIG, canvas setup, event listeners
        // init(), update(dt), draw(), loop()
    </script>
</body>
</html>
```

## Métodos Numéricos

- **RK4**: Integración de EDOs (péndulos, órbitas, atractores)
- **Newton-Raphson**: Ecuaciones trascendentes
- **Diferencias finitas**: PDEs (difusión, Gray-Scott)
- **Monte Carlo**: Distribuciones probabilísticas

## Convenciones

- **Idioma UI**: Español
- **Idioma código**: Inglés
- **Archivos**: kebab-case (ej: `potencial-accion.html`)
- **Variables**: camelCase
- **Un HTML por simulación**, autocontenido
- **Cada laboratorio tiene `index.html`** con grid de previews

## Knowledge Graph

Visualización interactiva de todas las simulaciones y sus conexiones (`_portal/knowledge-graph.html`).

**Tecnología:** D3.js force-directed graph

**Características:**
- ~110 nodos (simulaciones + conceptos transversales)
- ~230 conexiones (intra e inter-disciplina)
- Click en simulación → abre simulación
- Ctrl+Click → abre en nueva pestaña
- Click en concepto → propaga onda visual + zoom
- Filtros por disciplina
- Búsqueda por nombre

**Nodos especiales (conceptos transversales):**
- RK4, Caos, Energía, Entropía, Gravedad
- Cuántica, Electromagnetismo, Relatividad
- Ondas, Metabolismo, Dogma Central

**Al agregar simulaciones:** Actualizar `graphData.nodes` y `graphData.links` en knowledge-graph.html

## Caminos de Aprendizaje

Rutas guiadas interdisciplinarias (`_portal/paths/`).

**Caminos actuales:**
| Camino | Concepto | Archivo |
|--------|----------|---------|
| Entender el Caos | Sensibilidad, atractores | chaos.html |
| Oscilaciones Universales | Todo vibra | oscillations.html |
| Estados y Transiciones | Máquinas de estado | states.html |
| Algoritmos en Acción | Complejidad computacional | algorithms.html |
| Emergencia | Lo simple → complejo | emergence.html |

**Estructura de cada camino:**
- Introducción al concepto unificador
- Lista ordenada de simulaciones (5-6 pasos)
- Explicación de qué observar en cada paso
- Por qué estas simulaciones están conectadas

**Al crear nuevos caminos:** Ver ROADMAP.md para propuestas y mejoras futuras

## Comandos Útiles

```bash
# Abrir portal
open _portal/index.html

# Abrir mapa de conexiones
open _portal/knowledge-graph.html

# Crear nueva simulación
cp _templates/simulation-2d.html [Disciplina]/[Lab]/nueva-sim.html
```

## Al Crear Nuevas Simulaciones

1. Usar plantilla de `_templates/`
2. Cambiar `--accent` al color de la disciplina
3. Actualizar breadcrumbs
4. Escribir ecuación principal en sidebar
5. Implementar `update(dt)` con física real (RK4 si es EDO)
6. Implementar `draw()` con visualización clara
7. Añadir preview animado al `index.html` del lab
8. Actualizar portal `_portal/index.html` si es nuevo lab
9. Agregar nodo y conexiones en `_portal/knowledge-graph.html`
10. Actualizar README.md y ROADMAP.md

## Modelos Científicos Implementados

### Biology Visual Lab
- **Hodgkin-Huxley**: Potencial de acción neuronal
- **Lotka-Volterra**: Dinámica depredador-presa
- **Hardy-Weinberg**: Genética poblacional + deriva genética
- **Mitosis/Meiosis**: Fases de división celular
- **Algoritmo genético**: Selección natural

### Physics Visual Lab
- Péndulos (simple, doble, acoplados)
- Ondas, interferencia, difracción
- Relatividad especial
- Termodinámica

### Astronomy
- Ley de Hubble, redshift
- Órbitas relativistas (Schwarzschild)
- Diagrama H-R

### Geology Visual Lab
- **Ondas sísmicas**: P, S, superficiales + sismograma
- **Volcanes**: Escudo, estratovolcán, caldera (VEI)
- **Tectónica de placas**: Deriva continental, bordes
- **Ciclo de rocas**: Ígneas, sedimentarias, metamórficas
- **Erosión**: Hídrica, eólica, glaciar, costera
- **Estratigrafía**: Columna estratigráfica, datación

### Mathematics
- Atractor de Lorenz
- Mandelbrot, Julia
- Topología (Möbius, Klein)

### Computation Lab
- **Bubble Sort Race**: Ordenamiento burbuja con visualización de barras
- **Binary Search Tree**: BST interactivo con inserción/búsqueda/eliminación
- **Logic Gates Sandbox**: Compuertas lógicas drag & drop (AND/OR/NOT/XOR/NAND/NOR)
- **Stack Heap Visualizer**: Modelo de memoria con stack y heap
- **Perceptron Playground**: Perceptrón clasificando puntos 2D
- **Finite Automata**: DFA con estados, transiciones y testing de strings
