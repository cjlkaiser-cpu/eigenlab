# EigenLab WASM Development Loop

## Tu Tarea

Desarrollar simulaciones WebAssembly de alto rendimiento para EigenLab, una por una.

## Instrucciones

1. **Lee `WASM_AUTOMATION.md`** para ver el estado actual y la simulación a trabajar (CURRENT_SIM)

2. **Para la simulación actual**, implementa en orden:
   - C++ Header (`eigenlab-core/include/physics/`)
   - C++ Implementation (`eigenlab-core/src/physics/`)
   - Bindings en `src/bindings.cpp`
   - Actualizar `CMakeLists.txt`
   - Compilar: `cd eigenlab-core && ./build.sh`
   - Copiar WASM: `cp build/eigenlab-core.* ../_wasm/`
   - HTML completo en el Lab correspondiente
   - Agregar card en `_portal/wasm-labs.html`
   - Commit: `git add -A && git commit -m "feat(wasm): add [Name]"`

3. **Actualiza `WASM_AUTOMATION.md`**:
   - Marca checkboxes completados [x]
   - Cambia ESTADO a COMPLETED
   - Incrementa CURRENT_SIM
   - Marca [x] en la lista de Pendientes

4. **Criterios de calidad**:
   - Código C++ limpio, comentado
   - HTML con estilo EigenLab (dark theme, accent colors)
   - Controles interactivos funcionales
   - HUD con métricas (FPS, partículas, etc)
   - 60 FPS objetivo (30 FPS para 3D pesado)

5. **Cuando termines las 15 simulaciones**, escribe:
```
<promise>ALL_WASM_SIMULATIONS_COMPLETE</promise>
```

## Contexto

- **Proyecto:** /Users/carlos/Projects/EigenLab
- **WASM dir:** eigenlab-core/ (C++17 + Emscripten)
- **Output:** _wasm/eigenlab-core.js + .wasm
- **Portal:** _portal/wasm-labs.html

## Archivos Clave

- `WASM_AUTOMATION.md` - Estado y especificaciones
- `CLAUDE.md` - Contexto general del proyecto
- `eigenlab-core/CMakeLists.txt` - Build system
- `eigenlab-core/src/bindings.cpp` - Embind exports
- Ejemplos existentes en `eigenlab-core/include/physics/`

## Notas

- Una simulación por iteración
- Commits atómicos después de cada simulación completa
- Si hay error de compilación, arréglalo antes de continuar
- Prioriza funcionalidad sobre perfección visual inicial
