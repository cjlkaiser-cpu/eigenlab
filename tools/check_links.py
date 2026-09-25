#!/usr/bin/env python3
"""Comprueba los enlaces internos relativos de todos los HTML (href/src) y Markdown ([..](..)) de EigenLab.

Uso:
    python3 tools/check_links.py            # informe resumido, exit 1 si hay rotos
    python3 tools/check_links.py --all      # lista todos los enlaces rotos
    python3 tools/check_links.py --json     # salida JSON (para scripts)

Ignora enlaces externos (http, mailto, data...), plantillas JS (${...}) y anclas.
"""
import json
import os
import re
import sys
import urllib.parse

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SKIP_DIRS = {'.git', 'node_modules', 'target', 'build', 'pkg', '_promo', 'raw', '.claude',
             '_templates'}   # plantillas: sus rutas son para la ubicación final de la simulación
SKIP_FILES = re.compile(r'PAPER_DRAFT|_draft', re.I)   # borradores locales, no se publican
LINK_RE = re.compile(r'''(?:href|src)\s*=\s*["']([^"']+)["']''')
MD_LINK_RE = re.compile(r'\]\(([^)\s]+)\)')   # [texto](destino) en Markdown
EXTERNAL_RE = re.compile(r'^(?:[a-z][a-z0-9+.-]*:|//|#)', re.I)


def html_files():
    for dp, dn, fn in os.walk(ROOT):
        dn[:] = sorted(d for d in dn if d not in SKIP_DIRS)
        for f in sorted(fn):
            if f.endswith(('.html', '.md')) and not SKIP_FILES.search(f):
                yield os.path.join(dp, f)


def broken_links(path):
    src = open(path, encoding='utf-8', errors='ignore').read()
    base = os.path.dirname(path)
    for m in (MD_LINK_RE if path.endswith('.md') else LINK_RE).finditer(src):
        url = m.group(1).strip()
        if path.endswith('.md') and not re.search(r'[./]', url):   # código tipo f(x,y), no un enlace
            continue
        if not url or EXTERNAL_RE.match(url) or '${' in url or '{{' in url or "' +" in url or '" +' in url:
            continue
        target = urllib.parse.unquote(url.split('#')[0].split('?')[0])
        if not target:
            continue
        full = os.path.normpath(os.path.join(base, target))
        if os.path.isdir(full):
            full = os.path.join(full, 'index.html')
        if not os.path.exists(full):
            line = src.count('\n', 0, m.start()) + 1
            yield line, url


def main():
    results = []
    for f in html_files():
        for line, url in broken_links(f):
            results.append({'file': os.path.relpath(f, ROOT), 'line': line, 'url': url})
    if '--json' in sys.argv:
        print(json.dumps(results, ensure_ascii=False, indent=1))
    else:
        files = sorted({r['file'] for r in results})
        limit = None if '--all' in sys.argv else 40
        for r in results[:limit]:
            print(f"{r['file']}:{r['line']}  →  {r['url']}")
        if limit and len(results) > limit:
            print(f'... ({len(results) - limit} más, usa --all)')
        print(f'\n{len(results)} enlaces rotos en {len(files)} archivos')
    sys.exit(1 if results else 0)


if __name__ == '__main__':
    main()
