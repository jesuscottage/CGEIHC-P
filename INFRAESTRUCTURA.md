# Infraestructura y Stack Técnico — CGEIHC-P

> Proyecto: Calentamiento global en el polo norte
> Última actualización: 2026-05-16
> Ver también: `docs/knowledge/technology/especificaciones-proyecto.md`

---

## Resumen del Stack

| Capa | Herramienta | Versión | Propósito |
|------|------------|---------|-----------|
| Lenguaje | C++ | C++17 | Lenguaje principal del motor gráfico |
| API Gráfica | OpenGL | 3.3+ Core Profile | Renderizado 3D en tiempo real (requerido por la materia) |
| Cargador de Extensiones | GLAD | 2.x | Cargador moderno de funciones OpenGL (más sencillo que GLEW) |
| Gestión de Ventana | GLFW | 3.4+ | Ventanas, contexto OpenGL, manejo de eventos |
| Matemáticas | GLM | 1.0.x | Vectores, matrices, transformaciones MVP |
| Carga de Modelos | Assimp | 5.4.x | Importar GLTF/OBJ para modelos **estáticos** |
| Carga de Texturas | stb_image | 2.x (header-only) | Carga PNG/JPG/HDR para texturas |
| Escritura de Imágenes | stb_image_write | 2.x (header-only) | Guardar screenshots con `glReadPixels` — clave para desarrollo autónomo |
| Texto | stb_truetype | 1.x (header-only) | Rasterizar fuente TTF → atlas de textura OpenGL (baked al inicio) |
| UI/HUD | Dear ImGui | 1.91+ | Pantallas de título/cierre, indicador `[E] Explorar` |
| Audio | **miniaudio** | 0.11+ (header-only) | Audio 3D posicional + ambient loop. Reemplaza OpenAL Soft + libsndfile |
| Sistema de Build | CMake | 3.20+ | Gestión de dependencias con FetchContent (tags congelados) |
| IDE | Visual Studio 2022 Community | Free | Compilación y debugging en Windows |
| Debugging GPU | RenderDoc | 1.x | Captura y análisis de frames OpenGL — estándar de facto |
| Control de Versiones | Git + GitHub | — | Repositorio colaborativo |

---

## Decisiones de Stack — Justificación

### ✅ miniaudio en vez de OpenAL Soft + libsndfile

**Cambio**: reemplaza las dos bibliotecas de audio por una sola.

| Aspecto | OpenAL Soft + libsndfile | miniaudio |
|---------|--------------------------|-----------|
| Integración | 2 bibliotecas, DLLs separadas, setup complejo | Single-header (`miniaudio.h`) |
| DLLs en Windows | Requiere copiar `OpenAL32.dll` al .exe | Sin DLLs — todo en el header |
| Formatos | WAV (libsndfile), OGG necesita vorbis extra | WAV + OGG Vorbis built-in |
| Audio 3D | Sí (posicionamiento OpenAL) | Sí (node graph con spatialization) |
| Adopción | Estándar histórico | raylib lo adoptó en 2019, ampliamente usado en 2024-2025 |
| CMake | Requiere FetchContent + configuración | Una sola línea: copiar `miniaudio.h` a `src/` |

**Impacto en el proyecto**: elimina la Fase 0 de configurar OpenAL en Windows VS2022, que es uno de los riesgos técnicos identificados. Los 5 archivos de audio (WAV + OGG) se cargan igual de sencillo.

```cpp
// Uso mínimo — ambient loop global
ma_engine engine;
ma_engine_init(NULL, &engine);
ma_engine_play_sound(&engine, "assets/audio/arctic_wind_loop.ogg", NULL);

// Audio 3D posicional
ma_sound sound;
ma_sound_init_from_file(&engine, "assets/audio/ice_cracking.wav", 0, NULL, NULL, &sound);
ma_sound_set_position(&sound, x, y, z);
ma_sound_start(&sound);
```

### ✅ glReadPixels + stb_image_write en vez de PowerShell screenshot

**Cambio crítico para el desarrollo autónomo.** El enfoque anterior (PowerShell capturando la pantalla) tiene problemas graves:
- Falla si la ventana está tapada por otra
- Depende de timing (espera fija de 3 segundos)
- Captura la pantalla completa, no el framebuffer exacto de OpenGL

**Solución**: implementar `saveScreenshot()` directamente en el motor usando `glReadPixels` + `stb_image_write`. Claude activa la captura con una tecla (F12) o automáticamente tras N frames.

```cpp
void saveScreenshot(const char* path, int width, int height) {
    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    // OpenGL origin es bottom-left; stb_image_write espera top-left
    stbi_flip_vertically_on_write(true);
    stbi_write_png(path, width, height, 3, pixels.data(), width * 3);
}
// Uso en el loop: if (glfwGetKey(window, GLFW_KEY_F12)) saveScreenshot("active/screenshot.png", W, H);
```

Claude ejecuta la app, presiona F12 programáticamente o espera N frames, lee `active/screenshot.png` con el Read tool y evalúa el resultado visual. **Funciona independientemente de la posición o foco de la ventana.**

### ⚠️ Assimp — solo para modelos estáticos (GLTF animado: estrategia de fallback)

Assimp tiene problemas documentados con skeletal animation en GLTF. Para el proyecto:

- **Uso principal de Assimp**: cargar todos los modelos **estáticos** (iceberg, edificios, turbina, auto, árbol, globo, fauna) — funciona perfectamente.
- **Oso polar** (único modelo animado): intentar carga con Assimp primero. Si el clip idle no carga correctamente, fallback a **animación procedural simple** (bob en Y con función seno — no requiere esqueleto).

Si el fallback se activa, el oso simplemente oscila suavemente — es aceptable y evita introducir cgltf como dependencia adicional. El proyecto no falla por esto.

### ✅ CMake FetchContent con tags congelados

Se mantiene FetchContent sobre vcpkg por simplicidad académica. **Cambio obligatorio**: congelar tags de versión exactos para reproducibilidad y evitar problemas de red.

```cmake
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4                          # TAG CONGELADO — no usar HEAD o main
)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_Declare(assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.4.3
)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.6
)
```

### ✅ stb_truetype — correcto para este caso de uso

Para los letreros del museo: texto en español, caracteres ASCII + acentos, atlas generado **una sola vez en Init()**, sin texto dinámico en tiempo real. stb_truetype es la elección correcta — más ligero que FreeType y sin dependencias adicionales.

---

## Pipeline de Desarrollo Autónomo (Claude Code)

Este es el loop que permite a Claude trabajar con retroalimentación visual real del motor OpenGL:

```
1. Claude modifica código C++/GLSL
2. Bash: cmake --build build --config Debug --parallel
3. Bash: .\build\Debug\CGEIHC.exe (en background, N frames)
4. La app guarda automáticamente active/screenshot.png (glReadPixels, frame N)
5. Claude lee active/screenshot.png con Read tool — ve el resultado 3D
6. Itera basándose en el resultado visual
```

### Scripts en `scripts/`

```powershell
# build-and-run.ps1 — Build y ejecutar
cmake --build build --config Debug --parallel
Start-Process ".\build\Debug\CGEIHC.exe"
```

La captura de screenshot ocurre **dentro de la app** (F12 o automático al frame 60) — no requiere script externo.

### Debugging GPU con RenderDoc

- Captura manual de frames para analizar pipeline, buffers y shaders
- In-App API para captura programática: `RENDERDOC_API_1_1_2`
- Útil para artefactos visuales que el debug callback de OpenGL no explica
- Activar en Debug builds: `glEnable(GL_DEBUG_OUTPUT)` + `glDebugMessageCallback`

---

## Estructura del Código `src/`

```
src/
├── main.cpp                    # Entry point, AppState { TITULO, JUGANDO, CIERRE, SALIR }
│
├── core/
│   ├── Window.h/.cpp           # Ventana GLFW, callbacks, swap buffers
│   ├── Input.h/.cpp            # Polling teclado/mouse
│   └── Time.h/.cpp             # deltaTime, frame pacing
│
├── graphics/
│   ├── Shader.h/.cpp           # Compilar GLSL, uniforms (setMat4, setFloat, setVec3)
│   ├── Texture.h/.cpp          # stb_image → GLuint
│   ├── Mesh.h/.cpp             # VAO/VBO/EBO, draw()
│   ├── Model.h/.cpp            # Assimp → vector<Mesh>
│   ├── CameraFPS.h/.cpp        # FPS camera, MVP, clamp AABB
│   ├── Skybox.h/.cpp           # Cubemap 6 caras, shader skybox
│   └── particles/
│       └── SnowSystem.h/.cpp   # 80-120 billboards, VBO dinámico CPU→GPU
│
├── animation/
│   ├── Animator.h/.cpp         # Skeletal animation (solo oso polar idle)
│   └── Lerp.h                  # mix(), clamp(), helpers
│
├── modules/
│   ├── ModuloBase.h/.cpp       # Trigger XZ, EstadoModulo, t, animate(), reset()
│   ├── ModuloIceberg.h/.cpp
│   ├── ModuloPolarBear.h/.cpp
│   ├── ModuloSeaLevel.h/.cpp
│   ├── ModuloTurbina.h/.cpp
│   ├── ModuloAuto.h/.cpp
│   ├── ModuloArbol.h/.cpp
│   └── ModuloGlobo.h/.cpp
│
├── scene/
│   ├── MuseumScene.h/.cpp      # std::vector<std::unique_ptr<ModuloBase>>, update/render
│   ├── ResourceManager.h/.cpp  # Caché modelos/texturas/shaders por path
│   ├── SignSystem.h/.cpp       # stb_truetype → textura unlit (generada en Init)
│   └── TriggerZone.h/.cpp      # Distancia XZ, módulo activo
│
├── ui/
│   ├── HUD.h/.cpp              # [E] Explorar (Dear ImGui, bottom-center)
│   ├── TitleScreen.h/.cpp      # Foto ártica + fade-in, Enter para comenzar
│   └── CreditsScreen.h/.cpp    # Pantalla de cierre desde M5
│
└── audio/
    └── AudioEngine.h/.cpp      # miniaudio: ambient loop + 3 fuentes 3D posicionales
```

---

## Pipeline de Assets

```
Sketchfab / Poly Haven
    ↓  [Descargar GLTF/OBJ + texturas CC0]
assets/models/ + assets/textures/
    ↓  [Assimp carga en tiempo de ejecución]
Motor OpenGL (src/graphics/Model.cpp)
    ↓  [VAO/VBO → Shaders GLSL → Renderizado]
Pantalla
```

### Fuentes de Assets

| Tipo | Fuente | Licencia |
|------|--------|----------|
| Modelos 3D | [Sketchfab](https://sketchfab.com/) | CC0 / Free |
| Texturas PBR | [ambientcg.com](https://ambientcg.com/) | CC0 |
| Skybox HDR | [polyhaven.com](https://polyhaven.com/) | CC0 |
| Audio | [freesound.org](https://freesound.org/) | CC0 |
| Fuente TTF | Roboto-Regular (Google Fonts) | Apache 2.0 |

---

## Dependencias CMake (FetchContent, tags congelados)

```cmake
include(FetchContent)

# GLFW — ventana y eventos
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)

# GLAD — cargador OpenGL 3.3 Core (generado en glad.dav1d.de)
# Se incluye directamente en src/ como headers (no FetchContent)

# GLM — matemáticas
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)

# Assimp — carga de modelos estáticos
FetchContent_Declare(assimp GIT_REPOSITORY https://github.com/assimp/assimp.git GIT_TAG v5.4.3)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

# Dear ImGui — UI (añadir backend GLFW+OpenGL3 manualmente en src/)
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG v1.91.6)

# stb — texturas, escritura de imágenes, truetype (header-only, copiar a src/vendor/)
# miniaudio — audio (header-only, copiar miniaudio.h a src/vendor/)

FetchContent_MakeAvailable(glfw glm assimp imgui)
```

**stb y miniaudio se incluyen como headers** directamente en `src/vendor/` — sin FetchContent, sin configuración de CMake.

---

## Configuración del Entorno

### Prerrequisitos

1. **Visual Studio 2022 Community** — workload "Desktop development with C++"
2. **CMake 3.20+** — `winget install Kitware.CMake`
3. **Git** — `winget install Git.Git`
4. **RenderDoc** — [renderdoc.org](https://renderdoc.org/) (debugging GPU)

### Build

```powershell
git clone https://github.com/jesuscottage/CGEIHC-P.git
cd CGEIHC-P
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel
.\build\Debug\CGEIHC.exe
```

---

## Empaquetado Final

- **InstallForge** (gratis, recomendado por el profesor) — crear `setup.exe`
- Incluir: ejecutable, assets/, `README.txt`
- Con miniaudio (header-only): **sin DLLs de audio que redistribuir**
- Solo DLLs necesarias: las de Assimp y GLFW (gestionadas por CMake)
