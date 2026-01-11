# Astronomy Visual Lab - Documentación Técnica

## Descripción

El **Astronomy Visual Lab** contiene **9 simulaciones** que cubren relatividad general, cosmología, mecánica orbital, astrofísica estelar y formación galáctica. Implementa modelos astrofísicos rigurosos con Canvas 2D y WebAssembly para simulaciones N-body.

## Simulaciones (9 Total)

### RELATIVIDAD GENERAL (2)
1. **Agujero Negro** - Métrica de Schwarzschild, órbitas relativistas
2. **Lentes Gravitacionales** - Anillo de Einstein, magnificación

### COSMOLOGÍA (1)
3. **Expansión del Universo** - Ley de Hubble, redshift, galaxias

### MECÁNICA ORBITAL (3)
4. **Fases Lunares** - Geometría Sol-Tierra-Luna, período sinódico
5. **Las Estaciones** - Órbita elíptica + oblicuidad de 23.44°
6. **Anillos de Saturno** - Orbitas Keplerianas, División de Cassini (WASM) ⭐

### ASTROFÍSICA ESTELAR (2)
7. **Diagrama H-R** - Clasificación estelar, 300+ estrellas generadas
8. **Nucleosíntesis Estelar** - Estructura de cebolla, fusión nuclear

### FORMACIÓN GALÁCTICA (1)
9. **Formación de Galaxias** - Algoritmo N-body Barnes-Hut (WASM)

## Modelos Astrofísicos

### Agujero Negro (Schwarzschild)

**Métrica:**
```
r_s = 2GM/c²  (Radio de Schwarzschild)
ds² = -(1-r_s/r)dt² + (1-r_s/r)⁻¹dr² + r²dΩ²
```

**Correcciones post-Newtonianas:**
```
Corrección = 3ML²/(r⁵) · (r̂)  [precesión del perihelio]
```

**Escalas importantes:**
- Horizonte de eventos: r = r_s
- Esfera de fotones: r = 1.5r_s
- ISCO (órbita circular estable): r = 3r_s = 6M

**Integración:** Velocity Verlet con correcciones relativistas

### Lentes Gravitacionales

**Ecuaciones:**
```
β = θ - α(θ)  [Ecuación de la lente]
θ_E = √(4GM D_LS/(c² D_L D_S))  [Radio de Einstein]
α = θ_E²/θ  [Ángulo de desviación]
```

**Posiciones de imágenes:**
```
θ² - βθ - θ_E² = 0
θ = (β ± √(β² + 4θ_E²))/2
```

**Magnificación:**
```
μ = θ/β · dθ/dβ = θ⁴/(θ⁴ - θ_E⁴)
```

### Expansión del Universo

**Ley de Hubble-Lemaître:**
```
v = H₀ · d
z = v/c  [redshift]
a(t) = escala factor de FLRW
```

**Parámetros:**
- H₀ = 67 km/s/Mpc (CMB) o 73 km/s/Mpc (Local)
- 50-200 galaxias con estructura (espirales, elípticas)
- Color por redshift (azul → rojo)

### Fases Lunares

**Geometría:**
```
P_syn = 29.53 días  (Período sinódico)
1/P_syn = 1/P_sid - 1/P_⊕
θ_luna = (día/P_syn) × 2π
φ = π - θ_luna  [ángulo de fase]
Iluminación = (1 + cos(φ))/2
```

**8 fases:** Nueva, Creciente, Cuarto Creciente, Gibosa Creciente, Llena, etc.

### Las Estaciones

**Ecuaciones:**
```
ε = 23.44°  (Oblicuidad de la eclíptica)
δ = ε · sin(360° × (d-80)/365)  [Declinación solar]
Horas luz = (2H/π) × 12
donde cos(H) = -tan(φ)tan(δ)
```

**Eventos anuales:**
- Equinoccio primavera: día 80 (21 Mar)
- Solsticio verano: día 172 (21 Jun)
- Equinoccio otoño: día 266 (23 Sep)
- Solsticio invierno: día 355 (21 Dic)

### Diagrama H-R

**Ecuaciones:**
```
L = 4πR²σT⁴  [Stefan-Boltzmann]
M_abs = M_☉ - 2.5 log(L/L_☉)
L ∝ M^3.5  [Relación masa-luminosidad, secuencia principal]
```

**6 estrellas famosas con datos precisos:**
- Sol (G2V, 5778 K, 1 L☉, 4.6 Gyr)
- Sirio A (A1V, 9940 K, 25.4 L☉)
- Betelgeuse (M1-2Ia, 3500 K, 126,000 L☉)
- Rigel (B8Ia, 12,100 K, 120,000 L☉)
- Próxima Centauri (M5.5Ve, 3042 K, 0.0017 L☉)
- Polaris (F7Ib, 6015 K, 1260 L☉)

**Generación procedural:** 300+ estrellas (200 secuencia principal, 50 gigantes, 20 supergigantes, 30 enanas blancas)

### Nucleosíntesis Estelar

**Cadenas de fusión:**
```
Cadena p-p:  4¹H → ⁴He + 2e⁺ + 2νₑ + γ   ΔE = 26.7 MeV
Ciclo CNO:   4¹H → ⁴He (catalizador C,N,O)
Triple-α:    3⁴He → ¹²C + γ               ΔE = 7.27 MeV
```

**Estructura de cebolla (pre-supernova):**
- Fe core (3000 MK)
- Si → Fe layer
- O → Si,S layer
- Ne → O,Mg layer
- C → Ne,Mg layer
- He → C,O layer
- H → He shell

**19 elementos** con origen codificado (Big Bang, estelar, s-process, r-process)

### Anillos de Saturno ⭐

**Ecuaciones Keplerianas:**
```
v = √(GM/r)  [Velocidad orbital circular]
T = 2π√(r³/GM)  [Período orbital]
```

**Estructura de anillos (en radios de Saturno):**
- D Ring: 1.11-1.24 Rs (tenue)
- C Ring: 1.24-1.53 Rs
- B Ring: 1.53-1.95 Rs (el más denso)
- **División de Cassini**: 1.95-2.02 Rs (resonancia 2:1 con Mimas)
- A Ring: 2.02-2.27 Rs
- **Gap de Encke**: ~2.21 Rs

**Motor WASM:**
- 5000+ partículas de hielo/roca
- Colisiones elásticas con damping
- Lunas pastoras (Mimas, perturbaciones gravitacionales)
- Composición: 80% hielo, 20% roca

**Presets:**
1. Realista - Proporciones reales de Saturno
2. Denso - 10,000 partículas
3. Anillos Amplios - Sistema extendido
4. Anillos Finos - Solo anillos principales

### Formación de Galaxias

**Algoritmo Barnes-Hut:** O(n log n) vs O(n²)

**Ecuaciones:**
```
F = G m1 m2 / r²
Softening: r_eff = √(r² + ε²)
Criterio theta: θ = cell_size / distance
```

**Motor WASM:**
- Árbol cuadtree para aproximación multipolo
- 1000-20,000 estrellas renderizadas
- Parámetro theta (0.3-1.0): precisión/velocidad

**Presets:**
1. Galaxia espiral (disco de rotación)
2. Galaxia elíptica (distribución 3D)
3. Colisión de galaxias (tidal disruption)
4. Cúmulo estelar (relajación dinámica)

## Métodos Numéricos

### Velocity Verlet Integration

```javascript
x(t) = x(t-dt) + v(t-dt)·dt + 0.5·a(t-dt)·dt²
v(t) = v(t-dt) + 0.5·(a(t-dt) + a(t))·dt
```

Ventajas: Reversible, conserva energía, orden 2

### Métodos Analíticos

**Lentes Gravitacionales:** Solución cuadrática
```javascript
const discriminant = beta * beta + 4 * thetaE * thetaE
theta1 = (beta + sqrt(discriminant)) / 2
theta2 = (beta - sqrt(discriminant)) / 2
```

**Fases Lunares:** Iluminación analítica
```javascript
phaseAngle = getPhaseAngle()
illumination = (1 + cos(phaseAngle)) / 2
```

### Barnes-Hut Tree

```javascript
function computeForce(particle, node) {
    const distance = distance(particle, node.center)
    const theta = node.size / distance
    
    if (theta < THETA) {
        // Usar masa total del nodo
        return forceFromMass(particle, node.totalMass, node.center)
    }
    
    // Recursivamente evaluar subnodos
    return sum(node.children.map(child => computeForce(particle, child)))
}
```

Complejidad: O(n log n)

## Tecnología

### Core Stack

- **Rendering:** Canvas 2D (todas las simulaciones)
- **Styling:** Tailwind CSS (CDN) + custom CSS
- **JavaScript:** Vanilla ES6+
- **WASM:** Emscripten (formacion-galaxias.html)
- **Tipografía:** Inter, JetBrains Mono

### WebAssembly

**Archivo:** `eigenlab-core.wasm` (158 KB)

**Funciones expuestas:**
```javascript
const simulator = new Module.GalaxySimulator(config)
simulator.initializeDiskGalaxy(count, center, radius, massRatio)
simulator.setGravitationalConstant(G)
simulator.setSofteningLength(epsilon)
simulator.setTheta(theta)  // Barnes-Hut threshold
```

Ventaja: ~100× más rápido que JavaScript puro

## Paleta de Colores

- **Primaria:** Índigo `#6366f1`
- **Temperatura estelar:** Blackbody (rojo → blanco → azul)
- **Redshift:** Azul (recesión negativa) → Rojo (recesión positiva)

## Referencias

**Archivos de código:**
- Agujero Negro: `agujero-negro.html:250-300` (correcciones relativistas)
- H-R Diagram: `diagrama-hr.html:400-450` (generación procedural)
- Formación Galaxias: `eigenlab-core.wasm` (C++ compilado)

**Total:** 8 simulaciones, ~6,981 líneas HTML/CSS/JS + 158 KB WASM

---

**Última actualización:** 2026-01-10
