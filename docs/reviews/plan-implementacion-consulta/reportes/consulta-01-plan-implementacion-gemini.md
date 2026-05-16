# Plan de Implementación Óptimo — Museo Virtual 3D C++/OpenGL

### 1. Fases de desarrollo

| Fase | Nombre | Objetivo | Criterio de salida |
|------|--------|----------|--------------------|
| 1 | **Motor Core y Ventana** | Establecer el contexto GL, bucle principal de estados, y cámara FPS básica. | Fase 1 está lista cuando: el ejecutable compila sin warnings en VS2022, la ventana se abre con fondo de un color unificado, la cámara permite mirar 360° con pitch restringido y desplazarse en un plano, y la tecla Escape libera el mouse/cierra. |
| 2 | **Base Geométrica y Terreno** | Renderizar primitivas, límites AABB de los pasillos, y testear shaders de color sólido. | Fase 2 está lista cuando: el ejecutable compila sin warnings en VS2022, se visualiza el suelo plano en T invertida, las cámaras chocan con los límites definidos AABB (sin rebotar), y no existen memory leaks al cerrar. |
| 3 | **Carga de Assets (Assimp/stb)** | Integrar Assimp para modelos .gltf/.obj fijos y stb_image para texturas. | Fase 3 está lista cuando: el ejecutable compila sin warnings en VS2022, un modelo GLTF de Sketchfab aparece renderizado estáticamente en las plataformas, y stb_truetype logra cargar el texto del letrero base a una textura plana. |
| 4 | **Iluminación (Blinn-Phong)** | Implementar la iluminación principal (direccional), asegurando uso de normales. | Fase 4 está lista cuando: el ejecutable compila sin warnings en VS2022, los shaders Blinn-Phong operan usando las matrices MVP correctas, los cubos/modelos proyectan iluminación y los materiales reaccionan al componente difuso/especular. |
| 5 | **Sistema de Módulos (Anim/Estados)** | Programar `ModuleBase`, trigger de colisión XZ, animaciones LERP y reset interactivo. | Fase 5 está lista cuando: el ejecutable compila sin warnings en VS2022, acercarse a 5m de la plataforma permite que la tecla E modifique variables en el `update()` (LERP de scale o posición), y al alejarse todo vuelve a reset. |
| 6 | **Shaders Avanzados (Rúbrica)** | Implementar shader de Agua (ondas + Fresnel), Skybox, y Partículas CPU. | Fase 6 está lista cuando: el ejecutable compila sin warnings en VS2022, el shader del profesor para agua/Fresnel compila en tiempo de ejecución, deforma el grid del suelo con un seno continuo, y el Skybox mapea perfectamente atrás usando depth buffer. |
| 7 | **Ensamblaje Escénico y UI (ImGui)** | Colocar todos los M1-M5, fauna, conectar ImGui (Pantalla Título, Cierre, indicador E). | Fase 7 está lista cuando: el ejecutable compila sin warnings en VS2022, el HUD de Dear ImGui muestra la pantalla inicial sin glitchear la escena atrás, y toda la fauna y modelos de los 8 blueprints están en sus coordenadas exactas. |
| 8 | **Audio (OpenAL)** | Integrar sonido de viento global en loop y clips 3D por módulo (`activate.wav`). | Fase 8 está lista cuando: el ejecutable compila sin warnings en VS2022, OpenAL carga archivos locales WAV y OGG con libsndfile, existe audio estereofónico base de viento, y los audios posicionales se desvanecen al cruzar 20 metros. |

### 2. Prerequisitos antes de shaders avanzados

Antes de tocar el shader procedural de agua y la iluminación Fresnel, **debe estar verificado lo siguiente**:

- [ ] La cámara calcula la Matriz de Vista (`View Matrix`) y de Proyección (`Projection Matrix`) de manera perfecta (sin distorsión de perspectiva).
- [ ] La uniform variable `cameraPos` o `viewPos` se envía correctamente al Fragment Shader de los shaders base.
- [ ] La importación de modelos por Assimp exporta el vector normal explícito por cada vértice (requisito para Fresnel y sombreado).
- [ ] El framework base soporta la inyección dinámica de una variable uniform `time` ligada al deltaTime del main loop (necesario para oscilar el mar).
- [ ] El sistema de Z-Buffer (Depth testing `GL_DEPTH_TEST`) funciona limpiamente, ya que Fresnel suele amplificar problemas de intersección de geometría.

### 3. Estructura de código src/

```text
src/
├── main.cpp                  # Entry point, glfw init, bucle principal AppState { TITULO, JUGANDO, CIERRE, SALIR }
├── core/
│   ├── Window.cpp/h          # Responsabilidad: Creación de ventana GLFW, callbacks, manejo de deltaTime.
│   │                         # Métodos: init(), swapBuffers(), isClosed()
│   └── InputManager.cpp/h    # Responsabilidad: Polling de estados del mouse/teclado de forma aislada.
│                             # Métodos: isKeyPressed(), getMouseDelta()
├── renderer/
│   ├── Renderer.cpp/h        # Responsabilidad: Encapsular el estado de OpenGL y emitir llamadas de dibujado de bajo nivel.
│   │                         # Métodos: clear(), draw(vao, count), setBlendState()
│   ├── Shader.cpp/h          # Responsabilidad: Compilación, enlace y gestión de variables uniform de GLSL.
│   │                         # Métodos: use(), setMat4(), setFloat()
│   ├── Texture.cpp/h         # Responsabilidad: Lectura por stb_image/stb_truetype y generación de IDs OpenGL.
│   │                         # Métodos: loadFromFile(), bind()
│   └── Camera.cpp/h          # Responsabilidad: Manejar Free-fly y generar la MVP, aplicar clamp AABB.
│                             # Métodos: update(), getViewMatrix(), getProjectionMatrix()
├── scene/
│   ├── Scene.cpp/h           # Responsabilidad: Contenedor global de todos los módulos, el terreno y fauna.
│   │                         # Métodos: init(), update(dt), render()
│   ├── Model.cpp/h           # Responsabilidad: Carga con Assimp y empaquetado en VAO/VBO. Reutilización por ID.
│   │                         # Métodos: loadGLTF(), draw(shader)
│   └── Skybox.cpp/h          # Responsabilidad: Renderizado del cubemap estático y su shader dedicado de fondo.
│                             # Métodos: load(), draw()
├── logic/
│   └── ModuleBase.cpp/h      # Responsabilidad: Patrón compartido estado (INACTIVO/ANIMANDO) → LERP (E) → reset (out_dist).
│                             # Métodos: checkTrigger(playerPos), onInteract(), updateAnimation(dt)
├── fx/
│   └── ParticleSystem.cpp/h  # Responsabilidad: Manejar quads en CPU/VBO para la nieve del ambiente y billboard particles.
│                             # Métodos: init(maxParticles), update(dt), render()
├── audio/
│   └── AudioManager.cpp/h    # Responsabilidad: Contexto OpenAL, libsndfile decodes, playbacks paralelos y 3D pos.
│                             # Métodos: init(), playGlobal(ogg), play3D(wav, pos), updateListener(cam)
└── ui/
    └── HUD.cpp/h             # Responsabilidad: Ciclo de Dear ImGui, sobreposición de menú y captura de Enter/Esc.
                              # Métodos: renderTitleScreen(), renderCierreScreen(), renderInteractPrompt()
```

### 4. Riesgos y mitigaciones

| # | Riesgo | Probabilidad | Mitigación concreta |
|---|--------|--------------|---------------------|
| 1 | **Assimp leyendo GLTF con animaciones complejas** (Oso polar) | Alta | Ignorar el procesamiento de huesos/rigging si es muy frágil. Optar por exportar mallas estáticas secuenciales, o si se carga animación, extraer solo matrices precalculadas. Fallback: malla estática que oscila en Y (LERP). |
| 2 | **Corrupción de estado de OpenGL por Dear ImGui** | Alta | ImGui modifica VAOs, BlendFuncs y Depth Testing internamente. Asegurarse de realizar la llamada `ImGui::Render()` al **finalísimo del loop** del frame y restaurar banderas (ej. `glEnable(GL_DEPTH_TEST)`) al principio del siguiente frame de forma explícita. |
| 3 | **Stb_truetype corrompiendo memoria en el render font** | Media | Evitar mapeos dinámicos en tiempo real para letreros. Generar las texturas de los letreros en blanco y negro en CPU *una sola vez en `Init()`*, subirlas a GPU con `glTexImage2D` (usando `GL_RED` para el canal alpha/color) y liberar el buffer en CPU. |
| 4 | **FetchContent de OpenAL/Assimp dando timeout** | Media | Las redes universitarias o GitHub rate limits pueden bloquear descargas largas de CMake. Si FetchContent falla crónicamente, alojar pre-compiled binaries (DLL y headers) subiéndolos a un directorio `/third_party` del repositorio. |
| 5 | **Problemas de Transparencia y Z-Fighting** (Agua, Nieve, Letreros) | Media | Renderizar en estricto orden: primero opacos (modelos, terreno), aplicar render de skybox forzando z=1.0, desactivar escritura depth `glDepthMask(GL_FALSE)`, renderizar transparentes ordenados (Agua → Nieve → Letreros → ImGui), restablecer máscara. |

### 5. Recomendaciones adicionales

- **Separación del modelo LERP**: El blueprint define el estado en los módulos. Evita confiar en cinemáticas incrustadas directamente dentro del formato glTF si no son necesarias; utiliza variables puras de C++ (ej. la altura `Y` del agua, el `scale` transform del iceberg) enviándolas directamente a la matriz de modelo del Uniform. El profesor valora más la *animación procedimental interconectada a través de estados que programas en C++* que una reproducción de animación importada opaca de Blender.
- **Letreros Unlit (Sin Sombreado)**: Los letreros NO deberían procesar cálculos de luces (Blinn-Phong) porque bajo ciertos ángulos y condiciones podrían ser ilegibles para el jurado. Pásalos por un `hud.vs/fs` "Unlit Shader" que solo mapee la textura 2D difusa multiplicada por un canal alfa. Garantiza claridad estilo interfaz gráfica clásica.
- **Enfoque Pragmático (Ejecutable vs ECS)**: El profesor evaluará fuertemente que "el binario funcione al 100%" (ejecución sin errores = 0.5pts, empaquetado InstallForge = Obligatorio). No intentes construir un motor gráfico de propósito general o un Entity Component System (ECS) pesado. Una herencia de clases simple iterada de forma secuencial (`std::vector<ModuleBase*>`) cumplirá todo el conjunto de la rúbrica manteniendo el código rápido y fácil de debuggear por agentes automatizados (Claude).
