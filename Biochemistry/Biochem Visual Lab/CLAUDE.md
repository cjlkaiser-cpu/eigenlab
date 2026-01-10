# Biochem Visual Lab - Documentación Técnica

## Descripción

El **Biochem Visual Lab** contiene **16 simulaciones** que cubren procesos bioquímicos fundamentales: metabolismo, síntesis proteica, genética y regulación génica. Implementa modelos científicos validados con visualización molecular en Canvas 2D.

## Simulaciones (16 Total)

### CORE (Fundamentos Moleculares - 4)
1. **Michaelis-Menten** - Cinética enzimática: V = Vmax[S]/(Km+[S])
2. **ADN Replicación** - Semiconservativa, helicasa, polimerasa
3. **Membrana Fluida** - Modelo mosaico fluido Singer-Nicolson
4. **Lípidos** - Auto-ensamblaje de micelas

### METABOLISMO (Rutas Energéticas - 4)
5. **Glucólisis** - 10 pasos: Glucosa → 2 Piruvato (+2 ATP, +2 NADH)
6. **Ciclo de Krebs** - 8 intermediarios, 3 NADH + 1 FADH₂ + 1 GTP
7. **Cadena Respiratoria** - Complejos I-V, fosforilación oxidativa
8. **Fermentación** - Láctica vs alcohólica, regeneración NAD+

### PROTEÍNAS (Estructura y Función - 4)
9. **Síntesis de Proteínas** - Ribosoma, tRNA, código genético
10. **Plegamiento** - Chaperonas, ΔG minimización
11. **Estructura Proteica** - 4 niveles: 1°, 2°, 3°, 4°
12. **Desnaturalización** - Por pH, temperatura, agentes químicos

### GENÉTICA (Expresión Génica - 4)
13. **Transcripción** - ADN → ARN, RNA Polimerasa II
14. **Traducción** - ARN → Proteína, codones
15. **Mutaciones** - Silenciosa, missense, nonsense, frameshift
16. **Regulación Génica** - Operón lac, control transcripcional

## Modelos Científicos

### Metabolismo Oxidativo
```
Glucólisis:
Glucosa (C6) → 2 Piruvato (C3)
Balance: +2 ATP, +2 NADH

Krebs (por ciclo):
Acetil-CoA → 3 NADH + 1 FADH₂ + 1 GTP + 2 CO₂

Cadena Respiratoria:
NADH/FADH₂ → e⁻ → Complejos I-IV → O₂ → H₂O
Gradiente H+ → ATP sintasa → ATP
```

### Cinética Enzimática
```
Michaelis-Menten:
v = Vmax × [S] / (Km + [S])

Parámetros:
- Km: afinidad (↓Km = ↑afinidad)
- Vmax: velocidad máxima (saturación)
```

### Código Genético
```
64 codones → 20 aminoácidos
AUG = Metionina (START)
UAA, UAG, UGA = STOP
Degeneración: múltiples codones por aa
```

### Replicación Semiconservativa
```
ADN original → Helicasa separa → Polimerasa sintetiza
Resultado: 2 ADN (1 hebra original + 1 nueva cada uno)
```

## Paleta de Colores Molecular

- **ATP**: Amarillo `#fbbf24`
- **NADH**: Púrpura `#a855f7`
- **Piruvato**: Rojo `#ef4444`
- **Glucosa**: Azul `#38bdf8`
- **Intermediarios**: Lima `#a3e635`
- **Proteína**: Verde `#22c55e`
- **mRNA**: Cyan `#06b6d4`
- **Enzimas**: Magenta `#d946ef`

## Tecnología

### Canvas 2D
Todas las 16 simulaciones
- Partículas en movimiento browniano
- Efectos glow para énfasis molecular
- Animaciones suaves con interpolación

### Visualización Molecular
- Fosfolípidos: cabeza + colas
- Proteínas: aminoácidos coloreados
- ADN: doble hélice con bases ATCG
- Moléculas: representación esquemática

## Características UI

- **Modo paso a paso**: Glucólisis, transcripción
- **Sliders**: Velocidad, concentración, temperatura
- **Estadísticas en tiempo real**: ATP, NADH, productos
- **Leyendas**: Codificación de colores clara
- **Breadcrumbs**: Navegación a index

## Referencias

**Archivo más grande:** cinetica-colisiones.html (1,634 líneas)
**Total:** 16 simulaciones, ~20,000 líneas de código
**Categorías:** 4 (Core, Metabolismo, Proteínas, Genética)

---

**Última actualización:** 2026-01-10
