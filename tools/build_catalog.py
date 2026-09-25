#!/usr/bin/env python3
"""Genera el catálogo único de EigenLab a partir de los archivos.

Salida:
    _portal/catalog.json   datos (para herramientas)
    _portal/catalog.js     los mismos datos como window.EIGENLAB_CATALOG (funciona también con file://)

Uso:
    python3 tools/build_catalog.py          # regenera y muestra un resumen
    python3 tools/build_catalog.py --check  # exit 1 si el catálogo del repo está desactualizado

Fuentes, por orden de preferencia:
    1. La tarjeta de la simulación en el index.html de su laboratorio (título, descripción, categoría, ecuación)
    2. La propia simulación (<title>, meta description, primer párrafo)
Requiere beautifulsoup4.
"""
import json
import os
import re
import sys

from bs4 import BeautifulSoup

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from stats import LABS, ROOT, simulations, has_guide, GUIDE_ALIAS, VARIANT_SUFFIXES  # noqa: E402

SITE = 'https://cjlkaiser-cpu.github.io/eigenlab/'
DISCIPLINES = {  # clave: (nombre visible, color)
    'Physics': ('Física', '#22c55e'), 'Chemistry': ('Química', '#06b6d4'),
    'Biochemistry': ('Bioquímica', '#ec4899'), 'Biology': ('Biología', '#10b981'),
    'Geology': ('Geología', '#d97706'), 'Mathematics': ('Matemáticas', '#f97316'),
    'Astronomy': ('Astronomía', '#6366f1'), 'Computation': ('Computación', '#3b82f6'),
    'AI': ('IA', '#ef4444'), 'Music': ('Música', '#ec4899'),
}
SOUND_COLOR = '#a855f7'
# Tarjetas del portal que no son laboratorios de /Disciplina/: (tipo, disciplina)
EXTRAS = {
    'Chaos Lab': ('course', 'Mathematics'), 'Platonic Lab': ('course', 'Mathematics'),
    'Euler Lab': ('course', 'Mathematics'), 'Sculpt Lab': ('project', 'Mathematics'),
    'Puzzle Lab': ('project', 'Mathematics'), 'Generative Music Lab': ('project', 'Music'),
    'Contrapunctus': ('project', 'Music'),
}
# Subproyectos con repo propio que su lab ignora: no se publican bajo EigenLab, solo en su propia web
PUBLISHED_ELSEWHERE = {
    'harmonices-mundi': 'https://cjlkaiser-cpu.github.io/harmonices-mundi/',
    'contrapunctus': 'https://cjlkaiser-cpu.github.io/contrapunctus/',
}
NOISE_P = re.compile(r'file://|CORS|WASM no funciona|servidor local|localhost|JavaScript|^(Clic|Arrastra|Pulsa|Usa|Luego)\b|\s=\s|\s\|\s', re.I)
# Descripciones escritas a mano para páginas sin tarjeta ni párrafo descriptivo
DESC_OVERRIDE = {
    'harmonices-mundi': 'La música de las esferas de Kepler (1619): cada planeta canta según su velocidad orbital, v = √(GM(2/r − 1/a)). Coro completo, tríada de Kepler y datos de la NASA.',
    'orbifold-walker': 'Explorador del espacio de acordes de Tymoczko: un caminante recorre el orbifold de acordes en 3D y suena cada paso de la conducción de voces.',
    'tonnetz-atractor': 'Péndulo magnético múltiple sobre el Tonnetz: una partícula caótica es atraída por los nodos y cada nodo que visita suena como nota.',
    'michaelis-menten': 'Cinética enzimática E + S ⇌ ES → E + P. Curva v frente a [S] en tiempo real: Km mide la afinidad enzima-sustrato y Vmax la velocidad máxima.',
    'magmatismo-global': 'Del manto al volcán: fusión por descompresión en dorsales, por deshidratación en subducción y puntos calientes. Cómo se genera el magma en cada contexto tectónico.',
}
TITLE_SUFFIX = re.compile(r'\s*[-–—|·]\s*(EigenLab|[A-Za-z ]*Lab)\b.*$')


def soup(path):
    with open(path, encoding='utf-8', errors='ignore') as f:
        return BeautifulSoup(f.read(), 'html.parser')


def clean(text):
    return re.sub(r'\s+', ' ', text or '').strip()


def cards(index_path, root_keys=False):
    """{href: {title, desc, category, equation}} a partir de las tarjetas de un index.html."""
    if not os.path.exists(index_path):
        return {}
    out = {}
    for a in soup(index_path).find_all('a', href=True):
        href = a['href'].split('#')[0].split('?')[0]
        if not href or href.startswith(('http', '#')) or (href.startswith('../') and not root_keys):
            continue
        h = a.find(['h3', 'h2', 'h4'])
        p = a.find('p')
        if not h:
            continue
        cat = a.find(class_=re.compile('category-tag|tag|badge'))
        eq = a.find(class_=re.compile('equation|formula'))
        if root_keys:   # clave = ruta desde la raíz del repo
            key = os.path.relpath(os.path.normpath(os.path.join(os.path.dirname(index_path), href)), ROOT)
        else:
            key = href[:-len('/index.html')] if href.endswith('/index.html') else href.rstrip('/')
        out.setdefault(key, {
            'title': clean(h.get_text()), 'desc': clean(p.get_text()) if p else '',
            'category': clean(cat.get_text()) if cat else '', 'equation': clean(eq.get_text()) if eq else '',
        })
    return out


def page_info(path):
    """Título y descripción de la propia página, como respaldo."""
    s = soup(path)
    title = clean(s.title.get_text()) if s.title else ''
    title = TITLE_SUFFIX.sub('', title).strip() or title
    meta = s.find('meta', attrs={'name': 'description'})
    desc = clean(meta['content']) if meta and meta.get('content') else ''
    if not desc:
        for p in s.find_all('p'):
            t = clean(p.get_text())
            if len(t) > 40 and not NOISE_P.search(t):
                desc = t
                break
    return title, desc


def guide_path(lab, sim):
    names = [sim, GUIDE_ALIAS.get(sim, sim)] + [sim[:-len(s)] for s in VARIANT_SUFFIXES if sim.endswith(s)]
    for ext in ('.html', '.md'):
        for n in names:
            p = f'{lab}/guides/{n}{ext}'
            if os.path.exists(os.path.join(ROOT, p)):
                return p
    return None


def shorten(text, n=220):
    if len(text) <= n:
        return text
    cut = text[:n].rsplit(' ', 1)[0].rstrip(',;:')
    return cut + '…'


def lab_entries():
    entries = []
    wasm_cards = cards(os.path.join(ROOT, '_portal/wasm-labs.html'), root_keys=True)
    for disc, lab, _ in LABS:
        dname, color = DISCIPLINES[disc]
        if 'Sound' in lab:
            color = SOUND_COLOR
        lab_name = os.path.basename(lab)
        info = cards(os.path.join(ROOT, lab, 'index.html'))
        for group in ('metronomos', 'generativos'):
            for k, v in cards(os.path.join(ROOT, lab, group, 'index.html')).items():
                info.setdefault(k, v)
        lt, ld = page_info(os.path.join(ROOT, lab, 'index.html'))
        entries.append({'type': 'lab', 'id': lab_name.lower().replace(' ', '-'), 'title': lab_name,
                        'desc': shorten(ld), 'discipline': dname, 'color': color, 'lab': lab_name,
                        'url': f'{lab}/index.html'})
        for sim in simulations(lab):
            if os.path.exists(os.path.join(ROOT, lab, sim + '.html')):
                url = f'{lab}/{sim}.html'
            else:   # subproyecto de Sound Lab
                grp = next(g for g in ('metronomos', 'generativos') if os.path.isdir(os.path.join(ROOT, lab, g, sim)))
                url = f'{lab}/{grp}/{sim}/index.html'
            card = info.get(sim + '.html') or info.get(sim) or wasm_cards.get(url) or {}
            pt, pd = page_info(os.path.join(ROOT, url))
            e = {
                'type': 'sim', 'id': sim, 'title': card.get('title') or pt or sim,
                'desc': shorten(DESC_OVERRIDE.get(sim) or card.get('desc') or pd), 'discipline': dname, 'color': color, 'lab': lab_name,
                'url': url,
            }
            if sim in PUBLISHED_ELSEWHERE:
                e['url'] = PUBLISHED_ELSEWHERE[sim]
            if card.get('category'):
                e['category'] = card['category']
            if card.get('equation'):
                e['equation'] = card['equation']
            g = guide_path(lab, sim)
            if g:
                e['guide'] = g
            if sim.endswith('-wasm') or 'eigenlab-core' in open(os.path.join(ROOT, url), encoding='utf-8', errors='ignore').read():
                e['wasm'] = True
            entries.append(e)
    return entries


def portal_entries(known_urls):
    """Cursos y proyectos satélite enlazados desde el portal, y caminos de aprendizaje."""
    out = []
    s = soup(os.path.join(ROOT, '_portal/index.html'))
    for a in s.find_all('a', href=True):
        h = a.find(['h3', 'h4'])
        title = clean(h.get_text()) if h else ''
        if title not in EXTRAS or any(e['title'] == title for e in out):
            continue
        kind, disc = EXTRAS[title]
        dname, color = DISCIPLINES[disc]
        href = a['href']
        p = a.find('p')
        out.append({'type': kind, 'id': re.sub(r'[^a-z0-9]+', '-', title.lower()).strip('-'), 'title': title,
                    'desc': shorten(clean(p.get_text()) if p else ''), 'discipline': dname, 'color': color,
                    'lab': title, 'url': href if href.startswith('http') else os.path.normpath(os.path.join('_portal', href))})
    paths_dir = os.path.join(ROOT, '_portal/paths')
    for f in sorted(os.listdir(paths_dir)):
        if f.endswith('.html') and f != 'index.html':
            t, d = page_info(os.path.join(paths_dir, f))
            t = re.sub(r'\s*-\s*Caminos.*$', '', t)
            out.append({'type': 'path', 'id': f[:-5], 'title': t, 'desc': shorten(d), 'discipline': '',
                         'color': '#94a3b8', 'lab': 'Caminos de aprendizaje', 'url': f'_portal/paths/{f}'})
    return out


def build():
    entries = lab_entries()
    known = {e['url'] for e in entries}
    extra = portal_entries(known)
    seen = set()
    for e in extra:   # sin duplicados por URL
        if e['url'] not in seen and e['url'] not in known:
            entries.append(e)
            seen.add(e['url'])
    return {'site': SITE, 'entries': entries}


def sync_portal_badges(cat, write=True):
    """Pone el número real de simulaciones en las insignias «N sims» de las tarjetas del portal."""
    counts = {}
    for e in cat['entries']:
        if e['type'] == 'sim':
            counts[e['lab']] = counts.get(e['lab'], 0) + 1
    path = os.path.join(ROOT, '_portal/index.html')
    html = open(path, encoding='utf-8').read()
    changed = []

    def fix_card(m):
        block = m.group(0)
        t = re.search(r'<h3[^>]*>\s*([^<]+?)\s*</h3>', block)
        n = counts.get(t.group(1)) if t else None
        if n is None:
            return block
        new = re.sub(r'>\d+\+? sims<', f'>{n} sims<', block)
        if new != block:
            changed.append(f'{t.group(1)} → {n}')
        return new
    new_html = re.sub(r'<a [^>]*class="lab-card[^"]*"[^>]*>.*?</a>', fix_card, html, flags=re.S)
    if write and new_html != html:
        open(path, 'w', encoding='utf-8').write(new_html)
    return changed


def main():
    cat = build()
    badge_changes = sync_portal_badges(cat, write='--check' not in sys.argv)
    js = 'window.EIGENLAB_CATALOG = ' + json.dumps(cat, ensure_ascii=False, separators=(',', ':')) + ';\n'
    js_path = os.path.join(ROOT, '_portal/catalog.js')
    json_path = os.path.join(ROOT, '_portal/catalog.json')
    if '--check' in sys.argv:
        current = open(js_path, encoding='utf-8').read() if os.path.exists(js_path) else ''
        if current != js or badge_changes:
            print('catálogo o insignias del portal desactualizados: ejecuta python3 tools/build_catalog.py')
            sys.exit(1)
        print('catálogo al día')
        return
    with open(json_path, 'w', encoding='utf-8') as f:
        json.dump(cat, f, ensure_ascii=False, indent=1)
    with open(js_path, 'w', encoding='utf-8') as f:
        f.write(js)
    by_type = {}
    for e in cat['entries']:
        by_type[e['type']] = by_type.get(e['type'], 0) + 1
    no_desc = [e['url'] for e in cat['entries'] if e['type'] == 'sim' and not e['desc']]
    print('entradas:', by_type, '| sims sin descripción:', len(no_desc))
    if badge_changes:
        print('insignias del portal actualizadas:', ', '.join(badge_changes))
    for u in no_desc:
        print('   ', u)


if __name__ == '__main__':
    main()
