# Pendientes del Proyecto — CGEIHC-P

> Proyecto: Calentamiento global en el polo norte
> Última actualización: 2026-05-16
> Estado general: **Implementación completada — Fases 0-11 ✅ | Listo para empaquetado y entregables**

---

## Estado Actual

### ✅ Completado

- Primera entrega (06/05/2026): Idea + Boceto + Storyboard + Blueprints + Modelado + Rigging + Materiales + Animación + Iluminación/Texturizado + Renderizado + Sonido + Interacción
- **9 documentos blueprint completados** (01 al 09 en `blueprints/`)
- **Stack técnico validado** y documentado en `INFRAESTRUCTURA.md`
  - miniaudio (header-only) en lugar de OpenAL Soft + libsndfile
  - glReadPixels + stb_image_write para screenshots autónomos
  - FetchContent con tags congelados (glfw 3.4, glm 1.0.1, assimp v5.4.3, imgui v1.91.6)
- Consulta externa a Gemini + GPT para plan de implementación
- **Fase 0** — Bootstrap CMake ✅ (build reproducible, todas las dependencias)
- **Fase 1** — Motor Core ✅ (Window, Input, Time, CameraFPS, AppState loop)
- **Fase 2** — Pipeline de render ✅ (Shader, Mesh, Texture, matrices MVP)
- **Fase 3** — Asset Pipeline ✅ (Model.h con Assimp GLTF/OBJ)
- **Fase 4** — Layout del museo ✅ (suelo, techo, paredes, 7 plataformas, AABB)
- **Fase 5** — Animaciones LERP ✅ (7 módulos, tecla E, test mode CLI)
- **Fase 6** — Blinn-Phong + Fresnel ✅ (luz hemisférica + specular + Schlick)
- **Fase 7A** — Skybox cubemap ártico ✅ (cubemap procedural 64×64, aurora boreal)
- **Fase 7B** — Shader de agua ✅ (vertex displacement seno 3 capas + Fresnel + espuma)
- **Fase 8** — Efectos y fauna ✅ (100 copos billboard, foca + zorro + gaviota)
- **Fase 9** — UI + letreros + audio ✅ (ImGui narrativo, letreros 3D en cruz, miniaudio)
- **Fase 10** — Integración narrativa ✅ (pantalla título + cierre desde M5 + 5s countdown)
- **Fase 11** — QA ✅ (todos los módulos 59-61 FPS, animaciones correctas)

#### Resultados QA (frame 600, ~10 segundos):

| Módulo | FPS | animT | Estado |
|--------|-----|-------|--------|
| M1_IZQ | 60.97 | 0.895 | ✅ |
| M2_IZQ | 60.99 | 1.000 | ✅ |
| M3_IZQ | 59.96 | 0.746 | ✅ |
| M1_DER | 59.11 | 1.000 | ✅ |
| M2_DER | 59.11 | 1.000 | ✅ |
| M3_DER | 60.94 | 1.000 | ✅ |
| M5     | 59.13 | 0.746 | ✅ |

---

## Prioridad Inmediata

- [ ] **Integrar assets reales** (modelos GLTF CC0, texturas, audio WAV/OGG, skybox PNG)
- [ ] **Empaquetar con InstallForge** (.exe + assets/ en instalador Windows)
- [ ] **Reporte académico** (20-35 páginas, PDF, APA, español + inglés)
- [ ] **Video demostrativo** (3-5 min, DaVinci Resolve)

---

## Fases de Implementación

Referencia completa: `blueprints/09-orden-desarrollo.md`

### Fase 0 — Bootstrap CMake ✅

- [x] Crear `CMakeLists.txt` con FetchContent (GLFW 3.4, GLM 1.0.1, Assimp v5.4.3, ImGui v1.91.6)
- [x] GLAD, stb_image, stb_image_write, stb_truetype, miniaudio.h en `src/vendor/`
- [x] Build limpio: `cmake --build build --config Debug --parallel` → `CGEIHC.exe`

**Criterio de salida**: ✅ `.exe` compila sin errores.

---

### Fase 1 — Motor Core y Ventana ✅
- [x] Window.h, Input.h, Time.h, CameraFPS.h — loop con AppState { TITULO, JUGANDO, CIERRE, SALIR }

### Fase 2 — Pipeline de Render Básico ✅
- [x] Shader.h, Mesh.h, Texture.h — matrices MVP, VAO/VBO/EBO

### Fase 3 — Asset Pipeline ✅
- [x] Model.h con Assimp GLTF/OBJ

### Fase 4 — Layout del Museo ✅
- [x] Museum.h — suelo, techo, paredes, 7 plataformas r=5m, AABB cámara

### Fase 5 — Sistema de Módulos ✅
- [x] ModuleScene.h — 7 animaciones LERP, activación con E, test mode CLI (`--test-module`)

### Fase 6 — Iluminación Blinn-Phong ✅
- [x] standard.frag — luz hemisférica ártica + Blinn-Phong + Fresnel (Schlick)

### Fase 7A — Skybox Cubemap ✅
- [x] Skybox.h — cubemap 64×64 procedural (zenith azul, horizonte, aurora boreal), depth trick

### Fase 7B — Shader de Agua ✅
- [x] water.vert/.frag — desplazamiento sinusoidal 3 capas + espuma + Fresnel
- [x] makeGrid() en Mesh.h — 200×200m, 60×60 subdivisiones

### Fase 8 — Efectos y Fauna ✅
- [x] SnowSystem.h — 100 billboards orientados a cámara, caída + deriva sinusoidal
- [x] Fauna: drawSeal, drawArcticFox, drawSeagull en ModuleScene.h

### Fase 9 — UI + Letreros + Audio ✅
- [x] Panel debug (FPS/posición) + panel narrativo centrado (nombre + barra progreso)
- [x] Letreros 3D en cruz con labelColor sobre cada módulo (signCube compartido)
- [x] miniaudio inicializado con fallback gracioso si no hay archivos de audio

### Fase 10 — Integración Narrativa ✅
- [x] Pantalla de título (fondo oscuro, texto, "[ESPACIO] para iniciar")
- [x] Estado CIERRE activado cuando M5.animT=1.0 (con countdown 5s)
- [x] Loop completo: TITULO → JUGANDO → CIERRE → SALIR

### Fase 11 — QA ✅
- [x] Todos los módulos verificados: 59.1–61.0 FPS, animaciones correctas
- [x] PENDIENTES.md actualizado con estado real

---

## Pendiente para Entrega Final

- [ ] **Assets reales**: modelos GLTF CC0 (Sketchfab), texturas (ambientcg), skybox PNG (polyhaven)
- [ ] **Audio**: descargar OGG/WAV de freesound.org CC0 → `app/assets/audio/`
- [ ] **Fuente TTF**: Roboto-Regular.ttf → `app/assets/fonts/` (activar en ImGui)
- [ ] **Empaquetado**: InstallForge — `.exe` + `assets/` + `shaders/` en un instalador
- [ ] **Reporte en español** (20-35 páginas, PDF, formato APA)
- [ ] **Reporte en inglés** (misma estructura)
- [ ] **Video demostrativo** (3-5 min, MP4/H.264, narrado en DaVinci Resolve)

---

## Entregables Finales

- [ ] **Software ejecutable** (.exe con InstallForge, assets incluidos)
  - [ ] `README.txt` con instrucciones de instalación y uso
  - [ ] `manual.pdf` con descripción del sistema
- [ ] **Reporte en español** (20-35 páginas, PDF, formato APA)
- [ ] **Reporte en inglés** (misma estructura)
- [ ] **Video demostrativo** (3-5 min, MP4/H.264, narrado — edición en DaVinci Resolve)
- [ ] **Presentación oral y digital**

---

## Notas y Decisiones

- El código fuente **no se entrega** — se revisa en sesiones de avances
- Empaquetador: **InstallForge** (gratis, recomendado por el profesor)
- Editor de video: **DaVinci Resolve** (gratis)
- Referencias: **Mendeley o Zotero** para formato APA
- Todo sube a **Google Drive** del equipo (enlace en Moodle)
- Implementación **sin división de equipo** — Claude ejecuta toda la codificación

---

## Recursos Útiles

- **learnopengl.com** — Tutorial principal de referencia
- **cursos.computer-graphics.com.mx** — Plataforma del curso (Moodle)
- **Shaders del profesor**: `13_wavesAnimation-fresnel.vs/.fs` (base para agua y Fresnel)
- **Texturas CC0**: ambientcg.com, polyhaven.com, 3dtextures.me
- **Modelos 3D CC0**: sketchfab.com (filtro CC0), polyhaven.com
- **Audio CC0**: freesound.org
- **Fuente**: Roboto-Regular.ttf (Google Fonts, Apache 2.0)
