# Music Theory Lab - Documentación Técnica para Claude Code

## Descripción

**Music Theory Lab** es un laboratorio de teoría musical y composición algorítmica basado en el **Sistema Armónico Áureo** - un sistema armónico completo que utiliza la proporción áurea **φ (1.618...)** como operador estructural fundamental.

A diferencia de enfoques tradicionales que imitan el temperamento igual, este sistema redefine escalas, acordes, consonancia y función armónica desde primeros principios matemáticos.

**Ubicación:** `Music/Music Theory Lab/`

**Repositorio:** https://github.com/cjlkaiser-cpu/eigenlab

**Color del lab:** `#ec4899` (Rosa/Magenta)

## Estructura del Proyecto

```
Music/Music Theory Lab/
├── index.html                          # Hub del laboratorio (~550 líneas)
├── GOLDEN_HARMONY_THEORY.md            # Teoría completa (~1800 líneas)
├── golden-harmony-engine.js            # Motor matemático (~830 líneas)
├── escala-cromatica-aurea.html         # Sim 1: Explorador cromático (~850 líneas)
├── armonizador-aureo.html              # Sim 2: Constructor armónico (~1100 líneas)
├── compositor-aureo.html               # Sim 3: Generador SATB (~900 líneas)
├── escala-15-notas.html                # Sim 4: Escala microtonal (~550 líneas)
├── armonizador-15-notas.html           # Sim 5: Armonizador 15 notas (~600 líneas)
├── compositor-15-notas.html            # Sim 6: Compositor SATB 15 notas (~550 líneas)
├── escala-12-phiW.html                 # Sim 7: Escala 12-φW occidental (~650 líneas)
├── armonizador-12-phiW.html            # Sim 8: Armonizador 12-φW (~620 líneas)
├── compositor-12-phiW.html             # Sim 9: Compositor SATB 12-φW (~600 líneas)
└── CLAUDE.md                           # Este archivo

Total: ~10,200 líneas de código + documentación
```

## Marco Teórico Fundamental

### Principio Rector

**φ como operador estructural, no decorativo**

El Sistema Armónico Áureo NO es:
- Una aproximación al temperamento igual
- Una "micro-afinación alternativa"
- Un sistema de ratios just intonation basado en φ

El Sistema Armónico Áureo ES:
- Una redefinición completa de armonía desde primeros principios
- φ como generador matemático de toda la estructura
- Consonancia definida por proximidad a potencias de φ
- Función armónica como gradiente de φ (no tensión-resolución tradicional)

### Ecuaciones Clave

**Escala Cromática (12 notas) - División por φ:**
```
n_i = (1200 · φ^(-i)) mod 1200,  i ∈ [0, 11]
```

**Escala Cromática (15 notas) - Quintas Apiladas:**
```
cents_i = (i × 833.09) mod 1200,  i ∈ [0, 14]

donde 833.09 = 1200 × log₂(φ) = quinta áurea
```

**Escala 12-φW (12 notas "occidentales" - Quintas Apiladas):**
```
cents_i = (i × 833.09) mod 1200,  i ∈ [0, 11]

Semitono promedio: ~99 cents (casi idéntico a 12-TET: 100 cents)
Quinta: 833 cents (vs 700 cents en 12-TET)
```

**Consonancia:**
```
C(I) = e^(-(d²/σ²))

donde:
  d = distancia mínima de I a φ^k (k ∈ ℤ)
  σ = tolerance / 2  (típicamente 25 cents)
```

**Optimización de Escalas Diatónicas:**
```
Score(S₇) = Σ_{i<j} C(|n_i - n_j|) - Penalty(S₇)

Penalty = 10 × (número de microintervalos < 80 cents)
```

**Tríada Áurea:**
```
Tríada = [φ⁰, φ², φ³] (envueltos en octava)
```

### Valores Exactos de la Escala Cromática

| Índice φ | Cents | Ratio φ^(-i) | Frecuencia (A=440 Hz) |
|----------|-------|--------------|----------------------|
| φ₀ | 0.00 | 1.000000 | 440.00 |
| φ₁ | 741.64 | 0.618034 | 734.99 |
| φ₂ | 155.75 | 0.381966 | 454.21 |
| φ₃ | 897.39 | 0.236068 | 564.47 |
| φ₄ | 311.50 | 0.145898 | 349.08 |
| φ₅ | 1053.14 | 0.090170 | 430.92 |
| φ₆ | 467.25 | 0.055728 | 266.45 |
| φ₇ | 7.91 | 0.034442 | 442.14 |
| φ₈ | 623.36 | 0.021286 | 273.37 |
| φ₉ | 37.47 | 0.013156 | 446.03 |
| φ₁₀ | 779.11 | 0.008131 | 275.75 |
| φ₁₁ | 193.22 | 0.005025 | 457.08 |

**Intervalo de Quinta Áurea:** φ¹ ≈ 833.09 cents (NO 700 cents como 12-TET)

### Valores de la Escala de 15 Notas (Quintas Apiladas)

| Stack # | Cents | Gap al siguiente |
|---------|-------|------------------|
| Φ0 | 0.00 | 33.09 |
| Φ7 | 33.09 | 66.18 |
| Φ14 | 99.27 | 99.27 |
| Φ6 | 198.54 | 99.27 |
| Φ13 | 297.81 | 99.27 |
| Φ5 | 397.09 | 69.27 |
| Φ12 | 466.36 | 99.27 |
| Φ4 | 565.64 | 99.27 |
| Φ11 | 664.91 | 99.27 |
| Φ3 | 764.18 | 68.91 |
| Φ10 | 833.09 | 33.09 |
| Φ2 | 866.18 | 66.18 |
| Φ9 | 932.36 | 99.27 |
| Φ1 | 1031.64 | 99.27 |
| Φ8 | 1130.91 | 69.09 (→ octava) |

**Gap promedio:** ~80 cents (vs ~100 cents en 12-TET, vs variable en 12-φ)
**Gap máximo:** ~100 cents (vs 100 cents en 12-TET, vs ~147 cents en 12-φ)

### Valores de la Escala 12-φW (Quintas Apiladas - Occidental)

| Stack # | Cents | Western Name | Gap al siguiente | Δ vs 12-TET |
|---------|-------|--------------|------------------|-------------|
| 0φW | 0.00 | CφW | 33.09 | 0 |
| 7φW | 33.09 | C#φW | 66.18 | -67 |
| 2φW | 99.27 | DφW | 99.27 | -1 |
| 9φW | 198.54 | D#φW | 99.27 | -1 |
| 4φW | 297.81 | EφW | 69.27 | -2 |
| 11φW | 367.09 | FφW | 99.27 | -33 |
| 6φW | 466.36 | F#φW | 99.27 | -34 |
| 1φW | 565.64 | GφW | 99.27 | -34 |
| 8φW | 664.91 | G#φW | 68.91 | -35 |
| 3φW | 733.82 | AφW | 99.27 | -66 |
| 10φW | 833.09 | A#φW | 33.09 | -67 |
| 5φW | 866.18 | BφW | 133.82 (→ octava) | -34 |

**Característica distintiva:** El sistema 12-φW mantiene semitonos de ~99 cents (casi idénticos a 12-TET), pero la quinta es de 833 cents en lugar de 700 cents. Esto crea un puente entre la familiaridad del sistema occidental y las propiedades matemáticas de φ.

**Gap promedio:** ~99 cents (esencialmente igual a 12-TET)
**Gap máximo:** ~134 cents (en BφW → CφW)
**Desviación promedio vs 12-TET:** ~31 cents

## Arquitectura del Motor (golden-harmony-engine.js)

### Clase Principal: GoldenHarmonyEngine

```javascript
class GoldenHarmonyEngine {
    constructor() {
        this.PHI = (1 + Math.sqrt(5)) / 2;
        this.chromaticScale = this.generateChromaticScale();
        this.diatonicScales = {};
        this.consonanceCache = new Map();
        this.consonanceTolerance = 25;  // cents
    }
}
```

### Métodos Clave

**1. Generación de Escala Cromática**
```javascript
generateChromaticScale() {
    // Paso 1: Generar 12 valores por división φ
    for (let i = 0; i < 12; i++) {
        const ratio = Math.pow(this.PHI, -i);
        const centsRaw = 1200 * ratio;
        const cents = centsRaw % 1200;  // Envolver en octava
        notes.push({ index: i, cents, ratio, name: `φ${i}` });
    }

    // Paso 2: Ordenar ascendentemente
    notes.sort((a, b) => a.cents - b.cents);

    // Paso 3: Normalizar (tónica = 0 cents)
    const offset = notes[0].cents;
    notes.forEach(note => note.cents -= offset);

    return notes;
}
```

**2. Función de Consonancia (con caché)**
```javascript
consonance(intervalCents, tolerance = 25) {
    // Verificar cache
    const key = `${intervalCents.toFixed(2)}_${tolerance}`;
    if (this.consonanceCache.has(key)) {
        return this.consonanceCache.get(key);
    }

    // Buscar potencia de φ más cercana
    let minDistance = Infinity;
    for (let k = -5; k <= 5; k++) {
        const phiInterval = 1200 * Math.log2(Math.pow(this.PHI, k));
        const wrapped = ((phiInterval % 1200) + 1200) % 1200;
        let distance = Math.abs(intervalCents - wrapped);
        distance = Math.min(distance, 1200 - distance);
        minDistance = Math.min(minDistance, distance);
    }

    // Gaussiana
    const sigma = tolerance / 2.0;
    const consonanceValue = Math.exp(-(minDistance * minDistance) / (sigma * sigma));

    this.consonanceCache.set(key, consonanceValue);
    return consonanceValue;
}
```

**3. Optimización de Escala Diatónica (7 de 12)**
```javascript
generateDiatonicScale(preset = 'major') {
    // Búsqueda exhaustiva: C(12, 7) = 792 combinaciones
    const allCombinations = this.getCombinations(12, 7);
    let bestScore = -Infinity;
    let bestScale = null;

    for (const combo of allCombinations) {
        const scale = combo.map(idx => this.chromaticScale[idx]);
        const score = this.scaleStabilityScore(scale);

        if (score > bestScore) {
            bestScore = score;
            bestScale = scale;
        }
    }

    return { notes: bestScale, score: bestScore };
}

scaleStabilityScore(scale) {
    let score = 0.0;

    // Sumar consonancia de todos los pares
    for (let i = 0; i < scale.length; i++) {
        for (let j = i + 1; j < scale.length; j++) {
            const interval = Math.abs(scale[j].cents - scale[i].cents);
            score += this.consonance(interval);
        }
    }

    // Penalización por microintervalos < 80 cents
    for (let i = 0; i < scale.length - 1; i++) {
        const step = scale[i + 1].cents - scale[i].cents;
        if (step < 80) score -= 10;
    }

    return score;
}
```

**4. Generación de Acordes**
```javascript
generateTriad(rootCents) {
    return [
        rootCents,
        (rootCents + this.getPhiInterval(2)) % 1200,
        (rootCents + this.getPhiInterval(3)) % 1200
    ].sort((a, b) => a - b);
}

generateSeventh(rootCents) {
    const notes = [
        rootCents,
        (rootCents + this.getPhiInterval(2)) % 1200,
        (rootCents + this.getPhiInterval(3)) % 1200,
        (rootCents + this.getPhiInterval(4)) % 1200
    ];

    // Filtrar colisiones < 100 cents
    const filtered = [notes[0]];
    for (let i = 1; i < notes.length; i++) {
        if (!filtered.some(n => Math.abs(notes[i] - n) < 100)) {
            filtered.push(notes[i]);
        }
    }

    return filtered.sort((a, b) => a - b);
}
```

**5. Voice Leading Óptimo**
```javascript
optimalVoiceLeading(chord1, chord2) {
    // Algoritmo greedy: asignar cada voz al destino más cercano
    const mapping = [];
    let totalMovement = 0;
    const used = new Set();

    for (let i = 0; i < chord1.length; i++) {
        let minDist = Infinity;
        let bestJ = -1;

        for (let j = 0; j < chord2.length; j++) {
            if (used.has(j)) continue;

            const dist = Math.min(
                Math.abs(chord2[j] - chord1[i]),
                Math.abs((chord2[j] + 1200) - chord1[i]),
                Math.abs((chord2[j] - 1200) - chord1[i])
            );

            if (dist < minDist) {
                minDist = dist;
                bestJ = j;
            }
        }

        if (bestJ !== -1) {
            used.add(bestJ);
            mapping.push({ from: i, to: bestJ, distance: minDist });
            totalMovement += minDist;
        }
    }

    return { mapping, totalMovement };
}
```

**6. Generación de Preludios SATB**
```javascript
generatePrelude({ scale, measures, tempo, density, complexity }) {
    // Fase 1: Progresión armónica
    const numChords = { simple: 4, moderate: 8, complex: 16 }[complexity];
    const progression = this.generateProgression(scale.notes, numChords, complexity);

    // Fase 2: Bajo estructural (fundamentales)
    const bass = progression.map((chord, i) => ({
        cents: chord.notes[0],
        duration: 2,
        time: i * 2
    }));

    // Fase 3: Voces intermedias (Alto y Tenor)
    const alto = progression.map((chord, i) => ({
        cents: chord.notes[Math.min(1, chord.notes.length - 1)],
        duration: 2,
        time: i * 2
    }));

    const tenor = progression.map((chord, i) => ({
        cents: chord.notes[chord.notes.length > 2 ? 1 : 0],
        duration: 2,
        time: i * 2
    }));

    // Fase 4: Soprano (melodía)
    const soprano = this.generateSopranoLine(progression, density);

    return { soprano, alto, tenor, bass, progression, measures, tempo };
}
```

## Simulaciones

### 1. Escala Cromática Áurea (`escala-cromatica-aurea.html`)

**Concepto:** Explorador interactivo de las 12 notas cromáticas generadas por φ

**Características:**
- 12 botones de notas con notación φ (φ₀, φ₁, ..., φ₁₁)
- Visualización circular con espaciado proporcional a cents
- Gráfico de barras de cents con comparación opcional vs 12-TET
- Tabla completa de valores (cents, ratios, frecuencias, intervalos)
- Web Audio playback con ADSR envelope
- Selector de frecuencia base (220, 440, 880 Hz)
- Selector de forma de onda (sine, triangle, sawtooth, square)
- Toggle de comparación con temperamento igual

**Audio:**
```javascript
function playNote(frequency, duration = 1.0, volume = 0.3) {
    const oscillator = audioContext.createOscillator();
    const gainNode = audioContext.createGain();
    const filter = audioContext.createBiquadFilter();

    oscillator.type = waveform;  // configurable
    oscillator.frequency.value = frequency;

    filter.type = 'lowpass';
    filter.frequency.value = 2000;
    filter.Q.value = 1;

    // ADSR
    const now = audioContext.currentTime;
    gainNode.gain.setValueAtTime(0, now);
    gainNode.gain.linearRampToValueAtTime(volume, now + 0.05);
    gainNode.gain.linearRampToValueAtTime(volume * 0.7, now + 0.15);
    gainNode.gain.setValueAtTime(volume * 0.7, now + duration - 0.2);
    gainNode.gain.linearRampToValueAtTime(0, now + duration);

    oscillator.connect(filter);
    filter.connect(gainNode);
    gainNode.connect(audioContext.destination);

    oscillator.start(now);
    oscillator.stop(now + duration);
}
```

**Visualizaciones:**
- **Circular:** Teclado circular con 12 notas espaciadas proporcionalmente
- **Cents bar chart:** Altura de cada barra = cents value
- **Overlay 12-TET:** Puntos azules en posiciones equidistantes (0, 100, 200, ...)

**Líneas:** ~850

---

### 2. Armonizador Áureo (`armonizador-aureo.html`)

**Concepto:** Constructor de acordes y progresiones armónicas con análisis de consonancia

**Características:**
- Selector de escala diatónica (Mayor Áurea, Menor Áurea, Custom)
- Tipo de acorde: Tríada, 7ª, 9ª (basados en potencias de φ)
- Grid de 7 grados (I-VII) para construir acordes
- Constructor de progresión (click grados para añadir, X para quitar)
- Presets de progresiones: I-IV-V-I, ii-V-I, I-vi-IV-V, Círculo φ
- Visualización de acordes en pentagrama adaptado
- Heatmap de consonancia (matriz NxN de intervalos)
- Estadísticas: Score de escala, consonancia promedio, movimiento de voces

**Audio:**
```javascript
function playChord(chordCents, duration = 1.0, volume = 0.25) {
    const oscillators = [];

    chordCents.forEach(cents => {
        const freq = engine.centsToFrequency(cents);
        const osc = audioContext.createOscillator();
        const gain = audioContext.createGain();
        const filter = audioContext.createBiquadFilter();

        osc.type = waveform;  // configurable
        osc.frequency.value = freq;

        filter.type = 'lowpass';
        filter.frequency.value = 3000;
        filter.Q.value = 1.5;

        // ADSR (similar a Sim 1)
        // ...

        osc.connect(filter);
        filter.connect(gain);
        gain.connect(audioContext.destination);

        osc.start(now);
        osc.stop(now + duration);

        oscillators.push({ osc, gain });
    });
}
```

**Visualizaciones:**
- **Staff notation:** Pentagrama con 5 líneas, notas posicionadas por cents
- **Heatmap de consonancia:** Matriz simétrica con colores:
  - Verde: alta consonancia (> 0.7)
  - Amarillo: media (0.4-0.7)
  - Rojo: baja (< 0.4)
- **Estadísticas:**
  - Score de estabilidad de escala
  - Consonancia promedio de intervalos
  - Número de acordes en progresión
  - Movimiento total de voces (cents)

**Líneas:** ~1100

---

### 3. Compositor Áureo (`compositor-aureo.html`)

**Concepto:** Generador algorítmico de preludios SATB en estilo Bach usando armonía φ

**Características:**
- Parámetros de composición:
  - Escala base (Mayor/Menor Áurea)
  - Compases (8-32, slider)
  - Tempo (60-140 BPM)
  - Densidad rítmica (baja/media/alta)
  - Complejidad armónica (simple/moderate/complex)
- Controles de voces individuales (toggle S/A/T/B)
- Visualización de partitura (4 pentagramas)
- Playback con progress bar
- Estadísticas: compases, notas totales, acordes, consonancia promedio

**Audio (4 voces simultáneas):**
```javascript
function scheduleNote(cents, startTime, duration, waveform, pan, volume) {
    const freq = engine.centsToFrequency(cents);

    const osc = audioContext.createOscillator();
    const gain = audioContext.createGain();
    const filter = audioContext.createBiquadFilter();
    const panner = audioContext.createStereoPanner();

    osc.type = waveform;  // diferente por voz
    osc.frequency.value = freq;

    filter.type = 'lowpass';
    filter.frequency.value = 2500;

    panner.pan.value = pan;  // estéreo spread

    // ADSR
    // ...

    osc.connect(filter);
    filter.connect(gain);
    gain.connect(panner);
    panner.connect(audioContext.destination);

    osc.start(startTime);
    osc.stop(startTime + duration);
}

// Configuración de voces
const waveforms = ['triangle', 'sawtooth', 'square', 'sine'];  // S, A, T, B
const panning = [-0.7, -0.3, 0.3, 0.7];  // estéreo spread
```

**Algoritmo de Composición (simplificado):**
```
1. Generar progresión armónica (4-16 acordes según complejidad)
2. Bajo: fundamentales de acordes (φ⁰ de cada tríada)
3. Alto/Tenor: notas intermedias de acordes (φ², φ³)
4. Soprano: melodía con figuración rítmica según densidad:
   - Baja: 1 nota por acorde (blancas)
   - Media: 2 notas por acorde (negras/corcheas)
   - Alta: 4 notas por acorde (semicorcheas)
5. Verificar reglas de contrapunto áureo (quintas paralelas φ¹, saltos > φ³)
```

**Visualización:**
- 4 pentagramas (S, A, T, B) con colores distintos:
  - Soprano: #ec4899 (rosa)
  - Alto: #a78bfa (púrpura)
  - Tenor: #3b82f6 (azul)
  - Bajo: #22c55e (verde)
- Notas posicionadas por tiempo (x) y pitch (y)
- Staff lines simplificadas (5 líneas por voz)

**Líneas:** ~900

---

### 4. Escala de 15 Notas (`escala-15-notas.html`)

**Concepto:** Explorador de la escala microtonal de 15 notas generada por quintas áureas apiladas

**Características:**
- 15 botones de notas con notación Φ (Φ0, Φ1, ..., Φ14)
- Visualización de círculo de quintas áureas
- Comparación triple: 15-φ vs 12-TET vs 12-φ
- Distribución lineal mostrando gaps entre notas
- Estadísticas: gap promedio, gap máximo
- Reproducción en orden cromático o en orden de quintas
- Web Audio playback con ADSR

**Ventajas sobre la escala de 12 notas:**
- Distribución más uniforme (~80 cents promedio vs variable)
- Gap máximo ~100 cents (vs ~147 cents en 12-φ)
- Mejor cobertura de la octava
- Círculo de quintas áureas completo (15 notas = 10.41 octavas)

**Audio:**
```javascript
// Igual patrón que otras sims
async function playNote(frequency, dur, vol) {
    const osc = audioContext.createOscillator();
    const envelope = audioContext.createGain();
    // ADSR envelope
    // ...
}

// Modo especial: tocar en orden de quintas
async function playFifths() {
    const sortedByStack = [...scale15].sort((a, b) => a.stackOrder - b.stackOrder);
    // Reproduce Φ0 → Φ1 → Φ2 → ... → Φ14
}
```

**Visualizaciones:**
- **Círculo de quintas:** 15 notas con número de stack en el centro de cada punto
- **Distribución lineal:** Barras con altura = gap al siguiente
- **Comparación:** Toggle para superponer 12-TET y 12-φ

**Líneas:** ~550

---

### 5. Armonizador 15 Notas (`armonizador-15-notas.html`)

**Concepto:** Constructor de acordes y progresiones para el sistema de 15 notas

**Características:**
- Selección de notas cromáticas para formar escala diatónica (5-9 notas)
- Tipos de acorde: Tríada, Tétrada, Péntada
- Intervalos de apilación configurables (terceras, cuartas, quintas)
- Constructor de progresión con click en grados
- Presets: I-IV-V-I, Ascendente, Descendente, Círculo φ
- Visualización de acorde en pentagrama
- Heatmap de consonancia con análisis
- Estadísticas: consonancia promedio, movimiento de voces

**Diferencias con Armonizador 12 notas:**
- Base de 15 notas cromáticas en lugar de 12
- Escalas diatónicas de 5-9 notas (no fijo en 7)
- Selección manual de notas en la escala
- Más flexibilidad en construcción de acordes

**Generación de acordes:**
```javascript
function generateChordFromDegree(degreeIndex) {
    const numNotes = selectedChordType === 'triad' ? 3 :
                    selectedChordType === 'tetrad' ? 4 : 5;

    const chordNotes = [];
    const scaleLen = diatonicScale.length;

    for (let i = 0; i < numNotes; i++) {
        const noteIndex = (degreeIndex + i * chordInterval) % scaleLen;
        chordNotes.push(diatonicScale[noteIndex].cents);
    }

    return [...new Set(chordNotes)].sort((a, b) => a - b);
}
```

**Visualizaciones:**
- **Grid cromático:** 15 notas con toggle para incluir/excluir
- **Grid de grados:** I-XV según notas seleccionadas
- **Progresión:** Acordes arrastrables con botón eliminar
- **Pentagrama:** Notas del acorde actual
- **Heatmap:** Matriz de consonancia entre notas

**Líneas:** ~600

---

### 6. Compositor 15 Notas (`compositor-15-notas.html`)

**Concepto:** Generador algorítmico de preludios SATB para el sistema de 15 notas

**Características:**
- Selección de notas cromáticas para formar escala diatónica (5-12 notas)
- 4 voces independientes: Soprano, Alto, Tenor, Bajo
- Densidad rítmica: Baja (blancas), Media (negras), Alta (corcheas)
- Complejidad armónica: Simple (4), Moderada (8), Compleja (16 acordes)
- Toggle individual de voces
- Partitura visual con pentagrama por voz
- Reproducción con panning estéreo
- Barra de progreso

**Diferencias con Compositor 12 notas:**
- Base de 15 notas cromáticas
- Escalas diatónicas customizables (7-9 notas)
- Selección manual de notas en lugar de preset mayor/menor
- Acordes generados desde la escala seleccionada

**Generación de preludios:**
```javascript
function generatePrelude15(params) {
    const { measures, tempo, density, complexity } = params;

    const numChords = { simple: 4, moderate: 8, complex: 16 }[complexity];
    const progression = generateProgression15(numChords, complexity);

    // Genera notas para cada voz basándose en la progresión
    // Bass: nota raíz del acorde
    // Alto: segunda nota
    // Tenor: nota media
    // Soprano: melodía con densidad variable
}
```

**Audio:**
- Waveforms diferentes por voz: triangle, sawtooth, square, sine
- Panning: S(-0.7), A(-0.3), T(0.3), B(0.7)
- ADSR envelopes para cada nota
- Programación de notas con `scheduleNote()`

**Líneas:** ~550

---

### 7. Escala 12-φW (`escala-12-phiW.html`)

**Concepto:** Explorador de la escala 12-φW que combina la familiaridad del sistema occidental (12 notas, ~99 cents por semitono) con las propiedades matemáticas de φ (quinta de 833 cents)

**Color del sistema:** Naranja (#f97316)

**Características:**
- 12 botones de notas con notación φW (CφW, C#φW, ..., BφW)
- Visualización de círculo de quintas áureas
- Comparación triple: 12-φW vs 12-TET vs 12-φ original
- Distribución lineal mostrando gaps y desviaciones
- Comparador auditivo: alterna notas 12-φW ↔ 12-TET
- Estadísticas: desviación promedio, gap máximo
- Modo cromático y modo quintas (stack order)

**Ventajas del sistema 12-φW:**
- Semitonos casi idénticos a 12-TET (~99 cents vs 100 cents)
- Matemáticamente derivado de φ (no arbitrario)
- Círculo de quintas completo (12 notas = ~7.5 octavas)
- Puente natural entre teoría occidental y armonía áurea
- Más familiar al oído occidental que 12-φ original

**Audio:**
```javascript
// Comparación auditiva 12-φW vs 12-TET
async function playComparison() {
    const sorted = [...chromatic12WScale].sort((a, b) => a.cents - b.cents);
    for (const note of sorted) {
        // Nota 12-φW
        await playNote(centsToFreq(note.cents), 0.4);
        await sleep(100);
        // Nota 12-TET equivalente
        const tetCents = note.chromaticIndex * 100;
        await playNote(centsToFreq(tetCents), 0.4);
        await sleep(200);
    }
}
```

**Visualizaciones:**
- **Círculo de quintas:** 12 notas con color naranja, número de stack en cada punto
- **Distribución lineal:** Barras comparando posición vs 12-TET
- **Color-coding:** Verde (<5¢ diferencia), Amarillo (<20¢), Rojo (>20¢)
- **Comparación toggle:** Superponer 12-TET y/o 12-φ original

**Líneas:** ~650

---

### 8. Armonizador 12-φW (`armonizador-12-phiW.html`)

**Concepto:** Constructor de acordes y progresiones armónicas para el sistema 12-φW. Combina la familiaridad de las progresiones occidentales (I-IV-V-I, ii-V-I) con la quinta áurea de 833 cents.

**Color del sistema:** Naranja (#f97316)

**Características:**
- Escala diatónica de 7 notas por defecto (patrón mayor occidental)
- Escalas alternativas: pentatónica (5), hexatónica (6), octatónica (8)
- Tipos de acorde: Tríada, Tétrada, Péntada
- Intervalos de apilación: Terceras, Cuartas, Quintas
- Presets de progresiones clásicas: I-IV-V-I, ii-V-I, I-vi-IV-V, I-V-vi-IV, Blues φW
- Heatmap de consonancia con análisis
- Voice leading óptimo con estadísticas de movimiento

**Diferencias con Armonizador 15 notas:**
- Base de 12 notas cromáticas (familiar)
- Escala diatónica por defecto de 7 notas (como el sistema occidental)
- Presets orientados a progresiones clásicas del jazz y pop
- Nombres de notas occidentales adaptados (CφW, DφW, etc.)

**Presets de progresiones:**
```javascript
const presets = {
    'I-IV-V-I': [0, 3, 4, 0],           // Cadencia auténtica
    'ii-V-I': [1, 4, 0],                 // Jazz standard
    'I-vi-IV-V': [0, 5, 3, 4],           // 50s progression
    'I-V-vi-IV': [0, 4, 5, 3],           // Pop anthem
    'circle': [0, 3, 6, 2, 5, 1, 4, 0],  // Circle of φW fifths
    'blues': [0, 0, 0, 0, 3, 3, 0, 0, 4, 3, 0, 4]  // 12-bar blues φW
};
```

**Audio:**
```javascript
// Mismo patrón que armonizador-15-notas
async function playChord(chordCents, duration = 1.0, volume = 0.25) {
    const now = audioContext.currentTime;
    chordCents.forEach((cents) => {
        const freq = engine.centsToFrequency(cents);
        const osc = audioContext.createOscillator();
        osc.type = waveform;
        // ADSR envelope...
    });
}
```

**Visualizaciones:**
- **Grid cromático:** 12 notas con nombres occidentales (CφW, C#φW, ...)
- **Grid de grados:** I-VII según notas seleccionadas
- **Progresión:** Acordes con nombre de raíz y tipo
- **Pentagrama:** Notas del acorde actual con color naranja
- **Heatmap:** Matriz de consonancia φ entre notas

**Líneas:** ~620

---

### 9. Compositor 12-φW (`compositor-12-phiW.html`)

**Concepto:** Generador algorítmico de preludios a 4 voces (SATB) usando el sistema 12-φW. Combina la familiaridad del sistema occidental con las propiedades matemáticas de φ.

**Color del sistema:** Naranja (#f97316)

**Características:**
- 4 voces SATB con waveforms diferenciados (triangle, sawtooth, square, sine)
- Panning estéreo por voz (-0.7, -0.3, 0.3, 0.7)
- Escala diatónica de 7 notas por defecto (patrón mayor occidental)
- Opciones de 5-8 notas (pentatónica, hexatónica, octatónica)
- Densidad rítmica configurable (blancas, negras, corcheas)
- Complejidad armónica (4, 8, 16 acordes)
- Visualización de partitura con 4 pentagramas

**Progresiones generadas:**
- **Simple:** I-IV-V-I (cadencia auténtica)
- **Moderada:** ii-V-I, I-vi-IV-V (patrones jazz/pop)
- **Compleja:** Mezcla con inicio y final en tónica

**Audio:**
```javascript
// Scheduling de notas con Web Audio API
function scheduleNote(cents, startTime, duration, waveform, pan, volume) {
    const freq = engine.centsToFrequency(cents);
    const osc = audioContext.createOscillator();
    const envelope = audioContext.createGain();
    const panner = audioContext.createStereoPanner();

    osc.type = waveform;
    panner.pan.value = pan;
    // ADSR envelope scheduling...
}
```

**Visualización:**
- **Partitura:** 4 pentagramas con notas coloreadas por voz
- **Estadísticas:** Compases, notas totales, acordes, consonancia promedio
- **Barra de progreso:** Sincronizada con playback

**Diferencias con Compositor 15 notas:**
- Base de 12 notas cromáticas (familiar)
- Progresiones orientadas a patrones occidentales
- Escala diatónica de 7 notas por defecto

**Líneas:** ~600

---

## Reglas de Contrapunto Áureo

### Diferencias con Contrapunto Tradicional

| Concepto | Tradicional (Fux) | Áureo (φ) |
|----------|-------------------|-----------|
| **Consonancia perfecta** | Unísono, 8ª, 5ª justa | φ⁰, φ¹ (833¢), φ² |
| **Consonancia imperfecta** | 3ª, 6ª | φ³, φ⁴ |
| **Disonancia** | 2ª, 7ª, tritono | Intervalos alejados de φ^k |
| **Quintas paralelas** | Prohibidas (5ª = 700¢) | Prohibidas (φ¹ = 833¢) |
| **Resolución** | Disonancia → consonancia | d(φ^k) grande → d(φ^k) pequeña |
| **Saltos grandes** | 6ª mayor+ sin resolver | > φ³ sin resolver |

### Reglas Implementadas

**1. No quintas áureas paralelas:**
```javascript
verifyCounterpoint(satb) {
    const phiFifth = this.getPhiInterval(1);  // ~833 cents

    for (let i = 0; i < satb.soprano.length - 1; i++) {
        const interval1 = Math.abs(satb.soprano[i].cents - satb.bass[i].cents);
        const interval2 = Math.abs(satb.soprano[i + 1].cents - satb.bass[i + 1].cents);

        if (Math.abs(interval1 - phiFifth) < 20 &&
            Math.abs(interval2 - phiFifth) < 20) {
            violations.push({ type: 'parallel_phi_fifths', location: i });
        }
    }
}
```

**2. Saltos grandes sin resolución:**
```javascript
const maxJump = this.getPhiInterval(3);  // φ³

for (const voice of ['soprano', 'alto', 'tenor', 'bass']) {
    for (let i = 0; i < satb[voice].length - 1; i++) {
        const jump = Math.abs(satb[voice][i + 1].cents - satb[voice][i].cents);
        if (jump > maxJump) {
            violations.push({ type: 'large_unresolved_jump', voice, location: i });
        }
    }
}
```

**3. Movimiento preferido: contrario y oblicuo**
- Voice leading óptimo minimiza movimiento total
- Penalización por movimiento directo a intervalos no-consonantes

## Patrones de Web Audio

### Lazy Initialization (iOS/Safari)

```javascript
let audioContext = null;

function initAudio() {
    if (!audioContext) {
        audioContext = new (window.AudioContext || window.webkitAudioContext)();
    }
    if (audioContext.state === 'suspended') {
        audioContext.resume();
    }
}

// Llamar en primer click del usuario
playButton.onclick = () => {
    initAudio();  // Lazy init
    playNote(440);
};
```

### ADSR Envelope Pattern

```javascript
const attack = 0.05;   // 50ms
const decay = 0.1;     // 100ms
const sustain = volume * 0.7;  // 70% del volumen
const release = 0.2;   // 200ms

const now = audioContext.currentTime;
gainNode.gain.setValueAtTime(0, now);
gainNode.gain.linearRampToValueAtTime(volume, now + attack);
gainNode.gain.linearRampToValueAtTime(sustain, now + attack + decay);
gainNode.gain.setValueAtTime(sustain, now + duration - release);
gainNode.gain.linearRampToValueAtTime(0, now + duration);
```

### Cents → Frecuencia

```javascript
function centsToFrequency(cents, baseFreq = 440) {
    return baseFreq * Math.pow(2, cents / 1200);
}

// Ejemplo: φ₁ = 741.64 cents
const freq = centsToFrequency(741.64, 440);  // ~734.99 Hz
```

### Polyphony Management

```javascript
const currentNotes = [];  // Array de { oscillator, gainNode }

function playNote(freq, duration) {
    // ... crear osc y gain ...

    currentNotes.push({ oscillator, gainNode });

    // Cleanup automático
    setTimeout(() => {
        const index = currentNotes.findIndex(n => n.oscillator === oscillator);
        if (index !== -1) currentNotes.splice(index, 1);
    }, duration * 1000);
}

function stopAll() {
    currentNotes.forEach(({ oscillator, gainNode }) => {
        try {
            gainNode.gain.cancelScheduledValues(audioContext.currentTime);
            gainNode.gain.setValueAtTime(0, audioContext.currentTime);
            oscillator.stop(audioContext.currentTime + 0.01);
        } catch (e) {}
    });
    currentNotes.length = 0;
}
```

## Convenciones de Código

### Nomenclatura Musical

**Notación φ (interna):** φ₀, φ₁, φ₂, ..., φ₁₁

**Notación tradicional (UI):** Cφ, C#φ, Dφ, D#φ, Eφ, Fφ, F#φ, Gφ, G#φ, Aφ, A#φ, Bφ

**Grados romanos:** I, II, III, IV, V, VI, VII

**Voces SATB:** Soprano, Alto, Tenor, Bass (Bajo)

### Modos Áureos (nombres griegos)

```javascript
const modeNames = [
    'Φόνικο',   // Phōniko (Jónico áureo)
    'Αὐρίδιο',  // Aurídio (Dórico áureo)
    'Χρυσίγιο', // Chrysígio (Frigio áureo)
    'Λυδαυρό',  // Lydauró (Lidio áureo)
    'Μιξόφι',   // Mixóphi (Mixolidio áureo)
    'Αιόλφι',   // Aiólphi (Eólico áureo)
    'Λοκρόφι'   // Lokróphi (Locrio áureo)
];
```

### Colores por Voz

```css
.soprano { color: #ec4899; }  /* Rosa */
.alto { color: #a78bfa; }     /* Púrpura */
.tenor { color: #3b82f6; }    /* Azul */
.bass { color: #22c55e; }     /* Verde */
```

## Referencias Teóricas

### Documentación

**GOLDEN_HARMONY_THEORY.md:** Teoría completa con 14 secciones:
1. Introducción y motivación
2. Escala cromática áurea (derivación + tabla de valores)
3. Función de consonancia (Gaussiana + tolerancia)
4. Escalas diatónicas (optimización C(12,7))
5. Acordes áureos (tríadas, 7ª, 9ª)
6. Modos armónicos (7 rotaciones)
7. Función armónica áurea (I-IV-V redefinido)
8. Contrapunto áureo (reglas adaptadas)
9. Composición algorítmica (SATB)
10. Implementación (pseudocódigo completo)
11. Web Audio synthesis
12. Validación (tests de coherencia)
13. Conclusiones
14. Apéndices (tablas de intervalos, glosario)

### Conceptos Interdisciplinarios

**Matemática:**
- Proporción áurea (φ = 1.618...)
- Fibonacci (convergencia a φ)
- Optimización combinatoria (792 combinaciones)
- Distribución gaussiana
- Algoritmo húngaro (voice leading)

**Física:**
- Frecuencia = 440 · 2^(cents/1200)
- Ondas sonoras (Web Audio API)
- Envolventes ADSR
- Filtros pasa-bajos

**Computación:**
- Búsqueda exhaustiva (C(n, k))
- Algoritmos greedy (voice leading)
- Caché de valores (Map)
- Generación procedural (preludios)

**Música:**
- Temperamento igual vs. just intonation vs. φ
- Contrapunto (Fux, especies)
- Función armónica tonal
- Voice leading (Piston, Schönberg)

## Estadísticas del Proyecto

**Archivos:** 12 (index + teoría + motor + 9 sims + docs)

**Líneas de código:**
- golden-harmony-engine.js: ~830
- escala-cromatica-aurea.html: ~850
- armonizador-aureo.html: ~1100
- compositor-aureo.html: ~900
- escala-15-notas.html: ~550
- armonizador-15-notas.html: ~600
- compositor-15-notas.html: ~550
- escala-12-phiW.html: ~650
- armonizador-12-phiW.html: ~620
- compositor-12-phiW.html: ~600
- index.html: ~880
- **Total código:** ~8,130 líneas

**Líneas de documentación:**
- GOLDEN_HARMONY_THEORY.md: ~1800
- CLAUDE.md (este archivo): ~1600
- **Total docs:** ~3,400 líneas

**Total proyecto:** ~11,530 líneas

**Commits:**
- 2026-01-11: Compositor 12-φW (generador SATB occidental-φ)
- 2026-01-11: Armonizador 12-φW (progresiones occidentales con quinta φ)
- 2026-01-11: Escala 12-φW (sistema occidental con quinta áurea)
- 2026-01-11: Compositor 15 notas (generador SATB)
- 2026-01-11: Armonizador 15 notas (constructor de acordes)
- 2026-01-11: Escala de 15 notas (quintas áureas apiladas)
- 2026-01-10: Proyecto completo Music Theory Lab
- Teoría, motor, 3 simulaciones, portal actualizado

## Mejoras Futuras

### Fase 2: Expansión Teórica
- [x] **Escala de 15 notas con quintas áureas apiladas**: `cents_i = (i × 833.09) mod 1200` ✅
  - Implementada en `escala-15-notas.html`
  - Distribución más uniforme con gap promedio ~80¢
  - Gap máximo ~100¢ vs ~147¢ en escala de 12
- [x] **Sistema 12-φW (occidental)**: 12 notas con quinta áurea ✅
  - Implementada en `escala-12-phiW.html`
  - Semitonos de ~99¢ (casi idénticos a 12-TET)
  - Puente entre sistema occidental y armonía φ
- [x] **Armonizador 12-φW**: Constructor de acordes para sistema occidental-φ ✅
  - Implementado en `armonizador-12-phiW.html`
  - Presets de progresiones clásicas (I-IV-V-I, ii-V-I, Blues φW)
  - Escala diatónica de 7 notas por defecto
- [x] **Compositor 12-φW**: Generador SATB para sistema occidental-φ ✅
  - Implementado en `compositor-12-phiW.html`
  - Preludios a 4 voces con progresiones occidentales
  - Densidad y complejidad configurables
- [ ] Escalas de 5 notas (pentatónicas áureas)
- [ ] Escalas de 8-9 notas (octatónicas, nonatónicas)
- [ ] Microtonalidad φ (escalas > 12 notas por octava)
- [ ] Temperamentos históricos comparados con φ

### Fase 3: Compositor Avanzado
- [ ] Exportar MIDI (usando Web MIDI API)
- [ ] Formas musicales: Sonata, Fuga, Canon
- [ ] Análisis armónico automático (identificar cadencias)
- [ ] Machine learning: entrenar modelo con preludios generados

### Fase 4: Análisis y Herramientas
- [ ] Analizador de audio en tiempo real (micrófono → FFT → cents)
- [ ] Comparador de afinaciones (12-TET vs Just vs φ)
- [ ] Secuenciador MIDI con entrada φ
- [ ] Teclado MIDI virtual con layout áureo

---

**Última actualización:** 2026-01-11

**Autor:** Carlos Lorente Kaiser

**Co-Author:** Claude Opus 4.5

**Repositorio:** https://github.com/cjlkaiser-cpu/eigenlab
