#!/usr/bin/env python3
"""Cuenta simulaciones y guías de cada laboratorio a partir de los archivos.

Uso:
    python3 tools/stats.py          # tabla en Markdown (para README / ROADMAP)

Criterios:
- Simulación = .html en la raíz del laboratorio, excepto index.html y páginas de redirección.
- Sound Labs con subproyectos (Physics Sound Lab): cada carpeta con index.html es una simulación.
- Una simulación tiene guía si existe guides/<nombre>.html|md, o una guía de su versión base
  (p. ej. gas-ideal-wasm → gas-ideal) o una guía con nombre abreviado declarado en GUIDE_ALIAS.
"""
import glob
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# (disciplina, ruta, ¿tiene guías en su alcance?)
LABS = [
    ('Physics', 'Physics/Physics Visual Lab', True),
    ('Physics', 'Physics/Physics Sound Lab', False),
    ('Chemistry', 'Chemistry/Chemistry Visual Lab', True),
    ('Biochemistry', 'Biochemistry/Biochem Visual Lab', True),
    ('Biology', 'Biology/Biology Visual Lab', True),
    ('Geology', 'Geology/Geology Visual Lab', True),
    ('Mathematics', 'Mathematics/Math Visual Lab', True),
    ('Mathematics', 'Mathematics/Math Generative Art Lab', False),
    ('Mathematics', 'Mathematics/Math Sound Lab', False),
    ('Astronomy', 'Astronomy/Astronomy Visual Lab', True),
    ('Astronomy', 'Astronomy/Astronomy Sound Lab', True),
    ('Computation', 'Computation/Computation Lab', True),
    ('AI', 'AI/AI Visual Lab', True),
    ('Music', 'Music/Music Theory Lab', True),
]
COURSES = ['Mathematics/Chaos Lab', 'Mathematics/Platonic Lab', 'Mathematics/Euler Lab',
           'Chemistry/Chemistry Visual Lab/curso-tabla-periodica']
GUIDE_ALIAS = {  # simulación -> guía con otro nombre
    'bubble-sort-race': 'bubble-sort', 'logic-gates-sandbox': 'logic-gates',
    'perceptron-playground': 'perceptron', 'stack-heap-visualizer': 'stack-heap',
}
VARIANT_SUFFIXES = ('-wasm', '-simplified')


def simulations(lab):
    path = os.path.join(ROOT, lab)
    sims = []
    for f in sorted(glob.glob(os.path.join(path, '*.html'))):
        name = os.path.basename(f)[:-5]
        if name == 'index' or 'http-equiv="refresh"' in open(f, encoding='utf-8', errors='ignore').read():
            continue
        sims.append(name)
    for group in ('metronomos', 'generativos'):   # Sound Labs con subproyectos
        gdir = os.path.join(path, group)
        if os.path.isdir(gdir):
            sims += sorted(d for d in os.listdir(gdir)
                           if os.path.isfile(os.path.join(gdir, d, 'index.html')) and d != 'experimental')
    return sims


def has_guide(lab, sim):
    names = {sim, GUIDE_ALIAS.get(sim, sim)}
    for suf in VARIANT_SUFFIXES:
        if sim.endswith(suf):
            names.add(sim[: -len(suf)])
    return any(os.path.exists(os.path.join(ROOT, lab, 'guides', n + ext)) for n in names for ext in ('.html', '.md'))


def main():
    rows, tot_s, tot_g, scope_s = [], 0, 0, 0
    for disc, lab, guided in LABS:
        sims = simulations(lab)
        g = sum(has_guide(lab, s) for s in sims) if guided else None
        missing = [s for s in sims if guided and not has_guide(lab, s)]
        rows.append((disc, os.path.basename(lab), len(sims), g, missing))
        tot_s += len(sims)
        if guided:
            tot_g += g; scope_s += len(sims)
    print('| Disciplina | Laboratorio | Sims | Guías | Sin guía |')
    print('|---|---|:--:|:--:|---|')
    for disc, lab, n, g, missing in rows:
        gs = f'{g}/{n}' if g is not None else '—'
        print(f'| {disc} | {lab} | {n} | {gs} | {", ".join(missing)} |')
    print(f'\nTotal: {len(LABS)} laboratorios · {tot_s} simulaciones · '
          f'{tot_g}/{scope_s} con guía ({100 * tot_g // scope_s}%) · cursos: {len(COURSES)}')


if __name__ == '__main__':
    main()
