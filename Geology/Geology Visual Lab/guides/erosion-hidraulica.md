# Erosion Hidraulica: Simulacion por Gotas de Agua

## Descripcion

Esta simulacion implementa **erosion hidraulica basada en particulas**, donde gotas de agua virtuales fluyen sobre un terreno, erosionando y depositando sedimentos. Es la tecnica estandar para generar terrenos realistas en videojuegos y visualizacion cientifica.

## Conceptos Fundamentales

### Ciclo de una Gota de Agua

1. **Nacimiento**: La gota aparece en posicion aleatoria
2. **Flujo**: Desciende por la pendiente mas pronunciada
3. **Erosion**: Recoge sedimento del terreno
4. **Deposito**: Deja sedimento cuando pierde velocidad
5. **Evaporacion**: Pierde agua gradualmente
6. **Muerte**: Desaparece cuando se agota el agua

### Capacidad de Transporte

La cantidad maxima de sedimento que una gota puede llevar:

```
C = max(v * slope * water, C_min)
```

Donde:
- **v**: Velocidad de la gota
- **slope**: Pendiente del terreno
- **water**: Cantidad de agua restante
- **C_min**: Capacidad minima

### Erosion vs Deposito

- Si `sedimento < capacidad`: **Erosiona** el terreno
- Si `sedimento > capacidad`: **Deposita** sedimento

```
if (sediment < capacity) {
    // Erosion
    erode = (capacity - sediment) * erodeSpeed
    sediment += erode
    terrain -= erode
} else {
    // Deposito
    deposit = (sediment - capacity) * depositSpeed
    sediment -= deposit
    terrain += deposit
}
```

## Parametros de la Simulacion

### Inercia (0-0.3)

Controla cuanto la gota mantiene su direccion anterior vs seguir el gradiente.

| Valor | Efecto |
|-------|--------|
| 0 | Sigue exactamente la pendiente |
| 0.1 | Flujo natural |
| 0.3 | Trayectorias mas suaves, rios mas anchos |

### Velocidad de Erosion (0.1-1.0)

Que tan rapido la gota recoge sedimento.

- **Bajo**: Erosion lenta, terreno suave
- **Alto**: Canones profundos, terreno abrupto

### Velocidad de Deposito (0.1-1.0)

Que tan rapido la gota suelta sedimento.

- **Bajo**: Sedimento se lleva lejos
- **Alto**: Deposito rapido cerca del origen

### Capacidad de Sedimento (1-15)

Cantidad maxima de sedimento por gota.

- **Bajo**: Gotas ligeras, erosion superficial
- **Alto**: Gotas pesadas, canones profundos

### Radio de Erosion (1-8)

Area afectada por cada gota.

- **1-2**: Erosion puntual, detalles finos
- **4-6**: Erosion suave, valles anchos
- **7-8**: Erosion muy difusa

## Presets

| Preset | Caracteristicas | Resultado |
|--------|-----------------|-----------|
| **Suave** | Baja erosion, alta deposicion | Colinas onduladas |
| **Rio** | Alta inercia, baja evaporacion | Canales de rio |
| **Canon** | Alta erosion, baja deposicion | Canones profundos |
| **Costa** | Alta inercia, deposicion rapida | Playas, deltas |
| **Volcanico** | Erosion termica alta | Laderas escarpadas |

## Erosion Termica

Ademas de la erosion hidraulica, se simula el **deslizamiento por gravedad**:

```
if (slope > talusAngle) {
    transfer = (slope - talusAngle) * thermalRate
    // Mover material pendiente abajo
}
```

El **angulo de talud** es la pendiente maxima que el material puede mantener antes de deslizarse.

## Algoritmo Detallado

### Paso 1: Gradiente del Terreno

Calcular la pendiente usando diferencias centrales:

```
gradient.x = (height[x-1, y] - height[x+1, y]) / 2
gradient.y = (height[x, y-1] - height[x, y+1]) / 2
```

### Paso 2: Nueva Direccion

Combinar inercia con gradiente:

```
direction = direction * inertia + gradient * (1 - inertia)
normalize(direction)
```

### Paso 3: Movimiento

```
newPos = pos + direction
newHeight = interpolate(heightMap, newPos)
deltaHeight = newHeight - height
```

### Paso 4: Erosion/Deposito

Basado en la diferencia de altura y capacidad.

### Paso 5: Actualizacion de Velocidad

```
velocity = sqrt(max(0, velocity² + deltaHeight * gravity))
```

### Paso 6: Evaporacion

```
water *= (1 - evaporateSpeed)
```

## Interpolacion Bilineal

Para muestrear alturas en posiciones no enteras:

```
h = (1-fx)*(1-fy)*h00 + fx*(1-fy)*h10 +
    (1-fx)*fy*h01 + fx*fy*h11
```

Donde `fx, fy` son las fracciones de la posicion.

## Erosion con Radio

Para erosion suave, afectar multiples celdas con pesos:

```
for each cell in radius:
    weight = max(0, radius - distance)
    cell.height -= erodeAmount * weight / totalWeight
```

## Fenomenos Geomorfologicos

### Canones y Barrancos

Formados por erosion concentrada a lo largo de canales de drenaje.

### Deltas

Depositos en forma de abanico donde los rios pierden velocidad.

### Planicies Aluviales

Zonas planas donde se deposita sedimento fino.

### Carcavas

Pequenos canales de erosion en laderas.

### Terrazas Fluviales

Superficies escalonadas por cambios en el nivel base.

## Aplicaciones

### Videojuegos

- Generacion procedural de terrenos
- Mundos infinitos
- Mapas de misiones

### Simulacion Cientifica

- Modelado de cuencas hidrograficas
- Prediccion de inundaciones
- Gestion de recursos hidricos

### Geologia

- Reconstruccion paleoclimatica
- Evolucion del paisaje
- Formacion de minerales

### Ingenieria Civil

- Diseno de sistemas de drenaje
- Control de sedimentos
- Estabilidad de taludes

## Experimentos Sugeridos

1. **Comparar presets**: Mismo terreno inicial, diferentes configuraciones
2. **Un solo rio**: Iniciar plano con pequena pendiente
3. **Cordillera joven vs vieja**: Comparar erosion temprana vs tardia
4. **Delta formation**: Terreno alto hacia zona baja
5. **Erosion selectiva**: Pintar montanas y observar erosion

## Optimizaciones

### Spatial Hashing

Para erosion con radio grande, evitar iterar toda la grilla.

### GPU Compute

La erosion por gotas es altamente paralelizable.

### LOD (Level of Detail)

Usar diferentes resoluciones segun la distancia.

## Limitaciones del Modelo

1. **No hay rios permanentes**: Solo gotas individuales
2. **Sin transporte fluvial**: Sedimento no viaja en corriente
3. **Fisica simplificada**: Sin turbulencia ni cavitacion
4. **Escala temporal**: Millones de anos en segundos

## Referencias

- Musgrave, F.K. (1993). "Synthetic Terrain from Fractals"
- Olsen, J. (2004). "Realtime Procedural Terrain Generation"
- Mei, X., et al. (2007). "Fast Hydraulic Erosion Simulation"
- Beyer, J. (2015). "GDC: Terrain in Battlefield"

## Conexiones con Otras Simulaciones

- **Fluidos 2D**: Flujo de agua en superficie
- **Difusion de calor**: Ecuaciones de difusion similares
- **Formacion de galaxias**: Acumulacion gravitacional
- **Tectonica de placas**: Origen de las montanas
