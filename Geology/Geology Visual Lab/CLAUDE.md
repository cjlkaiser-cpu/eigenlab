# Geology Visual Lab - Documentación Técnica

## Descripción

El **Geology Visual Lab** contiene **8 simulaciones** que cubren sismología, vulcanología, tectónica de placas, ciclo de rocas, erosión, estratigrafía y magmatismo global. Implementa modelos geológicos rigurosos con visualización en Canvas 2D.

## Simulaciones (8 Total)

### SISMOLOGÍA (1)
1. **Ondas Sísmicas** - P, S, superficiales + triangulación

### VULCANOLOGÍA (1)
2. **Erupciones Volcánicas** - 3 tipos (escudo, estratovolcán, caldera) con VEI scale

### TECTÓNICA (1)
3. **Tectónica de Placas** - Pangea, 6 placas, 3 tipos de bordes

### CICLOS GEOLÓGICOS (2)
4. **Ciclo de las Rocas** - Ígnea ↔ Sedimentaria ↔ Metamórfica
5. **Erosión y Deposición** - 4 tipos (hídrica, eólica, glaciar, costera)

### ESTRATIGRAFÍA (1)
6. **Columna Estratigráfica** - Datación radiométrica, principios de superposición

### AVANZADAS (2) ⭐
7. **Erosión Hidráulica (WASM)** - Simulación física acelerada
8. **Magmatismo Global** - Distribución tectónica del magmatismo (3,849 líneas)

## Modelos Geológicos

### Ondas Sísmicas

**Capas Terrestres:**
```javascript
Inner Core:   1220 km, vP=11 km/s, vS=3.6 km/s
Outer Core:   3480 km, vP=9 km/s, vS=0 (líquido)
Lower Mantle: 5700 km, vP=12 km/s, vS=6 km/s
Upper Mantle: 6270 km, vP=8 km/s, vS=4.5 km/s
Crust:        6371 km, vP=6 km/s, vS=3.5 km/s
```

**Triangulación sísmica:**
```
Δt = tS - tP
D = Δt × (vP·vS)/(vP-vS)
```

### Erupciones Volcánicas

**Tipos de volcanes:**

**1. Volcán Escudo (Basalto):**
- Viscosidad: 0.2 (fluido)
- Gas: 1%, VEI: 0
- Slope: 0.15 (pendientes suaves)
- Temp: 1200°C, Sílice: 45%

**2. Estratovolcán (Andesita):**
- Viscosidad: 0.65 (viscoso)
- Gas: 3%, VEI: 3
- Slope: 0.5 (cono clásico)
- Temp: 950°C, Sílice: 60%

**3. Caldera (Riolita):**
- Viscosidad: 0.9 (muy viscoso)
- Gas: 5%, VEI: 6-8
- Profundidad cámara: 12 km
- Temp: 800°C, Sílice: 75%

**Ecuación VEI:**
```
VEI = baseVEI + log₁₀(gas+1)×2 + viscosity×3 + (1200-temp)/100 + pressure/100
```
Rango: 0-8 (8 = catástrofe global)

**Fases eruptivas:**
1. Intrusión (0-10%)
2. Desgasificación (10-20%)
3. Fragmentación (20-30%)
4. Columna Eruptiva (30-70%)
5. Colapso (70-90%)

### Tectónica de Placas

**6 Placas principales:**
- North America: vx=-0.5 cm/año
- South America: vx=-0.3 cm/año
- Eurasia: vx=+0.2 cm/año
- Africa: vx=+0.3 cm/año
- Pacific: vx=-0.8 cm/año (más rápida)
- Australia: vx=+0.5 cm/año

**Bordes:**
1. **Divergente:** Separación, magma ascendente, dorsal oceánica
2. **Convergente:** Subducción, arco volcánico, montañas
3. **Transformante:** Deslizamiento lateral, fallas (San Andrés)

**Pangea interpolation:**
```
pangeaFactor = state.time / 250 Ma
```
0 Ma = Presente, 250 Ma = Pangea completa

### Ciclo de las Rocas

**Tres tipos:**
1. **Ígneas** (rojo) - Enfriamiento de magma
2. **Sedimentarias** (ámbar) - Compactación de depósitos
3. **Metamórficas** (púrpura) - Presión + calor

**Transiciones:**
```
Ígnea → erosión → Sedimentaria → metamorfismo → Metamórfica → fusión → Magma → Ígnea
```

### Erosión

**Ecuación Universal de Pérdida de Suelo:**
```
E = K · R · S · L · C

E: erosion rate (ton/ha/año)
K: soil erodibility
R: rainfall erosivity
S: slope steepness
L: slope length
C: cover-management
```

**Tipos:**
- **Hídrica** (azul): Agua (ríos, lluvia) → valles, cañones
- **Eólica** (ámbar): Viento → dunas
- **Glaciar** (cyan): Hielo → valles en U
- **Costera** (cielo): Olas → acantilados, playas

### Estratigrafía

**Datación radiométrica:**
```
N = N₀ · e^(-λt)

N: núcleos restantes
N₀: cantidad inicial
λ: constante de decaimiento
t: tiempo
```

**Principios:**
1. **Superposición:** Capas más jóvenes arriba
2. **Horizontalidad:** Depósitos originales horizontales
3. **Continuidad:** Capas se extienden hasta bordes
4. **Sucesión faunística:** Fósiles guía por edad

## Métodos Numéricos

### Integración de EDOs

**Euler Simple:**
```javascript
// Partículas con gravedad
p.y += p.vy
p.vy += gravity * mass
p.vx *= airResistance
p.vy *= airResistance
```

### Trigonometría y Geometría

**Rayos sísmicos:**
```javascript
angle * EARTH_RADIUS  // Estaciones en arco
```

**Movimiento de placas:**
```javascript
offsetX = plate.vx * pangeaFactor * w * 0.3
```

### Exponenciales y Logaritmos

**Temperatura → Color:**
```javascript
if (temp > 1100) {
    r = 255; g = 255; b = 200; // Muy caliente
} else if (temp > 900) {
    r = 255; g = 180 + (temp - 900) / 5;
}
```

**VEI (logarítmica):**
```javascript
gasFactor = Math.log10(state.gas + 1) * 2
vei = baseVEI + gasFactor + viscosityFactor + ...
```

## Tecnología

### Stack Principal

- **Renderizado:** Canvas 2D (todas excepto erosion-hidraulica)
- **Estilos:** Tailwind CSS CDN + custom inline
- **Tipografía:** Inter, JetBrains Mono
- **Scripting:** Vanilla JavaScript ES6+
- **WASM:** eigenlab-core.wasm (erosion-hidraulica)

### Canvas 2D Features

```javascript
// Transformaciones
ctx.translate(x, y)
ctx.rotate(angle)
ctx.scale(sx, sy)

// Gradientes
ctx.createRadialGradient(x0, y0, r0, x1, y1, r1)

// Clipping
ctx.beginPath()
ctx.arc(x, y, r, 0, Math.PI * 2)
ctx.clip()

// Sombras y Glow
ctx.shadowColor = '#ef4444'
ctx.shadowBlur = 20

// Bezier curves
ctx.bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x2, y2)
```

## Paleta de Colores

- **Primaria:** Ámbar `#d97706`
- **Magma:** Rojo/Naranja gradient
- **Placas:** Verde escala (6 tonos)
- **Rocas:** Rojo (ígnea), Ámbar (sedimentaria), Púrpura (metamórfica)

## Referencias

**Archivo más grande:** `magmatismo-global.html` (3,849 líneas)
**Total:** 8 simulaciones, ~13,474 líneas de código

---

**Última actualización:** 2026-01-10
