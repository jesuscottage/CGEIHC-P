# Calentamiento Global en el Polo Norte

**Tipo**: Proyecto Final — Computación Gráfica e Interacción Humano-Computadora
**Institución**: UNAM, Facultad de Ingeniería — Semestre 2026-2
**Docente**: Dr. Sergio Teodoro Vite

## Descripción

Sistema gráfico interactivo 3D que simula un **museo virtual educativo** ambientado en el Polo Norte. El usuario recorre dos rutas narrativas que abordan el calentamiento global desde perspectivas opuestas:

- **Ruta izquierda**: Consecuencias científicas y ambientales del calentamiento global
- **Ruta derecha**: Soluciones sostenibles y tecnológicas

El proyecto demuestra la aplicación del pipeline gráfico completo: modelado geométrico, iluminación, texturizado, animación, simulación e interacción.

## Equipo

| Nombre | Cuenta |
|--------|--------|
| Hernández Cabañas Jesús | 422062224 |
| Navarro Nuño Juan Pablo | 319086779 |
| Macias Niño Carmen Violeta | 318285450 |
| Martinez Jimenez Israel | 312032184 |

## Stack Técnico

- **Lenguaje**: C++17
- **API Gráfica**: OpenGL 3.3 Core Profile + GLSL
- **Ventana/Eventos**: GLFW
- **Extensiones OpenGL**: GLAD
- **Matemáticas**: GLM
- **Modelos 3D**: Assimp
- **Texturas**: stb_image
- **UI/HUD**: Dear ImGui
- **Audio**: miniaudio 0.11 (header-only, reemplaza OpenAL)
- **Build**: CMake 4.x con FetchContent
- **Generación de audio**: edge-tts (Python, voz `es-ES-AlvaroNeural`)

## Compilar y ejecutar

```powershell
# Solo la primera vez
& "C:\Program Files\CMake\bin\cmake.exe" -S app -B build -G "Visual Studio 18 2026"

# Compilar y ejecutar
& "C:\Program Files\CMake\bin\cmake.exe" --build build --config Release --target CGEIHC --parallel
.\build\Release\CGEIHC.exe
```

## Controles

| Tecla | Acción |
|-------|--------|
| `WASD` | Moverse |
| Ratón | Girar cámara |
| `E` | Activar animación del módulo + popup narrativo |
| `TAB` | Cerrar popup y detener narración |
| `ESC` | Salir |

## Documentación

| Archivo | Descripción |
|---------|-------------|
| `PENDIENTES.md` | **Leer primero** — estado actual, cambios recientes, instrucciones de build |
| `INFRAESTRUCTURA.md` | Stack técnico completo y arquitectura |
| `CLAUDE.md` | Instrucciones para Claude Code |
| `docs/knowledge/INDEX.md` | Índice de la base de conocimiento |

## Recursos del Proyecto

- Texturas CC0: [ambientcg.com](https://ambientcg.com/), [polyhaven.com](https://polyhaven.com/), [3dtextures.me](https://3dtextures.me/)
- Modelos IA: [meshy.ai](https://www.meshy.ai/)
- Referencia OpenGL: [learnopengl.com](https://learnopengl.com/)
- Plataforma del curso: [cursos.computer-graphics.com.mx](https://cursos.computer-graphics.com.mx/)
