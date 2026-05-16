---
title: Proyectos de Referencia — Base de Código OpenGL C++
date: 2026-05-16
category: technology
tags: [opengl, referencia, codigo, glsl, shaders, animacion, assimp, imgui]
status: activo
---

# Proyectos de Referencia — Base de Código OpenGL C++

> Catalogado en Fase 0B — 2026-05-16
> Objetivo: partir de código funcional verificado, no inventar desde cero.

---

## Cómo usar este documento

1. Antes de implementar una tarea compleja, busca en la categoría correspondiente
2. Adapta el fragmento al stack y convenciones del proyecto
3. El código adaptado **siempre entra por `app/src/`**

---

## R-01 — OpenGL Core: LearnOpenGL (Joey de Vries)

**URL**: `github.com/JoeyDeVries/LearnOpenGL`
**Fases**: 1, 2, 3, 6
**Stack**: OpenGL 3.3 Core, GLFW, GLAD, GLM, Assimp, stb_image — **idéntico al nuestro**

Clases a aprovechar:
- `includes/learnopengl/camera.h` → CameraFPS (yaw, pitch, WASD, mouse, getViewMatrix)
- `includes/learnopengl/shader.h` → Shader (compilar, linkear, setUniform)
- `includes/learnopengl/mesh.h` → Mesh (VAO, VBO, EBO, Draw)
- `includes/learnopengl/model.h` → Model (Assimp scene, recursión de nodos)

---

## R-02 — OpenGL Core: SaschaWillems openglcpp

**URL**: `github.com/SaschaWillems/openglcpp`
**Fases**: 1, 2
**Qué aprovechar**: Ejemplos simples de VAO/VBO con GLFW. Útil como segunda referencia cuando LearnOpenGL no cubre un caso específico.

---

## R-10 — Iluminación: Shader del profesor (Fresnel + ondas)

**URL**: `docs/knowledge/technology/materiales-curso.md`
**Fases**: 6, 7B
**Fragmento clave**:
```glsl
// Fresnel approximation (Schlick)
float fresnelBase = max(0.0, 1.0 - dot(normalize(N), normalize(V)));
float fresnel = fZero + (1.0 - fZero) * pow(fresnelBase, 5.0);
// Donde fZero = 0.02 para hielo/agua
```

---

## R-11 — Iluminación: Blinn-Phong (LearnOpenGL Advanced Lighting)

**URL**: `learnopengl.com/Advanced-Lighting/Advanced-Lighting`
**Fases**: 6
**Fragmento clave**:
```glsl
// En el fragment shader
vec3 lightDir   = normalize(lightPos - FragPos);
vec3 viewDir    = normalize(viewPos - FragPos);
vec3 halfwayDir = normalize(lightDir + viewDir);
float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
vec3 specular = lightColor * spec;
// shininess recomendado: 32 (opaco) a 128 (espejo)
```

---

## R-20 — Agua: Shader de ondas sinusoidales

**URL**: `jayconrod.com/posts/34/water-simulation-in-glsl`
**URL2**: `mathweb.ucsd.edu/~sbuss/CourseWeb/Math155B_2019Spring/Project_2/`
**Fases**: 7B
**Fragmento clave (vertex shader)**:
```glsl
// Suma de senos para desplazamiento vertical del agua
float wave(float x, float z, float t) {
    return A1 * sin(k1x*x + k1z*z - w1*t)
         + A2 * sin(k2x*x + k2z*z - w2*t);
}
void main() {
    vec3 pos = aPos;
    pos.y += wave(pos.x, pos.z, time);
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
```
Parámetros de partida para el océano ártico: A=0.3, k=0.5, w=1.2

---

## R-21 — Agua: Ocean Simulation (Brennan Andruss)

**URL**: `brennanandruss.github.io`
**Fases**: 7B
**Qué aprovechar**: Cálculo de normales en el vertex shader para que las olas tengan iluminación correcta.

---

## R-30 — Skybox: LearnOpenGL Cubemaps

**URL**: `learnopengl.com/Advanced-OpenGL/Cubemaps`
**URL repo**: `github.com/JoeyDeVries/LearnOpenGL/blob/master/src/4.advanced_opengl/6.1.cubemaps_skybox/cubemaps_skybox.cpp`
**Fases**: 7A
**Fragmento clave**:
```glsl
// Vertex shader del skybox (depth trick: xyww)
out vec3 TexCoords;
uniform mat4 projection;
uniform mat4 view;
void main() {
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // hace que depth = 1.0 (fondo)
}
```
```cpp
// loadCubemap con stb_image
for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &nrCh, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
```
**Orden de caras**: right, left, top, bottom, front, back

---

## R-31 — Skybox: 3D C++ Tutorials (OpenGL 3.3)

**URL**: `http://www.3dcpptutorials.sk/index.php?id=9`
**Fases**: 7A
**Qué aprovechar**: Implementación alternativa con clase Skybox encapsulada.

---

## R-40 — Niebla: Megabyte Softworks OpenGL 3 Fog

**URL**: `www.mbsoftworks.sk/tutorials/opengl3/12-fog-outside/`
**Fases**: 7A
**Fragmento clave (fragment shader)**:
```glsl
// Niebla exponencial cuadrática (más suave que lineal)
float fogDensity = 0.03; // ajustar según visibilidad deseada
float fogDistance = gl_FragCoord.z / gl_FragCoord.w;
float fogFactor = 1.0 - clamp(exp(-pow(fogDensity * fogDistance, 2.0)), 0.0, 1.0);
vec3 fogColor = vec3(0.7, 0.8, 0.9); // blanco ártico
fragColor.rgb = mix(fragColor.rgb, fogColor, fogFactor);
```

---

## R-41 — Niebla: glsl-fog (Hugh Kennedy)

**URL**: `github.com/hughsk/glsl-fog`
**Fases**: 7A
**Qué aprovechar**: Funciones listas `fogFactorExp` y `fogFactorExp2` en GLSL. Muy compactas.

---

## R-50 — Partículas billboard: opengl-tutorial.org Tutorial 18

**URL**: `http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/`
**URL repo**: `github.com/opengl-tutorials/ogl/blob/master/tutorial18_billboards_and_particles/tutorial18_billboards.cpp`
**Fases**: 8
**Fragmento clave (vertex shader)**:
```glsl
// Billboard: expandir quad usando vectores right/up de la view matrix
vec3 CameraRight = vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
vec3 CameraUp    = vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
vec3 vertPos = particleCenter
             + CameraRight * squareVertices.x * BillboardSize.x
             + CameraUp    * squareVertices.y * BillboardSize.y;
gl_Position = VP * vec4(vertPos, 1.0);
```
**C++**: `glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)` — 4 vértices por billboard.

---

## R-51 — Partículas: LearnOpenGL Particles (2D Game)

**URL**: `learnopengl.com/In-Practice/2D-Game/Particles`
**Fases**: 8
**Qué aprovechar**: Sistema de pool de partículas con respawn, vida útil (life), velocidad, color con alpha fade.

---

## R-60 — Animación LERP: Implementación C++ simple

**URL**: `bsmarted.com/en/topics/opengl/implementing-animation-basics`
**URL wiki**: `www.khronos.org/opengl/wiki/Keyframe_Animation`
**Fases**: 5
**Patrón C++ para módulos**:
```cpp
// t va de 0.0 a 1.0 a lo largo de la duración de la animación
float t = glm::clamp(elapsed / duration, 0.0f, 1.0f);
// LERP de escala (ej. iceberg)
glm::vec3 scale = glm::mix(scaleStart, scaleEnd, t);
model = glm::scale(glm::mat4(1.0f), scale);
// Ease-in-out opcional: t = t*t*(3-2*t) (smoothstep)
float ts = t * t * (3.0f - 2.0f * t);
```
**Trigger**: `if (proximidadXZ < 5.0f && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) animActive = true;`

---

## R-61 — Animación LERP: Improved Lerp Smoothing (Game Developer)

**URL**: `gamedeveloper.com/programming/improved-lerp-smoothing-`
**Fases**: 5
**Qué aprovechar**: Frame-rate independent lerp: `value = glm::mix(target, value, pow(smoothFactor, dt))` — no necesita acumular `t` explícitamente.

---

## R-70 — Assimp GLTF: LearnOpenGL Model Loading

**URL**: `learnopengl.com/Model-Loading/Assimp`
**URL modelo**: `learnopengl.com/Model-Loading/Model`
**URL mesh**: `learnopengl.com/Model-Loading/Mesh`
**Fases**: 3
**Flags recomendados para GLTF**:
```cpp
unsigned int flags = aiProcess_Triangulate
                   | aiProcess_FlipUVs
                   | aiProcess_GenNormals
                   | aiProcess_CalcTangentSpace;
const aiScene* scene = importer.ReadFile(path, flags);
```
**NOTA**: para GLTF con Y-up correcto (Blender export), no usar `aiProcess_MakeLeftHanded`.

---

## R-71 — Assimp: Megabyte Softworks Tutorial 20

**URL**: `www.mbsoftworks.sk/tutorials/opengl3/20-assimp-model-loading/`
**Fases**: 3
**Qué aprovechar**: Manejo de texturas embebidas en GLTF (acceso via `aiScene::GetEmbeddedTexture`).

---

## R-80 — Dear ImGui: ejemplo oficial GLFW + OpenGL3

**URL**: `github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp`
**Fases**: 2, 9
**Patrón de inicialización**:
```cpp
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGui_ImplGlfw_InitForOpenGL(window, true);
ImGui_ImplOpenGL3_Init("#version 330");
// En cada frame:
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplGlfw_NewFrame();
ImGui::NewFrame();
// ... widgets ...
ImGui::Render();
ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
```

---

## R-81 — Dear ImGui: BasicGLFWImgui

**URL**: `github.com/gwerners/BasicGLFWImgui`
**Fases**: 2
**Qué aprovechar**: Proyecto mínimo con CMake que integra ImGui + GLFW + OpenGL3. Útil como referencia de CMakeLists.txt con ImGui.

---

## R-90 — stb_truetype: Render de Texto en OpenGL

**URL**: `dev.to/shreyaspranav/how-to-render-truetype-fonts-in-opengl-using-stbtruetypeh-1p5k`
**Fases**: 9
**Fragmento clave (atlas setup)**:
```cpp
stbtt_pack_context ctx;
stbtt_PackBegin(&ctx, (unsigned char*)bitmap, atlasW, atlasH, 0, 1, nullptr);
stbtt_PackFontRange(&ctx, fontData, 0, fontSize, 32, 96, packedChars); // ASCII 32-127
stbtt_PackEnd(&ctx);
// Textura GL_R8 (1 canal)
glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, atlasW, atlasH, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
// Para caracteres con acento (español): agregar rango 192-255
```
**Para QuadRendering**: `stbtt_GetPackedQuad(packedChars, atlasW, atlasH, charIdx, &x, &y, &quad, 0)`

---

## R-91 — stb_truetype: Subpixel rendering (Arkanis)

**URL**: `arkanis.de/weblog/2023-08-14-simple-good-quality-subpixel-text-rendering-in-opengl-with-stb-truetype-and-dual-source-blending/`
**Fases**: 9
**Qué aprovechar**: Técnica para mejorar legibilidad del texto pequeño en letreros 3D.

---

## R-100 — miniaudio: Espacialización 3D

**URL**: `miniaud.io/docs/examples/simple_spatialization.html`
**Fases**: 9
**Fragmento clave**:
```c
// miniaudio sigue el mismo sistema de coordenadas que OpenGL (-Z = forward)
ma_sound_init_from_file(&engine, "audio.ogg", 0, NULL, NULL, &sound);
ma_sound_set_position(&sound, x, y, z);       // posición del sonido en world space
ma_engine_listener_set_position(&engine, 0, 0, 0, 0); // posición del listener = cámara
ma_sound_set_looping(&sound, MA_TRUE);
ma_sound_start(&sound);
// Cada frame: actualizar posición del listener con la posición de la cámara
ma_engine_listener_set_position(&engine, 0, camPos.x, camPos.y, camPos.z, 0);
```

---

## R-101 — miniaudio: Documentación principal

**URL**: `github.com/mackron/miniaudio`
**Fases**: 9
**Qué aprovechar**: Referencia completa de `ma_engine`, `ma_sound`, `ma_decoder`, attenuation models.

---

## R-110 — Recorrido FPS: LearnOpenGL Camera

**URL**: `learnopengl.com/Getting-started/Camera`
**Fases**: 4, 5
**Implementación**: WASD + mouse con `glm::lookAt`. Pitch clampeado ±89°. Velocidad 4 m/s.

---

## R-111 — Colisiones AABB: LearnOpenGL Collision Detection

**URL**: `learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection`
**Fases**: 4
**Patrón AABB simple para colisión con paredes**:
```cpp
// Verificar si la nueva posición de la cámara está dentro del AABB del museo
// minX/maxX = límites X del corredor, minZ/maxZ = límites Z
glm::vec3 newPos = camera.position + velocity * dt;
newPos.x = glm::clamp(newPos.x, MUSEUM_MIN_X, MUSEUM_MAX_X);
newPos.z = glm::clamp(newPos.z, MUSEUM_MIN_Z, MUSEUM_MAX_Z);
camera.position = newPos;
```
Suficiente para los corredores del museo (geometría rectangular simple).

---

## Mapa rápido por Fase

| Fase | Referencias |
|------|-------------|
| Fase 1 — Motor Core | R-01 (Camera.h), R-02 |
| Fase 2 — Render básico | R-01 (Shader.h, Mesh.h), R-80 (ImGui) |
| Fase 3 — Assimp | R-70, R-71 |
| Fase 4 — Layout museo | R-110, R-111 |
| Fase 5 — Módulos LERP | R-60, R-61 |
| Fase 6 — Blinn-Phong | R-10, R-11 |
| Fase 7A — Skybox + niebla | R-30, R-31, R-40, R-41 |
| Fase 7B — Agua | R-20, R-21, R-10 |
| Fase 8 — Partículas | R-50, R-51 |
| Fase 9 — UI + Audio | R-80, R-81, R-90, R-91, R-100, R-101 |
| Fases 10-11 | todas las anteriores |
