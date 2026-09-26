/*
 * EigenLab · buscador global (paleta tipo ⌘K)
 *
 * Uso: <script src="search.js" defer></script> (ruta relativa a esta carpeta)
 *  - Atajos: ⌘K / Ctrl+K o "/" para abrir, ↑↓ para moverse, Enter para abrir, ⌘/Ctrl+Enter en pestaña nueva
 *  - Cualquier elemento con [data-eigenlab-search] abre el buscador al pulsarlo
 *  - ?q=texto en la URL abre el buscador con esa búsqueda
 * Los datos salen de catalog.js (generado por tools/build_catalog.py) y se cargan al abrir por primera vez.
 */
(function () {
    'use strict';

    const script = document.currentScript;
    const HERE = script.src.replace(/[^/]*$/, '');          // .../_portal/
    const ROOT = new URL('../', HERE).href;                  // raíz del repo
    const TYPE_LABEL = { sim: 'Simulación', lab: 'Laboratorio', course: 'Curso', lesson: 'Lección', project: 'Proyecto', path: 'Camino' };
    const MAX_RESULTS = 40;

    let catalog = null, loading = null, overlay, input, list, status, results = [], active = 0, lastFocus = null;

    // ---------------------------------------------------------------- datos
    function loadCatalog() {
        if (catalog) return Promise.resolve(catalog);
        if (loading) return loading;
        loading = new Promise((resolve, reject) => {
            const s = document.createElement('script');
            s.src = HERE + 'catalog.js';
            s.onload = () => {
                catalog = (window.EIGENLAB_CATALOG.entries || []).map(e => Object.assign({}, e, {
                    _title: norm(e.title),
                    _meta: norm([e.lab, e.discipline, e.category, TYPE_LABEL[e.type]].join(' ')),
                    _body: norm([e.desc, e.equation, e.id].join(' ')),
                }));
                resolve(catalog);
            };
            s.onerror = () => { loading = null; reject(new Error('No se pudo cargar el catálogo')); };
            document.head.appendChild(s);
        });
        return loading;
    }

    function norm(s) {
        return (s || '').normalize('NFD').replace(/[\u0300-\u036f]/g, '').toLowerCase();
    }

    function search(q) {
        const terms = norm(q).split(/\s+/).filter(Boolean);
        if (!terms.length) return featured();
        const out = [];
        for (const e of catalog) {
            let score = 0;
            for (const t of terms) {
                let s = 0;
                if (e._title.startsWith(t)) s = 12;
                else if (new RegExp('\\b' + t.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')).test(e._title)) s = 9;
                else if (e._title.includes(t)) s = 6;
                else if (e._meta.includes(t)) s = 3;
                else if (e._body.includes(t)) s = 1;
                if (!s) { score = 0; break; }
                score += s;
            }
            if (score) {
                if (e.type !== 'sim') score += 1;                    // labs y caminos ligeramente arriba
                out.push([score, e]);
            }
        }
        out.sort((a, b) => b[0] - a[0] || a[1].title.localeCompare(b[1].title, 'es'));
        return out.slice(0, MAX_RESULTS).map(x => x[1]);
    }

    function featured() {                                         // sin búsqueda: caminos y labs
        return catalog.filter(e => e.type === 'path').slice(0, 6)
            .concat(catalog.filter(e => e.type === 'lab'));
    }

    function href(e) {
        return /^https?:/.test(e.url) ? e.url : ROOT + e.url.split('/').map(encodeURIComponent).join('/');
    }

    // ---------------------------------------------------------------- UI
    const CSS = `
    .els-overlay{position:fixed;inset:0;z-index:1000;display:none;align-items:flex-start;justify-content:center;
      padding:10vh 16px 16px;background:rgba(3,7,18,.72);backdrop-filter:blur(6px);-webkit-backdrop-filter:blur(6px)}
    .els-overlay.open{display:flex}
    .els-box{width:100%;max-width:680px;max-height:78vh;display:flex;flex-direction:column;background:#0f172a;
      border:1px solid #334155;border-radius:16px;box-shadow:0 30px 80px rgba(0,0,0,.6);overflow:hidden;
      font-family:Inter,-apple-system,BlinkMacSystemFont,sans-serif;color:#f8fafc}
    .els-head{display:flex;align-items:center;gap:10px;padding:14px 16px;border-bottom:1px solid #1e293b}
    .els-head svg{flex:none;color:#94a3b8}
    .els-input{flex:1;min-width:0;background:none;border:0;outline:0;color:#f8fafc;font:inherit;font-size:17px}
    .els-input::placeholder{color:#64748b}
    .els-kbd{flex:none;font-size:11px;color:#94a3b8;border:1px solid #334155;border-radius:6px;padding:2px 6px}
    .els-list{list-style:none;margin:0;padding:6px;overflow-y:auto}
    .els-item{display:flex;gap:12px;align-items:flex-start;padding:10px 12px;border-radius:10px;cursor:pointer;text-decoration:none;color:inherit}
    .els-item[aria-selected=true]{background:#1e293b}
    .els-dot{flex:none;width:10px;height:10px;margin-top:6px;border-radius:50%;box-shadow:0 0 10px currentColor}
    .els-main{flex:1;min-width:0}
    .els-title{font-size:15px;font-weight:500;display:flex;gap:8px;align-items:baseline;flex-wrap:wrap}
    .els-type{font-size:11px;font-weight:500;letter-spacing:.04em;text-transform:uppercase;color:#94a3b8}
    .els-desc{font-size:13px;color:#94a3b8;margin-top:2px;overflow:hidden;display:-webkit-box;-webkit-line-clamp:2;-webkit-box-orient:vertical}
    .els-meta{font-size:12px;color:#64748b;margin-top:4px;display:flex;gap:10px;flex-wrap:wrap}
    .els-meta a{color:#94a3b8;text-decoration:underline;text-underline-offset:2px}
    .els-meta a:hover{color:#f8fafc}
    .els-status{padding:10px 18px;font-size:12px;color:#64748b;border-top:1px solid #1e293b;display:flex;justify-content:space-between;gap:12px}
    .els-empty{padding:28px 18px;text-align:center;color:#94a3b8;font-size:14px}
    mark.els-hl{background:none;color:#f8fafc;font-weight:600;text-decoration:underline;text-decoration-color:rgba(248,250,252,.35);text-underline-offset:3px}
    @media (max-width:560px){.els-overlay{padding-top:12px}.els-box{max-height:88vh}.els-status span:last-child{display:none}}
    @media (prefers-reduced-motion:no-preference){.els-overlay.open .els-box{animation:elsIn .16s ease-out}}
    @keyframes elsIn{from{opacity:0;transform:translateY(-8px) scale(.98)}to{opacity:1;transform:none}}`;

    function build() {
        const style = document.createElement('style');
        style.textContent = CSS;
        document.head.appendChild(style);
        overlay = document.createElement('div');
        overlay.className = 'els-overlay';
        overlay.innerHTML = `
          <div class="els-box" role="dialog" aria-modal="true" aria-label="Buscar en EigenLab">
            <div class="els-head">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" aria-hidden="true"><circle cx="11" cy="11" r="7"/><path d="m20 20-3.5-3.5"/></svg>
              <input class="els-input" type="text" enterkeyhint="search" placeholder="Buscar simulaciones, laboratorios, caminos…" autocomplete="off" spellcheck="false"
                     role="combobox" aria-expanded="true" aria-controls="els-list" aria-autocomplete="list">
              <span class="els-kbd">Esc</span>
            </div>
            <ul class="els-list" id="els-list" role="listbox"></ul>
            <div class="els-status"><span></span><span>↑↓ navegar · ↵ abrir · ${/Mac|iPhone|iPad/.test(navigator.platform || navigator.userAgent) ? '⌘' : 'Ctrl+'}↵ pestaña nueva</span></div>
          </div>`;
        document.body.appendChild(overlay);
        input = overlay.querySelector('.els-input');
        list = overlay.querySelector('.els-list');
        status = overlay.querySelector('.els-status span');
        overlay.addEventListener('mousedown', e => { if (e.target === overlay) close(); });
        input.addEventListener('input', render);
        input.addEventListener('keydown', onKey);
        list.addEventListener('mousemove', e => {
            const li = e.target.closest('.els-item');
            if (li) select(+li.dataset.i, false);
        });
    }

    function esc(s) {
        return (s || '').replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
    }

    function highlight(text, q) {
        let out = esc(text);
        const terms = q.trim().split(/\s+/).filter(t => t.length > 1);
        if (!terms.length) return out;
        // resaltado insensible a tildes: se busca sobre la versión normalizada y se marca en el original
        const n = norm(text), marks = [];
        for (const t of terms) {
            const nt = norm(t);
            let i = n.indexOf(nt);
            while (i !== -1) { marks.push([i, i + nt.length]); i = n.indexOf(nt, i + nt.length); }
        }
        if (!marks.length) return out;
        marks.sort((a, b) => a[0] - b[0]);
        let res = '', pos = 0;
        for (const [a, b] of marks) {
            if (a < pos) continue;
            res += esc(text.slice(pos, a)) + '<mark class="els-hl">' + esc(text.slice(a, b)) + '</mark>';
            pos = b;
        }
        return res + esc(text.slice(pos));
    }

    function render() {
        const q = input.value;
        results = search(q);
        active = 0;
        if (!results.length) {
            list.innerHTML = `<li class="els-empty">Nada para «${esc(q)}». Prueba con un concepto: <em>caos</em>, <em>ondas</em>, <em>ADN</em>…</li>`;
        } else {
            list.innerHTML = results.map((e, i) => {
                const guide = e.guide ? `<a href="${href({ url: e.guide })}" data-guide>Guía teórica</a>` : '';
                const eq = e.equation ? `<span>${esc(e.equation)}</span>` : '';
                const lab = e.type === 'sim' ? `<span>${esc(e.lab)}</span>` : (e.discipline ? `<span>${esc(e.discipline)}</span>` : '');
                const wasm = e.wasm ? '<span>⚡ WASM</span>' : '';
                return `<li class="els-item" role="option" id="els-opt-${i}" data-i="${i}" aria-selected="${i === 0}">
                    <span class="els-dot" style="color:${e.color};background:${e.color}"></span>
                    <div class="els-main">
                      <div class="els-title"><span>${highlight(e.title, q)}</span><span class="els-type">${TYPE_LABEL[e.type] || ''}</span></div>
                      ${e.desc ? `<div class="els-desc">${highlight(e.desc, q)}</div>` : ''}
                      <div class="els-meta">${lab}${eq}${wasm}${guide}</div>
                    </div></li>`;
            }).join('');
        }
        const nSims = catalog.filter(e => e.type === 'sim').length;
        status.textContent = q.trim() ? `${results.length}${results.length === MAX_RESULTS ? '+' : ''} resultados` : `${nSims} simulaciones · escribe para buscar`;
        input.setAttribute('aria-activedescendant', results.length ? 'els-opt-0' : '');
        list.scrollTop = 0;
    }

    function select(i, scroll = true) {
        if (!results.length) return;
        active = (i + results.length) % results.length;
        list.querySelectorAll('.els-item').forEach((li, k) => li.setAttribute('aria-selected', k === active));
        input.setAttribute('aria-activedescendant', 'els-opt-' + active);
        if (scroll) list.children[active].scrollIntoView({ block: 'nearest' });
    }

    function go(e, newTab) {
        const url = href(e);
        if (newTab || /^https?:/.test(e.url) && !url.startsWith(location.origin)) window.open(url, '_blank', 'noopener');
        else location.href = url;
    }

    function onKey(ev) {
        if (ev.key === 'ArrowDown') { ev.preventDefault(); select(active + 1); }
        else if (ev.key === 'ArrowUp') { ev.preventDefault(); select(active - 1); }
        else if (ev.key === 'Enter' && results[active]) { ev.preventDefault(); go(results[active], ev.metaKey || ev.ctrlKey); }
        else if (ev.key === 'Escape') { ev.preventDefault(); close(); }
    }

    function open(q) {
        if (!overlay) build();
        lastFocus = document.activeElement;
        overlay.classList.add('open');
        document.documentElement.style.overflow = 'hidden';
        if (typeof q === 'string') input.value = q;
        input.focus();
        input.select();
        list.innerHTML = '<li class="els-empty">Cargando catálogo…</li>';
        loadCatalog().then(render).catch(err => { list.innerHTML = `<li class="els-empty">${esc(err.message)}</li>`; });
    }

    function close() {
        overlay.classList.remove('open');
        document.documentElement.style.overflow = '';
        if (lastFocus && lastFocus.focus) lastFocus.focus();
    }

    // ---------------------------------------------------------------- arranque
    document.addEventListener('keydown', ev => {
        const typing = /^(INPUT|TEXTAREA|SELECT)$/.test(ev.target.tagName) || ev.target.isContentEditable;
        if ((ev.key === 'k' || ev.key === 'K') && (ev.metaKey || ev.ctrlKey)) { ev.preventDefault(); open(); }
        else if (ev.key === '/' && !typing) { ev.preventDefault(); open(); }
    });
    document.addEventListener('click', ev => {
        const t = ev.target.closest('[data-eigenlab-search]');
        if (t) { ev.preventDefault(); open(); return; }
        const li = ev.target.closest && ev.target.closest('.els-item');
        if (li && !ev.target.closest('[data-guide]')) go(results[+li.dataset.i], ev.metaKey || ev.ctrlKey);
    });
    const isMac = /Mac|iPhone|iPad/.test(navigator.platform || navigator.userAgent);
    const setKbd = () => document.querySelectorAll('[data-eigenlab-kbd]').forEach(k => { k.textContent = isMac ? '⌘K' : 'Ctrl K'; });
    document.readyState === 'loading' ? document.addEventListener('DOMContentLoaded', setKbd) : setKbd();
    const q = new URLSearchParams(location.search).get('q');
    if (q) (document.readyState === 'loading' ? document.addEventListener('DOMContentLoaded', () => open(q)) : open(q));

    window.EigenLabSearch = { open, close };
})();
