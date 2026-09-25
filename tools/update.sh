#!/bin/sh
# Regenera todo lo derivado de los archivos y valida el proyecto. Ejecutar antes de cada commit.
#   catálogo (+ insignias del portal) → metadatos de cada página → enlaces → cifras
set -e
cd "$(dirname "$0")/.."
PY=${PYTHON:-python3}
$PY tools/build_catalog.py
$PY tools/add_meta.py
$PY tools/check_links.py
$PY tools/stats.py | tail -1
