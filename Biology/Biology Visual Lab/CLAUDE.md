# Biology Visual Lab - Documentación Técnica

## Descripción

El **Biology Visual Lab** contiene **9 simulaciones** que cubren neurociencia, ecología, genética poblacional, biología celular y evolución. Implementa modelos científicos rigurosos con Canvas 2D y una simulación con WebAssembly para alto rendimiento.

## Simulaciones (9 Total)

### NEUROCIENCIA (1)
1. **Potencial de Acción** - Modelo de Hodgkin-Huxley completo

### ECOLOGÍA (1)
2. **Ecosistema** - Sistema depredador-presa de Lotka-Volterra

### GENÉTICA (1)
3. **Genética Poblacional** - Hardy-Weinberg + Deriva genética + Selección

### BIOLOGÍA CELULAR (2)
4. **Mitosis** - 6 fases con huso mitótico
5. **Meiosis** - 8 fases con crossing-over y segregación independiente

### EVOLUCIÓN (1)
6. **Selección Natural** - Algoritmo genético visual

### CRONOBIOLOGÍA (1)
7. **Ritmos Circadianos** - Oscilaciones biológicas de 24h

### FISIOLOGÍA (1)
8. **Visión** - Óptica ocular con defectos visuales

### COMPORTAMIENTO (1) ⭐
9. **Boids Masivo** - Algoritmo de Reynolds con WASM (10,000+ agentes)

## Modelos Científicos Implementados

### Hodgkin-Huxley (Neurona)

**Ecuaciones principales:**
```
C_m · dV/dt = I - I_ion
I_ion = g_Na·m³·h·(V-E_Na) + g_K·n⁴·(V-E_K) + g_L·(V-E_L)
```

**Parámetros:**
- `C_m = 1.0 µF/cm²` (capacitancia de membrana)
- `E_Na = +50 mV`, `E_K = -77 mV`, `E_L = -54.4 mV`
- `g_Na = 120 mS/cm²`, `g_K = 36 mS/cm²`, `g_L = 0.3 mS/cm²`

**Variables de estado:** V, m, h, n

**Funciones de velocidad:**
```javascript
alphaM(V) = 0.1 * (V + 40) / (1 - e^(-(V+40)/10))
betaM(V)  = 4 * e^(-(V+65)/18)
alphaH(V) = 0.07 * e^(-(V+65)/20)
betaH(V)  = 1 / (1 + e^(-(V+35)/10))
alphaN(V) = 0.01 * (V + 55) / (1 - e^(-(V+55)/10))
betaN(V)  = 0.125 * e^(-(V+65)/80)
```

### Lotka-Volterra (Ecosistema)

**Ecuaciones depredador-presa:**
```
dx/dt = αx - βxy    [Presas]
dy/dt = δxy - γy    [Depredadores]
```

**Parámetros:**
- `α` = Tasa reproducción presas (default 1.0)
- `β` = Tasa depredación (default 0.5)
- `γ` = Tasa muerte depredadores (default 0.5)
- `δ` = Eficiencia conversión (default 0.25)

**Integración:** RK4 de 4to orden

### Hardy-Weinberg (Genética Poblacional)

**Ecuación fundamental:**
```
p + q = 1
p² + 2pq + q² = 1

Frecuencias genotípicas:
- AA = p² (homocigoto dominante)
- Aa = 2pq (heterocigoto)
- aa = q² (homocigoto recesivo)
```

**Fuerzas evolutivas:**
1. **Selección natural:** `w_AA = 1, w_Aa = 1-h·s, w_aa = 1-s`
2. **Deriva genética:** Muestreo binomial con N individuos
3. **Mutación:** `p_new = p·(1-μ) + (1-p)·μ`

### Algoritmo Genético (Selección Natural)

**Función de fitness:**
```javascript
hueDiff = |individual.hue - envHue|
normalizedDiff = min(hueDiff, 360 - hueDiff) / 180
baseFitness = 1 - normalizedDiff
fitness = (baseFitness)^(1 + selectionStrength·3)
```

**Operadores:**
- Selección: Top (survivalRate × 100)% por fitness
- Crossover: Promedio de hues parentales
- Mutación: Variación aleatoria ±30°

### Ritmos Circadianos

**Modelos hormonales (sinusoidales):**
```javascript
getMelatonin(t) = max(0, cos((t - 3 - jetLagOffset) * π/12) * 0.5 + 0.5 - blueLightEffect * 0.5)
getTemperature(t) = cos((t - 5 - jetLagOffset - 12) * π/12) * 0.5 + 0.5
getCortisol(t) = min(1, 0.2 + max(0, 1 - |t - 8| / 4) * 0.8)
```

### Óptica Ocular (Visión)

**Ecuación de lentes:**
```
1/f = 1/d_o + 1/d_i
P = 1/f (dioptrías)
n₁ sin θ₁ = n₂ sin θ₂ (Snell)
```

**Parámetros oculares:**
- Potencia corneal = 43 D
- Potencia lente = 17-31 D (acomodación 0-14 D)
- Longitud ojo normal = 24 mm
- Miopía: 26 mm, Hipermetropía: 22 mm

### Boids (Algoritmo de Reynolds)

**Ecuación de fuerza:** F = S + A + C

**Separación (S):**
```
steer = Σ normalize(boid.pos - neighbor.pos) / distance
```

**Alineación (A):**
```
steer = avgVelocity - boid.velocity
```

**Cohesión (C):**
```
steer = centerOfMass - boid.pos
```

**Motor WASM:**
- Spatial partitioning O(n log n)
- 10,000+ partículas en tiempo real
- Aceleración ~100x vs JavaScript puro

## Métodos Numéricos

### Integración de EDOs

**RK4 (Runge-Kutta 4to orden)** - Lotka-Volterra
```javascript
k1 = f(t, y)
k2 = f(t + dt/2, y + k1*dt/2)
k3 = f(t + dt/2, y + k2*dt/2)
k4 = f(t + dt, y + k3*dt)
y_new = y + (k1 + 2k2 + 2k3 + k4) * dt/6
```
Error: O(dt⁵)

**Euler adelante** - Hodgkin-Huxley
```javascript
y_new = y + f(t, y) * dt
```
Error: O(dt²)

### Muestreo Probabilístico

**Muestreo binomial** - Deriva genética
```javascript
countA = 0
for i in [1, 2N]:
    if random() < p: countA++
p_new = countA / (2N)
```

## Tecnología

### Core
- **Canvas 2D** - 8 de 9 simulaciones
- **JavaScript ES6+** - Vanilla (sin frameworks)
- **CSS3** - Tailwind CDN + custom inline
- **WebAssembly** - Motor C++ para Boids (158 KB)

### Features Canvas
- Device Pixel Ratio handling para Retina
- `requestAnimationFrame` para 60 FPS
- Gradientes radiales/lineales
- Transformaciones 2D

## Paleta de Colores

- **Primaria**: Verde `#10b981` (Emerald-400)
- **Neurona**: Violeta `#8b5cf6`
- **Ecosistema**: Esmeralda `#10b981`
- **Genética**: Cyan `#06b6d4`
- **Mitosis/Meiosis**: Rosa `#f43f5e`
- **Evolución**: Ámbar `#f59e0b`
- **Circadianos**: Índigo `#4f46e5`
- **Visión**: Cielo `#0ea5e9`
- **Boids**: Teal `#10b981`

## Referencias

**Archivos de código:**
- Neurona: `neurona.html:250-310` (rate functions)
- Ecosistema: `ecosistema.html:420-480` (RK4 implementation)
- Boids: `eigenlab-core.wasm` (C++ compiled)

**Total:** 9 simulaciones, ~7,900 líneas HTML/CSS/JS + 158 KB WASM

---

**Última actualización:** 2026-01-10
