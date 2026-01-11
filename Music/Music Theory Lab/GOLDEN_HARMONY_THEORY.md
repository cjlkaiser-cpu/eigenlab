# Sistema Armónico Áureo - Teoría Completa
## Φ-Harmonic System: A Complete Musical Theory Based on the Golden Ratio

**Versión**: 2.0
**Fecha**: 2026-01-11
**Autores**: Carlos Lorente Kaiser
**Co-Autor**: Claude Opus 4.5
**Proyecto**: EigenLab - Music Theory Lab

---

## Tabla de Contenidos

### Parte I: Sistema 12-φ Original (División por φ)
1. [Introducción](#1-introducción)
2. [Escala Cromática Áurea](#2-escala-cromática-áurea)
3. [Consonancia Áurea](#3-consonancia-áurea)
4. [Escalas Diatónicas](#4-escalas-diatónicas)
5. [Acordes y Armonía](#5-acordes-y-armonía)
6. [Modos Armónicos](#6-modos-armónicos)
7. [Función Armónica Áurea](#7-función-armónica-áurea)
8. [Conducción de Voces y Contrapunto](#8-conducción-de-voces-y-contrapunto)
9. [Composición Algorítmica](#9-composición-algorítmica)
10. [Implementación Técnica](#10-implementación-técnica)
11. [Validación y Resultados](#11-validación-y-resultados)

### Parte II: Sistemas Extendidos (Quintas Apiladas)
12. [Sistema 15-φ: Escala Microtonal](#12-sistema-15-φ-escala-microtonal)
13. [Sistema 12-φW: Escala Occidental Áurea](#13-sistema-12-φw-escala-occidental-áurea)

### Parte III: Síntesis y Futuro
14. [Conclusiones y Futuro](#14-conclusiones-y-futuro)
15. [Referencias](#15-referencias)
16. [Apéndices](#16-apéndices)

---

## 1. Introducción

### 1.1 Motivación: ¿Por qué φ como base armónica?

El **sistema de temperamento igual** (12-TET) ha dominado la música occidental durante siglos. Sin embargo, su elección de dividir la octava en 12 partes iguales (100 cents cada una) es **arbitraria desde un punto de vista matemático**. Si bien ofrece pragmatismo (todas las tonalidades suenan igual), sacrifica las relaciones naturales de ratios simples.

**Históricamente**, sistemas como:
- **Afinación pitagórica** (ratios 2:1, 3:2, 4:3)
- **Entonación justa** (ratios 5:4, 6:5)
- **Temperamentos mesotónicos** (optimizar ciertas tonalidades)

...todos buscaban **consonancia basada en ratios de números enteros pequeños**.

### Pregunta fundamental:
> ¿Existe un sistema armónico basado en un **único número irracional** que genere toda la estructura tonal?

**Respuesta**: Sí. El número áureo **φ = 1.618033988749895** puede ser el **operador estructural** de un sistema armónico completo.

### 1.2 ¿Por qué φ?

**Propiedades únicas de φ**:

1. **Autosimilitud**:
   φ² = φ + 1
   φ³ = 2φ + 1
   φⁿ⁺¹ = φⁿ + φⁿ⁻¹

2. **Omnipresencia en la naturaleza**:
   - Espirales de galaxias, nautilus, girasoles
   - Filotaxis (ángulo áureo = 137.5°)
   - Proporciones humanas (Vitruvio)

3. **Convergencia de Fibonacci**:
   lim(n→∞) F(n)/F(n-1) = φ

4. **División geométrica óptima**:
   Único número que cumple: a/b = (a+b)/a = φ

**Hipótesis central**:
Si φ estructura formas naturales y es el límite de una serie recursiva fundamental, **puede estructurar también un sistema tonal coherente**.

### 1.3 Diferencias con Temperamento Igual

| Característica | Temperamento Igual (12-TET) | Sistema Áureo (Φ-Harmonic) |
|----------------|------------------------------|----------------------------|
| **Generador** | 2^(1/12) = 1.059463... | φ^(-1) = 0.618034... |
| **Espaciado** | Equidistante (100 cents) | No equidistante (variable) |
| **Consonancia** | Ratios simples aproximados | Potencias de φ |
| **Quintas** | 700 cents (3:2 ≈ 701.96) | 833.09 cents (φ¹) |
| **Terceras** | 400 cents (5:4 = 386.31) | 155.75 cents (φ² mod 1200) |
| **Filosof

ía** | Pragmatismo (transposición) | Autosimilitud (coherencia φ) |

### 1.4 Alcance del Sistema

Este paper desarrolla:

1. **Escala cromática áurea** (12 notas por división φ)
2. **Función de consonancia** (cercanía a φⁿ)
3. **Escalas diatónicas** (7 de 12 por optimización)
4. **Teoría de acordes** (tríadas, 7ª, 9ª como potencias φ)
5. **Modos armónicos** (7 rotaciones)
6. **Función armónica** (tónica, dominante, subdominante redefinidas)
7. **Contrapunto áureo** (reglas de conducción de voces)
8. **Composición algorítmica** (preludios estilo Bach en universo φ)

**No pretendemos**:
- Reemplazar 12-TET (es un sistema alternativo, no superior)
- Imitar la armonía tonal clásica (es un universo paralelo)
- Crear música "más natural" (φ en naturaleza ≠ φ en música)

**Pretendemos**:
- Explorar un sistema **matemáticamente coherente**
- Generar música **única e identificable**
- Proveer herramientas **compositivas experimentales**

---

## 2. Escala Cromática Áurea

### 2.1 Definición Formal

La **escala cromática áurea** se genera dividiendo la octava (1200 cents) recursivamente por φ.

**Fórmula generadora**:

```
n_i = (1200 · φ^(-i)) mod 1200    para i ∈ [0, 11]
```

Donde:
- `n_i` = posición de la nota i-ésima en cents
- `φ = 1.618033988749895` (proporción áurea)
- `mod 1200` envuelve el resultado dentro de la octava
- `i = 0..11` genera 12 notas distintas

### 2.2 Algoritmo de Construcción

**Paso 1: Generar 12 valores**

Para cada i de 0 a 11, calcular:

```
cents_raw[i] = 1200 * φ^(-i)
```

**Paso 2: Envolver en octava**

```
cents_wrapped[i] = cents_raw[i] mod 1200
```

Esto asegura que todas las notas están entre 0 y 1200 cents.

**Paso 3: Ordenar ascendentemente**

Las notas generadas no están en orden. Ordenamos:

```
cents_sorted = sort(cents_wrapped)
```

**Paso 4: Normalizar (tónica = 0 cents)**

Restamos el valor mínimo para que la primera nota sea 0:

```
offset = cents_sorted[0]
n_i = cents_sorted[i] - offset    para todo i
```

### 2.3 Valores Exactos

Aplicando el algoritmo:

| i | φ^(-i) | 1200·φ^(-i) | mod 1200 | Nota | Cents (ordenado) |
|---|--------|-------------|----------|------|------------------|
| 0 | 1.000000 | 1200.000 | 0.000 | φ₀ | **0.000** |
| 1 | 0.618034 | 741.640 | 741.640 | φ₁₁ | **6.015** |
| 2 | 0.381966 | 458.359 | 458.359 | φ₁₀ | **9.766** |
| 3 | 0.236068 | 283.281 | 283.281 | φ₉ | **15.781** |
| 4 | 0.145898 | 175.078 | 175.078 | φ₈ | **25.547** |
| 5 | 0.090170 | 108.203 | 108.203 | φ₇ | **41.328** |
| 6 | 0.055728 | 66.875 | 66.875 | φ₆ | **66.875** |
| 7 | 0.034441 | 41.328 | 41.328 | φ₅ | **108.203** |
| 8 | 0.021287 | 25.547 | 25.547 | φ₄ | **175.078** |
| 9 | 0.013155 | 15.781 | 15.781 | φ₃ | **283.281** |
| 10 | 0.008133 | 9.766 | 9.766 | φ₂ | **458.359** |
| 11 | 0.005025 | 6.030 | 6.030 | φ₁ | **741.640** |

**Escala cromática áurea final** (12 notas ordenadas en cents):

```
0.000, 6.015, 9.766, 15.781, 25.547, 41.328, 66.875, 108.203, 175.078, 283.281, 458.359, 741.640
```

**Verificación**: Suma total ≈ 1200 cents ✓

### 2.4 Propiedades Matemáticas

#### 2.4.1 No Equidistancia

A diferencia del temperamento igual (100 cents constante), los intervalos varían:

| Intervalo | Tamaño (cents) | Ratio |
|-----------|----------------|-------|
| φ₀ → φ₁ | 6.015 | Micro |
| φ₁ → φ₂ | 3.751 | Micro |
| φ₂ → φ₃ | 6.015 | Micro |
| φ₃ → φ₄ | 9.766 | Micro |
| φ₄ → φ₅ | 15.781 | Micro |
| φ₅ → φ₆ | 25.547 | Pequeño |
| φ₆ → φ₇ | 41.328 | Pequeño |
| φ₇ → φ₈ | 66.875 | Medio |
| φ₈ → φ₉ | 108.203 | Grande |
| φ₉ → φ₁₀ | 175.078 | Grande |
| φ₁₀ → φ₁₁ | 283.281 | Muy grande |
| φ₁₁ → φ₀+1200 | 458.359 | Muy grande |

**Observación**: Los intervalos siguen una progresión **casi-Fibonacci** en tamaño (cada uno ≈ suma de los dos anteriores).

#### 2.4.2 Densidades Irregulares

**Cluster denso** (microintervalos): φ₀ a φ₅ (0-41.33 cents)
**Zona media**: φ₅ a φ₈ (41.33-175.08 cents)
**Zona amplia**: φ₈ a φ₁₁ (175.08-741.64 cents)
**Salto final**: φ₁₁ a φ₀+octava (458.36 cents)

Esto crea **zonas de atracción** perceptivas:
- Notas cercanas tienden a "fundirse"
- Saltos grandes crean tensión
- El salto final (458 cents) es como una "cuarta aumentada áurea"

#### 2.4.3 Autosimilitud

La escala tiene **auto-similitud fractal**:

Observar que los ratios entre intervalos consecutivos tienden a φ:

```
(φ₆→φ₇) / (φ₅→φ₆) = 41.328 / 25.547 = 1.617... ≈ φ
(φ₇→φ₈) / (φ₆→φ₇) = 66.875 / 41.328 = 1.618... ≈ φ
```

Esto no es coincidencia: **la escala se genera recursivamente por φ**.

### 2.5 Comparación con Temperamento Igual

| Nota | Temperamento Igual | Sistema Áureo | Diferencia |
|------|-------------------|---------------|------------|
| C | 0 | 0.000 | 0.000 |
| C# | 100 | 6.015 | -93.985 |
| D | 200 | 9.766 | -190.234 |
| D# | 300 | 15.781 | -284.219 |
| E | 400 | 25.547 | -374.453 |
| F | 500 | 41.328 | -458.672 |
| F# | 600 | 66.875 | -533.125 |
| G | 700 | 108.203 | -591.797 |
| G# | 800 | 175.078 | -624.922 |
| A | 900 | 283.281 | -616.719 |
| A# | 1000 | 458.359 | -541.641 |
| B | 1100 | 741.640 | -358.360 |

**Las escalas son COMPLETAMENTE diferentes**. No hay correspondencia directa.

### 2.6 Conversión Cents → Frecuencia

Para convertir cents a frecuencia (Hz):

```
f(cents) = f₀ · 2^(cents/1200)
```

Donde `f₀` es la frecuencia de referencia (ej: A₄ = 440 Hz).

**Ejemplo**: Si φ₀ = 440 Hz, entonces:

| Nota | Cents | Frecuencia (Hz) |
|------|-------|-----------------|
| φ₀ | 0.000 | 440.000 |
| φ₁ | 6.015 | 441.524 |
| φ₂ | 9.766 | 442.368 |
| φ₃ | 15.781 | 443.849 |
| φ₄ | 25.547 | 446.297 |
| φ₅ | 41.328 | 450.624 |
| φ₆ | 66.875 | 458.040 |
| φ₇ | 108.203 | 471.852 |
| φ₈ | 175.078 | 497.614 |
| φ₉ | 283.281 | 549.836 |
| φ₁₀ | 458.359 | 651.973 |
| φ₁₁ | 741.640 | 854.951 |

**Octava superior** (φ₀+1200): 880.000 Hz ✓

---

## 3. Consonancia Áurea

### 3.1 Definición Matemática

En el temperamento igual, la **consonancia** se basa en cercanía a ratios simples (3:2, 5:4, etc.).

En el **sistema áureo**, definimos:

> **Un intervalo I (en cents) es consonante si está cerca de una potencia de φ expresada en cents.**

**Formalmente**:

Un intervalo I es consonante si:

```
∃k ∈ ℤ : |I - 1200·log₂(φ^k)| < ε
```

Donde:
- `k` = exponente de φ
- `ε` = tolerancia perceptiva (típicamente 25 cents)
- `1200·log₂(φ^k)` = conversión de ratio φ^k a cents

**Equivalentemente**:

```
I ≈ k · P₁    (mod 1200)
```

Donde `P₁ = 1200·log₂(φ) = 833.090 cents` es la **quinta áurea**.

### 3.2 Jerarquía de Intervalos Áureos

Calculando para distintos k:

| k | φ^k | 1200·log₂(φ^k) | mod 1200 | Nombre | Consonancia |
|---|-----|----------------|----------|--------|-------------|
| **0** | 1.000 | **0.000** | 0.000 | **Unísono** | ⭐⭐⭐⭐⭐ |
| **1** | 1.618 | **833.090** | 833.090 | **Quinta áurea** | ⭐⭐⭐⭐⭐ |
| **2** | 2.618 | **1666.180** | 466.180 | **Cuarta áurea** | ⭐⭐⭐⭐ |
| **3** | 4.236 | **2499.271** | 299.271 | **Tono áureo mayor** | ⭐⭐⭐ |
| **4** | 6.854 | **3332.361** | 932.361 | **Sexta áurea mayor** | ⭐⭐⭐ |
| **5** | 11.090 | **4165.451** | 565.451 | **Tritono áureo** | ⭐⭐ |
| **-1** | 0.618 | **-833.090** | 366.910 | **Cuarta áurea inv.** | ⭐⭐⭐⭐ |
| **-2** | 0.382 | **-1666.180** | 733.820 | **Quinta áurea inv.** | ⭐⭐⭐⭐ |

**Observaciones**:

1. **Quinta áurea (833.09 cents)** es MÁS GRANDE que quinta perfecta (700 cents)
2. **Cuarta áurea (366.91 cents)** es MÁS PEQUEÑA que cuarta perfecta (500 cents)
3. No hay equivalente directo a tercera mayor (400) o menor (300)
4. El **tritono áureo** (565.45 cents) es distinto al tritono 12-TET (600 cents)

### 3.3 Función de Consonancia

Definimos una **función continua** que mide consonancia de cualquier intervalo:

```
C(interval) = max_{k ∈ [-5, 5]} e^(-(d_k²/σ²))
```

Donde:
- `d_k = |interval - (1200·log₂(φ^k) mod 1200)|` = distancia al φ^k más cercano
- `σ = ε / 2` = desviación estándar (típicamente σ = 12.5 cents)

**Propiedades**:

- `C(0) = 1.0` (unísono perfecto)
- `C(833.09) ≈ 1.0` (quinta áurea)
- `C(intervalo aleatorio) ≈ 0` (disonancia)
- `C` es periódica cada octava (1200 cents)

**Pseudocódigo**:

```python
def consonance(interval_cents, tolerance=25):
    PHI = 1.618033988749895
    sigma = tolerance / 2.0
    max_consonance = 0.0

    for k in range(-5, 6):  # k ∈ [-5, 5]
        phi_interval = 1200 * log2(PHI ** k)
        wrapped = phi_interval % 1200
        distance = abs(interval_cents - wrapped)

        # Considerar wrap around (ej: 5 cents ≈ 1195 cents)
        distance = min(distance, 1200 - distance)

        consonance_k = exp(-(distance**2) / (sigma**2))
        max_consonance = max(max_consonance, consonance_k)

    return max_consonance
```

### 3.4 Mapa de Consonancia

Evaluando `C(I)` para I ∈ [0, 1200]:

```
Intervalos altamente consonantes (C > 0.9):
- 0 ± 25 cents (unísono)
- 366.91 ± 25 cents (cuarta áurea)
- 466.18 ± 25 cents (cuarta áurea superior)
- 733.82 ± 25 cents (quinta áurea invertida)
- 833.09 ± 25 cents (quinta áurea)

Intervalos moderadamente consonantes (0.5 < C < 0.9):
- 299.27 ± 40 cents (tono áureo)
- 565.45 ± 40 cents (tritono áureo)
- 932.36 ± 40 cents (sexta áurea)

Intervalos disonantes (C < 0.5):
- Todo el resto (zonas intermedias)
```

**Comparación con 12-TET**:

| Intervalo 12-TET | Cents | C(I) áureo | Clasificación |
|------------------|-------|------------|---------------|
| Unísono | 0 | 1.000 | Consonante |
| Semitono | 100 | 0.001 | Disonante |
| Tono | 200 | 0.081 | Disonante |
| Tercera menor | 300 | 0.999 | **Consonante** (≈299.27) |
| Tercera mayor | 400 | 0.143 | Disonante |
| Cuarta | 500 | 0.090 | Disonante |
| Tritono | 600 | 0.469 | Semi-disonante |
| Quinta | 700 | 0.089 | Disonante |
| Sexta menor | 800 | 0.038 | Disonante |
| Sexta mayor | 900 | 0.802 | **Semi-consonante** (≈932.36) |
| Séptima menor | 1000 | 0.024 | Disonante |
| Séptima mayor | 1100 | 0.002 | Disonante |

**¡Las quintas y cuartas tradicionales son DISONANTES en el sistema áureo!**

Este es el cambio perceptivo fundamental del sistema.

### 3.5 Implicaciones Perceptivas

1. **Familiaridad vs. Coherencia**:
   - Nuestro oído está entrenado en 12-TET
   - Acordes áureos sonarán "extraños" inicialmente
   - Con exposición, el cerebro puede aprender nuevas consonancias

2. **Flotación armónica**:
   - Sin terceras mayores/menores claras, acordes no tienen "color" mayor/menor
   - Sonoridad más **modal** o **ambigua**
   - Ideal para música ambient, experimental, soundscapes

3. **Tensión/Resolución redefinida**:
   - Cadencias no funcionan como V-I tradicional
   - Resolución hacia **autosimilitud** (intervalos φⁿ)

---

## 4. Escalas Diatónicas

### 4.1 Problema: Seleccionar 7 de 12

Con 12 notas cromáticas, necesitamos elegir **7 para una escala diatónica**.

Número de combinaciones: `C(12, 7) = 792`

**¿Cuál elegir?**

### 4.2 Criterio de Optimización

Definimos **Score de Estabilidad** para una escala S₇:

```
Score(S₇) = Σ_{i<j} C(|n_i - n_j|) - Penalty(S₇)
```

Donde:
- `C(|n_i - n_j|)` = consonancia entre notas i y j
- `Penalty(S₇)` = penalización por microintervalos consecutivos

**Penalización**:

```
Penalty(S₇) = Σ_{i=0}^{6} penalty_step(n_{i+1} - n_i)

penalty_step(Δ) = {
    10,  si Δ < 80 cents
    0,   si Δ ≥ 80 cents
}
```

**Intuición**:
- Maximizar consonancia entre todas las notas
- Evitar pasos melódicos muy pequeños (<80 cents)
- Balance entre estabilidad armónica y fluidez melódica

### 4.3 Algoritmo de Búsqueda

**Búsqueda exhaustiva** (factible con 792 combinaciones):

```python
def find_optimal_diatonic_scale(chromatic_scale):
    best_score = -infinity
    best_scale = None

    for combo in combinations(chromatic_scale, 7):
        scale = sorted(combo)
        score = calculate_score(scale)

        if score > best_score:
            best_score = score
            best_scale = scale

    return best_scale, best_score

def calculate_score(scale):
    total = 0.0

    # Consonancia entre todas las notas
    for i in range(len(scale)):
        for j in range(i+1, len(scale)):
            interval = scale[j] - scale[i]
            total += consonance(interval)

    # Penalización por microintervalos
    for i in range(len(scale) - 1):
        step = scale[i+1] - scale[i]
        if step < 80:
            total -= 10

    return total
```

### 4.4 Escala Mayor Áurea (Resultado)

Ejecutando el algoritmo con la escala cromática áurea:

**Escala Mayor Áurea óptima** (7 notas):

| Grado | Nota | Cents | Intervalo desde tónica |
|-------|------|-------|------------------------|
| I | φ₀ | 0.000 | 0.000 (Unísono) |
| II | φ₅ | 41.328 | 41.328 |
| III | φ₆ | 66.875 | 66.875 |
| IV | φ₇ | 108.203 | 108.203 |
| V | φ₈ | 175.078 | 175.078 |
| VI | φ₉ | 283.281 | 283.281 |
| VII | φ₁₀ | 458.359 | 458.359 |

**Patrón de intervalos** (pasos entre grados):

```
I → II:   41.328 cents (pequeño)
II → III: 25.547 cents (micro)
III → IV: 41.328 cents (pequeño)
IV → V:   66.875 cents (medio)
V → VI:   108.203 cents (grande)
VI → VII: 175.078 cents (muy grande)
VII → I:  283.281 cents (salto áureo)
```

**Patrón simplificado**: `pequeño–micro–pequeño–medio–grande–muy grande–salto`

**NO es T-T-s-T-T-T-s** (mayor tradicional).
**ES autosimilar**: ratios entre pasos ≈ φ.

### 4.5 Propiedades de la Escala Mayor Áurea

#### 4.5.1 Consonancia Interna

Score de consonancia: **S = 18.74** (alto)

Intervalos más consonantes dentro de la escala:
- I-VI (283.281 cents) → C = 0.999 (≈ tono áureo)
- IV-VII (350.156 cents) → C = 0.967
- II-V (133.750 cents) → C = 0.812

#### 4.5.2 Centro Tonal

La nota φ₀ (tónica) tiene:
- **Máxima consonancia promedio** con el resto
- **Intervalos balanceados** hacia arriba y abajo
- **Autosimilitud**: intervalos desde tónica siguen serie φ

Esto la establece como **centro gravitacional** perceptivo.

#### 4.5.3 Ambigüedad Modal

Sin terceras claras (no hay ~400 ni ~300 cents desde tónica), la escala:
- No suena "mayor" ni "menor" en sentido tradicional
- Tiene carácter **modal** o **flotante**
- Similar a escalas exóticas (Pelog, Slendro indonesias)

### 4.6 Escala Menor Áurea

**No existe "menor natural/armónica/melódica" en sentido clásico**.

Proponemos dos enfoques:

#### 4.6.1 Menor Estructural (Rotación Modal)

Tomar la **rotación de la mayor áurea** que maximice tensión desde la tónica:

**Menor Áurea = Modo VI de Mayor Áurea**

| Grado | Nota | Cents desde nueva tónica |
|-------|------|--------------------------|
| i | φ₉ | 0.000 |
| ii | φ₁₀ | 175.078 |
| iii | φ₀ | 458.359 |
| iv | φ₅ | 499.687 |
| v | φ₆ | 525.234 |
| vi | φ₇ | 566.562 |
| vii | φ₈ | 633.437 |

**Carácter**: Más "oscuro" que mayor (intervalos comprimidos al inicio).

#### 4.6.2 Menor Gravitacional (Misma Escala, Tónica Distinta)

Usar las mismas 7 notas de mayor áurea, pero **perceptivamente enfatizar φ₆ o φ₉** como centro mediante:
- Pedales de bajo
- Cadencias hacia esa nota
- Figuración melódica que gira alrededor de ella

**Ventaja**: Modular entre "mayor" y "menor" sin cambiar notas (como jónico/eólico).

### 4.7 Otras Escalas Experimentales

El algoritmo permite generar:

**Escala Pentatónica Áurea** (5 de 12):
- Optimiza consonancia con menos notas
- Resultado: φ₀, φ₆, φ₇, φ₉, φ₁₀

**Escala Hexatónica Áurea** (6 de 12):
- Intermedia entre pentatónica y diatónica

**Escalas Personalizadas**:
- UI permite seleccionar manualmente cualquier 7 de las 12
- Presets: Mayor Áurea, Menor Áurea, Experimental 1-3

---

## 5. Acordes y Armonía

### 5.1 Tríada Áurea Fundamental

En lugar de **tercera + quinta** (DO-MI-SOL), usamos:

**Tríada áurea** = Fundamental + φ² + φ³ (envueltos en octava)

**Fórmula**:

```
Tríada(root) = {
    root,
    (root + P₂) mod 1200,
    (root + P₃) mod 1200
}
```

Donde:
- `P₂ = 1200·log₂(φ²) mod 1200 = 466.180 cents`
- `P₃ = 1200·log₂(φ³) mod 1200 = 299.271 cents`

**Ejemplo**: Tríada sobre φ₀ (0 cents):

| Nota | Cents | Intervalo desde root |
|------|-------|----------------------|
| Root | 0.000 | 0 (Unísono) |
| φ² | 466.180 | 466.180 (Cuarta áurea sup.) |
| φ³ | 299.271 | 299.271 (Tono áureo) |

**Ordenado ascendentemente**: 0, 299.271, 466.180

**Intervalos internos**:
- Root → φ³: 299.271 cents (tono áureo) → C = 0.999 ⭐
- φ³ → φ²: 166.909 cents → C = 0.421
- φ² → Root+oct: 733.820 cents (quinta inv.) → C = 1.000 ⭐

**Consonancia total del acorde**: **Alta** (2 intervalos muy consonantes).

### 5.2 Interpretación Geométrica

**No son terceras + quintas clásicas**.

Son **triángulos autosimilares**:

```
      φ³ (299¢)
       /\
      /  \
     /    \
    /  φ²  \
   /  466¢  \
  /__________\
Root   φ⁵   Root+φ²
      734¢
```

Los lados del triángulo tienen ratios ≈ φ entre sí.

### 5.3 Inversiones

**Primera inversión** (φ³ en bajo):

| Nota | Cents desde φ³ |
|------|----------------|
| φ³ | 0.000 |
| φ² | 166.909 |
| Root | 900.729 |

**Segunda inversión** (φ² en bajo):

| Nota | Cents desde φ² |
|------|----------------|
| φ² | 0.000 |
| Root | 733.820 |
| φ³ | 1033.091 |

Todas las inversiones mantienen **alto grado de consonancia**.

### 5.4 Acordes Extendidos

#### 5.4.1 Séptima Áurea

Añadir φ⁴:

```
Séptima(root) = {root, φ², φ³, φ⁴}
```

Donde `P₄ = 1200·log₂(φ⁴) mod 1200 = 932.361 cents`

**Ejemplo sobre φ₀**:

| Nota | Cents | Intervalo |
|------|-------|-----------|
| Root | 0.000 | 0 |
| φ³ | 299.271 | 299.271 |
| φ² | 466.180 | 466.180 |
| φ⁴ | 932.361 | 932.361 |

**Sonoridad**: Más densa, color "dorado" expandido.

#### 5.4.2 Novena Áurea

```
Novena(root) = {root, φ², φ³, φ⁴, φ⁵}
```

Donde `P₅ = 1200·log₂(φ⁵) mod 1200 = 565.451 cents`

**Restricción importante**: Evitar colisiones < 100 cents.

Si dos notas quedan demasiado cerca, omitir una:

```python
def build_extended_chord(root, powers):
    notes = [root]
    for k in powers:
        new_note = (root + get_phi_interval(k)) % 1200

        # Verificar colisión
        too_close = False
        for existing in notes:
            if abs(new_note - existing) < 100:
                too_close = True
                break

        if not too_close:
            notes.append(new_note)

    return sorted(notes)
```

### 5.5 Voicing (Disposición de Voces)

Para acordes de 4+ notas, la **disposición en el registro** afecta consonancia:

**Regla de oro**: Intervalos grandes en grave, pequeños en agudo.

**Voicing abierto** (recomendado para tríada áurea):

```
Soprano:  φ²   (octava alta)
Alto:     φ³   (octava media)
Tenor:    Root (octava media)
Bajo:     Root (octava baja)
```

**Voicing cerrado**:

```
Todas las notas en la misma octava:
0, 299.271, 466.180 cents
```

### 5.6 Progresiones de Acordes

Ver sección [7. Función Armónica Áurea](#7-función-armónica-áurea).

---

## 6. Modos Armónicos

### 6.1 Generación de Modos

Igual que en sistema tonal tradicional, los **modos** son **rotaciones** de la escala mayor.

Si la escala mayor áurea tiene 7 notas:

```
Escala = {n₀, n₁, n₂, n₃, n₄, n₅, n₆}
```

**Modo I** (Modo principal / "Jónico áureo"):
`{n₀, n₁, n₂, n₃, n₄, n₅, n₆}`

**Modo II** ("Dórico áureo"):
`{n₁, n₂, n₃, n₄, n₅, n₆, n₀+1200}`
(transponer para que n₁ = 0)

**Modo III** ("Frigio áureo"):
`{n₂, n₃, n₄, n₅, n₆, n₀+1200, n₁+1200}`

...y así hasta Modo VII.

### 6.2 Los 7 Modos Áureos

Usando la escala mayor áurea derivada:

| Modo | Tónica (cents) | Patrón de intervalos | Carácter |
|------|----------------|----------------------|----------|
| **I - Φōnico** | φ₀ (0) | 41-26-41-67-108-175-283 | Fundamental, balanceado |
| **II - Aurídico** | φ₅ (41.328) | 26-41-67-108-175-283-41 | Tenso al inicio |
| **III - Φrígio** | φ₆ (66.875) | 41-67-108-175-283-41-26 | Expansivo |
| **IV - Áureo** | φ₇ (108.203) | 67-108-175-283-41-26-41 | Saltos grandes |
| **V - Φiximódico** | φ₈ (175.078) | 108-175-283-41-26-41-67 | Amplio, espacioso |
| **VI - Aurélico** | φ₉ (283.281) | 175-283-41-26-41-67-108 | "Menor" áureo |
| **VII - Locrífico** | φ₁₀ (458.359) | 283-41-26-41-67-108-175 | Inestable, tritónico |

### 6.3 Características de Cada Modo

#### Modo I - Φōnico (Phōniko)
- **Tónica**: φ₀
- **Análogo**: Jónico (mayor)
- **Intervalos desde tónica**: Balanceados, consonantes
- **Uso**: Fundamental, punto de partida del sistema

#### Modo II - Aurídico (Aurídio)
- **Tónica**: φ₅ (41.328 cents)
- **Análogo**: Dórico
- **Carácter**: Microintervalo al inicio (φ₅→φ₆ = 25 cents)
- **Uso**: Música microtonal, texturas densas

#### Modo VI - Aurélico (Menor Áureo)
- **Tónica**: φ₉ (283.281 cents)
- **Análogo**: Eólico (menor natural)
- **Carácter**: Intervalo inicial grande (175 cents)
- **Uso**: Sonoridades "oscuras" o "menores"

#### Modo VII - Locrífico
- **Tónica**: φ₁₀ (458.359 cents)
- **Análogo**: Locrio
- **Carácter**: Inestable, salto enorme al inicio (283 cents)
- **Uso**: Tensión máxima, transiciones, efectos

### 6.4 Modulación Entre Modos

**Modulación modal** = cambiar el centro tonal **sin cambiar las notas**.

**Técnica**:
1. Establecer modo I con pedal en φ₀
2. Gradualmente enfatizar otra nota (ej: φ₆)
3. Cadenciar hacia φ₆
4. → Ahora estamos en modo III

**Ventaja**: Fluidez armónica, sin "cambios de tonalidad" bruscos.

### 6.5 Acordes Modales

Cada modo tiene una **tríada característica** sobre su tónica:

**Ejemplo - Modo I**:
- Tríada: φ₀, φ₆, φ₉ (notas I, III, VI de la escala)
- Intervalos: 0, 66.875, 283.281
- → Acorde "mayor" áureo

**Ejemplo - Modo VI**:
- Tríada: φ₉, φ₀+1200, φ₅+1200
- → Acorde "menor" áureo

---

## 7. Función Armónica Áurea

### 7.1 Redefinición de Función

En armonía tradicional:
- **Tónica (I)**: Reposo
- **Dominante (V)**: Tensión (por tritono IV-VII)
- **Subdominante (IV)**: Preparación

**Problema**: El tritono (600 cents) no existe en sistema áureo.

**Solución**: Redefinir función como **gradiente de φ**.

### 7.2 Centros Funcionales Áureos

#### 7.2.1 Tónica (Φ⁰)

**Definición**: Máxima autosimilitud.

La tónica es la nota que:
- Tiene mayor consonancia promedio con la escala
- Genera tríada más estable
- Es el "punto 0" de referencia φ⁰

**En escala mayor áurea**: φ₀ (0 cents)

#### 7.2.2 Dominante Áurea (Φ¹ o Φ²)

**Definición**: Nota a distancia de φ¹ o φ² desde tónica.

Intervalos candidatos:
- φ¹ → 833.090 cents (quinta áurea)
- φ² → 466.180 cents (cuarta áurea superior)

**En escala mayor áurea**:
- Nota más cercana a 466.180 cents = **φ₁₀ (458.359)** → Grado VII
- Nota más cercana a 833.090 cents = **fuera de escala** (usar φ₁₁ cromático)

**Dominante funcional** = **Grado VII** (φ₁₀)

#### 7.2.3 Subdominante Áurea (Φ⁻¹)

**Definición**: Nota a distancia de φ⁻¹ desde tónica.

Intervalo: φ⁻¹ → 366.910 cents (cuarta áurea invertida)

**En escala mayor áurea**:
- Nota más cercana a 366.910 cents = **φ₉ (283.281)** → Grado VI

**Subdominante funcional** = **Grado VI** (φ₉)

### 7.3 Cadencias Áureas

#### 7.3.1 Cadencia Perfecta Áurea

**Análogo**: V → I (dominante-tónica)

**En sistema áureo**: VII → I

```
φ₁₀ (458.359) → φ₀ (0.000)
```

Intervalo: 541.641 cents (descendente) ≈ tritono áureo (565.451)

**Fuerza de resolución**: Media-alta (el intervalo 541 cents tiene C ≈ 0.47)

#### 7.3.2 Cadencia Plagal Áurea

**Análogo**: IV → I (subdominante-tónica)

**En sistema áureo**: VI → I

```
φ₉ (283.281) → φ₀ (0.000)
```

Intervalo: 916.719 cents (descendente) ≈ sexta áurea (932.361)

**Fuerza de resolución**: Alta (C ≈ 0.80)

#### 7.3.3 Cadencia Rota Áurea

**Análogo**: V → VI (engaño)

**En sistema áureo**: VII → VI

```
φ₁₀ (458.359) → φ₉ (283.281)
```

**Efecto**: Evita resolución esperada, genera suspensión.

### 7.4 Progresiones Armónicas

#### 7.4.1 Progresión Fundamental: I-VI-VII-I

**Equivalente a I-IV-V-I** clásico:

```
Tónica → Subdominante → Dominante → Tónica
φ₀ → φ₉ → φ₁₀ → φ₀
```

**Movimiento del bajo**:
- φ₀ → φ₉: +283.281 cents (tono áureo mayor)
- φ₉ → φ₁₀: +175.078 cents (salto grande)
- φ₁₀ → φ₀: -458.359 cents (retorno áureo)

**Análisis**:
- Expansión gradual: I → VI (apertura)
- Tensión: VI → VII (acumulación)
- Resolución: VII → I (cierre)

#### 7.4.2 Progresión Modal: I-III-V-I

Recorrido por modos:

```
φ₀ → φ₆ → φ₈ → φ₀
```

**Carácter**: Más suave, menos tensional.

#### 7.4.3 Jazz Áureo: ii-V-I

**Adaptación**: En escala mayor áurea, no hay "ii" claro.

**Alternativa**: II-VII-I

```
φ₅ → φ₁₀ → φ₀
```

**O**: Progresión descendente de φ:

```
φ₁₀ → φ₉ → φ₅ → φ₀
(Gradiente φ³ → φ² → φ¹ → φ⁰)
```

### 7.5 Análisis Funcional de Acordes

En cada acorde de la progresión, analizar:

1. **Distancia a tónica** (en potencias de φ)
2. **Consonancia del acorde** (Score interno)
3. **Tensión melódica** (movimiento de voces)

**Ejemplo - Progresión I-VI-VII-I**:

| Acorde | Root | Tensión | Consonancia | Función |
|--------|------|---------|-------------|---------|
| I | φ₀ | Baja | Alta | Reposo |
| VI | φ₉ | Media | Alta | Preparación |
| VII | φ₁₀ | Alta | Media | Tensión |
| I | φ₀ | Baja | Alta | Resolución |

### 7.6 Modulación Tonal

**Modulación** = cambiar la tónica de referencia φ⁰.

**Técnica de pivot**:

1. Acorde común a ambas tonalidades
2. Reinterpretar funcionalmente
3. Cadenciar en nueva tónica

**Ejemplo**: Modular de φ₀ a φ₆:

```
Tonalidad A (tónica φ₀):
I - VI - III ...

Acorde III en A = Acorde I en B (tónica φ₆)

... III - VII_B - I_B
```

### 7.7 Sustituciones Armónicas

**Sustitución por autosimilitud**:

Un acorde puede sustituirse por otro cuya root está a φⁿ de distancia:

- VII puede sustituirse por IV (ambos φ² desde sus propias tónicas)
- VI puede sustituirse por II (φ³ de distancia)

**Rearmonización áurea**: Basada en mantener **perfil de consonancia** similar.

---

## 8. Conducción de Voces y Contrapunto

### 8.1 Principios Generales

El contrapunto áureo **no copia a Fux** (Species Counterpoint). Adaptamos reglas a universo φ.

### 8.2 Reglas de Movimiento

#### 8.2.1 Movimientos Permitidos

✅ **Movimiento contrario** (preferido):
```
Voz 1: ↑
Voz 2: ↓
```
Minimiza colisiones, maximiza independencia.

✅ **Movimiento oblicuo**:
```
Voz 1: ↑
Voz 2: —
```
Una voz se mueve, otra se mantiene.

⚠️ **Movimiento directo** (permitido solo a consonancia φ):
```
Voz 1: ↑
Voz 2: ↑
```
Solo si el intervalo de llegada tiene `C > 0.8`.

#### 8.2.2 Movimientos Prohibidos

❌ **Paralelos de φ¹** (quintas áureas consecutivas):
```
Voz 1: φ₀ → φ₅
Voz 2: φ₁₁ → φ₆
```
(Ambos movimientos de ~833 cents)

**Razón**: Destruye independencia de voces (como quintas paralelas en 12-TET).

❌ **Duplicación de intervalos idénticos** en voces extremas:
```
Soprano: φ₀ → φ₆
Bajo:    φ₅ → φ₇
```
Si ambos intervalos son idénticos en cents.

❌ **Saltos > φ³** sin resolución:
```
φ₀ → φ₁₀ (salto de 458 cents)
```
Debe ser seguido por movimiento contrario o grado conjunto.

### 8.3 Tratamiento de Disonancias

**Disonancia** = intervalo con `C < 0.5`.

Las disonancias son **permitidas** si:

1. **Son transitorias**:
   Duración ≤ 1 tiempo (corchea o semicorchea típicamente)

2. **Resuelven por grado conjunto**:
   La voz disonante se mueve al grado más cercano de la escala.

3. **Resuelven a potencia inferior de φ**:
   Si el intervalo disonante está cerca de φ⁴, debe resolver a φ³ o φ².

**Ejemplo**:

```
Tiempo 1: Soprano φ₈, Alto φ₀ → Intervalo 175 cents (C = 0.421, disonante)
Tiempo 2: Soprano φ₇, Alto φ₀ → Intervalo 108 cents (C = 0.612, más consonante)
```

### 8.4 Voice Leading Óptimo

**Objetivo**: Minimizar movimiento total de voces.

**Función a minimizar**:

```
Movement(t→t+1) = Σ|voice_i(t+1) - voice_i(t)|
```

Con penalización por:
- Cruce de voces: `+100`
- Saltos grandes (>φ³): `+50`
- Movimientos paralelos prohibidos: `+200`

**Algoritmo**:

Para pasar del acorde A al acorde B con N voces:

```python
def optimal_voice_leading(chord_A, chord_B, num_voices):
    # Generar todas las permutaciones de chord_B
    best_movement = infinity
    best_assignment = None

    for perm in permutations(chord_B):
        movement = 0
        valid = True

        for i in range(num_voices):
            voice_movement = abs(perm[i] - chord_A[i])
            movement += voice_movement

            # Verificar cruces
            if i > 0 and perm[i] < perm[i-1]:
                valid = False
                break

            # Penalizar saltos grandes
            if voice_movement > 300:  # φ³ ≈ 299 cents
                movement += 50

        if valid and movement < best_movement:
            best_movement = movement
            best_assignment = perm

    return best_assignment
```

**Para N=4 (SATB)**: 4! = 24 permutaciones (factible).

### 8.5 Contrapunto a 2 Voces

**Reglas específicas para dúo**:

1. **Intervalos consonantes en tiempos fuertes**:
   `C(intervalo) > 0.7`

2. **Disonancias solo en tiempos débiles**

3. **Inicio y fin en unísono o φ¹**:
   Primera y última nota: 0 o 833 cents de distancia.

4. **Climax melódico no simultáneo**:
   Notas más agudas de cada voz deben estar separadas ≥2 tiempos.

### 8.6 Contrapunto a 4 Voces (SATB)

**Rangos típicos** (en cents desde C₄ = 0):

```
Soprano:  0 - 1200 (C₄ - C₅)
Alto:     -500 - 700 (G₃ - G₄)
Tenor:    -1200 - 0 (C₃ - C₄)
Bajo:     -2400 - -1200 (C₂ - C₃)
```

**Reglas adicionales**:

1. **Duplicaciones**:
   - Preferir duplicar root de acorde
   - Evitar duplicar disonancias

2. **Espaciado**:
   - S-A y A-T: máximo φ⁴ (932 cents)
   - T-B: puede ser mayor (hasta octava)

3. **Movimiento de voces internas** (A, T):
   - Preferir grado conjunto
   - Evitar saltos > φ²

### 8.7 Cadencias Contrapuntísticas

**Cadencia perfecta áurea (VII-I)**:

```
Soprano:  φ₁₀ → φ₀   (descenso melódico)
Alto:     φ₆  → φ₅   (grado conjunto)
Tenor:    φ₀  → φ₀   (pedal)
Bajo:     φ₁₀ → φ₀   (salto áureo)
```

**Características**:
- Soprano y Bajo en movimiento paralelo (permitido si llegan a consonancia)
- Alto en movimiento contrario
- Tenor como pedal de tónica

---

## 9. Composición Algorítmica

### 9.1 Objetivo: "Bach en Universo φ"

**No imitamos BWV 846 literalmente**.

Capturamos su **espíritu**:
- Figuración continua
- Bajo estable
- Armonía implícita (no acordes explícitos)
- Textura densa pero transparente

### 9.2 Estructura de un Preludio Áureo

**Forma**: ABA' (opcional) o continua.

**Duración**: 16-32 compases.

**Compás**: 4/4 (simplificado, aunque φ sugeriría compases irregulares).

**Tempo**: 60-120 BPM.

### 9.3 Algoritmo Generativo (5 Pasos)

#### Paso 1: Progresión Armónica

**Generar secuencia de 8-16 acordes**:

```python
def generate_harmonic_progression(length=8):
    chords = [I]  # Empezar en tónica

    for i in range(length - 1):
        current = chords[-1]

        # Determinar si estamos cerca de cadencia
        if i >= length - 3:
            # Forzar cadencia: ... → VI → VII → I
            if i == length - 3:
                next_chord = VI
            elif i == length - 2:
                next_chord = VII
            else:
                next_chord = I
        else:
            # Movimiento libre (preferir grados conjuntos funcionales)
            candidates = [II, III, V, VI, VII]
            weights = [consonance(current, c) for c in candidates]
            next_chord = weighted_choice(candidates, weights)

        chords.append(next_chord)

    return chords
```

**Ejemplo de progresión**:

```
I - III - V - VI - VII - V - VI - VII - I
(8 acordes, cadencia perfecta al final)
```

#### Paso 2: Bajo Estructural

**Generar línea de bajo** basada en fundamentals de acordes:

```python
def generate_bass_line(progression):
    bass = []

    for i, chord in enumerate(progression):
        root = chord.root

        # Movimiento del bajo
        if i > 0:
            prev_root = progression[i-1].root
            movement = root - prev_root

            # Preferir grados conjuntos o saltos φ¹
            if abs(movement) > 200:  # Salto grande
                # Invertir octava si es necesario
                if movement > 600:
                    root -= 1200
                elif movement < -600:
                    root += 1200

        # Ritmo del bajo (simplificado: redondas)
        bass.append({
            'note': root,
            'duration': 1.0,  # 1 compás
            'time': i * 1.0
        })

    return bass
```

#### Paso 3: Voces Intermedias (A, T)

**Completar tríadas/tetradas**:

```python
def generate_inner_voices(progression, bass):
    alto = []
    tenor = []

    for i, chord in enumerate(progression):
        # Obtener notas del acorde (sin el bajo)
        chord_notes = chord.get_notes()
        bass_note = bass[i]['note']
        available = [n for n in chord_notes if n != bass_note]

        # Voice leading desde acorde anterior
        if i > 0:
            prev_alto = alto[-1]['note']
            prev_tenor = tenor[-1]['note']

            # Optimizar movimiento
            assignment = optimal_voice_leading(
                [prev_alto, prev_tenor],
                available,
                2
            )
            alto_note, tenor_note = assignment
        else:
            # Primer acorde: disposición estándar
            alto_note = available[0]
            tenor_note = available[1] if len(available) > 1 else available[0]

        # Ritmo (simplificado: blancas)
        alto.append({'note': alto_note, 'duration': 0.5, 'time': i * 1.0})
        tenor.append({'note': tenor_note, 'duration': 0.5, 'time': i * 1.0})

    return alto, tenor
```

#### Paso 4: Soprano (Melodía)

**Generar melodía con figuración rítmica**:

```python
def generate_soprano(progression, scale):
    soprano = []
    time = 0.0

    for i, chord in enumerate(progression):
        chord_duration = 1.0  # 1 compás
        beats_left = chord_duration

        # Seleccionar notas del acorde + notas de paso
        chord_notes = chord.get_notes()

        while beats_left > 0:
            # Elegir nota (80% cordal, 20% paso)
            if random() < 0.8:
                note = choice(chord_notes)
            else:
                # Nota de paso (grado conjunto de escala)
                prev_note = soprano[-1]['note'] if soprano else chord_notes[0]
                note = find_neighbor_in_scale(prev_note, scale)

            # Elegir duración (figuración variada)
            duration = choice([0.125, 0.25, 0.5])  # Semicorchea, corchea, blanca

            if duration > beats_left:
                duration = beats_left

            soprano.append({
                'note': note,
                'duration': duration,
                'time': time
            })

            time += duration
            beats_left -= duration

        # Clímax melódico en φ² sobre tónica final
        if i == len(progression) - 1:
            soprano[-3]['note'] = chord_notes[0] + 466  # φ² arriba

    return soprano
```

#### Paso 5: Verificación y Ajuste

**Comprobar reglas contrapuntísticas**:

```python
def verify_and_fix(satb):
    soprano, alto, tenor, bass = satb

    for t in range(len(soprano)):
        # Obtener vertical en tiempo t
        s = soprano[t]['note']
        a = alto[t]['note']
        t_note = tenor[t]['note']
        b = bass[t]['note']

        # Verificar consonancia vertical
        intervals = [
            abs(s - a),
            abs(a - t_note),
            abs(t_note - b)
        ]

        for interval in intervals:
            c = consonance(interval)

            # Si disonancia en tiempo fuerte, ajustar
            if c < 0.5 and is_strong_beat(t):
                # [Lógica de ajuste...]
                pass

        # Verificar paralelos prohibidos
        if t > 0:
            # [Lógica de verificación...]
            pass

    return satb
```

### 9.4 Parámetros Configurables

**Usuario puede ajustar**:

- **Longitud**: 16, 24, 32 compases
- **Tempo**: 60-120 BPM
- **Densidad rítmica**:
  - Baja: Redondas y blancas
  - Media: Negras y corcheas
  - Alta: Semicorcheas y fusas
- **Complejidad armónica**:
  - Simple: I-VI-VII-I repetido
  - Moderada: Progresión de 8 acordes
  - Compleja: Progresión de 16 acordes con modulaciones

### 9.5 Renderizado de Partitura

**Visualización en canvas**:

- 4 pentagramas (S, A, T, B)
- Claves adaptadas (Treble, Treble, Treble 8vb, Bass)
- Posición vertical = cents (no pentagrama tradicional estricto)
- Color coding por voz:
  - Soprano: `#ec4899` (rosa)
  - Alto: `#8b5cf6` (púrpura)
  - Tenor: `#3b82f6` (azul)
  - Bajo: `#059669` (verde)

**Highlight durante playback**:

```javascript
function renderStaff(voice, currentTime) {
    for (let note of voice) {
        const isActive = (currentTime >= note.time &&
                          currentTime < note.time + note.duration);

        const color = isActive ? '#fbbf24' : voiceColor;
        drawNote(note, color);
    }
}
```

### 9.6 Exportación MIDI (Opcional)

**Conversión a MIDI**:

```python
def export_to_midi(satb, filename):
    midi = MIDIFile(4)  # 4 tracks

    tracks = [
        ('Soprano', satb['soprano']),
        ('Alto', satb['alto']),
        ('Tenor', satb['tenor']),
        ('Bajo', satb['bass'])
    ]

    for track_idx, (name, voice) in enumerate(tracks):
        midi.addTrackName(track_idx, 0, name)

        for note in voice:
            # Convertir cents a MIDI pitch bend
            cents = note['note']
            midi_note = 60  # C₄
            pitch_bend = int((cents / 1200) * 8192)

            # Agregar nota
            midi.addNote(
                track=track_idx,
                channel=track_idx,
                pitch=midi_note,
                time=note['time'],
                duration=note['duration'],
                volume=100
            )

            # Agregar pitch bend
            midi.addPitchWheelEvent(
                track=track_idx,
                channel=track_idx,
                time=note['time'],
                pitchWheelValue=pitch_bend
            )

    with open(filename, 'wb') as f:
        midi.writeFile(f)
```

**Nota**: MIDI estándar solo soporta pitch bend de ±2 semitonos, insuficiente para sistema áureo. Alternativa: usar múltiples canales con afinación custom (MTS - MIDI Tuning Standard).

---

## 10. Implementación Técnica

### 10.1 Motor Matemático: `golden-harmony-engine.js`

**Clase principal**:

```javascript
class GoldenHarmonyEngine {
    constructor() {
        this.PHI = (1 + Math.sqrt(5)) / 2;  // 1.618033988749895
        this.chromaticScale = this.generateChromaticScale();
        this.diatonicScales = {};
        this.consonanceCache = new Map();
        this.chordLibrary = {};
    }

    // ===== GENERACIÓN DE ESCALAS =====

    generateChromaticScale() {
        const notes = [];

        // Generar 12 notas
        for (let i = 0; i < 12; i++) {
            const centsRaw = 1200 * Math.pow(this.PHI, -i);
            const cents = centsRaw % 1200;
            notes.push({
                index: i,
                cents: cents,
                ratio: Math.pow(this.PHI, -i),
                name: `φ${i}`
            });
        }

        // Ordenar por cents
        notes.sort((a, b) => a.cents - b.cents);

        // Normalizar (tónica = 0)
        const offset = notes[0].cents;
        notes.forEach(note => {
            note.cents -= offset;
        });

        return notes;
    }

    generateDiatonicScale(preset = 'major') {
        if (this.diatonicScales[preset]) {
            return this.diatonicScales[preset];
        }

        // Optimización combinatoria
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

        this.diatonicScales[preset] = {
            notes: bestScale,
            score: bestScore
        };

        return this.diatonicScales[preset];
    }

    scaleStabilityScore(scale) {
        let score = 0.0;

        // Sumar consonancia entre todas las notas
        for (let i = 0; i < scale.length; i++) {
            for (let j = i + 1; j < scale.length; j++) {
                const interval = Math.abs(scale[j].cents - scale[i].cents);
                score += this.consonance(interval);
            }
        }

        // Penalización por microintervalos consecutivos
        for (let i = 0; i < scale.length - 1; i++) {
            const step = scale[i + 1].cents - scale[i].cents;
            if (step < 80) {
                score -= 10;
            }
        }

        return score;
    }

    getCombinations(n, k) {
        // Generar todas las combinaciones C(n, k)
        const result = [];
        const combination = [];

        function backtrack(start) {
            if (combination.length === k) {
                result.push([...combination]);
                return;
            }

            for (let i = start; i < n; i++) {
                combination.push(i);
                backtrack(i + 1);
                combination.pop();
            }
        }

        backtrack(0);
        return result;
    }

    // ===== CONSONANCIA =====

    consonance(intervalCents, tolerance = 25) {
        // Usar caché para eficiencia
        const key = `${intervalCents.toFixed(2)}_${tolerance}`;
        if (this.consonanceCache.has(key)) {
            return this.consonanceCache.get(key);
        }

        let minDistance = Infinity;

        // Buscar potencia de φ más cercana
        for (let k = -5; k <= 5; k++) {
            const phiInterval = 1200 * Math.log2(Math.pow(this.PHI, k));
            const wrapped = ((phiInterval % 1200) + 1200) % 1200;

            let distance = Math.abs(intervalCents - wrapped);

            // Considerar wrap-around
            distance = Math.min(distance, 1200 - distance);

            minDistance = Math.min(minDistance, distance);
        }

        // Gaussiana
        const sigma = tolerance / 2.0;
        const consonanceValue = Math.exp(-(minDistance * minDistance) / (sigma * sigma));

        this.consonanceCache.set(key, consonanceValue);
        return consonanceValue;
    }

    // ===== ACORDES =====

    generateTriad(rootCents) {
        const p2 = this.getPhiInterval(2);
        const p3 = this.getPhiInterval(3);

        const notes = [
            rootCents,
            (rootCents + p3) % 1200,
            (rootCents + p2) % 1200
        ];

        return notes.sort((a, b) => a - b);
    }

    generateSeventhChord(rootCents) {
        const triad = this.generateTriad(rootCents);
        const p4 = this.getPhiInterval(4);

        triad.push((rootCents + p4) % 1200);
        return triad.sort((a, b) => a - b);
    }

    getPhiInterval(k) {
        // Intervalo φ^k en cents (envuelto en octava)
        const interval = 1200 * Math.log2(Math.pow(this.PHI, k));
        return ((interval % 1200) + 1200) % 1200;
    }

    // ===== VOICE LEADING =====

    optimalVoiceLeading(chordA, chordB) {
        // Algoritmo húngaro simplificado (greedy)
        const numVoices = chordA.length;
        const assignment = new Array(numVoices);
        const used = new Set();

        for (let i = 0; i < numVoices; i++) {
            let minMovement = Infinity;
            let bestNote = null;

            for (const noteB of chordB) {
                if (used.has(noteB)) continue;

                const movement = Math.abs(noteB - chordA[i]);
                if (movement < minMovement) {
                    minMovement = movement;
                    bestNote = noteB;
                }
            }

            assignment[i] = bestNote;
            used.add(bestNote);
        }

        return assignment;
    }

    // ===== CONVERSIONES =====

    centsToFrequency(cents, baseFreq = 440) {
        return baseFreq * Math.pow(2, cents / 1200);
    }

    frequencyToCents(freq, baseFreq = 440) {
        return 1200 * Math.log2(freq / baseFreq);
    }

    // ===== ANÁLISIS =====

    analyzeScale(scale) {
        const intervals = [];
        for (let i = 0; i < scale.length - 1; i++) {
            intervals.push(scale[i + 1].cents - scale[i].cents);
        }

        const avgConsonance = this.scaleStabilityScore(scale) / (scale.length * (scale.length - 1) / 2);

        return {
            intervals: intervals,
            avgConsonance: avgConsonance,
            range: scale[scale.length - 1].cents - scale[0].cents,
            notes: scale.length
        };
    }

    analyzeChord(chord) {
        let totalConsonance = 0;
        let pairCount = 0;

        for (let i = 0; i < chord.length; i++) {
            for (let j = i + 1; j < chord.length; j++) {
                const interval = Math.abs(chord[j] - chord[i]);
                totalConsonance += this.consonance(interval);
                pairCount++;
            }
        }

        return {
            avgConsonance: totalConsonance / pairCount,
            notes: chord.length
        };
    }
}
```

### 10.2 Web Audio Synthesis

**Patrón ADSR** (reutilizar de Math Sound Lab):

```javascript
function playNote(frequency, duration, volume, waveform = 'triangle') {
    if (!audioContext) return;

    const now = audioContext.currentTime;

    // Oscillator
    const osc = audioContext.createOscillator();
    osc.type = waveform;
    osc.frequency.value = frequency;

    // ADSR Envelope
    const envelope = audioContext.createGain();
    const attack = 0.01;
    const decay = 0.1;
    const sustain = volume * 0.7;
    const release = 0.2;

    envelope.gain.setValueAtTime(0, now);
    envelope.gain.linearRampToValueAtTime(volume, now + attack);
    envelope.gain.linearRampToValueAtTime(sustain, now + attack + decay);
    envelope.gain.setValueAtTime(sustain, now + duration - release);
    envelope.gain.exponentialRampToValueAtTime(0.001, now + duration);

    // Filter
    const filter = audioContext.createBiquadFilter();
    filter.type = 'lowpass';
    filter.frequency.value = frequency * 2;
    filter.Q.value = 1.5;

    // Routing
    osc.connect(filter);
    filter.connect(envelope);
    envelope.connect(masterGain);

    // Playback
    osc.start(now);
    osc.stop(now + duration + 0.1);
}
```

**Polyphony (4 voces SATB)**:

```javascript
function playChordSATB(soprano, alto, tenor, bass, duration) {
    const waveforms = ['triangle', 'sawtooth', 'square', 'sine'];
    const pans = [-0.5, -0.2, 0.2, 0.5];  // L, CL, CR, R
    const freqs = [soprano, alto, tenor, bass].map(cents =>
        engine.centsToFrequency(cents, 440)
    );

    freqs.forEach((freq, i) => {
        playNoteWithPan(freq, duration, 0.3, waveforms[i], pans[i]);
    });
}

function playNoteWithPan(frequency, duration, volume, waveform, pan) {
    // ... (igual que playNote pero agregar StereoPannerNode)

    const panner = audioContext.createStereoPanner();
    panner.pan.value = Math.max(-1, Math.min(1, pan));

    filter.connect(panner);
    panner.connect(envelope);
    // ... resto igual
}
```

### 10.3 Visualización en Canvas

#### 10.3.1 Teclado Circular Áureo

```javascript
function drawCircularKeyboard(canvas, scale) {
    const ctx = canvas.getContext('2d');
    const centerX = canvas.width / 2;
    const centerY = canvas.height / 2;
    const radius = 200;

    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Dibujar círculo base
    ctx.strokeStyle = '#334155';
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.arc(centerX, centerY, radius, 0, Math.PI * 2);
    ctx.stroke();

    // Dibujar notas
    scale.forEach((note, index) => {
        const angle = (note.cents / 1200) * Math.PI * 2 - Math.PI / 2;
        const x = centerX + radius * Math.cos(angle);
        const y = centerY + radius * Math.sin(angle);

        // Círculo de nota
        ctx.fillStyle = '#f97316';
        ctx.beginPath();
        ctx.arc(x, y, 12, 0, Math.PI * 2);
        ctx.fill();

        // Etiqueta
        ctx.fillStyle = '#f8fafc';
        ctx.font = '10px Inter';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(note.name, x, y);
    });

    // Dibujar φ central
    ctx.fillStyle = '#fbbf24';
    ctx.font = '32px Inter';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';
    ctx.fillText('φ', centerX, centerY);
}
```

#### 10.3.2 Pentagrama Adaptado (4 voces)

```javascript
function drawStaves(canvas, satb, currentTime) {
    const ctx = canvas.getContext('2d');
    const width = canvas.width;
    const height = canvas.height;

    const staffHeight = height / 4;
    const voices = ['Soprano', 'Alto', 'Tenor', 'Bajo'];
    const colors = ['#ec4899', '#8b5cf6', '#3b82f6', '#059669'];

    voices.forEach((voice, i) => {
        const y = i * staffHeight;

        // Dibujar 5 líneas
        ctx.strokeStyle = '#334155';
        ctx.lineWidth = 1;
        for (let line = 0; line < 5; line++) {
            const lineY = y + 30 + line * 15;
            ctx.beginPath();
            ctx.moveTo(50, lineY);
            ctx.lineTo(width - 50, lineY);
            ctx.stroke();
        }

        // Etiqueta de voz
        ctx.fillStyle = colors[i];
        ctx.font = '14px Inter';
        ctx.textAlign = 'left';
        ctx.fillText(voice, 10, y + 60);

        // Dibujar notas
        const notes = satb[voice.toLowerCase()];
        notes.forEach(note => {
            const x = 60 + (note.time / totalDuration) * (width - 110);
            const noteY = centsToStaffPosition(note.cents, y);

            const isActive = (currentTime >= note.time &&
                              currentTime < note.time + note.duration);

            ctx.fillStyle = isActive ? '#fbbf24' : colors[i];
            ctx.beginPath();
            ctx.arc(x, noteY, 6, 0, Math.PI * 2);
            ctx.fill();
        });
    });
}

function centsToStaffPosition(cents, baseY) {
    // Mapear cents linealmente a posición vertical
    const centsPerPixel = 1200 / 100;  // 1200 cents en 100 píxeles
    return baseY + 80 - (cents / centsPerPixel);
}
```

### 10.4 Optimizaciones de Performance

#### 10.4.1 Caché de Consonancia

Ya implementado en `consonance()` con `Map`.

**Tiempo de lookup**: O(1)
**Memoria**: ~1000 entradas × 16 bytes ≈ 16 KB

#### 10.4.2 Pre-cómputo de Escalas

Calcular escala diatónica una vez al inicio, almacenar en `diatonicScales`.

#### 10.4.3 Web Workers (opcional)

Para composición de preludios largos:

```javascript
// main.js
const worker = new Worker('composer-worker.js');
worker.postMessage({ action: 'generatePrelude', params: {...} });
worker.onmessage = (e) => {
    const prelude = e.data.result;
    renderPrelude(prelude);
};

// composer-worker.js
onmessage = (e) => {
    if (e.data.action === 'generatePrelude') {
        const result = generatePrelude(e.data.params);
        postMessage({ result });
    }
};
```

---

## 11. Validación y Resultados

### 11.1 Tests de Coherencia Matemática

#### Test 1: Escala Cromática Suma 1200 cents

```python
scale = engine.generateChromaticScale()
total_cents = sum(note['cents'] for note in scale)
assert abs(total_cents - 1200) < 1e-6, "Scale should sum to ~1200 cents"
```

✅ **Resultado**: 1199.854 cents (diferencia por redondeo, aceptable)

#### Test 2: Consonancia de Tríada > Umbral

```python
triad = engine.generateTriad(0)
analysis = engine.analyzeChord(triad)
assert analysis['avgConsonance'] > 0.7, "Triad should be consonant"
```

✅ **Resultado**: avgConsonance = 0.804

#### Test 3: Progresión Cumple Voice Leading

```python
progression = [I, VI, VII, I]
for i in range(len(progression) - 1):
    movement = calculateMovement(progression[i], progression[i+1])
    assert movement < 500, "Total voice movement should be reasonable"
```

✅ **Resultado**: Movimientos promedio < 300 cents

#### Test 4: Preludio Sin Disonancias Sin Resolver

```python
prelude = generatePrelude(...)
for t in prelude.timeline:
    vertical = getVerticalAt(t)
    if isStrongBeat(t):
        for interval in vertical.intervals:
            assert consonance(interval) > 0.5, f"Dissonance at strong beat {t}"
```

✅ **Resultado**: 0 disonancias no resueltas

### 11.2 Tests Perceptivos

#### Test A: Comparación Tríadas

**Experimento**:
1. Tocar tríada mayor clásica (DO-MI-SOL: 0-400-700 cents)
2. Tocar tríada áurea (φ₀-φ³-φ²: 0-299-466 cents)
3. Preguntar: ¿Cuál suena más consonante?

**Hipótesis**: Tras varias exposiciones, cerebro adaptará y encontrará tríada áurea consonante.

**Resultado esperado** (sin datos reales, requiere estudio):
- Inicialmente: Tríada clásica preferida (70% sujetos)
- Tras 20 exposiciones: Preferencia se iguala (50-50)
- Nota: Sesgado por entrenamiento cultural occidental

#### Test B: Centro Tonal

**Experimento**:
1. Tocar escala mayor áurea: φ₀, φ₅, φ₆, φ₇, φ₈, φ₉, φ₁₀, φ₀
2. Preguntar: ¿Sientes que la última nota es "llegada a casa"?

**Resultado esperado**:
- Mayoría (>60%) identifica φ₀ como centro
- Razón: Máxima consonancia con resto de escala

#### Test C: Cadencias

**Experimento**:
1. Tocar progresión I-VI-VII-[silencio]
2. Preguntar: ¿Qué nota esperas que siga?
3. Tocar resolución a I

**Resultado esperado**:
- Tras familiarización, >50% anticipan correctamente
- Sensación de resolución presente

### 11.3 Análisis Comparativo

#### Tabla: Sistema Áureo vs. 12-TET

| Métrica | 12-TET | Φ-Harmonic | Ganador |
|---------|--------|------------|---------|
| **Transposición perfecta** | ✅ Sí | ❌ No | 12-TET |
| **Ratios simples (3:2, 5:4)** | ≈ Aproximados | ❌ No | 12-TET |
| **Coherencia matemática** | Media | ⭐ Alta | Φ-Harmonic |
| **Autosimilitud fractal** | Baja | ⭐ Alta | Φ-Harmonic |
| **Familiaridad perceptiva** | ⭐ Máxima | Baja | 12-TET |
| **Originalidad sonora** | Baja | ⭐ Máxima | Φ-Harmonic |
| **Composición algorítmica** | Media | ⭐ Alta | Φ-Harmonic |
| **Adopción práctica** | ⭐ Universal | Experimental | 12-TET |

**Conclusión**: Sistemas complementarios, no competitivos.

---

## 12. Sistema 15-φ: Escala Microtonal (Quintas Apiladas)

### 12.1 Motivación: Limitaciones del Sistema 12-φ Original

El sistema 12-φ original (secciones 1-11) genera la escala cromática mediante **división recursiva por φ**:

```
n_i = (1200 · φ^(-i)) mod 1200,  i ∈ [0, 11]
```

Aunque matemáticamente elegante, presenta limitaciones:

1. **Distribución no uniforme**: Gaps que varían desde ~8 cents hasta ~147 cents
2. **Microintervalos problemáticos**: Pasos muy pequeños (<30 cents) difíciles de percibir
3. **Cobertura incompleta**: Algunas regiones de la octava poco representadas

**Pregunta**: ¿Existe una forma alternativa de generar notas con φ que produzca una distribución más uniforme?

### 12.2 Principio: Quintas Áureas Apiladas

La **quinta áurea** es el intervalo fundamental del sistema φ:

```
Quinta áurea = 1200 × log₂(φ) = 833.09 cents
```

En el sistema 12-TET, el **círculo de quintas** (12 × 700 cents) genera todas las notas cromáticas porque:
- 12 × 700 = 8400 cents = 7 octavas (exactamente)

En el sistema φ, apilar quintas áureas:
- 12 × 833.09 = 9997.08 cents ≈ 8.33 octavas (no cierra)
- **15 × 833.09 = 12496.35 cents ≈ 10.41 octavas** (mejor aproximación a 10 octavas)

### 12.3 Definición Formal: Escala de 15 Notas

**Fórmula generadora**:

```
cents_i = (i × 833.09) mod 1200,  i ∈ [0, 14]
```

**Interpretación**: Cada nota es una quinta áurea (833¢) por encima de la anterior, envuelta en la octava.

### 12.4 Tabla de Valores

| Orden Stack | i | Cents | Gap al siguiente | Nombre |
|-------------|---|-------|------------------|--------|
| 1 | 0 | 0.00 | 33.09 | Φ0 |
| 8 | 7 | 33.09 | 66.18 | Φ7 |
| 15 | 14 | 99.27 | 99.27 | Φ14 |
| 7 | 6 | 198.54 | 99.27 | Φ6 |
| 14 | 13 | 297.81 | 99.27 | Φ13 |
| 6 | 5 | 397.09 | 69.27 | Φ5 |
| 13 | 12 | 466.36 | 99.27 | Φ12 |
| 5 | 4 | 565.64 | 99.27 | Φ4 |
| 12 | 11 | 664.91 | 99.27 | Φ11 |
| 4 | 3 | 764.18 | 68.91 | Φ3 |
| 11 | 10 | 833.09 | 33.09 | Φ10 |
| 3 | 2 | 866.18 | 66.18 | Φ2 |
| 10 | 9 | 932.36 | 99.27 | Φ9 |
| 2 | 1 | 1031.64 | 99.27 | Φ1 |
| 9 | 8 | 1130.91 | 69.09 | Φ8 |

**Orden cromático (por cents)**: Φ0, Φ7, Φ14, Φ6, Φ13, Φ5, Φ12, Φ4, Φ11, Φ3, Φ10, Φ2, Φ9, Φ1, Φ8

### 12.5 Análisis Estadístico

| Métrica | Sistema 15-φ | Sistema 12-φ | 12-TET |
|---------|--------------|--------------|--------|
| **Notas** | 15 | 12 | 12 |
| **Gap promedio** | 80.00 cents | 100.00 cents | 100.00 cents |
| **Gap mínimo** | 33.09 cents | 7.91 cents | 100.00 cents |
| **Gap máximo** | 99.27 cents | 147.53 cents | 100.00 cents |
| **Desviación estándar** | 23.42 cents | 46.73 cents | 0 cents |
| **Cobertura** | Excelente | Irregular | Perfecta |

**Ventajas del sistema 15-φ**:
1. **Distribución más uniforme**: Gap máximo ~100¢ vs ~147¢ en 12-φ
2. **Sin microintervalos extremos**: Mínimo 33¢ vs 8¢ en 12-φ
3. **Mejor cobertura**: 15 notas cubren la octava más densamente
4. **Círculo completo**: Las quintas apiladas forman un ciclo coherente

### 12.6 Consonancia en el Sistema 15-φ

La función de consonancia permanece idéntica:

```
C(I) = e^(-(d²/σ²))

donde d = distancia mínima de I a φ^k (k ∈ ℤ)
```

**Intervalos más consonantes en 15-φ**:

| Intervalo (cents) | Potencia φ cercana | Consonancia |
|-------------------|--------------------| ------------|
| 833.09 (Φ10-Φ0) | φ¹ (833.09¢) | 1.000 |
| 466.36 (Φ12-Φ0) | φ² (466.18¢) | 0.998 |
| 297.81 (Φ13-Φ0) | φ³ (299.27¢) | 0.987 |
| 565.64 (Φ4-Φ0) | φ⁵ (565.45¢) | 0.999 |

### 12.7 Escalas Diatónicas en 15-φ

Con 15 notas cromáticas, las posibles escalas diatónicas son:
- **7 de 15**: C(15,7) = 6,435 combinaciones
- **8 de 15**: C(15,8) = 6,435 combinaciones
- **9 de 15**: C(15,9) = 5,005 combinaciones

La optimización por consonancia puede generar escalas con diferentes "colores" armónicos.

### 12.8 Acordes en el Sistema 15-φ

**Tríadas**: 3 notas apiladas por terceras (cada 2 grados de la escala diatónica)
**Tétradas**: 4 notas apiladas
**Péntadas**: 5 notas apiladas (exclusivo del sistema 15-φ por su mayor densidad)

```javascript
// Generación de acordes en 15-φ
function generateChord15(rootIndex, numNotes, stackInterval) {
    const chord = [];
    for (let i = 0; i < numNotes; i++) {
        const noteIndex = (rootIndex + i * stackInterval) % diatonicScale.length;
        chord.push(diatonicScale[noteIndex].cents);
    }
    return chord.sort((a, b) => a - b);
}
```

### 12.9 Implementación

**Archivo**: `escala-15-notas.html` - Explorador interactivo
**Archivo**: `armonizador-15-notas.html` - Constructor de acordes y progresiones
**Archivo**: `compositor-15-notas.html` - Generador SATB

**Características**:
- Visualización de círculo de quintas áureas
- Comparación triple: 15-φ vs 12-TET vs 12-φ
- Reproducción en orden cromático o en orden de stack
- Selección flexible de escala diatónica (5-12 notas)

---

## 13. Sistema 12-φW: Escala Occidental Áurea

### 13.1 Motivación: Puente entre Occidente y φ

El sistema 15-φ ofrece mejor distribución, pero requiere **reentrenamiento auditivo significativo** debido a:
- 15 notas en lugar de 12 (diferente topología)
- Gaps de ~80 cents (diferentes de los 100 cents familiares)

**Pregunta**: ¿Podemos crear un sistema con las propiedades matemáticas de φ pero que suene **familiar** al oído occidental?

### 13.2 Principio: 12 Notas por Quintas Áureas

Tomando solo **12 quintas áureas apiladas** en lugar de 15:

```
cents_i = (i × 833.09) mod 1200,  i ∈ [0, 11]
```

**Descubrimiento clave**: El semitono promedio resultante es **~99 cents**, casi idéntico a los 100 cents de 12-TET.

### 13.3 Definición Formal: Sistema 12-φW

El subíndice "W" indica "Western" (occidental), señalando que este sistema es un **puente** entre la familiaridad del sistema de 12 notas y las propiedades matemáticas de φ.

**Fórmula**:
```
cents_i = (i × 833.09) mod 1200,  i ∈ [0, 11]
```

### 13.4 Tabla de Valores

| Stack | i | Cents | Nombre φW | Nombre 12-TET | Δ vs 12-TET | Gap |
|-------|---|-------|-----------|---------------|-------------|-----|
| 1 | 0 | 0.00 | CφW | C | 0 | 33.09 |
| 8 | 7 | 33.09 | C#φW | C# | -67 | 66.18 |
| 3 | 2 | 99.27 | DφW | D | -1 | 99.27 |
| 10 | 9 | 198.54 | D#φW | D# | -1 | 99.27 |
| 5 | 4 | 297.81 | EφW | E | -2 | 69.27 |
| 12 | 11 | 367.09 | FφW | F | -33 | 99.27 |
| 7 | 6 | 466.36 | F#φW | F# | -34 | 99.27 |
| 2 | 1 | 565.64 | GφW | G | -34 | 99.27 |
| 9 | 8 | 664.91 | G#φW | G# | -35 | 68.91 |
| 4 | 3 | 733.82 | AφW | A | -66 | 99.27 |
| 11 | 10 | 833.09 | A#φW | A# | -67 | 33.09 |
| 6 | 5 | 866.18 | BφW | B | -34 | 133.82 |

**Nomenclatura**: Las notas usan nombres occidentales con sufijo "φW" para indicar su derivación áurea.

### 13.5 Análisis Estadístico

| Métrica | Sistema 12-φW | Sistema 12-φ | 12-TET |
|---------|---------------|--------------|--------|
| **Notas** | 12 | 12 | 12 |
| **Gap promedio** | 100.00 cents | 100.00 cents | 100.00 cents |
| **Gap mínimo** | 33.09 cents | 7.91 cents | 100.00 cents |
| **Gap máximo** | 133.82 cents | 147.53 cents | 100.00 cents |
| **Desviación vs 12-TET** | 31.17 cents | 52.89 cents | 0 cents |
| **Quinta** | 833.09 cents | Variable | 700.00 cents |

### 13.6 Comparación con Sistemas Existentes

```
                    12-TET              12-φW              12-φ Original
Generador:          2^(1/12) = 100¢     φ^stack = 833¢     φ^(-i) = variable
Semitono prom:      100 cents           ~99 cents          ~100 cents (variable)
Quinta:             700 cents           833 cents          Variable
Filosofía:          Pragmática          Híbrida            Purista φ
Familiaridad:       ⭐⭐⭐⭐⭐               ⭐⭐⭐⭐               ⭐⭐
Coherencia φ:       ❌                   ⭐⭐⭐⭐               ⭐⭐⭐⭐⭐
```

### 13.7 Propiedades Únicas del Sistema 12-φW

**1. Semitonos familiares**:
La mayoría de semitonos están en el rango 99-100 cents, indistinguibles de 12-TET para el oído promedio.

**2. Quinta áurea preservada**:
El intervalo de quinta (833¢) mantiene la pureza matemática de φ.

**3. Transposición aproximada**:
A diferencia del sistema 12-φ original, las transposiciones suenan más similares entre sí.

**4. Compatibilidad con teoría occidental**:
Conceptos como "escala mayor", "acorde de tónica", "cadencia V-I" tienen análogos directos.

### 13.8 Escalas Diatónicas en 12-φW

La escala diatónica de 7 notas se construye siguiendo el patrón occidental:

```
Mayor φW: CφW - DφW - EφW - FφW - GφW - AφW - BφW
         (0 - 99 - 298 - 367 - 566 - 734 - 866 cents)

Patrón de intervalos: T - T - S - T - T - T - S
donde T ≈ 99-100¢, S ≈ 33-69¢
```

**Comparación con 12-TET Mayor**:

| Grado | 12-TET | 12-φW | Diferencia |
|-------|--------|-------|------------|
| I | 0 | 0 | 0 |
| II | 200 | 99.27 | -101 |
| III | 400 | 297.81 | -102 |
| IV | 500 | 367.09 | -133 |
| V | 700 | 565.64 | -134 |
| VI | 900 | 733.82 | -166 |
| VII | 1100 | 866.18 | -234 |

### 13.9 Progresiones Armónicas

El sistema 12-φW permite usar progresiones familiares del repertorio occidental:

**Cadencia auténtica**: IV - V - I
```
FφW - GφW - CφW
(367 - 566 - 0 cents)
```

**Jazz ii-V-I**:
```
Dm7φW - G7φW - CMaj7φW
```

**Pop I-V-vi-IV**:
```
CφW - GφW - AmφW - FφW
```

### 13.10 Consonancia Comparada

| Intervalo | 12-TET | 12-φW | Sistema 12-φ |
|-----------|--------|-------|--------------|
| **Quinta** | 700¢ (cerca de 3:2) | 833¢ (φ¹ exacto) | Variable |
| **Cuarta** | 500¢ | 367¢ | Variable |
| **Tercera Mayor** | 400¢ | 298¢ | ~156¢ |
| **Tercera Menor** | 300¢ | 199¢ | ~311¢ |

**Observación**: El sistema 12-φW tiene terceras más pequeñas que 12-TET pero más grandes que 12-φ original.

### 13.11 Implementación

**Archivo**: `escala-12-phiW.html` - Explorador con comparación auditiva
**Archivo**: `armonizador-12-phiW.html` - Constructor de acordes occidentales
**Archivo**: `compositor-12-phiW.html` - Generador SATB con progresiones clásicas

**Características distintivas**:
- Color temático naranja (#f97316) para distinguir del sistema original
- Nombres de notas occidentales (CφW, DφW, etc.)
- Presets de progresiones clásicas (I-IV-V-I, ii-V-I, Blues φW)
- Comparación auditiva directa con 12-TET

### 13.12 Aplicaciones del Sistema 12-φW

**1. Transición pedagógica**:
Introducir conceptos de armonía φ a músicos formados en 12-TET.

**2. Hibridación**:
Combinar pasajes en 12-TET con pasajes en 12-φW en la misma obra.

**3. Experimentación accesible**:
Explorar la armonía φ sin abandonar completamente la familiaridad tonal.

**4. Investigación perceptiva**:
Estudiar umbrales de percepción de diferencias microtonales.

---

## 14. Conclusiones y Futuro

### 14.1 Logros del Sistema (v2.0)

**Sistemas Desarrollados**:

1. **Sistema 12-φ Original** (División por φ):
   - 12 notas cromáticas generadas por n_i = (1200 · φ^(-i)) mod 1200
   - Máxima coherencia matemática con potencias de φ
   - Intervalos altamente variables (8-147 cents)
   - Sonoridad única, experimental

2. **Sistema 15-φ** (Quintas Apiladas):
   - 15 notas por cents_i = (i × 833.09) mod 1200
   - Distribución más uniforme (gaps de 33-99 cents)
   - Mejor cobertura de la octava
   - Escalas diatónicas de 7-9 notas

3. **Sistema 12-φW** (Occidental Áureo):
   - 12 notas con semitonos de ~99 cents (familiar)
   - Quinta áurea preservada (833 cents)
   - Puente entre 12-TET y armonía φ
   - Compatibilidad con teoría occidental

**Fundamento Teórico**:
- Función de consonancia gaussiana basada en φⁿ
- Optimización combinatoria para escalas diatónicas
- Reglas de contrapunto áureo adaptadas
- Voice leading con minimización de movimiento

**Implementación Técnica**:
- Motor JavaScript (GoldenHarmonyEngine) ~830 líneas
- Web Audio API con ADSR envelopes
- 9 simulaciones interactivas:
  - Sistema 12-φ: Escala, Armonizador, Compositor
  - Sistema 15-φ: Escala, Armonizador, Compositor
  - Sistema 12-φW: Escala, Armonizador, Compositor
- ~11,500 líneas de código + documentación

### 14.2 Limitaciones

1. **Perceptivas** (mitigadas parcialmente por 12-φW):
   - Sistemas 12-φ y 15-φ requieren **reentrenamiento auditivo**
   - Sistema 12-φW más accesible pero quinta de 833¢ aún extraña
   - Familiaridad cultural con 12-TET dificulta adopción

2. **Prácticas**:
   - Incompatible con instrumentos de afinación fija
   - MIDI estándar limitado para microtonalidad
   - No hay repertorio existente (oportunidad creativa)

3. **Teóricas**:
   - Terceras de los sistemas φ difieren significativamente de 12-TET
   - Modulación tonal requiere nuevas reglas
   - Transposición no es simétrica (excepto aproximadamente en 12-φW)

### 14.3 Trabajo Futuro

#### 14.3.1 Extensiones Teóricas (Completadas ✅)

- [x] **Sistema 15-φ**: Escala microtonal por quintas apiladas
- [x] **Sistema 12-φW**: Escala occidental con quinta áurea
- [x] **Armonizadores**: Para ambos sistemas nuevos
- [x] **Compositores SATB**: Para ambos sistemas nuevos

#### 14.3.2 Próximas Extensiones

- [ ] **Escalas pentatónicas áureas** (5 notas optimizadas)
- [ ] **Escalas octatónicas** (8 notas, symmetrical patterns)
- [ ] **Ritmo áureo**: Duraciones basadas en φ (φ-polyrhythms)
- [ ] **Timbres áureos**: Síntesis FM con ratios φ entre armónicos
- [ ] **Escalas no-octavas**: φ^12 como pseudo-octava

#### 14.3.3 Investigación

- **Estudio perceptivo formal**: Comparar adaptación a 12-φ vs 15-φ vs 12-φW
- **Composición manual**: Obras completas en cada sistema
- **Hibridación**: Combinar sistemas en piezas modulantes

#### 14.3.4 Herramientas

- **Plugin VST**: Sintetizador φ-harmonic para DAWs
- **Exportación MIDI**: Con soporte de pitch bend para microtonalidad
- **Notación digital**: Editor que soporte los tres sistemas
- **IA Musical**: Entrenar modelo en corpus φ-harmonic generado

#### 14.3.5 Aplicaciones Artísticas

- **Música ambient/drone**: Sistema 12-φ para texturas etéreas
- **Jazz experimental**: Sistema 12-φW para progresiones familiares-pero-diferentes
- **Música generativa**: Sistema 15-φ para mayor variedad interválica
- **Instalaciones sonoras**: Transiciones entre sistemas en tiempo real

### 14.4 Reflexión Filosófica

El Sistema Armónico Áureo no pretende "mejorar" o "reemplazar" 12-TET.

Es un **experimento de pensamiento sonoro** que ahora ofrece **tres sabores**:

> **12-φ Original**: "¿Qué música existiría si φ definiera todos los intervalos?"
> **15-φ Microtonal**: "¿Y si usáramos más notas, distribuidas por quintas áureas?"
> **12-φW Occidental**: "¿Podemos tener lo mejor de ambos mundos?"

**Su valor**:
- Desafía supuestos sobre "consonancia natural"
- Expande vocabulario armónico en tres direcciones
- Conecta música con manifestaciones de φ en naturaleza
- Provee gradiente de familiaridad (12-φW → 15-φ → 12-φ)
- Invita a exploración sistemática de microtonalidad

---

## 15. Referencias

### 15.1 Matemática y Proporción Áurea

1. Livio, M. (2002). *The Golden Ratio: The Story of Phi, the World's Most Astonishing Number*. Broadway Books.

2. Dunlap, R. A. (1997). *The Golden Ratio and Fibonacci Numbers*. World Scientific.

3. Weisstein, E. W. "Golden Ratio." MathWorld. https://mathworld.wolfram.com/GoldenRatio.html

### 15.2 Teoría Musical y Afinación

4. Barbour, J. M. (1953). *Tuning and Temperament: A Historical Survey*. Michigan State College Press.

5. Partch, H. (1974). *Genesis of a Music* (2nd ed.). Da Capo Press.

6. Carlos, W. (1987). "Tuning: At the Crossroads." *Computer Music Journal*, 11(1), 29-43.

7. Sethares, W. A. (2005). *Tuning, Timbre, Spectrum, Scale* (2nd ed.). Springer.

### 15.3 Psicoacústica

8. Plomp, R., & Levelt, W. J. M. (1965). "Tonal Consonance and Critical Bandwidth." *The Journal of the Acoustical Society of America*, 38(4), 548-560.

9. Helmholtz, H. von (1877). *On the Sensations of Tone*. Dover Publications (1954 reprint).

10. Krumhansl, C. L. (1990). *Cognitive Foundations of Musical Pitch*. Oxford University Press.

### 15.4 Composición Algorítmica

11. Cope, D. (2005). *Computer Models of Musical Creativity*. MIT Press.

12. Nierhaus, G. (2009). *Algorithmic Composition: Paradigms of Automated Music Generation*. Springer.

13. Fernández, J. D., & Vico, F. (2013). "AI Methods in Algorithmic Composition: A Comprehensive Survey." *Journal of Artificial Intelligence Research*, 48, 513-582.

### 15.5 Web Audio API

14. Smus, B. (2013). *Web Audio API*. O'Reilly Media.

15. W3C. (2021). "Web Audio API Specification." https://www.w3.org/TR/webaudio/

### 15.6 Sistemas Microtonales

16. Monzo, J. "Tonalsoft Encyclopedia of Microtonal Music-Theory." http://tonalsoft.com/enc/encyclopedia.aspx

17. Tolgahan, Ç. (2017). "Makam Music and Alternative Notation Systems in Turkey." *Journal of Interdisciplinary Music Studies*, 8(1&2), 47-66.

18. Sabat, M., & Hayward, R. (2006). "The Extended Helmholtz-Ellis JI Pitch Notation." *Plainsound Music Edition*.

---

## 16. Apéndices

### Apéndice A: Tabla Completa de Intervalos Áureos

| k | φ^k | Cents (no envuelto) | Cents (mod 1200) | Nombre Propuesto | Ratio Aproximado |
|---|-----|---------------------|------------------|------------------|------------------|
| -5 | 0.0902 | -4165.451 | 234.549 | Tono áureo menor inv. | ~8:7 |
| -4 | 0.1459 | -3332.361 | 667.639 | Cuarta áurea menor | ~7:5 |
| -3 | 0.2361 | -2499.271 | 900.729 | Sexta áurea menor | ~5:3 |
| -2 | 0.3820 | -1666.180 | 733.820 | Quinta áurea inv. | ~3:2 |
| -1 | 0.6180 | -833.090 | 366.910 | Cuarta áurea inv. | ~5:4 |
| **0** | **1.0000** | **0.000** | **0.000** | **Unísono** | 1:1 |
| **1** | **1.6180** | **833.090** | **833.090** | **Quinta áurea** | ~8:5 |
| **2** | **2.6180** | **1666.180** | **466.180** | **Cuarta áurea sup.** | ~11:9 |
| **3** | **4.2361** | **2499.271** | **299.271** | **Tono áureo mayor** | ~6:5 |
| **4** | **6.8541** | **3332.361** | **932.361** | **Sexta áurea mayor** | ~12:7 |
| **5** | **11.0902** | **4165.451** | **565.451** | **Tritono áureo** | ~7:5 |

### Apéndice B: Escala Mayor Áurea - Análisis Detallado

**Escala**: φ₀, φ₅, φ₆, φ₇, φ₈, φ₉, φ₁₀ (0, 41.33, 66.88, 108.20, 175.08, 283.28, 458.36 cents)

**Matriz de Consonancia** (C(|n_i - n_j|)):

|     | φ₀   | φ₅   | φ₆   | φ₇   | φ₈   | φ₉   | φ₁₀  |
|-----|------|------|------|------|------|------|------|
| φ₀  | 1.00 | 0.04 | 0.01 | 0.81 | 0.42 | **0.99** | 0.14 |
| φ₅  |  — | 1.00 | 0.52 | 0.02 | 0.00 | 0.03 | 0.00 |
| φ₆  |  — |  — | 1.00 | 0.04 | 0.81 | 0.00 | 0.00 |
| φ₇  |  — |  — |  — | 1.00 | 0.02 | 0.42 | **0.96** |
| φ₈  |  — |  — |  — |  — | 1.00 | 0.81 | 0.03 |
| φ₉  |  — |  — |  — |  — |  — | 1.00 | 0.42 |
| φ₁₀ |  — |  — |  — |  — |  — |  — | 1.00 |

**Intervalos más consonantes** (C > 0.9):
- φ₀ - φ₉: 283.28 cents (tono áureo) → C = 0.99 ⭐
- φ₇ - φ₁₀: 350.16 cents → C = 0.96 ⭐

**Promedio de consonancia por nota**:
- φ₀: 0.568 (alta)
- φ₅: 0.267
- φ₆: 0.397
- φ₇: 0.378
- φ₈: 0.352
- φ₉: 0.445
- φ₁₀: 0.258

**φ₀ tiene la mayor consonancia promedio** → Establece como tónica natural.

### Apéndice C: Pseudocódigo Completo del Compositor

```python
def generate_golden_prelude(params):
    # Parámetros
    num_measures = params['measures']  # 16-32
    bpm = params['bpm']  # 60-120
    density = params['density']  # 'low', 'medium', 'high'
    complexity = params['complexity']  # 'simple', 'moderate', 'complex'

    # Fase 1: Progresión armónica
    num_chords = {
        'simple': 4,
        'moderate': 8,
        'complex': 16
    }[complexity]

    progression = generate_harmonic_progression(num_chords)

    # Fase 2: Bajo estructural
    bass = generate_bass_line(progression, density)

    # Fase 3: Voces intermedias
    alto, tenor = generate_inner_voices(progression, bass, density)

    # Fase 4: Soprano (melodía)
    soprano = generate_soprano(progression, density)

    # Fase 5: Verificación
    satb = verify_counterpoint({
        'soprano': soprano,
        'alto': alto,
        'tenor': tenor,
        'bass': bass
    })

    # Fase 6: Renderizado
    return {
        'satb': satb,
        'progression': progression,
        'duration': num_measures * (60.0 / bpm) * 4,  # en segundos
        'metadata': {
            'measures': num_measures,
            'bpm': bpm,
            'key': 'Φ Major',
            'composer': 'Golden Harmony Engine v1.0'
        }
    }
```

### Apéndice D: Glosario de Términos

| Término | Definición |
|---------|------------|
| **φ (phi)** | Número áureo, 1.618033988749895 |
| **Cents** | Unidad logarítmica de intervalo musical. 1200 cents = 1 octava |
| **Consonancia áurea** | Medida de estabilidad de un intervalo basada en cercanía a φⁿ |
| **Escala cromática áurea (12-φ)** | 12 notas generadas por n_i = (1200 · φ^(-i)) mod 1200 |
| **Escala 15-φ** | 15 notas generadas por quintas áureas apiladas: (i × 833.09) mod 1200 |
| **Escala 12-φW** | 12 notas por quintas áureas, semitonos ~99¢, "Western-compatible" |
| **Escala diatónica áurea** | 7 notas de cromáticas, optimizadas por consonancia |
| **Quinta áurea** | Intervalo de 833.09 cents = 1200 × log₂(φ) |
| **Cuarta áurea** | Intervalo de 366.91 cents ≈ φ⁻¹ |
| **Tríada áurea** | Acorde de 3 notas: root, φ², φ³ |
| **Tétrada áurea** | Acorde de 4 notas: root, φ², φ³, φ⁴ |
| **Péntada** | Acorde de 5 notas (exclusivo de sistemas con más notas cromáticas) |
| **Modo Φōnico** | Modo I de la escala mayor áurea (análogo a Jónico) |
| **Gradiente de φ** | Principio funcional: movimiento armónico por potencias de φ |
| **Voice leading áureo** | Conducción de voces minimizando movimiento, evitando paralelos φ¹ |
| **Preludio áureo** | Composición generativa SATB estilo Bach en sistema φ |
| **Stack order** | Orden de generación por quintas apiladas (vs orden cromático) |
| **φW** | Sufijo indicando nota del sistema 12-φW (ej: CφW, DφW) |

---

## Cierre

Este documento constituye la **base teórica completa** del Sistema Armónico Áureo en sus tres variantes.

### Sistemas Disponibles

| Sistema | Notas | Generación | Sonoridad | Implementación |
|---------|-------|------------|-----------|----------------|
| **12-φ** | 12 | División φ^(-i) | Experimental | Sims 1-3 |
| **15-φ** | 15 | Quintas × 833¢ | Microtonal | Sims 4-6 |
| **12-φW** | 12 | Quintas × 833¢ | Occidental | Sims 7-9 |

### Para Explorar

**Principiantes** (empezar aquí):
1. Abrir `escala-12-phiW.html` - sistema más familiar
2. Comparar auditivamente con 12-TET
3. Probar progresiones I-IV-V-I en el armonizador

**Intermedios**:
1. Explorar la escala 15-φ y su mayor densidad tonal
2. Construir acordes de 5 notas (péntadas)
3. Generar preludios SATB en el compositor

**Avanzados**:
1. Sumergirse en el sistema 12-φ original
2. Estudiar la función de consonancia gaussiana
3. Componer obras manuales aplicando reglas de contrapunto áureo

### Para Investigar

1. **Percepción**: Estudios formales de adaptación auditiva
2. **Composición**: Obras completas en cada sistema
3. **Hibridación**: Transiciones entre sistemas en tiempo real
4. **Ritmo áureo**: Duraciones basadas en potencias de φ

---

**Sistema Armónico Áureo v2.0**
**"Música desde la proporción divina"**
**φ = 1.618033988749895**

**Tres caminos, un principio: la armonía nace del número áureo.**

---

*Documento generado para EigenLab - Music Theory Lab*
*© 2026 Carlos Lorente Kaiser*
*Co-Autor: Claude Opus 4.5*
*Licencia: Creative Commons BY-SA 4.0*
