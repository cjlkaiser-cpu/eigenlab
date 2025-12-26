# Computation Lab - Especificaciones Técnicas

## Benchmark de Calidad (basado en Math Visual Lab)

| Criterio | Estándar Mínimo |
|----------|-----------------|
| **Rigor algorítmico** | Implementación canónica, sin atajos |
| **Complejidad correcta** | Big O real, no simulado |
| **Visualización** | Smooth rendering, colores con propósito |
| **Interactividad** | Parámetros que afectan el comportamiento real |
| **Código** | Separación clara: algoritmo / visualización / UI |

---

## Fase 1: Especificaciones Detalladas

---

### 1. BUBBLE SORT RACE (`bubble-sort-race.html`)

#### 1.1 Algoritmo Canónico

```javascript
function bubbleSort(arr) {
    const n = arr.length;
    let comparisons = 0;
    let swaps = 0;

    for (let i = 0; i < n - 1; i++) {
        let swapped = false;

        for (let j = 0; j < n - i - 1; j++) {
            comparisons++;

            if (arr[j] > arr[j + 1]) {
                // Swap
                [arr[j], arr[j + 1]] = [arr[j + 1], arr[j]];
                swaps++;
                swapped = true;
            }
        }

        // Optimización: si no hubo swaps, ya está ordenado
        if (!swapped) break;
    }

    return { comparisons, swaps };
}
```

#### 1.2 Complejidad Real

| Caso | Comparaciones | Swaps |
|------|---------------|-------|
| Peor (inverso) | n(n-1)/2 | n(n-1)/2 |
| Promedio | n(n-1)/2 | n(n-1)/4 |
| Mejor (ordenado) | n-1 | 0 |

**Big O:** O(n²) tiempo, O(1) espacio

#### 1.3 Visualización

```
Estado de barras:
- DEFAULT (gris):     #64748b - Sin procesar
- COMPARING (amarillo): #fbbf24 - Par actual siendo comparado
- SWAPPING (rojo):    #ef4444 - Intercambiándose
- SORTED (verde):     #22c55e - En posición final
- PIVOT (cyan):       #22d3ee - (para otros algoritmos)
```

**Animación paso a paso:**
1. Resaltar par `arr[j]` y `arr[j+1]` en amarillo
2. Si swap: animar intercambio (transición CSS o lerp manual)
3. Tras cada pasada exterior: marcar último elemento como SORTED
4. Al finalizar: todos verdes

#### 1.4 Controles

| Control | Rango | Default | Efecto |
|---------|-------|---------|--------|
| Elementos | 10-100 | 30 | Cantidad de barras |
| Velocidad | 1-100 | 50 | ms entre comparaciones |
| Auto-step | toggle | off | Avanza automáticamente |

**Botones:**
- Shuffle: Fisher-Yates shuffle
- Step: Una comparación
- Play/Pause: Animación continua
- Reset: Volver a estado inicial

#### 1.5 Métricas en Tiempo Real

```
Comparaciones: 0 / 435 (máx teórico)
Swaps: 0 / 435 (máx teórico)
Pasadas completadas: 0 / 29
Tiempo: 0.00s
Estado: [Ejecutando / Pausado / Completado]
```

#### 1.6 Extras Educativos

- **Modo comparación**: Ejecutar Bubble vs Selection vs Insertion lado a lado
- **Caso peor**: Botón para generar array inverso
- **Caso mejor**: Botón para generar array ordenado

---

### 2. BINARY SEARCH TREE (`binary-search-tree.html`)

#### 2.1 Estructura de Datos

```javascript
class TreeNode {
    constructor(value) {
        this.value = value;
        this.left = null;
        this.right = null;
        this.x = 0;      // Posición para renderizado
        this.y = 0;
        this.highlight = false;
    }
}

class BST {
    constructor() {
        this.root = null;
    }

    insert(value) {
        const newNode = new TreeNode(value);

        if (!this.root) {
            this.root = newNode;
            return { path: [], inserted: true };
        }

        const path = [];  // Para animación
        let current = this.root;

        while (true) {
            path.push(current);

            if (value < current.value) {
                if (!current.left) {
                    current.left = newNode;
                    return { path, inserted: true };
                }
                current = current.left;
            } else if (value > current.value) {
                if (!current.right) {
                    current.right = newNode;
                    return { path, inserted: true };
                }
                current = current.right;
            } else {
                // Valor duplicado
                return { path, inserted: false, reason: 'duplicate' };
            }
        }
    }

    search(value) {
        const path = [];
        let current = this.root;

        while (current) {
            path.push(current);

            if (value === current.value) {
                return { found: true, path, node: current };
            }

            current = value < current.value ? current.left : current.right;
        }

        return { found: false, path };
    }

    delete(value) {
        // Implementar los 3 casos:
        // 1. Nodo hoja: eliminar directamente
        // 2. Un hijo: reemplazar con hijo
        // 3. Dos hijos: reemplazar con sucesor inorder
    }

    // Recorridos
    inorder(node = this.root, result = []) {
        if (node) {
            this.inorder(node.left, result);
            result.push(node.value);
            this.inorder(node.right, result);
        }
        return result;
    }

    preorder(node = this.root, result = []) {
        if (node) {
            result.push(node.value);
            this.preorder(node.left, result);
            this.preorder(node.right, result);
        }
        return result;
    }

    postorder(node = this.root, result = []) {
        if (node) {
            this.postorder(node.left, result);
            this.postorder(node.right, result);
            result.push(node.value);
        }
        return result;
    }
}
```

#### 2.2 Complejidad

| Operación | Promedio | Peor (desbalanceado) |
|-----------|----------|---------------------|
| Insert | O(log n) | O(n) |
| Search | O(log n) | O(n) |
| Delete | O(log n) | O(n) |

#### 2.3 Layout del Árbol (Algoritmo de Posicionamiento)

```javascript
function calculatePositions(root, canvasWidth, canvasHeight) {
    if (!root) return;

    const levelHeight = 60;
    const minNodeSpacing = 40;

    // Asignar posición X usando inorder traversal
    let xPosition = 0;

    function assignX(node) {
        if (!node) return;
        assignX(node.left);
        node.x = xPosition * minNodeSpacing + 50;
        xPosition++;
        assignX(node.right);
    }

    // Asignar posición Y por nivel
    function assignY(node, level = 0) {
        if (!node) return;
        node.y = level * levelHeight + 50;
        assignY(node.left, level + 1);
        assignY(node.right, level + 1);
    }

    assignX(root);
    assignY(root);

    // Centrar horizontalmente
    const bounds = getBounds(root);
    const offsetX = (canvasWidth - bounds.width) / 2 - bounds.minX;
    offsetAllX(root, offsetX);
}
```

#### 2.4 Visualización

```
Nodo:
- Círculo: radio 20px
- DEFAULT: #3b82f6 (azul)
- HIGHLIGHTED (búsqueda): #fbbf24 (amarillo)
- FOUND: #22c55e (verde)
- NOT_FOUND: #ef4444 (rojo)
- INSERTING: #a855f7 (violeta)

Aristas:
- Línea de padre a hijo
- Curva Bezier suave (opcional)
- Color: #64748b (gris)
```

**Animación de búsqueda:**
1. Comenzar en root (highlight)
2. Delay 500ms
3. Mover highlight al siguiente nodo en path
4. Repetir hasta encontrar o null
5. Resultado: verde (found) o rojo (not found)

#### 2.5 Controles

| Control | Tipo | Acción |
|---------|------|--------|
| Input + Insert | text + button | Insertar valor |
| Input + Search | text + button | Buscar valor |
| Input + Delete | text + button | Eliminar valor |
| Recorrido | select | inorder/preorder/postorder |
| Animar recorrido | button | Visualizar secuencia |
| Random tree | button | Generar árbol aleatorio |
| Clear | button | Vaciar árbol |

#### 2.6 Métricas

```
Nodos: 15
Altura: 4
Balance: -2 (izquierda pesada)
Último: Insert 42 → path [50, 25, 30, 42]
Recorrido inorder: [10, 15, 20, 25, 30, ...]
```

---

### 3. LOGIC GATES SANDBOX (`logic-gates-sandbox.html`)

#### 3.1 Modelo de Compuertas

```javascript
const GATES = {
    AND:  (a, b) => a && b,
    OR:   (a, b) => a || b,
    NOT:  (a) => !a,
    NAND: (a, b) => !(a && b),
    NOR:  (a, b) => !(a || b),
    XOR:  (a, b) => a !== b,
    XNOR: (a, b) => a === b
};

class Gate {
    constructor(type, x, y) {
        this.type = type;
        this.x = x;
        this.y = y;
        this.inputs = [];      // Referencias a conexiones
        this.output = null;    // Valor calculado
        this.id = generateId();
    }

    evaluate() {
        const inputValues = this.inputs.map(conn => conn.value);

        if (this.type === 'NOT') {
            this.output = GATES.NOT(inputValues[0] ?? false);
        } else {
            this.output = GATES[this.type](
                inputValues[0] ?? false,
                inputValues[1] ?? false
            );
        }

        return this.output;
    }
}

class Circuit {
    constructor() {
        this.gates = [];
        this.connections = [];
        this.inputs = [];      // Switches de entrada
        this.outputs = [];     // LEDs de salida
    }

    // Evaluación topológica (respeta dependencias)
    evaluate() {
        // 1. Ordenar gates topológicamente
        const sorted = this.topologicalSort();

        // 2. Evaluar en orden
        for (const gate of sorted) {
            gate.evaluate();
        }

        // 3. Propagar a outputs
        this.outputs.forEach(out => {
            out.value = out.connection?.value ?? false;
        });
    }

    topologicalSort() {
        // Kahn's algorithm o DFS
        // ...
    }
}
```

#### 3.2 Símbolos de Compuertas (SVG paths)

```javascript
const GATE_PATHS = {
    AND: `M 0,0 L 30,0 A 20,20 0 0 1 30,40 L 0,40 Z`,
    OR:  `M 0,0 Q 15,20 0,40 Q 30,40 50,20 Q 30,0 0,0`,
    NOT: `M 0,10 L 30,20 L 0,30 Z M 35,20 A 5,5 0 1 1 35,21`,
    // ... etc
};
```

#### 3.3 Sistema de Conexiones

```javascript
class Connection {
    constructor(fromGate, fromPort, toGate, toPort) {
        this.from = { gate: fromGate, port: fromPort };
        this.to = { gate: toGate, port: toPort };
        this.value = false;
        this.path = [];  // Puntos para renderizar cable
    }

    calculatePath() {
        // Bezier curve desde output de fromGate a input de toGate
        const start = this.from.gate.getOutputPosition();
        const end = this.to.gate.getInputPosition(this.to.port);

        const midX = (start.x + end.x) / 2;

        this.path = [
            start,
            { x: midX, y: start.y },  // Control point 1
            { x: midX, y: end.y },    // Control point 2
            end
        ];
    }
}
```

#### 3.4 Visualización

```
Colores de cables:
- FALSE (0): #3b82f6 (azul apagado)
- TRUE (1):  #fbbf24 (amarillo brillante)
- UNDEFINED: #64748b (gris)

Animación de señal:
- Pulso viajando por el cable cuando cambia valor
- Transición suave de color (150ms)

Grid de fondo:
- Puntos cada 20px para alineación
- Snap-to-grid al soltar compuertas
```

#### 3.5 Interacción Drag & Drop

```javascript
// Estados de interacción
let dragState = null;  // 'gate' | 'connection' | null
let selectedGate = null;
let connectionStart = null;

canvas.addEventListener('mousedown', (e) => {
    const pos = getCanvasPos(e);

    // ¿Click en gate existente?
    const gate = findGateAt(pos);
    if (gate) {
        // ¿En puerto de salida? → Iniciar conexión
        if (isOnOutputPort(gate, pos)) {
            dragState = 'connection';
            connectionStart = { gate, port: 'output' };
        } else {
            // Arrastrar gate
            dragState = 'gate';
            selectedGate = gate;
        }
        return;
    }

    // ¿Click en input switch?
    const input = findInputAt(pos);
    if (input) {
        input.value = !input.value;
        circuit.evaluate();
        render();
    }
});

canvas.addEventListener('mousemove', (e) => {
    if (dragState === 'gate') {
        selectedGate.x = e.clientX - offset.x;
        selectedGate.y = e.clientY - offset.y;
        snapToGrid(selectedGate);
        render();
    }

    if (dragState === 'connection') {
        // Dibujar cable temporal
        renderTempConnection(connectionStart, getCanvasPos(e));
    }
});
```

#### 3.6 Tabla de Verdad Dinámica

```javascript
function generateTruthTable(circuit) {
    const numInputs = circuit.inputs.length;
    const rows = Math.pow(2, numInputs);
    const table = [];

    for (let i = 0; i < rows; i++) {
        // Generar combinación de inputs
        const inputValues = [];
        for (let j = 0; j < numInputs; j++) {
            inputValues.push((i >> (numInputs - 1 - j)) & 1);
        }

        // Aplicar y evaluar
        circuit.inputs.forEach((inp, idx) => {
            inp.value = inputValues[idx] === 1;
        });
        circuit.evaluate();

        // Guardar resultado
        table.push({
            inputs: [...inputValues],
            outputs: circuit.outputs.map(o => o.value ? 1 : 0)
        });
    }

    return table;
}
```

#### 3.7 Presets de Circuitos

```javascript
const PRESETS = {
    halfAdder: {
        gates: [
            { type: 'XOR', x: 200, y: 100 },  // Sum
            { type: 'AND', x: 200, y: 200 }   // Carry
        ],
        inputs: [
            { label: 'A', x: 50, y: 120 },
            { label: 'B', x: 50, y: 180 }
        ],
        outputs: [
            { label: 'S', x: 350, y: 100 },
            { label: 'C', x: 350, y: 200 }
        ],
        connections: [
            { from: 'A', to: 'XOR.0' },
            { from: 'B', to: 'XOR.1' },
            { from: 'A', to: 'AND.0' },
            { from: 'B', to: 'AND.1' },
            { from: 'XOR', to: 'S' },
            { from: 'AND', to: 'C' }
        ]
    },
    srLatch: { /* ... */ },
    fullAdder: { /* ... */ }
};
```

---

### 4. STACK HEAP VISUALIZER (`stack-heap-visualizer.html`)

#### 4.1 Modelo de Memoria

```javascript
class MemoryModel {
    constructor(stackSize = 1024, heapSize = 4096) {
        this.stack = {
            base: heapSize,           // Stack empieza después del heap
            pointer: heapSize,        // SP actual
            frames: []                // Stack frames
        };

        this.heap = {
            base: 0,
            size: heapSize,
            blocks: [],               // Bloques asignados
            freeList: [{ start: 0, size: heapSize }]
        };

        this.nextAddress = 0;
    }

    // STACK OPERATIONS
    pushFrame(functionName, variables = []) {
        const frame = {
            name: functionName,
            returnAddress: this.generateAddress(),
            basePointer: this.stack.pointer,
            variables: []
        };

        // Reservar espacio para variables locales
        variables.forEach(v => {
            this.stack.pointer += v.size;
            frame.variables.push({
                name: v.name,
                type: v.type,
                value: v.value,
                address: this.stack.pointer - v.size
            });
        });

        this.stack.frames.push(frame);
        return frame;
    }

    popFrame() {
        const frame = this.stack.frames.pop();
        if (frame) {
            this.stack.pointer = frame.basePointer;
        }
        return frame;
    }

    // HEAP OPERATIONS (First-Fit allocation)
    malloc(size, label = '') {
        // Buscar primer bloque libre suficiente
        for (let i = 0; i < this.heap.freeList.length; i++) {
            const free = this.heap.freeList[i];

            if (free.size >= size) {
                const block = {
                    address: free.start,
                    size: size,
                    label: label,
                    data: null
                };

                // Actualizar free list
                if (free.size === size) {
                    this.heap.freeList.splice(i, 1);
                } else {
                    free.start += size;
                    free.size -= size;
                }

                this.heap.blocks.push(block);
                return block;
            }
        }

        return null;  // Out of memory
    }

    free(address) {
        const idx = this.heap.blocks.findIndex(b => b.address === address);
        if (idx === -1) return false;

        const block = this.heap.blocks.splice(idx, 1)[0];

        // Agregar a free list y merge con adyacentes
        this.heap.freeList.push({ start: block.address, size: block.size });
        this.coalesceFreeList();

        return true;
    }

    coalesceFreeList() {
        // Ordenar por dirección
        this.heap.freeList.sort((a, b) => a.start - b.start);

        // Merge bloques adyacentes
        for (let i = 0; i < this.heap.freeList.length - 1; i++) {
            const curr = this.heap.freeList[i];
            const next = this.heap.freeList[i + 1];

            if (curr.start + curr.size === next.start) {
                curr.size += next.size;
                this.heap.freeList.splice(i + 1, 1);
                i--;  // Re-check current
            }
        }
    }
}
```

#### 4.2 Pseudocódigo Ejecutable

```javascript
const EXAMPLE_CODE = `
function main() {
    int x = 10;           // Stack: variable local
    int y = 20;           // Stack: variable local
    int* ptr = malloc(4); // Heap: asignación dinámica
    *ptr = 42;            // Escribir en heap
    int z = add(x, y);    // Llamada a función
    free(ptr);            // Liberar heap
    return z;
}

function add(int a, int b) {
    int result = a + b;   // Stack frame de add()
    return result;
}
`;

// Parser simplificado
function parseCode(code) {
    const instructions = [];
    // ... tokenize y generar instrucciones
    return instructions;
}

// Ejecutor paso a paso
class Executor {
    constructor(memory, instructions) {
        this.memory = memory;
        this.instructions = instructions;
        this.pc = 0;  // Program counter
    }

    step() {
        if (this.pc >= this.instructions.length) return false;

        const instr = this.instructions[this.pc];

        switch (instr.type) {
            case 'PUSH_FRAME':
                this.memory.pushFrame(instr.name, instr.vars);
                break;
            case 'POP_FRAME':
                this.memory.popFrame();
                break;
            case 'MALLOC':
                this.memory.malloc(instr.size, instr.label);
                break;
            case 'FREE':
                this.memory.free(instr.address);
                break;
            case 'ASSIGN':
                // ... actualizar variable
                break;
        }

        this.pc++;
        return true;
    }
}
```

#### 4.3 Visualización

```
Layout vertical (edificio):

┌─────────────────────────────┐  ← Tope de memoria
│                             │
│          STACK              │  Crece hacia abajo ↓
│   ┌───────────────────┐     │
│   │ main()            │     │
│   │   x = 10          │     │
│   │   y = 20          │     │
│   │   ptr = 0x1000    │ ────┼──┐  (flecha a heap)
│   └───────────────────┘     │  │
│   ┌───────────────────┐     │  │
│   │ add()             │     │  │
│   │   a = 10          │     │  │
│   │   b = 20          │     │  │
│   │   result = 30     │     │  │
│   └───────────────────┘     │  │
│                             │  │
│═══════════════════════════════│  ← Frontera Stack/Heap
│                             │  │
│          HEAP               │  │
│   ┌───────────────┐         │  │
│   │ 0x1000: 42    │ ←───────┼──┘
│   │ (4 bytes)     │         │
│   └───────────────┘         │
│   ┌───────────────┐         │
│   │ FREE BLOCK    │         │
│   │ (4088 bytes)  │         │
│   └───────────────┘         │
│                             │
└─────────────────────────────┘  ← Base de memoria (0x0000)
```

#### 4.4 Colores

```
Stack frames: gradiente por profundidad
- Frame 0 (main):  #3b82f6 (azul)
- Frame 1:         #8b5cf6 (violeta)
- Frame 2:         #a855f7 (púrpura)
- ...

Heap blocks:
- Allocated:       #22c55e (verde)
- Free:            #1e293b (gris oscuro)
- Dangling ptr:    #ef4444 (rojo) - puntero a memoria liberada

Punteros (flechas):
- Valid:           #fbbf24 (amarillo)
- Dangling:        #ef4444 (rojo, línea punteada)
```

---

### 5. PERCEPTRON PLAYGROUND (`perceptron-playground.html`)

#### 5.1 Modelo Matemático

```javascript
class Perceptron {
    constructor(inputSize = 2) {
        // Inicializar pesos aleatorios pequeños
        this.weights = Array(inputSize).fill(0).map(() =>
            (Math.random() - 0.5) * 0.1
        );
        this.bias = (Math.random() - 0.5) * 0.1;
        this.learningRate = 0.1;
    }

    // Forward pass: y = sign(w · x + b)
    predict(inputs) {
        let sum = this.bias;
        for (let i = 0; i < inputs.length; i++) {
            sum += this.weights[i] * inputs[i];
        }
        return sum >= 0 ? 1 : 0;  // Step function
    }

    // Valor antes de activación (para visualizar decision boundary)
    weightedSum(inputs) {
        let sum = this.bias;
        for (let i = 0; i < inputs.length; i++) {
            sum += this.weights[i] * inputs[i];
        }
        return sum;
    }

    // Entrenamiento: Perceptron Learning Rule
    // w_new = w_old + α * (target - prediction) * input
    train(inputs, target) {
        const prediction = this.predict(inputs);
        const error = target - prediction;

        if (error !== 0) {
            // Actualizar pesos
            for (let i = 0; i < this.weights.length; i++) {
                this.weights[i] += this.learningRate * error * inputs[i];
            }
            this.bias += this.learningRate * error;

            return { updated: true, error };
        }

        return { updated: false, error: 0 };
    }

    // Decision boundary: w1*x1 + w2*x2 + b = 0
    // Despejando x2: x2 = -(w1*x1 + b) / w2
    getDecisionBoundary(x1) {
        if (Math.abs(this.weights[1]) < 0.0001) return null;
        return -(this.weights[0] * x1 + this.bias) / this.weights[1];
    }
}
```

#### 5.2 Dataset

```javascript
class Dataset {
    constructor() {
        this.points = [];  // { x: [x1, x2], y: 0|1 }
    }

    addPoint(x1, x2, label) {
        this.points.push({
            x: [x1, x2],
            y: label
        });
    }

    // Generar dataset linealmente separable
    generateLinear(n = 50) {
        this.points = [];

        // Línea separadora aleatoria
        const angle = Math.random() * Math.PI;
        const offset = (Math.random() - 0.5) * 0.5;

        for (let i = 0; i < n; i++) {
            const x1 = (Math.random() - 0.5) * 2;
            const x2 = (Math.random() - 0.5) * 2;

            // Clasificar según lado de la línea
            const side = x2 - (Math.tan(angle) * x1 + offset);
            const label = side >= 0 ? 1 : 0;

            this.addPoint(x1, x2, label);
        }
    }

    // Dataset XOR (no linealmente separable)
    generateXOR() {
        this.points = [
            { x: [-0.5, -0.5], y: 0 },
            { x: [0.5, 0.5], y: 0 },
            { x: [-0.5, 0.5], y: 1 },
            { x: [0.5, -0.5], y: 1 }
        ];
    }
}
```

#### 5.3 Visualización

```
Canvas:
- Fondo: gradiente según w·x + b (azul negativo, rojo positivo)
- Decision boundary: línea blanca donde w·x + b = 0
- Puntos clase 0: círculos azules
- Puntos clase 1: círculos rojos
- Puntos mal clasificados: borde amarillo grueso

Panel lateral:
- Pesos actuales: w1 = 0.32, w2 = -0.18, b = 0.05
- Ecuación: 0.32·x₁ - 0.18·x₂ + 0.05 = 0
- Accuracy: 92% (46/50)
- Epoch: 15
```

#### 5.4 Entrenamiento Visual

```javascript
async function trainEpoch(perceptron, dataset, delay = 100) {
    let totalError = 0;

    for (const point of dataset.points) {
        const result = perceptron.train(point.x, point.y);
        totalError += Math.abs(result.error);

        // Actualizar visualización
        highlightPoint(point);
        drawDecisionBoundary();
        updateWeightsDisplay();

        await sleep(delay);
    }

    return totalError;
}

async function trainUntilConvergence(maxEpochs = 100) {
    for (let epoch = 0; epoch < maxEpochs; epoch++) {
        const error = await trainEpoch(perceptron, dataset);

        updateEpochDisplay(epoch + 1);

        if (error === 0) {
            showMessage('¡Convergió!');
            break;
        }
    }
}
```

---

### 6. FINITE AUTOMATA (`finite-automata.html`)

#### 6.1 Modelo DFA/NFA

```javascript
class State {
    constructor(id, isAccept = false) {
        this.id = id;
        this.isAccept = isAccept;
        this.transitions = new Map();  // symbol → State (DFA) o Set<State> (NFA)
        this.x = 0;
        this.y = 0;
    }
}

class DFA {
    constructor() {
        this.states = new Map();      // id → State
        this.startState = null;
        this.alphabet = new Set();
        this.currentState = null;
    }

    addState(id, isAccept = false) {
        const state = new State(id, isAccept);
        this.states.set(id, state);
        return state;
    }

    setStartState(id) {
        this.startState = this.states.get(id);
        this.currentState = this.startState;
    }

    addTransition(fromId, symbol, toId) {
        const from = this.states.get(fromId);
        const to = this.states.get(toId);

        if (from && to) {
            from.transitions.set(symbol, to);
            this.alphabet.add(symbol);
        }
    }

    reset() {
        this.currentState = this.startState;
    }

    step(symbol) {
        if (!this.currentState) return { valid: false, reason: 'no current state' };

        const nextState = this.currentState.transitions.get(symbol);

        if (!nextState) {
            return {
                valid: false,
                reason: `no transition for '${symbol}' from state ${this.currentState.id}`
            };
        }

        const prevState = this.currentState;
        this.currentState = nextState;

        return {
            valid: true,
            from: prevState,
            to: nextState,
            symbol
        };
    }

    accepts(input) {
        this.reset();

        for (const symbol of input) {
            const result = this.step(symbol);
            if (!result.valid) return false;
        }

        return this.currentState?.isAccept ?? false;
    }

    // Ejecutar con historial completo
    run(input) {
        this.reset();
        const history = [{ state: this.currentState, symbol: null }];

        for (const symbol of input) {
            const result = this.step(symbol);
            history.push({
                state: this.currentState,
                symbol,
                valid: result.valid
            });

            if (!result.valid) break;
        }

        return {
            accepted: this.currentState?.isAccept ?? false,
            history
        };
    }
}
```

#### 6.2 Ejemplos de Autómatas

```javascript
const PRESET_DFAS = {
    // Acepta strings que terminan en "01"
    endsWith01: {
        states: ['q0', 'q1', 'q2'],
        accept: ['q2'],
        start: 'q0',
        transitions: [
            ['q0', '0', 'q1'],
            ['q0', '1', 'q0'],
            ['q1', '0', 'q1'],
            ['q1', '1', 'q2'],
            ['q2', '0', 'q1'],
            ['q2', '1', 'q0']
        ],
        description: 'Acepta cadenas binarias que terminan en "01"'
    },

    // Acepta strings con número par de ceros
    evenZeros: {
        states: ['even', 'odd'],
        accept: ['even'],
        start: 'even',
        transitions: [
            ['even', '0', 'odd'],
            ['even', '1', 'even'],
            ['odd', '0', 'even'],
            ['odd', '1', 'odd']
        ],
        description: 'Acepta cadenas con número par de ceros'
    },

    // Acepta "ab*a"
    abStarA: {
        states: ['q0', 'q1', 'q2', 'dead'],
        accept: ['q2'],
        start: 'q0',
        transitions: [
            ['q0', 'a', 'q1'],
            ['q0', 'b', 'dead'],
            ['q1', 'a', 'q2'],
            ['q1', 'b', 'q1'],
            ['q2', 'a', 'dead'],
            ['q2', 'b', 'dead'],
            ['dead', 'a', 'dead'],
            ['dead', 'b', 'dead']
        ],
        description: 'Acepta cadenas de la forma ab*a'
    }
};
```

#### 6.3 Visualización

```
Estados:
- Normal:      círculo simple, borde #64748b
- Aceptación:  doble círculo
- Actual:      relleno #3b82f6 (azul)
- Visitado:    relleno suave #3b82f6/30

Transiciones:
- Flecha curva de estado a estado
- Etiqueta con símbolo en el medio
- Self-loop: arco sobre el estado
- Activa: #fbbf24 (amarillo), grosor 3px

Input tape:
- Cinta horizontal con símbolos
- Cabezal indicando posición actual
- Símbolos leídos: verde
- Símbolo actual: amarillo
- Por leer: gris
```

#### 6.4 Animación de Ejecución

```javascript
async function animateRun(dfa, input, delay = 500) {
    dfa.reset();

    // Renderizar estado inicial
    highlightState(dfa.currentState);
    await sleep(delay);

    for (let i = 0; i < input.length; i++) {
        const symbol = input[i];

        // Highlight símbolo en tape
        highlightTapePosition(i);
        await sleep(delay / 2);

        // Ejecutar transición
        const result = dfa.step(symbol);

        if (result.valid) {
            // Animar flecha de transición
            animateTransition(result.from, result.to, symbol);
            await sleep(delay);

            // Highlight nuevo estado
            highlightState(dfa.currentState);
        } else {
            // Error: no hay transición
            showError(`Sin transición para '${symbol}'`);
            break;
        }
    }

    // Resultado final
    if (dfa.currentState?.isAccept) {
        showResult('ACEPTADO', 'green');
    } else {
        showResult('RECHAZADO', 'red');
    }
}
```

---

## Checklist de Calidad por Simulación

Antes de considerar completa cada simulación:

- [ ] **Algoritmo correcto**: Implementación canónica verificada
- [ ] **Complejidad real**: Big O mostrado y demostrable
- [ ] **Sin atajos**: No simular comportamiento, ejecutar realmente
- [ ] **Smooth rendering**: 60fps, sin jank
- [ ] **Interactividad real**: Cambios en vivo, no recarga
- [ ] **Colores con propósito**: Cada color tiene significado
- [ ] **Métricas visibles**: Counters, estados, tiempos
- [ ] **Step mode**: Poder ejecutar paso a paso
- [ ] **Presets**: Ejemplos interesantes precargados
- [ ] **Responsive**: Funciona en diferentes tamaños
- [ ] **Código limpio**: Separación algoritmo/render/UI

---

## Referencias Canónicas

| Simulación | Fuente |
|------------|--------|
| Sorting | CLRS Cap. 2, 7, 8 |
| BST | CLRS Cap. 12 |
| Logic Gates | Nand2Tetris Cap. 1 |
| Memory Model | CSAPP Cap. 9 |
| Perceptron | Rosenblatt 1958, Bishop Cap. 4 |
| DFA/NFA | Sipser Cap. 1 |

---

## Orden de Implementación

1. **bubble-sort-race.html** - Establece patrón de visualización de arrays
2. **logic-gates-sandbox.html** - Establece patrón de drag & drop
3. **perceptron-playground.html** - Establece patrón de ML visual
4. **binary-search-tree.html** - Usa patrón de visualización de estructuras
5. **stack-heap-visualizer.html** - Más complejo, combina patrones
6. **finite-automata.html** - Similar a logic gates pero con grafos
