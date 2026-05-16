# Consulta 01 — Plan de Implementación: Museo Virtual 3D en C++/OpenGL

## Tu rol

Eres un **arquitecto de software senior especializado en gráficos por computadora** con experiencia en proyectos OpenGL en C++ para entornos académicos. Tu objetivo es dar un plan de implementación concreto, ordenado y directamente aplicable — sin generalidades.

---

## Contexto del proyecto

**Tipo**: Proyecto final universitario — Museo virtual educativo 3D sobre calentamiento global en el Polo Norte.
**Institución**: UNAM, Facultad de Ingeniería — Computación Gráfica e Interacción Humano-Computadora (CGEIHC), 2026-2.

### Stack tecnológico (fijo, no negociable)

- **Lenguaje**: C++17
- **API gráfica**: OpenGL 3.3 Core Profile + GLSL
- **Ventana/eventos**: GLFW 3.3+
- **Cargador GL**: GLAD 2.x (NO GLEW)
- **Matemáticas**: GLM
- **Carga de modelos 3D**: Assimp (GLTF preferido, OBJ alternativa)
- **Texturas**: stb_image (header-only)
- **Texto/fuentes**: stb_truetype (rasterización en CPU → textura OpenGL)
- **UI/HUD**: Dear ImGui
- **Audio**: OpenAL Soft + libsndfile
- **Build**: CMake 3.20+ con FetchContent
- **IDE**: Visual Studio 2022 Community

### Requisitos obligatorios de rúbrica (debe cumplir TODO)

1. OpenGL 3.3+ Core Profile con GLSL
2. Mínimo 2 técnicas de iluminación: **Blinn-Phong** + **Fresnel**
3. Environment mapping: **cubemap skybox** estático
4. **3 tipos de animación**:
   - Básica (transformaciones directas, sin interpolación)
   - Keyframe (LERP/SLERP — interpolación entre dos valores)
   - Procedural (generativa, basada en funciones matemáticas o partículas)
5. Recorrido narrativo con punto inicial y final definidos

---

## Arquitectura del museo (ya diseñada)

Layout en **T invertida**:

```
         [VESTÍBULO]  ← posición inicial del jugador (0, 1.7, 0)
          /       \
    [CORREDOR IZQ]  [CORREDOR DER]
    M1→M2→M3        M1→M2→M3
          \       /
           [ M5 ]  ← final del recorrido
```

- Dimensiones: vestíbulo 20×20m, corredores 8m×60m, 1 unidad = 1 metro
- Plataformas circulares r=5m elevadas 0.3m con borde emisivo
- Trigger de proximidad: distancia XZ < 5m activa módulo
- HUD completamente vacío (solo texto `[E] Explorar` cuando hay trigger activo)
- Letreros 3D fijos (planos 2×1.2m) con texto completo, siempre visibles, orientación fija hacia el corredor

### Módulos y sus animaciones (LERP keyframe, tecla E)

| ID | Nombre | Animación principal |
|----|--------|---------------------|
| M1 izq | El hielo que desaparece | scale lerp `(1,1,1)→(0.3,0.1,0.3)` en 10s |
| M2 izq | Los últimos habitantes | XZ scale lerp disco de hielo `1.0→0.2` en 8s (oso GLTF idle en loop) |
| M3 izq | El mar que avanza | waterY lerp `-1.0→+2.5` en 12s (shader de olas) |
| M1 der | La energía del planeta | rotSpeed lerp `0→120°/s` en 5s + billboard particles |
| M2 der | Moverse sin quemar | emissive lerp `0→1` en 2s + carZ lerp A→B→A en 6s |
| M3 der | La Tierra respira | scale lerp `0.05→1.0` en 8s + partículas CO₂ espiral |
| M5 | Juntos o no funciona | rotación constante globo + alpha lerp secuencial de N líneas |

### Entorno y efectos

- **Skybox**: cubemap estático 6 caras PNG (ártico con aurora boreal)
- **Terreno**: plano único con heightmap estático ±0.3m (calculado al inicio)
- **Niebla**: exponencial en fragment shader
- **Partículas de nieve**: 80-120 billboards, actualización CPU → GPU cada frame
- **Fauna decorativa**: foca, zorro, gaviota (estáticas, sin interacción)

### Shaders necesarios

1. **Shader principal**: Blinn-Phong + Fresnel + niebla integrada
2. **Shader de agua**: vertex displacement seno + Fresnel en fragmento
3. **Shader de skybox**: cubemap sampler, depth trick `gl_Position = pos.xyww`
4. **Shader unlit**: letreros, billboards, partículas (sin iluminación)

### Assets externos

- 7 modelos 3D Sketchfab CC0 en GLTF (iceberg, oso polar con idle, edificios, turbina, auto, árbol, globo)
- Texturas CC0 de ambientcg.com (hielo, agua normal map)
- Skybox HDR de polyhaven.com → convertir con `cmgen` a 6 PNG
- Fuente TTF Roboto (stb_truetype)
- 5 audios: 1 OGG ambient + 4 WAV efectos
- 1 foto PNG ártica para pantalla de título (Unsplash)

### UI/Pantallas

- **Pantalla de título**: foto ártica fullscreen + título "Nuestro Mundo" + `[Enter] Comenzar` + fade-in
- **Pantalla de cierre**: se activa con Escape desde M5 en estado COMPLETADO — texto de reflexión + créditos
- **Dear ImGui**: gestiona título, cierre, e indicador `[E] Explorar`
- **Estados de app**: `TITULO → JUGANDO → CIERRE → SALIR`

### Audio

- OpenAL Soft + libsndfile
- 1 fuente global ambient (OGG, loop)
- 1 sonido 2D de activación (WAV, al presionar E)
- 3 fuentes 3D posicionales (M1 izq, M3 izq, M3 der) con atenuación lineal

---

## Preguntas específicas

### 1. Orden óptimo de desarrollo

¿Cuál es la secuencia de fases que minimiza retrabajos y bloqueos para este proyecto concreto? Considera que:
- Hay shaders avanzados que dependen de la geometría base
- Los módulos son independientes entre sí una vez que el sistema de trigger funciona
- Assimp + GLTF puede ser un cuello de botella si no se prueba temprano
- El audio es completamente independiente del pipeline gráfico

### 2. Prerequisitos antes de shaders avanzados

¿Qué debe estar funcionando y verificado con OpenGL básico antes de introducir Blinn-Phong, Fresnel y el shader de agua?

### 3. Estructura del código fuente

Propón una estructura de carpetas `src/` y las clases principales con sus responsabilidades. Considera:
- Motor mínimo (no un engine completo)
- Sistema de módulos que todos compartan el mismo patrón (trigger → estado → LERP → reset)
- Un `ModuleBase` o similar que abstraiga el comportamiento común sin over-engineering
- Gestión de shaders (compilar, cachear, uniforms)
- Gestión de assets (cargar una vez, reutilizar)
- Bucle principal con estados de app

### 4. Riesgos técnicos y mitigaciones

Identifica los 3-5 riesgos más probables de este stack/proyecto y cómo mitigarlos. Considera:
- Assimp + GLTF con animaciones (clips idle del oso polar)
- stb_truetype rasterizando texto al inicio
- Dear ImGui coexistiendo con el pipeline OpenGL
- OpenAL Soft inicialización en Windows con VS2022
- CMake FetchContent con dependencias externas en red universitaria

### 5. Criterios de "listo para pasar a la siguiente fase"

Para cada fase propuesta, define un criterio de salida concreto y verificable (no subjetivo). Ejemplo del formato esperado:
> "Fase N está lista cuando: el ejecutable compila sin warnings, la ventana abre a 60fps estables en la máquina de desarrollo, y [criterio específico de esa fase]."

---

## Formato de respuesta esperado

Estructura tu respuesta así:

1. **Fases de desarrollo** (tabla o lista numerada con nombre + objetivo + duración estimada)
2. **Prerequisitos para shaders avanzados** (checklist)
3. **Estructura de código** (`src/` con árbol de directorios + descripción de cada clase)
4. **Riesgos y mitigaciones** (tabla: riesgo | probabilidad | mitigación)
5. **Criterios de salida por fase** (uno por fase, concreto y verificable)

Sé directo y específico. Evita recomendaciones genéricas como "usa buenas prácticas" — cada recomendación debe ser accionable en este proyecto con este stack.
