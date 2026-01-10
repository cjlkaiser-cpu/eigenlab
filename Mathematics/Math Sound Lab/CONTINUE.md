# Prompt de Continuación: Math Sound Lab - EigenLab

## Contexto del Proyecto

Estás trabajando en **EigenLab**, un ecosistema de laboratorios virtuales educativos con simulaciones científicas y matemáticas interactivas en HTML/CSS/JavaScript vanilla.

**Repositorio**: https://github.com/cjlkaiser-cpu/eigenlab
**Directorio local**: `/Users/carlos/Projects/EigenLab/`

## Estado Actual: Math Sound Lab

Acabamos de crear el **Math Sound Lab** (`Mathematics/Math Sound Lab/`), una nueva rama que sonifica patrones matemáticos siguiendo el patrón de Physics (Visual Lab + Sound Lab).

### Implementación Completada

#### 1. Game of Life Musical (`game-of-life-musical.html`)
✅ Instrumento musical generativo basado en autómata celular de Conway
- **Escala**: Pentatónica menor/mayor seleccionable
- **Mapeo**: Filas → Notas (5 notas cíclicas), Columnas → Octavas (C3-C7)
- **Síntesis**: Web Audio API con ADSR envelopes + filtro lowpass
- **Modulación**: Edad→Duración, Vecinos→Volumen (ajustables)
- **Formas de onda**: Sine, Triangle, Square, Sawtooth
- **Efectos visuales**: Glow effect en células que suenan
- **Optimización**: Polifonía limitada (64 voces), Map() para gestión O(1)

#### 2. Index del Laboratorio
✅ Página principal del Math Sound Lab con grid de simulaciones
✅ Preview animado del Game of Life Musical
✅ Placeholder para futuras simulaciones

#### 3. Portal Actualizado
✅ Nueva tarjeta "Math Sound Lab" en `_portal/index.html`
✅ Contador actualizado: 4 laboratorios
✅ Preview canvas animado con cellular automaton + waveform

### Archivos Clave

```
Mathematics/Math Sound Lab/
├── index.html                        # Grid de simulaciones
└── game-of-life-musical.html         # Instrumento pentatónico

Referencia para patrones de audio:
├── Physics/Physics Sound Lab/generativos/chromatic-emission/js/audio.js
├── Astronomy/Astronomy Sound Lab/pulsares.html
└── Astronomy/Astronomy Sound Lab/musica-esferas-moderna.html
```

### Convenciones del Proyecto

- **Idioma UI**: Español
- **Idioma código**: Inglés (variables, funciones, comentarios)
- **Archivos**: kebab-case (`game-of-life-musical.html`)
- **Color Math Labs**: `#f97316` (orange)
- **Estructura**: HTML autocontenido, sin bundler
- **Audio**: Web Audio API nativa, lazy init (iOS/Safari compatible)

## Próximos Pasos Sugeridos

### Nuevas Simulaciones para Math Sound Lab:

1. **Fractales Sonoros**
   - Mandelbrot/Julia con síntesis FM
   - Mapeo: profundidad de iteración → frecuencia
   - Coordenadas complejas → timbre

2. **Atractor de Lorenz Musical**
   - Trayectorias caóticas → melodías
   - Ejes X,Y,Z → parámetros sonoros (pitch, volumen, pan)
   - Visualización 3D con Three.js

3. **Números Primos como Ritmos**
   - Secuencias de primos → patterns rítmicos
   - Distancia entre primos → duración de notas
   - Test de primalidad visual + sonoro

4. **Autómatas Celulares 1D Musicales**
   - Rule 30, Rule 110 sonificados
   - Cada columna = paso temporal
   - Filas = notas en escala

5. **Transformada de Fourier Interactiva**
   - Síntesis aditiva en tiempo real
   - Dibujar forma de onda → escuchar resultado
   - Espectro de frecuencias visual

6. **Cadenas de Markov Generativas**
   - Composición algorítmica
   - Matriz de probabilidades de transición
   - Training con melodías conocidas

### Tareas Técnicas Pendientes:

- [ ] Agregar guías (`Mathematics/Math Sound Lab/guides/`)
- [ ] Actualizar `CLAUDE.md` con info de Math Sound Lab
- [ ] Crear presets musicales guardables (localStorage)
- [ ] Implementar export WAV/MIDI del Game of Life
- [ ] Optimizar para móvil (touch gestures)
- [ ] Agregar más escalas (dórica, frigia, cromática)

## Cómo Continuar

**Para agregar nueva simulación:**
1. Usar plantilla: `cp _templates/simulation-2d.html Mathematics/Math\ Sound\ Lab/nueva-sim.html`
2. Cambiar `--accent` a `#f97316`
3. Implementar lógica matemática + audio
4. Agregar preview al `index.html` del lab
5. Actualizar `_portal/index.html` (incrementar badge count)
6. Commit con mensaje: `feat(math-sound): add [nombre-simulación]`

**Archivos de referencia para Web Audio:**
- ADSR: `chromatic-emission/js/audio.js:111-138`
- Edge detection: `musica-esferas-moderna.html:573-583`
- Lazy init: `pulsares.html:259-262`

## Último Commit

```
7b2c8f2 feat: add Math Sound Lab with Game of Life Musical
```

## Instrucciones para Nueva Sesión

Cuando inicies una nueva sesión de Claude Code, copia y pega este archivo completo en el chat. Claude recordará todo el contexto y podrá continuar exactamente donde lo dejaste.

**Inicio de sesión sugerido:**
```
Hola, quiero continuar trabajando en Math Sound Lab de EigenLab.
[Pegar contenido de este archivo]
¿En qué simulación trabajamos hoy?
```
