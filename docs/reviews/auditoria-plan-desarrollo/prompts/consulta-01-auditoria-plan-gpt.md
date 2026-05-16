# Consulta 01 — Auditoría del Plan Maestro de Desarrollo (ChatGPT)

> Proyecto CGEIHC-P — Calentamiento global en el polo norte
> Tipo: Auditoría de plan de implementación C++/OpenGL
> Interfaz: chatgpt.com — contexto embebido

---

## Tu rol

Eres un **ingeniero senior de gráficos en tiempo real** con amplia experiencia en C++17 + OpenGL 3.3 y en el desarrollo de proyectos académicos universitarios. Tienes conocimiento profundo de Assimp, GLFW, Dear ImGui, y sistemas de audio de bajo nivel. Tu tarea es auditar un plan de desarrollo de 12 fases para un proyecto de museo virtual 3D, diseñado para ser ejecutado de forma **completamente autónoma por Claude Code** (un agente de IA).

---

## Contexto del proyecto

**Nombre**: Nuestro Mundo — Museo virtual 3D sobre calentamiento global en el Polo Norte
**Tipo**: Proyecto final universitario (UNAM, Computación Gráfica e Interacción Humano-Computadora, 2026-2)
**Restricción clave**: Todo el código lo escribe Claude Code de forma autónoma. La simplicidad y la viabilidad autónoma son criterios tan importantes como la calidad técnica.

### Stack técnico (fijo, no cambiar)

| Capa | Tecnología | Versión |
|------|-----------|---------|
| Lenguaje | C++17 | — |
| API Gráfica | OpenGL 3.3 Core Profile | — |
| Cargador GL | GLAD 2.x | header-only en src/vendor/ |
| Ventana | GLFW | 3.4 (FetchContent tag congelado) |
| Matemáticas | GLM | 1.0.1 (FetchContent tag congelado) |
| Modelos 3D | Assimp | v5.4.3 (FetchContent tag congelado) |
| Texturas | stb_image | 2.x header-only |
| Screenshots | stb_image_write | 2.x header-only |
| Texto | stb_truetype | 1.x header-only |
| UI/HUD | Dear ImGui | v1.91.6 (FetchContent tag congelado) |
| Audio | miniaudio | 0.11+ header-only (sin DLLs) |
| Build | CMake | 3.20+ con FetchContent |
| IDE | Visual Studio 2022 Community | Windows 11 |

### Restricciones de diseño (académicas)

- **NO implementar**: ECS, sombras dinámicas, PBR completo, IBL, multithreading, deferred rendering, normal mapping complejo
- **Máximo 2 niveles de herencia**: `ModuloBase → ModuloEspecifico`
- **Objetivo de FPS**: ≥45 FPS en hardware modesto

---

## El proyecto en resumen

Museo virtual en T invertida: vestíbulo central → 2 corredores (izquierdo: consecuencias del calentamiento; derecho: soluciones sostenibles) → M5 compartido al final.

**7 módulos interactivos**, cada uno con:
- Plataforma circular r=5m con trigger de proximidad (distancia XZ < 5m)
- Letrero 3D fijo (plano 2×1.2m, stb_truetype → textura OpenGL, siempre visible)
- Animación LERP activada con tecla E (una variable animada por módulo)
- Reset instantáneo al salir del trigger

| Módulo | Animación LERP | Duración |
|--------|---------------|----------|
| M1 izq — Iceberg | scale vec3(1,1,1)→vec3(0.3,0.1,0.3) | 10 s |
| M2 izq — Oso polar | iceScale XZ 1.0→0.2 (disco de hielo) | 8 s |
| M3 izq — Nivel del mar | waterY lerp -1.0→+2.5 | 12 s |
| M1 der — Turbina eólica | rotSpeed lerp 0→120°/s | 5 s |
| M2 der — Auto eléctrico | emissive lerp 0→1 + carZ ping-pong | 6 s |
| M3 der — Árbol/bosque | treeScale lerp 0.05→1.0 | 8 s |
| M5 — Globo terráqueo | alpha lerp secuencial de N líneas | variable |

**3 tipos de animación** (rúbrica obligatoria):
- **Básica**: rotación constante del globo (15°/s), rotación de aspas turbina
- **Keyframe (LERP)**: las 7 animaciones de los módulos
- **Procedural**: olas de agua (función seno), partículas de nieve (80-120 billboards), CO₂ espiral

**4 shaders**:
1. `standard.vert/frag` — Blinn-Phong + Fresnel + niebla exponencial
2. `water.vert/frag` — displacement seno en vértices + Fresnel
3. `skybox.vert/frag` — cubemap con depth trick (gl_Position = pos.xyww)
4. `unlit.vert/frag` — letreros, billboards, partículas

**AppState**: `{ TITULO, JUGANDO, CIERRE, SALIR }`

---

## El pipeline de verificación visual autónoma (clave del plan)

El plan depende de que Claude Code pueda verificar resultados 3D sin intervención humana. El mecanismo:

```
1. Claude modifica código C++/GLSL
2. cmake --build build --config Debug --parallel
3. Ejecutar CGEIHC.exe en background
   → La app guarda automáticamente active/screenshot.png al frame 120 (≈2s)
   → La app se cierra automáticamente
4. Claude lee active/screenshot.png con la herramienta Read
5. Analiza visualmente: ¿geometría correcta? ¿texturas cargadas? ¿iluminación correcta?
6. Itera hasta resultado correcto o defiere como "error diferido" tras 3 intentos fallidos
```

**Implementación técnica del screenshot**:
```cpp
// Frame 120 → guardar y cerrar
void saveScreenshot(const char* path, int w, int h) {
    std::vector<unsigned char> pixels(w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    stbi_flip_vertically_on_write(true);
    stbi_write_png(path, w, h, 3, pixels.data(), w * 3);
}
```

**Modo F12**: captura manual en cualquier momento durante desarrollo interactivo.

**Error diferido**: si 3 iteraciones no resuelven el problema, se registra en PENDIENTES.md con formato estandarizado y se implementa la mitigación más simple disponible.

---

## Las 12 fases del plan

| Fase | Nombre | Criterio de salida |
|------|--------|-------------------|
| 0 | Bootstrap CMake | `.exe` compila; DLLs copiadas automáticamente |
| 1 | Motor core y ventana | Ventana abre; cámara FPS estable; screenshot automático al frame 120 funcional |
| 2 | Pipeline de render básico | Cubo texturizado con matrices MVP correctas |
| 3 | Asset pipeline (Assimp) | Modelo GLTF estático renderizado sin warnings |
| 4 | Layout del museo | Museo navegable; plataformas, letreros, flechas en posición |
| 5 | Sistema de módulos | 7 módulos con LERP funcional, trigger y reset |
| 6 | Blinn-Phong | Escena iluminada, specular visible en ángulos oblicuos |
| 7 | Shaders avanzados | Agua animada con seno; Fresnel visible; skybox ártico; niebla |
| 8 | Efectos y fauna | 80-120 partículas de nieve; fauna posicionada; partículas de módulos |
| 9 | UI, texto y audio | Pantalla de título, [E] Explorar, letreros con texto real, audio |
| 10 | Integración narrativa | Flujo TITULO→JUGANDO→M5 COMPLETADO→CIERRE→SALIR sin crashes |
| 11 | Pulido y QA | Build estable 15+ min; empaquetable con InstallForge |

### Detalle de tareas críticas por fase

**Fase 0** (Bootstrap):
- CMakeLists.txt con FetchContent y tags congelados
- Headers stb + miniaudio.h en src/vendor/ con una sola TU de implementación (#define STB_IMAGE_IMPLEMENTATION, etc.)
- GLAD headers en src/vendor/
- Post-build: copiar DLLs de Assimp automáticamente

**Fase 1** (Motor core):
- `Window.h/.cpp` — GLFW + GL_DEBUG_OUTPUT + glDebugMessageCallback desde el inicio
- `Input.h/.cpp` — GLFW_CURSOR_DISABLED, mouse delta acumulado
- `Time.h/.cpp` — deltaTime clampeado [0.0001, 0.05]
- `CameraFPS.h/.cpp` — WASD 4m/s, pitch ±80°, AABB clamp
- Sistema screenshot: saveScreenshot() + AUTO_SCREENSHOT_FRAME = 120

**Fase 2** (Render básico):
- `Shader.h/.cpp` — load(), use(), setMat4/Vec3/Float/Int con logs de error legibles
- `Mesh.h/.cpp` — VAO/VBO/EBO con Vertex { vec3 pos, vec3 normal, vec2 uv }
- `Texture.h/.cpp` — stb_image → GLuint, GL_LINEAR_MIPMAP_LINEAR
- Cubo de prueba con matrices MVP correctas

**Fase 3** (Asset pipeline):
- `Model.h/.cpp` — Assimp con flags: aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs
- `ResourceManager.h/.cpp` — caché con unordered_map por path

**Fase 4** (Layout museo):
- Plano de terreno (200×200m, grid 64×64 subdivisiones para displacement futuro)
- 7 plataformas circulares (32 segmentos, r=5m, y=0.3m) en posiciones exactas
- AABB clamp del jugador (vestíbulo + 2 corredores + M5)
- `SignSystem.h/.cpp` — stb_truetype → textura 512×256 RGBA por letrero, generada en Init()
- Flechas de suelo con texto "← Las consecuencias" / "Las soluciones →"

**Fase 5** (Sistema módulos):
- `ModuloBase.h/.cpp` — EstadoModulo { INACTIVO, ANIMANDO, COMPLETADO }, float t, animate(float)=0
- Lógica: trigger XZ → INACTIVO; E pressed → ANIMANDO; t=1 → COMPLETADO; salir → reset instantáneo + INACTIVO
- `TriggerZone.h/.cpp` — módulo más cercano dentro de su radio
- 7 módulos concretos: ModuloIceberg, ModuloPolarBear, ModuloSeaLevel, ModuloTurbina, ModuloAuto, ModuloArbol, ModuloGlobo
- `MuseumScene.h/.cpp` — vector<unique_ptr<ModuloBase>>

**Fase 6** (Blinn-Phong):
- standard.vert — Normal matrix = transpose(inverse(mat3(model)))
- standard.frag — Blinn-Phong con halfDir = normalize(lightDir + viewDir), specular con pow(..., 32)
- Luz direccional (sol ártico bajo): lightDir = normalize(0.3, -0.8, 0.5)

**Fase 7** (Shaders avanzados) — ordenados:
- Fresnel en standard.frag: pow(1 - dot(norm, viewDir), 3.0) * fresnelStrength
- water.vert/frag basado en shaders del profesor (13_wavesAnimation-fresnel)
- Skybox cubemap (6 PNG, depth trick xyww)
- Niebla exponencial en standard.frag: exp(-fogDensity * dist)
- **Orden de render obligatorio**: opacos → skybox → agua (glDepthMask GL_FALSE) → partículas → letreros → ImGui → glDepthMask GL_TRUE

**Fases 8-11**: efectos de partículas, UI/audio, integración, QA — ver resumen en tabla anterior.

---

## Preguntas de auditoría

### 1. Orden de fases — ¿Es óptimo?
¿El orden de las 12 fases minimiza retrabajos y bloqueos? ¿Hay alguna tarea en la fase incorrecta? ¿Alguna fase demasiado cargada o vacía? Sé específico: señala fase + tarea concreta si hay un problema.

### 2. Pipeline de verificación visual — ¿Es suficiente?
¿El screenshot al frame 120 + Read tool es suficiente para que Claude detecte errores visuales en 3D (geometría, texturas, iluminación, posición de objetos)? ¿Qué casos de error no capturaría? ¿Hay mejoras concretas que añadirías?

### 3. Tareas faltantes o sobredimensionadas
¿Falta alguna tarea crítica? ¿Hay algo en el plan que es demasiado complejo para un proyecto académico y debería simplificarse?

### 4. Criterios de salida — ¿Son verificables por un agente?
¿Los checkboxes de criterio de salida son suficientemente específicos para que Claude los verifique automáticamente (sin necesidad de input humano)? ¿Alguno es ambiguo o subjetivo?

### 5. Riesgos no cubiertos
¿Hay riesgos técnicos importantes que el plan no menciona? ¿Alguna mitigación propuesta es incorrecta para el stack dado?

---

## Restricciones para tu respuesta

- Responde en **español**
- No sugieras cambios de stack (el stack es fijo)
- No sugieras hacer el proyecto más complejo (es académico, la simplicidad es un valor)
- Sé específico: si hay un problema, indica qué fase/tarea/línea y por qué
- Si algo está bien, dilo explícitamente (no solo señales problemas)

## Formato de respuesta

Para cada pregunta:
```
### [Número] [Título]
**Veredicto**: [Aceptable / Correcciones menores / Correcciones mayores]
**Análisis**: [tu análisis directo]
**Correcciones sugeridas** (si aplica):
- [corrección específica]
```

Al final: **Resumen ejecutivo** con las 3-5 correcciones más importantes y un veredicto global.
