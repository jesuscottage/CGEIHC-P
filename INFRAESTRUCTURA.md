# Infraestructura y Stack Técnico — CGEIHC-P

> Proyecto: Calentamiento global en el polo norte
> Última actualización: 2026-05-15
> Ver también: `docs/knowledge/technology/especificaciones-proyecto.md`

---

## Resumen del Stack

| Capa | Herramienta | Versión | Propósito |
|------|------------|---------|-----------|
| Lenguaje | C++ | C++17 | Lenguaje principal del motor gráfico |
| API Gráfica | OpenGL | 3.3+ Core Profile | Renderizado 3D en tiempo real (requerido por la materia) |
| Cargador de Extensiones | GLAD | 2.x | Cargador moderno de funciones OpenGL (reemplaza GLEW) |
| Gestión de Ventana | GLFW | 3.3+ | Ventanas, contexto OpenGL, manejo de eventos |
| Matemáticas | GLM | 0.9.9+ | Vectores, matrices, transformaciones MVP |
| Carga de Modelos | Assimp | 5.x | Importar FBX/OBJ/GLTF exportados desde Blender |
| Carga de Texturas | stb_image | 2.x (header-only) | Carga PNG/JPG/HDR para texturas |
| UI/HUD | Dear ImGui | 1.9x | Interfaz minimalista (HUD, menús, paneles info) |
| Audio | OpenAL Soft | 1.23+ | Audio 3D posicional (libre y multiplataforma) |
| Audio (archivos) | libsndfile | 1.2+ | Carga de archivos WAV/OGG para OpenAL |
| Sistema de Build | CMake | 3.20+ | Gestión de dependencias con FetchContent |
| IDE | Visual Studio 2022 Community | Free | Compilación y debugging en Windows |
| Modelado 3D | Blender | 4.x | Creación de modelos, rigging y animaciones |
| Generación IA | Meshy.ai | Web | Generación de modelos 3D desde texto (free tier) |
| Debugging GPU | RenderDoc | 1.x | Captura y análisis de frames OpenGL |
| Control de Versiones | Git + GitHub | — | Repositorio colaborativo |

---

## Stack de Desarrollo Autónomo (Claude Code)

Esta sección documenta las herramientas que permiten a Claude Code trabajar de forma autónoma en el proyecto, con retroalimentación visual del resultado 3D.

### El Problema: Claude No Puede "Ver" OpenGL Directamente

A diferencia de navegadores web (donde existe Playwright), no hay una herramienta estándar para que un agente IA controle y observe una ventana OpenGL. La solución es un **bucle de retroalimentación visual manual-asistido**:

```
Código C++/GLSL  →  CMake Build  →  Ejecutar .exe  →  Screenshot  →  Claude lee imagen  →  Itera
```

### Herramientas del Bucle Autónomo

#### 1. Build Automatizado — CMake + PowerShell

```powershell
# Script de build y ejecución (scripts/build-and-run.ps1)
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
.\build\Debug\CGEIHC.exe
```

Claude ejecuta este script via Bash tool después de cada modificación de código.

#### 2. Captura de Pantalla — PowerShell

```powershell
# Tomar screenshot después de N segundos de ejecución
# scripts/screenshot.ps1
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing
Start-Sleep -Seconds 3  # Esperar que la ventana OpenGL abra
$screen = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds
$bmp = New-Object System.Drawing.Bitmap($screen.Width, $screen.Height)
$graphics = [System.Drawing.Graphics]::FromImage($bmp)
$graphics.CopyFromScreen($screen.Location, [System.Drawing.Point]::Empty, $screen.Size)
$bmp.Save("$PSScriptRoot\..\active\screenshot.png")
```

Claude luego lee `active/screenshot.png` con el Read tool (soporte de imágenes) para evaluar el resultado visual.

#### 3. RenderDoc — Debugging de GPU

- **URL**: [renderdoc.org](https://renderdoc.org/)
- Permite capturar frames individuales con análisis detallado del pipeline
- In-App API: `RENDERDOC_API_1_1_2` para captura programática
- Muestra: buffers, texturas, shaders, draw calls, estado del pipeline

Uso típico: cuando hay artefactos visuales no explicados por el código.

#### 4. OpenGL Debug Callback (en código)

```cpp
// Activar en modo Debug — detecta errores de shader y estado
glEnable(GL_DEBUG_OUTPUT);
glDebugMessageCallback(MessageCallback, 0);
```

Salida directa a consola → Claude lee la salida del proceso via Bash.

#### 5. Blender Headless (Automatización de Assets)

```bash
# Exportar modelo desde Blender sin UI
blender --background modelo.blend --python export_script.py
```

Claude puede generar scripts de exportación para automatizar el pipeline de assets.

---

## Pipeline de Assets (Blender → OpenGL)

```
Blender (.blend)
    ↓  [Modelado + Rigging + Animación]
    ↓  [Export: File > Export > FBX/GLTF]
Asset exportado (.fbx / .gltf / .obj)
    ↓  [Assimp carga en tiempo de ejecución]
Motor OpenGL (C++)
    ↓  [VAO/VBO → Shaders GLSL → Renderizado]
Pantalla
```

### Formatos de Export Recomendados

| Formato | Ventajas | Uso |
|---------|----------|-----|
| **GLTF/GLB** | Estándar moderno, incluye animaciones, texturas embebidas | Modelos con animación |
| **FBX** | Amplio soporte en Assimp, incluye rigging | Personajes con esqueleto |
| **OBJ + MTL** | Simple, universal, fácil de debuggear | Objetos estáticos del entorno |

### Repositorios de Modelos Gratuitos

- [Sketchfab](https://sketchfab.com/) — Modelos 3D CC0/free, exportar en OBJ/FBX
- [Meshy.ai](https://www.meshy.ai/) — Generación IA de modelos 3D desde texto
- [Poly Haven](https://polyhaven.com/) — Modelos y texturas CC0

---

## Fuentes de Texturas (CC0 / Gratuitas)

| Fuente | Tipo de texturas |
|--------|-----------------|
| [ambientcg.com](https://ambientcg.com/) | PBR completas (albedo, normal, roughness, AO) |
| [polyhaven.com](https://polyhaven.com/) | PBR completas + HDR para skybox |
| [3dtextures.me](https://3dtextures.me/) | PBR gratuitas de alta calidad |
| [texturelabs.org](https://texturelabs.org/) | Variedad de materiales |
| [architextures.org](https://architextures.org/) | Arquitectura y superficies |
| [humus.name](https://www.humus.name/index.php?page=Textures) | Cubemaps / skyboxes |

**Texturas clave para este proyecto**:
- Hielo (con normal map para grietas, transparencia con alpha)
- Nieve (diffuse + normal)
- Agua ártica (animated via shader, normal map para ondas)
- Roca congelada
- Metal oxidado (ruta problemática)
- Panel solar, turbina eólica (ruta sostenible)

---

## Arquitectura del Motor (Módulos)

Según los blueprints del proyecto:

```
CGEIHC/
├── src/
│   ├── main.cpp              # Entry point, init GLFW/GLAD/OpenGL
│   ├── renderer/
│   │   ├── Renderer.cpp/h    # Módulo de renderizado (VAO/VBO, draw calls)
│   │   ├── Shader.cpp/h      # Compilación y gestión de shaders GLSL
│   │   └── Texture.cpp/h     # Carga y binding de texturas (stb_image)
│   ├── scene/
│   │   ├── Scene.cpp/h       # Módulo de escena (grafo de escena)
│   │   ├── Model.cpp/h       # Carga de modelos via Assimp
│   │   └── Skybox.cpp/h      # Cubemap/skybox polar
│   ├── camera/
│   │   └── Camera.cpp/h      # Free-fly camera (WASD + mouse + delta time)
│   ├── animation/
│   │   ├── Animator.cpp/h    # Keyframe + LERP/SLERP
│   │   └── Procedural.cpp/h  # Animación procedural (partículas, océano)
│   ├── input/
│   │   └── InputManager.cpp/h # Teclado, mouse, triggers de proximidad
│   ├── audio/
│   │   └── AudioManager.cpp/h # OpenAL, carga WAV/OGG
│   ├── ui/
│   │   └── HUD.cpp/h          # Dear ImGui: paneles info, HUD climático
│   └── particles/
│       └── ParticleSystem.cpp/h # Nieve, humo, metano, hielo
├── shaders/
│   ├── blinn_phong.vs/fs     # Iluminación principal (opaco)
│   ├── fresnel.vs/fs         # Materiales translúcidos (hielo, agua)
│   ├── ocean.vs/fs           # Océano procedural (Perlin Noise, ondas)
│   ├── particle.vs/fs        # Sistema de partículas
│   ├── skybox.vs/fs          # Skybox cubemap
│   └── hud.vs/fs             # Renderizado de UI 2D
├── assets/
│   ├── models/               # Modelos 3D exportados (.obj, .fbx, .gltf)
│   ├── textures/             # Texturas (PNG, JPG, HDR)
│   └── audio/                # Efectos de sonido (WAV, OGG)
├── CMakeLists.txt            # Sistema de build con FetchContent
└── README.txt                # Instrucciones de instalación y uso
```

---

## Configuración del Entorno (Setup)

### Prerrequisitos

1. **Visual Studio 2022 Community** (gratis) — con workload "Desktop development with C++"
2. **CMake 3.20+** — `winget install Kitware.CMake`
3. **Blender 4.x** — [blender.org](https://www.blender.org/)
4. **Git** — ya instalado (proyecto en git)
5. **RenderDoc** — [renderdoc.org](https://renderdoc.org/)

### Dependencias Gestionadas por CMake (FetchContent)

Las siguientes dependencias se descargan automáticamente al hacer `cmake -B build`:
- GLFW (ventana y eventos)
- GLAD (cargador de extensiones OpenGL 3.3 Core)
- GLM (matemáticas)
- Assimp (carga de modelos)
- stb_image (header-only, carga de texturas)
- Dear ImGui (UI/HUD)

OpenAL Soft y libsndfile se instalan por separado o vía vcpkg.

### Comandos de Build

```powershell
# Desde la raíz del proyecto
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel
.\build\Debug\CGEIHC.exe
```

---

## Técnicas OpenGL Requeridas por la Rúbrica

| Requisito | Implementación |
|-----------|---------------|
| 2+ técnicas de iluminación | Blinn-Phong (objetos opacos) + Fresnel (hielo, agua) |
| Environment mapping | Cubemap/skybox + reflection mapping en agua y superficies metálicas |
| Animación básica | Transformaciones directas (rotación de turbinas, traslación de vehículos) |
| Animación keyframe | LERP/SLERP en icebergs derritiéndose, apertura de módulos, personajes |
| Animación procedural | Océano (Perlin Noise en vertex shader), partículas de nieve, niebla |
| Interacción | Free-fly camera + triggers por proximidad + HUD |

---

## Optimizaciones Planificadas

- **Hardware instancing** (rocas, partículas de nieve repetidas)
- **Frustum culling** (no renderizar objetos fuera del campo de visión)
- **Reutilización de shaders**
- **VAO/VBO estáticos** para geometría que no cambia
- **LOD básico** para modelos distantes (si es necesario)
- **Gestión eficiente de memoria gráfica**

---

## Empaquetado Final

- **InstallForge** (gratis) — crear setup.exe con las DLLs necesarias
- Incluir: ejecutable, assets (modelos, texturas, audio), `README.txt`, `manual.pdf`
- El instalador debe funcionar en cualquier PC con driver OpenGL 3.3+
