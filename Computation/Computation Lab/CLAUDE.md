# Computation Lab - Documentación Técnica

## Descripción

El **Computation Lab** contiene **6 simulaciones** que cubren algoritmos de ordenamiento, estructuras de datos, autómatas finitos, modelo de memoria, redes neuronales y lógica digital. Implementa visualización interactiva de conceptos fundamentales de ciencias de la computación.

## Simulaciones (6 Total)

1. **Bubble Sort Race** - Ordenamiento burbuja con carrera visual
2. **Binary Search Tree** - Árbol binario con CRUD completo
3. **Finite Automata** - DFA con 4 presets
4. **Stack & Heap Visualizer** - Modelo de memoria C/bajo nivel
5. **Perceptron Playground** - Red neuronal binaria interactiva
6. **Logic Gates Sandbox** - Compuertas lógicas + circuitos

## Algoritmos Implementados

### Bubble Sort (Ordenamiento)

**Algoritmo:**
```javascript
for (i = 0; i < n-1; i++) {
    for (j = 0; j < n-i-1; j++) {
        if (arr[j] > arr[j+1]) {
            swap(arr[j], arr[j+1])
            swaps++
        }
        comparisons++
    }
}
```

**Complejidad:**
- Mejor: O(n) [ya ordenado]
- Promedio: O(n²)
- Peor: O(n²) [reverso]
- Espacio: O(1)
- Estable: Sí

**Optimización:** Early exit si no hay cambios en una pasada

### Binary Search Tree

**Clase BST:**
```javascript
class TreeNode {
    constructor(value) {
        this.value = value
        this.left = null
        this.right = null
    }
}

class BST {
    insert(value)     // O(log n) promedio, O(n) peor
    search(value)     // O(log n) promedio
    delete(value)     // O(log n) promedio
    inorder()         // O(n) - izq-raíz-der
    preorder()        // O(n) - raíz-izq-der
    postorder()       // O(n) - izq-der-raíz
    levelorder()      // O(n) - BFS
    getHeight()       // O(n)
}
```

**Delete de dos hijos:** Inorder successor (mínimo del subárbol derecho)

### Finite Automata (DFA)

**Definición formal:** M = (Q, Σ, δ, q₀, F)
- Q: Conjunto de estados
- Σ: Alfabeto
- δ: Función de transición (Q × Σ → Q)
- q₀: Estado inicial
- F: Estados de aceptación

**4 Presets:**

1. **endsWith01** - Cadenas binarias terminadas en "01"
   - Estados: q0, q1, q2
   - Transiciones: 6

2. **evenZeros** - Número par de ceros
   - Estados: even, odd
   - Transiciones: 4

3. **abStarA** - Patrón ab*a (a + b* + a)
   - Estados: q0, q1, q2, dead
   - Transiciones: 8

4. **divisibleBy3** - Números binarios divisibles por 3
   - Estados: r0, r1, r2 (resto mod 3)
   - Transiciones: 6

**Ejecución:** Lectura paso a paso del input, resultado: ACEPTADO/RECHAZADO

### Stack & Heap (Memoria)

**Conceptos:**
- **Stack:** LIFO, frames de función (variables locales)
- **Heap:** Memoria dinámica (malloc/free)
- **Variables:** int, pointers (referencia a heap)

**Instrucciones:**
- `func` - Declarar función
- `var` - Variable local
- `malloc` - Asignar memoria heap
- `assign` - Asignar valor
- `call` - Llamar función
- `free` - Liberar memoria
- `return` - Retornar de función

**Visualización:** Punteros con líneas Bezier punteadas (stack → heap)

### Perceptrón (Red Neuronal)

**Modelo:** y = sign(w · x + b)

**Perceptron Learning Rule:**
```javascript
for each (x, y) in dataset:
    ŷ = sign(w·x + b)
    if ŷ ≠ y:
        w += α(y - ŷ)x    // Actualizar pesos
        b += α(y - ŷ)     // Actualizar bias
```

**Métodos:**
- `predict(x)` - Predicción binaria O(2)
- `train(x, target)` - Regla de aprendizaje
- `getDecisionBoundaryY(x1)` - Ecuación frontera: w₁x₁ + w₂x₂ + b = 0

**Datasets presets:**
- **linear:** y = 0 (horizontal)
- **diagonal:** y = x
- **vertical:** x = 0
- **xor:** Inseparable (demuestra limitación perceptrón)

**Visualización:** Gradient de decisión, vector normal, puntos mal clasificados

### Logic Gates (Lógica Digital)

**Compuertas implementadas:** AND, OR, NOT, XOR, NAND, NOR, XNOR, INPUT, OUTPUT

**Definiciones:**
```javascript
AND:   (inputs) => inputs.every(v => v)
OR:    (inputs) => inputs.some(v => v)
NOT:   (inputs) => !inputs[0]
XOR:   (inputs) => inputs.reduce((a,b) => a !== b, false)
NAND:  (inputs) => !inputs.every(v => v)
NOR:   (inputs) => !inputs.some(v => v)
XNOR:  (inputs) => !inputs.reduce((a,b) => a !== b, false)
```

**Evaluación:** Topological sort para orden correcto

**4 Presets de circuitos:**
1. **Half Adder** - XOR (suma) + AND (carry)
2. **SR Latch** - Flip-flop basado en NOR (memoria 1 bit)
3. **XOR from NAND** - XOR usando solo NAND (compuerta universal)
4. **MUX 2:1** - Multiplexor 2-a-1 (selector)

**Tabla de verdad:** Generada dinámicamente para cualquier circuito

## Complejidad Algorítmica

| Algoritmo | Mejor | Promedio | Peor | Espacio | Estable |
|-----------|-------|----------|------|---------|---------|
| Bubble Sort | O(n) | O(n²) | O(n²) | O(1) | Sí |
| BST Insert | O(log n) | O(log n) | O(n) | O(1) | N/A |
| BST Search | O(log n) | O(log n) | O(n) | O(log n) | N/A |
| DFA Ejecución | O(n) | O(n) | O(n) | O(1) | N/A |
| Perceptron Train | O(1) | O(1) | O(1) | O(1) | N/A |
| Gates (AND/OR/NOT) | O(1) | O(1) | O(1) | O(1) | N/A |

## Tecnología

### Rendering
- **Canvas 2D API:** Todas las simulaciones
- **DPR (Device Pixel Ratio):** `ctx.scale(dpr, dpr)` para Retina

### Técnicas Canvas

| Simulación | Técnicas |
|------------|----------|
| Bubble Sort | Barras rectangulares, índices superpuestos, flechas de comparación |
| BST | Círculos para nodos, líneas para aristas, sombras para highlight |
| DFA | Círculos de estado (simple + doble), transiciones curvadas, self-loops |
| Stack/Heap | Rectángulos para frames/bloques, Bezier curves para punteros |
| Perceptron | Gradient de fondo, grid, línea de frontera, puntos con colores |
| Logic Gates | Rectángulos redondeados, puertos I/O, wires curvadas |

### Interfaz de Usuario

**Header con navegación:**
```
EigenLab / Computation Lab / [Simulación]
[Enlace a guía]
[Badge de estado]
```

**Canvas principal:** Flex: 1, aspecto 16:9

**Panel lateral:** 340px
- Ecuación principal
- Explicación rápida
- Sliders con valores
- Botones de control
- Estadísticas
- Leyenda de colores

## Paleta de Colores

- **Fondo:** `#030712` (casi negro)
- **Secundario:** `#0f172a` (azul oscuro)
- **Acento:** `#3b82f6` (azul)
- **Verde:** `#22c55e` (completado)
- **Amarillo:** `#fbbf24` (activo)
- **Rojo:** `#ef4444` (error)

## Referencias Cruzadas

### Con Mathematics
- **Complejidad:** O(n²), O(log n) ↔ Análisis asintótico
- **Árboles binarios:** BST ↔ Topología de grafos
- **Autómatas:** DFA ↔ Teoría de lenguajes formales
- **Redes neuronales:** Perceptrón ↔ Machine Learning

### Futuras simulaciones
- Algoritmo de Euclides (MCD)
- Hashing y colisiones (tabla hash)
- Grafos (DFS/BFS)
- Programación dinámica (Fibonacci, Knapsack)
- Compresión de datos (Huffman coding)
- Algoritmo A* (pathfinding)

## Referencias

**Total:** 6 simulaciones, ~8,368 líneas de código

**Desglose:**
- HTML/Estructura: ~8.5%
- CSS/Estilos: ~27.9%
- JavaScript/Lógica: ~63.6%

---

**Última actualización:** 2026-01-10
