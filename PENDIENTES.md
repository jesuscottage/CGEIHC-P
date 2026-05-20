# Pendientes del Proyecto — CGEIHC-P

> Proyecto: Calentamiento global en el polo norte
> Última actualización: 2026-05-20
> Estado general: **Fase 15 completa — Galería en todas las paredes + Tesla Model S real con rotación**

---

## Cómo compilar (para compañeros que clonen el repo)

```powershell
# 1. Configurar (solo primera vez)
& "C:\Program Files\CMake\bin\cmake.exe" -S app -B build -G "Visual Studio 18 2026"

# 2. Compilar
& "C:\Program Files\CMake\bin\cmake.exe" --build build --config Release --target CGEIHC --parallel

# 3. Ejecutar
.\build\Release\CGEIHC.exe
```

**Requisitos**: CMake 4.x (`winget install Kitware.CMake`), Visual Studio 2026 Community con C++.  
El resto de dependencias (GLFW, GLM, Assimp, ImGui) se descargan automáticamente con FetchContent.

---

## Controles del Juego

| Tecla | Acción |
|-------|--------|
| `WASD` | Moverse por el museo |
| Ratón | Girar la cámara |
| `E` | Activar animación del módulo cercano + abrir popup narrativo |
| `TAB` | Cerrar popup narrativo y detener narración |
| `ESC` | Salir de la aplicación |
| `ESPACIO` | Pasar la pantalla de título |

---

## Estado Actual

### Modelos 3D integrados en DecoScene

| Modelo | Origen | Textura | Uso en escena |
|--------|--------|---------|---------------|
| pine_snow.glb | Quaternius CC0 | Embebida | M3_DER árbol que crece |
| electric_car.glb | Kenney CC0 | Color override azul | M2_DER auto eléctrico (fallback si ModelS.obj no carga) |
| ModelS.obj | Blender v2.79 (manual) | Color override azul fijo | M2_DER Tesla Model S principal — 115 objetos, escala 1:1 m |
| building_a/b/c.glb | Kenney CC0 | Color override concreto | M3_IZQ ciudad inundada |
| globe.glb | KhronosGroup CC0 | Embebida (metal) | M5 globo giratorio |
| iceberg.glb | Quaternius CC0 | Color override azul hielo | M1_IZQ iceberg derritiéndose |
| polar_bear.glb | Quaternius CC0 | Color override crema | M2_IZQ oso polar sobre hielo |
| fox.glb | KhronosGroup CC0 | Embebida | Fauna decorativa (zorro) |
| seagull.glb | three.js (Stork) | Color override blanco | Fauna decorativa (ave) |
| seal.glb | Quaternius CC0 | Color override gris | Fauna decorativa (foca) |
| whale.glb | Quaternius CC0 | Color override azul-gris | Fauna decorativa (ballena) |
| rock.glb | Quaternius CC0 | Color override gris | Decoración rocas (4 posiciones) |
| tree.glb | Kenney CC0 | Sin textura (legacy) | No usado (reemplazado por pine_snow) |
| igloo_real.obj | 3ds Max (CC0) | MTL nieve (0.88,0.93,0.98) | Vestíbulo, decoración estructura |
| wolf.glb | Descarga manual | Color override gris-café | Lobos patrullando corredores y vestíbulo |
| broken_pole.fbx | Blender 2.91 (CC0) | Color override marrón | Postes rotos decorativos |
| penguin.obj | Generado proceduralmente | Negro + pico naranja | Pingüinos animados por todo el museo |

### Resultados QA Visual v4

| Escena | Estado | Notas |
|--------|--------|-------|
| Vestíbulo | OK | Igloo real, lobo patrullando en X, 2 pingüinos patrullando en Z |
| M1_IZQ Iceberg | OK | Roca azul que encoge con animT. Escala 5.0 base |
| M2_IZQ Oso polar | OK | Wolf modelo crema sobre disco de hielo |
| M3_IZQ Edificios | OK | 3 edificios concreto a escala 1.8. Agua sube con animT |
| M1_DER Turbina | OK | Modelo real EolicOBJ.obj — palas 0-2 giran, estáticos 3-5, suelo omitido |
| M2_DER Auto | OK | Tesla Model S real (ModelS.obj), azul fijo, rota sobre Y al presionar E |
| M3_DER Árbol | OK | Pino nevado que crece de semilla a árbol completo |
| M5 Globo | OK | DamagedHelmet texturizado, rotación constante |
| Fauna | OK | Zorro, ave, foca, ballena, 4 rocas dispersas |
| Skybox | OK | Cubemap procedural ártico con aurora |
| Agua | OK | Shader sinusoidal + Fresnel, Y=-0.6 (no traspasa suelo) |
| Nieve | OK | 100 billboards cayendo |
| Suelo hielo | OK | Textura ice.png en todo el piso del museo |
| Corredores izq/der | OK | Lobo patrullando Z 12↔57, 2 pingüinos en rangos separados |
| Pasillo central | OK | 1 pingüino muy lento Z 12↔58 |
| Sala M5 | OK | 2 lobos en órbita circular, 5 pingüinos en semicírculo |
| Terreno exterior | OK | Grid 600×600m nevado, montículos procedurales, 12 árboles exteriores |

---

## Cambios en Fase 13 (2026-05-19)

### DecoScene — fauna animada y decoración del museo (`app/src/scene/DecoScene.h`)

Nuevo sistema de decoración ambiental con modelos reales y animación de patrulla:

#### Modelos reales integrados
- **Igloo** (`igloo_real.obj`, 5.2 MB): escala 0.35, rotación -90° en X para corregir orientación de 3ds Max. MTL custom con color nieve.
- **Lobo ártico** (`wolf.glb`, 4.3 MB): disco gris eliminado vía manipulación binaria del GLB (mesh `Circle` con `primitives` vaciado). Escala 2.2–3.0×.
- **Poste roto** (`broken_pole.fbx`, 1.6 MB): escala 0.01 (cm→m, exportado desde Blender 2.91).
- **Pingüino** (`penguin.obj`, generado): 322v/602f con cuerpo negro y pico naranja (cono `mPyram` separado, posición local `(0, 0.73, 0.22)`, rotado -90° en X).

#### Sistema de patrulla sinusoidal
```cpp
PatrolResult patrol(vec3 a, vec3 b, float speed, float phase, float t);
// s = sin(t*speed+phase): posición ∈ [a,b]
// fwd = cos(t*speed+phase): orientación (gira 180° en los extremos)
```

#### Distribución de fauna animada
| Zona | Lobos | Pingüinos |
|------|-------|-----------|
| Vestíbulo | 1 (X: -5↔+5, vel 0.4) | 2 (Z: 8↔2, vel 0.6) |
| Corredor izq | 1 (Z: 12↔57, vel 0.25) | 2 (rangos Z separados) |
| Corredor der | 1 (Z: 57↔12, vel 0.25, fase opuesta) | 2 (rangos Z separados) |
| Pasillo central | — | 1 (Z: 12↔58, vel 0.18) |
| Sala M5 | 2 (órbita circular r=9m, vel 0.3) | 5 (semicírculo, vel 0.55) |

#### Terreno exterior nevado
- Grid 600×600m (`makeGrid`) con textura `snow_albedo.png` a Y=-0.15
- Montículos procedurales (mPyram) en posiciones fijas
- 12 árboles exteriores con `drawArbolNieve()` (tronco + 3 capas piramidales + nieve)

#### Piso de hielo
- `Museum.h`: `floorTex.load("textures/ice.png")` — piso con textura de hielo en toda la T

#### Texturas nuevas
- `snow_albedo.png` (1024×1024): convertida de TIF del pack `58-landscape` con Pillow
- `snow_detail.png` (1024×1024): detalle adicional de nieve

#### Fix técnico: conflicto de nombre `time` en MSVC
- Parámetro `float time` renombrado a `float t` en todas las funciones de DecoScene
- Motivo: `<ctime>` expone `::time()` y MSVC resolvía el nombre como puntero a función en expresiones de inicialización

---

## Cambios en Fase 12 (2026-05-18)

### Sistema de Colisiones — reescrito completamente (`app/src/main.cpp`)

La función `applyCollision(pos, prevX)` divide el museo en zonas:

| Zona | Z | Colisión X |
|------|---|-----------|
| Vestíbulo | Z < 9.8 | Paredes exteriores ±9.6 únicamente |
| Corredores | 9.8 ≤ Z < 60 | Exteriores ±20 + **interiores ±4 bidireccionales** |
| Sala M5 | Z ≥ 60 | Exteriores ±20 únicamente |

- Las paredes interiores (X=±4.25) ahora bloquean en **ambas direcciones** en los corredores.
- Barrera de zona en Z=9.8: si el jugador está en un corredor (|X| > 9.25) no puede retroceder al vestíbulo (evita teleportación).
- Geometría de paredes interiores movida a Z∈[10,70] en `Museum.h` — antes empezaban en Z=0 y se podían atravesar en el vestíbulo.

### Popup Narrativo por Módulo (`app/src/main.cpp`)

- Al presionar **E** cerca de un módulo, aparece una ventana ImGui centrada con:
  - Título del módulo
  - Texto descriptivo extenso (3–4 párrafos con datos reales)
  - Indicador `[ TAB ] Cerrar`
- **TAB** cierra el popup Y detiene la narración de voz inmediatamente.
- Si se activa otro módulo con E mientras hay un popup abierto, la narración anterior se detiene y empieza la nueva.

### Narración de Voz (`app/assets/audio/`)

7 archivos MP3 generados con Microsoft Edge TTS:
- Voz: `es-ES-AlvaroNeural` (masculina, la más grave disponible en español)
- Ritmo: −15% (pausado, narrativo)
- Tono: −8 Hz (más grave)

| Archivo | Módulo |
|---------|--------|
| `narr_M1_IZQ.mp3` | Deshielo del Ártico |
| `narr_M2_IZQ.mp3` | Fauna en Peligro |
| `narr_M3_IZQ.mp3` | Ciudades Inundadas |
| `narr_M1_DER.mp3` | Energía Eólica |
| `narr_M2_DER.mp3` | Auto Eléctrico |
| `narr_M3_DER.mp3` | Captura de Carbono |
| `narr_M5.mp3` | Acuerdos por el Clima |

Para regenerar los audios (si se cambian los textos):
```powershell
pip install edge-tts
python scripts/generar_narraciones.py
```

### Input.h — tecla TAB agregada

`app/src/core/Input.h` ahora rastrea `GLFW_KEY_TAB` en el loop de update.  
Si se necesita agregar más teclas en el futuro, añadirlas al array `KEYS[]` en `Input::update()`.

---

## Cambios en Fase 15 (2026-05-20)

### Galería de arte — cobertura total de paredes (`app/src/scene/GalleryScene.h`)

#### Pinturas pegadas a las paredes (flush)
Las pinturas de los corredores estaban flotando 0.3–0.35 m frente a las paredes. Corregido usando la cara interna exacta de cada muro:

| Pared | Posición anterior | Posición corregida |
|-------|-------------------|--------------------|
| Ext. izq. (X=−20) | X=−19.70 | X=−19.99 |
| Int. izq. (X=−4.25) | X=−4.85 | X=−4.51 |
| Int. der. (X=+4.25) | X=+4.85 | X=+4.51 |
| Ext. der. (X=+20) | X=+19.70 | X=+19.99 |

#### Nuevas paredes con pinturas
Todas las paredes del museo ahora tienen cuadros enmarcados:

| Zona | Pared | Pinturas | Orientación |
|------|-------|----------|-------------|
| Corredor central | Int. izq. cara centro (X=−4.0) | 6 (01-06) | yRot=0 (normal +X) |
| Corredor central | Int. der. cara centro (X=+4.0) | 6 (07-12) | yRot=π (normal −X) |
| Cierre corredores | Pared Z=65, corredor izq. | 3 (X=−17,−12,−7) | yRot=π/2 (normal −Z) |
| Cierre corredores | Pared Z=65, corredor der. | 3 (X=7,12,17) | yRot=π/2 (normal −Z) |
| Sala M5 | Pared trasera Z=75 | 5 (X=−16,−8,0,8,16) | yRot=π/2 (normal −Z) |
| Vestíbulo | Pared frontal Z=−5 | 3 (X=−6,0,6) | yRot=−π/2 (normal +Z) |
| Vestíbulo | Pared lat. izq. X=−10 | 2 (Z=0.5, 7.5) | yRot=0 (normal +X) |
| Vestíbulo | Pared lat. der. X=+10 | 2 (Z=0.5, 7.5) | yRot=π (normal −X) |

**Total de pinturas visibles en escena**: 40 (vs. 24 anteriores)

#### Notas técnicas
- Las paredes con normal en eje Z usan `makeWallQuad` rotado con `yRot=±π/2`, convirtiendo el ancho del quad de Z a X.
- El marco (4 tiras) funciona correctamente para cualquier yRot porque `glm::translate(base, offset)` opera en espacio local.

---

### Auto Eléctrico — Tesla Model S real (`app/src/scene/ModuleScene.h`)

#### Modelo reemplazado
- **Antes**: `electric_car.glb` (Kenney, movimiento sinusoidal en Z)
- **Ahora**: `ModelS.obj` (Blender v2.79, 115 objetos, escala 1:1 en metros)
  - Fuente: `uploads_files_2035868_Model+S.obj` (descarga manual)
  - Copiado a `app/assets/models/ModelS.obj`

#### Cambios de comportamiento
| Aspecto | Antes | Ahora |
|---------|-------|-------|
| Color | Interpolación gris→azul con `animT` | Azul eléctrico fijo `(0.08, 0.38, 0.90)` |
| Animación | Movimiento sinusoidal en Z | Rotación continua sobre eje Y |
| Velocidad rotación | 20°/s (solo cuando `animT=1`) | 45°/s proporcional a `animT` |
| Meshes omitidos | 0–3 (planos de suelo del showroom) | Solo 0 (`Plane.030` — piso 37×37 m) |

#### Fórmula de rotación
```cpp
float yAngle = glm::radians(45.0f * t * time);
// t=0 → sin rotación; t=1 → 45°/s continuo
```

---

## Cambios en Fase 14 (2026-05-19)

### Modelos reales en exhibiciones (`app/src/scene/ModuleScene.h`)

#### M1_DER — Turbina Eólica real (`EolicOBJ.obj`)
- Submeshes 0-2 = palas (rotan sobre el eje Z alrededor del cubo central)
- Submeshes 3-5 = torre/góndola/base (estáticos)
- Centro de rotación calculado a partir del bounding-box del hub: `(0, 27.85, 1.3)` en espacio del modelo
- Escala 0.28×, desplazada −3.8 u en Y para caber bajo el techo (Y_base ≈ −2.1, Y_top ≈ 5.6)
- `Model::drawMesh(int index, Shader&, overrideColor*)` añadido a `Model.h` para dibujar submeshes individualmente

#### M2_DER — Tesla Model S (`ModelS.obj`, 183 MB)
- Meshes 0-3 son planos del suelo del showroom → se omiten para no pintar el piso del museo
- Rotación sinusoidal sobre Y a 20°/s cuando `animT > 0`
- Color override: interpolación gris-oscuro → azul eléctrico con `animT`

#### M3_DER — Fábrica con humo (`Factory.obj` + `Factory.mtl`)
- `Factory.mtl` creado manualmente: verde industrial (`defolt`) y rojo ladrillo (`None`)
- Partículas de humo procedural: 7 discos por chimenea que suben y se expanden (`drawSmoke()`)
- Posiciones de chimeneas calculadas desde el bounding-box del OBJ: chimney1=(−0.87, 2.70, −0.77), chimney2=(−1.62, 2.74, −0.78) en espacio del módulo

### Galería de arte — `app/src/scene/GalleryScene.h`

24 pinturas enmarcadas distribuidas en los cuatro muros de los corredores:

| Corredor | Pared | Pinturas | Temática |
|----------|-------|----------|----------|
| Izquierdo | Exterior (X=−20) | 01-06 | Consecuencias del calentamiento |
| Izquierdo | Interior (X=−4.25) | 01-06 | Consecuencias del calentamiento |
| Derecho | Interior (X=+4.25) | 07-12 | Soluciones y esperanza |
| Derecho | Exterior (X=+20) | 07-12 | Soluciones y esperanza |

- Lienzo: 1.80 × 1.35 m (relación 4:3), centrado a Y=2.80 m
- Marco: tiras de 0.08 m en marrón oscuro (4 strips: 2 horizontales + 2 verticales)
- UV espejado en corredor derecho para evitar texto/imagen invertida
- `makeWallQuad(w, h, mirrorU)`: quad vertical en plano YZ con normal +X, sin culling
- Generadas con `scripts/generar_pinturas.py` (Pillow) → `app/assets/textures/gallery/pintura_01..12.png`

#### Pinturas generadas

| # | Archivo | Tema |
|---|---------|------|
| 01 | pintura_01.png | Deshielo Ártico — icebergs fracturándose en aguas oscuras |
| 02 | pintura_02.png | Fauna en Peligro — oso polar en plataforma de hielo menguante |
| 03 | pintura_03.png | Ciudades Sumergidas — skyline a medio sumergir |
| 04 | pintura_04.png | Glaciares en Retroceso — split hielo/océano con "+2.7 °C" |
| 05 | pintura_05.png | Tormenta Climática — huracán espiral con rayos |
| 06 | pintura_06.png | CO₂ Sin Control — mapa de calor, chimeneas y humo |
| 07 | pintura_07.png | Energía del Viento — parque eólico con sol |
| 08 | pintura_08.png | Poder del Sol — sol radiante sobre paneles solares |
| 09 | pintura_09.png | Movilidad Limpia — auto eléctrico con rayo cyan |
| 10 | pintura_10.png | Bosques Restaurados — dosel verde con rayos de sol |
| 11 | pintura_11.png | Acuerdos Globales — globo terráqueo con laureles dorados |
| 12 | pintura_12.png | Ártico Renacido — aurora boreal sobre montañas nevadas |

---

## Bugs Visuales Conocidos

### Críticos (afectan presentación)
- [ ] **Modelos Kenney sin textura embebida** — electric_car.glb, building_a/b/c.glb usan color override como workaround
- [ ] **Letreros 3D como líneas blancas** — los cubos en cruz son visibles como líneas desde lejos; considerar eliminarlos

### Cosméticos (no bloquean entrega)
- [ ] **Vestíbulo vacío** — no hay elementos decorativos al inicio
- [ ] **Turbina procedural delgada** — las palas de cubo son difíciles de ver
- [ ] **Fuente Roboto no cargada** — ImGui usa su fuente default

---

## Pendientes para Entrega Final

### Por el equipo (acción manual)
- [ ] **Grabar video** con OBS + editar con DaVinci Resolve (guion en docs/reports/guion-video.md)
- [ ] **Crear instalador** con InstallForge GUI desde dist/
- [ ] **Convertir reportes a PDF** con Pandoc o Word
- [ ] **Presentación oral**

---

## Fases de Implementación

| Fase | Descripción | Estado |
|------|-------------|--------|
| 0 | Bootstrap CMake + dependencias | Completada |
| 1 | Motor core (Window, Input, Time, CameraFPS) | Completada |
| 2 | Pipeline de render (Shader, Mesh, Texture, MVP) | Completada |
| 3 | Asset pipeline (Model.h con Assimp) | Completada |
| 4 | Layout del museo (suelo, techo, paredes, plataformas) | Completada |
| 5 | Animaciones LERP (7 módulos, tecla E, test mode) | Completada |
| 6 | Blinn-Phong + Fresnel | Completada |
| 7A | Skybox cubemap ártico procedural | Completada |
| 7B | Shader de agua (vertex displacement + Fresnel) | Completada |
| 8 | Nieve billboards + fauna decorativa | Completada |
| 9 | UI ImGui + letreros 3D + miniaudio | Completada |
| 10 | Integración narrativa (título/cierre) | Completada |
| 11 | QA rendimiento (165 FPS con VSync off) | Completada |
| 12 | Colisiones bidireccionales + popup narrativo + audio por módulo | Completada |
| 13 | DecoScene: fauna animada (lobos + pingüinos), modelos reales, piso hielo, terreno exterior | Completada |
| 14 | Modelos reales en exhibiciones (turbina EolicOBJ, Tesla Model S, fábrica) + galería de arte 24 pinturas | Completada |
| 15 | Galería en todas las paredes (flush + corredor central + cierre + M5 + vestíbulo) + Tesla Model S real con rotación Y | Completada |
| Assets | Texturas CC0 + modelos Quaternius/Kenney/KhronosGroup + modelos reales Fase 13-14 | Completada |
| QA Visual | Verificación escena por escena con screenshots | Completada |
