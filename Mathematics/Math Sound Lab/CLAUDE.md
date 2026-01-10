# Math Sound Lab - Documentación Técnica

## Descripción

El **Math Sound Lab** es un laboratorio de sonificación matemática que convierte patrones, algoritmos y estructuras matemáticas en experiencias musicales interactivas. Utiliza Web Audio API para síntesis en tiempo real y Canvas 2D/Three.js para visualización sincronizada.

**Filosofía:** Cada simulación es un instrumento musical único donde los parámetros matemáticos controlan propiedades sonoras (pitch, duración, timbre, volumen, espacialización).

## Características del Laboratorio

- **8 simulaciones** diversas: autómatas, fractales, caos, teoría de números, composición generativa
- **~7,631 líneas de código** total
- **Archivos autocontenidos** (HTML/CSS/JS en un solo archivo)
- **Sin dependencias externas** (excepto Tailwind CSS y Three.js vía CDN)
- **Compatible iOS/Safari** (lazy audio initialization)
- **Responsive** y optimizado para móvil

## Stack Tecnológico

### Audio
- **Web Audio API** nativa (sin bibliotecas)
- **ADSR Envelopes** para modelado de notas
- **Filtros**: lowpass, highpass, bandpass
- **Síntesis FM** (Frequency Modulation)
- **Síntesis aditiva** (hasta 32 armónicos)
- **Espacialización estéreo** (PannerNode, StereoPannerNode)
- **Gestión de polifonía** (límite de 64 voces simultáneas)

### Visualización
- **Canvas 2D** para rendering principal
- **Three.js** para visualización 3D (Lorenz)
- **requestAnimationFrame** para animaciones 60 FPS
- **Device Pixel Ratio** handling para pantallas Retina

### Matemática
- **Integración numérica**: Runge-Kutta 4 (RK4)
- **Teoría de números**: Criba de Eratóstenes, Espiral de Ulam
- **Geometría fractal**: Mandelbrot, Julia, L-Systems
- **Sistemas dinámicos**: Lorenz, autómatas celulares
- **Probabilidad**: Cadenas de Markov, matrices estocásticas
- **Análisis espectral**: FFT conceptual, síntesis de Fourier

## Arquitectura de Audio

### Patrón: Lazy Initialization

Todas las simulaciones implementan inicialización perezosa del `AudioContext` para compatibilidad con iOS/Safari (requiere interacción del usuario):

```javascript
let audioContext = null;
let masterGain = null;
let isAudioInitialized = false;

function initAudioContext() {
    if (isAudioInitialized) return;
    audioContext = new (window.AudioContext || window.webkitAudioContext)();
    masterGain = audioContext.createGain();
    masterGain.connect(audioContext.destination);
    masterGain.gain.value = CONFIG.volume;
    isAudioInitialized = true;
}

// Llamar antes de reproducir primer sonido
btnPlay.addEventListener('click', () => {
    initAudioContext();  // Solo se ejecuta una vez
    // ... reproducir audio
});
```

### Patrón: ADSR Envelope

Envolvente Attack-Decay-Sustain-Release para modelado realista de notas:

```javascript
function playNote(frequency, duration, volume) {
    initAudioContext();

    const now = audioContext.currentTime;

    // Oscillator
    const osc = audioContext.createOscillator();
    osc.type = CONFIG.waveform;  // 'sine', 'triangle', 'square', 'sawtooth'
    osc.frequency.value = frequency;

    // ADSR Envelope
    const envelope = audioContext.createGain();
    const attack = 0.01;   // 10ms ataque rápido
    const decay = 0.1;     // 100ms decay
    const sustain = volume * 0.7;  // 70% del volumen peak
    const release = 0.2;   // 200ms release

    envelope.gain.setValueAtTime(0, now);
    envelope.gain.linearRampToValueAtTime(volume, now + attack);
    envelope.gain.linearRampToValueAtTime(sustain, now + attack + decay);
    envelope.gain.setValueAtTime(sustain, now + duration - release);
    envelope.gain.exponentialRampToValueAtTime(0.001, now + duration);

    // Filter (opcional)
    const filter = audioContext.createBiquadFilter();
    filter.type = 'lowpass';
    filter.frequency.value = frequency * 3;
    filter.Q.value = 1;

    // Routing
    osc.connect(filter);
    filter.connect(envelope);
    envelope.connect(masterGain);

    // Playback
    osc.start(now);
    osc.stop(now + duration + 0.1);  // +0.1 para margen de release
}
```

### Patrón: Gestión de Polifonía

Para autómatas celulares y generadores masivos de notas, limitar voces activas:

```javascript
const activeVoices = new Map();  // key: 'row-col', value: {osc, envelope, startTime}
const MAX_VOICES = 64;

function playNotePolyphonic(id, frequency, duration) {
    // Cleanup de voces expiradas
    const now = audioContext.currentTime;
    for (const [voiceId, voice] of activeVoices) {
        if (now > voice.startTime + voice.duration) {
            activeVoices.delete(voiceId);
        }
    }

    // Limitar polifonía
    if (activeVoices.size >= MAX_VOICES) {
        return;  // Rechazar nueva nota
    }

    // Crear y registrar nueva voz
    const voice = createVoice(frequency, duration);
    activeVoices.set(id, { ...voice, startTime: now, duration });
}
```

### Patrón: Síntesis FM (Frequency Modulation)

Para timbres complejos (usado en Fractales Sonoros):

```javascript
function playNoteFM(carrier, modulator, modulationIndex, duration) {
    initAudioContext();

    const now = audioContext.currentTime;

    // Carrier oscillator (frecuencia audible)
    const carrierOsc = audioContext.createOscillator();
    carrierOsc.frequency.value = carrier;

    // Modulator oscillator (modula la frecuencia del carrier)
    const modulatorOsc = audioContext.createOscillator();
    modulatorOsc.frequency.value = modulator;

    // FM Amount (profundidad de modulación)
    const modulatorGain = audioContext.createGain();
    modulatorGain.gain.value = modulationIndex;

    // Routing FM
    modulatorOsc.connect(modulatorGain);
    modulatorGain.connect(carrierOsc.frequency);  // FM!

    // Envelope
    const envelope = audioContext.createGain();
    envelope.gain.setValueAtTime(0, now);
    envelope.gain.linearRampToValueAtTime(0.3, now + 0.01);
    envelope.gain.exponentialRampToValueAtTime(0.001, now + duration);

    carrierOsc.connect(envelope);
    envelope.connect(masterGain);

    carrierOsc.start(now);
    modulatorOsc.start(now);
    carrierOsc.stop(now + duration);
    modulatorOsc.stop(now + duration);
}
```

### Patrón: Síntesis Aditiva

Sumar múltiples sinusoides (armónicos) para crear timbres complejos:

```javascript
function playSynthesizedSound(fundamental, harmonics, duration) {
    initAudioContext();

    const now = audioContext.currentTime;
    const envelope = audioContext.createGain();

    // Crear múltiples osciladores (uno por armónico)
    harmonics.forEach((amplitude, index) => {
        if (amplitude === 0) return;

        const harmonic = index + 1;  // 1st, 2nd, 3rd...
        const osc = audioContext.createOscillator();
        osc.frequency.value = fundamental * harmonic;

        const gain = audioContext.createGain();
        gain.gain.value = amplitude;

        osc.connect(gain);
        gain.connect(envelope);

        osc.start(now);
        osc.stop(now + duration);
    });

    // ADSR para el conjunto
    envelope.gain.setValueAtTime(0, now);
    envelope.gain.linearRampToValueAtTime(0.3, now + 0.01);
    envelope.gain.exponentialRampToValueAtTime(0.001, now + duration);

    envelope.connect(masterGain);
}
```

## Escalas Musicales Implementadas

### Pentatónicas (5 notas)

```javascript
const scales = {
    'pentatonic-minor': [0, 3, 5, 7, 10],  // C, Eb, F, G, Bb
    'pentatonic-major': [0, 2, 4, 7, 9],   // C, D, E, G, A
};

function midiToPentatonic(baseNote, scaleNote, octaveOffset = 0) {
    const scale = scales['pentatonic-minor'];
    return baseNote + scale[scaleNote % scale.length] + octaveOffset * 12;
}
```

### Modos Griegos (7 notas)

```javascript
const modes = {
    'ionian':     [0, 2, 4, 5, 7, 9, 11],  // Mayor natural
    'dorian':     [0, 2, 3, 5, 7, 9, 10],  // Menor con 6ª mayor
    'phrygian':   [0, 1, 3, 5, 7, 8, 10],  // Menor con 2ª menor
    'lydian':     [0, 2, 4, 6, 7, 9, 11],  // Mayor con 4ª aumentada
    'mixolydian': [0, 2, 4, 5, 7, 9, 10],  // Mayor con 7ª menor
    'aeolian':    [0, 2, 3, 5, 7, 8, 10],  // Menor natural
    'locrian':    [0, 1, 3, 5, 6, 8, 10],  // Disminuido
};
```

### Cromática (12 notas)

```javascript
function midiNoteToFrequency(midiNote) {
    return 440 * Math.pow(2, (midiNote - 69) / 12);
}
```

## Simulaciones Implementadas

### 1. Game of Life Musical (`game-of-life-musical.html`)

**Concepto:** Autómata celular de Conway como instrumento pentatónico

**Mapeo sonoro:**
- **Filas (Y)** → Notas de escala (0-4, cíclico)
- **Columnas (X)** → Octavas (C3-C7)
- **Edad de célula** → Duración de nota (configurable)
- **Vecinos vivos** → Volumen (configurable)

**Audio:**
- ADSR envelopes con filtro lowpass
- Escalas: pentatónica menor/mayor
- 4 formas de onda seleccionables
- Gestión de polifonía con Map() para O(1) lookup

**Visualización:**
- Canvas 2D con efecto glow en células que suenan
- Touch gestures para móvil (toggle células)
- FPS counter

**Características únicas:**
- Patrón observable con clicks de ratón
- Presets: Glider, Blinker, Pulsar, Random
- Modulation sliders para edad y vecinos

**Líneas de código:** ~1,136

---

### 2. Autómatas Celulares 1D Musical (`automatas-celulares-1d-musical.html`)

**Concepto:** Reglas de Wolfram (30, 90, 110, 184) generan patrones musicales

**Mapeo sonoro:**
- **Posición horizontal** → Nota en escala pentatónica
- **Generación (tiempo)** → Secuencia temporal
- **Células activas** → Tocan nota al nacer

**Audio:**
- Reproducción secuencial (generación por generación)
- BPM ajustable (30-300)
- Síntesis con ADSR

**Visualización:**
- Scroll vertical (nuevas generaciones arriba)
- Colores por regla
- Highlight de generación actual durante playback

**Características únicas:**
- 256 reglas de Wolfram (input 0-255)
- Class I/II/III/IV classification
- Exportable como imagen

**Líneas de código:** 774

---

### 3. Transformada de Fourier Interactiva (`fourier-interactiva.html`)

**Concepto:** Síntesis aditiva en tiempo real con 32 armónicos

**Mapeo sonoro:**
- **Armónicos (1-32)** → Frecuencias múltiplos de fundamental
- **Amplitudes** → Sliders individuales para cada armónico
- **Forma de onda resultante** → Visualización tiempo real

**Audio:**
- Síntesis aditiva pura (hasta 32 osciladores simultáneos)
- Fundamental ajustable (50-800 Hz)
- Waveform presets: sine, square, sawtooth, triangle

**Visualización:**
- Triple canvas: waveform + spectrum + combined
- Espectro de frecuencias con barras
- Forma de onda sintética resultante

**Características únicas:**
- Preset waveforms que ajustan armónicos automáticamente
- Análisis espectral visual
- Educativo para entender FFT/síntesis

**Líneas de código:** 759

---

### 4. Números Primos como Ritmos (`numeros-primos-ritmos.html`)

**Concepto:** Teoría de números sonificada con Criba de Eratóstenes y Espiral de Ulam

**Mapeo sonoro:**
- **Gaps entre primos** → Duración de notas
- **Posición en espiral** → Pitch (mapping circular)
- **Primos gemelos** → Acorde armónico
- **Primos de Sophie Germain** → Nota especial

**Audio:**
- Playback secuencial de primos hasta N
- Cymbal hit en primos especiales
- Escalas pentatónicas

**Visualización:**
- Espiral de Ulam con primos destacados
- Animación durante playback
- Estadísticas: π(n), gaps promedio

**Características únicas:**
- Criba de Eratóstenes optimizada
- Detección de primos gemelos y Sophie Germain
- Visualización tipo Ulam spiral

**Líneas de código:** 798

---

### 5. Atractor de Lorenz Musical (`atractor-lorenz-musical.html`)

**Concepto:** Sistema caótico 3D con trayectorias convertidas en música

**Mapeo sonoro:**
- **X** → Pitch (mapeo continuo)
- **Y** → Volumen
- **Z** → Pan estéreo (izquierda-derecha)
- **Velocidad** → Rate de notas

**Audio:**
- Síntesis continua con actualización paramétrica
- Espacialización estéreo 3D
- ADSR adaptativos

**Visualización:**
- Three.js para rendering 3D
- Trail de partículas (historial de trayectoria)
- Cámara orbital interactiva

**Características únicas:**
- Integración RK4 para precisión numérica
- Parámetros σ, ρ, β ajustables
- Condiciones iniciales configurables
- Mariposa del caos audible

**Líneas de código:** 733

---

### 6. Fractales Sonoros (`fractales-sonoros.html`)

**Concepto:** Mandelbrot y Julia sets con síntesis FM

**Mapeo sonoro:**
- **Iteraciones hasta divergencia** → FM modulation index
- **Coordenadas (x, y)** → Carrier y modulator frequencies
- **Click en fractal** → Toca nota

**Audio:**
- Síntesis FM (Frequency Modulation)
- Carrier = 200-800 Hz (basado en Y)
- Modulator = 50-400 Hz (basado en X)
- Modulation index = iteraciones / maxIterations

**Visualización:**
- Canvas 2D con zoom infinito interactivo
- 5 paletas de colores (fire, ocean, forest, psychedelic, monochrome)
- Fractal type: Mandelbrot vs Julia

**Características únicas:**
- Zoom infinito con scroll
- Julia sets con parámetro C ajustable
- Max iterations configurable (10-200)

**Líneas de código:** 701

---

### 7. Cadenas de Markov Generativas (`cadenas-markov-generativas.html`)

**Concepto:** Composición algorítmica probabilística con training

**Mapeo sonoro:**
- **Estados bidimensionales** → (pitch, duration) atomic states
- **Matriz de transición NxN** → Probabilidades de cambio de estado
- **Generación** → Secuencias musicales aleatorias basadas en matriz

**Audio:**
- Síntesis con ADSR
- Estados configurables (2-12)
- Escalas pentatónicas
- Training con 4 melodías preset

**Visualización:**
- Dual visualización: Heatmap + Graph (tabs)
- Heatmap: HSL colors mapping probability → hue
- Graph: Nodos circulares con flechas de transición

**Características únicas:**
- Training desde presets (Twinkle Star, Ode to Joy, etc.)
- Recording con teclado (A-L keys)
- Edición manual de matriz
- Cálculo de entropía de Shannon
- Detección de estados absorbentes

**Líneas de código:** ~1,630

---

### 8. L-Systems Musicales (`l-systems-musicales.html`)

**Concepto:** Gramáticas de Lindenmayer que generan fractales vegetales sonoros

**Mapeo sonoro:**
- **F/G (avanzar)** → Tocar nota (pitch basado en profundidad de stack)
- **+ (girar derecha)** → Aumentar pitch
- **- (girar izquierda)** → Disminuir pitch
- **[ (push stack)** → Subir registro (octava)
- **] (pop stack)** → Bajar registro

**Audio:**
- ADSR con filtros lowpass
- Escalas pentatónicas
- BPM ajustable (30-240)
- Timing sincronizado con crecimiento visual

**Visualización:**
- Stack-based rendering con auto-scaling
- Colores por profundidad (verde → naranja)
- Grosor decreciente (taper)
- Animación progresiva sincronizada con audio

**Características únicas:**
- 6 presets clásicos (binary tree, fern, sierpinski, etc.)
- Editor de reglas personalizado
- Iteraciones configurables (1-7)
- Estadísticas en tiempo real (string length, segments, depth)

**Líneas de código:** ~1,100

---

## Convenciones de Código

### Nomenclatura

```javascript
// Variables: camelCase
let audioContext = null;
let masterGain = null;
const MAX_VOICES = 64;

// Funciones: camelCase
function initAudioContext() { }
function playNote(frequency, duration) { }
function generateAudioEvents() { }

// Constantes de configuración: UPPER_CASE
const CONFIG = {
    volume: 0.3,
    bpm: 120,
    waveform: 'sine'
};

// Clases: PascalCase
class LSystem { }
class MarkovChain { }
```

### Estructura de archivo HTML

```html
<!DOCTYPE html>
<html lang="es">
<head>
    <!-- Meta, title, Google Fonts -->
    <style>
        :root {
            --bg-primary: #030712;
            --bg-secondary: #0f172a;
            --accent: #f97316;  /* Orange para Math Labs */
        }
        /* Estilos... */
    </style>
</head>
<body>
    <header class="header">
        <!-- Breadcrumb -->
    </header>
    <main class="main">
        <div class="canvas-container">
            <canvas id="canvas"></canvas>
        </div>
        <aside class="controls">
            <!-- Math box con ecuación -->
            <!-- Sliders, buttons, valores -->
        </aside>
    </main>
    <script>
        // === CONFIG ===
        const CONFIG = { };

        // === CANVAS SETUP ===
        const canvas = document.getElementById('canvas');
        const ctx = canvas.getContext('2d');

        // === AUDIO ===
        let audioContext = null;
        function initAudioContext() { }
        function playNote() { }

        // === GENERATION/PHYSICS ===
        function generate() { }
        function update(dt) { }

        // === RENDERING ===
        function draw() { }

        // === PLAYBACK ===
        function playLoop(timestamp) { }

        // === CONTROLS ===
        // Event listeners...

        // === INIT ===
        init();
    </script>
</body>
</html>
```

### Canvas Setup con DPR

```javascript
const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
let dpr = window.devicePixelRatio || 1;

function resizeCanvas() {
    const rect = canvas.parentElement.getBoundingClientRect();
    canvas.width = rect.width * dpr;
    canvas.height = rect.height * dpr;
    canvas.style.width = rect.width + 'px';
    canvas.style.height = rect.height + 'px';
    ctx.scale(dpr, dpr);
}

window.addEventListener('resize', resizeCanvas);
resizeCanvas();
```

### Loop Principal con Delta Time

```javascript
let lastTime = 0;

function loop(time) {
    const dt = Math.min((time - lastTime) / 1000, 0.1);  // Max 100ms
    lastTime = time;

    update(dt);
    draw();

    requestAnimationFrame(loop);
}

requestAnimationFrame(loop);
```

## Referencias Cruzadas

### Para implementar audio básico:
Leer: `game-of-life-musical.html:328-650`

### Para síntesis FM:
Leer: `fractales-sonoros.html:339-420`

### Para síntesis aditiva:
Leer: `fourier-interactiva.html:467-485`

### Para playback secuencial:
Leer: `numeros-primos-ritmos.html:676-706` o `cadenas-markov-generativas.html:919-948`

### Para playback continuo 3D:
Leer: `atractor-lorenz-musical.html:289-337`

### Para dual visualización (tabs):
Leer: `cadenas-markov-generativas.html:450-550`

### Para Three.js setup:
Leer: `atractor-lorenz-musical.html:125-242`

### Para stack-based rendering:
Leer: `l-systems-musicales.html:590-650`

### Para integración RK4:
Leer: `atractor-lorenz-musical.html:355-410`

## Mejores Prácticas

### 1. Siempre usar lazy audio init

```javascript
// ✅ CORRECTO
btnPlay.addEventListener('click', () => {
    initAudioContext();  // Solo se inicializa con interacción
    playSound();
});

// ❌ INCORRECTO
const audioContext = new AudioContext();  // Bloqueado en iOS
```

### 2. Limitar polifonía

```javascript
// ✅ CORRECTO
const MAX_VOICES = 64;
if (activeVoices.size >= MAX_VOICES) return;

// ❌ INCORRECTO
// Crear voces sin límite → crash en navegador
```

### 3. Cleanup de osciladores

```javascript
// ✅ CORRECTO
osc.start(now);
osc.stop(now + duration + 0.1);  // Cleanup automático

// ❌ INCORRECTO
osc.start(now);
// Nunca llamar .stop() → memory leak
```

### 4. Exponential ramp to near-zero

```javascript
// ✅ CORRECTO
envelope.gain.exponentialRampToValueAtTime(0.001, now + duration);

// ❌ INCORRECTO
envelope.gain.exponentialRampToValueAtTime(0, now + duration);
// exponentialRamp no puede llegar a 0 exacto (log(0) = -∞)
```

### 5. DPR scaling para Canvas

```javascript
// ✅ CORRECTO
canvas.width = rect.width * dpr;
canvas.height = rect.height * dpr;
ctx.scale(dpr, dpr);

// ❌ INCORRECTO
canvas.width = rect.width;  // Borroso en Retina
```

### 6. Delta time clamping

```javascript
// ✅ CORRECTO
const dt = Math.min((time - lastTime) / 1000, 0.1);

// ❌ INCORRECTO
const dt = (time - lastTime) / 1000;
// Si tab pierde foco, dt puede ser enorme → inestabilidad numérica
```

## Futuras Simulaciones Sugeridas

### Alta Prioridad

1. **Ondas de Turing Sonoras**
   - Gray-Scott reaction-diffusion
   - Texturas sonoras emergentes
   - Inspirado en Physics Sound Lab

2. **Secuencia de Fibonacci Musical**
   - Proporción áurea como intervalos
   - Espiral de Fibonacci visual
   - Conexión con naturaleza

3. **Curvas de Lissajous Sonoras**
   - Ratios de frecuencia → armonía
   - Figuras paramétricas
   - x(t) = A·sin(aωt + δ), y(t) = B·sin(bωt)

### Media Prioridad

4. **Algoritmos de Ordenamiento Sonoros**
   - Bubble, Quick, Merge sort visualizados
   - Comparaciones → beeps
   - Educativo para complejidad algorítmica

5. **Péndulo de Foucault Musical**
   - Precesión → cambio tonal
   - Three.js 3D
   - Parámetro: latitud

6. **Caos de Collatz**
   - Conjetura 3n+1
   - Trayectorias → melodías
   - Convergencia a 1

## Recursos Externos

### Web Audio API
- MDN: https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API
- Spec: https://www.w3.org/TR/webaudio/

### Three.js
- Docs: https://threejs.org/docs/
- Examples: https://threejs.org/examples/

### Matemática
- Wolfram MathWorld: https://mathworld.wolfram.com/
- OEIS: https://oeis.org/ (secuencias numéricas)

---

**Última actualización:** 2026-01-10
**Versión del laboratorio:** 8 simulaciones, ~7,631 líneas de código
