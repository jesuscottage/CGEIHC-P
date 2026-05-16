# Doc 09 — Orden de Desarrollo

> Estado: COMPLETO
> Última actualización: 2026-05-16
> Basado en síntesis de consulta a Gemini + ChatGPT (consulta-01, docs/reviews/plan-implementacion-consulta/)

---

## Fases de Desarrollo

| Fase | Nombre | Objetivo | Criterio de salida |
|------|--------|----------|--------------------|
| 0 | **Bootstrap CMake** | Build reproducible en VS2022 con todas las dependencias integradas | `cmake --build` genera `.exe` limpio; DLLs copiadas automáticamente; FetchContent con tags congelados |
| 1 | **Motor core y ventana** | Contexto OpenGL 3.3, loop principal con `AppState`, cámara FPS | Ventana abre, cámara FPS con pitch ±80° y WASD, Escape cierra, deltaTime estable |
| 2 | **Pipeline de render básico** | `Shader`, `Mesh`, VAO/VBO/EBO, matrices MVP, texturas con stb_image | Cubo texturizado renderizado con matrices Model/View/Projection correctas |
| 3 | **Asset pipeline** | `ResourceManager` + Assimp GLTF, caché de modelos y texturas | Un modelo GLTF de Sketchfab carga y renderiza estáticamente sin warnings |
| 4 | **Layout del museo** | Suelo, plataformas circulares, límites AABB, señalización | Jugador recorre el museo completo (T invertida) sin glitches; plataformas visibles |
| 5 | **Sistema de módulos** | `ModuloBase`, triggers XZ, estados, LERP, reset instantáneo | Los 7 módulos responden a tecla E con su animación LERP y reset al salir |
| 6 | **Iluminación Blinn-Phong** | Shader principal con luz direccional y normales correctas | Toda la escena iluminada con Blinn-Phong; specular visible en ángulos oblicuos |
| 7 | **Shaders avanzados** | Fresnel + agua procedural (shader del profesor) + skybox cubemap | Agua anima con seno; Fresnel visible en bordes; skybox ártico carga sin artefactos |
| 8 | **Efectos y fauna** | Partículas de nieve, fauna decorativa, partículas de módulos | 80-120 copos de nieve estables; fauna posicionada; particles de turbina y CO₂ funcionales |
| 9 | **UI, texto y audio** | Dear ImGui, stb_truetype, OpenAL — pantallas, letreros y audio | Pantalla de título, indicador `[E]`, letreros visibles, audio ambiental y 3D posicional |
| 10 | **Integración narrativa** | Flujo completo inicio → recorrido → M5 → pantalla de cierre | Usuario completa el recorrido de inicio a fin sin crashes; pantalla de cierre activa desde M5 |
| 11 | **Pulido y QA** | Resolver leaks, stutter, validar build final, empaquetado | Build estable 15+ minutos continuos; `.exe` empaquetable con InstallForge |

---

## Estructura del Código `src/`

```
src/
├── main.cpp                    # Entry point, init, bucle principal, AppState { TITULO, JUGANDO, CIERRE, SALIR }
│
├── core/
│   ├── Window.h/.cpp           # Ventana GLFW, callbacks, swap buffers
│   ├── Input.h/.cpp            # Polling teclado/mouse desacoplado del render
│   └── Time.h/.cpp             # deltaTime, glfwGetTime(), frame pacing
│
├── graphics/
│   ├── Shader.h/.cpp           # Compilación GLSL, linking, uniforms (setMat4, setFloat, setVec3)
│   ├── Texture.h/.cpp          # stb_image → GLuint, bind()
│   ├── Mesh.h/.cpp             # VAO/VBO/EBO, draw()
│   ├── Model.h/.cpp            # Assimp GLTF → vector<Mesh>, draw(shader)
│   ├── CameraFPS.h/.cpp        # FPS camera, getViewMatrix(), getProjectionMatrix(), clamp AABB
│   ├── Skybox.h/.cpp           # Cubemap 6 caras, shader de skybox, draw()
│   └── particles/
│       ├── SnowSystem.h/.cpp   # 80-120 billboards, update CPU → glBufferSubData, render()
│       └── BillboardParticle.h # Struct partícula: pos, vel, alpha, life
│
├── animation/
│   ├── Animator.h/.cpp         # Skeletal animation GLTF (solo para oso polar idle)
│   └── Lerp.h                  # mix(), clamp(), helpers de interpolación
│
├── modules/
│   ├── ModuloBase.h/.cpp       # Trigger XZ, EstadoModulo, t, update(dt), virtual animate(float)=0, reset()
│   ├── ModuloIceberg.h/.cpp    # scale lerp vec3
│   ├── ModuloPolarBear.h/.cpp  # iceScale lerp XZ + Animator idle loop
│   ├── ModuloSeaLevel.h/.cpp   # waterY lerp + shader de agua compartido
│   ├── ModuloTurbina.h/.cpp    # rotSpeed lerp + BillboardParticle
│   ├── ModuloAuto.h/.cpp       # emissive lerp + carZ lerp
│   ├── ModuloArbol.h/.cpp      # treeScale lerp + partículas CO₂ espiral
│   └── ModuloGlobo.h/.cpp      # rotación constante + alpha lerp secuencial de N cilindros
│
├── scene/
│   ├── MuseumScene.h/.cpp      # Instanciar todos los módulos, fauna, suelo, señalización; update/render
│   ├── ResourceManager.h/.cpp  # Caché de modelos/texturas/shaders por path — cargar una vez, reutilizar
│   ├── SignSystem.h/.cpp       # Letreros 3D: stb_truetype → textura unlit por módulo (generada en Init)
│   └── TriggerZone.h/.cpp      # Cálculo de distancia XZ, módulo activo, dispatch de eventos
│
├── ui/
│   ├── HUD.h/.cpp              # Dear ImGui: indicador [E] Explorar (bottom-center, solo en trigger)
│   ├── TitleScreen.h/.cpp      # Foto ártica fullscreen + fade-in, captura Enter
│   └── CreditsScreen.h/.cpp    # Pantalla de cierre: texto + créditos + Escape para salir
│
└── audio/
    ├── AudioEngine.h/.cpp      # alcOpenDevice, contexto OpenAL, playGlobal(OGG), play3D(WAV, pos)
    └── AudioListener.h/.cpp    # updateListener(camera) — alListener3f + alListenerfv cada frame
```

---

## Prerequisitos Antes de Shaders Avanzados

Checklist obligatorio antes de implementar Blinn-Phong, Fresnel y el shader de agua (Fase 7):

- [ ] GLFW crea contexto OpenGL 3.3 Core sin errores
- [ ] GLAD inicializa correctamente en Windows/VS2022
- [ ] `glEnable(GL_DEPTH_TEST)` activo y funcionando
- [ ] Cámara FPS genera View y Projection correctas (sin distorsión)
- [ ] Uniform `viewPos` / `cameraPos` enviado al fragment shader
- [ ] Assimp exporta normales explícitas por vértice (requeridas por Blinn-Phong y Fresnel)
- [ ] Shader loader: compilación, linking, logs de error legibles
- [ ] Uniform system funcional: `setMat4`, `setVec3`, `setFloat`, arrays
- [ ] VAO/VBO/EBO reutilizables, sin memory leaks
- [ ] Texturas 2D cargadas con stb_image, UVs correctas
- [ ] Uniform `time` ligado a `glfwGetTime()` (necesario para olas del agua)
- [ ] `glGetError()` instrumentado en debug builds — activar desde la Fase 1
- [ ] Skybox básico renderiza sin depth artifacts antes de agregar iluminación
- [ ] Dear ImGui coexiste con el pipeline sin romper depth state
- [ ] Frame pacing estable (≥60 FPS objetivo en máquina de entrega)

---

## Riesgos Técnicos y Mitigaciones

| # | Riesgo | Probabilidad | Mitigación concreta |
|---|--------|--------------|---------------------|
| 1 | **Assimp + GLTF con animación skeletal** (oso polar) | Alta | Implementar primero carga estática. Agregar animación únicamente al oso polar, sin intentar un sistema genérico. Fallback: escala/bobbing en Y con LERP si el clip no carga |
| 2 | **ImGui corrompe el estado de OpenGL** | Alta | Llamar `ImGui::Render()` al final del frame. Restaurar explícitamente `glEnable(GL_DEPTH_TEST)`, blend state y cull face al inicio del siguiente frame |
| 3 | **Orden de transparencia** (agua, nieve, letreros, billboards, ImGui) | Alta | Definir orden estricto desde Fase 7: opacos → skybox → agua → partículas → letreros → ImGui. Usar `glDepthMask(GL_FALSE)` para transparentes |
| 4 | **FetchContent con timeout** en red universitaria | Media | Congelar tags/commits exactos de cada dependencia. Si falla, subir binarios precompilados a `third_party/` del repositorio |
| 5 | **stb_truetype corrompiendo memoria** si se llama en tiempo real | Media | Generar **todas** las texturas de letreros **una sola vez en `Init()`** con `GL_RED` como canal alpha. Liberar buffer CPU tras el upload |

---

## Notas Críticas de Implementación

### Assets: congelar antes de Fase 6
No cambiar modelos ni texturas después de que el sistema de módulos esté funcionando. Cambiarlos tarde rompe escalas, pivotes, animaciones y UVs.

### Orden de integración final
```
suelo y layout → módulos con LERP → iluminación → skybox → agua → partículas → UI → audio → narrativa completa
```

### Patrones de código
- `std::vector<std::unique_ptr<ModuloBase>>` en `MuseumScene` — iterar secuencialmente, sin ECS
- Nunca mezclar lógica con draw calls: separar `update(dt)` de `render()` en cada clase
- Solo dos niveles de herencia: `ModuloBase → ModuloEspecifico` — nada más
- Nunca implementar: sombras, SSR, PBR, IBL, normal mapping complejo, ECS, multithreading

### Debug visual durante desarrollo
Renderizar wireframes de los triggers (círculos r=5m) y mostrar `EstadoModulo` en ImGui durante desarrollo. Ahorra horas de debugging.

### Shaders: mantenerlos pequeños
`standard.frag` es el más complejo. Blinn-Phong + Fresnel + niebla exponencial cumple toda la rúbrica. No agregar más.

---

## Criterio de "Listo para Entregar"

El proyecto está listo cuando:

1. Los 7 módulos tienen animación LERP funcional con tecla E y reset al salir
2. Los letreros muestran el texto correcto en español con fuentes verificadas (datos de FUENTES.md)
3. Blinn-Phong + Fresnel visibles y estables en la escena
4. Las 3 categorías de animación presentes: básica (turbina/globo) + keyframe (LERP módulos) + procedural (olas, nieve, partículas)
5. Skybox ártico y niebla activos en toda la escena
6. Audio ambiental en loop y los 3 módulos con audio 3D posicional funcionan
7. Pantalla de título (foto + Enter) y pantalla de cierre (desde M5 + Escape) operativas
8. El ejecutable abre, se juega el recorrido completo y se cierra sin crashes
9. Build empaquetable con InstallForge (entregable final del curso)
