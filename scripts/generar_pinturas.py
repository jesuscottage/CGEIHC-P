#!/usr/bin/env python3
"""Genera 12 pinturas PNG para la galería del museo virtual — Calentamiento Global.

Pinturas 01-06: consecuencias (corredor izquierdo)
Pinturas 07-12: soluciones   (corredor derecho)
"""

from PIL import Image, ImageDraw
import os, math, random

OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                   '..', 'app', 'assets', 'textures', 'gallery')
os.makedirs(OUT, exist_ok=True)

W, H = 512, 384


def lc(c1, c2, t):
    return tuple(int(c1[i] + t * (c2[i] - c1[i])) for i in range(3))


def grad_v(draw, y0, y1, c1, c2):
    for y in range(y0, y1):
        t = (y - y0) / max(1, y1 - y0 - 1)
        draw.line([(0, y), (W, y)], fill=lc(c1, c2, t) + (255,))


def nueva(bg=(0, 0, 0)):
    img = Image.new('RGBA', (W, H), bg + (255,))
    draw = ImageDraw.Draw(img)
    return img, draw


def overlay(img, color_rgba):
    ov = Image.new('RGBA', (W, H), color_rgba)
    img.paste(ov, (0, 0), ov)


# ─── Pintura 01 — Deshielo Ártico ────────────────────────────────────────────
def p01():
    img, draw = nueva()
    grad_v(draw, 0, H // 2, (5, 30, 90), (80, 160, 210))
    grad_v(draw, H // 2, H, (0, 15, 60), (0, 8, 40))
    # Masas de hielo
    ice = (215, 238, 255, 255)
    draw.polygon([(40, H // 2), (130, H // 4 + 10), (220, H // 2)], fill=ice)
    draw.polygon([(200, H // 2 - 5), (300, H // 4 - 10), (400, H // 2)], fill=(195, 225, 248, 255))
    draw.polygon([(350, H // 2 + 5), (430, H // 4 + 20), (490, H // 2)], fill=(220, 242, 255, 255))
    # Grieta principal
    crack_pts = [(130, H // 4 + 10), (160, H // 2 - 10), (200, H // 2 - 5)]
    draw.line(crack_pts, fill=(50, 130, 190, 255), width=3)
    # Reflejo en el agua
    for i in range(8):
        y = H // 2 + 12 + i * 18
        draw.line([(20 + i * 8, y), (160 - i * 6, y)], fill=(40, 90, 160, 255), width=2)
        draw.line([(220 + i * 5, y), (350 - i * 4, y)], fill=(35, 80, 145, 255), width=2)
    # Tinte frío
    overlay(img, (0, 40, 120, 30))
    return img


# ─── Pintura 02 — Fauna en Peligro ───────────────────────────────────────────
def p02():
    img, draw = nueva()
    grad_v(draw, 0, int(H * 0.55), (20, 5, 0), (180, 60, 5))
    grad_v(draw, int(H * 0.55), H, (200, 70, 5), (240, 150, 20))
    # Luna roja
    draw.ellipse([W - 100, 20, W - 30, 90], fill=(230, 50, 20, 255))
    # Plataforma de hielo menguante
    draw.ellipse([100, int(H * 0.55) - 15, 360, int(H * 0.55) + 45], fill=(210, 235, 252, 255))
    # Silueta oso polar
    bx, by = 190, int(H * 0.55) - 55
    draw.ellipse([bx, by, bx + 65, by + 38], fill=(245, 250, 255, 255))   # cuerpo
    draw.ellipse([bx + 50, by - 22, bx + 85, by + 14], fill=(240, 248, 255, 255))  # cabeza
    draw.ellipse([bx + 75, by - 8, bx + 90, by + 8], fill=(240, 248, 255, 255))    # hocico
    draw.ellipse([bx + 4, by + 30, bx + 22, by + 52], fill=(235, 245, 255, 255))   # pata
    draw.ellipse([bx + 38, by + 28, bx + 56, by + 50], fill=(235, 245, 255, 255))  # pata
    # Aves (siluetas en V)
    for bx2, by2 in [(50, 70), (100, 50), (390, 80), (440, 55), (475, 75)]:
        draw.line([(bx2, by2), (bx2 + 12, by2 - 8)], fill=(10, 5, 0, 255), width=2)
        draw.line([(bx2 + 12, by2 - 8), (bx2 + 24, by2)], fill=(10, 5, 0, 255), width=2)
    # Tinte rojo dramático
    overlay(img, (160, 0, 0, 55))
    return img


# ─── Pintura 03 — Ciudades Sumergidas ────────────────────────────────────────
def p03():
    img, draw = nueva()
    wl = int(H * 0.42)
    grad_v(draw, 0, wl, (8, 8, 28), (30, 30, 75))
    grad_v(draw, wl, H, (0, 18, 58), (0, 8, 38))
    # Edificios
    buildings = [(40, 180), (80, 140), (120, 200), (170, 160),
                 (210, 120), (255, 175), (300, 140), (345, 190),
                 (390, 150), (435, 170), (475, 130)]
    widths    = [35, 38, 32, 40, 42, 33, 38, 35, 36, 32, 34]
    for (bx, bh), bw in zip(buildings, widths):
        top = wl - bh
        if top < wl:
            draw.rectangle([bx, top, bx + bw, wl], fill=(55, 55, 75, 255))
            for wy in range(top + 6, wl, 14):
                for wx in range(bx + 5, bx + bw - 5, 10):
                    draw.rectangle([wx, wy, wx + 5, wy + 7], fill=(200, 175, 40, 255))
        draw.rectangle([bx, wl, bx + bw, H], fill=(0, 35, 78, 255))
    # Brillo submarino ondulante
    for i in range(9):
        y = wl + 15 + i * 22
        draw.line([(0, y), (W, y)], fill=(0, 55 - i * 4, 95 - i * 7, 255), width=4)
    draw.line([(0, wl), (W, wl)], fill=(0, 160, 215, 255), width=3)
    overlay(img, (0, 10, 50, 20))
    return img


# ─── Pintura 04 — Glaciares en Retroceso ─────────────────────────────────────
def p04():
    img, draw = nueva()
    # Mitad izquierda: glaciar
    grad_v(draw, 0, H, (195, 218, 238), (240, 250, 255))
    # Mitad derecha: océano oscuro
    rng = random.Random(42)
    x_cur = W // 2
    pts_glacier = [(x_cur, 0)]
    y = 0
    while y < H + 40:
        step_y = rng.randint(22, 50)
        step_x = rng.randint(-28, 28)
        y += step_y
        x_cur = max(W // 3, min(2 * W // 3, x_cur + step_x))
        pts_glacier.append((x_cur, y))
    pts_glacier += [(W, H + 5), (W, 0)]
    draw.polygon(pts_glacier, fill=(0, 25, 72, 255))
    # Bloques de hielo flotando en el océano
    for bx, by, bw in [(330, 90, 28), (400, 210, 18), (455, 310, 15), (360, 330, 22)]:
        draw.polygon([(bx, by), (bx + bw, by - 9), (bx + bw * 2, by), (bx + bw, by + 7)],
                     fill=(215, 232, 248, 255))
    # Temperatura (+2.7 °C) en rojo brillante
    for dx in range(-2, 3):
        for dy in range(-2, 3):
            if dx != 0 or dy != 0:
                draw.text((W * 3 // 4 - 45 + dx, H // 5 + dy), "+2.7°C",
                          fill=(80, 0, 0, 200))
    draw.text((W * 3 // 4 - 45, H // 5), "+2.7°C", fill=(255, 40, 20, 255))
    return img


# ─── Pintura 05 — Tormenta Climática ─────────────────────────────────────────
def p05():
    img, draw = nueva()
    grad_v(draw, 0, H, (4, 0, 18), (38, 10, 65))
    cx, cy = W // 2, H // 2 - 15
    # Bandas del huracán (anillos concéntricos degradados)
    for r in range(140, 8, -12):
        alpha = min(255, 50 + int((140 - r) * 1.5))
        g_val = 100 + int((140 - r) * 0.9)
        band = Image.new('RGBA', (W, H), (0, 0, 0, 0))
        bd = ImageDraw.Draw(band)
        bd.ellipse([cx - r, cy - int(r * 0.62), cx + r, cy + int(r * 0.62)],
                   outline=(g_val, g_val, g_val + 40, alpha), width=4)
        img.paste(band, (0, 0), band)
    draw2 = ImageDraw.Draw(img)
    draw2.ellipse([cx - 16, cy - 11, cx + 16, cy + 11], fill=(70, 30, 110, 255))
    # Rayos eléctricos
    def rayo(x1, y1, x2, y2, col):
        mx = (x1 + x2) // 2 + 18
        my = (y1 + y2) // 2
        draw2.line([(x1, y1), (mx, my), (x2, y2)], fill=col, width=3)

    rayo(55, 95, 125, 195, (255, 210, 0, 255))
    rayo(375, 75, 310, 185, (255, 230, 50, 255))
    rayo(415, 255, 475, 315, (255, 190, 0, 255))
    rayo(45, 285, 115, 340, (255, 220, 50, 255))
    rayo(200, 30, 180, 90, (255, 240, 120, 255))
    # Tinte violeta profundo
    overlay(img, (40, 0, 80, 40))
    return img


# ─── Pintura 06 — CO₂ Sin Control ────────────────────────────────────────────
def p06():
    img, draw = nueva()
    # Mapa de calor: azul frío → rojo caliente
    for x in range(W):
        t = x / (W - 1)
        if t < 0.45:
            c = lc((0, 80, 200), (240, 220, 0), t / 0.45)
        else:
            c = lc((240, 220, 0), (220, 0, 0), (t - 0.45) / 0.55)
        draw.line([(x, 0), (x, int(H * 0.72))], fill=c + (255,))
    grad_v(draw, int(H * 0.72), H, (25, 18, 12), (15, 10, 6))
    # Chimeneas y humo
    chimneys = [(70, int(H * 0.20), 38), (160, int(H * 0.16), 32),
                (280, int(H * 0.21), 40), (370, int(H * 0.14), 35),
                (450, int(H * 0.22), 30)]
    for cx2, cy2, cw in chimneys:
        draw.rectangle([cx2, cy2, cx2 + cw, int(H * 0.72)], fill=(18, 12, 8, 255))
        for i in range(5):
            r = 18 + i * 13
            sy = cy2 - 25 - i * 26
            alpha = max(0, 190 - i * 35)
            smoke = Image.new('RGBA', (W, H), (0, 0, 0, 0))
            sd = ImageDraw.Draw(smoke)
            sd.ellipse([cx2 + cw // 2 - r, sy - r, cx2 + cw // 2 + r, sy + r],
                       fill=(155, 145, 135, alpha))
            img.paste(smoke, (0, 0), smoke)
    # Valor de CO₂ (diseño gráfico)
    draw2 = ImageDraw.Draw(img)
    for dx in range(-2, 3):
        for dy in range(-2, 3):
            draw2.text((W // 2 - 52 + dx, 14 + dy), "417 ppm CO₂",
                       fill=(100, 0, 0, 200))
    draw2.text((W // 2 - 52, 14), "417 ppm CO₂", fill=(255, 55, 25, 255))
    return img


# ─── Pintura 07 — Energía del Viento ─────────────────────────────────────────
def p07():
    img, draw = nueva()
    grad_v(draw, 0, int(H * 0.82), (90, 175, 240), (195, 228, 255))
    grad_v(draw, int(H * 0.82), H, (35, 115, 50), (18, 72, 22))
    # Líneas de viento
    rng = random.Random(7)
    for i in range(14):
        y = 30 + i * 24
        x1 = rng.randint(0, 120)
        length = rng.randint(60, 140)
        draw.line([(x1, y), (x1 + length, y)], fill=(160, 210, 245, 255), width=2)
    # Turbina principal
    tx, ty_base = W // 2 + 50, int(H * 0.82)
    tower_h = 195
    # Torre (troncocónica)
    draw.polygon([(tx - 6, ty_base), (tx + 6, ty_base),
                  (tx + 3, ty_base - tower_h), (tx - 3, ty_base - tower_h)],
                 fill=(190, 200, 210, 255))
    hub_x, hub_y = tx, ty_base - tower_h
    # Góndola
    draw.ellipse([hub_x - 13, hub_y - 12, hub_x + 13, hub_y + 5], fill=(175, 185, 196, 255))
    # 3 palas en estrella
    for angle_deg in [90, 210, 330]:
        a = math.radians(angle_deg)
        length_pala = 88
        ex = int(hub_x + math.cos(a) * length_pala)
        ey = int(hub_y + math.sin(a) * length_pala)
        perp = math.radians(angle_deg + 90)
        p1 = (int(hub_x + math.cos(perp) * 7), int(hub_y + math.sin(perp) * 7))
        p2 = (int(hub_x - math.cos(perp) * 7), int(hub_y - math.sin(perp) * 7))
        draw.polygon([p1, p2, (ex, ey)], fill=(238, 243, 248, 255))
    # Sol
    draw.ellipse([W - 85, 18, W - 25, 78], fill=(255, 245, 120, 255))
    # Turbinas de fondo (más pequeñas)
    for stx, sth in [(80, 100), (160, 120), (400, 90)]:
        sty_base = int(H * 0.82)
        draw.line([(stx, sty_base), (stx, sty_base - sth)], fill=(180, 195, 208, 255), width=3)
        for sad in [90, 210, 330]:
            sa = math.radians(sad)
            draw.line([(stx, sty_base - sth),
                       (int(stx + math.cos(sa) * 40), int(sty_base - sth + math.sin(sa) * 40))],
                      fill=(220, 230, 238, 255), width=2)
    return img


# ─── Pintura 08 — Poder del Sol ──────────────────────────────────────────────
def p08():
    img, draw = nueva()
    grad_v(draw, 0, H, (255, 215, 0), (165, 60, 0))
    # Rayos radiales desde el sol
    sun_x, sun_y = W // 2, 35
    for i in range(24):
        a = math.radians(i * 15)
        r1, r2 = 60, 130
        x1 = sun_x + int(math.cos(a) * r1)
        y1 = sun_y + int(math.sin(a) * r1)
        x2 = sun_x + int(math.cos(a) * r2)
        y2 = sun_y + int(math.sin(a) * r2)
        draw.line([(x1, y1), (x2, y2)], fill=(255, 255, 200, 255), width=4)
    # Disco solar
    draw.ellipse([sun_x - 52, sun_y - 52, sun_x + 52, sun_y + 52], fill=(255, 255, 220, 255))
    draw.ellipse([sun_x - 44, sun_y - 44, sun_x + 44, sun_y + 44], fill=(255, 255, 180, 255))
    # Paneles solares en perspectiva
    pw2, ph2 = 55, 38
    for row in range(3):
        for col in range(5):
            x = 28 + col * (pw2 + 10)
            y = int(H * 0.5) + row * (ph2 + 8)
            draw.rectangle([x, y, x + pw2, y + ph2], fill=(8, 18, 55, 255),
                           outline=(45, 75, 140, 255), width=1)
            # Reflexión interna
            draw.line([(x + 6, y + 7), (x + pw2 - 6, y + 7)], fill=(60, 100, 185, 255), width=2)
            draw.line([(x + 6, y + 14), (x + pw2 - 6, y + 14)], fill=(50, 85, 165, 255), width=2)
            draw.line([(x + 6, y + 21), (x + pw2 - 6, y + 21)], fill=(40, 70, 150, 255), width=2)
    # Reflejo dorado sobre los paneles
    overlay(img, (255, 180, 0, 25))
    return img


# ─── Pintura 09 — Movilidad Limpia ───────────────────────────────────────────
def p09():
    img, draw = nueva()
    grad_v(draw, 0, H, (4, 18, 38), (8, 55, 75))
    # Líneas de velocidad (teal neón)
    rng = random.Random(9)
    for i in range(22):
        y = 20 + i * 17
        x1 = rng.randint(0, 80)
        length = rng.randint(80, 220)
        alpha = rng.randint(60, 160)
        streak = Image.new('RGBA', (W, H), (0, 0, 0, 0))
        sd = ImageDraw.Draw(streak)
        sd.line([(x1, y), (x1 + length, y)], fill=(0, 195, 215, alpha), width=2)
        img.paste(streak, (0, 0), streak)
    draw2 = ImageDraw.Draw(img)
    # Carrocería del auto (silueta estilizada)
    cy2 = H // 2 + 25
    draw2.rectangle([W // 2 - 155, cy2 + 12, W // 2 + 155, cy2 + 38], fill=(235, 242, 252, 255))
    draw2.polygon([(W // 2 - 95, cy2 + 12), (W // 2 + 95, cy2 + 12),
                   (W // 2 + 75, cy2 - 38), (W // 2 - 75, cy2 - 38)],
                  fill=(220, 232, 248, 255))
    # Ventana
    draw2.polygon([(W // 2 - 60, cy2 + 10), (W // 2 + 60, cy2 + 10),
                   (W // 2 + 45, cy2 - 30), (W // 2 - 45, cy2 - 30)],
                  fill=(10, 100, 160, 255))
    # Ruedas
    for wx in [W // 2 - 100, W // 2 + 100]:
        draw2.ellipse([wx - 28, cy2 + 22, wx + 28, cy2 + 55], fill=(18, 18, 25, 255))
        draw2.ellipse([wx - 14, cy2 + 30, wx + 14, cy2 + 47], fill=(90, 95, 110, 255))
    # Rayo eléctrico cyan neón
    bolt = [(W * 3 // 4, H // 5), (W * 3 // 4 + 28, H // 2 - 15),
            (W * 3 // 4 + 5, H // 2 - 15), (W * 3 // 4 + 35, H * 3 // 4)]
    draw2.polygon(bolt, fill=(0, 230, 255, 255))
    # Halo del rayo
    glow = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    gd = ImageDraw.Draw(glow)
    gd.polygon(bolt, fill=(0, 200, 255, 70))
    img.paste(glow, (0, 0), glow)
    return img


# ─── Pintura 10 — Bosques Restaurados ────────────────────────────────────────
def p10():
    img, draw = nueva()
    grad_v(draw, 0, int(H * 0.65), (170, 225, 75), (55, 155, 25))
    grad_v(draw, int(H * 0.65), H, (28, 75, 10), (10, 38, 4))
    # Rayos de sol desde arriba
    sun_x2, sun_y2 = W // 2, -10
    for i in range(-40, 41, 7):
        a = math.radians(i + 90)
        r2 = 380
        gx2 = sun_x2 + int(math.cos(a) * r2)
        gy2 = sun_y2 + int(math.sin(a) * r2)
        ray = Image.new('RGBA', (W, H), (0, 0, 0, 0))
        rd = ImageDraw.Draw(ray)
        rd.line([(sun_x2, sun_y2), (gx2, gy2)], fill=(255, 255, 200, 35), width=22)
        img.paste(ray, (0, 0), ray)
    draw2 = ImageDraw.Draw(img)
    # Árboles con siluetas ricas
    trees_data = [(65, int(H * 0.5), 26), (140, int(H * 0.48), 31),
                  (215, int(H * 0.52), 24), (295, int(H * 0.47), 29),
                  (370, int(H * 0.50), 27), (445, int(H * 0.49), 25)]
    for tx3, ty3, tr3 in trees_data:
        # Tronco
        draw2.rectangle([tx3 - 5, ty3, tx3 + 5, ty3 + int(H * 0.22)], fill=(70, 44, 16, 255))
        # Copa (capas de verde)
        for layer, (dy3, rmul, shade) in enumerate([(0, 1.3, 0), (-12, 1.0, 18), (-22, 0.75, 35)]):
            g3 = (24 + shade, 95 + shade, 8 + shade, 255)
            r3 = int(tr3 * rmul)
            draw2.ellipse([tx3 - r3, ty3 + dy3 - r3, tx3 + r3, ty3 + dy3 + r3], fill=g3)
    # Pájaros (V)
    for bx3, by3 in [(110, 55), (175, 38), (340, 68), (420, 44)]:
        draw2.line([(bx3, by3), (bx3 + 10, by3 - 6)], fill=(8, 28, 4, 255), width=2)
        draw2.line([(bx3 + 10, by3 - 6), (bx3 + 20, by3)], fill=(8, 28, 4, 255), width=2)
    return img


# ─── Pintura 11 — Acuerdos Globales ──────────────────────────────────────────
def p11():
    img, draw = nueva()
    grad_v(draw, 0, H, (4, 12, 48), (18, 38, 95))
    # Estrellas de fondo
    rng = random.Random(11)
    for _ in range(80):
        sx = rng.randint(0, W)
        sy = rng.randint(0, H)
        br = rng.randint(140, 255)
        draw.ellipse([sx, sy, sx + 2, sy + 2], fill=(br, br, br, 255))
    draw2 = ImageDraw.Draw(img)
    # Globo terráqueo
    gx4, gy4, gr4 = W // 2, H // 2 - 10, 108
    # Resplandor
    for extra in [35, 22, 11]:
        glow = Image.new('RGBA', (W, H), (0, 0, 0, 0))
        gd4 = ImageDraw.Draw(glow)
        gd4.ellipse([gx4 - gr4 - extra, gy4 - gr4 - extra,
                     gx4 + gr4 + extra, gy4 + gr4 + extra],
                    fill=(18, 55, 195, 22))
        img.paste(glow, (0, 0), glow)
    draw2.ellipse([gx4 - gr4, gy4 - gr4, gx4 + gr4, gy4 + gr4],
                  fill=(28, 75, 178, 255), outline=(90, 175, 255, 255), width=2)
    # Meridianos
    for lon in range(-3, 4):
        mx4 = gx4 + int(lon * gr4 / 3.5)
        dist = abs(mx4 - gx4)
        if dist < gr4:
            h_mer = int(math.sqrt(gr4 * gr4 - dist * dist))
            draw2.line([(mx4, gy4 - h_mer), (mx4, gy4 + h_mer)],
                       fill=(75, 145, 225, 255), width=1)
    # Paralelos
    for lat in range(-2, 3):
        ry4 = int(lat * gr4 / 3)
        rx4 = int(math.sqrt(max(0, gr4 * gr4 - ry4 * ry4)))
        if rx4 > 4:
            draw2.ellipse([gx4 - rx4, gy4 + ry4 - 3, gx4 + rx4, gy4 + ry4 + 3],
                          outline=(75, 145, 225, 255), width=1)
    # Laureles dorados a los lados
    for side, xs4 in [(-1, gx4 - gr4 - 28), (1, gx4 + gr4 + 10)]:
        for i in range(6):
            lx4 = xs4 + side * i * 3
            ly4 = gy4 - 22 + i * 9
            draw2.ellipse([lx4, ly4, lx4 + 14, ly4 + 7], fill=(195, 158, 18, 255))
    return img


# ─── Pintura 12 — Ártico Renacido ────────────────────────────────────────────
def p12():
    img, draw = nueva((0, 4, 18))
    # Cielo estrellado
    rng = random.Random(12)
    for _ in range(130):
        sx = rng.randint(0, W)
        sy = rng.randint(0, int(H * 0.62))
        br = rng.randint(130, 255)
        sz = rng.randint(1, 3)
        draw.ellipse([sx, sy, sx + sz, sy + sz], fill=(br, br, br, 255))
    # Aurora boreal (bandas sinusoidales multicapa)
    auroras_data = [
        ((0,  200, 110, 170), H * 0.18, 28),
        ((175,  0, 200, 140), H * 0.27, 22),
        ((0,  175, 200, 155), H * 0.35, 18),
        ((180, 230,   0, 115), H * 0.42, 15),
    ]
    for (r5, g5, b5, alpha5), base_y5, amp5 in auroras_data:
        band = Image.new('RGBA', (W, H), (0, 0, 0, 0))
        bd5 = ImageDraw.Draw(band)
        for x5 in range(W):
            wave5 = math.sin(x5 * 0.014 + base_y5 * 0.09) * amp5
            yc = int(base_y5 + wave5)
            for dy5 in range(-16, 17):
                fade5 = int(alpha5 * max(0, 1.0 - abs(dy5) / 17.0))
                cy5 = yc + dy5
                if 0 <= cy5 < H:
                    bd5.point((x5, cy5), fill=(r5, g5, b5, fade5))
        img.paste(band, (0, 0), band)
    draw2 = ImageDraw.Draw(img)
    # Montañas árticas (silueta blanca)
    mtn_pts = [(0, H)]
    mtn_def = [(0, 0.64), (55, 0.44), (110, 0.60), (175, 0.46),
               (248, 0.33), (310, 0.52), (375, 0.38), (445, 0.57),
               (512, 0.47), (512, 1.0)]
    for mx5, my5_frac in mtn_def:
        mtn_pts.append((int(mx5), int(H * my5_frac)))
    draw2.polygon(mtn_pts, fill=(215, 232, 248, 255))
    # Nieve suave en las cimas
    for mx5, my5_frac in mtn_def[:-1]:
        ty5 = int(H * my5_frac)
        draw2.ellipse([int(mx5) - 15, ty5 - 5, int(mx5) + 15, ty5 + 8], fill=(240, 248, 255, 255))
    # Luna creciente
    draw2.ellipse([W - 88, 16, W - 28, 76], fill=(238, 238, 210, 255))
    draw2.ellipse([W - 80, 10, W - 28, 70], fill=(0, 4, 18, 255))
    return img


# ─── Ejecutar ─────────────────────────────────────────────────────────────────
PINTURAS = [p01, p02, p03, p04, p05, p06, p07, p08, p09, p10, p11, p12]

print(f"Generando {len(PINTURAS)} pinturas en: {OUT}")
for i, fn in enumerate(PINTURAS, 1):
    img = fn()
    path = os.path.join(OUT, f"pintura_{i:02d}.png")
    img.convert("RGB").save(path)
    print(f"  [{i:02d}] pintura_{i:02d}.png guardada")
print("¡Listo!")
