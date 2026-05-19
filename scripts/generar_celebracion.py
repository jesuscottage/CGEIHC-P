"""
Genera app/assets/audio/celebracion.wav
Fanfarria de victoria con timbre de trompeta (síntesis aditiva).
Solo usa librerías estándar de Python: math, array, wave.
"""

import math
import array
import wave
import os

RATE = 44100

def make_note(freq, duration, amp=0.72, attack=0.03, decay=0.06, sustain=0.75, release=0.12):
    n = int(RATE * duration)
    samples = []
    atk = int(RATE * attack)
    dec = int(RATE * (attack + decay))
    rel_start = n - int(RATE * release)

    # Armónicos de trompeta (síntesis aditiva)
    harmonics = [(1, 1.00), (2, 0.65), (3, 0.45), (4, 0.28),
                 (5, 0.18), (6, 0.10), (7, 0.06)]
    norm = sum(a for _, a in harmonics)

    for i in range(n):
        t = i / RATE
        s = sum(a * math.sin(2 * math.pi * freq * h * t) for h, a in harmonics) / norm
        # Envolvente ADSR
        if i < atk:
            env = i / atk
        elif i < dec:
            env = 1.0 - (1.0 - sustain) * (i - atk) / (dec - atk)
        elif i < rel_start:
            env = sustain
        else:
            rlen = n - rel_start
            env = sustain * (1.0 - (i - rel_start) / rlen) if rlen > 0 else 0.0
        samples.append(s * env * amp)
    return samples


def silence(duration):
    return [0.0] * int(RATE * duration)


# ── Melodía: fanfarria de victoria clásica ───────────────────────────────────
# Patrón: ascenso rápido + nota larga + frase conclusiva
melody = [
    # Llamada inicial
    (392, 0.12),  # G4
    (523, 0.12),  # C5
    (659, 0.12),  # E5
    (784, 0.38),  # G5  ←  nota larga
    (784, 0.12),  # G5
    (784, 0.55),  # G5  ←  muy larga
    (None, 0.10), # silencio

    # Frase descendente gloriosa
    (699, 0.18),  # F5
    (659, 0.18),  # E5
    (622, 0.18),  # Eb5
    (523, 0.55),  # C5  ←  larga
    (None, 0.10),

    # Remate ascendente
    (415, 0.18),  # Ab4
    (622, 0.18),  # Eb5
    (622, 0.18),  # Eb5
    (587, 0.65),  # D5  ←  final largo
    (None, 0.15),

    # Segunda vuelta: más energía
    (392, 0.10),
    (523, 0.10),
    (659, 0.10),
    (784, 0.10),
    (880, 0.45),  # A5
    (880, 0.10),
    (880, 0.60),
    (None, 0.12),

    (784, 0.18),
    (740, 0.18),
    (698, 0.18),
    (659, 0.65),
    (None, 0.12),

    (523, 0.18),
    (784, 0.18),
    (784, 0.18),
    (784, 0.75),  # nota final, larga con fade natural
]

all_samples = []
for item in melody:
    freq, dur = item
    if freq is None:
        all_samples.extend(silence(dur))
    else:
        all_samples.extend(make_note(freq, dur))

# Normalizar para evitar clipping
peak = max(abs(s) for s in all_samples)
if peak > 0:
    all_samples = [s / peak * 0.92 for s in all_samples]

# Guardar como WAV 16-bit mono
out_path = os.path.join(os.path.dirname(__file__), "..", "app", "assets", "audio", "celebracion.wav")
out_path = os.path.normpath(out_path)

data = array.array('h', [int(max(-32767, min(32767, s * 32767))) for s in all_samples])

with wave.open(out_path, 'w') as f:
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(RATE)
    f.writeframes(data.tobytes())

dur_s = len(all_samples) / RATE
print(f"Generado: {out_path}  ({dur_s:.1f}s)")
