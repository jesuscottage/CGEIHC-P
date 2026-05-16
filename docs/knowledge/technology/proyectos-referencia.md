---
title: Proyectos de Referencia — Base de Código OpenGL C++
date: 2026-05-16
category: technology
tags: [opengl, referencia, codigo, glsl, shaders, animacion, assimp, imgui]
status: activo
---

# Proyectos de Referencia — Base de Código OpenGL C++

> Este documento es una base de código de referencia viva.
> Se actualiza durante la **Fase 0B** del plan maestro y se enriquece en cada fase cuando se encuentran recursos adicionales.
> Objetivo: partir de código funcional verificado, no inventar desde cero.

---

## Cómo usar este documento

1. Antes de implementar una tarea compleja, busca en la categoría correspondiente
2. Lee el código de referencia (WebFetch o clonar localmente en `active/` temporal)
3. Extrae el fragmento útil y adáptalo al stack y convenciones del proyecto
4. El código adaptado **siempre entra por `app/src/`** — nunca dejar código fuente fuera del repo

---

## Categoría: OpenGL Core — Proyectos Generales

Proyectos con arquitectura completa en C++ + OpenGL 3.3 Core Profile. Útiles para estructura de clases, loop principal, manejo de errores.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-01 | LearnOpenGL — Joey de Vries | `github.com/JoeyDeVries/LearnOpenGL` | Shader.h, Mesh.h, Model.h, Camera.h — estructura de clases probada y limpia | 1, 2, 3, 6 |
| R-02 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- LearnOpenGL (R-01) usa exactamente OpenGL 3.3 Core, GLFW, GLAD, GLM, Assimp y stb_image — mismo stack que este proyecto. Sus `Shader.h`, `Mesh.h` y `Camera.h` son las referencias primarias para las Fases 1-3.

---

## Categoría: Iluminación — Blinn-Phong y Fresnel

Shaders de iluminación Blinn-Phong con términos ambiental/difuso/especular y efecto Fresnel.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-10 | Shaders del profesor — `13_wavesAnimation-fresnel.vs/.fs` | `docs/knowledge/technology/materiales-curso.md` | Fresnel term: `pow(1 - dot(N, V), 3.0)` — base para standard.frag | 6, 7B |
| R-11 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- R-10 es el shader más importante: el profesor lo usó en clase y define exactamente la fórmula Fresnel esperada en la rúbrica.
- Buscar implementaciones con `halfDir = normalize(lightDir + viewDir)` para Blinn-Phong (no Phong clásico).

---

## Categoría: Agua — Shader Procedural con Vertex Displacement

Shaders de agua con desplazamiento de vértices usando función seno y Fresnel en fragmento.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-20 | Shaders del profesor — base de ondas | `docs/knowledge/technology/materiales-curso.md` | Displacement seno en vertex shader: `y += amplitude * sin(freq * x + time)` | 7B |
| R-21 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- El shader de agua es el elemento técnicamente más frágil. Conviene tener al menos 3 referencias con el displacement implementado diferente para poder comparar.
- Buscar: "vertex displacement water shader OpenGL GLSL sine wave".

---

## Categoría: Skybox — Cubemap

Implementación de skybox con cubemap, depth trick (`gl_Position = pos.xyww`) y carga de 6 caras PNG.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-30 | LearnOpenGL — Capítulo Cubemaps | `learnopengl.com/Advanced-OpenGL/Cubemaps` | Skybox VAO/VBO, shader xyww trick, `glTexImage2D` para 6 caras | 7A |
| R-31 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- Buscar implementaciones que incluyan el fallback procedural (gradiente de color) para cuando no hay cubemap PNG disponible.

---

## Categoría: Niebla Exponencial

Niebla en fragment shader integrada con iluminación.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-40 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- Buscar: "fog shader OpenGL GLSL exponential fragment shader".
- Fórmula: `float fogFactor = exp(-fogDensity * dist)` mezclada con `fogColor` en el fragmento.
- Algunos proyectos la integran en el shader principal (preferido) vs post-proceso (evitar).

---

## Categoría: Partículas y Billboards

Sistema de partículas con quads orientados a cámara (billboarding) para nieve y efectos de módulos.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-50 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- Buscar: "OpenGL billboard particles C++ instancing" o sin instancing (más simple).
- La nieve usa 80-120 quads; no se necesita instancing GPU — VBO de posiciones actualizado por CPU es suficiente.
- Buscar también "snow particle system OpenGL".

---

## Categoría: Animaciones LERP y Keyframe

Interpolación lineal de transformaciones (posición, escala, rotación) activada por evento.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-60 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- Buscar: "LERP animation OpenGL C++ scale position keyframe".
- El patrón clave es `t = min(t + dt / duration, 1.0f)` con `glm::mix()` — verificar que algún proyecto lo implementa con reset al salir del trigger.
- La animación del iceberg (escala) y nivel del mar (Y position) son los más complejos — buscar ejemplos de escala LERP de objeto completo.

---

## Categoría: Carga de Modelos con Assimp

Integración de Assimp para cargar GLTF/OBJ, procesamiento de meshes y texturas embebidas.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-70 | LearnOpenGL — Model Loading | `learnopengl.com/Model-Loading/Assimp` | Model.h/.cpp con procesamiento de nodos recursivo, carga de texturas por material | 3 |
| R-71 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- R-70 es la referencia primaria. Verificar que incluye `aiProcess_GenSmoothNormals` para modelos sin normales.
- Buscar proyectos que cargan GLTF específicamente (algunos solo muestran OBJ).

---

## Categoría: Dear ImGui — HUD y Overlay de Debug

Integración de Dear ImGui con GLFW + OpenGL 3.3, ventanas de debug y HUD de juego.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-80 | Dear ImGui — ejemplos oficiales | `github.com/ocornut/imgui/tree/master/examples` | `example_glfw_opengl3/` — inicialización completa con GLFW + OpenGL3 backend | 2, 9 |
| R-81 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- R-80 es la fuente oficial. El ejemplo `example_glfw_opengl3` tiene exactamente la inicialización que se necesita.
- Buscar proyectos de juegos C++ con HUD en ImGui para referencia de `ImGui::SetNextWindowPos` con posición fija.

---

## Categoría: Texto en OpenGL — stb_truetype

Renderizado de texto con stb_truetype generando atlas de texturas.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-90 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- Buscar: "stb_truetype OpenGL texture atlas C++ example".
- Crítico: encontrar ejemplo que use `stbtt_PackFontRanges` (no solo `stbtt_BakeFontBitmap`) para poder especificar rangos de glifos personalizados (Latin-1 para español).
- Buscar también ejemplos de text rendering con quads indexados y UV mapping correcto.

---

## Categoría: Audio — miniaudio

Integración de miniaudio para audio 2D, loop ambiental y audio 3D posicional.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-100 | miniaudio — ejemplos oficiales | `github.com/mackron/miniaudio/tree/master/examples` | `simple_playback.c`, `spatial.c` — audio 3D posicional con `ma_sound_set_position` | 9 |
| R-101 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- R-100 es la fuente oficial. El ejemplo `spatial.c` cubre exactamente lo que se necesita para audio 3D de los módulos.
- Verificar `ma_engine_listener_set_position` vs `ma_sound_set_position` (el primero mueve el oyente, el segundo la fuente).

---

## Categoría: Museo / Recorrido FPS en OpenGL

Proyectos de demostración de recorrido en primera persona por espacios interiores.

| # | Título / Autor | Repositorio / URL | Qué código aprovechar | Aplica a Fase |
|---|----------------|-------------------|----------------------|---------------|
| R-110 | _(agregar en Fase 0B)_ | — | — | — |

**Notas**:
- Buscar: "OpenGL museum demo C++ first person" o "OpenGL gallery walkthrough github".
- También útil: "OpenGL room interior demo" para referencia de layout y colisiones AABB.
- El AABB clamp del jugador (no atravesar paredes) es el fragmento más valioso.

---

## Registro de proyectos investigados — Historial

| Fecha | Fase que motivó la búsqueda | Término buscado | Resultado |
|-------|----------------------------|-----------------|-----------|
| _(llenar en Fase 0B)_ | — | — | — |

---

## Cómo agregar una entrada

```markdown
| R-NN | Título claro del proyecto / Autor | URL o ruta del repo | Fragmento específico útil (clase, función, shader) | Fase N |
```

Reglas:
- Solo agregar proyectos con código **funcional y verificado** (no tutoriales sin código)
- Incluir el fragmento específico que se va a aprovechar (no solo "el proyecto completo")
- Si el código se adapta al proyecto, registrar en qué archivo de `app/src/` quedó
