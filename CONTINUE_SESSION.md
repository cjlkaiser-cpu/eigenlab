# Prompt de Continuación: Math Sound Lab - EigenLab

## Contexto del Proyecto

Estás trabajando en EigenLab, un ecosistema de laboratorios virtuales educativos con simulaciones científicas y matemáticas interactivas en HTML/CSS/JavaScript vanilla.

**Repositorio:** https://github.com/cjlkaiser-cpu/eigenlab
**Directorio local:** /Users/carlos/Projects/EigenLab/

## Estado Actual: Math Sound Lab - COMPLETADO ✅

El Math Sound Lab está ahora **completamente funcional** con **6 simulaciones** de sonificación matemática.

### Simulaciones Implementadas

| # | Archivo | Líneas | Concepto | Mapeo Sonoro |
|---|---------|--------|----------|--------------|
| 1 | `game-of-life-musical.html` | ~1,136 | Autómata celular 2D, emergencia | Filas→Notas, Cols→Octavas |
| 2 | `automatas-celulares-1d-musical.html` | 774 | Reglas de Wolfram (30, 90, 110, 184) | Generación→Tiempo, Posición→Nota |
| 3 | `fourier-interactiva.html` | 759 | Síntesis aditiva, análisis espectral | 32 Armónicos→Timbre |
| 4 | `numeros-primos-ritmos.html` | 798 | Teoría de números, Espiral de Ulam | Gaps→Duración, Posición→Pitch |
| 5 | `atractor-lorenz-musical.html` | 733 | Sistemas caóticos, EDOs (Three.js) | X→Pitch, Y→Volumen, Z→Pan |
| 6 | `fractales-sonoros.html` | 701 | Mandelbrot/Julia, geometría fractal | Iteraciones→FM Index |

**Total:** ~5,314 líneas de código

### Últimos Commits (Sesión Actual)

```
371a3f3 feat(math-sound): añadir Fractales Sonoros con FM Synthesis
0b1a51c feat(math-sound): añadir Atractor de Lorenz Musical
21f8582 feat(math-sound): añadir Números Primos como Ritmos
05cc10b feat(math-sound): añadir Autómatas Celulares 1D y Fourier Interactiva
```

**Estado del repositorio:** Todo pusheado, branch main actualizado

## Archivos Clave

```
Mathematics/Math Sound Lab/
├── index.html                              # Grid de 6 simulaciones
├── game-of-life-musical.html               # ✅ Autómata 2D
├── automatas-celulares-1d-musical.html     # ✅ Reglas de Wolfram
├── fourier-interactiva.html                # ✅ Síntesis aditiva
├── numeros-primos-ritmos.html              # ✅ Teoría de números
├── atractor-lorenz-musical.html            # ✅ Caos 3D
└── fractales-sonoros.html                  # ✅ Mandelbrot/Julia FM
```

Portal actualizado:
- `_portal/index.html` → Badge: **6 sims**

## Tecnologías Implementadas

**Audio Engine (Web Audio API):**
- ✅ Lazy initialization (iOS/Safari compatible)
- ✅ ADSR envelopes con filtros lowpass
- ✅ Síntesis FM (Frequency Modulation)
- ✅ Síntesis aditiva (hasta 32 armónicos)
- ✅ Gestión de polifonía (64 voces simultáneas)
- ✅ Pan estéreo y espacialización 3D
- ✅ 6+ escalas musicales (pentatónicas, modos griegos)
- ✅ 4 formas de onda (sine, triangle, square, sawtooth)

**Visualización:**
- ✅ Canvas 2D optimizado (5 simulaciones)
- ✅ Three.js 3D (Atractor de Lorenz)
- ✅ Efectos glow sincronizados con audio
- ✅ Animaciones 60 FPS
- ✅ 5 paletas de colores (fractales)
- ✅ Zoom infinito interactivo

**Matemática y Algoritmos:**
- ✅ Integración RK4 (Lorenz, autómatas)
- ✅ Criba de Eratóstenes (primos)
- ✅ Reglas de Wolfram 0-255
- ✅ FFT conceptual (Fourier)
- ✅ Espiral de Ulam
- ✅ Iteración compleja z² + c

## Próximas Simulaciones Sugeridas

Para continuar expandiendo el Math Sound Lab:

### Prioridad Alta

1. **Cadenas de Markov Generativas**
   - Composición algorítmica probabilística
   - Matriz de transición editable
   - Training con melodías conocidas
   - Generación de secuencias MIDI-like

2. **L-Systems Musicales**
   - Gramáticas generativas (Lindenmayer)
   - Crecimiento de plantas → melodías
   - Fractales vegetales sonoros
   - Reglas: F, +, -, [, ]

### Prioridad Media

3. **Ondas de Turing Sonoras**
   - Reacción-difusión (Gray-Scott)
   - Patrones emergentes → texturas sonoras
   - Parámetros: F, k (feed, kill)
   - Inspirado en Physics Sound Lab

4. **Secuencia de Fibonacci Musical**
   - Proporción áurea en ritmo
   - Espiral de Fibonacci visual
   - Ratios áureos → intervalos musicales
   - Conexión con naturaleza

5. **Péndulo de Foucault Musical**
   - Física + geometría
   - Precesión → cambio tonal
   - Visualización 3D con Three.js
   - Latitud → parámetros sonoros

6. **Curvas de Lissajous Sonoras**
   - x(t) = A·sin(aωt + δ)
   - y(t) = B·sin(bωt)
   - Ratios de frecuencia → armonía visual
   - Figuras de Lissajous clásicas

## Convenciones del Proyecto

- **Idioma UI:** Español
- **Idioma código:** Inglés (variables, funciones, comentarios)
- **Archivos:** kebab-case (`fourier-interactiva.html`)
- **Color Math Labs:** `#f97316` (orange)
- **Estructura:** HTML autocontenido, sin bundler
- **Audio:** Web Audio API nativa, lazy init

## Patrón de Audio (Referencia)

```javascript
// Lazy initialization
function initAudioContext() {
    if (isAudioInitialized) return;
    audioContext = new (window.AudioContext || window.webkitAudioContext)();
    masterGain = audioContext.createGain();
    masterGain.connect(audioContext.destination);
    isAudioInitialized = true;
}

// ADSR Envelope pattern (from game-of-life-musical.html)
const envelope = audioContext.createGain();
envelope.gain.setValueAtTime(0, now);
envelope.gain.linearRampToValueAtTime(volume, now + attack);
envelope.gain.linearRampToValueAtTime(sustain, now + attack + decay);
envelope.gain.exponentialRampToValueAtTime(0.001, now + duration);
```

## Archivos de Referencia

**Audio patterns:**
- `game-of-life-musical.html:328-650` - ADSR, gestión de voces
- `fourier-interactiva.html:467-485` - Síntesis aditiva
- `fractales-sonoros.html:339-420` - FM Synthesis
- `atractor-lorenz-musical.html:289-337` - Mapeo continuo 3D

**Visualización:**
- `fourier-interactiva.html:520-583` - Triple canvas sync
- `fractales-sonoros.html:162-243` - Rendering optimizado
- `atractor-lorenz-musical.html:125-242` - Three.js setup

## Cómo Agregar Nueva Simulación

1. Crear archivo: `Mathematics/Math Sound Lab/nueva-sim.html`
2. Usar color `--accent: #f97316`
3. Implementar lazy audio init
4. Agregar preview al `index.html`
5. Incrementar badge en `_portal/index.html`
6. Commit: `feat(math-sound): add [nombre-simulación]`

## Estado del Repositorio

```
Branch: main (up to date with origin/main)
Último commit: 371a3f3
Cambios pendientes: Ninguno relacionado con Math Sound Lab
Archivos sin track: magmatismo-global*.html (Geology)
```

## Siguientes Pasos Recomendados

1. **Implementar Cadenas de Markov** - Composición probabilística
2. **Agregar guías educativas** (`Mathematics/Math Sound Lab/guides/`)
3. **Actualizar CLAUDE.md** con info de Math Sound Lab
4. **Implementar export WAV/MIDI** (opcional)
5. **Crear presets guardables** con localStorage

## Notas Importantes

- El laboratorio está **completo y funcional**
- Todas las simulaciones tienen audio implementado
- Código optimizado para móvil (touch gestures en Game of Life)
- Compatible con iOS/Safari (lazy audio init)
- Sin dependencias externas excepto Tailwind CSS y Three.js (CDN)

---

**Última actualización:** 2026-01-10
**Sesión completada con éxito:** 6 simulaciones, 4 commits, ~5,300 líneas de código
