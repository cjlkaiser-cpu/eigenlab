"""Imágenes para compartir (Open Graph, 1200×630) de cada laboratorio, a partir de las escenas del vídeo.

Uso: python3.11 og_images.py            # todas, en paralelo
     python3.11 og_images.py <clave>    # una sola
Salida: _portal/og/<clave>.jpg
"""
import os
import sys
from concurrent.futures import ProcessPoolExecutor

import numpy as np
from PIL import Image, ImageDraw

import video
import promo2

OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), '_portal', 'og')
OW, OH = 1200, 630

# clave -> (escena, fotograma, etiqueta, título, color)
SHOTS = {
    'physics-visual-lab': ('b_pend', 45, 'Física', 'Physics Visual Lab', 'physics'),
    'physics-sound-lab': ('c_chladni', 100, 'Física del sonido', 'Physics Sound Lab', 'sound'),
    'chemistry-visual-lab': ('d_orbitals', 130, 'Química', 'Chemistry Visual Lab', 'chemistry'),
    'biochem-visual-lab': ('e_dna', 90, 'Bioquímica', 'Biochem Visual Lab', 'biochem'),
    'biology-visual-lab': ('f_boids', 110, 'Biología', 'Biology Visual Lab', 'biology'),
    'geology-visual-lab': ('g_erosion', 140, 'Geología', 'Geology Visual Lab', 'geology'),
    'math-visual-lab': ('a_intro', 300, 'Matemáticas', 'Math Visual Lab', 'math'),
    'math-generative-art-lab': ('l_montage', 20, 'Arte generativo', 'Math Generative Art Lab', 'biochem'),
    'math-sound-lab': ('l_montage', 100, 'Sonificación matemática', 'Math Sound Lab', 'chemistry'),
    'astronomy-visual-lab': ('h_galaxy', 110, 'Astronomía', 'Astronomy Visual Lab', 'astro'),
    'astronomy-sound-lab': ('l_montage', 300, 'Astronomía y sonido', 'Astronomy Sound Lab', 'astro'),
    'computation-lab': ('i_life', 100, 'Computación', 'Computation Lab', 'computation'),
    'ai-visual-lab': ('j_neural', 100, 'Inteligencia artificial', 'AI Visual Lab', 'ai'),
    'music-theory-lab': ('k_phyllo', 120, 'Música', 'Music Theory Lab', 'music'),
    'eigenlab': ('q_logo', 150, None, None, None),   # fotograma del logo, sin rótulo añadido
}


def no_caption(img, *a, **k):
    return img


def frame(scene, idx):
    # sin rótulos del vídeo: se sustituyen por los de la imagen
    for mod in (video, promo2):
        mod.caption = no_caption
        mod.caption_fast = no_caption
    fn = promo2.span(scene)[0]
    for k, img in enumerate(fn()):
        if k == idx:
            return img


def render(key):
    scene, idx, label, title, ck = SHOTS[key]
    img = Image.fromarray(frame(scene, idx))
    # recorte 1.905:1 centrado y escalado
    w, h = img.size
    ch = int(w * OH / OW)
    img = img.crop((0, (h - ch) // 2, w, (h - ch) // 2 + ch)).resize((OW, OH), Image.LANCZOS)
    if title is None:
        return save(img, key)
    # degradado inferior para legibilidad
    a = np.asarray(img).astype(np.float32)
    yy = np.linspace(0, 1, OH)[:, None, None]
    a *= 1 - 0.85 * np.clip((yy - 0.38) / 0.5, 0, 1) ** 1.2
    img = Image.fromarray(a.astype(np.uint8)).convert('RGBA')
    layer = Image.new('RGBA', img.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(layer)
    col = video.rgba(video.C[ck], 1)
    wordmark = video.font(video.FONT_SERIF, 34, 'Regular')
    d.text((60, 70), 'EigenLab', font=wordmark, fill=video.rgba(video.C['white'], 0.9), anchor='ls')
    lab_f = video.font(video.FONT_SANS, 22, 'SemiBold')
    title_f = video.font(video.FONT_SERIF, 64 if len(title) < 26 else 52, 'Regular')
    d.rectangle([60, OH - 150, 64, OH - 62], fill=col)
    video.spaced_text(d, (84, OH - 122), label.upper(), lab_f, col, 5)
    d.text((82, OH - 66), title, font=title_f, fill=video.rgba(video.C['white'], 1), anchor='ls')
    return save(Image.alpha_composite(img, layer).convert('RGB'), key)


def save(img, key):
    os.makedirs(OUT, exist_ok=True)
    img.save(os.path.join(OUT, key + '.jpg'), quality=84, optimize=True, progressive=True)
    return key


if __name__ == '__main__':
    keys = sys.argv[1:] or list(SHOTS)
    with ProcessPoolExecutor(max_workers=8) as ex:
        for k in ex.map(render, keys):
            print('ok', k, flush=True)
