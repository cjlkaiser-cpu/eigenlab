"""EigenLab promo — vídeo procedural 1920x1080 @30fps, 40 s.
Cada escena es una simulación real renderizada con splatting aditivo + bloom.
Uso: python3.11 video.py <escena|all>
"""
import sys, os, subprocess, math
import numpy as np
from PIL import Image, ImageDraw, ImageFont, ImageFilter
from scipy.ndimage import gaussian_filter

W, H, FPS = 1920, 1080, 30
BPM = 90
BAR = 4 * 60 / BPM                    # 2.6667 s
HERE = os.path.dirname(os.path.abspath(__file__))
FONT_SERIF = os.path.join(HERE, 'fonts/Playfair.ttf')
FONT_SANS = os.path.join(HERE, 'fonts/Inter.ttf')

BG = np.array([3, 7, 18]) / 255.0


def hexc(h):
    h = h.lstrip('#')
    return np.array([int(h[i:i + 2], 16) for i in (0, 2, 4)]) / 255.0


C = {k: hexc(v) for k, v in dict(
    physics='#22c55e', sound='#a855f7', chemistry='#06b6d4', biochem='#ec4899',
    math='#f97316', astro='#6366f1', biology='#10b981', geology='#d97706',
    computation='#3b82f6', ai='#ef4444', music='#ec4899', white='#f8fafc',
    muted='#94a3b8').items()}

DISCIPLINES = [('Física', 'physics'), ('Química', 'chemistry'), ('Bioquímica', 'biochem'),
               ('Biología', 'biology'), ('Geología', 'geology'), ('Matemáticas', 'math'),
               ('Astronomía', 'astro'), ('Computación', 'computation'), ('IA', 'ai'),
               ('Música', 'sound')]


def font(path, size, var=None):
    f = ImageFont.truetype(path, size)
    if var:
        f.set_variation_by_name(var)
    return f


def ease(x):
    x = min(max(x, 0.0), 1.0)
    return x * x * (3 - 2 * x)


def ease_out(x):
    x = min(max(x, 0.0), 1.0)
    return 1 - (1 - x) ** 3


# ------------------------------------------------------------ render core
def splat(buf, x, y, col, w=1.0):
    """Splat bilineal antialiasado. col: (3,) o (n,3); w: escalar o (n,)."""
    x = np.asarray(x, float).ravel(); y = np.asarray(y, float).ravel()
    w = np.broadcast_to(np.asarray(w, float), x.shape)
    ok = (x >= 0) & (x < W - 1) & (y >= 0) & (y < H - 1)
    x, y, w = x[ok], y[ok], w[ok]
    x0 = x.astype(int); y0 = y.astype(int)
    fx = x - x0; fy = y - y0
    idx = np.concatenate([y0 * W + x0, y0 * W + x0 + 1, (y0 + 1) * W + x0, (y0 + 1) * W + x0 + 1])
    ww = np.concatenate([(1 - fx) * (1 - fy), fx * (1 - fy), (1 - fx) * fy, fx * fy]) * np.tile(w, 4)
    flat = buf.reshape(-1, 3)
    col = np.asarray(col, float)
    if col.ndim == 1:
        m = np.bincount(idx, ww, minlength=W * H)
        flat += m[:, None] * col[None, :]
    else:
        col = col[ok]
        cc = np.tile(col, (4, 1))
        for c in range(3):
            flat[:, c] += np.bincount(idx, ww * cc[:, c], minlength=W * H)


def seg_points(x0, y0, x1, y1, spacing=0.8):
    """Puntos densos a lo largo de segmentos (arrays) para dibujar líneas vía splat."""
    x0, y0, x1, y1 = map(np.atleast_1d, (x0, y0, x1, y1))
    L = np.hypot(x1 - x0, y1 - y0)
    n = np.maximum(2, (L / spacing).astype(int))
    tot = n.sum()
    seg = np.repeat(np.arange(len(n)), n)
    start = np.repeat(np.cumsum(n) - n, n)
    u = (np.arange(tot) - start) / np.repeat(n - 1, n)
    return x0[seg] + (x1 - x0)[seg] * u, y0[seg] + (y1 - y0)[seg] * u, seg, u


def polyline_points(xs, ys, spacing=0.8):
    px, py, seg, u = seg_points(xs[:-1], ys[:-1], xs[1:], ys[1:], spacing)
    return px, py, seg + u  # posición paramétrica a lo largo de la curva


YY, XX = np.mgrid[0:H, 0:W]
_r = np.hypot((XX - W / 2) / (W / 2), (YY - H / 2) / (H / 2))
VIGNETTE = (1 - 0.45 * np.clip(_r - 0.35, 0, 1) ** 1.5)[..., None].astype(np.float32)
BGIMG = (BG[None, None, :] * (1.0 + 0.9 * np.exp(-_r ** 2 * 1.6))[..., None]).astype(np.float32)
del YY, XX, _r


def finish(buf, exposure=1.0, bloom=0.9, fade=1.0, flash=0.0):
    """Tone-map + bloom + viñeta -> uint8 RGB."""
    x = np.asarray(buf, np.float32) * exposure
    small = x.reshape(H // 4, 4, W // 4, 4, 3).mean((1, 3))
    b1 = gaussian_filter(small, (3, 3, 0))
    b2 = gaussian_filter(small, (12, 12, 0))
    b = np.clip(0.6 * b1 + 0.8 * b2, 0, 50)
    bi = np.stack([np.asarray(Image.fromarray(b[..., c]).resize((W, H), Image.BILINEAR)) for c in range(3)], -1)
    x = x + bloom * bi
    img = 1 - np.exp(-x)
    img = BGIMG + (1 - BGIMG) * img
    img = img * VIGNETTE
    img = img * fade + flash
    return (np.clip(img, 0, 1) ** (1 / 1.0) * 255).astype(np.uint8)


# ------------------------------------------------------------ texto
def spaced_text(draw, xy, text, fnt, fill, spacing=0, anchor='l'):
    """Texto con tracking. anchor: 'l' izquierda, 'm' centrado."""
    widths = [draw.textlength(ch, font=fnt) for ch in text]
    total = sum(widths) + spacing * (len(text) - 1)
    x, y = xy
    if anchor == 'm':
        x -= total / 2
    for ch, w in zip(text, widths):
        draw.text((x, y), ch, font=fnt, fill=fill, anchor='ls')
        x += w + spacing
    return total


def overlay(img8, fn):
    """fn(draw) dibuja sobre una capa RGBA que se compone encima."""
    im = Image.fromarray(img8)
    layer = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    fn(ImageDraw.Draw(layer), layer)
    im = Image.alpha_composite(im.convert('RGBA'), layer).convert('RGB')
    return np.asarray(im)


def rgba(col, a):
    col = np.clip(np.asarray(col), 0, 1)
    return tuple(int(v * 255) for v in col) + (int(np.clip(a, 0, 1) * 255),)


F_CAP_LABEL = font(FONT_SANS, 22, 'SemiBold')
F_CAP_TITLE = font(FONT_SERIF, 52, 'Regular')


_yy, _xx = np.mgrid[0:H, 0:W]
CAP_SHADE = (0.6 * np.exp(-(((_xx - 330) / 520) ** 2 + ((_yy - 930) / 170) ** 2)))[..., None].astype(np.float32)
del _yy, _xx


def shade(img8, a):
    return (img8 * (1 - CAP_SHADE * a)).astype(np.uint8)


def caption(img8, tl, dur, label, title, colkey, x0=110, y0=960):
    a = ease((tl - 0.35) / 0.5) * (1 - ease((tl - (dur - 0.45)) / 0.35))
    if a <= 0:
        return img8
    img8 = shade(img8, a)
    slide = (1 - ease_out((tl - 0.35) / 0.7)) * 24

    def fn(d, layer):
        col = C[colkey]
        d.rectangle([x0, y0 - 88 + slide, x0 + 3, y0 - 8 + slide], fill=rgba(col, a))
        spaced_text(d, (x0 + 22, y0 - 62 + slide), label.upper(), F_CAP_LABEL, rgba(col, a), 5)
        spaced_text(d, (x0 + 20, y0 - 12 + slide), title, F_CAP_TITLE, rgba(C['white'], a * 0.95), 0.5)
    return overlay(img8, fn)


# ============================================================ ESCENAS
# Cada escena: generador que devuelve frames uint8 (duración en compases)

def lorenz_traj(n, dt=0.005, s=10, r=28, b=8 / 3):
    p = np.array([0.1, 0.0, 0.0])
    out = np.empty((n, 3))

    def f(p):
        x, y, z = p
        return np.array([s * (y - x), x * (r - z) - y, x * y - b * z])
    for i in range(n):
        k1 = f(p); k2 = f(p + dt / 2 * k1); k3 = f(p + dt / 2 * k2); k4 = f(p + dt * k3)
        p = p + dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4)
        out[i] = p
    return out


def lorenz_project(P, ang, tilt=0.35, scale=21.0, cx=W / 2 + 60, cy=H / 2 + 20):
    x, y, z = P[:, 0], P[:, 1], P[:, 2] - 25
    ca, sa = math.cos(ang), math.sin(ang)
    X = ca * x - sa * y
    Y = sa * x + ca * y
    ct, st = math.cos(tilt), math.sin(tilt)
    Zs = ct * z - st * Y
    D = st * z + ct * Y
    persp = 1 / (1 + D * 0.006)
    return cx + X * scale * persp, cy - Zs * scale * persp, D


def scene_intro_lorenz():
    """0–10.67 s: manifiesto + ecuaciones -> el atractor de Lorenz se dibuja."""
    dur = 4 * BAR
    N = 16000
    P = lorenz_traj(N)[300:]
    nP = len(P)
    f_man = font(FONT_SERIF, 64, 'Regular')
    f_eq = font(FONT_SERIF, 46, 'Regular')
    f_eq_i = ImageFont.truetype('/System/Library/Fonts/Supplemental/STIXTwoText-Italic.ttf', 50)
    lines = ['Las ecuaciones no describen objetos.', 'Describen comportamientos.']
    eqs = ['dx/dt = σ (y − x)', 'dy/dt = x (ρ − z) − y', 'dz/dt = x y − β z']
    t_draw0 = 2 * BAR - 0.3
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        buf = np.zeros((H, W, 3), np.float32)
        # --- Lorenz ---
        prog = ease_out((t - t_draw0) / (dur - t_draw0 - 0.3)) if t > t_draw0 else 0
        ang = 0.6 + 0.16 * t
        if prog > 0:
            n = max(3, int(prog * (nP - 1)))
            sx, sy, D = lorenz_project(P[:n + 1], ang)
            px, py, s = polyline_points(sx, sy, 0.9)
            rel = s / n
            depth = np.interp(s, np.arange(len(D)), D)
            bright = (0.10 + 0.25 * rel ** 3) * (1.1 - 0.012 * depth)
            col = np.outer(1 - rel, C['math'] * 0.8) + np.outer(rel, np.array([1.0, 0.78, 0.45]))
            splat(buf, px, py, col, bright)
            # cabeza
            hx, hy = sx[-1], sy[-1]
            gx, gy = np.meshgrid(np.arange(-6, 7), np.arange(-6, 7))
            g = np.exp(-(gx ** 2 + gy ** 2) / 6.0).ravel()
            splat(buf, hx + gx.ravel(), hy + gy.ravel(), np.array([1, 0.9, 0.7]), g * 1.6)
        else:
            # punto semilla latiendo en el centro
            a = ease((t - 0.2) / 1.0)
            sx, sy, _ = lorenz_project(P[:1], ang)
            gx, gy = np.meshgrid(np.arange(-8, 9), np.arange(-8, 9))
            g = np.exp(-(gx ** 2 + gy ** 2) / 8.0).ravel()
            pulse = 1 + 0.25 * math.sin(t * 2 * math.pi * BPM / 60)
            splat(buf, sx[0] + gx.ravel(), sy[0] + gy.ravel(), np.array([1, 0.75, 0.45]), g * a * 1.4 * pulse)
        img = finish(buf, exposure=1.3, bloom=1.1, fade=ease(t / 0.8))

        def fn(d, layer):
            # manifiesto
            a1 = ease((t - 0.4) / 0.8) * (1 - ease((t - 4.4) / 0.6))
            a2 = ease((t - 1.9) / 0.8) * (1 - ease((t - 4.6) / 0.6))
            if a1 > 0:
                d.text((W / 2, 470), lines[0], font=f_man, fill=rgba(C['muted'], a1), anchor='ms')
            if a2 > 0:
                d.text((W / 2, 570), lines[1], font=f_man, fill=rgba(C['white'], a2), anchor='ms')
            # ecuaciones: aparecen y se desvanecen cuando la curva toma el relevo
            for i, e in enumerate(eqs):
                ae = ease((t - (4.9 + 0.25 * i)) / 0.5) * (1 - ease((t - 7.4) / 1.0))
                if ae > 0:
                    d.text((150, 420 + 72 * i), e, font=f_eq_i, fill=rgba(C['math'] * 0.5 + 0.5 * C['white'], ae * 0.9), anchor='ls')
        img = overlay(img, fn)
        img = caption(img, t - 2 * BAR, 2 * BAR, 'Matemáticas · Caos', 'Atractor de Lorenz', 'math')
        yield img


def scene_pendulum():
    """Péndulos dobles casi idénticos que divergen."""
    dur = 2 * BAR
    n = 36
    g, l = 9.81, 1.0
    th = np.stack([np.full(n, 2.35) + np.arange(n) * 1e-5, np.full(n, 2.6), np.zeros(n), np.zeros(n)], 1)

    def deriv(s):
        t1, t2, w1, w2 = s.T
        d = t1 - t2
        den = 3 - np.cos(2 * d)
        a1 = (-g * 3 * np.sin(t1) - g * np.sin(t1 - 2 * t2) - 2 * np.sin(d) * (w2 ** 2 * l + w1 ** 2 * l * np.cos(d))) / (l * den)
        a2 = (2 * np.sin(d) * (w1 ** 2 * l * 2 + g * 2 * np.cos(t1) + w2 ** 2 * l * np.cos(d))) / (l * den)
        return np.stack([w1, w2, a1, a2], 1)

    # pre-avance: que ya estén en movimiento y a punto de divergir
    dt = 0.004
    for _ in range(int(0.9 / dt)):
        k1 = deriv(th); k2 = deriv(th + dt / 2 * k1); k3 = deriv(th + dt / 2 * k2); k4 = deriv(th + dt * k3)
        th = th + dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4)
    cx, cy, Lp = W / 2, 400, 210
    cols = np.array([C['physics'] * (1 - u) + np.array([0.55, 1.0, 0.85]) * u for u in np.linspace(0, 1, n)])
    trail = np.zeros((H, W, 3), np.float32)
    sim_per_frame = 1.75 / FPS
    sub = int(sim_per_frame / dt)
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        trail *= 0.93
        xs, ys = [], []
        for _ in range(sub):
            k1 = deriv(th); k2 = deriv(th + dt / 2 * k1); k3 = deriv(th + dt / 2 * k2); k4 = deriv(th + dt * k3)
            th = th + dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4)
            x2 = cx + Lp * (np.sin(th[:, 0]) + np.sin(th[:, 1]))
            y2 = cy + Lp * (np.cos(th[:, 0]) + np.cos(th[:, 1]))
            xs.append(x2); ys.append(y2)
        xs = np.array(xs); ys = np.array(ys)  # (sub, n)
        # trazos: unir sub-pasos consecutivos
        px, py, seg, u = seg_points(xs[:-1].ravel(), ys[:-1].ravel(), xs[1:].ravel(), ys[1:].ravel(), 0.9)
        pend = np.tile(np.arange(n), sub - 1)[seg]
        splat(trail, px, py, cols[pend], 0.16)
        buf = trail.copy()
        x1 = cx + Lp * np.sin(th[:, 0]); y1 = cy + Lp * np.cos(th[:, 0])
        x2 = x1 + Lp * np.sin(th[:, 1]); y2 = y1 + Lp * np.cos(th[:, 1])
        for (ax, ay, bx, by) in ((np.full(n, cx), np.full(n, cy), x1, y1), (x1, y1, x2, y2)):
            px, py, seg, _ = seg_points(ax, ay, bx, by, 0.9)
            splat(buf, px, py, cols[seg], 0.05)
        gx, gy = np.meshgrid(np.arange(-3, 4), np.arange(-3, 4))
        gk = np.exp(-(gx ** 2 + gy ** 2) / 2.5).ravel()
        for X, Y, w in ((x1, y1, 0.08), (x2, y2, 0.25)):
            splat(buf, (X[:, None] + gx.ravel()).ravel(), (Y[:, None] + gy.ravel()).ravel(),
                  np.repeat(cols, len(gk), 0), np.tile(gk, n) * w)
        splat(buf, [cx], [cy], np.array([1, 1, 1]), 3.0)
        img = finish(buf, exposure=1.2, bloom=1.0, fade=ease(t / 0.25) * (1 - ease((t - dur + 0.2) / 0.2)))
        img = caption(img, t, dur, 'Física · Sensibilidad inicial', '36 péndulos dobles, casi idénticos', 'physics')
        yield img


def scene_boids():
    dur = 2 * BAR
    rng = np.random.default_rng(3)
    n = 900
    pos = rng.random((n, 2)) * [W, H]
    ang = rng.random(n) * 2 * np.pi
    vel = np.stack([np.cos(ang), np.sin(ang)], 1) * 4
    hue = rng.random(n)
    cols = np.outer(1 - hue, C['biology']) + np.outer(hue, np.array([0.45, 0.95, 0.9]))
    trail = np.zeros((H, W, 3), np.float32)

    def step(pos, vel, t):
        d = pos[None, :, :] - pos[:, None, :]
        d[..., 0] = (d[..., 0] + W / 2) % W - W / 2
        d[..., 1] = (d[..., 1] + H / 2) % H - H / 2
        dist = np.hypot(d[..., 0], d[..., 1]) + np.eye(n) * 1e6
        near = dist < 70
        close = dist < 20
        cnt = near.sum(1, keepdims=True) + 1e-9
        coh = (d * near[..., None]).sum(1) / cnt
        ali = (near[..., None] * vel[None]).sum(1) / cnt - vel
        sep = -(d * close[..., None] / (dist[..., None] ** 2 + 1)).sum(1)
        # atractor errante (remolino)
        half = (np.arange(n) % 2)[:, None]
        a1 = np.array([W / 2 + 500 * math.cos(t * 0.7), H / 2 + 250 * math.sin(t * 1.1)])
        a2 = np.array([W / 2 - 450 * math.cos(t * 0.5), H / 2 - 230 * math.sin(t * 0.9)])
        tgt = np.where(half, a1, a2)
        att = (tgt - pos) * 0.00012
        vel = vel + 0.004 * coh + 0.06 * ali + 1.6 * sep + att
        sp = np.hypot(vel[:, 0], vel[:, 1])[:, None]
        vel = vel / sp * np.clip(sp, 3.0, 6.5)
        return (pos + vel) % [W, H], vel

    tt = 0
    for _ in range(90):
        pos, vel = step(pos, vel, tt); tt += 1 / FPS
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        pos, vel = step(pos, vel, tt); tt += 1 / FPS
        trail *= 0.80
        tail = pos - vel * 2.2
        ok = np.hypot(*(pos - tail).T) < 40
        px, py, seg, u = seg_points(tail[ok, 0], tail[ok, 1], pos[ok, 0], pos[ok, 1], 0.8)
        splat(trail, px, py, cols[ok][seg], 0.2 * (0.3 + u))
        img = finish(trail, exposure=1.25, bloom=1.0, fade=ease(t / 0.25) * (1 - ease((t - dur + 0.2) / 0.2)))
        img = caption(img, t, dur, 'Biología · Emergencia', 'Bandadas: 3 reglas, 900 agentes', 'biology')
        yield img


def scene_grayscott(dur):
    h, w = H // 4, W // 4
    U = np.ones((h, w)); V = np.zeros((h, w))
    rng = np.random.default_rng(1)
    for _ in range(40):
        y, x = rng.integers(10, h - 10), rng.integers(10, w - 10)
        V[y - 3:y + 3, x - 3:x + 3] = 0.5; U[y - 3:y + 3, x - 3:x + 3] = 0.25
    F, k, Du, Dv = 0.0545, 0.062, 0.16, 0.08

    def lap(Z):
        return np.roll(Z, 1, 0) + np.roll(Z, -1, 0) + np.roll(Z, 1, 1) + np.roll(Z, -1, 1) - 4 * Z

    def run(U, V, s):
        for _ in range(s):
            uvv = U * V * V
            U = U + Du * lap(U) - uvv + F * (1 - U)
            V = V + Dv * lap(V) + uvv - (F + k) * V
        return U, V
    U, V = run(U, V, 2600)
    pink, deep = C['biochem'], np.array([0.25, 0.05, 0.35])
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        U, V = run(U, V, 14)
        v = np.clip(V * 3.2, 0, 1)
        big = np.asarray(Image.fromarray((v * 255).astype(np.uint8)).resize((W, H), Image.BICUBIC)) / 255.0
        buf = (big[..., None] ** 1.3 * (pink * 1.4)[None, None] + (big[..., None] ** 3) * np.array([1, 0.85, 0.95]) * 0.9
               + (1 - big[..., None]) * deep * 0.15 * big.mean())
        yield finish(buf.astype(np.float32), exposure=1.3, bloom=0.6), ('Arte generativo', 'Reacción–difusión de Gray-Scott', 'biochem')


def scene_mandel(dur):
    cx, cy = -0.743643887037151, 0.131825904205330
    h, w = H // 2, W // 2
    nF = int(dur * FPS)
    for fi in range(nF):
        t = fi / FPS
        scale = 0.9 * math.exp(-t * 3.0)  # zoom exponencial
        xs = cx + (np.arange(w) - w / 2) / w * scale * 3.2
        ys = cy + (np.arange(h) - h / 2) / w * scale * 3.2
        Cc = xs[None, :] + 1j * ys[:, None]
        Z = np.zeros_like(Cc); it = np.full(Cc.shape, 0.0); alive = np.ones(Cc.shape, bool)
        maxit = 260 + int(t * 90)
        for i in range(maxit):
            Z[alive] = Z[alive] ** 2 + Cc[alive]
            esc = alive & (np.abs(Z) > 4)
            it[esc] = i + 1 - np.log2(np.log(np.abs(Z[esc])))
            alive &= ~esc
            if i % 20 == 0 and not alive.any():
                break
        v = np.where(alive, 0, it)
        lv = np.log1p(v)
        band = (0.5 + 0.5 * np.cos(v * 0.35 - t * 6)) ** 1.5
        edge = np.clip((lv - 2.5) / 2.5, 0, 1) ** 1.5
        col = (band[..., None] * (C['math'][None, None] * 1.3 + edge[..., None] * np.array([0.6, 0.5, 0.25]))
               * (0.15 + 0.85 * edge[..., None]))
        col = np.where(alive[..., None], 0, col).astype(np.float32)
        big = np.stack([np.asarray(Image.fromarray(col[..., c]).resize((W, H), Image.BICUBIC)) for c in range(3)], -1)
        yield finish(np.clip(big, 0, None), exposure=1.0, bloom=0.5), ('Matemáticas · Fractales', 'Conjunto de Mandelbrot', 'math')


_yy, _xx = np.mgrid[0:H, 0:W]
GALAXY_CORE = np.exp(-(((_xx - W / 2) / 70) ** 2 + ((_yy - H / 2) / 38) ** 2)).astype(np.float32)
del _yy, _xx


def scene_galaxy(dur, wind=0.35):
    rng = np.random.default_rng(5)
    n = 60000
    arm = rng.integers(0, 2, n)
    r = np.minimum(rng.exponential(0.3, n) + 0.08, 1.5)
    theta0 = arm * np.pi + np.log(r / 0.03) / 0.42 + rng.normal(0, 0.28, n) * (0.4 + r)
    z = rng.normal(0, 0.03, n) * np.exp(-r)
    core = rng.random(n) < 0.10
    r[core] = 0.01 + np.abs(rng.normal(0, 0.07, core.sum()))
    theta0[core] = rng.random(core.sum()) * 2 * np.pi
    temp = np.clip(1 - r / 0.9, 0, 1)
    cols = (np.outer(temp ** 2, np.array([1.0, 0.92, 0.8])) + np.outer(1 - temp ** 2, C['astro'] * 1.2 + np.array([0.05, 0.1, 0.25])))
    br = (0.25 + 0.5 * rng.random(n) ** 4) * np.where(core, 0.25, 1.0) * np.clip(r / 0.12, 0.3, 1)
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        om = 1.4 / (r + 0.15)
        th = theta0 + om * t * wind + 0.5
        x = r * np.cos(th); y = r * np.sin(th)
        tilt = 1.05 - 0.12 * t
        Y = y * math.cos(tilt) - z * math.sin(tilt)
        sc = 620 * (1 + 0.08 * t)
        buf = np.zeros((H, W, 3), np.float32)
        splat(buf, W / 2 + x * sc, H / 2 + Y * sc, cols, br)
        buf += (GALAXY_CORE * 1.4)[..., None] * np.array([1.0, 0.9, 0.8], np.float32)
        yield finish(buf, exposure=1.0, bloom=1.4), ('Astronomía', 'Formación de galaxias', 'astro')


def scene_neural(dur):
    rng = np.random.default_rng(11)
    layers = [6, 10, 12, 10, 4]
    xs = np.linspace(360, W - 360, len(layers))
    nodes = []
    for li, k in enumerate(layers):
        ys = np.linspace(H / 2 - 40 - (k - 1) * 30, H / 2 - 40 + (k - 1) * 30, k)
        nodes += [(xs[li], y, li) for y in ys]
    nodes = np.array(nodes)
    edges = [(i, j) for i in range(len(nodes)) for j in range(len(nodes)) if nodes[j, 2] == nodes[i, 2] + 1]
    edges = np.array(edges)
    wts = rng.normal(0, 1, len(edges))
    ex0, ey0 = nodes[edges[:, 0], 0], nodes[edges[:, 0], 1]
    ex1, ey1 = nodes[edges[:, 1], 0], nodes[edges[:, 1], 1]
    px, py, seg, u = seg_points(ex0, ey0, ex1, ey1, 1.0)
    offs = rng.random(len(edges))
    red, hot = C['ai'], np.array([1.0, 0.75, 0.55])
    gx, gy = np.meshgrid(np.arange(-7, 8), np.arange(-7, 8))
    gk = np.exp(-(gx ** 2 + gy ** 2) / 10).ravel()
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        buf = np.zeros((H, W, 3), np.float32)
        wave = t * 1.9 * 1.0  # onda de activación que cruza la red
        layer_x = (wave % 1.3) * (len(layers) - 1) / 1.0
        splat(buf, px, py, red, 0.05 + 0.06 * np.abs(wts[seg]))
        # pulsos
        ph = (t * 1.6 + offs) % 1.0
        active = np.abs(nodes[edges[:, 0], 2] + ph - layer_x) < 0.9
        qx = ex0 + (ex1 - ex0) * ph; qy = ey0 + (ey1 - ey0) * ph
        ai = np.where(active)[0]
        splat(buf, (qx[ai, None] + gx.ravel() * 0.5).ravel(), (qy[ai, None] + gy.ravel() * 0.5).ravel(), hot,
              np.tile(gk, len(ai)) * 0.35)
        act = np.exp(-((nodes[:, 2] - layer_x) ** 2) * 2.5)
        for (nx, ny, li), a in zip(nodes, act):
            splat(buf, nx + gx.ravel(), ny + gy.ravel(), red * 0.8 + hot * 0.4 * a, gk * (0.9 + 2.5 * a))
        yield finish(buf, exposure=1.2, bloom=1.1), ('Inteligencia artificial', 'Redes neuronales', 'ai')


def scene_montage():
    """4 clips de un compás/2 (1.33 s) cada uno, cortes en la parte."""
    clip = BAR / 2
    for gen in (scene_grayscott(clip), scene_mandel(clip), scene_galaxy(clip), scene_neural(clip)):
        k = 0
        nF = int(round(clip * FPS))
        for img, (lab, title, ck) in gen:
            t = k / FPS
            # destello en el corte
            fl = max(0.0, 1 - t / 0.18) * 0.25
            f = ease(t / 0.12)
            img = (np.clip(img.astype(np.float32) * f + fl * 255, 0, 255)).astype(np.uint8)
            img = caption_fast(img, t, lab, title, ck)
            yield img
            k += 1
            if k >= nF:
                break


def caption_fast(img8, t, label, title, ck, x0=110, y0=960):
    a = ease(t / 0.2)
    img8 = shade(img8, a)
    def fn(d, layer):
        col = C[ck]
        d.rectangle([x0, y0 - 88, x0 + 3, y0 - 8], fill=rgba(col, a))
        spaced_text(d, (x0 + 22, y0 - 62), label.upper(), F_CAP_LABEL, rgba(col, a), 5)
        spaced_text(d, (x0 + 20, y0 - 12), title, F_CAP_TITLE, rgba(C['white'], a * 0.95), 0.5)
    return overlay(img8, fn)


def graph_data():
    rng = np.random.default_rng(21)
    K = len(DISCIPLINES)
    counts = [30, 19, 18, 11, 10, 48, 14, 6, 20, 23]   # proporcional a sims reales (aprox.)
    nodes, cl = [], []
    for ci in range(K):
        a = -np.pi / 2 + ci * 2 * np.pi / K
        ccx, ccy = math.cos(a) * 400, math.sin(a) * 270
        m = max(5, int(counts[ci] / 2.2))
        rr = np.sqrt(rng.random(m)) * (46 + 5 * m ** 0.5)
        aa = rng.random(m) * 2 * np.pi
        for q in range(m):
            nodes.append((ccx + rr[q] * math.cos(aa[q]), ccy + rr[q] * math.sin(aa[q])))
            cl.append(ci)
    nodes = np.array(nodes); cl = np.array(cl)
    edges = []
    for i in range(len(nodes)):
        same = np.where(cl == cl[i])[0]
        d = np.hypot(*(nodes[same] - nodes[i]).T)
        for j in same[np.argsort(d)[1:3]]:
            edges.append((i, j))
    for _ in range(70):   # enlaces interdisciplinares
        i, j = rng.integers(0, len(nodes), 2)
        if cl[i] != cl[j]:
            edges.append((i, j))
    return nodes, cl, np.array(edges)


def scene_graph():
    """El grafo de conocimiento: 10 disciplinas conectadas. Termina implosionando."""
    dur = 2 * BAR
    nodes, cl, edges = graph_data()
    K = len(DISCIPLINES)
    ncol = np.array([C[DISCIPLINES[c][1]] for c in cl])
    rng = np.random.default_rng(2)
    order = np.argsort(cl * 10 + rng.random(len(cl)))
    appear = np.empty(len(nodes)); appear[order] = np.linspace(0.05, 2.0, len(nodes))
    inter = cl[edges[:, 0]] != cl[edges[:, 1]]
    e_start = np.maximum(appear[edges[:, 0]], appear[edges[:, 1]]) + np.where(inter, 0.5 + rng.random(len(edges)) * 1.2, 0.1)
    gx, gy = np.meshgrid(np.arange(-6, 7), np.arange(-6, 7))
    gk = np.exp(-(gx ** 2 + gy ** 2) / 5).ravel()
    f_lab = font(FONT_SANS, 21, 'Medium')
    f_big = font(FONT_SERIF, 60, 'Regular')
    f_small = font(FONT_SANS, 20, 'Medium')
    stats = [('16', 'laboratorios'), ('~200', 'simulaciones'), ('10', 'disciplinas')]
    CX, CY = W / 2, H / 2 - 40
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        rot = 0.08 * t
        implode = ease((t - (dur - 0.55)) / 0.5) ** 2
        R = np.array([[math.cos(rot), -math.sin(rot)], [math.sin(rot), math.cos(rot)]])
        P = nodes @ R.T * (1 + 0.03 * t) * (1 - implode)
        # respiración
        P = P + np.stack([np.sin(t * 1.3 + np.arange(len(P))), np.cos(t * 1.1 + np.arange(len(P)) * 1.7)], 1) * 3
        X = CX + P[:, 0]; Y = CY + P[:, 1]
        buf = np.zeros((H, W, 3), np.float32)
        # aristas que crecen
        prog = np.clip((t - e_start) / 0.45, 0, 1)
        vis = prog > 0
        if vis.any():
            e = edges[vis]; pr = prog[vis]
            x0, y0 = X[e[:, 0]], Y[e[:, 0]]
            x1 = x0 + (X[e[:, 1]] - x0) * pr; y1 = y0 + (Y[e[:, 1]] - y0) * pr
            px, py, seg, u = seg_points(x0, y0, x1, y1, 1.0)
            cA = ncol[e[:, 0]][seg]; cB = ncol[e[:, 1]][seg]
            col = cA * (1 - u[:, None]) + cB * u[:, None]
            w = np.where(inter[vis][seg], 0.07, 0.12)
            splat(buf, px, py, col, w)
            # pulsos viajando por enlaces interdisciplinares
            ie = np.where(inter[vis] & (pr >= 1))[0]
            if len(ie):
                ph = (t * 0.9 + ie * 0.37) % 1
                qx = X[e[ie, 0]] + (X[e[ie, 1]] - X[e[ie, 0]]) * ph
                qy = Y[e[ie, 0]] + (Y[e[ie, 1]] - Y[e[ie, 0]]) * ph
                splat(buf, (qx[:, None] + gx.ravel() * 0.4).ravel(), (qy[:, None] + gy.ravel() * 0.4).ravel(),
                      np.array([1, 1, 1]), np.tile(gk, len(ie)) * 0.25)
        # nodos
        pop = np.clip((t - appear) / 0.25, 0, 1)
        sz = np.where(pop < 1, 1 + 0.8 * np.sin(pop * np.pi), 1) * (pop > 0)
        on = np.where(sz > 0)[0]
        splat(buf, (X[on, None] + gx.ravel() * sz[on, None]).ravel(), (Y[on, None] + gy.ravel() * sz[on, None]).ravel(),
              np.repeat(ncol[on], len(gk), 0), np.tile(gk, len(on)) * 0.9)
        fl = ease((t - (dur - 0.12)) / 0.12) * 0.9
        img = finish(buf, exposure=1.6, bloom=1.3, fade=ease(t / 0.25), flash=fl)

        def fn(d, layer):
            la = ease((t - 1.0) / 0.6) * (1 - ease((t - (dur - 0.7)) / 0.3))
            for ci, (name, ck) in enumerate(DISCIPLINES):
                a = -np.pi / 2 + ci * 2 * np.pi / K + rot
                s_ = 1 + 0.03 * t
                lx, ly = CX + math.cos(a) * 560 * s_, CY + math.sin(a) * 380 * s_
                d.text((lx, ly + 7), name.upper(), font=f_lab, fill=rgba(C[ck] * 0.6 + 0.4, la * 0.9), anchor='ms')
            # estadísticas, una por pulso
            for i, (num, lab) in enumerate(stats):
                a = ease((t - (0.5 + i * 2 * 60 / BPM)) / 0.4) * (1 - ease((t - (dur - 0.6)) / 0.3))
                if a <= 0:
                    continue
                x = W / 2 + (i - 1) * 260
                d.text((x, 985), num, font=f_big, fill=rgba(C['white'], a), anchor='ms')
                spaced_text(d, (x, 1022), lab.upper(), f_small, rgba(C['muted'], a), 4, anchor='m')
        yield overlay(img, fn)


def scene_logo(dur=None):
    """Onda de choque, logo, lema, disciplinas, fundido."""
    dur = dur or (DUR_TOTAL - 12 * BAR)
    f_logo = font(FONT_SERIF, 170, 'Regular')
    f_tag = font(FONT_SANS, 38, 'Light')
    f_disc = font(FONT_SANS, 20, 'Medium')
    f_url = font(FONT_SANS, 22, 'Regular')
    rng = np.random.default_rng(9)
    n = 2600
    ang = rng.random(n) * 2 * np.pi
    spd = rng.gamma(2.0, 1.0, n) * 170
    ci = rng.integers(0, len(DISCIPLINES), n)
    pcol = np.array([C[DISCIPLINES[c][1]] for c in ci]) * 0.8 + 0.2
    orbit_r = 520 + rng.normal(0, 60, n)
    trail = np.zeros((H, W, 3), np.float32)
    for fi in range(int(dur * FPS)):
        t = fi / FPS
        trail *= 0.86
        # partículas: explosión que se asienta en una órbita lenta
        rr = orbit_r * (1 - np.exp(-t * spd / 120))
        th = ang + t * 0.12 * (600 / orbit_r)
        x = W / 2 + rr * np.cos(th); y = H / 2 - 10 + rr * np.sin(th) * 0.42
        a = 0.5 * (1 - 0.5 * ease((t - 1.0) / 2))
        splat(trail, x, y, pcol, a)
        # anillo de choque
        buf = trail.copy()
        if t < 1.2:
            ring_r = 60 + 1500 * ease_out(t / 1.2)
            th2 = np.linspace(0, 2 * np.pi, 4000)
            splat(buf, W / 2 + ring_r * np.cos(th2), H / 2 + ring_r * np.sin(th2) * 0.6, np.array([0.8, 0.85, 1]), 0.9 * (1 - t / 1.2) ** 2)
        fade_out = 1 - ease((t - (dur - 1.4)) / 1.3)
        flash = max(0, 1 - t / 0.35) * 0.8
        img = finish(buf, exposure=1.0, bloom=1.3, fade=fade_out, flash=flash * fade_out)

        def fn(d, layer):
            a_logo = ease((t - 0.25) / 1.0) * fade_out
            spread = 22 * (1 - ease_out((t - 0.25) / 2.2))
            if a_logo > 0:
                spaced_text(d, (W / 2, H / 2 + 30), 'EigenLab', f_logo, rgba(C['white'], a_logo), spread + 2, anchor='m')
            a_tag = ease((t - 1.3) / 0.8) * fade_out
            if a_tag > 0:
                d.text((W / 2, H / 2 + 115), 'Donde las ecuaciones cobran vida', font=f_tag, fill=rgba(C['muted'] * 0.5 + 0.5, a_tag), anchor='ms')
            # disciplinas con puntos de color, en cascada
            names = [nm.upper() for nm, _ in DISCIPLINES]
            ws = [d.textlength(nm, font=f_disc) + 3 * (len(nm) - 1) for nm in names]
            gap = 46
            total = sum(ws) + gap * (len(ws) - 1)
            x = W / 2 - total / 2
            for i, (nm, ck) in enumerate(DISCIPLINES):
                a = ease((t - (2.3 + i * 0.09)) / 0.4) * fade_out
                if a > 0:
                    d.ellipse([x - 18, H / 2 + 196 - 11, x - 8, H / 2 + 196 - 1], fill=rgba(C[ck], a))
                    spaced_text(d, (x, H / 2 + 196), names[i], f_disc, rgba(C['white'] * 0.85, a), 3)
                x += ws[i] + gap
            a_url = ease((t - 3.4) / 0.8) * fade_out
            if a_url > 0:
                spaced_text(d, (W / 2, H - 90), '16 LABORATORIOS  ·  ~200 SIMULACIONES INTERACTIVAS  ·  CÓDIGO ABIERTO', f_url,
                            rgba(C['muted'], a_url * 0.8), 3, anchor='m')
        yield overlay(img, fn)


DUR_TOTAL = 40.0
SCENES = {
    'a_intro': (scene_intro_lorenz, 0, 4),
    'b_pend': (scene_pendulum, 4, 2),
    'c_boids': (scene_boids, 6, 2),
    'd_montage': (scene_montage, 8, 2),
    'e_graph': (scene_graph, 10, 2),
    'f_logo': (scene_logo, 12, None),
}


def frames_needed(name):
    _, b0, nb = SCENES[name]
    t0 = b0 * BAR
    t1 = (b0 + nb) * BAR if nb else DUR_TOTAL
    return int(round(t1 * FPS)) - int(round(t0 * FPS))


def render(name):
    gen, _, _ = SCENES[name]
    need = frames_needed(name)
    out = os.path.join(HERE, 'seg', f'{name}.mov')
    os.makedirs(os.path.dirname(out), exist_ok=True)
    p = subprocess.Popen(['ffmpeg', '-y', '-loglevel', 'error', '-f', 'rawvideo', '-pix_fmt', 'rgb24',
                          '-s', f'{W}x{H}', '-r', str(FPS), '-i', '-', '-c:v', 'libx264', '-preset', 'medium',
                          '-crf', '10', '-pix_fmt', 'yuv420p', out], stdin=subprocess.PIPE)
    k = 0
    last = None
    for img in gen():
        if k >= need:
            break
        p.stdin.write(np.ascontiguousarray(img).tobytes()); last = img; k += 1
        if k % 30 == 0:
            print(name, k, '/', need, flush=True)
    while k < need:   # rellenar si la escena se queda corta por redondeo
        p.stdin.write(last.tobytes()); k += 1
    p.stdin.close(); p.wait()
    print('done', name, need)


if __name__ == '__main__':
    arg = sys.argv[1]
    if arg == 'still':   # previsualización: un fotograma de cada escena
        name, fidx = sys.argv[2], int(sys.argv[3])
        for k, img in enumerate(SCENES[name][0]()):
            if k == fidx:
                Image.fromarray(img).save(os.path.join(HERE, f'still_{name}_{fidx}.png')); break
    else:
        render(arg)
