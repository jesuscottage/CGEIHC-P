"""
Genera modelos OBJ low-poly temáticos para el museo de calentamiento global.
Sin dependencias externas — solo math, os.
Assimp calcula normales por cara automáticamente (flat shading = look low-poly).
"""
import math, os

OUT = os.path.normpath(os.path.join(os.path.dirname(__file__),
      '..', 'app', 'assets', 'models'))

# ── Utilidades ────────────────────────────────────────────────────────────────

def ring(y, r, n, a0=0.0):
    return [(r*math.cos(a0+2*math.pi*i/n), y, r*math.sin(a0+2*math.pi*i/n))
            for i in range(n)]

def tris_from_quads(faces):
    out = []
    for f in faces:
        if len(f) == 3:
            out.append(f)
        else:
            a,b,c,d = f
            out.append((a,b,c)); out.append((a,c,d))
    return out

def side_strip(r0, r1, n):
    """Quads between two rings of same n, 0-indexed offsets r0,r1."""
    f = []
    for i in range(n):
        j = (i+1)%n
        f.append((r0+i, r0+j, r1+j, r1+i))
    return f

def top_fan(apex, ring_start, n, flip=False):
    f = []
    for i in range(n):
        j = (i+1)%n
        f.append((apex, ring_start+j, ring_start+i) if flip else
                 (apex, ring_start+i, ring_start+j))
    return f

def sphere_vf(cx,cy,cz, r, rows=5, cols=8):
    """Esfera low-poly: filas×columnas. Devuelve (verts_locales, tris_locales)."""
    V=[]; F=[]
    V.append((cx,cy+r,cz))                          # ápice superior (0)
    for ri in range(1,rows):
        phi=math.pi*ri/rows
        yr=cy+r*math.cos(phi); rr=r*math.sin(phi)
        V+=[(cx+rr*math.cos(2*math.pi*ci/cols), yr,
             cz+rr*math.sin(2*math.pi*ci/cols)) for ci in range(cols)]
    V.append((cx,cy-r,cz))                          # ápice inferior
    # Tapa superior
    F+=top_fan(0,1,cols)
    # Franjas laterales
    for ri in range(rows-2):
        F+=side_strip(1+ri*cols, 1+(ri+1)*cols, cols)
    # Tapa inferior
    bot=len(V)-1; lr=1+(rows-2)*cols
    F+=top_fan(bot,lr,cols,flip=True)
    return V, tris_from_quads(F)

def merge(pieces):
    """Fusiona [(verts,tris)] en un único (verts,tris) reindexado."""
    V=[]; T=[]
    for verts,tris in pieces:
        off=len(V)
        V+=verts
        T+=[(a+off,b+off,c+off) for a,b,c in tris]
    return V,T

def save(name, V, T):
    path=os.path.join(OUT,name+'.obj')
    with open(path,'w') as f:
        f.write(f"# low-poly {name} — CGEIHC-P\n")
        for v in V: f.write(f"v {v[0]:.5f} {v[1]:.5f} {v[2]:.5f}\n")
        for t in T: f.write(f"f {t[0]+1} {t[1]+1} {t[2]+1}\n")
    print(f"  {name}.obj  {len(V)}v {len(T)}f")

# ── 1. PINGÜINO ───────────────────────────────────────────────────────────────
# Cuerpo 24 segmentos, cabeza esférica 18×12, aletas con grosor, pico cónico.
def make_penguin():
    pieces = []

    S = 24  # segmentos cuerpo
    HS = 18 # segmentos cabeza

    # ── Cuerpo (anillos apilados) ──
    body_rings = [
        (0.00, 0.00),  # ápice inferior
        (0.03, 0.10),
        (0.08, 0.20),
        (0.16, 0.30),
        (0.28, 0.36),  # panza máxima
        (0.40, 0.33),
        (0.50, 0.26),  # hombros
        (0.58, 0.19),  # cuello
    ]
    V=[]; F=[]
    V.append((0,0,0)); apex_bot=0
    for i,(y,r) in enumerate(body_rings[1:]):
        start=len(V); V+=ring(y,r,S)
        if i==0: F+=top_fan(apex_bot,start,S)
        else:    F+=side_strip(start-S,start,S)
    pieces.append((V, tris_from_quads(F)))

    # ── Cabeza: esfera aplanada ──
    hcy=0.72; hcr=0.21
    hV, hF = sphere_vf(0, hcy, 0, hcr, rows=8, cols=HS)
    pieces.append((hV, hF))

    # ── Pico: cono de 8 lados apuntando +Z ──
    bk_r=0.04; bk_h=0.09; bk_y=0.72; bk_z=0.21
    bV=[]; bF=[]
    bV.append((0, bk_y, bk_z+bk_h))           # punta
    bV += ring(bk_y, bk_r, 8, a0=0)           # base circular
    for i in range(8):
        j=(i+1)%8; bF.append((0,1+i,1+j))
    # tapa base
    for i in range(1,7): bF.append((1,1+i+1,1+i))
    pieces.append((bV, bF))

    # ── Aletas con grosor: quad extruido ──
    for sx in [-1,1]:
        ax=sx*0.32; ay=0.38; aw=0.26; ah=0.18; tk=0.04
        wV=[]; wF=[]
        # 8 vértices del prisma aplastado
        wV += [(ax,      ay,    -aw*0.5),(ax,      ay,     aw*0.5),
               (ax+sx*aw,ay-ah,-aw*0.3),(ax+sx*aw,ay-ah,  aw*0.3)]
        # repetir desplazados en X para dar grosor
        for x,y,z in wV[:4]: wV.append((x+sx*tk,y,z))
        # caras laterales
        quads=[(0,1,3,2),(4,6,7,5),(0,4,5,1),(2,3,7,6),(0,2,6,4),(1,5,7,3)]
        for q in quads: wF.append(q)
        pieces.append((wV, tris_from_quads(wF)))

    save("penguin", *merge(pieces))

# ── 2. BARRIL DE PETRÓLEO ─────────────────────────────────────────────────────
def make_barrel():
    V=[]; F=[]
    S=10; h=0.90; r=0.32; rb=0.28  # rb: radio base (tapas)

    # Anillos: base, banda inf, panza, banda sup, tapa
    rings=[
        (0.00, rb),
        (0.04, r),
        (0.10, r*1.05),
        (0.45, r*1.08),   # panza (ligeramente más ancha)
        (0.80, r*1.05),
        (0.86, r),
        (0.90, rb),
    ]
    V.append((0,0,0)); apex_bot=0     # centro tapa inferior
    for yi,(y,ri) in enumerate(rings):
        start=len(V); V+=ring(y,ri,S)
        if yi==0:
            F+=top_fan(apex_bot,start,S)
        else:
            F+=side_strip(start-S,start,S)
    # Tapa superior
    apex_top=len(V); V.append((0,h,0))
    F+=top_fan(apex_top,len(V)-1-S,S,flip=True)

    # Anillo metálico decorativo al centro (franja elevada)
    for band_y in [0.20,0.70]:
        bs=len(V); V+=ring(band_y,r*1.10,S)
        bs2=len(V); V+=ring(band_y+0.03,r*1.10,S)
        F+=side_strip(bs,bs2,S)

    save("barrel",V,tris_from_quads(F))

# ── 3. IGLÚ ───────────────────────────────────────────────────────────────────
def make_igloo():
    V=[]; F=[]
    R=1.40; rows=5; cols=10

    # Media esfera (domo)
    V.append((0,R,0)); apex=0           # ápice
    for ri in range(1,rows+1):
        phi=math.pi*0.5*ri/rows         # solo mitad superior (0..π/2)
        yr=R*math.cos(phi); rr=R*math.sin(phi)
        V+=ring(0,rr,cols)
        # último anillo: base en Y=0
        V[-cols:]= [(x,0,z) for x,y,z in V[-cols:]]
        if ri==1:
            F+=top_fan(apex,1,cols)
        else:
            F+=side_strip(len(V)-cols*2,len(V)-cols,cols)

    # Suelo (disco)
    gc=len(V); V.append((0,-0.02,0))
    base_ring=1+(rows-1)*cols
    F+=top_fan(gc,base_ring,cols,flip=True)

    # Entrada (túnel rectangular bajo)
    tw=0.40; th=0.65; td=0.60
    tx=0; tz=R+td*0.5
    ev=len(V)
    V+=[(tx-tw,0,tz-td),(tx+tw,0,tz-td),
        (tx+tw,th,tz-td),(tx-tw,th,tz-td),
        (tx-tw,0,tz+td),(tx+tw,0,tz+td),
        (tx+tw,th,tz+td),(tx-tw,th,tz+td)]
    F+=[(ev+0,ev+1,ev+2,ev+3),   # pared trasera
        (ev+4,ev+7,ev+6,ev+5),   # pared delantera
        (ev+0,ev+4,ev+5,ev+1),   # suelo
        (ev+3,ev+2,ev+6,ev+7),   # techo
        (ev+0,ev+3,ev+7,ev+4),   # lado izq
        (ev+1,ev+5,ev+6,ev+2)]   # lado der

    save("igloo",V,tris_from_quads(F))

# ── 4. CHIMENEA INDUSTRIAL ────────────────────────────────────────────────────
def make_chimney():
    pieces=[]
    S=6

    # Torre: prisma hexagonal alto y estrecho
    tw=0.30; th=3.50
    V=[]; F=[]
    V.append((0,0,0)); V+=ring(0,tw,S)
    F+=top_fan(0,1,S,flip=True)      # base inferior
    V+=ring(th,tw*0.85,S)
    F+=side_strip(1,1+S,S)
    apex=len(V); V.append((0,th,0))
    F+=top_fan(apex,1+S,S)           # tapa superior (abierta visualmente)
    pieces.append((V,tris_from_quads(F)))

    # Nube de humo: 3 esferas desplazadas sobre la chimenea
    for sx,sy,sr in [(0,3.9,0.32),(0.28,4.3,0.26),(-0.22,4.6,0.22)]:
        sv,sf=sphere_vf(sx,sy,0, sr, rows=4, cols=6)
        pieces.append((sv,sf))

    save("chimney",*merge(pieces))

# ── 5. MUÑECO DE NIEVE (derritiéndose) ───────────────────────────────────────
def make_snowman():
    pieces=[]
    # 3 esferas: base (grande), cuerpo (mediana), cabeza (pequeña)
    for cx,cy,cz,r in [(0,0.42,0,0.42),(0,1.10,0,0.30),(0,1.64,0,0.23)]:
        sv,sf=sphere_vf(cx,cy,cz,r,rows=5,cols=8)
        pieces.append((sv,sf))

    # Charco de nieve derretida (disco aplastado en la base)
    V=[]; F=[]
    S=12; pr=0.70
    V.append((0,0.01,0)); V+=ring(0.01,pr,S)
    F+=top_fan(0,1,S); V+=ring(0.01,pr*0.8,S)
    F+=side_strip(1,1+S,S)
    pieces.append((V,tris_from_quads(F)))

    # Sombrero: disco + cilindro corto
    V2=[]; F2=[]
    brim_r=0.22; hat_r=0.14; hat_h=0.20; hat_y=1.84
    V2.append((0,hat_y,0)); V2+=ring(hat_y,brim_r,10)
    F2+=top_fan(0,1,10)
    V2+=ring(hat_y,brim_r,10)
    V2+=ring(hat_y+0.04,hat_r,10)
    F2+=side_strip(len(V2)-20,len(V2)-10,10)
    V2+=ring(hat_y+hat_h,hat_r,10)
    F2+=side_strip(len(V2)-20,len(V2)-10,10)
    apex2=len(V2); V2.append((0,hat_y+hat_h,0))
    F2+=top_fan(apex2,len(V2)-1-10,10,flip=True)
    pieces.append((V2,tris_from_quads(F2)))

    save("snowman",*merge(pieces))

# ── 6. FARO ÁRTICO ────────────────────────────────────────────────────────────
def make_lighthouse():
    V=[]; F=[]
    S=8

    # Torre principal (cónica ligeramente)
    tower=[
        (0.00,0.55),(0.50,0.52),(1.00,0.50),
        (1.50,0.48),(2.00,0.46),(2.50,0.44),
        (3.00,0.42),(3.20,0.55),  # balcón
        (3.25,0.55),(3.25,0.40),  # cuerpo de balcón
        (3.60,0.40),(3.80,0.30),  # linterna
        (3.90,0.00),              # ápice
    ]
    apex_bot=len(V); V.append((0,0,0))
    for idx,(y,r) in enumerate(tower):
        start=len(V); V+=ring(y,r,S)
        if idx==0: F+=top_fan(apex_bot,start,S,flip=True)
        else:       F+=side_strip(start-S,start,S)
    F+=top_fan(len(V)-1,len(V)-1-S,S)  # tapa cónica top

    # Luz: pequeña esfera en la cima
    lv,lf=sphere_vf(0,4.10,0,0.25,rows=4,cols=6)
    all_v=V+lv
    all_f=tris_from_quads(F)+[(a+len(V),b+len(V),c+len(V)) for a,b,c in lf]
    save("lighthouse",all_v,all_f)

# ── 7. NARVAL ─────────────────────────────────────────────────────────────────
def make_narwhal():
    pieces=[]
    S=8
    # Cuerpo: fusiforme (anillos apilados en X)
    body=[
        (-1.20,0.00),(-0.90,0.18),(-0.50,0.28),
        ( 0.00,0.32),( 0.40,0.28),( 0.70,0.22),
        ( 0.90,0.16),( 1.10,0.08),( 1.20,0.00),
    ]
    V=[]; F=[]
    apex_back=0; V.append((-1.20,0,0))
    for idx,(x,r) in enumerate(body[1:]):
        start=len(V); V+=[(x, r*math.cos(2*math.pi*i/S),
                              r*math.sin(2*math.pi*i/S)) for i in range(S)]
        if idx==0: F+=top_fan(apex_back,start,S)
        else:       F+=side_strip(start-S,start,S)
    apex_front=len(V); V.append((1.20,0,0))
    F+=top_fan(apex_front,len(V)-1-S,S,flip=True)

    # Cuerno (espiral simplificada): cono largo
    horn=[
        (-1.25,0.06),(-1.45,0.05),(-1.65,0.04),
        (-1.85,0.03),(-2.05,0.02),(-2.20,0.00),
    ]
    hs=4  # 4 lados para el cuerno
    V2=[]; F2=[]
    apex_hbase=0; V2.append((-1.20,0.32,0))
    for hx,hr in horn:
        hs_start=len(V2)
        V2+=[(hx, 0.32+hr*math.cos(2*math.pi*i/hs),
                   hr*math.sin(2*math.pi*i/hs)) for i in range(hs)]
        if hs_start==1: F2+=top_fan(0,1,hs)
        else:            F2+=side_strip(hs_start-hs,hs_start,hs)
    apex_htip=len(V2); V2.append((-2.20,0.32,0))
    F2+=top_fan(apex_htip,len(V2)-1-hs,hs,flip=True)

    # Aleta dorsal
    fv=len(V2)
    V2+=[(-0.20,0.32,0),(-0.40,0.65,0),(0.10,0.32,0)]
    F2.append((fv,fv+1,fv+2))

    pieces.append((V,tris_from_quads(F)))
    pieces.append((V2,tris_from_quads(F2)))
    save("narwhal",*merge(pieces))

# ── Ejecutar ──────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    os.makedirs(OUT, exist_ok=True)
    print("Generando modelos low-poly en:", OUT)
    make_barrel()
    make_igloo()
    make_chimney()
    make_snowman()
    make_lighthouse()
    make_narwhal()
    make_penguin()
    print("Listo.")
