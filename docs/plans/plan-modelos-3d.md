# Plan — Descarga e Integración de Modelos 3D Reales

> Fecha: 2026-05-16
> Objetivo: Reemplazar TODA geometría procedural (cubos/discos) por modelos 3D reales descargados
> Principio: un modelo descargado con texturas incluidas se ve 100x mejor que cubos apilados

---

## Estado Actual — Mapeo Completo

### Modelos que YA tenemos (GLB descargados)

| Archivo | Fuente | Usado en | Calidad visual | Estado |
|---------|--------|----------|---------------|--------|
| tree.glb | Kenney Nature Kit | M3_DER (árbol) | Buena — cono verde con tronco | OK |
| electric_car.glb | Kenney Car Kit | M2_DER (auto) | Buena — sedan reconocible | OK |
| building_a.glb | Kenney City Kit | M3_IZQ (edificio 1) | Buena — edificio con ventanas | OK |
| building_b.glb | Kenney City Kit | M3_IZQ (edificio 2) | Buena — edificio diferente | OK |
| building_c.glb | Kenney City Kit | M3_IZQ (edificio 3) | Buena — edificio variado | OK |
| globe.glb | KhronosGroup CarbonFibre | M5 (globo) | MALA — negro, PBR incompatible | Reemplazar |

### Geometría PROCEDURAL que necesita modelo real

| Componente | Módulo | Geometría actual | Calidad | Modelo necesario |
|-----------|--------|-----------------|---------|-----------------|
| **Iceberg** | M1_IZQ | Cubos escalados azul-blanco | Mala | Roca/iceberg low-poly |
| **Oso polar** | M2_IZQ | 5 cubos blancos (cuerpo+cabeza+patas) | Muy mala | Oso polar o oso genérico |
| **Disco de hielo** | M2_IZQ | Disc procedural | Aceptable | Puede quedarse |
| **Turbina eólica** | M1_DER | Poste + góndola + palas (cubos) | Mala | Turbina eólica |
| **Globo terráqueo** | M5 | Cubo azul + cubo verde | Muy mala | Esfera/globo terráqueo |
| **Líneas de acuerdos** | M5 | Discos procedurales | Aceptable | Puede quedarse |
| **Foca** | Fauna | 4 cubos grises | Muy mala | Foca o animal marino |
| **Zorro ártico** | Fauna | 5 cubos blancos | Muy mala | Zorro o lobo |
| **Gaviota** | Fauna | 5 cubos blancos | Muy mala | Pájaro/ave |

### Entorno del museo (geometría del edificio)

| Componente | Geometría actual | Calidad | Necesita modelo? |
|-----------|-----------------|---------|-----------------|
| Suelo | Quad con textura concreto | OK tras fix winding | No |
| Techo | Quad (winding por corregir) | Bug pendiente | Solo fix código |
| Paredes | Cubos con textura clara | Aceptable | No |
| Plataformas | Discos con textura hielo | Aceptable | No |

---

## Modelos a Descargar — 9 modelos

| # | Modelo | Para | Fuente primaria | Búsqueda | Formato |
|---|--------|------|-----------------|----------|---------|
| 1 | Iceberg/roca de hielo | M1_IZQ | Poly Haven / Kenney | "rock ice" "iceberg" | GLB |
| 2 | Oso polar | M2_IZQ | Poly Haven / GitHub | "polar bear" "bear" | GLB |
| 3 | Turbina eólica | M1_DER | Poly Haven / GitHub | "wind turbine" "windmill" | GLB |
| 4 | Globo terráqueo | M5 | GitHub / Poly Haven | "earth globe sphere" | GLB |
| 5 | Foca | Fauna | Poly Haven / GitHub | "seal" "sea lion" | GLB |
| 6 | Zorro | Fauna | Kenney / Poly Haven | "fox" "wolf" "dog" | GLB |
| 7 | Pájaro/ave | Fauna | Kenney / Poly Haven | "bird" "seagull" | GLB |
| 8 | Panel solar | M1_DER alt. | Kenney / GitHub | "solar panel" | GLB |
| 9 | Roca nevada | Decoración | Poly Haven | "rock snow" | GLB |

**Regla de corte**: 3 intentos por modelo. Si no se encuentra → buscar modelo genérico sustituto (ej: oso → cualquier animal cuadrúpedo).

---

## Fuentes de Descarga (validado post-auditoría GPT)

### Tier 1 — Descarga directa confirmada
| Fuente | Tipo | URL | Licencia |
|--------|------|-----|----------|
| **Poly Pizza** | Modelos individuales GLTF | poly.pizza/u/Quaternius | CC0 |
| **Quaternius** | Packs ZIP con GLB | quaternius.com/packs/ | CC0 |
| **Poly Haven** | Modelos + texturas | dl.polyhaven.org | CC0 |
| **Kenney.nl** | Packs ZIP con GLB | kenney.nl/assets | CC0 |

### Tier 2 — Búsqueda + descarga
| Fuente | Notas |
|--------|-------|
| **OpenGameArt** | Calidad variable, algunos GLB CC0 |
| **GitHub repos** | `site:github.com {modelo} glb CC0` |
| **KhronosGroup** | Modelos de ejemplo para testing |

### Estrategia recomendada (GPT)
- **Quaternius/Poly Pizza** → animales (oso, zorro, pájaro, foca)
- **Poly Haven** → rocas/hielo (iceberg)
- **Kenney** → props tecnológicos (turbina, panel solar)
- **Estilo**: lowpoly stylized para coherencia visual

---

## Workflow de Descarga e Integración

```
Por cada modelo:

1. BUSCAR
   - WebSearch en fuentes Tier 1 primero
   - Si no hay → Tier 2 → Tier 3
   - Máximo 3 intentos

2. DESCARGAR
   - curl -L -o /tmp/model.glb (o .zip → unzip → find .glb)
   - Verificar tamaño (>1KB, <15MB)
   - Mover a app/assets/models/{nombre}.glb

3. INTEGRAR EN CÓDIGO
   - Agregar Model member en ModuleScene.h
   - Cargar en init()
   - Reemplazar función draw procedural por model.draw()
   - Ajustar escala y posición

4. VERIFICAR VISUALMENTE
   - Build + ejecutar --test-module {módulo}
   - Screenshot frontal
   - Evaluar: ¿el modelo es reconocible? ¿escala correcta? ¿colores bien?
   - Si no → ajustar escala/rotación/posición
   - Si negro/invisible → verificar normales, desactivar cull face

5. SIGUIENTE MODELO
   - Solo avanzar cuando el actual se ve bien
```

---

## Orden de Prioridad (actualizado post-auditoría)

1. **Globo terráqueo** (M5) — cubo negro/azul, máximo impacto al corregir
2. **Iceberg/roca** (M1_IZQ) — fácil de encontrar en Poly Haven, alto impacto
3. **Oso polar** (M2_IZQ) — Quaternius/Poly Pizza, el más icónico del proyecto
4. **Turbina eólica** (M1_DER) — Kenney, comunicar "energía renovable"
5. **Roca nevada** (decoración) — Poly Haven, mejora ambiental inmediata
6. **Pájaro** (fauna) — Quaternius, fácil
7. **Zorro ártico** (fauna) — Quaternius, puede ser wolf blanco
8. **Foca** (fauna) — el más difícil CC0, puede ser pez/animal marino genérico

---

## Fuentes Específicas a Explorar

### Quaternius (quaternius.com) — Packs CC0
Quaternius tiene packs gratuitos de:
- **Ultimate Animals** — incluye oso, zorro, foca, pájaros (low-poly, CC0)
- **Ultimate Nature** — rocas, árboles
- Formato: GLB/FBX/OBJ

### Kenney Packs Adicionales
- **Space Kit** — podría tener esferas para globo
- **Platformer Kit** — animales simples
- **Holiday Kit** — figuras

### Poly Haven Models
- Rocas realistas (para iceberg)
- Props industriales (para turbina)
