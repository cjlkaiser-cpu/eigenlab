# Boids Masivo: Comportamiento Emergente en Bandadas

## Descripcion

Esta simulacion implementa el algoritmo **Boids** de Craig Reynolds (1987), que demuestra como comportamientos complejos de grupo (bandadas de pajaros, cardumenes de peces) emergen de reglas individuales simples. Usando un motor C++/WebAssembly con spatial hashing, puede simular miles de agentes en tiempo real.

## Conceptos Fundamentales

### Comportamiento Emergente

El patron de la bandada no esta programado explicitamente. Emerge de la interaccion de agentes que siguen **tres reglas simples**:

```
F_total = w_s * Separacion + w_a * Alineacion + w_c * Cohesion
```

### Las Tres Reglas de Reynolds

| Regla | Descripcion | Efecto |
|-------|-------------|--------|
| **Separacion** | Evitar colisiones con vecinos cercanos | Mantiene distancia minima |
| **Alineacion** | Igualar velocidad con vecinos | Sincroniza direccion |
| **Cohesion** | Moverse hacia el centro de los vecinos | Mantiene el grupo unido |

### Separacion (S)

Cada boid calcula un vector que lo aleja de vecinos demasiado cercanos:

```
S = Σ (posicion_propia - posicion_vecino) / distancia²
```

La division por distancia² hace que la repulsion sea mas fuerte cuanto mas cerca este el vecino.

### Alineacion (A)

Promedia la velocidad de todos los vecinos visibles:

```
A = (Σ velocidad_vecinos) / num_vecinos
```

Esto crea la sincronizacion caracteristica de las bandadas.

### Cohesion (C)

Calcula el centro de masa de los vecinos y se dirige hacia el:

```
centro = (Σ posicion_vecinos) / num_vecinos
C = centro - posicion_propia
```

## Parametros de la Simulacion

### Pesos de Comportamiento

| Parametro | Rango | Efecto |
|-----------|-------|--------|
| Separacion | 0-5 | Alto = mas disperso |
| Alineacion | 0-5 | Alto = mas coordinado |
| Cohesion | 0-5 | Alto = grupos mas compactos |

### Movimiento

| Parametro | Descripcion |
|-----------|-------------|
| Velocidad maxima | Limite de rapidez |
| Fuerza maxima | Cuanto puede girar por frame |
| Radio de percepcion | Distancia a la que "ve" vecinos |

### Presets Biologicos

| Preset | Caracteristicas | Comportamiento |
|--------|-----------------|----------------|
| **Pajaros** | Velocidad media, alineacion alta | Formaciones fluidas |
| **Peces** | Cohesion alta, separacion fuerte | Cardumenes densos |
| **Insectos** | Alta velocidad, mucho ruido | Movimiento erratico |
| **Medusas** | Muy lentos, baja fuerza | Movimiento suave |
| **Caos** | Ruido extremo, poca cohesion | Sin patron claro |

## Metricas de la Bandada

### Polarizacion (0-1)

Mide que tan alineada esta la bandada:

```
polarizacion = |velocidad_promedio| / velocidad_promedio_individual
```

- **0**: Cada boid va en direccion diferente
- **1**: Todos van en la misma direccion

### Dispersion

Desviacion estandar de posiciones respecto al centro:

```
dispersion = √(Σ(pos - centro)² / N)
```

- **Baja**: Grupo compacto
- **Alta**: Grupo disperso

### Vecinos Promedio

Numero promedio de vecinos que cada boid puede ver. Indica la "conectividad social" de la bandada.

## Optimizacion: Spatial Hashing

El algoritmo ingenuo para encontrar vecinos es O(n²), lo cual limita la simulacion a ~500 boids en JavaScript.

### El Problema

```
Para cada boid:
    Para cada otro boid:
        if distancia < radio_percepcion:
            agregar a vecinos
```

Con 2000 boids: 2000 × 2000 = 4 millones de comparaciones por frame.

### La Solucion: Grid Hash

Dividimos el espacio en celdas del tamano del radio de percepcion:

```
1. Asignar cada boid a su celda
2. Para encontrar vecinos, solo revisar 9 celdas (3x3)
3. Complejidad promedio: O(n)
```

Esto permite simular 10,000+ boids en tiempo real.

## Depredadores

La simulacion incluye depredadores que:

1. **Persiguen** al boid mas cercano
2. **"Cazan"** si estan suficientemente cerca
3. Los boids **huyen** de depredadores cercanos

Esto crea patrones realistas de evasion grupal, donde la bandada se abre y reagrupa.

## Fenomenos Emergentes

### Patron de "Torus"

Con bordes toroidales (wrap), la bandada puede formar un anillo que gira perpetuamente.

### Division y Fusion

Cuando un depredador divide la bandada, los grupos se reunifican gradualmente gracias a la cohesion.

### Vortices

Alta cohesion + alta alineacion pueden crear movimientos circulares espontaneos.

### "Murmuration"

Patrones ondulantes caracteristicos de los estorninos, visibles con muchos boids y parametros balanceados.

## Aplicaciones del Mundo Real

### Animacion por Computadora

- Escenas de multitudes en peliculas
- Cardumenes en documentales de naturaleza
- Grupos de enemigos en videojuegos

### Robotica de Enjambre

- Drones que vuelan en formacion
- Robots exploradores coordinados
- Busqueda y rescate distribuida

### Trafico y Evacuacion

- Simulacion de flujo peatonal
- Evacuacion de emergencia
- Comportamiento en conciertos/estadios

### Biologia

- Estudio de comportamiento animal
- Migraciones
- Dinamicas de depredador-presa

## Experimentos Sugeridos

1. **Alineacion cero**: Quita alineacion y observa grupos sin coordinacion
2. **Cohesion maxima**: Un solo grupo muy compacto
3. **Solo separacion**: Distribucion uniforme, sin grupo
4. **Depredador unico**: Observa como divide y reagrupa la bandada
5. **Dos especies**: Diferentes parametros para ver interaccion
6. **Bordes wrap vs solidos**: Compara dinamicas

## Codigo Clave

### Estructura del Boid (C++)

```cpp
struct Boid {
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    u32 neighborCount;
};
```

### Calculo de Fuerzas

```cpp
Vec2 computeSeparation(u32 index, const vector<u32>& neighbors) {
    Vec2 steer = {0, 0};
    for (u32 other : neighbors) {
        if (distance(boids[index], boids[other]) < separationRadius) {
            Vec2 diff = boids[index].position - boids[other].position;
            steer += diff / distance²;
        }
    }
    return limit(steer, maxForce);
}
```

## Referencias

- Reynolds, C. (1987). "Flocks, Herds, and Schools: A Distributed Behavioral Model"
- Reynolds, C. (1999). "Steering Behaviors For Autonomous Characters"
- Vicsek, T. (1995). "Novel Type of Phase Transition in a System of Self-Driven Particles"

## Conexiones con Otras Simulaciones

- **Ecosistema**: Depredador-presa (Lotka-Volterra)
- **Gas ideal**: Particulas con colisiones
- **Redes neuronales**: Comportamiento emergente
- **Formacion de galaxias**: Fuerzas de largo alcance
