"""EigenLab promo (versión larga) — música sintetizada, 44 compases a 90 BPM (~117 s), Re menor.

Estructura (compases):
  0–3   intro: pad + riser, arpegio desde 2
  4–11  recorrido I: bajo + bombo a medio tiempo
  12–23 recorrido II: bombo completo + hi-hat en contratiempo
  24–27 montaje: armonía a compás, palmas, 16avos, arpegio octava alta
  28–32 motor WASM + teseracto: groove
  33–35 pausa (cita): solo pad, riser hacia el clímax
  36–39 clímax (grafo): todo
  40–43 logo: impacto, campana áurea, cola
"""
import numpy as np
from scipy.signal import butter, sosfilt, fftconvolve
import wave

SR = 44100
BPM = 90
BEAT = 60 / BPM
BAR = 4 * BEAT
NBARS = 44
DUR = NBARS * BAR
N = int(SR * DUR)
rng = np.random.default_rng(7)
L = np.zeros(N)
R = np.zeros(N)


def midi(m):
    return 440 * 2 ** ((m - 69) / 12)


def add(sig, start, gain=1.0, pan=0.0):
    i = int(start * SR)
    if i >= N or i + len(sig) <= 0:
        return
    if i < 0:
        sig = sig[-i:]; i = 0
    sig = sig[: N - i]
    L[i:i + len(sig)] += sig * gain * np.cos((pan + 1) * np.pi / 4)
    R[i:i + len(sig)] += sig * gain * np.sin((pan + 1) * np.pi / 4)


def lp(x, fc, order=2):
    return sosfilt(butter(order, fc, 'low', fs=SR, output='sos'), x)


def hp(x, fc, order=2):
    return sosfilt(butter(order, fc, 'high', fs=SR, output='sos'), x)


def bp(x, lo, hi):
    return sosfilt(butter(2, [lo, hi], 'band', fs=SR, output='sos'), x)


def saw(f, n):
    return 2 * ((rng.random() + f * np.arange(n) / SR) % 1.0) - 1


Dm9 = [50, 57, 60, 64, 65]
Bb = [46, 53, 57, 62, 65]
F = [41, 53, 57, 60, 64]
Cc = [48, 55, 62, 64, 67]
Gm9 = [43, 55, 58, 62, 69]
Asus = [45, 52, 57, 62, 64]
prog = ([Dm9, Dm9, Bb, Bb, F, F, Cc, Cc] * 3            # 0–23
        + [Dm9, Bb, F, Cc]                               # 24–27 montaje
        + [Dm9, Dm9, Bb, F, Cc]                          # 28–32
        + [Gm9, Bb, Asus]                                # 33–35 pausa
        + [Dm9, Bb, F, Cc]                               # 36–39 clímax
        + [Dm9] * 4)                                     # 40–43 logo
assert len(prog) == NBARS


def in_(b, *ranges):
    return any(lo <= b <= hi for lo, hi in ranges)


# ---- pad (acordes agrupados cuando se repiten) ----
def pad_chord(notes, dur, bright):
    n = int(dur * SR)
    x = sum(saw(midi(m + d), n) for m in notes for d in (-0.07, 0.0, 0.07))
    x = lp(x, bright) / (len(notes) * 3)
    k = np.arange(n)
    return x * np.minimum(1, k / (0.6 * SR)) * np.minimum(1, (n - k) / (0.6 * SR))

b = 0
while b < NBARS:
    j = b
    while j + 1 < NBARS and prog[j + 1] is prog[b] and not (j + 1 in (24, 33, 36, 40)):
        j += 1
    start = b * BAR
    dur = (j - b + 1) * BAR + 0.6 if j < NBARS - 1 else DUR - start
    if in_(b, (0, 3)):
        bright, g = 700, 0.22
    elif in_(b, (33, 35)):
        bright, g = 900 + 300 * (b - 33), 0.34
    elif in_(b, (24, 27), (36, 39)):
        bright, g = 2600, 0.26
    elif b >= 40:
        bright, g = 1800, 0.32
    else:
        bright, g = 1300 + 40 * b, 0.28
    for pan in (-0.3, 0.3):
        p = pad_chord(prog[b], dur, bright)
        if b == 0:
            p *= np.minimum(1, np.arange(len(p)) / (4 * SR))
        add(p, start - (0.3 if b else 0), g * (1 if pan < 0 else 0.9), pan)
    b = j + 1

# ---- sub bajo ----
for b in range(4, NBARS):
    if in_(b, (33, 35)):
        continue
    root = prog[b][0]
    while root > 45:
        root -= 12
    last = b == 40
    n = int(((DUR - b * BAR) if last else BAR) * SR)
    tt = np.arange(n) / SR
    s = np.sin(2 * np.pi * midi(root) * tt) + 0.15 * np.sin(4 * np.pi * midi(root) * tt)
    env = np.minimum(1, tt / 0.05) * np.exp(-tt * (0.25 if last else 0.35))
    add(s * env, b * BAR, 0.32 if last else 0.26)
    if last:
        break

# ---- arpegio: índices de Fibonacci ----
fib = [0, 1]
while len(fib) < 1200:
    fib.append(fib[-1] + fib[-2])
step = BEAT / 4
k = 0
for b in range(2, 40):
    if in_(b, (33, 35)):
        continue
    notes = sorted(prog[b][1:])
    up = 12 if in_(b, (24, 27), (36, 39)) else 0
    for s16 in range(16):
        tt0 = b * BAR + s16 * step
        m = notes[fib[k] % len(notes)] + 12 + up + (12 if (fib[k] % 7 == 0 and b >= 6) else 0)
        k += 1
        n = int(0.5 * SR); tt = np.arange(n) / SR; f = midi(m)
        tone = np.sin(2 * np.pi * f * tt) + 0.25 * np.sin(4 * np.pi * f * tt) + 0.08 * np.sin(6 * np.pi * f * tt)
        env = np.minimum(1, tt / 0.004) * np.exp(-tt * 9)
        vel = 0.55 + 0.45 * (s16 % 4 == 0)
        ramp = min(1, (b - 1) / 3)
        pan = np.sin(k * 0.9) * 0.6
        add(tone * env, tt0, 0.075 * vel * ramp, pan)
        add(tone * env, tt0 + 0.75 * BEAT, 0.03 * vel * ramp, -pan)

# ---- percusión ----
def kick():
    n = int(0.45 * SR); tt = np.arange(n) / SR
    ph = 2 * np.pi * np.cumsum(45 + 80 * np.exp(-tt * 30)) / SR
    return np.sin(ph) * np.exp(-tt * 7) * np.minimum(1, tt / 0.002)


def hat(decay):
    n = int(0.12 * SR)
    return hp(rng.standard_normal(n), 7000) * np.exp(-np.arange(n) / SR * decay)


def clap():
    n = int(0.3 * SR); tt = np.arange(n) / SR
    nz = bp(rng.standard_normal(n), 900, 5000)
    env = sum(np.exp(-np.maximum(tt - d, 0) * 60) * (tt >= d) for d in (0, 0.011, 0.023)) * 0.5 + np.exp(-tt * 14) * 0.6
    body = np.sin(2 * np.pi * 185 * tt) * np.exp(-tt * 30) * 0.5
    return nz * env + body

K = kick()
for b in range(4, 40):
    if in_(b, (33, 35)):
        continue
    for q in range(4):
        if b < 12 and q % 2:
            continue
        add(K, b * BAR + q * BEAT, 0.45 if b < 12 else 0.55)
    # hi-hats
    if in_(b, (12, 23)):
        for e in range(1, 8, 2):
            add(hat(80), b * BAR + e * BEAT / 2, 0.045, 0.35)
    elif in_(b, (24, 27), (36, 39)):
        for s in range(16):
            add(hat(90 if s % 2 else 60), b * BAR + s * BEAT / 4, 0.05 if s % 4 == 2 else 0.025, 0.35)
        for q in (1, 3):
            add(clap(), b * BAR + q * BEAT, 0.22, -0.1)
    elif in_(b, (28, 32)):
        for e in range(8):
            add(hat(60 if e % 2 else 90), b * BAR + e * BEAT / 2, 0.05 if e % 2 else 0.03, 0.35)

# ---- risers ----
def riser(b0, b1, gain):
    n = int((b1 - b0) * BAR * SR); tt = np.arange(n) / SR
    nz = rng.standard_normal(n); out = np.zeros(n)
    for i0 in range(0, n, 2048):
        fc = 300 + 7000 * (i0 / n) ** 2
        seg = lp(nz[max(0, i0 - 4096):i0 + 2048], fc)
        out[i0:i0 + 2048] = seg[-len(out[i0:i0 + 2048]):]
    add(out * (tt / tt[-1]) ** 2.5, b0 * BAR, gain)

n = int(2 * BAR * SR); tt = np.arange(n) / SR
add(lp(rng.standard_normal(n), 1500) * (tt / tt[-1]) ** 2 * 0.5, 0, 0.12)
riser(22, 24, 0.12)
riser(34, 36, 0.18)
riser(38, 40, 0.14)

# ---- impactos ----
def boom(t0, gain, secs=6):
    n = int(secs * SR); tt = np.arange(n) / SR
    add(np.sin(2 * np.pi * (38 + 40 * np.exp(-tt * 8)) * tt) * np.exp(-tt * 1.2), t0, gain)
    add(lp(rng.standard_normal(n), 3000) * np.exp(-tt * 3), t0, gain * 0.22)

boom(24 * BAR, 0.35, 3)
boom(36 * BAR, 0.4, 3)
boom(40 * BAR, 0.55)
phi = (1 + 5 ** 0.5) / 2
n = int(8 * SR); tt = np.arange(n) / SR
bell = sum(np.sin(2 * np.pi * midi(74) * phi ** (i / 2) * tt) * np.exp(-tt * (0.6 + i)) / (i + 1) for i in range(5))
add(bell, 40 * BAR, 0.12, -0.2); add(bell, 40 * BAR + 0.02, 0.12, 0.2)
# campanas suaves durante la cita
for i, m in enumerate([69, 72, 74]):
    add(sum(np.sin(2 * np.pi * midi(m) * phi ** (h / 2) * tt) * np.exp(-tt * (1.2 + h)) / (h + 1) for h in range(4)),
        (33 + i) * BAR, 0.06, (-0.4, 0.0, 0.4)[i])

# ---- reverb y master ----
def reverb(x, secs, seed):
    r = np.random.default_rng(seed); n = int(secs * SR)
    ir = lp(r.standard_normal(n) * np.exp(-np.arange(n) / SR * (6.9 / secs)), 5000)
    return fftconvolve(x, ir / np.sqrt(np.sum(ir ** 2)))[: len(x)]

L2 = L + 0.35 * reverb(L, 3.2, 1)
R2 = R + 0.35 * reverb(R, 3.2, 2)
fade = np.ones(N); fn = int(3 * SR)
fade[-fn:] = np.linspace(1, 0, fn) ** 1.5
st = np.tanh(np.stack([L2 * fade, R2 * fade], 1) * 1.1)
st /= np.max(np.abs(st)) / 0.89
with wave.open('music2.wav', 'wb') as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(SR)
    w.writeframes((st * 32767).astype('<i2').tobytes())
print('music2.wav', round(DUR, 2), 's')
