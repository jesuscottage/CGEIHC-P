# Plan Maestro de Desarrollo — CGEIHC-P

> Proyecto: Calentamiento global en el polo norte — Museo virtual 3D
> Última actualización: 2026-05-16
> Revisado por: Gemini + ChatGPT (ver `docs/reviews/plan-implementacion-consulta/`)
> Estado: ACTIVO — guía principal de implementación

---

## Propósito de este documento

Este plan es la guía operativa completa para que Claude Code implemente el proyecto de forma autónoma.
Contiene el orden de desarrollo, las tareas específicas por fase, el pipeline de verificación visual,
los criterios de salida por fase, y el protocolo para manejar errores no resolubles.

**Auditabilidad**: cada sección está escrita con suficiente detalle para que un agente externo
pueda verificar si una fase está completa y correcta, sin consultar documentos adicionales.

**Referencias**:
- `blueprints/01-09` — especificaciones detalladas de cada aspecto del proyecto
- `INFRAESTRUCTURA.md` — stack técnico y justificaciones
- `PENDIENTES.md` — checklist ejecutable por fase

---

## Reglas Globales de Desarrollo

Estas reglas aplican a **todas** las fases sin excepción:

1. **Separar `update(dt)` de `render()`** — nunca mezclar lógica con draw calls
2. **Solo dos niveles de herencia**: `ModuloBase → ModuloEspecifico` — sin ECS, sin herencia más profunda
3. **No implementar**: sombras dinámicas, SSR, PBR completo, IBL, normal mapping complejo, multithreading
4. **`glGetError()` instrumentado desde Fase 1** — activar `GL_DEBUG_OUTPUT` + `glDebugMessageCallback`
5. **Tags FetchContent congelados** — nunca usar `HEAD`, `main` o `latest` en dependencias
6. **Un modelo activo a la vez** — si el trigger se solapa, gana el módulo más cercano
7. **Assets congelados desde Fase 4** — no cambiar modelos ni texturas después de integrarlos
8. **1 unidad OpenGL = 1 metro**, eje Y = arriba, Y = 0 es el suelo

---

## Pipeline de Verificación Visual Autónoma

Este pipeline es el mecanismo central que permite a Claude verificar resultados 3D sin intervención humana.

### El ciclo base

```
┌─────────────────────────────────────────────────────────────────┐
│  1. Modificar código C++/GLSL                                   │
│  2. cmake --build build --config Debug --parallel               │
│     Si falla → leer errores del compilador → corregir → volver 1│
│  3. Ejecutar CGEIHC.exe en background (N frames o F12)          │
│  4. La app guarda active/screenshot.png (glReadPixels)          │
│  5. Claude lee active/screenshot.png con la herramienta Read    │
│  6. ¿Resultado correcto? → continuar a la siguiente tarea       │
│                         → No → identificar problema → volver 1  │
└─────────────────────────────────────────────────────────────────┘
```

### Implementación del screenshot autónomo

El sistema de screenshot debe implementarse en **Fase 1** y usarse en todas las fases posteriores.

```cpp
// En main.cpp o Window.cpp — función de captura
void saveScreenshot(const char* path, int width, int height) {
    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    stbi_flip_vertically_on_write(true);  // OpenGL origin = bottom-left
    stbi_write_png(path, width, height, 3, pixels.data(), width * 3);
}
```

**Modos de captura**:
- **Tecla F12**: captura manual al presionar F12 durante ejecución
- **Automático al frame N**: configurar `AUTO_SCREENSHOT_FRAME = 120` (2 s a 60 FPS)
  ```cpp
  if (frameCount == AUTO_SCREENSHOT_FRAME) {
      saveScreenshot("active/screenshot.png", SCREEN_W, SCREEN_H);
      glfwSetWindowShouldClose(window, GLFW_TRUE);  // cierra automáticamente
  }
  ```

**Uso para verificación autónoma**:
```bash
# Build
cmake --build build --config Debug --parallel

# Ejecutar y capturar automáticamente (cierra solo al frame 120)
./build/Debug/CGEIHC.exe

# Claude lee el resultado
# Read: active/screenshot.png
```

### Verificaciones por tipo de problema

| Problema | Qué buscar en el screenshot | Herramienta adicional |
|----------|----------------------------|----------------------|
| Geometría incorrecta | Formas deformadas, polígonos invertidos | Activar wireframe `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)` → screenshot |
| Texturas incorrectas | Color sólido (UV mapping fallido), negro (textura no cargada) | Log de stb_image |
| Iluminación | Superficies completamente negras o completamente blancas | Verificar normales con color debug |
| Profundidad/Z-fighting | Flickering en superficies cercanas | `glEnable(GL_DEPTH_TEST)` verificado |
| Transparencia | Orden incorrecto (objetos tapados por transparentes) | Verificar orden de render |
| Modelos Assimp | Mesh vacío o escala incorrecta | Log de Assimp con `aiProcess_Triangulate` |

### Protocolo de errores diferidos

Si después de **3 iteraciones** (código → screenshot → corrección) el problema no se resuelve:

1. **Registrar en `PENDIENTES.md`** con el formato:
   ```markdown
   ## ⚠️ Error Diferido — [Nombre del error]
   - **Fase**: [número de fase]
   - **Descripción**: [qué falla exactamente]
   - **Síntomas visuales**: [qué se ve en el screenshot]
   - **Intentos realizados**: [qué se probó]
   - **Impacto**: [qué funcionalidad bloquea]
   - **Propuesta de mitigación**: [alternativa más simple]
   ```
2. **Implementar la mitigación** si existe (por ejemplo: fallback procedural para el oso polar)
3. **Continuar** con la siguiente tarea de la fase

### RenderDoc para debugging GPU

Para problemas visuales que el debug callback de OpenGL no explica (artefactos, z-fighting complejo, shaders silenciosos):

```bash
# Captura programática — inicializar en debug builds
#include "renderdoc_app.h"
// En Window::init():
RENDERDOC_API_1_1_2* rdoc = nullptr;
// ... cargar la API ...
rdoc->StartFrameCapture(nullptr, nullptr);
// render frame
rdoc->EndFrameCapture(nullptr, nullptr);
```

Usar RenderDoc para:
- Inspeccionar VAO/VBO/EBO en un frame específico
- Verificar que los uniforms (MVP, viewPos, time) llegan al shader
- Depurar shaders de agua y Fresnel con vertex/fragment shader viewer

---

## Fase 0 — Bootstrap CMake

**Objetivo**: Build reproducible en VS2022 con todas las dependencias integradas mediante FetchContent.

**Tiempo estimado**: una sesión de trabajo.

### Tareas

#### 0.1 Crear estructura de directorios

```
CGEIHC-P/
├── src/
│   └── vendor/         # headers stb + miniaudio (header-only, sin FetchContent)
├── shaders/            # archivos .vert y .frag
├── assets/
│   ├── models/
│   ├── textures/
│   ├── audio/
│   ├── fonts/
│   └── skybox/
├── active/             # gitignored — screenshots temporales
├── build/              # gitignored — directorio de build
└── CMakeLists.txt
```

#### 0.2 Escribir CMakeLists.txt

Contenido completo requerido:

```cmake
cmake_minimum_required(VERSION 3.20)
project(CGEIHC VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW — tags congelados obligatorios
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
# GLM
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
# Assimp — solo para modelos estáticos
FetchContent_Declare(assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.4.3
)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)

# Dear ImGui (backend manual)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.6
)
FetchContent_MakeAvailable(glfw glm assimp imgui)

# GLAD — incluido manualmente en src/vendor/
# stb_image, stb_image_write, stb_truetype — en src/vendor/
# miniaudio — en src/vendor/

# Recopilar fuentes
file(GLOB_RECURSE SOURCES "src/*.cpp")
# Fuentes de ImGui backend
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(CGEIHC ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(CGEIHC PRIVATE
    src/
    src/vendor/
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(CGEIHC PRIVATE glfw glm::glm assimp)

# Copiar DLLs automáticamente al lado del .exe (Windows)
add_custom_command(TARGET CGEIHC POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:assimp>
        $<TARGET_FILE_DIR:CGEIHC>
)
```

#### 0.3 Descargar y colocar headers en `src/vendor/`

Archivos requeridos en `src/vendor/`:
- `glad.h` + `glad.c` — descargar desde [glad.dav1d.de](https://glad.dav1d.de/) con OpenGL 3.3 Core
- `KHR/khrplatform.h` — incluido con GLAD
- `stb_image.h` — [github.com/nothings/stb](https://github.com/nothings/stb)
- `stb_image_write.h` — mismo repositorio
- `stb_truetype.h` — mismo repositorio
- `miniaudio.h` — [miniaud.io](https://miniaud.io/) (versión 0.11+)

En exactamente **un** archivo `.cpp` (por ejemplo `src/vendor_impl.cpp`):
```cpp
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
```

#### 0.4 Crear `src/main.cpp` mínimo de prueba

```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "CGEIHC-P", nullptr, nullptr);
    if (!window) { std::cerr << "GLFW failed\n"; return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD failed\n"; return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.05f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
```

### Verificación de la Fase 0

**Comando de build**:
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel
```

**Criterio de salida**:
- [ ] Build sin errores ni warnings críticos
- [ ] `build/Debug/CGEIHC.exe` existe
- [ ] La ventana abre con fondo azul oscuro
- [ ] La ventana se cierra con Alt+F4 o la X

**Sin screenshot automático en esta fase** — verificación manual: ¿la ventana abre?

---

## Fase 1 — Motor Core y Ventana

**Objetivo**: Contexto OpenGL 3.3 estable, loop principal con `AppState`, cámara FPS funcional, y sistema de screenshot autónomo operativo.

### Tareas

#### 1.1 `src/core/Window.h/.cpp`

Responsabilidades:
- Crear y configurar ventana GLFW (1280×720, título "Nuestro Mundo")
- `GLFW_OPENGL_CORE_PROFILE`, version 3.3
- Framebuffer resize callback
- `swapBuffers()`, `pollEvents()`, `shouldClose()`, `getSize()`
- Activar **VSync**: `glfwSwapInterval(1)`
- Activar **debug output** de OpenGL:
  ```cpp
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glDebugCallbackFn, nullptr);
  // glDebugCallbackFn imprime: [source][type][severity] message
  ```

#### 1.2 `src/core/Input.h/.cpp`

Responsabilidades:
- `isKeyPressed(int glfwKey)` — polling simple, sin estado previo
- `isKeyJustPressed(int glfwKey)` — detecta flanco (requiere estado previo por tecla)
- `getMouseDelta()` → `glm::vec2` — diferencia desde el frame anterior
- `captureMouse(GLFWwindow*)` — `GLFW_CURSOR_DISABLED`
- `releaseMouse(GLFWwindow*)` — `GLFW_CURSOR_NORMAL`
- Configurar `glfwSetCursorPosCallback` para acumular delta

#### 1.3 `src/core/Time.h/.cpp`

Responsabilidades:
- `update()` — llamar al inicio de cada frame
- `getDeltaTime()` → `float` — clampeado a `[0.0001, 0.05]` para evitar spikes
- `getTime()` → `float` — `glfwGetTime()` desde el inicio

#### 1.4 `src/graphics/CameraFPS.h/.cpp`

Responsabilidades:
- Posición `glm::vec3 position` con Y fija = 1.7 m
- `yaw` y `pitch` con pitch clampeado a `[-80°, 80°]`
- `processInput(Input&, float dt)` — WASD a 4 m/s, mouse para rotación
- `getViewMatrix()` → `glm::mat4` — `glm::lookAt(position, position + front, up)`
- `getProjectionMatrix(float aspectRatio)` → `glm::mat4` — FOV=60°, near=0.1f, far=200.0f
- `clampAABB(float minX, float maxX, float minZ, float maxZ)` — aplicar después de movimiento
- Sensibilidad configurable: `MOUSE_SENSITIVITY = 0.1f`

#### 1.5 Sistema de screenshot autónomo

En `src/core/Window.h/.cpp` o en `main.cpp`:

```cpp
// Constantes de configuración
constexpr bool  AUTO_SCREENSHOT_ENABLED = true;
constexpr int   AUTO_SCREENSHOT_FRAME   = 120;  // frame 120 ≈ 2s a 60fps
constexpr char  SCREENSHOT_PATH[]       = "active/screenshot.png";

void saveScreenshot(const char* path, int width, int height) {
    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    stbi_flip_vertically_on_write(true);
    stbi_write_png(path, width, height, 3, pixels.data(), width * 3);
}
```

Uso en el loop:
```cpp
int frameCount = 0;
while (!window.shouldClose()) {
    // ... render ...
    frameCount++;
    if (AUTO_SCREENSHOT_ENABLED && frameCount == AUTO_SCREENSHOT_FRAME) {
        saveScreenshot(SCREENSHOT_PATH, 1280, 720);
        window.close();  // cierre automático para verificación autónoma
    }
    // F12 para captura manual
    if (input.isKeyJustPressed(GLFW_KEY_F12))
        saveScreenshot(SCREENSHOT_PATH, 1280, 720);
}
```

#### 1.6 `src/main.cpp` — AppState y loop principal

```cpp
enum class AppState { TITULO, JUGANDO, CIERRE, SALIR };
AppState state = AppState::TITULO;

// Loop principal
while (state != AppState::SALIR) {
    time.update();
    float dt = time.getDeltaTime();
    input.update();  // acumular mouse delta

    switch (state) {
        case AppState::TITULO:   updateTitulo(dt, input, state);  renderTitulo();  break;
        case AppState::JUGANDO:  updateJugando(dt, input, state); renderJugando(); break;
        case AppState::CIERRE:   updateCierre(dt, input, state);  renderCierre();  break;
        case AppState::SALIR:    break;
    }

    window.swapBuffers();
    window.pollEvents();
    frameCount++;
    // screenshot autónomo ...
}
```

### Verificación de la Fase 1

**Método**: build → ejecutar → screenshot automático al frame 120 → leer `active/screenshot.png`

**Qué debe verse**:
- Fondo de color sólido azul oscuro (`#0d1a2e` aproximado)
- Sin artefactos, sin ventana negra

**Verificación adicional de cámara** (manual o con tecla especial):
- WASD mueve la cámara
- Mouse rota sin gimbal lock
- Pitch se detiene a ±80°

**Criterio de salida**:
- [ ] Ventana abre correctamente a 1280×720
- [ ] Screenshot guardado en `active/screenshot.png`
- [ ] Cámara FPS responde a WASD + mouse
- [ ] `getDeltaTime()` estable (verificar con `printf` al inicio del loop)
- [ ] Debug callback de OpenGL activo (verificar que no imprime errores al arranque)

---

## Fase 2 — Pipeline de Render Básico

**Objetivo**: Shader manager, Mesh con VAO/VBO/EBO, carga de texturas con stb_image, matrices MVP correctas. El criterio de salida es un cubo texturizado renderizado correctamente.

### Tareas

#### 2.1 `src/graphics/Shader.h/.cpp`

```cpp
class Shader {
public:
    bool load(const char* vertPath, const char* fragPath);
    void use();
    void setMat4(const char* name, const glm::mat4& mat);
    void setVec3(const char* name, const glm::vec3& vec);
    void setFloat(const char* name, float val);
    void setInt(const char* name, int val);
private:
    GLuint programID = 0;
    // Leer archivo → string → compilar → linkear → log de errores legible
};
```

Requiere:
- Log de errores **completo y legible**: `glGetShaderInfoLog`, `glGetProgramInfoLog`
- Verificar `GL_COMPILE_STATUS` y `GL_LINK_STATUS` — fallar ruidosamente con `std::cerr`

#### 2.2 `src/graphics/Texture.h/.cpp`

```cpp
class Texture {
public:
    bool loadFromFile(const char* path, bool flipVertically = true);
    void bind(int textureUnit = 0);
    GLuint getID() const;
private:
    GLuint textureID = 0;
};
```

Requiere:
- `stbi_set_flip_vertically_on_load(flipVertically)` antes de cargar
- `GL_RGBA` como formato interno (soporta PNG con alpha)
- `GL_LINEAR_MIPMAP_LINEAR` + `glGenerateMipmap`
- Log de error si `stbi_load` retorna null

#### 2.3 `src/graphics/Mesh.h/.cpp`

```cpp
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void draw(const Shader& shader);
private:
    GLuint VAO, VBO, EBO;
    unsigned int indexCount;
};
```

#### 2.4 Shaders de prueba (`shaders/test.vert` / `shaders/test.frag`)

Shader mínimo para verificar MVP:

```glsl
// test.vert
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}

// test.frag
#version 330 core
in vec2 TexCoord;
uniform sampler2D texture1;
out vec4 FragColor;
void main() {
    FragColor = texture(texture1, TexCoord);
}
```

#### 2.5 Cubo de prueba en `main.cpp`

Construir un cubo con vertices manuales (8 vértices, 36 índices) → cargarlo como `Mesh` → renderizarlo con una textura de prueba y matrices MVP.

Transformaciones de prueba:
- **Model**: rotación lenta en Y (`glm::rotate(glm::mat4(1.0f), time * 30°, up)`)
- **View**: cámara FPS
- **Projection**: perspectiva FOV=60°

### Verificación de la Fase 2

**Screenshot esperado**: cubo texturizado centrado en pantalla, girando lentamente, con perspectiva visible.

**Checklist visual**:
- [ ] El cubo tiene textura visible (no negro, no magenta)
- [ ] Las caras distantes se ven más pequeñas (perspectiva correcta)
- [ ] El cubo no está invertido (cara frontal visible)
- [ ] No hay z-fighting entre caras del cubo

**Debug de problemas comunes**:
- **Negro**: textura no cargó → revisar ruta, log de stb_image
- **Magenta**: convención de "textura faltante" — mismo problema
- **Cubo aplanado/deformado**: aspecto ratio incorrecto en Projection → `width/height` como float
- **Caras faltantes**: face culling activado sin normales correctas → `glDisable(GL_CULL_FACE)` temporalmente

---

## Fase 3 — Asset Pipeline

**Objetivo**: Assimp GLTF funcional + ResourceManager con caché. Un modelo GLTF de Sketchfab renderiza correctamente.

### Tareas

#### 3.1 `src/graphics/Model.h/.cpp`

```cpp
class Model {
public:
    bool loadFromFile(const char* path);
    void draw(const Shader& shader);
private:
    std::vector<Mesh> meshes;
    std::string directory;

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    GLuint loadMaterialTexture(aiMaterial* mat, aiTextureType type);
};
```

Flags de importación de Assimp:
```cpp
unsigned int flags = aiProcess_Triangulate
                   | aiProcess_GenSmoothNormals   // normales para Blinn-Phong
                   | aiProcess_FlipUVs             // UV convention OpenGL
                   | aiProcess_CalcTangentSpace;
```

#### 3.2 `src/scene/ResourceManager.h/.cpp`

```cpp
class ResourceManager {
public:
    static Model*   loadModel(const std::string& path);
    static Texture* loadTexture(const std::string& path);
    static Shader*  loadShader(const std::string& name,
                               const std::string& vertPath,
                               const std::string& fragPath);
    static void clear();  // liberar todo al cerrar

private:
    static std::unordered_map<std::string, std::unique_ptr<Model>>   models;
    static std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
    static std::unordered_map<std::string, std::unique_ptr<Shader>>  shaders;
};
```

#### 3.3 Descargar modelo de prueba

Descargar un modelo GLTF simple de Sketchfab CC0 (por ejemplo un iceberg o un objeto ártico simple).
- Formato: `.glb` (todo en un archivo) preferido sobre `.gltf` separado
- Colocar en `assets/models/test/`

#### 3.4 Renderizar modelo de prueba

En `main.cpp`:
```cpp
Model* model = ResourceManager::loadModel("assets/models/test/iceberg.glb");
// En render:
shader.use();
glm::mat4 modelMatrix = glm::mat4(1.0f);
shader.setMat4("model", modelMatrix);
shader.setMat4("view", camera.getViewMatrix());
shader.setMat4("projection", camera.getProjectionMatrix(1280.0f/720.0f));
model->draw(shader);
```

### Verificación de la Fase 3

**Screenshot esperado**: modelo GLTF renderizado con su textura, sin deformaciones.

**Checklist**:
- [ ] El modelo tiene textura (no negro ni magenta)
- [ ] La escala es razonable (no microscópico ni enorme)
- [ ] Sin artefactos de z-fighting
- [ ] `glGetError()` no reporta errores después de la carga
- [ ] Log de Assimp no reporta warnings críticos

**Riesgo alto — Assimp GLTF con animación skeletal**:
- En esta fase solo cargar modelos **estáticos**
- Si el modelo elegido tiene animaciones, ignorarlas — no implementar `Animator` aún
- El oso polar (Fase 5) es el único modelo animado y tiene su propio plan de mitigación

---

## Fase 4 — Layout del Museo

**Objetivo**: Suelo nevado, 7 plataformas circulares, límites AABB, letreros informativos y flechas de suelo. El museo es navegable.

### Tareas

#### 4.1 Geometría del suelo

Plano único que cubre toda la escena:
- Dimensiones: 200 × 200 m (cubre vestíbulo + 2 corredores + M5 con margen)
- Vértices: grid de 64×64 subdivisiones para el displacement shader (Fase 7)
- UV: tileadas para textura de hielo (escala de UV = 0.1 para tile de 10m)

```cpp
// Construir plano de terreno programáticamente en Mesh
Mesh createTerrainPlane(int subdivisionsX, int subdivisionsZ, float width, float depth);
```

#### 4.2 Geometría de plataformas circulares (7 módulos)

Cada plataforma:
- Disco circular con 32 segmentos, radio = 5 m, elevación = 0.3 m sobre el suelo
- Generada proceduralmente como `Mesh` en CPU

```cpp
Mesh createCirclePlatform(int segments, float radius, float yOffset);
```

Posiciones de plataformas (sistema de coordenadas: vestíbulo en origen, corredores en Z positivo):

| Módulo | Posición (x, z) | Corredor |
|--------|-----------------|----------|
| M1 izq | (-12, 15) | Izquierdo |
| M2 izq | (-12, 30) | Izquierdo |
| M3 izq | (-12, 45) | Izquierdo |
| M1 der | (+12, 15) | Derecho |
| M2 der | (+12, 30) | Derecho |
| M3 der | (+12, 45) | Derecho |
| M5     | (0, 65)   | Central |

#### 4.3 Límites AABB del jugador

```cpp
struct MuseumBounds {
    // Vestíbulo
    static constexpr float VESTIBULO_MIN_X = -10.0f;
    static constexpr float VESTIBULO_MAX_X =  10.0f;
    // Corredor izquierdo
    static constexpr float CORREDOR_IZQ_MIN_X = -20.0f;
    static constexpr float CORREDOR_IZQ_MAX_X =  -4.0f;
    // Corredor derecho
    static constexpr float CORREDOR_DER_MIN_X =   4.0f;
    static constexpr float CORREDOR_DER_MAX_X =  20.0f;
    // Z global
    static constexpr float MIN_Z = -5.0f;
    static constexpr float MAX_Z = 75.0f;
};
```

Aplicar como clamp en `CameraFPS::processInput()` o en el update principal.

#### 4.4 `src/scene/SignSystem.h/.cpp` — Letreros informativos

Sistema de letreros 3D con texto generado por stb_truetype:

```cpp
class SignSystem {
public:
    void init(const char* fontPath);  // carga Roboto-Regular.ttf
    // Genera textura para un letrero específico
    GLuint createSign(const std::string& title,
                      const std::string& dataText,
                      const std::string& description,
                      const std::string& source);
    void renderSign(GLuint textureID, const glm::mat4& modelMatrix,
                    const Shader& unlitShader);
private:
    stbtt_fontinfo fontInfo;
    std::vector<unsigned char> fontBuffer;
    // Atlas de píxeles: 512×256 por letrero
};
```

**Proceso de generación de textura de letrero**:
1. `stbtt_InitFont()` con buffer del archivo TTF
2. Crear buffer RGBA de 512×256 píxeles, fondo = `#0d2137` (azul oscuro)
3. Dibujar borde cyan de 2px con `memset` en filas/columnas del borde
4. Rasterizar cada línea de texto con `stbtt_GetBakedQuad` (atlas de glifos precalculado)
5. `glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 256, ...)` → subir a GPU
6. Liberar buffer CPU

Letreros posicionados 2 m detrás del modelo principal del módulo (respecto al corredor), a y = 1.4 m (centro), orientación fija hacia el corredor.

#### 4.5 Flechas en el suelo

- 2 flechas en el vestíbulo: planos 0.8×0.4 m, y = 0.01 m, con textura PNG
- Textura: generar con stb_truetype o incluir PNG pre-generado con GIMP
- Texto: `"← Las consecuencias"` y `"Las soluciones →"`
- Shader: unlit con alpha blending

#### 4.6 Depuración de triggers (visualización de debug)

Durante el desarrollo (Fase 4-5), renderizar círculos wireframe de los triggers para verificar posición:
```cpp
#ifdef DEBUG_TRIGGERS
// Renderizar círculo r=5m en xz para cada módulo con shader de color sólido
renderDebugCircle(modulePos, 5.0f, glm::vec3(0,1,0), shader);
#endif
```

### Verificación de la Fase 4

**Screenshots requeridos** (capturar desde distintas posiciones de cámara):

1. **Vista desde el vestíbulo** (`position = (0, 1.7, -3)`, mirando hacia Z+): deben verse las flechas en el suelo y el inicio de los dos corredores
2. **Vista desde M1 izq** (`position = (-12, 1.7, 13)`, mirando hacia el módulo): plataforma circular visible con borde emisivo, letrero a la derecha
3. **Vista desde M5** (`position = (0, 1.7, 62)`, mirando atrás): convergencia de los dos corredores visible
4. **Modo wireframe** (activar con tecla W temporal): verificar geometría del plano de suelo

**Checklist**:
- [ ] Suelo visible con textura de hielo
- [ ] 7 plataformas en posiciones correctas
- [ ] Plataformas elevadas 0.3 m sobre el suelo
- [ ] Borde emisivo de plataformas visible
- [ ] Letreros con texto legible (aunque sea texto de prueba)
- [ ] Flechas en el suelo visibles con texto
- [ ] Jugador no puede salir de los límites AABB
- [ ] Y de la cámara fija en 1.7 m (no cae, no sube)

---

## Fase 5 — Sistema de Módulos

**Objetivo**: `ModuloBase` + `TriggerZone` + los 7 módulos específicos con animación LERP activada por tecla E.

### Tareas

#### 5.1 `src/modules/ModuloBase.h/.cpp`

```cpp
enum class EstadoModulo { INACTIVO, ANIMANDO, COMPLETADO };

class ModuloBase {
public:
    ModuloBase(glm::vec3 position, float triggerRadius = 5.0f);

    void update(float dt, const glm::vec3& playerPos, bool ePressed);
    virtual void render(const Shader& shader) = 0;
    virtual void reset() = 0;

    bool isPlayerInTrigger() const;
    EstadoModulo getEstado() const;

protected:
    virtual void animate(float normalizedT) = 0;  // t ∈ [0, 1]

    glm::vec3 position;
    float triggerRadius;
    float animDuration;   // duración total de la animación en segundos
    float t = 0.0f;       // tiempo normalizado ∈ [0, 1]
    float elapsedTime = 0.0f;
    EstadoModulo estado = EstadoModulo::INACTIVO;
    bool playerWasInside = false;
};
```

**Lógica de `update()`**:
```cpp
void ModuloBase::update(float dt, const glm::vec3& playerPos, bool ePressed) {
    float dist = glm::length(glm::vec2(playerPos.x - position.x,
                                       playerPos.z - position.z));
    bool playerInside = (dist < triggerRadius);

    // Reset al salir
    if (playerWasInside && !playerInside) {
        reset();
        estado = EstadoModulo::INACTIVO;
        elapsedTime = 0.0f;
        t = 0.0f;
    }

    // Activar con E
    if (playerInside && ePressed && estado == EstadoModulo::INACTIVO)
        estado = EstadoModulo::ANIMANDO;

    // Avanzar animación
    if (estado == EstadoModulo::ANIMANDO) {
        elapsedTime += dt;
        t = glm::clamp(elapsedTime / animDuration, 0.0f, 1.0f);
        animate(t);
        if (t >= 1.0f) estado = EstadoModulo::COMPLETADO;
    }

    playerWasInside = playerInside;
}
```

#### 5.2 `src/scene/TriggerZone.h/.cpp`

Determina qué módulo está activo (el más cercano dentro de su radio):

```cpp
class TriggerZone {
public:
    ModuloBase* getActiveModule(const glm::vec3& playerPos,
                                std::vector<std::unique_ptr<ModuloBase>>& modules);
};
```

#### 5.3 Los 7 módulos específicos

##### `ModuloIceberg` (M1 izq)
```cpp
class ModuloIceberg : public ModuloBase {
    // animDuration = 10.0f
    void animate(float t) override {
        glm::vec3 targetScale = glm::mix(glm::vec3(1.0f),
                                          glm::vec3(0.3f, 0.1f, 0.3f), t);
        icebergModel.setScale(targetScale);
    }
    void reset() override { icebergModel.setScale(glm::vec3(1.0f)); }
};
```

##### `ModuloPolarBear` (M2 izq)
```cpp
class ModuloPolarBear : public ModuloBase {
    // animDuration = 8.0f
    // El oso tiene animación idle GLTF en loop (independiente del LERP)
    // El LERP anima el disco de hielo en XZ
    void animate(float t) override {
        float iceScale = glm::mix(1.0f, 0.2f, t);
        iceDisc.setScale(glm::vec3(iceScale, 1.0f, iceScale));
    }
    void reset() override { iceDisc.setScale(glm::vec3(1.0f)); }
};
```

**Nota importante sobre el oso polar**: la animación idle del GLTF se ejecuta en loop desde el
arranque. El LERP de tecla E anima **el disco de hielo**, no el oso. Si la animación GLTF del
oso falla, aplicar fallback de bob en Y (ver sección de Riesgos).

##### `ModuloSeaLevel` (M3 izq)
```cpp
void animate(float t) override {
    float waterY = glm::mix(-1.0f, 2.5f, t);
    waterPlane.setPositionY(waterY);
}
```

##### `ModuloTurbina` (M1 der)
```cpp
// animDuration = 5.0f para el LERP de velocidad
float rotSpeed = 0.0f;  // grados/segundo
void animate(float t) override {
    rotSpeed = glm::mix(0.0f, 120.0f, t);
}
void update(float dt, ...) override {
    ModuloBase::update(dt, playerPos, ePressed);
    // Rotación acumulada (independiente del LERP)
    blades.rotation.y += rotSpeed * dt;
}
```

##### `ModuloAuto` (M2 der)
```cpp
// emissive lerp: 2s; carZ lerp: 6s (ping-pong A→B→A→B)
void animate(float t) override {
    emissive = glm::mix(0.0f, 1.0f, glm::min(t * 3.0f, 1.0f));  // primeros 2s
    // Movimiento ping-pong en Z
    float pingPong = glm::abs(glm::sin(t * glm::pi<float>()));
    carZ = glm::mix(zA, zB, pingPong);
}
```

##### `ModuloArbol` (M3 der)
```cpp
// animDuration = 8.0f
void animate(float t) override {
    float scale = glm::mix(0.05f, 1.0f, t);
    tree.setScale(glm::vec3(scale));
    // Actualizar sistema de partículas CO₂
    co2Particles.setActive(t > 0.0f);
    co2Particles.setIntensity(1.0f - t);  // más CO₂ capturado = menos partículas
}
```

##### `ModuloGlobo` (M5)
```cpp
// Rotación constante en Y (siempre activa, independiente de E)
// LERP de alpha secuencial de N líneas de conexión
void update(float dt, ...) override {
    ModuloBase::update(dt, playerPos, ePressed);
    globeRotation += 15.0f * dt;  // 15°/s en Y
}
void animate(float t) override {
    // Activar líneas secuencialmente: cada 0.5s aparece una nueva
    for (int i = 0; i < connectionLines.size(); i++) {
        float lineT = glm::clamp((t * animDuration - i * 0.5f) / 0.3f, 0.0f, 1.0f);
        connectionLines[i].alpha = lineT;
    }
}
```

#### 5.4 `src/scene/MuseumScene.h/.cpp`

```cpp
class MuseumScene {
public:
    void init(ResourceManager& rm);
    void update(float dt, const glm::vec3& playerPos, bool ePressed);
    void render(const Shader& standardShader, const Shader& unlitShader,
                const Shader& waterShader);

    ModuloBase* getActiveModule();
    bool isM5Completado() const;

private:
    std::vector<std::unique_ptr<ModuloBase>> modules;
    TriggerZone triggerZone;
    // Geometría estática (suelo, plataformas, letreros, flechas)
};
```

### Verificación de la Fase 5

**Screenshots requeridos por módulo**:

Para cada módulo (M1 izq, M2 izq, M3 izq, M1 der, M2 der, M3 der, M5):
1. **Estado INACTIVO**: screenshot con el módulo en su estado inicial
2. **Estado ANIMANDO al 50%**: screenshot en mitad de la animación
3. **Estado COMPLETADO**: screenshot con la animación terminada

Total: 21 screenshots de referencia (guardar en `active/fase5/`)

**Checklist**:
- [ ] Todos los módulos están en sus posiciones correctas (revisar blueprint 02)
- [ ] E solo funciona dentro del trigger (r=5m)
- [ ] E no funciona dos veces seguidas para el mismo módulo
- [ ] Reset funciona al salir del trigger (instantáneo)
- [ ] Solo un módulo activo a la vez
- [ ] Las animaciones LERP avanzan correctamente en el tiempo dado
- [ ] Indicador `[E] Explorar` aparece al entrar en trigger (aunque ImGui se implementa en Fase 9)

**Debug durante Fase 5**: activar círculos wireframe de triggers con `#define DEBUG_TRIGGERS`.

---

## Fase 6 — Iluminación Blinn-Phong

**Objetivo**: Shader principal con iluminación direccional Blinn-Phong y normales correctas en todos los modelos.

### Tareas

#### 6.1 `shaders/standard.vert`

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;  // transpose(inverse(mat3(model)))

out vec3 FragPos;   // en world space
out vec3 Normal;    // en world space
out vec2 TexCoord;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    Normal = normalMatrix * aNormal;
    TexCoord = aTexCoord;
    gl_Position = projection * view * worldPos;
}
```

#### 6.2 `shaders/standard.frag` — Blinn-Phong

```glsl
#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture_diffuse;
uniform vec3 lightDir;     // dirección normalizada de la luz (sol ártico)
uniform vec3 lightColor;   // color de la luz
uniform vec3 viewPos;      // posición de la cámara
uniform float ambientStr;  // ~0.3 para interior ártico

out vec4 FragColor;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);

    // Ambiente
    vec3 ambient = ambientStr * lightColor;

    // Difuso (Lambert)
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // Especular (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), 32.0);
    vec3 specular = spec * lightColor * 0.5;

    vec4 texColor = texture(texture_diffuse, TexCoord);
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor = vec4(result, texColor.a);
}
```

#### 6.3 Configuración de la luz

```cpp
// Luz direccional — sol ártico bajo en el horizonte
glm::vec3 lightDir = glm::normalize(glm::vec3(0.3f, -0.8f, 0.5f));
glm::vec3 lightColor = glm::vec3(1.0f, 0.95f, 0.85f);  // luz cálida-fría ártica
float ambientStr = 0.35f;  // sin sombras duras — ambiente ártico

// Enviar cada frame
shader.setVec3("lightDir", lightDir);
shader.setVec3("lightColor", lightColor);
shader.setVec3("viewPos", camera.getPosition());
shader.setFloat("ambientStr", ambientStr);

// Normal matrix — recalcular si el model matrix cambia
glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
shader.setMat3("normalMatrix", normalMatrix);
```

### Verificación de la Fase 6

**Screenshot esperado**: escena con gradientes de luz y sombra visibles, specular en ángulos oblicuos.

**Checklist**:
- [ ] Las caras opuestas a la luz son más oscuras (no todas del mismo brillo)
- [ ] El specular (punto brillante) es visible en superficies cuando la cámara está en ángulo
- [ ] El suelo tiene variación de brillo correcta
- [ ] Los modelos GLTF reciben iluminación correcta (normales correctas desde Assimp)
- [ ] Sin caras completamente negras (luz ambiental activa)
- [ ] Sin caras completamente blancas (oversaturation)

**Debug de normales**:
Si las superficies se ven extrañas, verificar normales con debug shader:
```glsl
// fragment shader de debug
FragColor = vec4(Normal * 0.5 + 0.5, 1.0);  // RGB = XYZ de la normal
```
- Rojo puro = normal apunta en +X
- Verde puro = normal apunta en +Y (correcto para el suelo)
- Azul puro = normal apunta en +Z

---

## Fase 7 — Shaders Avanzados

**Objetivo**: Fresnel en el shader principal, agua procedural (shader del profesor adaptado), skybox cubemap, y niebla exponencial.

### Tareas

#### 7.1 Fresnel en `shaders/standard.frag`

Agregar al final del fragment shader:

```glsl
// Fresnel — efecto borde para hielo y agua
uniform float fresnelStrength;  // 0.0 para objetos sin Fresnel; ~0.8 para hielo/agua
uniform vec3 fresnelColor;      // color del borde Fresnel

float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 3.0);
vec3 fresnelContrib = fresnel * fresnelStrength * fresnelColor;
result += fresnelContrib;
```

Para el hielo: `fresnelStrength = 0.6`, `fresnelColor = vec3(0.5, 0.8, 1.0)` (azul-blanco)
Para el suelo: `fresnelStrength = 0.0` (sin Fresnel)

#### 7.2 `shaders/water.vert` / `shaders/water.frag`

Basado directamente en `13_wavesAnimation-fresnel.vs/.fs` del profesor. Adaptar:

```glsl
// water.vert — displacement vertical en vértices
uniform float time;
uniform float waveAmplitude;  // ~0.15 m
uniform float waveFrequency;  // ~0.5

void main() {
    vec3 pos = aPos;
    pos.y += waveAmplitude * sin(waveFrequency * pos.x + time)
           + waveAmplitude * 0.5 * sin(waveFrequency * pos.z * 1.3 + time * 0.7);
    gl_Position = projection * view * model * vec4(pos, 1.0);
    // ... pasar FragPos, Normal, TexCoord ...
}
```

```glsl
// water.frag — color + Fresnel (del shader del profesor)
uniform float time;

void main() {
    vec3 waterColor = vec3(0.1f, 0.23f, 0.36f);  // #1a3a5c
    // Blinn-Phong aplicado al agua
    // Fresnel fuerte en bordes
    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 2.5);
    vec3 result = mix(waterColor, vec3(0.7, 0.9, 1.0), fresnel * 0.6);
    FragColor = vec4(result, 0.85);  // semi-transparente
}
```

Enviar `time` cada frame: `waterShader.setFloat("time", (float)glfwGetTime())`

#### 7.3 `src/graphics/Skybox.h/.cpp`

```cpp
class Skybox {
public:
    bool load(const std::vector<std::string>& facePaths);
    // facePaths: {right, left, top, bottom, front, back}
    void draw(const Shader& skyboxShader, const glm::mat4& view,
              const glm::mat4& projection);
private:
    GLuint VAO, VBO, textureID;
};
```

**Shader de skybox** (`shaders/skybox.vert`/`.frag`):
```glsl
// skybox.vert — truco de profundidad: gl_Position = pos.xyww
void main() {
    vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
    gl_Position = pos.xyww;  // w == z → siempre en profundidad máxima
    TexCoords = aPos;
}

// skybox.frag
uniform samplerCube skybox;
void main() { FragColor = texture(skybox, TexCoords); }
```

**Preparación del HDR de Poly Haven**:
```bash
# Descargar aurora_borealis.hdr de polyhaven.com
# Convertir a 6 caras PNG con cmgen (incluido en Filament tools)
cmgen --format=png --size=512 aurora_borealis.hdr
# → genera: px.png, nx.png, py.png, ny.png, pz.png, nz.png
# Colocar en assets/skybox/
```

Alternativa si cmgen no está disponible: usar stb_image para cargar HDR equirectangular
y convertir a cubemap con un shader de conversión (un render pass extra).

#### 7.4 Niebla exponencial en `standard.frag`

```glsl
// Al final de main() en standard.frag
uniform vec3  fogColor;    // #e8eef2 — blanco-gris muy pálido
uniform float fogDensity;  // ~0.008

float dist = length(FragPos - viewPos);
float fogFactor = exp(-fogDensity * dist);
result = mix(fogColor, result, fogFactor);
FragColor = vec4(result, texColor.a);
```

#### 7.5 Orden de render con transparencia

Desde esta fase en adelante, orden de render **obligatorio** en cada frame:

```
1. glDepthMask(GL_TRUE)   → renderizar opacos (suelo, plataformas, modelos)
2. Skybox                  → depth trick xyww, glDepthMask(GL_TRUE)
3. glDepthMask(GL_FALSE)  → renderizar transparentes (agua, nieve, letreros)
   3a. Agua (semi-transparente)
   3b. Partículas de nieve (alpha blending)
   3c. Letreros (alpha blending con fondo semi-opaco)
4. glDepthMask(GL_TRUE)   → restaurar
5. ImGui                  → siempre al último
```

### Verificación de la Fase 7

**Screenshots requeridos**:

1. **Skybox**: vista hacia el cielo — aurora boreal visible, gradiente azul ártico
2. **Agua con ondas**: M3-izq con `waterY = -0.5` — ondas visibles en el plano de agua
3. **Fresnel**: vista oblicua del plano de agua — borde blanco-azulado visible en los bordes
4. **Niebla**: vista desde el vestíbulo hacia el horizonte — los módulos lejanos (M3, M5) difuminados

**Checklist**:
- [ ] Skybox se ve sin artefactos de profundidad (no tapa los modelos)
- [ ] Agua tiene movimiento visible de ondas (seno en vértices)
- [ ] Fresnel visible en ángulos oblicuos del agua
- [ ] Niebla suaviza el horizonte sin afectar objetos cercanos (r < 30 m)
- [ ] No hay z-fighting entre agua y suelo
- [ ] ImGui no rompe el estado de profundidad (verificar que `glEnable(GL_DEPTH_TEST)` está activo al inicio del siguiente frame)

---

## Fase 8 — Efectos y Fauna

**Objetivo**: Sistema de partículas de nieve, fauna decorativa, y partículas de módulos (turbina y CO₂).

### Tareas

#### 8.1 `src/graphics/particles/SnowSystem.h/.cpp`

```cpp
struct SnowParticle {
    glm::vec3 position;
    float speed;      // velocidad de caída m/s
    float drift;      // amplitud del drift sinusoidal
    float phase;      // fase aleatoria del seno
    float alpha;      // transparencia actual
};

class SnowSystem {
public:
    void init(int count = 100);          // 80-120 partículas
    void update(float dt, const glm::vec3& cameraPos);
    void uploadGPU();                    // glBufferSubData
    void render(const Shader& unlitShader, const glm::mat4& view,
                const glm::mat4& projection, const glm::vec3& cameraPos);

private:
    std::vector<SnowParticle> particles;
    GLuint VAO, VBO;
    // VBO dinámico: actualizar posiciones cada frame
};
```

**Lógica de partícula**:
```cpp
void updateParticle(SnowParticle& p, float dt, float time) {
    p.position.y -= p.speed * dt;
    p.position.x += p.drift * sin(time * 0.5f + p.phase);

    if (p.position.y < 0.0f) {
        // Reciclar: mover a la parte superior
        p.position.y = 8.0f;
        p.position.x = randomFloat(-20, 20);  // relativo a la cámara
        p.position.z = randomFloat(-20, 20);
    }
}
```

**Renderizado como billboard**:
- El quad siempre mira hacia la cámara: `mat4 = glm::lookAt` inverso
- Tamaño: 0.05 m × 0.05 m
- Sin textura — shader unlit con punto blanco y alpha 0.6

#### 8.2 Fauna decorativa

Fauna puramente estática — sin trigger, sin animación (excepto oso polar en M2):

| Animal | Archivo | Instancias | Posiciones sugeridas |
|--------|---------|------------|----------------------|
| Foca anillada | `assets/models/fauna/seal.glb` | 4-6 | Fuera de límites AABB, x ∈ [-30, -25] o [25, 30] |
| Zorro ártico | `assets/models/fauna/arctic_fox.glb` | 2-3 | Bordes lejanos, z > 70 |
| Gaviota (billboard) | PNG transparente de gaviota | 6-8 | y ∈ [4, 6], posición fija |

Renderizar con el mismo shader `standard` con Blinn-Phong, con model matrix de escala fija.

#### 8.3 Partículas de módulos

**Turbina (M1 der)** — billboard particles de energía limpia:
```cpp
// Partículas blancas pequeñas (0.03 m) emitidas desde la punta de las aspas
// Movimiento: hacia arriba y hacia afuera con velocidad proporcional a rotSpeed
// Activas solo cuando estado == ANIMANDO || COMPLETADO
```

**CO₂ espiral (M3 der)** — partículas grises que caen hacia el árbol:
```cpp
// Ángulo golden ratio: phi = i * 2.399963...
// Posición: esfera de r decreciente mientras t avanza (más carbono capturado)
float r = maxRadius * (1.0f - treeScale);
float x = r * cos(phi * i) + tree.position.x;
float z = r * sin(phi * i) + tree.position.z;
float y = initialY * (1.0f - t) + tree.position.y * t;  // cae hacia el árbol
```

### Verificación de la Fase 8

**Screenshots**:
1. **Nieve**: vista general con partículas de nieve visibles
2. **Fauna**: foca y zorro ártico posicionados en el entorno
3. **Turbina girando**: M1 der en estado ANIMANDO con partículas
4. **Árbol con CO₂**: M3 der en estado ANIMANDO con espiral de partículas

**Checklist**:
- [ ] 80-120 copos de nieve visibles simultáneamente sin lag
- [ ] Partículas de nieve se reciclan correctamente (no desaparecen acumuladas en y=0)
- [ ] Fauna renderizada en posiciones correctas (fuera del área jugable)
- [ ] Partículas de turbina activas cuando M1-der está ANIMANDO
- [ ] CO₂ espiral visible en M3-der durante animación
- [ ] Sin degradación de FPS notable con todas las partículas activas

---

## Fase 9 — UI, Texto y Audio

**Objetivo**: Dear ImGui para pantallas y HUD, letreros con texto real (stb_truetype), y audio con miniaudio.

### Tareas

#### 9.1 Inicializar Dear ImGui con backend GLFW+OpenGL3

```cpp
// En Window::init() — DESPUÉS de gladLoadGLLoader
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGui::StyleColorsDark();
ImGui_ImplGlfw_InitForOpenGL(window, true);
ImGui_ImplOpenGL3_Init("#version 330");

// Cargar fuente Roboto
ImGuiIO& io = ImGui::GetIO();
io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 18.0f);
// Agregar tamaños adicionales para distintos usos
```

#### 9.2 `src/ui/TitleScreen.h/.cpp`

Elementos:
- Fondo: quad fullscreen con textura de foto ártica (PNG de Unsplash/Pexels CC0)
- Overlay negro semitransparente (alpha ~0.45)
- Texto ImGui:
  - "Nuestro Mundo" — Roboto 48px, centrado
  - "Calentamiento global en el Polo Norte" — 22px, centrado
  - "[Enter] Comenzar" — 18px, centrado en parte inferior

```cpp
void TitleScreen::render(float fadeAlpha) {
    // Render quad con foto ártica (shader unlit fullscreen)
    renderFullscreenQuad(arcticTexture, overlayAlpha);

    // ImGui overlay
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // ... ventana ImGui fullscreen sin decoración ...
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Restaurar estado OpenGL después de ImGui
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}
```

#### 9.3 `src/ui/HUD.h/.cpp`

```cpp
void HUD::renderInteractPrompt(bool visible) {
    if (!visible) return;
    // Ventana ImGui bottom-center
    // Sin decoración (sin título, sin resize)
    // Fondo #0d2137 alpha 0.75
    // Texto "[E] Explorar" con Roboto 18px
}
```

#### 9.4 `src/ui/CreditsScreen.h/.cpp`

Pantalla de cierre — se activa desde M5 con Escape:
- Fade a negro desde la escena (0.5 s)
- Texto grande: *"El Ártico no espera. Tú tampoco tienes que hacerlo."*
- Párrafo de cierre (texto del blueprint 01)
- Créditos del equipo
- "ODS 13 — Acción por el Clima"
- "[Escape] Salir"

#### 9.5 `SignSystem` — letreros con texto real

Completar la implementación iniciada en Fase 4 con el texto real de cada módulo (ver blueprint 01):

Cada letrero genera su textura al inicio con el contenido específico:
- Título del módulo (grande)
- Dato impactante (tipografía destacada)
- Descripción (3 oraciones máximo)
- Fuente (institución + año)

La textura se genera **una sola vez en Init()** y se reutiliza.

#### 9.6 `src/audio/AudioEngine.h/.cpp`

```cpp
class AudioEngine {
public:
    bool init();                    // ma_engine_init
    void playAmbient(const char* path);    // arctic_wind_loop.ogg en loop
    void playActivate();                   // activate.wav 2D
    void play3D(const char* path, glm::vec3 pos);  // sonido posicional
    void updateListener(const glm::vec3& pos, const glm::vec3& front,
                       const glm::vec3& up);
    void stopAll();
    void shutdown();               // ma_engine_uninit

private:
    ma_engine engine;
    ma_sound  ambientSound;
    std::unordered_map<std::string, ma_sound> sounds3D;
};
```

**Inicialización**:
```cpp
ma_engine_init(NULL, &engine);
// Ambient loop
ma_sound_init_from_file(&engine, "assets/audio/arctic_wind_loop.ogg",
                         MA_SOUND_FLAG_STREAM, NULL, NULL, &ambientSound);
ma_sound_set_looping(&ambientSound, MA_TRUE);
ma_sound_set_volume(&ambientSound, 0.3f);  // 30% del máximo
ma_sound_start(&ambientSound);
```

**Audio 3D** (para los 3 módulos):
```cpp
ma_sound_set_position(&sound, x, y, z);
ma_sound_set_min_distance(&sound, 8.0f);   // volumen máximo dentro del trigger
ma_sound_set_max_distance(&sound, 20.0f);  // silencio total más allá
```

**Actualizar listener cada frame**:
```cpp
ma_engine_listener_set_position(&engine, 0, camera.x, camera.y, camera.z);
ma_engine_listener_set_direction(&engine, 0, front.x, front.y, front.z);
ma_engine_listener_set_world_up(&engine, 0, 0.0f, 1.0f, 0.0f);
```

### Verificación de la Fase 9

**Screenshots**:
1. **Pantalla de título**: foto ártica + overlay + texto "Nuestro Mundo"
2. **HUD [E] Explorar**: screenshot dentro del trigger de cualquier módulo
3. **Letrero M1 izq con texto real**: "El hielo que desaparece" + dato + descripción
4. **Pantalla de cierre**: texto de reflexión + créditos

**Checklist**:
- [ ] Pantalla de título se ve correctamente (foto, overlay, texto centrado)
- [ ] [E] Explorar aparece solo dentro del trigger y desaparece al presionar E
- [ ] Todos los letreros tienen el texto correcto del blueprint 01
- [ ] Texto de letreros es legible (no pixelado, contraste blanco sobre azul oscuro)
- [ ] Pantalla de cierre se activa con Escape desde M5 COMPLETADO
- [ ] Audio ambiental arranca automáticamente (escuchar, no hay screenshot)
- [ ] ImGui no produce errores de OpenGL (verificar con debug callback)
- [ ] `glEnable(GL_DEPTH_TEST)` restaurado después de cada render de ImGui

---

## Fase 10 — Integración Narrativa

**Objetivo**: Flujo completo de inicio a fin sin crashes. `AppState` correctamente transitado.

### Tareas

#### 10.1 Flujo completo de AppState

```
TITULO ──[Enter]──► JUGANDO ──[M5 completado + Escape]──► CIERRE ──[Escape]──► SALIR
                        │
                  [Escape en cualquier otro momento]
                        │
                       ▼
                 liberar cursor / cerrar app
```

#### 10.2 Verificar narrativa completa

Recorrido de prueba completo:
1. Pantalla de título → Enter
2. Fade-in al escenario (1 s)
3. Navegar a M1 izq → leer letrero → presionar E → ver animación iceberg
4. M2 izq → oso polar con idle → presionar E → disco de hielo se reduce
5. M3 izq → presionar E → agua sube cubriendo los edificios
6. Cruzar a corredor derecho
7. M1 der → presionar E → turbina acelera + partículas
8. M2 der → presionar E → auto enciende faros y se mueve
9. M3 der → presionar E → árbol crece + CO₂ capturado
10. Llegar a M5 → presionar E → globo gira + líneas aparecen secuencialmente
11. Con M5 en COMPLETADO → Escape → fade a negro → pantalla de cierre
12. Escape → la app se cierra

#### 10.3 Sincronización de audio con módulos

Conectar el AudioEngine con los módulos correspondientes:
- Al activar M1 izq (E): `audioEngine.play3D("assets/audio/ice_cracking.wav", modulePos)`
- Al activar M3 izq (E): `audioEngine.play3D("assets/audio/water_flowing.wav", modulePos)`
- Al activar M3 der (E): `audioEngine.play3D("assets/audio/nature_breeze.wav", modulePos)`
- Al salir del trigger: `audioEngine.stop3D(moduleIndex)`

### Verificación de la Fase 10

**Screenshot de pantalla de cierre**: captura del estado final con texto de reflexión y créditos.

**Recorrido completo sin crashes** — ejecutar 3 veces:
- [ ] La app no crashea en ningún punto del recorrido
- [ ] No hay frames congelados (lag mayor a 5 s)
- [ ] El fade-in desde la pantalla de título es suave
- [ ] El fade-out a la pantalla de cierre es suave
- [ ] Escape cierra la app correctamente desde la pantalla de cierre

---

## Fase 11 — Pulido y QA

**Objetivo**: Build estable, sin leaks, rendimiento ≥60 FPS, empaquetado con InstallForge.

### Tareas

#### 11.1 Verificar gestión de memoria

- `ResourceManager::clear()` llamado en el destructor
- `ma_engine_uninit(&engine)` en `AudioEngine::shutdown()`
- `ImGui_ImplOpenGL3_Shutdown()`, `ImGui_ImplGlfw_Shutdown()`, `ImGui::DestroyContext()`
- `glfwDestroyWindow()`, `glfwTerminate()`
- RenderDoc: capturar frame inicial y final para verificar sin leaks de recursos OpenGL

#### 11.2 Verificar rendimiento

Medir FPS con `1.0 / deltaTime` en ImGui debug overlay:
- En máquina de desarrollo: objetivo ≥60 FPS en toda la escena
- Con nieve (80-120 partículas) + todos los módulos cargados + skybox + audio: ≥45 FPS mínimo
- Si hay degradación severa: revisar llamadas a `glBufferData` vs `glBufferSubData` para VBOs dinámicos

#### 11.3 Build de Release

```bash
cmake -B build-release -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --config Release --parallel
```

Verificar que el Release no rompe ningún comportamiento vs Debug.

#### 11.4 Empaquetado con InstallForge

Contenido del paquete:
- `CGEIHC.exe` (Release)
- `assets/` (todos los subdirectorios: models, textures, audio, fonts, skybox)
- DLLs necesarias: `assimp-vc143-mt.dll` (o la versión generada por el build)
- `README.txt` con instrucciones de instalación y uso

Con miniaudio (header-only): **no hay DLL de audio que incluir**.

#### 11.5 Prueba de instalación limpia

Instalar en una máquina sin el entorno de desarrollo y ejecutar el recorrido completo.

### Verificación final — Criterio de entrega

- [ ] Build Release compila sin warnings críticos
- [ ] El ejecutable arranca desde el instalador sin DLLs adicionales
- [ ] Recorrido completo de inicio a fin sin crashes: 3 ejecuciones consecutivas
- [ ] Duración de ejecución estable: 15+ minutos sin crashes ni degradación visible
- [ ] FPS ≥45 en hardware objetivo
- [ ] Los 7 módulos tienen animación LERP funcional con tecla E y reset al salir
- [ ] Letreros muestran texto correcto en español con fuentes verificadas
- [ ] Blinn-Phong + Fresnel visibles y estables
- [ ] 3 categorías de animación presentes: básica (globo/turbina) + keyframe (LERP módulos) + procedural (olas, nieve, partículas)
- [ ] Skybox ártico y niebla activos
- [ ] Audio ambiental en loop + los 3 módulos con audio 3D posicional
- [ ] Pantalla de título y pantalla de cierre operativas
- [ ] Ejecutable empaquetable con InstallForge

---

## Gestión de Errores Diferidos

Cuando un problema no se resuelve en 3 iteraciones (código → screenshot → corrección), se registra como error diferido en `PENDIENTES.md`:

```markdown
## ⚠️ Error Diferido — [Nombre descriptivo]

- **Fase**: [0-11]
- **Fecha detectado**: YYYY-MM-DD
- **Descripción**: [qué falla exactamente]
- **Síntomas visuales**: [qué se ve en el screenshot, o qué error aparece en consola]
- **Intentos realizados**:
  1. [primer intento y resultado]
  2. [segundo intento y resultado]
  3. [tercer intento y resultado]
- **Impacto**: [qué funcionalidad bloquea o degrada]
- **Mitigación implementada**: [qué alternativa más simple se aplicó]
- **Estado**: ABIERTO / MITIGADO / RESUELTO
```

### Mitigaciones conocidas (pre-definidas)

| Riesgo | Mitigación |
|--------|-----------|
| Assimp GLTF animation (oso polar) | Bob en Y con función seno: `y += 0.1 * sin(time * 2.0f)` |
| stb_truetype texto borroso | Aumentar resolución del atlas a 1024×512 |
| Skybox HDR → cmgen no disponible | Shader procedural: gradiente azul ártico + banda verde aurora |
| FetchContent timeout | Subir binarios precompilados a `third_party/` del repositorio |
| ImGui corrupting OpenGL state | Llamar `glEnable(GL_DEPTH_TEST)` explícitamente al inicio de cada frame |

---

## Checklist de Prerequisitos antes de Shaders Avanzados (Fase 7)

Antes de comenzar la Fase 7, verificar que **todos** estos puntos están resueltos:

- [ ] GLFW crea contexto OpenGL 3.3 Core sin errores
- [ ] GLAD inicializa correctamente en Windows/VS2022
- [ ] `glEnable(GL_DEPTH_TEST)` activo y funcionando (sin z-fighting en la prueba del cubo)
- [ ] Cámara FPS genera View y Projection correctas (cubo de prueba sin distorsión)
- [ ] Uniform `viewPos` enviado correctamente al fragment shader
- [ ] Assimp exporta normales explícitas por vértice
- [ ] Shader loader: compilación, linking, y logs de error legibles
- [ ] Uniform system funcional: `setMat4`, `setVec3`, `setFloat`, `setInt`
- [ ] VAO/VBO/EBO reutilizables, sin memory leaks
- [ ] Texturas 2D cargadas con stb_image, UVs correctas
- [ ] Uniform `time` ligado a `glfwGetTime()` (necesario para olas del agua)
- [ ] `glGetError()` instrumentado en debug builds
- [ ] Skybox básico renderiza sin depth artifacts
- [ ] Dear ImGui coexiste con el pipeline sin romper depth state
- [ ] Frame pacing estable (≥60 FPS en hardware de desarrollo)

---

## Apéndice A — Recursos y Fuentes

### Assets a descargar (antes de Fase 4)

| Asset | Fuente | Notas |
|-------|--------|-------|
| Iceberg GLTF | sketchfab.com (CC0) | Formato .glb preferido |
| Oso polar GLTF animado | sketchfab.com (CC0) | Verificar que tiene clip "idle" |
| Edificios low-poly (3-5) | sketchfab.com o polyhaven.com | Formato GLTF |
| Turbina eólica GLTF | sketchfab.com (CC0) | Aspas en mesh separado si posible |
| Auto eléctrico low-poly | sketchfab.com (CC0) | Con faros como mesh separado |
| Árbol/pino GLTF | sketchfab.com (CC0) | |
| Globo terráqueo GLTF | sketchfab.com (CC0) | Con textura de mapa |
| Foca anillada GLTF | sketchfab.com (CC0) | Estática |
| Zorro ártico GLTF | sketchfab.com (CC0) | Estático |
| Textura hielo/nieve | ambientcg.com (CC0) | Albedo + normal map tileable |
| Textura agua | ambientcg.com (CC0) | Normal map para agua |
| Skybox HDR ártico | polyhaven.com (CC0) | Con aurora boreal |
| arctic_wind_loop.ogg | freesound.org (CC0) | Loop sin silencio al inicio/fin |
| activate.wav | freesound.org (CC0) | < 0.5 s, click/tono suave |
| ice_cracking.wav | freesound.org (CC0) | Crujido de hielo |
| water_flowing.wav | freesound.org (CC0) | Agua fluyendo |
| nature_breeze.wav | freesound.org (CC0) | Brisa natural |
| Roboto-Regular.ttf | fonts.google.com | Apache 2.0, libre distribución |
| Foto ártica PNG | unsplash.com (CC0) | 1280×720 mínimo, paisaje polar |

### Comandos de build rápidos

```bash
# Build debug (desarrollo)
cmake --build build --config Debug --parallel

# Build y captura automática de screenshot
cmake --build build --config Debug --parallel && ./build/Debug/CGEIHC.exe
# (la app captura active/screenshot.png al frame 120 y se cierra)

# Build release (entrega)
cmake --build build-release --config Release --parallel
```
