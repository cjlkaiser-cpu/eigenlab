#!/usr/bin/env python3
"""Compila Tailwind (v3) a CSS estático y sustituye el Play CDN (cdn.tailwindcss.com), no apto para producción.

Agrupa las páginas por laboratorio y por configuración inline (tailwind.config = {...}); cada grupo genera
un CSS que solo contiene las clases que usan sus páginas:
    <lab>/tailwind.css            páginas sin configuración propia
    <lab>/tailwind-<hash>.css     páginas con una configuración propia
El <link> se coloca justo antes de </head>, que es donde el Play CDN inyectaba su <style>, para
conservar la misma precedencia frente a los estilos de cada página.

Uso:
    python3 tools/build_css.py            # migra las páginas que aún usan el CDN y recompila todos los CSS
    python3 tools/build_css.py --rebuild  # solo recompila (tras añadir clases nuevas a páginas ya migradas)
Requiere Node (npx descarga tailwindcss@3.4.17 la primera vez).
"""
import hashlib
import json
import os
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from check_links import git_ignored, SKIP_DIRS  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TAILWIND = 'tailwindcss@3.4.17'
CDN_TAG = re.compile(r'[ \t]*<script src="https://cdn\.tailwindcss\.com"></script>\n?')
LINK_RE = re.compile(r'<link rel="stylesheet" href="([^"]*tailwind(?:-[0-9a-f]{8})?\.css)" data-eigenlab-tailwind>')
# Clases que algunas páginas construyen en JavaScript (el compilador no las ve como texto completo)
SAFELIST = ['text-cyan-400', 'text-gray-400'] + [f'{p}-{c}-{s}' for c in ('violet', 'cyan', 'amber')
                                                 for p, s in (('bg', '500/20'), ('text', '400'))]


def lab_root(rel):
    parts = rel.split('/')
    if len(parts) >= 3 and os.path.isfile(os.path.join(ROOT, parts[0], parts[1], 'index.html')):
        return '/'.join(parts[:2])
    return os.path.dirname(rel) or '.'


def extract_config(src):
    """Devuelve (config_json_normalizado, src_sin_config)."""
    m = re.search(r'tailwind\.config\s*=\s*\{', src)
    if not m:
        return '', src
    i, depth = m.end() - 1, 0
    for j in range(i, len(src)):
        depth += {'{': 1, '}': -1}.get(src[j], 0)
        if depth == 0:
            break
    obj = src[i:j + 1]
    cfg = subprocess.run(['node', '-e', f'process.stdout.write(JSON.stringify({obj}))'],
                         capture_output=True, text=True, check=True).stdout
    end = j + 1
    while end < len(src) and src[end] in ' \t;':
        end += 1
    src = src[:m.start()] + src[end:]
    # si el <script> queda vacío, se elimina entero
    src = re.sub(r'[ \t]*<script>\s*</script>\n?', '', src)
    return json.dumps(json.loads(cfg), sort_keys=True), src


def pages():
    for dp, dn, fn in os.walk(ROOT):
        dn[:] = sorted(d for d in dn if d not in SKIP_DIRS)
        for f in sorted(fn):
            if f.endswith('.html'):
                full = os.path.join(dp, f)
                if not git_ignored(full):
                    yield os.path.relpath(full, ROOT)


def migrate():
    """Sustituye el CDN por <link> en las páginas que aún lo usan. Devuelve {css_rel: config_json}."""
    configs = {}
    for rel in pages():
        path = os.path.join(ROOT, rel)
        src = open(path, encoding='utf-8').read()
        if not CDN_TAG.search(src):
            continue
        cfg, src = extract_config(src)
        root = lab_root(rel)
        name = 'tailwind.css' if not cfg else f'tailwind-{hashlib.sha1(cfg.encode()).hexdigest()[:8]}.css'
        css_rel = os.path.join(root, name)
        configs[css_rel] = cfg
        href = os.path.relpath(os.path.join(ROOT, css_rel), os.path.dirname(path))
        src = CDN_TAG.sub('', src, count=1)
        link = f'    <link rel="stylesheet" href="{href}" data-eigenlab-tailwind>\n'
        i = src.lower().index('</head>')
        src = src[:i] + link + src[i:]
        open(path, 'w', encoding='utf-8').write(src)
    return configs


def build(configs):
    groups = {}
    for rel in pages():
        path = os.path.join(ROOT, rel)
        src = open(path, encoding='utf-8').read()
        m = LINK_RE.search(src)
        if m:
            css = os.path.normpath(os.path.join(os.path.dirname(rel), m.group(1)))
            groups.setdefault(css, []).append(src)
    meta_path = os.path.join(ROOT, 'tools', 'tailwind-configs.json')
    stored = json.load(open(meta_path)) if os.path.exists(meta_path) else {}
    stored.update(configs)
    with tempfile.TemporaryDirectory() as tmp:
        inp = os.path.join(tmp, 'in.css')
        open(inp, 'w').write('@tailwind base;\n@tailwind components;\n@tailwind utilities;\n')
        for css, sources in sorted(groups.items()):
            cfg = json.loads(stored.get(css) or '{}')
            cfg['content'] = [{'raw': s, 'extension': 'html'} for s in sources]
            cfg['safelist'] = SAFELIST
            cfg_path = os.path.join(tmp, 'tailwind.config.js')
            open(cfg_path, 'w').write('module.exports = ' + json.dumps(cfg) + ';\n')
            out = os.path.join(ROOT, css)
            subprocess.run(['npx', '-y', TAILWIND, '-c', cfg_path, '-i', inp, '-o', out, '--minify'],
                           check=True, capture_output=True)
            print(f'{css}  ({len(sources)} páginas, {os.path.getsize(out) // 1024} KB)')
    with open(meta_path, 'w') as f:
        json.dump({k: v for k, v in sorted(stored.items()) if k in groups}, f, indent=1, sort_keys=True)


if __name__ == '__main__':
    migrated = {} if '--rebuild' in sys.argv else migrate()
    build(migrated)
