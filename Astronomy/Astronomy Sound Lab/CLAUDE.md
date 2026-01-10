# Astronomy Sound Lab - Documentación Técnica

## Descripción

El **Astronomy Sound Lab** contiene **3 simulaciones** de sonificación astronómica que convierten fenómenos astrofísicos en experiencias musicales: pulsares como metronomos, ondas gravitacionales como chirps, y órbitas planetarias como armonías.

## Simulaciones (3 Total)

1. **Pulsares** - Faros cósmicos como ritmo
2. **Ondas Gravitacionales** - El chirp cósmico de LIGO
3. **Música de las Esferas** - Exoplanetas cantando (Kepler 1619)

## Modelos de Sonificación

### Pulsares: Faros Cósmicos como Ritmo

**Física:**
- Objeto: Estrella de neutrones en rotación rápida
- Sonificación: Período orbital → BPM / Frecuencia de audio

**Ecuaciones:**
```
BPM = 60,000 / P_ms
angularVelocity = (2π) / (P_ms / 1000) rad/s
frequency_audio = 1000 / P_ms (Hz)
```

**4 Pulsares implementados:**
1. **Crab** (33 ms) → ~1818 BPM
2. **Vela** (89 ms) → ~674 BPM
3. **PSR B1919+21** (1337 ms) → 44.8 BPM [Primer pulsar descubierto]
4. **PSR J1748-2446ad** (1.4 ms → 50 ms escalado) → ~1200 BPM [Más rápido conocido]

**Algoritmo de pulso:**
```javascript
duration = (period * pulseWidth / 100) / 1000  // segundos
gain.gain.linearRampToValueAtTime(volume, now + 0.005)       // Attack
gain.gain.linearRampToValueAtTime(volume * 0.8, now + duration * 0.5) // Decay
gain.gain.linearRampToValueAtTime(0, now + duration)         // Release
```

**Visualización especial:**
- Campos magnéticos curvos (8 líneas)
- Dos haces de radiación opuestos
- **Historial Joy Division:** Canvas deslizante (100 puntos, imitando Unknown Pleasures)

### Ondas Gravitacionales: El Chirp Cósmico

**Física:**
- Objeto: Fusión de dos masas (agujeros negros o estrellas de neutrones)
- Modelo: Inspiración → Plunge → Ringdown

**Ecuaciones:**
```
Chirp Mass: M_c = (m1 * m2)^0.6 / (m1 + m2)^0.2

GW Frequency (Inspiral):
f_GW = (1/π) * (GM_c/c³)^(-5/8) * (t_c - t)^(-3/8)

Strain (Amplitud):
h ∝ (M_c/r) * (π*f*M_c)^(2/3)

Simplified:
freq(t) = baseFreq * (1 / timeToMerger)^0.375
amplitude(t) = √(1 - separation) * (1 + (1-separation)²)
```

**3 Eventos LIGO:**
1. **GW150914** (36 M☉ + 29 M☉) - Primera detección
2. **GW170817** (1.5 M☉ + 1.3 M☉) - Estrellas de neutrones
3. **GW190521** (85 M☉ + 66 M☉) - Más masivo

**Algoritmo de fusión:**
```javascript
separation = Math.pow(timeToMerger / mergerTime, 0.25)
orbitalPhase += (1 / separation^1.5) * dt * 10

gwFreq = baseFreq * Math.pow(1/timeToMerger, 0.375)
oscillator.frequency.setValueAtTime(gwFreq, audioCtx.currentTime)

// Ringdown (post-merger)
if (isMerged) {
    strain = exp(-ringdownTime * 5) * sin(phase * 20)
}
```

**Visualización:**
- **Deformación del espacio-tiempo:** Grid deformado por curvatura
- **Ondas propagándose:** Anillos concéntricos expandiéndose
- **Waveform canvas:** Historial de strain h(t) como LIGO
- **Spectrogram:** Diagrama tiempo-frecuencia (chirp ascendente)

### Música de las Esferas: Exoplanetas Cantando

**Fundamento histórico:** Kepler, "Harmonices Mundi" (1619)

**Mapeo:** Órbita → Nota Musical

**Tercera Ley de Kepler:**
```
T² = a³  (período² proporcional distancia³)

Período Orbital → Frecuencia de nota
f_n = f_0 * 2^(n/12)  (semitones desde raíz)

MIDI frequency:
f = 440 * 2^((midi - 69)/12)
```

**3 Sistemas implementados:**

1. **TRAPPIST-1** (7 planetas en resonancia):
   - b: 1.51d → nota base
   - c: 2.42d → +2 semitones
   - e: 6.10d → +5 semitones (resonancia perfecta)
   - h: 18.77d → +11 semitones

2. **Sistema Solar** (8 planetas, escala logarítmica):
   - Mercurio (88d) → Sol agudo
   - Neptuno (60190d) → Sol grave

3. **Kepler-90** (8 planetas, sistema compacto)

**6 Escalas musicales:**
- Major, Minor, Pentatonic, Lydian (favorita de Kepler), Chromatic, Harmonic

**Algoritmo de trigger:**
```javascript
triggerZone = ±0.1 rad  // Tope de la órbita (-π/2)
isInTriggerZone = |planet.angle - (-π/2)| < 0.1

if (isInTriggerZone && !wasInTriggerZone) {
    playNote(i)  // Nota de 500ms / speed
    wasInTriggerZone = true
}
```

**Visualización:**
- **Piano Roll:** Historial de notas (últimas 5 segundos, fade-out)
- **Órbitas con colores:** Cada planeta distintivo
- **Trigger points:** Círculos de activación
- **Acorde actual:** Display en tiempo real

## Web Audio API Implementation

### Lazy Initialization (iOS/Safari compatible)

```javascript
let audioCtx = null

function initAudio() {
    if (audioCtx) return
    audioCtx = new (window.AudioContext || window.webkitAudioContext)()
    // Inicializar gain nodes
}

document.getElementById('start-btn').addEventListener('click', () => {
    initAudio()  // Solo después de user gesture
    isPlaying = !isPlaying
})
```

### ADSR Envelopes

**Pulsares:**
```javascript
gain.gain.setValueAtTime(0, now)
gain.gain.linearRampToValueAtTime(volume, now + 0.005)        // Attack
gain.gain.linearRampToValueAtTime(volume * 0.8, now + duration * 0.5) // Decay
gain.gain.linearRampToValueAtTime(0, now + duration)           // Release
```

**Música Esferas:**
```javascript
gain.gain.setValueAtTime(0, audioCtx.currentTime)
gain.gain.linearRampToValueAtTime(volume * 0.3, audioCtx.currentTime + 0.05)
// Fade-out automático con setTimeout
```

### Síntesis

| Lab | Tipo | Detalles |
|-----|------|----------|
| Pulsares | Oscilador + White Noise | sine/square/sawtooth/triangle/noise bandpass |
| Ondas GW | Oscilador simple | freq modulada dinámicamente |
| Música Esferas | Oscilador + Detune | Detune basado en planetIndex |

### Gestión de Polifonía

**Música Esferas:**
```javascript
const activeOscillators = new Map()

// Stop existing, create new
if (activeOscillators.has(planetIndex)) {
    old.gain.gain.setTargetAtTime(0, audioCtx.currentTime, 0.1)
    setTimeout(() => old.osc.stop(), 200)
}

const osc = audioCtx.createOscillator()
const gain = audioCtx.createGain()
activeOscillators.set(planetIndex, { osc, gain })
```

## Tecnología

### Canvas 2D (Rendering)

| Simulación | Técnicas |
|------------|----------|
| Pulsares | Gradientes + rotación, haces luminosos, historial Joy Division |
| Ondas GW | Grid deformado + rings, deformación spacetime, ringdown pulsante |
| Música Esferas | Órbitas + glow, planetas con colores, trigger zones |

### Web Audio API
- Osciladores (sine, square, sawtooth, triangle)
- Gain nodes (ADSR envelopes)
- Frequency modulation (ondas GW)
- Polyphony management (hasta 8 voces)

## Ecuaciones Específicas

### Pulsares

**Rotación:**
```
θ(t) = ω·t = (2π/P)·t
```

**Mapeo período → BPM:**
```
BPM = 60,000 / P_ms
```

### Ondas Gravitacionales

**Inspiración → Merger:**
```
separation(t) = (1 - t/mergerTime)^0.25
freq = baseFreq · (1/timeToMerger)^0.375
```

**Chirp mass:**
```
M_c = (m1·m2)^0.6 / (m1+m2)^0.2
```

### Música Esferas

**MIDI frequency:**
```
f = 440 · 2^((midi-69)/12)
midi = baseNote + scaleOffset
```

**Angular velocity:**
```
ω = 2π / (period/minPeriod * 5)
```

## Referencias

**Total código:** 2,396 líneas (3 simulaciones autocontenidas)

**Comparación con Math Sound Lab:**
- Math Sound: 8 sims, síntesis FM/Aditiva avanzada, estructuras matemáticas
- Astronomy Sound: 3 sims, síntesis simple, fenómenos astrofísicos reales

---

**Última actualización:** 2026-01-10
