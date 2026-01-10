# Template: Prompt de Continuación para EigenLab

> **Uso**: Copia este template y adáptalo para cada laboratorio/feature en desarrollo.

---

# Prompt de Continuación: [NOMBRE DEL LAB/FEATURE]

## Contexto del Proyecto

Estás trabajando en **EigenLab**, un ecosistema de laboratorios virtuales educativos con simulaciones científicas y matemáticas interactivas en HTML/CSS/JavaScript vanilla.

**Repositorio**: https://github.com/cjlkaiser-cpu/eigenlab
**Directorio local**: `/Users/carlos/Projects/EigenLab/`
**Documentación**: Ver `CLAUDE.md` para convenciones completas

## Estado Actual: [NOMBRE DEL LAB]

[Descripción breve de qué se está desarrollando]

### Implementación Completada

#### 1. [Feature/Simulación 1]
✅ [Descripción]
- **[Aspecto clave]**: [Detalle]
- **[Aspecto clave]**: [Detalle]
- **[Aspecto clave]**: [Detalle]

#### 2. [Feature/Simulación 2]
✅ [Descripción]

#### 3. [Otros componentes]
✅ [Descripción]

### Archivos Clave

```
[Disciplina]/[Lab]/
├── index.html                        # [Descripción]
├── [simulacion-1].html              # [Descripción]
└── [simulacion-2].html              # [Descripción]

Referencia para patrones:
├── [path/al/archivo/referencia.js]
└── [path/al/otro/archivo.html]
```

### Convenciones del Proyecto

- **Idioma UI**: Español
- **Idioma código**: Inglés (variables, funciones, comentarios)
- **Archivos**: kebab-case (`mi-simulacion.html`)
- **Color del lab**: `[HEX]` ([nombre del color])
- **Estructura**: HTML autocontenido, sin bundler
- **[Otra convención específica]**: [Detalle]

## Paleta de Colores por Disciplina

| Disciplina | Variable CSS | Hex |
|------------|--------------|-----|
| Physics Visual | `--color-physics-visual` | `#22c55e` |
| Physics Sound | `--color-physics-sound` | `#a855f7` |
| Chemistry | `--color-chemistry` | `#06b6d4` |
| Biochemistry | `--color-biochemistry` | `#ec4899` |
| Biology | `--color-biology` | `#10b981` |
| Geology | `--color-geology` | `#d97706` |
| Mathematics | `--color-mathematics` | `#f97316` |
| Astronomy | `--color-astronomy` | `#6366f1` |
| Computation | `--color-computation` | `#3b82f6` |

## Próximos Pasos Sugeridos

### Nuevas Funcionalidades/Simulaciones:

1. **[Nombre de feature 1]**
   - [Descripción técnica]
   - [Qué implementar]
   - [Tecnología/método]

2. **[Nombre de feature 2]**
   - [Descripción técnica]
   - [Qué implementar]
   - [Tecnología/método]

3. **[Nombre de feature 3]**
   - [Descripción técnica]
   - [Qué implementar]
   - [Tecnología/método]

### Tareas Técnicas Pendientes:

- [ ] [Tarea pendiente 1]
- [ ] [Tarea pendiente 2]
- [ ] [Tarea pendiente 3]
- [ ] Actualizar `CLAUDE.md` con cambios
- [ ] Agregar tests/validación
- [ ] Optimizar para móvil

## Cómo Continuar

**Para agregar nueva simulación:**
1. Usar plantilla: `cp _templates/simulation-[2d/3d].html [Disciplina]/[Lab]/nueva-sim.html`
2. Cambiar `--accent` a `[COLOR_HEX]`
3. Implementar lógica de la simulación
4. Agregar preview al `index.html` del lab
5. Actualizar `_portal/index.html` (incrementar badge count si es necesario)
6. Commit con mensaje: `feat([lab-slug]): add [nombre-simulación]`

**Archivos de referencia útiles:**
- [Patrón X]: `[path/al/archivo:líneas]`
- [Patrón Y]: `[path/al/archivo:líneas]`
- [Patrón Z]: `[path/al/archivo:líneas]`

## Último Commit

```
[HASH] [mensaje del commit]
```

## Problemas Conocidos / Notas

- [Problema/nota 1]
- [Problema/nota 2]

## Instrucciones para Nueva Sesión

Cuando inicies una nueva sesión de Claude Code:

1. Abre el archivo `[path]/CONTINUE.md`
2. Copia y pega todo su contenido en el chat
3. Claude recordará todo el contexto y podrá continuar donde lo dejaste

**Inicio de sesión sugerido:**
```
Hola, quiero continuar trabajando en [NOMBRE DEL LAB] de EigenLab.
[Pegar contenido del archivo CONTINUE.md]
¿En qué [feature/simulación] trabajamos hoy?
```

---

## Ejemplos de CONTINUE.md Existentes

- `Mathematics/Math Sound Lab/CONTINUE.md` - Math Sound Lab development
- [Agregar más aquí según se creen]
