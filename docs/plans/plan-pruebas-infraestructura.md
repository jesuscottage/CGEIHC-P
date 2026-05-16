# Plan de Pruebas de Infraestructura — CGEIHC-P

> Fecha: 2026-05-16
> Propósito: Validar que todas las herramientas del pipeline autónomo funcionan correctamente ANTES de ejecutar el plan maestro de desarrollo.
> Si algún test falla, se documenta la mitigación y se ajusta el plan maestro.

---

## Por qué este plan existe

El plan maestro depende de un pipeline de herramientas que Claude usa de forma autónoma:
build → ejecutar → screenshot → leer visualmente → iterar. Si cualquier eslabón falla
(CMake no está en PATH, glReadPixels produce imagen negra, Claude no puede leer PNGs,
`ACTIVE_DIR` genera rutas incorrectas en Windows), el desarrollo autónomo se rompe silenciosamente.

Este plan prueba cada eslabón de forma aislada, con criterios de aceptación binarios (PASA/FALLA),
**antes** de escribir una sola línea del proyecto real.

---

## Riesgos conocidos a validar

| Riesgo | Por qué importa |
|--------|----------------|
| Path `Jesús` con caracter no-ASCII (é) | CMake, MSVC y git pueden fallar con paths no-ASCII en Windows |
| `ACTIVE_DIR` en Windows | Las barras invertidas y espacios en rutas absolutas pueden romper el `#define` de CMake |
| `glReadPixels` produce negro | Si el contexto GL no está disponible en background, los screenshots serán negros |
| Claude no puede leer PNGs significativamente | Si el contenido 3D no es interpretable visualmente, el ciclo de verificación falla |
| CMake no está en PATH del shell | Bash en VSCode puede no encontrar `cmake` si no está en PATH del sistema |
| FetchContent bloqueado o lento | Sin internet o con proxy corporativo, las dependencias no descargan |
| `WebSearch`/`WebFetch` no disponibles | Bloquea la Fase 0B de investigación de proyectos referencia |

---

## Estructura de archivos de prueba

Los tests usan `app/` directamente (misma estructura que Fase 0 del plan maestro).
El código de prueba en `app/src/main_test.cpp` se reemplaza al iniciar Fase 1.

```
CGEIHC-P/
├── app/
│   ├── CMakeLists.txt       ← creado en T-02, evoluciona con cada test
│   └── src/
│       ├── main_test.cpp    ← archivo de prueba (reemplazado en Fase 1)
│       └── vendor/          ← GLAD + stb headers (creados en T-04/T-09)
├── active/                  ← screenshots de prueba (gitignored)
└── build/                   ← build de prueba (gitignored)
```

---

## Tests

### T-01 — Entorno del Sistema

**Objetivo**: Verificar que las herramientas de build están disponibles en el PATH del shell que usa Claude.

**Comandos**:
```bash
cmake --version
cl.exe 2>&1 | head -1  # MSVC — puede fallar si VS2022 no está en PATH
where cmake
where git
git --version
```

**Criterio de aceptación**:
- [ ] `cmake --version` muestra 3.20 o superior
- [ ] `git --version` responde correctamente
- [ ] El PATH del shell es el mismo que el PATH del sistema (CMake encontrable)

**Si falla**:
- CMake no en PATH → ajustar el comando a la ruta absoluta de cmake.exe en VS2022 (típicamente `C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe`) y documentar en el plan maestro
- cl.exe no en PATH → usar `cmake --build` con `--config Debug` (no requiere cl.exe en PATH directo)

---

### T-02 — Build CMake Mínimo (sin dependencias)

**Objetivo**: Verificar que CMake puede configurar y compilar un proyecto C++17 en `app/` con build en la raíz, y que el path `Jesús` (caracter é) no causa problemas.

**Archivos a crear**:

`app/CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.20)
project(CGEIHC_TEST VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CGEIHC_TEST src/main_test.cpp)
```

`app/src/main_test.cpp`:
```cpp
#include <iostream>
int main() {
    std::cout << "[T-02] Build minimo OK\n";
    return 0;
}
```

**Comandos**:
```bash
cmake -B build -S app -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel
./build/Debug/CGEIHC_TEST.exe
```

**Criterio de aceptación**:
- [ ] `cmake -B build -S app` completa sin errores
- [ ] `cmake --build build` produce `build/Debug/CGEIHC_TEST.exe`
- [ ] El exe imprime `[T-02] Build minimo OK`
- [ ] El path con `é` (Jesús) no causa errores de encoding en CMake ni MSVC

**Si falla**:
- Path no-ASCII → mover el proyecto a `C:\CGEIHC-P\` (sin caracteres especiales) y actualizar todas las rutas del plan maestro y del repositorio git
- CMake no encuentra el compilador → instalar "C++ Desktop Development" en VS2022 installer

---

### T-03 — ACTIVE_DIR: Rutas Absolutas en Windows

**Objetivo**: Verificar que la compile definition `ACTIVE_DIR` genera una ruta correcta en Windows (barras, espacios, non-ASCII) y que la app puede escribir un archivo en `CGEIHC-P/active/`.

**Modificar `app/CMakeLists.txt`**:
```cmake
cmake_minimum_required(VERSION 3.20)
project(CGEIHC_TEST VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Test de ACTIVE_DIR: ruta absoluta a active/ en la raíz del repo
get_filename_component(REPO_ROOT "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
message(STATUS "REPO_ROOT = ${REPO_ROOT}")
message(STATUS "ACTIVE_DIR = ${REPO_ROOT}/active/")

target_compile_definitions(CGEIHC_TEST PRIVATE
    ACTIVE_DIR="${REPO_ROOT}/active/"
)

add_custom_command(TARGET CGEIHC_TEST POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${REPO_ROOT}/active"
)

add_executable(CGEIHC_TEST src/main_test.cpp)
```

**Modificar `app/src/main_test.cpp`**:
```cpp
#include <iostream>
#include <fstream>
#include <string>

#ifndef ACTIVE_DIR
#define ACTIVE_DIR "active/"
#endif

int main() {
    std::string path = std::string(ACTIVE_DIR) + "test_t03.txt";
    std::cout << "[T-03] Escribiendo en: " << path << "\n";

    std::ofstream f(path);
    if (!f.is_open()) {
        std::cerr << "[T-03] FALLO: No se pudo abrir " << path << "\n";
        return 1;
    }
    f << "T-03 OK - ACTIVE_DIR funciona correctamente\n";
    f.close();
    std::cout << "[T-03] Archivo escrito correctamente\n";
    return 0;
}
```

**Comandos**:
```bash
cmake -B build -S app -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel
./build/Debug/CGEIHC_TEST.exe
```

**Criterio de aceptación**:
- [ ] El `message(STATUS)` de CMake muestra la ruta correcta (con barras `/`, no `\`)
- [ ] `active/test_t03.txt` existe en la raíz de `CGEIHC-P/`
- [ ] El contenido de `test_t03.txt` es `T-03 OK - ACTIVE_DIR funciona correctamente`
- [ ] Claude puede leer `active/test_t03.txt` con la herramienta Read

**Si falla**:
- Ruta con backslashes o espacios → usar `file(TO_CMAKE_PATH ...)` en CMake para normalizar
- Caracteres non-ASCII en el path → confirmar si es el `é` de Jesús; si es así, ejecutar T-02 mitigation primero
- No puede escribir el archivo → problema de permisos; verificar que `active/` fue creado por el POST_BUILD

---

### T-04 — GLFW + GLAD: Contexto OpenGL 3.3 Core

**Objetivo**: Verificar que GLFW descarga correctamente con FetchContent, que GLAD (header-only) compila, y que se puede crear un contexto OpenGL 3.3 Core Profile en esta máquina.

**Prerequisito**: GLAD headers deben existir en `app/src/vendor/`. Descargar desde glad.dav1d.de con:
- Language: C/C++
- Specification: OpenGL
- Profile: Core
- Version: 3.3
- Extensiones: ninguna adicional
- Generar: Click "GENERATE" → descargar zip → extraer `glad.h`, `glad.c`, `KHR/khrplatform.h` a `app/src/vendor/`

**Modificar `app/CMakeLists.txt`**: añadir GLFW via FetchContent:
```cmake
include(FetchContent)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ... (mantener todo lo anterior) ...
target_include_directories(CGEIHC_TEST PRIVATE src/ src/vendor/)
target_link_libraries(CGEIHC_TEST PRIVATE glfw)
```

**Modificar `app/src/main_test.cpp`**:
```cpp
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    if (!glfwInit()) {
        std::cerr << "[T-04] FALLO: glfwInit()\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "T-04 Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "[T-04] FALLO: No se pudo crear ventana OpenGL 3.3 Core\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[T-04] FALLO: gladLoadGLLoader()\n";
        return 1;
    }

    std::cout << "[T-04] OpenGL: " << glGetString(GL_VERSION) << "\n";
    std::cout << "[T-04] Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "[T-04] PASA: Contexto OpenGL 3.3 Core creado\n";

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

**Criterio de aceptación**:
- [ ] FetchContent descarga GLFW sin errores (requiere internet)
- [ ] El exe compila sin errores
- [ ] La salida muestra la versión de OpenGL (≥ 3.3) y el nombre del renderer
- [ ] La ventana abre y el proceso termina limpiamente

**Si falla**:
- GPU no soporta OpenGL 3.3 → verificar con `glGetString(GL_VERSION)`; si es < 3.3, el proyecto no es viable en esta máquina
- FetchContent falla por internet → descargar GLFW manualmente y usar `add_subdirectory`
- GLAD headers no encontrados → verificar rutas en `src/vendor/`

---

### T-05 — Screenshot Pipeline: glReadPixels → PNG → Read (CRÍTICO)

**Objetivo**: Este es el test más importante. Verificar que:
1. `glReadPixels` captura el framebuffer correctamente (no produce imagen negra)
2. `stb_image_write` escribe un PNG válido en `active/`
3. La herramienta **Read** de Claude puede leer el PNG y ver su contenido visual significativamente
4. Claude puede distinguir entre diferentes estados visuales de la escena

**Prerequisito**: `stb_image_write.h` en `app/src/vendor/`.

**Modificar `app/src/main_test.cpp`**:
```cpp
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

#ifndef ACTIVE_DIR
#define ACTIVE_DIR "active/"
#endif

void saveScreenshot(const char* filename, int w, int h) {
    std::vector<unsigned char> pixels(w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    stbi_flip_vertically_on_write(true);
    std::string path = std::string(ACTIVE_DIR) + filename;
    if (stbi_write_png(path.c_str(), w, h, 3, pixels.data(), w * 3))
        std::cout << "[T-05] Screenshot guardado: " << path << "\n";
    else
        std::cerr << "[T-05] FALLO: No se pudo escribir " << path << "\n";
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int W = 1280, H = 720;
    GLFWwindow* window = glfwCreateWindow(W, H, "T-05 Screenshot Test", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int frame = 0;
    while (!glfwWindowShouldClose(window)) {
        // Alternar colores cada 60 frames para que Claude vea diferencias
        if (frame < 60)
            glClearColor(0.8f, 0.1f, 0.1f, 1.0f);  // Rojo
        else if (frame < 120)
            glClearColor(0.1f, 0.7f, 0.1f, 1.0f);  // Verde
        else
            glClearColor(0.1f, 0.1f, 0.8f, 1.0f);  // Azul

        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (frame == 30)  saveScreenshot("test_t05_rojo.png",  W, H);
        if (frame == 90)  saveScreenshot("test_t05_verde.png", W, H);
        if (frame == 150) saveScreenshot("test_t05_azul.png",  W, H);
        if (frame >= 151) glfwSetWindowShouldClose(window, GLFW_TRUE);

        frame++;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

**Comandos**:
```bash
cmake -B build -S app -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel
./build/Debug/CGEIHC_TEST.exe
```
Luego Claude lee los tres PNGs con la herramienta Read y verifica:

**Criterio de aceptación**:
- [ ] Los tres archivos `active/test_t05_*.png` existen y tienen tamaño > 0
- [ ] Claude puede leer `test_t05_rojo.png` con Read y describir: fondo rojo
- [ ] Claude puede leer `test_t05_verde.png` con Read y describir: fondo verde
- [ ] Claude puede leer `test_t05_azul.png` con Read y describir: fondo azul
- [ ] Los tres screenshots son visualmente distintos (prueba que el frame counter funciona)

**Si falla**:
- PNGs negros → `glReadPixels` falló antes del `SwapBuffers`; mover la captura a después del swap o usar `glFinish()` antes de leer
- PNGs invertidos verticalmente → `stbi_flip_vertically_on_write(true)` ya lo compensa; verificar
- Claude no puede leer el PNG → el pipeline visual está roto; considerar alternativa con `glGetTexImage` o captura diferente
- Claude puede leer pero ve negro → la ventana no está dibujando (problema de contexto compartido con background process); investigar si la ventana necesita estar en primer plano

**Impacto en el plan maestro si falla**:
Si Claude no puede distinguir visualmente el contenido de los PNGs, el ciclo de verificación autónoma no es viable. Opciones de mitigación:
- A) Confiar en el JSON de estado y logs de consola únicamente, sin verificación visual
- B) Agregar ImGui debug overlay desde el inicio con texto explícito que Claude pueda leer en el PNG
- C) Reducir tamaño de ventana a 320×240 para que el contenido sea más compacto y legible

---

### T-06 — Multi-Screenshot y JSON de Estado

**Objetivo**: Verificar el pipeline completo de captura automática: 3 frames + JSON de estado + cierre automático.

**Modificar `app/src/main_test.cpp`** (reemplazar con versión multi-screenshot + JSON):
```cpp
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <chrono>

#ifndef ACTIVE_DIR
#define ACTIVE_DIR "active/"
#endif

const int CAPTURE_FRAMES[] = {60, 300, 600};
const char* CAPTURE_NAMES[] = {
    "screenshot_f060.png", "screenshot_f300.png", "screenshot_f600.png"
};
constexpr int LAST_FRAME = 600;

void saveScreenshot(const char* name, int w, int h) {
    std::vector<unsigned char> pixels(w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    stbi_flip_vertically_on_write(true);
    std::string path = std::string(ACTIVE_DIR) + name;
    stbi_write_png(path.c_str(), w, h, 3, pixels.data(), w * 3);
    std::cout << "[T-06] Screenshot: " << path << "\n";
}

void saveStateJSON(int frame, float fps) {
    std::string path = std::string(ACTIVE_DIR) + "state.json";
    std::ofstream f(path);
    f << "{\n"
      << "  \"frame\": " << frame << ",\n"
      << "  \"fps\": " << fps << ",\n"
      << "  \"test\": \"T-06 OK\",\n"
      << "  \"activeModule\": \"ninguno\",\n"
      << "  \"moduleT\": 0.0\n"
      << "}\n";
    std::cout << "[T-06] JSON: " << path << "\n";
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int W = 1280, H = 720;
    GLFWwindow* window = glfwCreateWindow(W, H, "T-06", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int frame = 0;
    auto t0 = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration<float>(now - t0).count();
        float fps = (frame > 0) ? frame / elapsed : 60.0f;

        // Color que cambia con el tiempo — visible en screenshots
        float t = (float)frame / LAST_FRAME;
        glClearColor(t, 0.3f, 1.0f - t, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();

        for (int i = 0; i < 3; i++)
            if (frame == CAPTURE_FRAMES[i])
                saveScreenshot(CAPTURE_NAMES[i], W, H);

        if (frame >= LAST_FRAME) {
            saveStateJSON(frame, fps);
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        frame++;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "[T-06] PASA: " << frame << " frames, 3 screenshots + JSON\n";
    return 0;
}
```

**Criterio de aceptación**:
- [ ] `active/screenshot_f060.png` existe — color azulado (t=0.1)
- [ ] `active/screenshot_f300.png` existe — color intermedio
- [ ] `active/screenshot_f600.png` existe — color rojizo (t=1.0)
- [ ] `active/state.json` existe y Claude puede leerlo con Read
- [ ] El JSON contiene `"test": "T-06 OK"`
- [ ] Claude puede distinguir los tres screenshots como visualmente diferentes entre sí
- [ ] La app cierra automáticamente sin intervención

**Si falla**:
- Frame 600 nunca llega → agregar watchdog: `if (frame > 700) exit(0)`
- Screenshots idénticos → verificar que el `glClearColor` con `t` variable está produciendo diferencias

---

### T-07 — Test Mode CLI

**Objetivo**: Verificar que el argumento `--test-module M1_IZQ` es parseado correctamente y produce la posición de cámara correspondiente.

**Modificar `app/src/main_test.cpp`** (añadir parsing de args antes del loop):
```cpp
// En main(int argc, char** argv):
struct TestConfig {
    bool enabled = false;
    std::string testModule = "";
};

TestConfig cfg;
for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "--test-module" && i + 1 < argc) {
        cfg.enabled = true;
        cfg.testModule = argv[++i];
    }
}
if (cfg.enabled)
    std::cout << "[T-07] Test mode: " << cfg.testModule << "\n";
```

**Comandos**:
```bash
./build/Debug/CGEIHC_TEST.exe --test-module M1_IZQ
./build/Debug/CGEIHC_TEST.exe --test-module M5
./build/Debug/CGEIHC_TEST.exe  # sin argumentos
```

**Criterio de aceptación**:
- [ ] Con `--test-module M1_IZQ` imprime `[T-07] Test mode: M1_IZQ`
- [ ] Con `--test-module M5` imprime `[T-07] Test mode: M5`
- [ ] Sin argumentos no imprime la línea de test mode (no rompe el comportamiento normal)

---

### T-08 — FetchContent: GLM + Assimp + ImGui

**Objetivo**: Verificar que las tres dependencias principales descargan correctamente con FetchContent y que el binario las enlaza sin errores.

**Tiempo estimado**: 5-15 minutos (descarga de Assimp es la más pesada).

**Modificar `app/CMakeLists.txt`** (añadir las dependencias):
```cmake
# GLM
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
# Assimp (solo GLTF y OBJ)
FetchContent_Declare(assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.4.3
)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
# Dear ImGui
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.6
)
FetchContent_MakeAvailable(glfw glm assimp imgui)

# Fuentes ImGui (solo las necesarias)
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
```

**Modificar `app/src/main_test.cpp`** (añadir includes de verificación):
```cpp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// (No incluir Assimp aquí todavía — solo verificar que linka)

// En main():
glm::vec3 testVec(1.0f, 2.0f, 3.0f);
glm::mat4 testMat = glm::translate(glm::mat4(1.0f), testVec);
std::cout << "[T-08] GLM OK: vec3 creado\n";

// ImGui init
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGui_ImplGlfw_InitForOpenGL(window, true);
ImGui_ImplOpenGL3_Init("#version 330");
std::cout << "[T-08] ImGui OK: contexto creado\n";

ImGui_ImplOpenGL3_Shutdown();
ImGui_ImplGlfw_Shutdown();
ImGui::DestroyContext();
```

**Criterio de aceptación**:
- [ ] `cmake -B build -S app` descarga las 3 dependencias sin errores de red
- [ ] El build compila sin errores de linking
- [ ] La app imprime `[T-08] GLM OK` y `[T-08] ImGui OK`
- [ ] DLL de Assimp copiada junto al exe (verificar que `assimp-vc*.dll` existe en `build/Debug/`)

**Si falla**:
- FetchContent timeout → usar flag `--retry 3` o descargar manualmente y poner en `third_party/`
- Assimp error de linking → verificar que `target_link_libraries` incluye `assimp`

---

### T-09 — stb Headers: Compile y Funcionalidad

**Objetivo**: Verificar que `stb_image.h`, `stb_image_write.h` y `stb_truetype.h` compilan correctamente con una sola Translation Unit de implementación.

**Archivo a crear**: `app/src/stb_impl.cpp` (este archivo sobrevive al proyecto real):
```cpp
// Una sola TU para todas las implementaciones stb
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
```

**Modificar `app/CMakeLists.txt`**: añadir `src/stb_impl.cpp` a las fuentes del ejecutable.

**Verificar en `app/src/main_test.cpp`**:
```cpp
// stb_truetype: crear atlas con rangos Latin-1 (prueba de glifos español)
unsigned char atlasPixels[512 * 256];
stbtt_pack_context packCtx;
stbtt_PackBegin(&packCtx, atlasPixels, 512, 256, 0, 1, nullptr);

// Cargar fuente temporal (usar cualquier .ttf disponible en el sistema)
// Si no hay fuente disponible, solo verificar que la API existe
std::cout << "[T-09] stb headers: OK (API disponible)\n";
```

**Criterio de aceptación**:
- [ ] `stb_impl.cpp` compila sin errores ni warnings críticos
- [ ] No hay errores de "multiple definition" de las funciones stb
- [ ] La app imprime `[T-09] stb headers: OK`

---

### T-10 — miniaudio: Compile e Inicialización

**Objetivo**: Verificar que `miniaudio.h` compila en Windows y que `ma_engine_init` no falla.

**Archivo a crear**: `app/src/miniaudio_impl.cpp`:
```cpp
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
```

**Verificar en `app/src/main_test.cpp`**:
```cpp
#include "miniaudio.h"

ma_engine engine;
ma_result result = ma_engine_init(NULL, &engine);
if (result != MA_SUCCESS) {
    std::cerr << "[T-10] FALLO: ma_engine_init = " << result << "\n";
} else {
    std::cout << "[T-10] miniaudio OK: engine inicializado\n";
    ma_engine_uninit(&engine);
}
```

**Criterio de aceptación**:
- [ ] `miniaudio_impl.cpp` compila sin errores
- [ ] `ma_engine_init` retorna `MA_SUCCESS`
- [ ] `ma_engine_uninit` no produce crash

**Si falla**:
- `MA_NO_BACKEND` en Windows → raro; verificar que Windows Audio Service está activo
- Errores de compilación → verificar versión de miniaudio (0.11+) y que `MINIAUDIO_IMPLEMENTATION` está en exactamente una TU

---

### T-11 — WebSearch y WebFetch

**Objetivo**: Verificar que Claude puede buscar en internet y acceder a URLs externas (necesario para la Fase 0B de investigación de proyectos de referencia).

**Test WebSearch**:
```
WebSearch: "learnopengl.com LearnOpenGL source code github Joey de Vries"
```
Criterio: devuelve resultados relevantes, incluye github.com/JoeyDeVries

**Test WebFetch**:
```
WebFetch: URL del README de github.com/JoeyDeVries/LearnOpenGL
```
Criterio: Claude puede leer el contenido del README

**Criterio de aceptación**:
- [ ] WebSearch devuelve resultados para términos técnicos de OpenGL C++
- [ ] WebFetch puede leer el contenido de repositorios de GitHub
- [ ] Las herramientas están disponibles (no bloqueadas por permisos)

**Si falla**:
- Sin WebSearch → Fase 0B debe ejecutarse manualmente por el usuario, que provee las URLs a Claude
- Sin WebFetch → Claude puede recibir fragmentos de código pegados directamente en el chat
- Ajuste al plan: documentar que la Fase 0B requiere intervención humana para proveer URLs

---

### T-12 — Git Workflow: Commit + Push

**Objetivo**: Confirmar que el flujo de commit y push funciona correctamente desde el shell de Claude (ya funciona desde sesiones previas, pero se verifica formalmente).

**Comando**:
```bash
git status
git log --oneline -3
```

**Criterio de aceptación**:
- [ ] `git status` muestra el estado limpio o los cambios esperados
- [ ] `git log` muestra los commits recientes con mensajes correctos
- [ ] El flujo de `/commit` funciona (verificado en sesiones previas)

---

## Resultados de los Tests

> Ejecutados: 2026-05-16

| Test | Estado | Notas |
|------|--------|-------|
| T-01 Entorno | ✅ PASA | cmake 3.29, git 2.45 en PATH. MSVC a través de `cmake --build` (no requiere cl.exe directo). |
| T-02 CMake mínimo | ✅ PASA | C++17 compila sin problemas. Path con `é` (Jesús) funciona correctamente en CMake y MSVC. |
| T-03 ACTIVE_DIR | ✅ PASA | `get_filename_component(REPO_ROOT ...)` genera rutas con `/` en Windows. `active/test_t03.txt` creado correctamente. |
| T-04 GLFW + GLAD + OpenGL | ✅ PASA | FetchContent descarga GLFW 3.4. GLAD generado con `python -m glad --api gl=3.3 --profile core --generator c`. OpenGL 3.3 Core context creado. **NOTA**: estructura de GLAD es `vendor/include/glad/glad.h` (no flat). |
| T-05 Screenshot + Read (CRÍTICO) | ✅ PASA | `glReadPixels` captura correctamente. Los tres PNGs (rojo/verde/azul) son visualmente distintos y Claude los interpreta correctamente con la herramienta Read. Pipeline autónomo validado. |
| T-06 Multi-screenshot + JSON | ✅ PASA | 3 screenshots (f060, f300, f600) con colores progresivos (azul→púrpura→rojo). `state.json` con `"test": "T-06 OK"`, fps ~817 (sin vsync). App cierra automáticamente en frame 600. |
| T-07 Test Mode CLI | ✅ PASA | `--test-module M1_IZQ` parseado correctamente. Sin argumentos no rompe el comportamiento. |
| T-08 FetchContent deps | ✅ PASA | GLM 1.0.1, Assimp v5.4.3, ImGui v1.91.6 descargan y compilan. **NOTA IMPORTANTE**: Assimp compila como librería *estática* (`assimp-vc143-mtd.lib`) en Debug — no genera DLL. El `copy_if_different` del POST_BUILD es un no-op en Debug. |
| T-09 stb headers | ✅ PASA | `stb_impl.cpp` (con stb_image + stb_image_write + stb_truetype) compila sin errores ni definiciones múltiples. Archivo permanente del proyecto. |
| T-10 miniaudio | ✅ PASA | `ma_engine_init` retorna `MA_SUCCESS`. Sample rate: 48000 Hz. `ma_engine_uninit` sin crash. Archivo `miniaudio_impl.cpp` permanente. |
| T-11 WebSearch + WebFetch | ✅ PASA | WebSearch: encontró github.com/JoeyDeVries/LearnOpenGL. WebFetch: extrajo shader GLSL de learnopengl.com/Getting-started/Hello-Triangle. Ambas herramientas disponibles. |
| T-12 Git workflow | ✅ PASA | Commits y push verificados en sesiones previas. `git log` muestra historial limpio. |

### Hallazgos clave para el plan maestro

1. **GLAD path**: usar `#include "glad/glad.h"` (no `#include "glad.h"`). Rutas CMake: `src/vendor/include` + `src/vendor`.
2. **Assimp estático en Debug**: no copiar DLL en Debug. El POST_BUILD `copy_if_different` puede mantenerse por compatibilidad con Release.
3. **Path é (Jesús)**: no requiere mitigación. Funciona en CMake, MSVC y bash.
4. **Vsync en producción**: sin `glfwSwapInterval(1)`, la app corre a ~817 FPS. Agregar vsync en la Fase 1 real.
5. **Pipeline visual PASA**: el ciclo `código → build → exe → screenshot → Read visual` funciona de forma autónoma. Se puede ejecutar el plan maestro sin supervisión.

---

## Matriz de Decisión Post-Tests

Después de ejecutar todos los tests, evaluar:

| Resultado | Acción |
|-----------|--------|
| T-01 FALLA (CMake no en PATH) | Hardcodear ruta absoluta de cmake.exe en todos los comandos del plan maestro |
| T-02 FALLA (path é rompe CMake) | Mover repo a `C:\CGEIHC-P\` y actualizar remote git |
| T-03 FALLA (ACTIVE_DIR incorrecto) | Cambiar a rutas relativas desde el exe o usar argv[0] para calcular el path |
| T-05 FALLA (Claude no ve PNG) | Depender únicamente del JSON de estado + logs; agregar ImGui con texto grande |
| T-05 FALLA (PNGs negros) | Usar `glFinish()` antes de `glReadPixels`, mover la captura post-swap |
| T-08 FALLA (FetchContent) | Descargar dependencias manualmente y hacer `add_subdirectory` con `third_party/` |
| T-11 FALLA (sin web) | Fase 0B requiere intervención manual: usuario provee URLs de proyectos referencia |
| Todos PASAN | Ejecutar plan maestro sin modificaciones |
