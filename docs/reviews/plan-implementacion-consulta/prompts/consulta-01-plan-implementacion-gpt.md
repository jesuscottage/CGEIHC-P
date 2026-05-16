# Plan de Implementación Óptimo — Museo Virtual 3D C++/OpenGL

## Tu rol

Eres un **ingeniero senior de gráficos en tiempo real** con amplia experiencia en **proyectos C++/OpenGL, pipeline de renderizado, sistemas de animación y arquitectura de motores gráficos académicos**.

## Proyecto

**CGEIHC-P** — Museo virtual educativo 3D sobre calentamiento global en el Polo Norte — proyecto final universitario C++/OpenGL, UNAM Facultad de Ingeniería, CGEIHC 2026-2.

## Contexto

Tenemos 8 blueprints de diseño completamente definidos y necesitamos el plan de implementación más robusto posible. El stack es fijo. El ejecutable se entrega en Windows (.exe).

## Stack tecnológico (fijo, no negociable)

| Componente | Tecnología |
|------------|-----------|
| Lenguaje | C++17 |
| API gráfica | OpenGL 3.3 Core Profile + GLSL |
| Ventana/eventos | GLFW 3.3+ |
| Cargador GL | GLAD 2.x (NO GLEW) |
| Matemáticas | GLM |
| Modelos 3D | Assimp (GLTF preferido, OBJ alternativa) |
| Texturas | stb_image (header-only) |
| Texto | stb_truetype (rasteriza fuente TTF → textura OpenGL) |
| UI/HUD | Dear ImGui |
| Audio | OpenAL Soft + libsndfile |
| Build | CMake 3.20+ con FetchContent |
| IDE | Visual Studio 2022, Windows |

## Requisitos de rúbrica (obligatorios)

1. OpenGL 3.3+ Core Profile con GLSL
2. Blinn-Phong + Fresnel (mínimo 2 técnicas de iluminación)
3. Cubemap skybox (environment mapping)
4. 3 tipos de animación: **básica** (transformaciones directas) + **keyframe** (LERP) + **procedural** (seno/partículas)
5. Recorrido narrativo con punto inicial y final

## Arquitectura del museo

Layout T invertida: Vestíbulo (20×20m) → dos corredores (8×60m c/u) → M5 compartido (20×20m).
7 módulos interactivos en plataformas circulares r=5m.
1 unidad = 1 metro. Cámara FPS a y=1.7m. Movimiento WASD, 4m/s, sin salto.

**Sistema de módulos** (patrón compartido por los 7):
```
Trigger: dist XZ < 5m → mostrar [E] Explorar
E presionada → EstadoModulo: INACTIVO → ANIMANDO → COMPLETADO
Al salir: reset instantáneo (t=0)
Estructura: struct Modulo { float t; EstadoModulo estado; GLuint vao; ... }
```

**7 módulos y sus animaciones LERP (tecla E):**

| Módulo | Variable animada | Inicial → Final | Duración |
|--------|-----------------|-----------------|----------|
| M1 izq — iceberg | scale vec3 | (1,1,1)→(0.3,0.1,0.3) | 10s |
| M2 izq — oso/disco hielo | iceScale float (XZ) | 1.0→0.2 | 8s |
| M3 izq — nivel del mar | waterY float | -1.0→+2.5 | 12s |
| M1 der — turbina | rotSpeed float | 0→120°/s | 5s |
| M2 der — auto eléctrico | emissive + carZ float | 0→1 / A→B→A | 2s / 6s |
| M3 der — árbol | treeScale float | 0.05→1.0 | 8s |
| M5 — globo+líneas | lineAlpha[N] | 0→1 secuencial | 0.5s/línea |

El oso polar (M2 izq) tiene un clip idle GLTF que corre en loop permanente. E solo anima el disco de hielo.

**Shaders necesarios (4):**
1. Principal: Blinn-Phong + Fresnel + niebla exponencial integrada
2. Agua: vertex displacement `y += A*sin(freq*x+time)` + Fresnel en fragmento
3. Skybox: cubemap, `gl_Position = pos.xyww`
4. Unlit: letreros, billboards, partículas

**Efectos del entorno:**
- Skybox: cubemap estático 6 PNG (ártico + aurora boreal)
- Niebla exponencial: `fragColor = mix(fogColor, sceneColor, exp(-density*dist))`
- Nieve: 80-120 billboards, actualización CPU→GPU por frame (VBO dinámico)
- Fauna estática: foca, zorro, gaviota billboard (sin interacción)

**UI/Audio:**
- HUD completamente vacío durante el recorrido (solo `[E] Explorar` Dear ImGui en trigger)
- Letreros 3D fijos (planos 2×1.2m, textura stb_truetype, unlit, orientación fija)
- Pantalla de título: foto ártica fullscreen + ImGui (Enter para comenzar, fade)
- Pantalla de cierre: Escape desde M5 COMPLETADO → texto de reflexión + créditos
- Audio: OpenAL, 1 ambient OGG loop + 1 WAV activación + 3 fuentes 3D posicionales

**Assets externos:**
- 7 modelos GLTF CC0 de Sketchfab
- Texturas CC0 de ambientcg.com
- Skybox HDR de polyhaven.com → convertir con cmgen a 6 PNG
- Fuente Roboto-Regular.ttf (Google Fonts)
- 5 audios WAV/OGG de Freesound.org

## Decisiones de diseño ya tomadas (no cambiar)

- No SLERP — LERP de euler para todas las rotaciones
- Heightmap del suelo: estático (calculado al inicio, no animado)
- Reset de animaciones: instantáneo al salir del trigger
- Un solo módulo activo a la vez
- La pantalla de título no precarga el escenario 3D — carga al presionar Enter
- No hay música de fondo
- No hay colisión con la geometría — solo AABB position clamp para límites

## Tarea

Dame el plan de implementación más robusto y directamente aplicable para este proyecto. Responde:

**1. Orden óptimo de desarrollo**
Fases de desarrollo que minimicen retrabajos y bloqueos. Para cada fase: nombre, objetivo, y criterio de salida concreto.

**2. Prerequisitos antes de shaders avanzados**
Checklist de lo que debe funcionar con OpenGL básico antes de implementar Blinn-Phong, Fresnel y el shader de agua.

**3. Estructura del código fuente**
Árbol de `src/` con clases principales, sus responsabilidades y métodos clave. Enfocado en:
- Sistema de módulos con patrón compartido
- Gestión de shaders
- Gestión de assets (cargar una vez, reutilizar)
- Bucle principal con AppState

**4. Riesgos técnicos y mitigaciones**
Los 5 riesgos más probables con mitigación concreta. Prioriza: Assimp+GLTF con animaciones, stb_truetype+Roboto, ImGui coexistiendo con pipeline OpenGL, OpenAL en Windows VS2022, CMake FetchContent.

**5. Criterios de salida por fase**
Uno por fase, concreto y verificable.

## Restricciones

- Stack fijo — no sugerir alternativas de bibliotecas
- Contexto académico: sin over-engineering, que funcione sobre que sea perfecto
- App de escritorio Windows monolítica (no web, no Docker, no CI/CD)
- Las clases deben ser simples de implementar por una IA (Claude Code)
- No incluir código fuente completo — solo estructuras, pseudocódigo o firmas de métodos

## Formato de respuesta

### 1. Fases de desarrollo
Tabla: Fase | Nombre | Objetivo | Criterio de salida

### 2. Prerequisitos antes de shaders avanzados
Checklist con ítems concretos

### 3. Estructura de código src/
Árbol de directorios + descripción de cada clase (responsabilidad + métodos principales)

### 4. Riesgos y mitigaciones
Tabla: # | Riesgo | Probabilidad | Mitigación concreta

### 5. Recomendaciones adicionales
Cualquier insight no cubierto por las preguntas anteriores, específico para este proyecto.

Sé directo. Cada recomendación debe ser accionable con el stack dado.

## Instrucción de output

Proporciona tu reporte completo directamente en tu respuesta, formateado en Markdown.
El usuario lo copiará al archivo correspondiente del proyecto.
