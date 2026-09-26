#!/usr/bin/env python3
"""Añade (o actualiza) metadatos para buscadores y redes sociales en todas las páginas publicables.

Inserta tras <title> un bloque delimitado por
    <!-- eigenlab:meta --> ... <!-- /eigenlab:meta -->
con description, Open Graph (título, descripción, imagen) y tarjeta de Twitter.
Idempotente: si el bloque existe se reemplaza. Los datos salen de _portal/catalog.json
(ejecuta antes tools/build_catalog.py). Las imágenes están en _portal/og/ (generadas por _promo/og_images.py).

Uso:
    python3 tools/add_meta.py          # escribe
    python3 tools/add_meta.py --check  # exit 1 si alguna página no tiene el bloque al día
"""
import html
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BLOCK_RE = re.compile(r'\n?[ \t]*<!-- eigenlab:meta -->.*?<!-- /eigenlab:meta -->', re.S)
OG_DIR = '_portal/og'
PORTAL_PAGES = {  # páginas del portal que no están en el catálogo
    '_portal/index.html': ('EigenLab · Donde las ecuaciones cobran vida',
                           '~200 simulaciones interactivas de física, química, biología, matemáticas, astronomía, IA y música. Las ecuaciones no describen objetos: describen comportamientos.'),
    '_portal/paths/index.html': ('Caminos de aprendizaje · EigenLab',
                                 '12 rutas interdisciplinarias que conectan simulaciones de distintas ciencias bajo una misma idea: caos, ondas, emergencia, simetría…'),
    '_portal/knowledge-graph.html': ('Mapa de conexiones · EigenLab',
                                     'Grafo interactivo de las simulaciones de EigenLab y los conceptos que las unen entre disciplinas.'),
    '_portal/academia.html': ('Academia · EigenLab', 'Cursos, teoría y artículos: Chaos Lab, Platonic Lab, Euler Lab y más.'),
    '_portal/wasm-labs.html': ('WASM Labs · EigenLab',
                               'Simulaciones de alto rendimiento con un motor C++ compilado a WebAssembly: fluidos, galaxias, erosión, tela, plasma.'),
    '_portal/showcase.html': ('Showcase · EigenLab', 'Una selección de las simulaciones más espectaculares de EigenLab.'),
}


def tracked(path):
    """True si algún repositorio (el principal o un submódulo) versiona el archivo y ninguno que lo
    contenga lo ignora (los subproyectos con repo propio ignorados por su lab no se publican aquí)."""
    repos, d = [], os.path.dirname(path)
    while True:
        if os.path.exists(os.path.join(d, '.git')):
            repos.append(d)
        if os.path.samefile(d, ROOT):
            break
        d = os.path.dirname(d)
    run = lambda d, *a: subprocess.run(['git', '-C', d, *a, os.path.relpath(path, d)], capture_output=True).returncode  # noqa: E731
    if any(run(d, 'check-ignore', '-q') == 0 for d in repos):
        return False
    return any(run(d, 'ls-files', '--error-unmatch') == 0 for d in repos)


def og_image(site, lab):
    key = re.sub(r'[^a-z0-9]+', '-', (lab or '').lower()).strip('-')
    if not os.path.exists(os.path.join(ROOT, OG_DIR, key + '.jpg')):
        key = 'eigenlab'
    return f'{site}{OG_DIR}/{key}.jpg'


def block(title, desc, image, indent):
    a = lambda s: html.escape(s, quote=True)  # noqa: E731
    lines = [
        '<!-- eigenlab:meta -->',
        f'<meta name="description" content="{a(desc)}">',
        '<meta property="og:site_name" content="EigenLab">',
        '<meta property="og:type" content="website">',
        '<meta property="og:locale" content="es_ES">',
        f'<meta property="og:title" content="{a(title)}">',
        f'<meta property="og:description" content="{a(desc)}">',
        f'<meta property="og:image" content="{a(image)}">',
        '<meta property="og:image:width" content="1200">',
        '<meta property="og:image:height" content="630">',
        '<meta name="twitter:card" content="summary_large_image">',
        '<meta name="theme-color" content="#030712">',
        '<!-- /eigenlab:meta -->',
    ]
    return '\n' + '\n'.join(indent + l for l in lines)


def targets():
    cat = json.load(open(os.path.join(ROOT, '_portal/catalog.json'), encoding='utf-8'))
    site = cat['site']
    pages = {}
    for e in cat['entries']:
        if e['url'].startswith('http'):
            continue
        if e['type'] == 'sim':
            pages[e['url']] = (f"{e['title']} · {e['lab']}", e['desc'], og_image(site, e['lab']))
            g = e.get('guide')
            if g and g.endswith('.html'):
                pages[g] = (f"Guía: {e['title']} · {e['lab']}",
                            f"Guía teórica de «{e['title']}»: ecuación principal, experimentos guiados y conexiones. {e['desc']}",
                            og_image(site, e['lab']))
        elif e['type'] == 'lab':
            pages[e['url']] = (f"{e['title']} · EigenLab", e['desc'], og_image(site, e['lab']))
        elif e['type'] in ('course', 'lesson'):
            t = f"{e['title']} · Curso" if e['type'] == 'lesson' else f"{e['title']} · Curso · EigenLab"
            pages[e['url']] = (t, e['desc'], og_image(site, e['url'].split('/')[1]))
        elif e['type'] == 'path':
            pages[e['url']] = (f"{e['title']} · Caminos de aprendizaje · EigenLab", e['desc'], og_image(site, None))
    for url, (t, d) in PORTAL_PAGES.items():
        pages[url] = (t, d, og_image(site, None))
    return pages


def apply(src, meta):
    title, desc, image = meta
    src = BLOCK_RE.sub('', src)
    m = re.search(r'</title>', src, re.I) or re.search(r'<head[^>]*>', src, re.I)
    if not m:
        return None
    line_start = src.rfind('\n', 0, m.start()) + 1
    indent = re.match(r'[ \t]*', src[line_start:]).group(0)
    return src[:m.end()] + block(title, desc[:300], image, indent) + src[m.end():]


def main():
    check = '--check' in sys.argv
    changed, skipped, stale = [], [], []
    for url, meta in sorted(targets().items()):
        path = os.path.join(ROOT, url)
        if not os.path.exists(path) or not tracked(path):
            skipped.append(url)
            continue
        src = open(path, encoding='utf-8').read()
        if re.search(r'property="og:title"', BLOCK_RE.sub('', src)):
            skipped.append(url + ' (ya tiene og propio)')
            continue
        new = apply(src, meta)
        if new is None:
            skipped.append(url + ' (sin <head>)')
        elif new != src:
            (stale if check else changed).append(url)
            if not check:
                open(path, 'w', encoding='utf-8').write(new)
    if check:
        print(f'{len(stale)} páginas con metadatos desactualizados')
        for u in stale[:20]:
            print('   ', u)
        sys.exit(1 if stale else 0)
    print(f'{len(changed)} páginas actualizadas · {len(skipped)} omitidas')
    for u in skipped:
        print('   omitida:', u)


if __name__ == '__main__':
    main()
