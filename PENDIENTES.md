# Pendientes del Proyecto — CGEIHC-P

> Proyecto: Calentamiento global en el polo norte
> Última actualización: 2026-05-19
> Estado general: **Fase 13 completa — DecoScene: fauna animada, modelos reales, piso de hielo, terreno exterior nevado**

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
| electric_car.glb | Kenney CC0 | Color override azul | M2_DER auto eléctrico |
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
| M1_DER Turbina | Funcional | Procedural: poste + góndola + 3 palas rotatorias |
| M2_DER Auto | OK | Kenney car azul con movimiento sinusoidal |
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
| Assets | Texturas CC0 + modelos Quaternius/Kenney/KhronosGroup + modelos reales Fase 13 | Completada |
| QA Visual | Verificación escena por escena con screenshots | Completada |
