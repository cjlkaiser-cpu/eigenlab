"""EigenLab promo — versión larga (~2 min, 44 compases a 90 BPM).
Reutiliza el motor de render y las escenas de video.py y añade las nuevas.
Uso: python3.11 promo2.py <escena>   |   python3.11 promo2.py still <escena> <frame>
"""
import sys, os, subprocess, math
import numpy as np
from PIL import Image, ImageFont
from scipy.ndimage import gaussian_filter, map_coordinates
from video import (W, H, FPS, BPM, BAR, HERE, FONT_SERIF, FONT_SANS, C, DISCIPLINES,
                   font, ease, ease_out, splat, seg_points, polyline_points, finish, spaced_text,
                   overlay, rgba, caption, caption_fast, shade, lorenz_traj, lorenz_project,
                   graph_data, scene_intro_lorenz, scene_pendulum, scene_boids, scene_grayscott,
                   scene_mandel, scene_galaxy, scene_neural, scene_logo)

BEAT = 60 / BPM
NBARS = 44
DUR_TOTAL = NBARS * BAR
FONT_MATH = '/System/Library/Fonts/Supplemental/STIXTwoText-Italic.ttf'


def fio(t, dur, a=0.25, b=0.2):
    return ease(t / a) * (1 - ease((t - dur + b) / b))


def glow_kernel(r, s):
    gx, gy = np.meshgrid(np.arange(-r, r + 1), np.arange(-r, r + 1))
    return gx.ravel().astype(float), gy.ravel().astype(float), np.exp(-(gx ** 2 + gy ** 2) / s).ravel()


def splat_glow(buf, x, y, col, w, r=4, s=4.0, size=1.0):
    gx, gy, gk = glow_kernel(r, s)
    x = np.atleast_1d(x); y = np.atleast_1d(y)
    w = np.broadcast_to(np.asarray(w, float), x.shape)
    size = np.broadcast_to(np.asarray(size, float), x.shape)
    col = np.asarray(col, float)
    cc = np.repeat(col, len(gk), 0) if col.ndim == 2 else col
    splat(buf, (x[:, None] + gx * size[:, None]).ravel(), (y[:, None] + gy * size[:, None]).ravel(),
          cc, (w[:, None] * gk).ravel())


def mix(a, b, u):
    u = np.asarray(u, float)[..., None]
    return a * (1 - u) + b * u


# ================================================================ SONIDO: Chladni
def scene_chladni():
    dur = 2 * BAR
    rng = np.random.default_rng(4)
    n = 60000
    P = rng.random((n, 2))
    modes = [(2, 5), (3, 7), (1, 6), (4, 9)]
    side, cx, cy = 800, W / 2, H / 2 - 20
    col_a, col_b = C['sound'], np.array([0.95, 0.85, 1.0])
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        mi = min(int(t / (BAR / 2)), 3)
        a, b = modes[mi]
        if fi > 0 and int(t / (BAR / 2)) != int((fi - 1) / FPS / (BAR / 2)):
            P += rng.normal(0, 0.05, P.shape)   # la placa cambia de modo: la arena salta
        pa, pb = np.pi * a, np.pi * b
        for _ in range(3):
            x, y = P[:, 0], P[:, 1]
            f = np.cos(pa * x) * np.cos(pb * y) - np.cos(pb * x) * np.cos(pa * y)
            fx = -pa * np.sin(pa * x) * np.cos(pb * y) + pb * np.sin(pb * x) * np.cos(pa * y)
            fy = -pb * np.cos(pa * x) * np.sin(pb * y) + pa * np.cos(pb * x) * np.sin(pa * y)
            P[:, 0] -= 0.00035 * f * fx
            P[:, 1] -= 0.00035 * f * fy
            P += rng.normal(0, 1, P.shape) * (0.004 * np.abs(f))[:, None]
            P = np.abs(P); P = 1 - np.abs(1 - P)       # reflejo en los bordes
        buf = np.zeros((H, W, 3), np.float32)
        u = np.clip(np.abs(f) * 2, 0, 1)
        splat(buf, cx + (P[:, 0] - 0.5) * side, cy + (P[:, 1] - 0.5) * side, mix(col_b, col_a, u), 0.09)
        # marco de la placa
        e = np.linspace(0, 1, 3000)
        for (x0, y0, x1, y1) in ((0, 0, 1, 0), (1, 0, 1, 1), (1, 1, 0, 1), (0, 1, 0, 0)):
            splat(buf, cx + (x0 + (x1 - x0) * e - 0.5) * (side + 24), cy + (y0 + (y1 - y0) * e - 0.5) * (side + 24), col_a, 0.05)
        img = finish(buf, exposure=1.2, bloom=0.9, fade=fio(t, dur))

        def fn(d, layer):
            aa = ease((t - 0.4) / 0.4) * (1 - ease((t - dur + 0.4) / 0.3))
            fm = ImageFont.truetype(FONT_MATH, 40)
            d.text((W - 150, 200), f'n = {a},  m = {b}', font=fm, fill=rgba(col_b, aa * 0.8), anchor='rs')
        img = overlay(img, fn)
        yield caption(img, t, dur, 'Física del sonido · Resonancia', 'Figuras de Chladni', 'sound')


# ================================================================ QUÍMICA: orbitales
def sample_orbital(psi, box, n, rng):
    out = []
    got = 0
    c = rng.uniform(-box, box, (400000, 3))
    mx = (psi(c) ** 2).max() * 1.1
    while got < n:
        c = rng.uniform(-box, box, (1000000, 3))
        p = psi(c)
        keep = rng.random(len(c)) < p ** 2 / mx
        out.append(np.c_[c[keep], np.sign(p[keep])]); got += keep.sum()
    X = np.concatenate(out)[:n]
    r97 = np.percentile(np.linalg.norm(X[:, :3], axis=1), 97)
    X[:, :3] /= r97
    return X


def scene_orbitals():
    dur = 2 * BAR
    rng = np.random.default_rng(8)
    n = 70000
    r = lambda c: np.linalg.norm(c, axis=1)
    orbs = [
        ('2p', lambda c: c[:, 2] * np.exp(-r(c) / 2), 14),
        ('3d', lambda c: (3 * c[:, 2] ** 2 - r(c) ** 2) * np.exp(-r(c) / 3), 26),
        ('4f', lambda c: c[:, 2] * (5 * c[:, 2] ** 2 - 3 * r(c) ** 2) * np.exp(-r(c) / 4), 40),
    ]
    S = [sample_orbital(p, b, n, rng) for _, p, b in orbs]
    for s_ in S:
        rng.shuffle(s_)
    pos_c, neg_c = C['chemistry'] * 1.1, np.array([0.55, 0.5, 1.0])
    marks = [(0, 1.5), (1.5, 2.1), (3.4, 4.0)]
    f_orb = font(FONT_SERIF, 46, 'Regular')
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        if t < 1.5:
            A, u, cur = S[0], 0.0, 0
        elif t < 2.1:
            A, u, cur = None, ease((t - 1.5) / 0.6), 1
            X = mix(S[0], S[1], u)
        elif t < 3.4:
            A, u, cur = S[1], 0.0, 1
        elif t < 4.0:
            A, u, cur = None, ease((t - 3.4) / 0.6), 2
            X = mix(S[1], S[2], u)
        else:
            A, u, cur = S[2], 0.0, 2
        if A is not None:
            X = A
        # rotación 3D
        ang = 0.4 + 0.5 * t
        x, y, z = X[:, 0], X[:, 1], X[:, 2]
        xr = x * math.cos(ang) - y * math.sin(ang); yr = x * math.sin(ang) + y * math.cos(ang)
        tl = 0.35
        zs = z * math.cos(tl) - yr * math.sin(tl); dep = z * math.sin(tl) + yr * math.cos(tl)
        pr = 1 / (1 + dep * 0.25)
        sc = 400
        sgn = (X[:, 3] + 1) / 2
        col = mix(neg_c, pos_c, sgn)
        buf = np.zeros((H, W, 3), np.float32)
        splat(buf, W / 2 + xr * sc * pr, H / 2 - 30 - zs * sc * pr, col, 0.24 * (1.1 - 0.3 * dep))
        # núcleo
        splat_glow(buf, [W / 2], [H / 2 - 30], np.array([1, 1, 1]), 1.2, r=5, s=5)
        img = finish(buf, exposure=1.2, bloom=1.0, fade=fio(t, dur))

        def fn(d, layer):
            for i, (nm, _, _) in enumerate(orbs):
                a = 0.95 if i == cur else 0.25
                a *= ease((t - 0.3) / 0.4) * (1 - ease((t - dur + 0.4) / 0.3))
                d.text((W - 150, 330 + i * 80), nm, font=f_orb, fill=rgba(C['white'] if i == cur else C['muted'], a), anchor='rs')
                if i == cur:
                    d.rectangle([W - 135, 330 + i * 80 - 34, W - 131, 330 + i * 80 - 2], fill=rgba(C['chemistry'], a))
        img = overlay(img, fn)
        yield caption(img, t, dur, 'Química · Mecánica cuántica', 'Orbitales del hidrógeno', 'chemistry')


# ================================================================ BIOQUÍMICA: ADN
def scene_dna():
    dur = 2 * BAR
    rng = np.random.default_rng(12)
    pitch, R = 380.0, 170.0
    k = 2 * np.pi / pitch
    xs = np.arange(-300, W + 300, 0.8)
    n_bp = 200
    bases = rng.integers(0, 4, n_bp)
    base_cols = [(C['biochem'], np.array([0.4, 0.85, 1.0])), (np.array([0.4, 0.85, 1.0]), C['biochem']),
                 (np.array([1.0, 0.72, 0.3]), np.array([0.65, 0.5, 1.0])), (np.array([0.65, 0.5, 1.0]), np.array([1.0, 0.72, 0.3]))]
    dust = rng.random((1500, 3)) * [W, H, 1]
    tilt = math.radians(-6)
    ct, st = math.cos(tilt), math.sin(tilt)

    def to_screen(x, y, z):
        pr = 1 / (1 - z / 1800)
        X = W / 2 + (x - W / 2) * pr; Y = H / 2 - 30 + y * pr
        return W / 2 + (X - W / 2) * ct - (Y - H / 2) * st, H / 2 + (X - W / 2) * st + (Y - H / 2) * ct
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        buf = np.zeros((H, W, 3), np.float32)
        shift = t * 35
        ph = 1.1 * t
        for si, off in enumerate((0.0, 2.3)):
            a = k * (xs + shift) + ph + off
            y = R * np.sin(a); z = R * np.cos(a)
            sx, sy = to_screen(xs, y, z)
            br = 0.5 + 0.5 * z / R
            c = C['biochem'] * (0.7 + 0.3 * br[:, None]) + np.array([0.3, 0.2, 0.3]) * br[:, None] ** 3
            for dy in (-1.2, 0, 1.2):
                splat(buf, sx, sy + dy, c, 0.30 * (0.25 + 0.9 * br))
        # pares de bases
        bx = np.arange(n_bp) * 34.0 - 300 - (shift % 34)
        idx = (np.arange(n_bp) + int(shift // 34)) % n_bp
        a1 = k * (bx + shift) + ph; a2 = a1 + 2.3
        p1 = to_screen(bx, R * np.sin(a1), R * np.cos(a1)); p2 = to_screen(bx, R * np.sin(a2), R * np.cos(a2))
        zmid = (np.cos(a1) + np.cos(a2)) / 2
        px, py, seg, u = seg_points(p1[0], p1[1], p2[0], p2[1], 0.9)
        cb = np.array([base_cols[b][0] for b in bases[idx]]); ce = np.array([base_cols[b][1] for b in bases[idx]])
        col = np.where((u < 0.5)[:, None], cb[seg], ce[seg])
        splat(buf, px, py, col, 0.22 * (0.3 + 0.7 * (0.5 + 0.5 * zmid[seg])) * (np.abs(u - 0.5) > 0.04))
        splat_glow(buf, np.r_[p1[0], p2[0]], np.r_[p1[1], p2[1]], C['biochem'] * 0.6 + 0.4,
                   np.r_[0.5 + 0.5 * np.cos(a1), 0.5 + 0.5 * np.cos(a2)] * 0.9 + 0.1, r=3, s=3)
        # polvo en suspensión
        dx = (dust[:, 0] + t * 20 * (0.5 + dust[:, 2])) % W
        splat(buf, dx, dust[:, 1], C['biochem'] * 0.5 + 0.3, 0.15 * dust[:, 2])
        img = finish(buf, exposure=1.2, bloom=1.0, fade=fio(t, dur))
        yield caption(img, t, dur, 'Bioquímica · Genética', 'La doble hélice del ADN', 'biochem')


# ================================================================ GEOLOGÍA: erosión
def scene_erosion():
    dur = 2 * BAR
    rng = np.random.default_rng(15)
    h, w = H // 4, W // 4
    hm = sum(gaussian_filter(rng.standard_normal((h, w)), s, mode='wrap') * s ** 1.1 for s in (6, 14, 30, 70))
    hm = (hm - hm.min()) / (hm.max() - hm.min())
    hm = hm ** 1.3
    nd = 5000
    P = rng.random((nd, 2)) * [w - 2, h - 2]
    V = np.zeros((nd, 2))
    trail = np.zeros((H, W, 3), np.float32)
    water = np.array([0.55, 0.85, 1.0])
    ema = P.copy()

    def step():
        nonlocal P, V, hm, ema
        gy, gx = np.gradient(gaussian_filter(hm, 1.0))
        ix = np.clip(P[:, 0].astype(int), 0, w - 1); iy = np.clip(P[:, 1].astype(int), 0, h - 1)
        V = V * 0.85 - np.c_[gx[iy, ix], gy[iy, ix]] * 40
        sp = np.hypot(V[:, 0], V[:, 1])
        V = V / np.maximum(sp, 1e-9)[:, None] * np.minimum(sp, 1.2)[:, None]
        old = P.copy()
        P = P + V
        np.subtract.at(hm, (iy, ix), 0.0009 * np.minimum(sp, 1.2))      # erosión
        ema = ema * 0.9 + P * 0.1
        stuck = np.hypot(*(P - ema).T) < 0.6
        dead = stuck | (P[:, 0] < 1) | (P[:, 0] > w - 2) | (P[:, 1] < 1) | (P[:, 1] > h - 2) | (sp < 0.02) | (rng.random(nd) < 0.01)
        P[dead] = rng.random((dead.sum(), 2)) * [w - 2, h - 2]; V[dead] = 0
        old[dead] = P[dead]; ema[dead] = P[dead] + 5
        return old
    for _ in range(120):
        step()
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        trail *= 0.965
        for _ in range(2):
            old = step()
            spd = np.clip(np.hypot(*(P - old).T) / 0.8, 0, 1) ** 2
            px, py, seg, u = seg_points(old[:, 0] * 4, old[:, 1] * 4, P[:, 0] * 4, P[:, 1] * 4, 1.0)
            splat(trail, px, py, water, 0.03 * spd[seg])
        big = np.asarray(Image.fromarray(hm.astype(np.float32)).resize((W, H), Image.BICUBIC))
        c = big * 16
        gy, gx = np.gradient(c)
        dpx = np.abs(c - np.round(c)) / (np.hypot(gx, gy) + 1e-4)
        lines = np.exp(-(dpx / 0.9) ** 2) * (0.3 + 0.7 * big)
        major = (np.round(c) % 5 == 0)
        lines = lines * np.where(major, 1.0, 0.45)
        sgy, sgx = np.gradient(gaussian_filter(hm, 1))
        shadeimg = np.clip(0.5 + (sgx - sgy) * 18, 0, 1)
        shade_big = np.asarray(Image.fromarray(shadeimg.astype(np.float32)).resize((W, H), Image.BILINEAR))
        buf = (lines[..., None] * C['geology'] * 0.55 + (shade_big * big)[..., None] * C['geology'] * 0.05 + trail).astype(np.float32)
        img = finish(buf, exposure=1.1, bloom=0.7, fade=fio(t, dur))
        yield caption(img, t, dur, 'Geología · Modelado del relieve', 'Erosión hidráulica', 'geology')


# ================================================================ ASTRONOMÍA / IA (envolturas)
def wrap_meta(gen_fn, label=None, title=None):
    def scene():
        dur = 2 * BAR
        for fi, (img, (lab, tit, ck)) in enumerate(gen_fn(dur)):
            t = fi / FPS
            img = (img.astype(np.float32) * fio(t, dur)).astype(np.uint8)
            yield caption(img, t, dur, label or lab, title or tit, ck)
    return scene


# ================================================================ COMPUTACIÓN: vida
def scene_life():
    dur = 2 * BAR
    rng = np.random.default_rng(6)
    cs = 12
    gh, gw = H // cs, W // cs
    G = rng.random((gh, gw)) < 0.28

    def life(G):
        nb = sum(np.roll(np.roll(G, i, 0), j, 1) for i in (-1, 0, 1) for j in (-1, 0, 1) if (i, j) != (0, 0))
        return (nb == 3) | (G & (nb == 2))
    for _ in range(25):
        G = life(G)
    heat = G.astype(np.float32)
    born = np.zeros_like(heat)
    block = np.zeros((cs, cs), np.float32); block[1:-1, 1:-1] = 1
    glider = np.array([[0, 1, 0], [0, 0, 1], [1, 1, 1]], bool)
    blue, white = C['computation'], np.array([0.8, 0.9, 1.0])
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        if fi % 5 == 0:                                   # una generación por semicorchea
            newG = life(G)
            born = (newG & ~G).astype(np.float32)
            G = newG
            if fi % 40 == 0:                              # sembrar planeadores para mantener vida
                for _ in range(6):
                    y, x = rng.integers(0, gh - 3), rng.integers(0, gw - 3)
                    G[y:y + 3, x:x + 3] |= np.rot90(glider, rng.integers(0, 4))
        heat = np.maximum(heat * 0.78, G.astype(np.float32))
        born *= 0.8
        v = np.kron(heat, block)[:H, :W]
        b = np.kron(born, block)[:H, :W]
        buf = (v[..., None] * blue * 0.55 + b[..., None] * white * 0.9).astype(np.float32)
        img = finish(buf, exposure=1.1, bloom=0.9, fade=fio(t, dur))
        yield caption(img, t, dur, 'Computación · Autómatas celulares', 'El juego de la vida de Conway', 'computation')


# ================================================================ MÚSICA: filotaxis
def scene_phyllo():
    dur = 2 * BAR
    N = 2300
    n = np.arange(1, N + 1)
    golden = math.pi * (3 - math.sqrt(5))
    appear = 3.0 * (n / N) ** 0.8
    rad = 9.8 * np.sqrt(n)
    rel = n / N
    base = mix(np.array([1.0, 0.85, 0.95]), C['music'], np.clip(rel * 1.6, 0, 1))
    base = mix(base, C['sound'], np.clip((rel - 0.5) * 2, 0, 1))
    fams = [8, 13, 21, 34, 21, 13, 34, 55]
    f_big = font(FONT_SERIF, 64, 'Regular')
    f_sm = font(FONT_SANS, 20, 'Medium')
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        beat = int(t / BEAT); tb = t - beat * BEAT
        F = fams[beat % len(fams)]
        pulse = math.exp(-tb * 4)
        vis = t >= appear
        pop = np.clip((t - appear) / 0.3, 0, 1)
        th = n * golden + 0.08 * t
        x = W / 2 + rad * np.cos(th); y = H / 2 - 10 + rad * np.sin(th)
        hl = ((n % F) % 2 == 0).astype(float) * pulse
        buf = np.zeros((H, W, 3), np.float32)
        on = np.where(vis)[0]
        size = 0.5 + 0.6 * rel[on] ** 0.5 + 0.3 * (1 - pop[on])
        splat_glow(buf, x[on], y[on], mix(base[on], np.array([1, 1, 1]), hl[on] * 0.4), (0.4 + 0.8 * hl[on]) * pop[on],
                   r=5, s=5, size=size)
        img = finish(buf, exposure=1.2, bloom=1.0, fade=fio(t, dur))

        def fn(d, layer):
            a = ease((t - 1.0) / 0.5) * (1 - ease((t - dur + 0.4) / 0.3))
            d.text((W - 150, 230), '137,5°', font=f_big, fill=rgba(C['white'], a), anchor='rs')
            spaced_text(d, (W - 150 - 290, 268), 'ÁNGULO ÁUREO', f_sm, rgba(C['muted'], a), 4)
            if t > 3.0 and a > 0:
                d.text((W - 150, 340), f'{F}', font=f_big, fill=rgba(C['music'] * 0.5 + 0.5, a * (0.4 + 0.6 * pulse)), anchor='rs')
                spaced_text(d, (W - 150 - 290, 378), 'ESPIRALES', f_sm, rgba(C['muted'], a), 4)
        img = overlay(img, fn)
        yield caption(img, t, dur, 'Música · Sistema Armónico Áureo', 'Filotaxis y Fibonacci', 'music')


# ================================================================ MONTAJE: clips nuevos
def clip_clifford(dur):
    rng = np.random.default_rng(3)
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        a, b, c, d = -1.7 + 0.25 * math.sin(t * 0.9), 1.3 + 0.2 * t, -0.1 + 0.15 * t, -1.21
        P = rng.uniform(-1, 1, (180000, 2))
        buf = np.zeros((H, W, 3), np.float32)
        for i in range(24):
            x, y = P[:, 0], P[:, 1]
            P = np.c_[np.sin(a * y) + c * np.cos(a * x), np.sin(b * x) + d * np.cos(b * y)]
            if i >= 10:
                u = np.clip((P[:, 1] + 2) / 4, 0, 1)
                splat(buf, W / 2 + P[:, 0] * 230, H / 2 - 20 + P[:, 1] * 230, mix(C['chemistry'], np.array([0.7, 0.55, 1.0]), u), 0.012)
        yield finish(buf, exposure=1.0, bloom=0.8), ('Arte generativo', 'Atractor de Clifford', 'chemistry')


def clip_harmonograph(dur):
    s = np.linspace(0, 160, 90000)
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        d = 0.012
        e = np.exp(-d * s)
        x = (np.sin(2.01 * s + 0.4 * t) + np.sin(3.0 * s + 1.2)) * e
        y = (np.sin(3.003 * s + 0.5) + np.sin(2.0 * s + 0.9 * t + 2.0)) * e
        frac = 0.35 + 0.65 * ease_out(t / dur * 1.3)
        m = int(len(s) * frac)
        px, py, pos = polyline_points(W / 2 + x[:m] * 210, H / 2 - 20 + y[:m] * 210, 1.0)
        u = pos / m
        buf = np.zeros((H, W, 3), np.float32)
        splat(buf, px, py, mix(C['physics'], np.array([0.6, 1.0, 0.9]), u), 0.04 + 0.12 * u ** 4)
        yield finish(buf, exposure=1.2, bloom=0.9), ('Física · Oscilaciones', 'Harmonógrafo', 'physics')


def clip_julia(dur):
    h, w = H // 2, W // 2
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        cc = 0.7885 * np.exp(1j * (2.75 + 0.22 * t))
        xs = (np.arange(w) - w / 2) / w * 3.3
        ys = (np.arange(h) - h / 2) / w * 3.3
        Z = xs[None, :] + 1j * ys[:, None]
        it = np.zeros(Z.shape); alive = np.ones(Z.shape, bool)
        for i in range(160):
            Z[alive] = Z[alive] ** 2 + cc
            esc = alive & (np.abs(Z) > 4)
            it[esc] = i + 1 - np.log2(np.log(np.abs(Z[esc])))
            alive &= ~esc
        lv = np.log1p(it)
        e = np.clip((lv - 1.2) / 3.2, 0, 1)
        band = 0.55 + 0.45 * np.cos(it * 0.3)
        col = (e[..., None] ** 1.6 * band[..., None] * (C['sound'][None, None] * 1.4 + e[..., None] ** 3 * np.array([0.5, 0.5, 0.4])))
        col = np.where(alive[..., None], C['sound'] * 0.08, col).astype(np.float32)
        big = np.stack([np.asarray(Image.fromarray(col[..., k]).resize((W, H), Image.BICUBIC)) for k in range(3)], -1)
        yield finish(np.clip(big, 0, None), exposure=1.0, bloom=0.6), ('Matemáticas · Caos', 'Conjuntos de Julia', 'sound')


def clip_flow(dur):
    rng = np.random.default_rng(10)
    n = 14000
    P = rng.random((n, 2)) * [W, H]
    trail = np.zeros((H, W, 3), np.float32)
    life_ = rng.random(n) * 80

    def adv(P, t):
        x, y = P[:, 0], P[:, 1]
        th = 2.2 * (np.sin(x * 0.0035 + t * 0.4) + np.cos(y * 0.0042 - t * 0.3) + np.sin((x - y) * 0.0021))
        return P + np.c_[np.cos(th), np.sin(th)] * 3.2
    tt = 0
    for fi in range(-45, int(dur * FPS)):
        t = max(fi, 0) / FPS
        tt += 1 / FPS
        trail *= 0.965
        Q = adv(P, tt)
        u = np.clip(P[:, 1] / H, 0, 1)
        px, py, seg, _ = seg_points(P[:, 0], P[:, 1], Q[:, 0], Q[:, 1], 1.0)
        splat(trail, px, py, mix(C['geology'], C['biochem'], u)[seg], 0.14)
        P = Q
        life_ -= 1
        dead = (life_ < 0) | (P[:, 0] < 0) | (P[:, 0] >= W) | (P[:, 1] < 0) | (P[:, 1] >= H)
        P[dead] = rng.random((dead.sum(), 2)) * [W, H]; life_[dead] = 60 + rng.random(dead.sum()) * 60
        if fi >= 0:
            yield finish(trail, exposure=1.2, bloom=0.8), ('Arte generativo', 'Campos de flujo', 'geology')


def clip_thomas(dur):
    rng = np.random.default_rng(13)
    b = 0.19
    P = rng.uniform(-4, 4, (40000, 3))

    def f(P):
        x, y, z = P.T
        return np.c_[np.sin(y) - b * x, np.sin(z) - b * y, np.sin(x) - b * z]
    for _ in range(300):
        P = P + 0.05 * f(P)
    trail = np.zeros((H, W, 3), np.float32)

    def proj(P, ang):
        x, y, z = P.T
        xr = x * math.cos(ang) - y * math.sin(ang); yr = x * math.sin(ang) + y * math.cos(ang)
        return W / 2 + xr * 95, H / 2 - 20 - (z * 0.9 - yr * 0.3) * 95, yr
    for fi in range(-10, int(dur * FPS)):
        t = max(fi, 0) / FPS
        trail *= 0.8
        ang = 0.5 + fi * 0.012
        for _ in range(2):
            x0, y0, _ = proj(P, ang)
            P = P + 0.06 * f(P)
            x1, y1, dz = proj(P, ang)
            px, py, seg, _ = seg_points(x0, y0, x1, y1, 1.0)
            u = np.clip((dz[seg] + 4) / 8, 0, 1)
            splat(trail, px, py, mix(C['astro'], np.array([0.7, 0.85, 1.0]), u), 0.04)
        if fi >= 0:
            yield finish(trail, exposure=1.1, bloom=0.9), ('Matemáticas · Caos', 'Atractor de Thomas', 'astro')


def clip_waves(dur):
    h, w = H // 2, W // 2
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        sep = 90 + 40 * t
        acc = 0
        for sx in (w / 2 - sep, w / 2 + sep):
            r = np.hypot(xx - sx, yy - h / 2) + 1
            acc = acc + np.sin(r * 0.19 - t * 9) / np.sqrt(r)
        I = (acc ** 2) * 6
        big = np.asarray(Image.fromarray(I.astype(np.float32)).resize((W, H), Image.BICUBIC))
        buf = (big[..., None] * mix(C['computation'], np.array([0.7, 0.9, 1.0]), np.clip(big / 3, 0, 1))).astype(np.float32)
        yield finish(buf * 0.7, exposure=1.0, bloom=0.6), ('Física · Ondas', 'Interferencia', 'computation')


def scene_montage2():
    clip = BAR / 2
    clips = [scene_grayscott, scene_mandel, clip_clifford, clip_harmonograph,
             clip_julia, clip_flow, clip_thomas, clip_waves]
    for gen_fn in clips:
        nF = int(round(clip * FPS))
        for k, (img, (lab, title, ck)) in enumerate(gen_fn(clip)):
            if k >= nF:
                break
            t = k / FPS
            fl = max(0.0, 1 - t / 0.18) * 0.25
            img = np.clip(img.astype(np.float32) * (1 + fl * 2) + fl * 60, 0, 255).astype(np.uint8)
            yield caption_fast(img, t, lab, title, ck)


# ================================================================ MOTOR WASM: fluidos
def scene_fluid():
    dur = 3 * BAR
    h, w = 135, 240
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float64)
    u = np.zeros((h, w)); v = np.zeros((h, w)); dye = np.zeros((h, w, 3))
    cols = [C[k] for _, k in DISCIPLINES]
    rng = np.random.default_rng(17)

    def adv(f, u, v):
        return map_coordinates(f, [yy - v, xx - u], order=1, mode='grid-wrap')

    def rollx(a, s): return np.roll(a, s, 1)
    def rolly(a, s): return np.roll(a, s, 0)

    def project(u, v):
        div = 0.5 * (rollx(u, -1) - rollx(u, 1) + rolly(v, -1) - rolly(v, 1))
        p = np.zeros_like(u)
        for _ in range(30):
            p = (rollx(p, 1) + rollx(p, -1) + rolly(p, 1) + rolly(p, -1) - div) / 4
        return u - 0.5 * (rollx(p, -1) - rollx(p, 1)), v - 0.5 * (rolly(p, -1) - rolly(p, 1))

    def inject(cx, cy, dx, dy, col, rad, force, amount):
        nonlocal u, v, dye
        g = np.exp(-((xx - cx) ** 2 + (yy - cy) ** 2) / rad ** 2)
        u += g * dx * force; v += g * dy * force
        dye += g[..., None] * col * amount

    step_i = 0

    def step(tsim, beat_hit, burst_col):
        nonlocal u, v, dye, step_i
        step_i += 1
        for j in range(3):
            a = tsim * 0.5 + j * 2 * np.pi / 3
            cx, cy = w / 2 + math.cos(a) * 45, h / 2 + math.sin(a) * 30
            ta = a + np.pi / 2 + 0.6
            ci = (j * 3 + int(tsim / BAR)) % len(cols)
            inject(cx, cy, math.cos(ta), math.sin(ta), cols[ci], 4.0, 0.35, 0.10)
        if beat_hit:
            cx, cy = rng.uniform(40, w - 40), rng.uniform(25, h - 25)
            a = rng.uniform(0, 2 * np.pi)
            inject(cx, cy, math.cos(a), math.sin(a), burst_col, 7.0, 3.0, 0.9)
        # confinamiento de vorticidad
        curl = 0.5 * (rollx(v, -1) - rollx(v, 1)) - 0.5 * (rolly(u, -1) - rolly(u, 1))
        ac = np.abs(curl)
        gx = 0.5 * (rollx(ac, -1) - rollx(ac, 1)); gy = 0.5 * (rolly(ac, -1) - rolly(ac, 1))
        nm = np.hypot(gx, gy) + 1e-6
        u += 0.25 * (gy / nm) * curl; v += 0.25 * (-gx / nm) * curl
        u, v = project(u, v)
        u2, v2 = adv(u, u, v), adv(v, u, v)
        u, v = u2 * 0.999, v2 * 0.999
        u, v = project(u, v)
        dye = np.stack([adv(dye[..., c], u, v) for c in range(3)], -1) * 0.993

    # arranque previo
    for i in range(40):
        step(i / FPS, i % 20 == 0, cols[i % len(cols)])
    beats_done = -1
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        bi = int(t / BEAT)
        hit = bi != beats_done and bi % 2 == 0
        beats_done = bi
        step(t + 2, hit, cols[(bi // 2) % len(cols)])
        big = np.stack([np.asarray(Image.fromarray(dye[..., c].astype(np.float32)).resize((W, H), Image.BICUBIC)) for c in range(3)], -1)
        img = finish(np.clip(big, 0, None) * 0.9, exposure=1.0, bloom=0.6, fade=fio(t, dur))
        yield caption(img, t, dur, 'EigenLab Core · C++ → WebAssembly', 'Fluidos en tiempo real, en el navegador', 'computation')


# ================================================================ PLATONIC LAB: teseracto
def scene_tesseract():
    dur = 2 * BAR
    V = np.array([[x, y, z, w_] for x in (-1, 1) for y in (-1, 1) for z in (-1, 1) for w_ in (-1, 1)], float)
    E = np.array([(i, j) for i in range(16) for j in range(i + 1, 16) if np.sum(V[i] != V[j]) == 1])
    trail = np.zeros((H, W, 3), np.float32)
    warm, cool = C['math'], C['astro'] * 1.2 + 0.1

    def rot(a, i, j):
        M = np.eye(4); c, s = math.cos(a), math.sin(a)
        M[i, i] = c; M[j, j] = c; M[i, j] = -s; M[j, i] = s
        return M
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        M = rot(0.55 * t + 0.3, 0, 3) @ rot(0.33 * t, 1, 2) @ rot(0.25 * t + 0.5, 2, 3)
        P = V @ M.T
        s4 = 1 / (2.6 - P[:, 3])
        P3 = P[:, :3] * s4[:, None]
        ang = 0.4 + 0.18 * t
        x = P3[:, 0] * math.cos(ang) - P3[:, 2] * math.sin(ang)
        z = P3[:, 0] * math.sin(ang) + P3[:, 2] * math.cos(ang)
        y = P3[:, 1]
        s3 = 1 / (3.2 - z)
        sx = W / 2 + x * s3 * 1150; sy = H / 2 - 20 + y * s3 * 1150
        trail *= 0.72
        px, py, seg, uu = seg_points(sx[E[:, 0]], sy[E[:, 0]], sx[E[:, 1]], sy[E[:, 1]], 0.8)
        wv = P[E[seg, 0], 3] * (1 - uu) + P[E[seg, 1], 3] * uu
        col = mix(cool, warm, np.clip((wv + 1.4) / 2.8, 0, 1))
        for o in (-0.8, 0, 0.8):
            splat(trail, px + o * 0.5, py + o, col, 0.09)
        buf = trail.copy()
        splat_glow(buf, sx, sy, mix(cool, warm, np.clip((P[:, 3] + 1.4) / 2.8, 0, 1)) * 0.6 + 0.4, 0.9, r=5, s=5)
        img = finish(buf, exposure=1.2, bloom=1.1, fade=fio(t, dur))
        yield caption(img, t, dur, 'Platonic Lab · Geometría en 4D', 'Un teseracto: el cubo en cuatro dimensiones', 'math')


# ================================================================ PAUSA: cita del manifiesto
def scene_quote():
    dur = 3 * BAR
    rng = np.random.default_rng(19)
    T = lorenz_traj(20000)[500:]
    P = T[rng.integers(0, len(T), 30000)] + rng.normal(0, 0.3, (30000, 3))
    trail = np.zeros((H, W, 3), np.float32)
    s, r, b = 10, 28, 8 / 3
    f_q = font(FONT_SERIF, 58, 'Regular')
    f_src = font(FONT_SANS, 20, 'Medium')
    l1 = 'Si un sistema puede describirse matemáticamente,'
    l2 = 'su estado puede hacerse visible.'
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        trail *= 0.85
        x0, y0, _ = lorenz_project(P, 0.9 + 0.05 * t, scale=19, cx=W / 2, cy=H / 2 + 10)
        for _ in range(2):
            x, y, z = P.T
            P = P + 0.0035 * np.c_[s * (y - x), x * (r - z) - y, x * y - b * z]
        x1, y1, _ = lorenz_project(P, 0.9 + 0.05 * t, scale=19, cx=W / 2, cy=H / 2 + 10)
        px, py, seg, _ = seg_points(x0, y0, x1, y1, 1.2)
        splat(trail, px, py, np.array([1.0, 0.62, 0.3]), 0.025)
        img = finish(trail, exposure=1.0, bloom=0.8, fade=fio(t, dur, 0.6, 0.5) * 0.75)

        def fn(d, layer):
            a1 = ease((t - 0.6) / 1.0) * (1 - ease((t - dur + 0.7) / 0.6))
            a2 = ease((t - 2.2) / 1.0) * (1 - ease((t - dur + 0.7) / 0.6))
            a3 = ease((t - 3.6) / 0.8) * (1 - ease((t - dur + 0.7) / 0.6))
            d.text((W / 2, H / 2 - 20), l1, font=f_q, fill=rgba(C['muted'] * 0.4 + 0.6, a1), anchor='ms')
            d.text((W / 2, H / 2 + 70), l2, font=f_q, fill=rgba(C['white'], a2), anchor='ms')
            spaced_text(d, (W / 2, H / 2 + 150), '— MANIFIESTO EIGENLAB', f_src, rgba(C['math'], a3 * 0.8), 5, anchor='m')
        yield overlay(img, fn)


# ================================================================ CLÍMAX: grafo + caminos
def scene_graph2():
    dur = 4 * BAR
    nodes, cl, edges = graph_data()
    K = len(DISCIPLINES)
    ncol = np.array([C[DISCIPLINES[c][1]] for c in cl])
    rng = np.random.default_rng(2)
    order = np.argsort(cl * 10 + rng.random(len(cl)))
    appear = np.empty(len(nodes)); appear[order] = np.linspace(0.1, 2.8, len(nodes))
    inter = cl[edges[:, 0]] != cl[edges[:, 1]]
    e_start = np.maximum(appear[edges[:, 0]], appear[edges[:, 1]]) + np.where(inter, 0.6 + rng.random(len(edges)) * 1.6, 0.1)
    f_lab = font(FONT_SANS, 21, 'Medium')
    f_big = font(FONT_SERIF, 60, 'Regular')
    f_small = font(FONT_SANS, 20, 'Medium')
    f_path = font(FONT_SERIF, 54, 'Regular')
    stats = [('16', 'laboratorios'), ('~200', 'simulaciones'), ('132', 'guías teóricas'), ('12', 'caminos')]
    # caminos de aprendizaje (índices de DISCIPLINES)
    paths = [('Caos y atractores', [5, 0, 3, 6]),
             ('Emergencia y autoorganización', [3, 7, 8, 2]),
             ('Oscilaciones y ondas', [0, 9, 1, 6])]
    path_nodes = []
    for _, cls in paths:
        ids = []
        for c in cls:
            cand = np.where(cl == c)[0]
            ctr = nodes[cand].mean(0)
            ids.append(cand[np.argmin(np.hypot(*(nodes[cand] - ctr).T))])
        path_nodes.append(ids)
    p_start = [4 * BAR / 2 + i * BAR / 2 for i in range(3)]      # 5.33, 6.67, 8.0 s
    p_len = BAR / 2
    CX, CY = W / 2, H / 2 - 10
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        rot = 0.06 * t
        implode = ease((t - (dur - 0.55)) / 0.5) ** 2
        Rm = np.array([[math.cos(rot), -math.sin(rot)], [math.sin(rot), math.cos(rot)]])
        Pn = nodes @ Rm.T * (1 + 0.02 * t) * (1 - implode)
        Pn = Pn + np.stack([np.sin(t * 1.3 + np.arange(len(Pn))), np.cos(t * 1.1 + np.arange(len(Pn)) * 1.7)], 1) * 3
        X = CX + Pn[:, 0]; Y = CY + Pn[:, 1]
        active = [i for i in range(3) if p_start[i] <= t < p_start[i] + p_len + 0.25]
        dim = 0.45 if active else 1.0
        buf = np.zeros((H, W, 3), np.float32)
        prog = np.clip((t - e_start) / 0.45, 0, 1)
        vis = prog > 0
        if vis.any():
            e = edges[vis]; pr = prog[vis]
            x0, y0 = X[e[:, 0]], Y[e[:, 0]]
            x1 = x0 + (X[e[:, 1]] - x0) * pr; y1 = y0 + (Y[e[:, 1]] - y0) * pr
            px, py, seg, u = seg_points(x0, y0, x1, y1, 1.0)
            col = mix(ncol[e[:, 0]][seg], ncol[e[:, 1]][seg], u)
            splat(buf, px, py, col, np.where(inter[vis][seg], 0.07, 0.12) * dim)
            ie = np.where(inter[vis] & (pr >= 1))[0]
            if len(ie):
                ph = (t * 0.9 + ie * 0.37) % 1
                qx = X[e[ie, 0]] + (X[e[ie, 1]] - X[e[ie, 0]]) * ph
                qy = Y[e[ie, 0]] + (Y[e[ie, 1]] - Y[e[ie, 0]]) * ph
                splat_glow(buf, qx, qy, np.array([1, 1, 1]), 0.25 * dim, r=3, s=2)
        pop = np.clip((t - appear) / 0.25, 0, 1)
        sz = np.where(pop < 1, 1 + 0.8 * np.sin(pop * np.pi), 1) * (pop > 0)
        on = np.where(sz > 0)[0]
        splat_glow(buf, X[on], Y[on], ncol[on], 0.9 * dim, r=6, s=5, size=sz[on])
        # camino activo: trazo brillante que recorre las disciplinas
        for i in active:
            ids = path_nodes[i]
            pp = ease_out((t - p_start[i]) / (p_len * 0.8))
            fade_p = 1 - ease((t - p_start[i] - p_len) / 0.25)
            xs, ys = X[ids], Y[ids]
            px, py, pos = polyline_points(xs, ys, 0.8)
            m = pos <= pp * (len(ids) - 1)
            seg_c = mix(ncol[ids][np.minimum(pos.astype(int), len(ids) - 2)], ncol[ids][np.minimum(pos.astype(int) + 1, len(ids) - 1)], pos % 1)
            splat(buf, px[m], py[m], seg_c[m] * 0.5 + 0.5, 0.5 * fade_p)
            reached = np.arange(len(ids)) <= pp * (len(ids) - 1) + 1e-6
            splat_glow(buf, xs[reached], ys[reached], ncol[ids][reached] * 0.5 + 0.5, 2.5 * fade_p, r=8, s=10)
            if m.any():
                hx, hy = px[m][-1], py[m][-1]
                splat_glow(buf, [hx], [hy], np.array([1, 1, 1]), 3.0 * fade_p, r=8, s=8)
        fl = ease((t - (dur - 0.12)) / 0.12) * 0.9
        img = finish(buf, exposure=1.6, bloom=1.3, fade=ease(t / 0.25), flash=fl)

        def fn(d, layer):
            la = ease((t - 1.2) / 0.6) * (1 - ease((t - (dur - 0.7)) / 0.3))
            for ci, (name, ck) in enumerate(DISCIPLINES):
                a = -np.pi / 2 + ci * 2 * np.pi / K + rot
                s_ = 1 + 0.02 * t
                lx, ly = CX + math.cos(a) * 570 * s_, CY + math.sin(a) * 345 * s_
                hl = any(ci in paths[i][1] for i in active)
                d.text((lx, ly + 7), name.upper(), font=f_lab,
                       fill=rgba(C[ck] * 0.6 + 0.4, la * (1.0 if hl or not active else 0.45)), anchor='ms')
            for i, (num, lab) in enumerate(stats):
                a = ease((t - (0.8 + i * 2 * BEAT)) / 0.4) * (1 - ease((t - (dur - 0.6)) / 0.3))
                if a <= 0:
                    continue
                x = W / 2 + (i - 1.5) * 280
                d.text((x, 985), num, font=f_big, fill=rgba(C['white'], a), anchor='ms')
                spaced_text(d, (x, 1022), lab.upper(), f_small, rgba(C['muted'], a), 4, anchor='m')
            for i in active:
                a = ease((t - p_start[i]) / 0.2) * (1 - ease((t - p_start[i] - p_len) / 0.25))
                spaced_text(d, (W / 2, 52), 'CAMINO DE APRENDIZAJE', f_small, rgba(C['muted'], a), 5, anchor='m')
                d.text((W / 2, 112), paths[i][0], font=f_path, fill=rgba(C['white'], a), anchor='ms')
        yield overlay(img, fn)


def scene_logo2():
    yield from scene_logo(4 * BAR)


SCENES = [  # (nombre, función, compases)
    ('a_intro', scene_intro_lorenz, 4),
    ('b_pend', scene_pendulum, 2),
    ('c_chladni', scene_chladni, 2),
    ('d_orbitals', scene_orbitals, 2),
    ('e_dna', scene_dna, 2),
    ('f_boids', scene_boids, 2),
    ('g_erosion', scene_erosion, 2),
    ('h_galaxy', wrap_meta(lambda d: scene_galaxy(d, wind=0.1), 'Astronomía · Dinámica galáctica'), 2),
    ('i_life', scene_life, 2),
    ('j_neural', wrap_meta(scene_neural, 'Inteligencia artificial · Aprendizaje'), 2),
    ('k_phyllo', scene_phyllo, 2),
    ('l_montage', scene_montage2, 4),
    ('m_fluid', scene_fluid, 3),
    ('n_tesseract', scene_tesseract, 2),
    ('o_quote', scene_quote, 3),
    ('p_graph', scene_graph2, 4),
    ('q_logo', scene_logo2, 4),
]
assert sum(nb for _, _, nb in SCENES) == NBARS


def span(name):
    b0 = 0
    for nm, fn, nb in SCENES:
        if nm == name:
            return fn, int(round(b0 * BAR * FPS)), int(round((b0 + nb) * BAR * FPS))
        b0 += nb


def render(name):
    fn, f0, f1 = span(name)
    need = f1 - f0
    out = os.path.join(HERE, 'seg2', f'{name}.mov')
    os.makedirs(os.path.dirname(out), exist_ok=True)
    p = subprocess.Popen(['ffmpeg', '-y', '-loglevel', 'error', '-f', 'rawvideo', '-pix_fmt', 'rgb24',
                          '-s', f'{W}x{H}', '-r', str(FPS), '-i', '-', '-c:v', 'libx264', '-preset', 'medium',
                          '-crf', '10', '-pix_fmt', 'yuv420p', out], stdin=subprocess.PIPE)
    k, last = 0, None
    for img in fn():
        if k >= need:
            break
        p.stdin.write(np.ascontiguousarray(img).tobytes()); last = img; k += 1
        if k % 30 == 0:
            print(name, k, '/', need, flush=True)
    while k < need:
        p.stdin.write(np.ascontiguousarray(last).tobytes()); k += 1
    p.stdin.close(); p.wait()
    print('done', name, need)


if __name__ == '__main__':
    if sys.argv[1] == 'still':
        name, fidx = sys.argv[2], int(sys.argv[3])
        fn = span(name)[0]
        for k, img in enumerate(fn()):
            if k == fidx:
                Image.fromarray(img).save(os.path.join(HERE, f'still_{name}_{fidx}.png')); break
    elif sys.argv[1] == 'list':
        print(' '.join(nm for nm, _, _ in SCENES))
    else:
        render(sys.argv[1])
