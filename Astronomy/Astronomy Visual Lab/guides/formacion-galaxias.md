# Formacion de Galaxias: Simulacion N-body Gravitacional

## Descripcion

Esta simulacion implementa el **problema de N cuerpos** gravitacional usando el algoritmo **Barnes-Hut** para lograr complejidad O(n log n) en lugar de O(n²). Permite simular miles de estrellas formando galaxias espirales, elipticas y colisiones galacticas.

## Conceptos Fundamentales

### Ley de Gravitacion Universal

Cada par de cuerpos se atrae con una fuerza:

```
F = G * m1 * m2 / r²
```

Donde:
- **G**: Constante gravitacional
- **m1, m2**: Masas de los cuerpos
- **r**: Distancia entre centros

### El Problema de N Cuerpos

Calcular la fuerza sobre cada particula requiere sumar las contribuciones de todas las demas:

```
F_i = G * m_i * Σ (m_j * (r_j - r_i)) / |r_j - r_i|³
```

Con N particulas:
- **Fuerza bruta**: O(n²) operaciones por paso
- **Barnes-Hut**: O(n log n) operaciones

## Algoritmo Barnes-Hut

### Idea Principal

Cuerpos lejanos pueden aproximarse como un unico cuerpo con masa total concentrada en su centro de masa.

### Estructura de Datos: Quadtree

El espacio se divide recursivamente en cuadrantes:

```
+-------+-------+
|  NW   |  NE   |
+-------+-------+
|  SW   |  SE   |
+-------+-------+
```

Cada nodo almacena:
- Centro de masa de todas las particulas contenidas
- Masa total
- Punteros a 4 hijos (o particula si es hoja)

### Criterio de Apertura

Para decidir si usar la aproximacion:

```
theta = tamaño_nodo / distancia_al_nodo
```

- Si `theta < umbral`: usar aproximacion (tratar nodo como particula unica)
- Si `theta >= umbral`: descender a los hijos

### Valor de Theta

| Theta | Precision | Velocidad |
|-------|-----------|-----------|
| 0.3 | Muy alta | Lento |
| 0.5 | Alta (recomendado) | Medio |
| 0.7 | Media | Rapido |
| 1.0 | Baja | Muy rapido |

## Suavizado (Softening)

Para evitar singularidades cuando las particulas estan muy cerca:

```
F = G * m1 * m2 / (r² + ε²)
```

El parametro **ε (epsilon)** es la longitud de suavizado:
- Previene divisiones por cero
- Representa el tamaño finito de las estrellas
- Valor tipico: 1-10% del radio de la galaxia

## Integracion Temporal: Leapfrog

El metodo leapfrog es simplectico (conserva energia a largo plazo):

```
v(t + dt/2) = v(t) + a(t) * dt/2
x(t + dt) = x(t) + v(t + dt/2) * dt
a(t + dt) = F(x(t + dt)) / m
v(t + dt) = v(t + dt/2) + a(t + dt) * dt/2
```

### Ventajas del Leapfrog

1. **Conservacion de energia**: Error de energia acotado
2. **Reversible en el tiempo**: Fisicamente correcto
3. **Segundo orden**: Mas preciso que Euler

## Tipos de Galaxias

### Galaxia Espiral (Disco)

- Disco rotante de estrellas
- Agujero negro supermasivo central
- Densidad exponencial: ρ(r) = ρ₀ * e^(-r/h)
- Velocidad orbital: v = √(GM/r)

### Galaxia Eliptica

- Distribucion esferoidal (modelo de Plummer)
- Sin rotacion neta
- Dispersion de velocidades isotropica
- Estrellas mas viejas, rojizas

### Colision Galactica

- Dos galaxias aproximandose
- Parametro de impacto controla tipo de colision
- Puede formar colas de marea, anillos, fusiones

## Parametros de la Simulacion

### Constante G

- **Bajo (< 0.5)**: Interacciones debiles, galaxias dispersas
- **Normal (1.0)**: Comportamiento estandar
- **Alto (> 1.5)**: Colapso rapido, interacciones intensas

### Masa del Agujero Negro

- **0**: Sin agujero negro central
- **1000-2000**: Via Lactea tipica
- **> 3000**: Agujero negro supermasivo (tipo Andromeda)

### Velocidad Temporal

Controla cuantos Myr (millones de anos) pasan por segundo de simulacion.

## Metricas Fisicas

### Energia Total

```
E = E_cinetica + E_potencial
E_cinetica = Σ (1/2) * m * v²
E_potencial = -Σ G * m_i * m_j / r_ij
```

En un sistema ligado: E_total < 0

### Momento Angular

```
L = Σ m * (r × v)
```

Se conserva si no hay torques externos.

### Teorema del Virial

Para un sistema en equilibrio:

```
2 * E_cinetica + E_potencial = 0
```

O equivalentemente:
```
E_potencial = -2 * E_cinetica
```

## Fenomenos Observables

### Brazos Espirales

Ondas de densidad que rotan mas lento que las estrellas. Las estrellas entran y salen de los brazos.

### Colas de Marea

Durante colisiones, la fuerza diferencial arranca estrellas formando largas colas.

### Fusion Galactica

Dos galaxias pueden fusionarse en una sola, usualmente formando una galaxia eliptica.

### Barra Central

Inestabilidad que forma una estructura elongada en el centro de galaxias espirales.

## Aplicaciones

### Cosmologia

- Formacion de estructuras en el universo
- Evolucion de cumulos de galaxias
- Materia oscura y su distribucion

### Astrofisica

- Dinamica de cumulos globulares
- Evolucion de galaxias
- Prediccion de colisiones (Via Lactea - Andromeda)

### Metodos Numericos

- Validacion de algoritmos N-body
- Desarrollo de metodos mas eficientes
- Paralelizacion en GPU

## Experimentos Sugeridos

1. **Disco vs Eliptica**: Compara la evolucion de ambos tipos
2. **Variacion de theta**: Observa el tradeoff precision/velocidad
3. **Colision frontal vs tangencial**: Diferentes parametros de impacto
4. **Sin agujero negro**: Observa la dispersion sin masa central
5. **Multiples agujeros negros**: Agrega varios con el mouse

## Codigo Clave

### Insercion en Quadtree

```cpp
void insertParticle(QuadTreeNode* node, u32 particleIndex) {
    if (node->isLeaf()) {
        if (node->particleIndex < 0) {
            // Hoja vacia
            node->particleIndex = particleIndex;
        } else {
            // Subdivide
            for (int q = 0; q < 4; q++) {
                node->children[q] = new QuadTreeNode(...);
            }
            // Reinsert existing and new
        }
    } else {
        // Descender al cuadrante correcto
        int q = node->getQuadrant(particle.position);
        insertParticle(node->children[q], particleIndex);
    }
    // Update mass and center of mass
}
```

### Calculo de Fuerza

```cpp
Vec2 computeForceFromNode(QuadTreeNode* node, Vec2 pos) {
    Real dist = distance(node->centerOfMass, pos);

    if (node->isLeaf() || (node->size / dist < theta)) {
        // Usar aproximacion
        return G * node->totalMass / (dist² + ε²);
    } else {
        // Recursar
        Vec2 force = {0, 0};
        for (auto child : node->children) {
            force += computeForceFromNode(child, pos);
        }
        return force;
    }
}
```

## Referencias

- Barnes, J. & Hut, P. (1986). "A hierarchical O(N log N) force-calculation algorithm"
- Binney, J. & Tremaine, S. (2008). "Galactic Dynamics"
- Springel, V. (2005). "The cosmological simulation code GADGET-2"

## Conexiones con Otras Simulaciones

- **Gas ideal**: Particulas con colisiones vs gravedad
- **Boids**: Comportamiento emergente de muchos agentes
- **Expansion del universo**: Escala cosmologica
- **Agujero negro**: Orbitas relativistas
