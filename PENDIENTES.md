# Pendientes del Proyecto — CGEIHC-P

> Proyecto: Calentamiento global en el polo norte
> Última actualización: 2026-05-16
> Estado general: **Planificación completa — listo para implementación**

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

---

## Prioridad Inmediata

- [ ] **Fase 0 — Bootstrap CMake**: build reproducible en VS2022 con todas las dependencias integradas

---

## Fases de Implementación

Referencia completa: `blueprints/09-orden-desarrollo.md`

### Fase 0 — Bootstrap CMake

- [ ] Crear `CMakeLists.txt` con FetchContent (tags congelados: GLFW 3.4, GLM 1.0.1, Assimp v5.4.3, ImGui v1.91.6)
- [ ] Agregar GLAD (headers) y stb (stb_image.h, stb_image_write.h, stb_truetype.h) a `src/vendor/`
- [ ] Agregar miniaudio.h a `src/vendor/`
- [ ] Configurar copia automática de DLLs al directorio del .exe
- [ ] Verificar: `cmake --build build --config Debug --parallel` genera `.exe` limpio

**Criterio de salida**: `.exe` compila sin errores; DLLs copiadas automáticamente.

---

### Fase 1 — Motor Core y Ventana

- [ ] `Window.h/.cpp` — ventana GLFW + contexto OpenGL 3.3 Core + callbacks
- [ ] `Input.h/.cpp` — polling de teclado/mouse desacoplado del render
- [ ] `Time.h/.cpp` — deltaTime, `glfwGetTime()`, frame pacing
- [ ] `main.cpp` — loop principal con `AppState { TITULO, JUGANDO, CIERRE, SALIR }`
- [ ] Cámara FPS: WASD 4 m/s, pitch ±80°, `GLFW_CURSOR_DISABLED`
- [ ] Activar `glEnable(GL_DEBUG_OUTPUT)` + `glDebugMessageCallback` desde el inicio

**Criterio de salida**: ventana abre, cámara FPS estable, Escape cierra, deltaTime correcto.

---

### Fase 2 — Pipeline de Render Básico

- [ ] `Shader.h/.cpp` — compilación GLSL, linking, uniforms (setMat4, setFloat, setVec3)
- [ ] `Mesh.h/.cpp` — VAO/VBO/EBO, `draw()`
- [ ] `Texture.h/.cpp` — stb_image → GLuint
- [ ] Matrices MVP correctas
- [ ] Cubo texturizado renderizado de prueba

**Criterio de salida**: cubo texturizado renderizado con matrices Model/View/Projection correctas.

---

### Fase 3 — Asset Pipeline

- [ ] `Model.h/.cpp` — Assimp GLTF → `vector<Mesh>`
- [ ] `ResourceManager.h/.cpp` — caché de modelos/texturas/shaders por path
- [ ] Un modelo GLTF de Sketchfab carga y renderiza sin warnings

**Criterio de salida**: modelo GLTF estático renderizado en escena.

---

### Fase 4 — Layout del Museo

- [ ] Suelo nevado (plano texturizado, PBR hielo)
- [ ] 7 plataformas circulares r=5m
- [ ] Límites AABB para cámara FPS
- [ ] Letreros 3D estáticos (`SignSystem.h/.cpp` — stb_truetype → textura unlit)
- [ ] Flechas de suelo: "← Las consecuencias" / "Las soluciones →"
- [ ] Jugador recorre el museo completo (T invertida) sin glitches

**Criterio de salida**: layout del museo navegable con señalización visible.

---

### Fase 5 — Sistema de Módulos

- [ ] `ModuloBase.h/.cpp` — trigger XZ, EstadoModulo, t, `update(dt)`, `virtual animate(float)=0`, `reset()`
- [ ] `TriggerZone.h/.cpp` — distancia XZ < 5m, módulo activo
- [ ] Los 7 módulos con animación LERP responden a tecla E y hacen reset al salir
  - [ ] `ModuloIceberg.h/.cpp` — scale lerp vec3
  - [ ] `ModuloPolarBear.h/.cpp` — iceScale lerp XZ
  - [ ] `ModuloSeaLevel.h/.cpp` — waterY lerp
  - [ ] `ModuloTurbina.h/.cpp` — rotSpeed lerp
  - [ ] `ModuloAuto.h/.cpp` — emissive lerp + carZ lerp
  - [ ] `ModuloArbol.h/.cpp` — treeScale lerp
  - [ ] `ModuloGlobo.h/.cpp` — rotación constante + alpha lerp líneas

**Criterio de salida**: los 7 módulos responden a tecla E con animación LERP y reset al salir.

---

### Fase 6 — Iluminación Blinn-Phong

- [ ] Shader principal con luz direccional (sol ártico)
- [ ] Normales correctas por vértice desde Assimp
- [ ] Specular visible en ángulos oblicuos
- [ ] Uniform `viewPos` y `time` enviados cada frame

**Criterio de salida**: escena iluminada con Blinn-Phong, specular correcto.

---

### Fase 7 — Shaders Avanzados

- [ ] Fresnel en shader principal (hielo y agua)
- [ ] Shader de agua procedural (vertex displacement seno, base: `13_wavesAnimation-fresnel.vs/.fs`)
- [ ] `Skybox.h/.cpp` — cubemap 6 caras PNG, shader de skybox con depth trick
- [ ] Niebla exponencial integrada en shader principal (densidad ~0.008, color ártico)

**Criterio de salida**: agua animada con seno, Fresnel visible en bordes, skybox ártico sin artefactos.

---

### Fase 8 — Efectos y Fauna

- [ ] `SnowSystem.h/.cpp` — 80-120 billboards, VBO dinámico CPU→GPU
- [ ] Fauna decorativa posicionada (foca, zorro, gaviota billboard)
- [ ] Partículas de módulos (turbina billboard, CO₂ espiral M3-der)

**Criterio de salida**: 80-120 copos de nieve estables; fauna posicionada; partículas de módulos funcionales.

---

### Fase 9 — UI, Texto y Audio

- [ ] `HUD.h/.cpp` — Dear ImGui, indicador [E] Explorar (bottom-center, solo en trigger)
- [ ] `TitleScreen.h/.cpp` — foto ártica fullscreen (stb_image) + fade-in + ImGui + Enter
- [ ] `CreditsScreen.h/.cpp` — pantalla de cierre desde M5 + Escape para salir
- [ ] Fuente Roboto-Regular.ttf cargada en ImGui (tamaños 14/18/22/32/48px)
- [ ] `AudioEngine.h/.cpp` — miniaudio: ambient loop + 3 fuentes 3D posicionales
- [ ] Obtener assets de audio (Freesound.org CC0):
  - [ ] arctic_wind_loop.ogg
  - [ ] activate.wav
  - [ ] ice_cracking.wav
  - [ ] water_flowing.wav
  - [ ] nature_breeze.wav

**Criterio de salida**: pantalla de título, indicador [E], letreros visibles, audio ambiental y 3D posicional funcionando.

---

### Fase 10 — Integración Narrativa

- [ ] Flujo completo: TITULO → JUGANDO → M5 COMPLETADO → CIERRE → SALIR
- [ ] Pantalla de cierre se activa solo desde M5 con animación completada (Escape)
- [ ] Recorrido narrativo completo sin crashes

**Criterio de salida**: usuario completa el recorrido de inicio a fin sin crashes.

---

### Fase 11 — Pulido y QA

- [ ] Activar `saveScreenshot()` (glReadPixels + stb_image_write, tecla F12)
- [ ] Resolver memory leaks (verificar con RenderDoc)
- [ ] Build estable 15+ minutos continuos a ≥60 FPS
- [ ] Empaquetar con InstallForge (.exe + assets/)

**Criterio de salida**: build empaquetable; ejecutable estable sin crashes.

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
