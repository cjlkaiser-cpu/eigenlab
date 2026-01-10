# Prompt de Continuación: Math Sound Lab - EigenLab

## Contexto del Proyecto

Estás trabajando en **EigenLab**, un ecosistema de laboratorios virtuales educativos con simulaciones científicas y matemáticas interactivas en HTML/CSS/JavaScript vanilla.

**Repositorio**: https://github.com/cjlkaiser-cpu/eigenlab
**Directorio local**: `/Users/carlos/Projects/EigenLab/`

## Estado Actual: Math Sound Lab - COMPLETADO (8 simulaciones) ✅

El **Math Sound Lab** está completamente funcional con **8 simulaciones** de sonificación matemática. Es un laboratorio maduro que combina algoritmos, teoría de números, geometría fractal y composición generativa con síntesis de audio en tiempo real.

### Simulaciones Implementadas

| # | Archivo | Líneas | Concepto | Mapeo Sonoro | Estado |
|---|---------|--------|----------|--------------|--------|
| 1 | `game-of-life-musical.html` | ~1,136 | Autómata celular 2D, emergencia | Filas→Notas, Cols→Octavas | ✅ |
| 2 | `automatas-celulares-1d-musical.html` | 774 | Reglas de Wolfram (30, 90, 110, 184) | Generación→Tiempo, Posición→Nota | ✅ |
| 3 | `fourier-interactiva.html` | 759 | Síntesis aditiva, análisis espectral | 32 Armónicos→Timbre | ✅ |
| 4 | `numeros-primos-ritmos.html` | 798 | Teoría de números, Espiral de Ulam | Gaps→Duración, Posición→Pitch | ✅ |
| 5 | `atractor-lorenz-musical.html` | 733 | Sistemas caóticos, EDOs (Three.js) | X→Pitch, Y→Volumen, Z→Pan | ✅ |
| 6 | `fractales-sonoros.html` | 701 | Mandelbrot/Julia, geometría fractal | Iteraciones→FM Index | ✅ |
| 7 | `cadenas-markov-generativas.html` | ~1,630 | Composición algorítmica probabilística | Matriz de transición→Secuencias | ✅ |
| 8 | `l-systems-musicales.html` | ~1,100 | Gramáticas de Lindenmayer, fractales | Símbolos→Notas, Profundidad→Octavas | ✅ |

**Total:** ~7,631 líneas de código

### Tecnologías Implementadas

**Audio Engine (Web Audio API):**
- ✅ Lazy initialization (iOS/Safari compatible)
- ✅ ADSR envelopes con filtros lowpass/highpass
- ✅ Síntesis FM (Frequency Modulation)
- ✅ Síntesis aditiva (hasta 32 armónicos)
- ✅ Gestión de polifonía (64 voces simultáneas)
- ✅ Pan estéreo y espacialización 3D
- ✅ 6+ escalas musicales (pentatónicas, modos griegos, cromática)
- ✅ 4 formas de onda (sine, triangle, square, sawtooth)

**Visualización:**
- ✅ Canvas 2D optimizado (7 simulaciones)
- ✅ Three.js 3D (Atractor de Lorenz)
- ✅ Efectos glow sincronizados con audio
- ✅ Animaciones 60 FPS
- ✅ Dual visualización (heatmap + graph en Markov)
- ✅ 5 paletas de colores (fractales)
- ✅ Zoom infinito interactivo

**Matemática y Algoritmos:**
- ✅ Integración RK4 (Lorenz, autómatas)
- ✅ Criba de Eratóstenes (primos)
- ✅ Reglas de Wolfram 0-255
- ✅ FFT conceptual (Fourier)
- ✅ Espiral de Ulam
- ✅ Iteración compleja z² + c (Mandelbrot/Julia)
- ✅ Cadenas de Markov con training
- ✅ L-Systems con stack-based rendering

### Últimos Commits (Sesión Actual)

```
22253d6 feat(math-sound): add L-Systems Musicales simulation
00ff40b feat(math-sound): add Cadenas de Markov Generativas
371a3f3 feat(math-sound): añadir Fractales Sonoros con FM Synthesis
0b1a51c feat(math-sound): añadir Atractor de Lorenz Musical
21f8582 feat(math-sound): añadir Números Primos como Ritmos
05cc10b feat(math-sound): añadir Autómatas Celulares 1D y Fourier Interactiva
```

**Estado del repositorio:** Todo pusheado, branch main actualizado

### Archivos Clave

```
Mathematics/Math Sound Lab/
├── index.html                              # Grid de 8 simulaciones
├── CLAUDE.md                               # Documentación técnica del laboratorio
├── game-of-life-musical.html               # ✅ Autómata 2D
├── automatas-celulares-1d-musical.html     # ✅ Reglas de Wolfram
├── fourier-interactiva.html                # ✅ Síntesis aditiva
├── numeros-primos-ritmos.html              # ✅ Teoría de números
├── atractor-lorenz-musical.html            # ✅ Caos 3D
├── fractales-sonoros.html                  # ✅ Mandelbrot/Julia FM
├── cadenas-markov-generativas.html         # ✅ Composición probabilística
└── l-systems-musicales.html                # ✅ Gramáticas generativas

Portal actualizado:
_portal/index.html → Badge: **8 sims**
```

## Próximas Simulaciones Sugeridas

Para continuar expandiendo el Math Sound Lab:

### Prioridad Alta

1. **Ondas de Turing Sonoras**
   - Reacción-difusión (Gray-Scott)
   - Patrones emergentes → texturas sonoras
   - Parámetros: F, k (feed, kill)
   - Inspirado en Physics Sound Lab

2. **Secuencia de Fibonacci Musical**
   - Proporción áurea en ritmo
   - Espiral de Fibonacci visual
   - Ratios áureos → intervalos musicales
   - Conexión con naturaleza

3. **Curvas de Lissajous Sonoras**
   - x(t) = A·sin(aωt + δ)
   - y(t) = B·sin(bωt)
   - Ratios de frecuencia → armonía visual
   - Figuras de Lissajous clásicas

### Prioridad Media

4. **Péndulo de Foucault Musical**
   - Física + geometría
   - Precesión → cambio tonal
   - Visualización 3D con Three.js
   - Latitud → parámetros sonoros

5. **Algoritmos de Ordenamiento Sonoros**
   - Bubble, Quick, Merge sort
   - Comparaciones → sonido
   - Visualización de barras animadas
   - Complejidad O(n) audible

6. **Caos de Collatz Musical**
   - Conjetura 3n+1
   - Secuencias impredecibles → melodías
   - Visualización de trayectorias
   - Convergencia a 1

## Convenciones del Proyecto

- **Idioma UI:** Español
- **Idioma código:** Inglés (variables, funciones, comentarios)
- **Archivos:** kebab-case (`fourier-interactiva.html`)
- **Color Math Labs:** `#f97316` (orange)
- **Estructura:** HTML autocontenido, sin bundler
- **Audio:** Web Audio API nativa, lazy init

## Patrón de Audio (Referencia)

```javascript
// Lazy initialization (iOS/Safari compatible)
function initAudioContext() {
    if (isAudioInitialized) return;
    audioContext = new (window.AudioContext || window.webkitAudioContext)();
    masterGain = audioContext.createGain();
    masterGain.connect(audioContext.destination);
    isAudioInitialized = true;
}

// ADSR Envelope pattern
const envelope = audioContext.createGain();
envelope.gain.setValueAtTime(0, now);
envelope.gain.linearRampToValueAtTime(volume, now + attack);
envelope.gain.linearRampToValueAtTime(sustain, now + attack + decay);
envelope.gain.exponentialRampToValueAtTime(0.001, now + duration);

// Filtro lowpass
const filter = audioContext.createBiquadFilter();
filter.type = 'lowpass';
filter.frequency.value = frequency * 3;
filter.Q.value = 1;
```

## Archivos de Referencia

**Audio patterns:**
- `game-of-life-musical.html:328-650` - ADSR, gestión de voces
- `fourier-interactiva.html:467-485` - Síntesis aditiva
- `fractales-sonoros.html:339-420` - FM Synthesis
- `atractor-lorenz-musical.html:289-337` - Mapeo continuo 3D
- `cadenas-markov-generativas.html:919-948` - Playback secuencial
- `l-systems-musicales.html:590-650` - Stack-based audio generation

**Visualización:**
- `fourier-interactiva.html:520-583` - Triple canvas sync
- `fractales-sonoros.html:162-243` - Rendering optimizado
- `atractor-lorenz-musical.html:125-242` - Three.js setup
- `cadenas-markov-generativas.html:450-550` - Heatmap + graph dual viz
- `l-systems-musicales.html:710-780` - Progressive animation

## Cómo Agregar Nueva Simulación

1. Crear archivo: `Mathematics/Math Sound Lab/nueva-sim.html`
2. Usar color `--accent: #f97316`
3. Implementar lazy audio init (ver patrón arriba)
4. Agregar preview al `index.html`
5. Actualizar badge en `_portal/index.html`
6. Commit: `feat(math-sound): add [nombre-simulación]`

## Estado del Repositorio

```
Branch: main (up to date with origin/main)
Último commit: 22253d6 (L-Systems Musicales)
Cambios pendientes: Ninguno relacionado con Math Sound Lab
```

## Notas Importantes

- El laboratorio está **completo y funcional** con 8 simulaciones diversas
- Todas las simulaciones tienen audio implementado correctamente
- Código optimizado para móvil (touch gestures en Game of Life)
- Compatible con iOS/Safari (lazy audio init en todas)
- Sin dependencias externas excepto Tailwind CSS y Three.js (CDN)
- Cada simulación es autocontenida (HTML/CSS/JS en un solo archivo)
- Documentación técnica completa en `CLAUDE.md`

---

**Última actualización:** 2026-01-10
**Sesión completada con éxito:** 8 simulaciones totales, ~7,631 líneas de código

## Instrucciones para Nueva Sesión

Cuando inicies una nueva sesión de Claude Code, lee primero `CLAUDE.md` del Math Sound Lab para contexto técnico completo, luego este archivo para el estado actual.

**Inicio de sesión sugerido:**
```
Hola, quiero continuar trabajando en Math Sound Lab de EigenLab.
Lee /Users/carlos/Projects/EigenLab/Mathematics/Math Sound Lab/CLAUDE.md
y /Users/carlos/Projects/EigenLab/Mathematics/Math Sound Lab/CONTINUE.md

¿En qué simulación trabajamos hoy?
```
