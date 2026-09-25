"""EigenLab promo — música generativa sintetizada (40 s, 90 BPM, Re menor)."""
import numpy as np
from scipy.signal import butter, sosfilt, fftconvolve
import wave

SR = 44100
DUR = 40.0
BPM = 90
BEAT = 60 / BPM
BAR = 4 * BEAT
N = int(SR * DUR)
t_all = np.arange(N) / SR
rng = np.random.default_rng(7)

L = np.zeros(N)
R = np.zeros(N)


def midi(m):
    return 440 * 2 ** ((m - 69) / 12)


def add(sig, start, gain=1.0, pan=0.0):
    i = int(start * SR)
    if i >= N:
        return
    sig = sig[: N - i]
    gl = gain * np.cos((pan + 1) * np.pi / 4)
    gr = gain * np.sin((pan + 1) * np.pi / 4)
    L[i:i + len(sig)] += sig * gl
    R[i:i + len(sig)] += sig * gr


def lp(x, fc, order=2):
    return sosfilt(butter(order, fc, 'low', fs=SR, output='sos'), x)


def hp(x, fc, order=2):
    return sosfilt(butter(order, fc, 'high', fs=SR, output='sos'), x)


def saw(f, n, phase=0.0):
    ph = (phase + f * np.arange(n) / SR) % 1.0
    return 2 * ph - 1


# ---- progresión (por compás, 15 compases) ----
Dm9 = [50, 57, 60, 64, 65]       # D A C E F
Bbmaj7 = [46, 53, 57, 62, 65]    # Bb F A D F
Fmaj7 = [41, 53, 57, 60, 64]     # F  F A C E
Cadd9 = [48, 55, 62, 64, 67]     # C G D E G
Gm9 = [43, 55, 58, 62, 69]
prog = [Dm9, Dm9, Bbmaj7, Bbmaj7, Fmaj7, Fmaj7, Cadd9, Cadd9,
        Dm9, Dm9, Bbmaj7, Cadd9, Dm9, Dm9, Dm9]

# ---- pad: sierras desafinadas, filtradas, crossfade por acorde ----
def pad_chord(notes, dur, bright):
    n = int(dur * SR)
    x = np.zeros(n)
    for m in notes:
        for d in (-0.07, 0.0, 0.07):
            x += saw(midi(m + d), n, rng.random())
    x = lp(x, bright, 2) / (len(notes) * 3)
    env = np.minimum(1, np.arange(n) / (0.6 * SR)) * np.minimum(1, (n - np.arange(n)) / (0.6 * SR))
    return x * env

seg = 0
while seg < len(prog):
    j = seg
    while j + 1 < len(prog) and prog[j + 1] is prog[seg]:
        j += 1
    start = seg * BAR
    dur = (j - seg + 1) * BAR + 0.6
    if seg == 12:
        dur = DUR - start
    bright = 700 if seg < 2 else (1400 if seg < 8 else 2200)
    if seg >= 12:
        bright = 1800
    g = 0.22 if seg == 0 else 0.30
    p = pad_chord(prog[seg], dur, bright)
    if seg == 0:   # entrada en fade lento
        p *= np.minimum(1, np.arange(len(p)) / (4 * SR))
    add(p, start - (0.3 if seg else 0), g, -0.3)
    add(pad_chord(prog[seg], dur, bright), start - (0.3 if seg else 0), g * 0.9, 0.3)
    seg = j + 1

# ---- sub bajo (desde compás 2) ----
for b in range(2, 15):
    root = prog[b][0]
    while root > 45:
        root -= 12
    n = int(BAR * SR)
    tt = np.arange(n) / SR
    s = np.sin(2 * np.pi * midi(root) * tt)
    env = np.minimum(1, tt / 0.05) * np.exp(-tt * 0.35)
    if b >= 12:
        n = int((DUR - b * BAR) * SR); tt = np.arange(n) / SR
        s = np.sin(2 * np.pi * midi(root) * tt); env = np.minimum(1, tt / 0.05) * np.exp(-tt * 0.25)
        add(s * env, b * BAR, 0.32)
        break
    add(s * env, b * BAR, 0.26)

# ---- arpegio: secuencia de Fibonacci mod 5 sobre notas del acorde ----
fib = [0, 1]
while len(fib) < 400:
    fib.append(fib[-1] + fib[-2])
step = BEAT / 4
k = 0
for b in range(2, 12):
    notes = sorted(prog[b][1:])
    for s16 in range(16):
        tt0 = b * BAR + s16 * step
        m = notes[fib[k] % len(notes)] + 12 + (12 if (fib[k] % 7 == 0 and b >= 6) else 0)
        k += 1
        n = int(0.5 * SR)
        tt = np.arange(n) / SR
        f = midi(m)
        tone = np.sin(2 * np.pi * f * tt) + 0.25 * np.sin(2 * np.pi * 2 * f * tt) + 0.08 * np.sin(2 * np.pi * 3 * f * tt)
        env = np.minimum(1, tt / 0.004) * np.exp(-tt * 9)
        vel = 0.55 + 0.45 * (s16 % 4 == 0)
        ramp = min(1, (b - 1) / 3)
        add(tone * env, tt0, 0.075 * vel * ramp, np.sin(k * 0.9) * 0.6)
        # eco (delay de corchea con puntillo)
        add(tone * env, tt0 + 0.75 * BEAT, 0.03 * vel * ramp, -np.sin(k * 0.9) * 0.6)

# ---- bombo (compases 4–11) ----
def kick():
    n = int(0.45 * SR)
    tt = np.arange(n) / SR
    f = 45 + 80 * np.exp(-tt * 30)
    ph = 2 * np.pi * np.cumsum(f) / SR
    return np.sin(ph) * np.exp(-tt * 7) * np.minimum(1, tt / 0.002)

K = kick()
for b in range(4, 12):
    for q in range(4):
        if b < 6 and q % 2:   # medio tiempo al principio
            continue
        add(K, b * BAR + q * BEAT, 0.55 if b >= 6 else 0.45)

# ---- hi-hat (compases 8–11) ----
def hat(decay):
    n = int(0.12 * SR)
    x = hp(rng.standard_normal(n), 7000, 2)
    return x * np.exp(-np.arange(n) / SR * decay)

for b in range(8, 12):
    for e in range(8):
        add(hat(60 if e % 2 else 90), b * BAR + e * BEAT / 2, 0.05 if e % 2 else 0.03, 0.35)

# ---- riser (ruido) antes del clímax en compás 12 ----
rs, re_ = 10 * BAR, 12 * BAR
n = int((re_ - rs) * SR)
tt = np.arange(n) / SR
nz = rng.standard_normal(n)
sweep = np.zeros(n)
for i0 in range(0, n, 2048):   # filtro que abre progresivamente
    fc = 300 + 7000 * (i0 / n) ** 2
    sweep[i0:i0 + 2048] = lp(nz[max(0, i0 - 4096):i0 + 2048], fc)[-len(sweep[i0:i0 + 2048]):]
add(sweep * (tt / tt[-1]) ** 2.5, rs, 0.16, 0.0)

# riser inicial suave (0–5 s)
n = int(5.3 * SR); tt = np.arange(n) / SR
add(lp(rng.standard_normal(n), 1500) * (tt / tt[-1]) ** 2 * 0.5, 0, 0.12)

# ---- impacto en el logo (compás 12 = 32 s) ----
n = int(6 * SR); tt = np.arange(n) / SR
boom = np.sin(2 * np.pi * (38 + 40 * np.exp(-tt * 8)) * tt) * np.exp(-tt * 1.2)
add(boom, 12 * BAR, 0.55)
add(lp(rng.standard_normal(n), 3000) * np.exp(-tt * 3), 12 * BAR, 0.12)
# campana áurea: parciales a razón φ
phi = (1 + 5 ** 0.5) / 2
bell = sum(np.sin(2 * np.pi * midi(74) * phi ** (i / 2) * tt) * np.exp(-tt * (0.8 + i)) / (i + 1) for i in range(5))
add(bell, 12 * BAR, 0.12, -0.2)
add(bell, 12 * BAR + 0.02, 0.12, 0.2)

# ---- reverb por convolución ----
def reverb(x, secs, seed):
    r = np.random.default_rng(seed)
    n = int(secs * SR)
    ir = r.standard_normal(n) * np.exp(-np.arange(n) / SR * (6.9 / secs))
    ir = lp(ir, 5000)
    ir /= np.sqrt(np.sum(ir ** 2))
    return fftconvolve(x, ir)[: len(x)]

wetL = reverb(L, 3.2, 1)
wetR = reverb(R, 3.2, 2)
L2 = L + 0.35 * wetL
R2 = R + 0.35 * wetR

# fade final y master
fade = np.ones(N)
fn = int(2.5 * SR)
fade[-fn:] = np.linspace(1, 0, fn) ** 1.5
L2 *= fade; R2 *= fade
st = np.stack([L2, R2], 1)
st = np.tanh(st * 1.1)
st /= np.max(np.abs(st)) / 0.89

pcm = (st * 32767).astype('<i2')
with wave.open('music.wav', 'wb') as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(SR)
    w.writeframes(pcm.tobytes())
print('music.wav', DUR, 's')
