# Pendientes del Proyecto — CGEIHC-P

> Proyecto: Calentamiento global en el polo norte
> Última actualización: 2026-05-18
> Estado general: **Fase 12 completa — Colisiones, narración y audio por módulo integrados**

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

### Modelos 3D (16 archivos GLB en app/assets/models/)

| Modelo | Origen | Textura | Uso en escena |
|--------|--------|---------|---------------|
| pine_snow.glb | Quaternius CC0 | Embebida | M3_DER arbol que crece |
| electric_car.glb | Kenney CC0 | Color override azul | M2_DER auto electrico |
| building_a/b/c.glb | Kenney CC0 | Color override concreto | M3_IZQ ciudad inundada |
| globe.glb | KhronosGroup CC0 | Embebida (metal) | M5 globo giratorio |
| iceberg.glb | Quaternius CC0 | Color override azul hielo | M1_IZQ iceberg derritiendose |
| polar_bear.glb | Quaternius CC0 | Color override crema | M2_IZQ oso polar sobre hielo |
| fox.glb | KhronosGroup CC0 | Embebida | Fauna decorativa (zorro) |
| seagull.glb | three.js (Stork) | Color override blanco | Fauna decorativa (ave) |
| seal.glb | Quaternius CC0 | Color override gris | Fauna decorativa (foca) |
| whale.glb | Quaternius CC0 | Color override azul-gris | Fauna decorativa (ballena) |
| rock.glb | Quaternius CC0 | Color override gris | Decoracion rocas (4 posiciones) |
| tree.glb | Kenney CC0 | Sin textura (legacy) | No usado (reemplazado por pine_snow) |

### Resultados QA Visual v3

| Escena | Estado | Notas |
|--------|--------|-------|
| Vestibulo | Funcional | Suelo texturizado, paredes, techo. Espacio vacio pero funcional |
| M1_IZQ Iceberg | OK | Roca azul que encoge con animT. Escala 5.0 base |
| M2_IZQ Oso polar | OK | Wolf modelo crema sobre disco de hielo |
| M3_IZQ Edificios | OK | 3 edificios concreto a escala 1.8. Agua sube con animT |
| M1_DER Turbina | Funcional | Procedural: poste + gondola + 3 palas rotatorias |
| M2_DER Auto | OK | Kenney car azul con movimiento sinusoidal |
| M3_DER Arbol | OK | Pino nevado que crece de semilla a arbol completo |
| M5 Globo | OK | DamagedHelmet texturizado, rotacion constante |
| Fauna | OK | Zorro, ave, foca, ballena, 4 rocas dispersas |
| Skybox | OK | Cubemap procedural artico con aurora |
| Agua | OK | Shader sinusoidal + Fresnel, Y=-0.6 (no traspasa suelo) |
| Nieve | OK | 100 billboards cayendo |

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
| Assets | Texturas CC0 + modelos Quaternius/Kenney/KhronosGroup | Completada |
| QA Visual | Verificación escena por escena con screenshots | Completada |
