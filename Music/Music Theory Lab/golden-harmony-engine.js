/**
 * Golden Harmony Engine
 * Motor matemático para el Sistema Armónico Áureo (Φ-Harmonic System)
 *
 * Este motor implementa la teoría completa documentada en GOLDEN_HARMONY_THEORY.md
 * Genera escalas, acordes, progresiones y preludios basados en la proporción áurea φ
 *
 * @author EigenLab - Music Theory Lab
 * @version 1.0.0
 */

class GoldenHarmonyEngine {
    constructor() {
        // Proporción áurea - operador estructural fundamental
        this.PHI = (1 + Math.sqrt(5)) / 2; // 1.618033988749895

        // Referencia de afinación (A4 = 440 Hz)
        this.baseFrequency = 440;
        this.baseNote = 69; // MIDI note number for A4

        // Nombres de notas (notación φ) - MUST be defined before generateChromaticScale()
        this.noteNames = ['Cφ', 'C#φ', 'Dφ', 'D#φ', 'Eφ', 'Fφ', 'F#φ', 'Gφ', 'G#φ', 'Aφ', 'A#φ', 'Bφ'];

        // Nombres de modos áureos
        this.modeNames = [
            'Φόνικο',   // Phōniko (Jónico áureo)
            'Αὐρίδιο',  // Aurídio (Dórico áureo)
            'Χρυσίγιο', // Chrysígio (Frigio áureo)
            'Λυδαυρό',  // Lydauró (Lidio áureo)
            'Μιξόφι',   // Mixóphi (Mixolidio áureo)
            'Αιόλφι',   // Aiólphi (Eólico áureo)
            'Λοκρόφι'   // Lokróphi (Locrio áureo)
        ];

        // Escala cromática áurea (12 notas)
        this.chromaticScale = this.generateChromaticScale();

        // Escala cromática áurea extendida (15 notas - quintas apiladas)
        this.chromatic15Scale = this.generateChromatic15Scale();

        // Escala cromática 12-φW (12 notas - quintas apiladas, estilo occidental)
        this.chromatic12WScale = this.generateChromatic12WScale();

        // Cache de escalas diatónicas
        this.diatonicScales = {};

        // Cache de valores de consonancia para optimización
        this.consonanceCache = new Map();

        // Configuración de consonancia
        this.consonanceTolerance = 25; // cents
    }

    /**
     * Genera la escala cromática áurea de 15 notas (quintas áureas apiladas)
     * Formula: cents_i = (i × 833.09) mod 1200, i ∈ [0, 14]
     *
     * La quinta áurea (φ¹) = 1200 × log₂(φ) ≈ 833.09 cents
     * Apilar 15 quintas áureas cubre la octava con gaps más uniformes (~80-100¢)
     *
     * @returns {Array} Array de 15 objetos con propiedades {index, cents, name, phiPower}
     */
    generateChromatic15Scale() {
        const notes = [];
        const phiFifth = 1200 * Math.log2(this.PHI); // ~833.09 cents

        // Generar 15 notas por apilación de quintas áureas
        for (let i = 0; i < 15; i++) {
            const centsRaw = i * phiFifth;
            const cents = centsRaw % 1200;

            notes.push({
                index: i,
                stackOrder: i, // Orden en el círculo de quintas áureas
                cents: cents,
                phiFifth: phiFifth,
                name: `Φ${i}`
            });
        }

        // Ordenar por altura (cents ascendentes)
        notes.sort((a, b) => a.cents - b.cents);

        // Asignar índice cromático después de ordenar
        notes.forEach((note, idx) => {
            note.chromaticIndex = idx;
        });

        return notes;
    }

    /**
     * Genera la escala cromática 12-φW (12 notas por quintas áureas apiladas)
     * Formula: cents_i = (i × 833.09) mod 1200, i ∈ [0, 11]
     *
     * Este sistema es el puente entre el mundo occidental y el universo φ:
     * - Los semitonos son ~99 cents (casi igual a 12-TET con 100 cents)
     * - Pero la quinta es 833 cents (no 700 cents como 12-TET)
     * - El círculo de quintas cierra en 12 pasos (como el occidental)
     *
     * @returns {Array} Array de 12 objetos con propiedades {index, cents, name, stackOrder}
     */
    generateChromatic12WScale() {
        const notes = [];
        const phiFifth = 1200 * Math.log2(this.PHI); // ~833.09 cents

        // Nombres estilo occidental para familiaridad
        const westernNames = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];

        // Generar 12 notas por apilación de quintas áureas
        for (let i = 0; i < 12; i++) {
            const centsRaw = i * phiFifth;
            const cents = centsRaw % 1200;

            notes.push({
                index: i,
                stackOrder: i, // Orden en el círculo de quintas áureas
                cents: cents,
                phiFifth: phiFifth,
                name: `${i}φW` // Nombre temporal, se asignará después de ordenar
            });
        }

        // Ordenar por altura (cents ascendentes)
        notes.sort((a, b) => a.cents - b.cents);

        // Asignar índice cromático y nombres occidentales después de ordenar
        notes.forEach((note, idx) => {
            note.chromaticIndex = idx;
            note.westernName = westernNames[idx] + 'φ';
            note.name = westernNames[idx] + 'φW';
        });

        // Calcular gaps entre notas
        for (let i = 0; i < notes.length; i++) {
            const nextIdx = (i + 1) % notes.length;
            const nextCents = nextIdx === 0 ? 1200 : notes[nextIdx].cents;
            notes[i].gap = nextCents - notes[i].cents;
        }

        return notes;
    }

    /**
     * Genera la escala cromática áurea de 12 notas
     * Formula: n_i = (1200 · φ^(-i)) mod 1200, i ∈ [0, 11]
     *
     * @returns {Array} Array de 12 objetos con propiedades {index, cents, ratio, name}
     */
    generateChromaticScale() {
        const notes = [];

        // Paso 1: Generar 12 valores por división φ
        for (let i = 0; i < 12; i++) {
            const ratio = Math.pow(this.PHI, -i);
            const centsRaw = 1200 * ratio;
            const cents = centsRaw % 1200; // Envolver en octava

            notes.push({
                index: i,
                cents: cents,
                ratio: ratio,
                name: `φ${i}`
            });
        }

        // Paso 2: Ordenar por altura (cents ascendentes)
        notes.sort((a, b) => a.cents - b.cents);

        // Paso 3: Normalizar - tónica = 0 cents
        const offset = notes[0].cents;
        notes.forEach(note => {
            note.cents -= offset;
        });

        // Paso 4: Asignar nombres de notas tradicionales
        notes.forEach((note, idx) => {
            note.noteName = this.noteNames[idx % 12];
            note.chromaticIndex = idx;
        });

        return notes;
    }

    /**
     * Función de consonancia áurea
     * Un intervalo es consonante si está cerca de una potencia de φ
     *
     * @param {number} intervalCents - Intervalo en cents
     * @param {number} tolerance - Tolerancia perceptiva (default: 25 cents)
     * @returns {number} Valor de consonancia [0, 1]
     */
    consonance(intervalCents, tolerance = null) {
        if (tolerance === null) tolerance = this.consonanceTolerance;

        // Verificar cache
        const key = `${intervalCents.toFixed(2)}_${tolerance}`;
        if (this.consonanceCache.has(key)) {
            return this.consonanceCache.get(key);
        }

        // Normalizar intervalo a [0, 1200)
        intervalCents = ((intervalCents % 1200) + 1200) % 1200;

        // Buscar la potencia de φ más cercana
        let minDistance = Infinity;

        for (let k = -5; k <= 5; k++) {
            // Intervalo correspondiente a φ^k en cents
            const phiInterval = 1200 * Math.log2(Math.pow(this.PHI, k));
            const wrapped = ((phiInterval % 1200) + 1200) % 1200;

            // Calcular distancia mínima (considerando wrap-around)
            let distance = Math.abs(intervalCents - wrapped);
            distance = Math.min(distance, 1200 - distance);

            minDistance = Math.min(minDistance, distance);
        }

        // Función gaussiana de consonancia
        const sigma = tolerance / 2.0;
        const consonanceValue = Math.exp(-(minDistance * minDistance) / (sigma * sigma));

        // Guardar en cache
        this.consonanceCache.set(key, consonanceValue);

        return consonanceValue;
    }

    /**
     * Calcula el score de estabilidad de una escala
     * Score(S₇) = Σ(i<j) C(|n_i - n_j|) - Penalty
     *
     * @param {Array} scale - Array de notas
     * @returns {number} Score de estabilidad
     */
    scaleStabilityScore(scale) {
        let score = 0.0;

        // Sumar consonancia de todos los pares de intervalos
        for (let i = 0; i < scale.length; i++) {
            for (let j = i + 1; j < scale.length; j++) {
                const interval = Math.abs(scale[j].cents - scale[i].cents);
                score += this.consonance(interval);
            }
        }

        // Penalización por microintervalos consecutivos (< 80 cents)
        for (let i = 0; i < scale.length - 1; i++) {
            const step = scale[i + 1].cents - scale[i].cents;
            if (step < 80) {
                score -= 10; // Penalización fuerte
            }
        }

        // Penalización leve por pasos muy grandes (> 300 cents)
        for (let i = 0; i < scale.length - 1; i++) {
            const step = scale[i + 1].cents - scale[i].cents;
            if (step > 300) {
                score -= 2;
            }
        }

        return score;
    }

    /**
     * Genera todas las combinaciones de k elementos de un array de tamaño n
     * Usado para búsqueda exhaustiva de escalas diatónicas (C(12, 7) = 792)
     *
     * @param {number} n - Tamaño del conjunto
     * @param {number} k - Número de elementos a seleccionar
     * @returns {Array} Array de combinaciones
     */
    getCombinations(n, k) {
        const combinations = [];

        const combine = (start, combo) => {
            if (combo.length === k) {
                combinations.push([...combo]);
                return;
            }

            for (let i = start; i < n; i++) {
                combo.push(i);
                combine(i + 1, combo);
                combo.pop();
            }
        };

        combine(0, []);
        return combinations;
    }

    /**
     * Genera una escala diatónica áurea (7 de 12) optimizando Score(S₇)
     *
     * @param {string} preset - Preset de escala ('major', 'minor', 'custom')
     * @param {Array} customIndices - Índices personalizados si preset='custom'
     * @returns {Object} {notes: Array, score: number, intervals: Array}
     */
    generateDiatonicScale(preset = 'major', customIndices = null) {
        // Si ya está en cache, retornar
        const cacheKey = preset === 'custom' ? `custom_${customIndices.join('_')}` : preset;
        if (this.diatonicScales[cacheKey]) {
            return this.diatonicScales[cacheKey];
        }

        let scale = null;
        let bestScore = -Infinity;

        if (preset === 'custom' && customIndices) {
            // Escala personalizada
            scale = customIndices.map(idx => this.chromaticScale[idx]);
            bestScore = this.scaleStabilityScore(scale);
        } else {
            // Búsqueda exhaustiva de la mejor escala
            const allCombinations = this.getCombinations(12, 7);

            for (const combo of allCombinations) {
                const candidateScale = combo.map(idx => this.chromaticScale[idx]);
                const score = this.scaleStabilityScore(candidateScale);

                if (score > bestScore) {
                    bestScore = score;
                    scale = candidateScale;
                }
            }
        }

        // Calcular intervalos entre grados consecutivos
        const intervals = [];
        for (let i = 0; i < scale.length - 1; i++) {
            intervals.push(scale[i + 1].cents - scale[i].cents);
        }
        // Añadir intervalo de vuelta a la octava
        intervals.push(1200 - scale[scale.length - 1].cents);

        const result = {
            notes: scale,
            score: bestScore,
            intervals: intervals,
            preset: preset
        };

        // Guardar en cache
        this.diatonicScales[cacheKey] = result;

        return result;
    }

    /**
     * Genera los 7 modos áureos a partir de una escala diatónica
     *
     * @param {Array} diatonicScale - Escala diatónica base
     * @returns {Array} Array de 7 modos
     */
    generateModes(diatonicScale) {
        const modes = [];

        for (let i = 0; i < 7; i++) {
            // Rotar la escala
            const mode = [];
            for (let j = 0; j < 7; j++) {
                const note = diatonicScale[(i + j) % 7];
                // Ajustar cents relativos al nuevo tónico
                let adjustedCents = note.cents - diatonicScale[i].cents;
                if (adjustedCents < 0) adjustedCents += 1200;

                mode.push({
                    ...note,
                    centsInMode: adjustedCents,
                    degree: j + 1
                });
            }

            modes.push({
                name: this.modeNames[i],
                notes: mode,
                tonic: diatonicScale[i]
            });
        }

        return modes;
    }

    /**
     * Obtiene el intervalo correspondiente a φ^k en cents
     *
     * @param {number} k - Exponente de φ
     * @returns {number} Intervalo en cents
     */
    getPhiInterval(k) {
        const interval = 1200 * Math.log2(Math.pow(this.PHI, k));
        return ((interval % 1200) + 1200) % 1200;
    }

    /**
     * Genera una tríada áurea estable
     * Notas: φ⁰, φ², φ³ (envueltos en octava)
     *
     * @param {number} rootCents - Fundamental en cents
     * @returns {Array} Array de 3 notas [cents]
     */
    generateTriad(rootCents) {
        const notes = [
            rootCents,
            (rootCents + this.getPhiInterval(2)) % 1200,
            (rootCents + this.getPhiInterval(3)) % 1200
        ];

        // Ordenar ascendentemente
        notes.sort((a, b) => a - b);

        return notes;
    }

    /**
     * Genera un acorde de séptima áureo
     * Notas: φ⁰, φ², φ³, φ⁴
     *
     * @param {number} rootCents - Fundamental en cents
     * @returns {Array} Array de 4 notas [cents]
     */
    generateSeventh(rootCents) {
        const notes = [
            rootCents,
            (rootCents + this.getPhiInterval(2)) % 1200,
            (rootCents + this.getPhiInterval(3)) % 1200,
            (rootCents + this.getPhiInterval(4)) % 1200
        ];

        // Filtrar colisiones (notas < 100 cents de distancia)
        const filtered = [notes[0]];
        for (let i = 1; i < notes.length; i++) {
            let tooClose = false;
            for (let j = 0; j < filtered.length; j++) {
                const dist = Math.abs(notes[i] - filtered[j]);
                if (dist < 100 && dist > 0) {
                    tooClose = true;
                    break;
                }
            }
            if (!tooClose) {
                filtered.push(notes[i]);
            }
        }

        filtered.sort((a, b) => a - b);
        return filtered;
    }

    /**
     * Genera un acorde de novena áureo
     * Notas: φ⁰, φ², φ³, φ⁴, φ⁵
     *
     * @param {number} rootCents - Fundamental en cents
     * @returns {Array} Array de hasta 5 notas [cents]
     */
    generateNinth(rootCents) {
        const notes = [
            rootCents,
            (rootCents + this.getPhiInterval(2)) % 1200,
            (rootCents + this.getPhiInterval(3)) % 1200,
            (rootCents + this.getPhiInterval(4)) % 1200,
            (rootCents + this.getPhiInterval(5)) % 1200
        ];

        // Filtrar colisiones
        const filtered = [notes[0]];
        for (let i = 1; i < notes.length; i++) {
            let tooClose = false;
            for (let j = 0; j < filtered.length; j++) {
                const dist = Math.abs(notes[i] - filtered[j]);
                if (dist < 100 && dist > 0) {
                    tooClose = true;
                    break;
                }
            }
            if (!tooClose) {
                filtered.push(notes[i]);
            }
        }

        filtered.sort((a, b) => a - b);
        return filtered;
    }

    /**
     * Convierte cents a frecuencia en Hz
     *
     * @param {number} cents - Altura en cents (0 = A4 = 440 Hz)
     * @param {number} baseFreq - Frecuencia base (default: 440 Hz)
     * @returns {number} Frecuencia en Hz
     */
    centsToFrequency(cents, baseFreq = null) {
        if (baseFreq === null) baseFreq = this.baseFrequency;
        return baseFreq * Math.pow(2, cents / 1200);
    }

    /**
     * Convierte frecuencia a cents
     *
     * @param {number} frequency - Frecuencia en Hz
     * @param {number} baseFreq - Frecuencia base (default: 440 Hz)
     * @returns {number} Cents
     */
    frequencyToCents(frequency, baseFreq = null) {
        if (baseFreq === null) baseFreq = this.baseFrequency;
        return 1200 * Math.log2(frequency / baseFreq);
    }

    /**
     * Voice leading óptimo entre dos acordes
     * Minimiza el movimiento total de las voces
     *
     * @param {Array} chord1 - Acorde inicial [cents]
     * @param {Array} chord2 - Acorde final [cents]
     * @returns {Object} {mapping: Array, totalMovement: number}
     */
    optimalVoiceLeading(chord1, chord2) {
        // Algoritmo greedy: asignar cada voz al destino más cercano
        const used = new Set();
        const mapping = [];
        let totalMovement = 0;

        for (let i = 0; i < chord1.length; i++) {
            let minDist = Infinity;
            let bestJ = -1;

            for (let j = 0; j < chord2.length; j++) {
                if (used.has(j)) continue;

                // Calcular distancia (considerando octavas)
                let dist = Math.abs(chord2[j] - chord1[i]);

                // También considerar movimiento con octava
                const distUp = Math.abs((chord2[j] + 1200) - chord1[i]);
                const distDown = Math.abs((chord2[j] - 1200) - chord1[i]);

                dist = Math.min(dist, distUp, distDown);

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

        return {
            mapping: mapping,
            totalMovement: totalMovement
        };
    }

    /**
     * Genera una progresión armónica áurea
     *
     * @param {Array} scale - Escala diatónica
     * @param {number} length - Longitud de la progresión
     * @param {string} type - Tipo ('simple', 'moderate', 'complex')
     * @returns {Array} Array de acordes [cents]
     */
    generateProgression(scale, length = 8, type = 'moderate') {
        const progression = [];

        // Definir cadencias funcionales áureas
        const cadences = {
            // I (tónica)
            tonic: [0, 2, 4],
            // V áureo (φ¹ o φ² desde tónica)
            dominant: [4, 6, 1],
            // IV áureo (φ⁻¹ desde tónica)
            subdominant: [3, 5, 0]
        };

        // Patrón simple: I-IV-V-I
        if (type === 'simple') {
            const pattern = ['tonic', 'subdominant', 'dominant', 'tonic'];
            for (let i = 0; i < length; i++) {
                const chordType = pattern[i % pattern.length];
                const degrees = cadences[chordType];
                const chord = degrees.map(deg => scale[deg].cents);
                progression.push({ type: chordType, notes: chord, degrees: degrees });
            }
        }
        // Patrón moderado: ii-V-I con variaciones
        else if (type === 'moderate') {
            const pattern = ['subdominant', 'dominant', 'tonic', 'tonic',
                           'subdominant', 'dominant', 'tonic', 'dominant'];
            for (let i = 0; i < length; i++) {
                const chordType = pattern[i % pattern.length];
                const degrees = cadences[chordType];
                const chord = degrees.map(deg => scale[deg % 7].cents);
                progression.push({ type: chordType, notes: chord, degrees: degrees });
            }
        }
        // Patrón complejo: movimientos por gradiente φ
        else {
            for (let i = 0; i < length; i++) {
                // Generar acorde basado en grado φ
                const degree = Math.floor(Math.random() * 7);
                const chord = this.generateTriad(scale[degree].cents);
                const chordType = i === length - 1 ? 'tonic' : 'transition';
                progression.push({ type: chordType, notes: chord, degrees: [degree, (degree + 2) % 7, (degree + 4) % 7] });
            }
            // Asegurar que termina en tónica
            progression[length - 1] = {
                type: 'tonic',
                notes: cadences.tonic.map(deg => scale[deg].cents),
                degrees: cadences.tonic
            };
        }

        return progression;
    }

    /**
     * Genera un preludio áureo de 4 voces (SATB)
     * Estilo inspirado en Bach pero en universo φ
     *
     * @param {Object} params - Parámetros {scale, measures, tempo, density, complexity}
     * @returns {Object} {soprano, alto, tenor, bass, chords, measures}
     */
    generatePrelude(params) {
        const {
            scale,
            measures = 16,
            tempo = 80,
            density = 'medium',
            complexity = 'moderate'
        } = params;

        // Fase 1: Generar progresión armónica
        const numChords = Math.max(4, Math.floor(measures / 2));
        const progression = this.generateProgression(scale.notes, numChords, complexity);

        // Fase 2: Generar bajo estructural (fundamentales)
        const bass = [];
        for (let i = 0; i < progression.length; i++) {
            const chord = progression[i];
            // Fundamental del acorde
            bass.push({
                cents: chord.notes[0],
                duration: 2, // 2 tiempos
                time: i * 2
            });
        }

        // Fase 3: Generar voces intermedias (Alto y Tenor)
        const alto = [];
        const tenor = [];

        for (let i = 0; i < progression.length; i++) {
            const chord = progression[i];

            // Alto toma la segunda o tercera nota del acorde
            alto.push({
                cents: chord.notes[Math.min(1, chord.notes.length - 1)],
                duration: 2,
                time: i * 2
            });

            // Tenor toma nota intermedia
            const tenorIdx = chord.notes.length > 2 ? 1 : 0;
            tenor.push({
                cents: chord.notes[tenorIdx],
                duration: 2,
                time: i * 2
            });
        }

        // Fase 4: Generar soprano (melodía)
        const soprano = [];

        for (let i = 0; i < progression.length; i++) {
            const chord = progression[i];

            // Soprano varía entre notas cordales y notas de paso
            if (density === 'low') {
                soprano.push({
                    cents: chord.notes[chord.notes.length - 1],
                    duration: 2,
                    time: i * 2
                });
            } else if (density === 'medium') {
                // Dos notas por acorde
                soprano.push({
                    cents: chord.notes[chord.notes.length - 1],
                    duration: 1,
                    time: i * 2
                });
                soprano.push({
                    cents: chord.notes[Math.floor(chord.notes.length / 2)],
                    duration: 1,
                    time: i * 2 + 1
                });
            } else {
                // Alta densidad: 4 notas por acorde
                for (let j = 0; j < 4; j++) {
                    const noteIdx = j % chord.notes.length;
                    soprano.push({
                        cents: chord.notes[noteIdx],
                        duration: 0.5,
                        time: i * 2 + j * 0.5
                    });
                }
            }
        }

        // Fase 5: Asegurar que termina en acorde de tónica
        const finalChord = this.generateTriad(scale.notes[0].cents);

        return {
            soprano: soprano,
            alto: alto,
            tenor: tenor,
            bass: bass,
            progression: progression,
            measures: measures,
            tempo: tempo
        };
    }

    /**
     * Verifica reglas de contrapunto áureo
     *
     * @param {Object} satb - Objeto con voces {soprano, alto, tenor, bass}
     * @returns {Array} Array de violaciones encontradas
     */
    verifyCounterpoint(satb) {
        const violations = [];

        // Regla 1: No quintas áureas paralelas (φ¹)
        const phiFifth = this.getPhiInterval(1); // ~833 cents

        for (let i = 0; i < satb.soprano.length - 1; i++) {
            const interval1 = Math.abs(satb.soprano[i].cents - satb.bass[Math.min(i, satb.bass.length - 1)].cents);
            const interval2 = Math.abs(satb.soprano[i + 1].cents - satb.bass[Math.min(i + 1, satb.bass.length - 1)].cents);

            if (Math.abs(interval1 - phiFifth) < 20 && Math.abs(interval2 - phiFifth) < 20) {
                violations.push({
                    type: 'parallel_phi_fifths',
                    location: i,
                    severity: 'high'
                });
            }
        }

        // Regla 2: Evitar saltos grandes sin resolución (> φ³)
        const maxJump = this.getPhiInterval(3);

        for (const voice of ['soprano', 'alto', 'tenor', 'bass']) {
            for (let i = 0; i < satb[voice].length - 1; i++) {
                const jump = Math.abs(satb[voice][i + 1].cents - satb[voice][i].cents);
                if (jump > maxJump) {
                    violations.push({
                        type: 'large_unresolved_jump',
                        voice: voice,
                        location: i,
                        severity: 'medium'
                    });
                }
            }
        }

        return violations;
    }

    /**
     * Analiza la consonancia global de una sonoridad (acorde)
     *
     * @param {Array} chord - Array de notas en cents
     * @returns {Object} {average: number, min: number, max: number, pairs: Array}
     */
    analyzeChordConsonance(chord) {
        const consonances = [];

        for (let i = 0; i < chord.length; i++) {
            for (let j = i + 1; j < chord.length; j++) {
                const interval = Math.abs(chord[j] - chord[i]);
                const cons = this.consonance(interval);
                consonances.push({
                    interval: interval,
                    consonance: cons,
                    notes: [i, j]
                });
            }
        }

        if (consonances.length === 0) {
            return { average: 1, min: 1, max: 1, pairs: [] };
        }

        const values = consonances.map(c => c.consonance);
        const average = values.reduce((a, b) => a + b, 0) / values.length;
        const min = Math.min(...values);
        const max = Math.max(...values);

        return {
            average: average,
            min: min,
            max: max,
            pairs: consonances
        };
    }

    /**
     * Exporta configuración actual del motor
     *
     * @returns {Object} Estado completo del motor
     */
    exportState() {
        return {
            phi: this.PHI,
            chromaticScale: this.chromaticScale,
            diatonicScales: this.diatonicScales,
            baseFrequency: this.baseFrequency,
            consonanceTolerance: this.consonanceTolerance
        };
    }

    /**
     * Importa configuración al motor
     *
     * @param {Object} state - Estado a importar
     */
    importState(state) {
        if (state.baseFrequency) this.baseFrequency = state.baseFrequency;
        if (state.consonanceTolerance) this.consonanceTolerance = state.consonanceTolerance;
        if (state.diatonicScales) this.diatonicScales = state.diatonicScales;
    }
}

// Exportar para uso en navegador o Node.js
if (typeof module !== 'undefined' && module.exports) {
    module.exports = GoldenHarmonyEngine;
}
