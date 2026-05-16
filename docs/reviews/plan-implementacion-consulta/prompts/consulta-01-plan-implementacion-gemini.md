# Plan de Implementación Óptimo — Museo Virtual 3D C++/OpenGL

## Tu rol

Eres un **arquitecto de software senior especializado en gráficos por computadora en tiempo real** con amplia experiencia en **proyectos OpenGL/C++ académicos y profesionales, pipeline de renderizado, sistemas de animación y gestión de assets 3D**.

## Proyecto

**CGEIHC-P** — Museo virtual educativo 3D sobre calentamiento global en el Polo Norte — proyecto final universitario C++/OpenGL, UNAM Facultad de Ingeniería, CGEIHC 2026-2.

## Contexto

Tenemos 8 blueprints de diseño completamente definidos y necesitamos el plan de implementación más robusto posible. El stack es fijo y no negociable. El ejecutable se entrega en Windows como `.exe`.

## Archivos de referencia

Antes de responder, investiga a fondo los siguientes archivos del proyecto. Lee todo lo que necesites para dar una respuesta bien fundamentada:

**Diseño conceptual completo (blueprints):**
- `blueprints/01-vision-narrativa.md` — contenido del museo, 7 módulos, textos, formato de letreros
- `blueprints/02-layout-escenario.md` — layout T invertida, dimensiones, plataformas, terreno, límites
- `blueprints/03-catalogo-modelos.md` — modelos 3D por módulo, animaciones LERP, fauna, letreros, skybox
- `blueprints/04-interaccion-controles.md` — WASD, cámara FPS, trigger de proximidad, estados de módulo, pantallas
- `blueprints/05-materiales-texturas.md` — materiales PBR, niebla, partículas de nieve, shader de agua, letreros unlit
- `blueprints/06-animaciones.md` — clasificación de 3 tipos (rúbrica), tabla LERP, procedurales, reset
- `blueprints/07-audio.md` — OpenAL, ambient OGG, activación WAV, 3 módulos con audio 3D posicional
- `blueprints/08-hud-ui.md` — HUD vacío, indicador E, pantalla de título con foto, pantalla de cierre

**Stack y requisitos técnicos:**
- `INFRAESTRUCTURA.md` — stack completo, arquitectura del motor, pipeline de assets
- `CLAUDE.md` — requisitos de rúbrica, estructura del proyecto
- `docs/knowledge/technology/especificaciones-proyecto.md` — rúbricas y entregables del curso

Si consideras que hay otros archivos relevantes, investígalos también.

## Tarea

Dame el plan de implementación más robusto y directamente aplicable para este proyecto. Responde:

**1. Orden óptimo de desarrollo**
Fases de desarrollo que minimicen retrabajos y bloqueos. Para cada fase: nombre, objetivo, y criterio de salida concreto y verificable (qué debe verse/funcionar para considerarla lista).

**2. Prerequisitos antes de shaders avanzados**
Checklist explícito de lo que debe estar funcionando con OpenGL básico antes de implementar Blinn-Phong, Fresnel y el shader de agua.

**3. Estructura del código fuente**
Árbol de `src/` con clases principales, sus responsabilidades y métodos clave. Considera:
- Motor mínimo (no un engine completo)
- Sistema de módulos con patrón compartido: `trigger → estado → LERP → reset`
- `ModuleBase` o equivalente sin over-engineering
- Gestión de shaders (compilar, cachear uniforms)
- Gestión de assets (cargar una vez, reutilizar por ID)
- Bucle principal con `AppState { TITULO, JUGANDO, CIERRE, SALIR }`

**4. Riesgos técnicos y mitigaciones**
Los 5 riesgos más probables para este stack/proyecto. Considera especialmente:
- Assimp cargando GLTF con clips de animación (oso polar requiere idle en loop)
- stb_truetype rasterizando Roboto TTF al inicio
- Dear ImGui coexistiendo con el pipeline OpenGL en el mismo loop de render
- OpenAL Soft en Windows con Visual Studio 2022
- CMake FetchContent en entorno universitario (posibles problemas de red/proxy)

**5. Criterios de salida por fase**
Para cada fase propuesta, un criterio de salida concreto. Formato:
> "Fase N está lista cuando: el ejecutable compila sin warnings en VS2022, [criterio específico], y [criterio específico]."

## Restricciones

- Stack fijo — no sugerir alternativas de bibliotecas
- Contexto académico: sin over-engineering, que funcione sobre que sea perfecto
- App de escritorio Windows monolítica (no web, no Docker, no CI/CD)
- El código lo escribe Claude Code (IA) — las clases deben ser simples de implementar
- No hay división de trabajo por persona — toda implementación la hace Claude Code

## Formato de respuesta

### 1. Fases de desarrollo
Tabla: Fase | Nombre | Objetivo | Criterio de salida

### 2. Prerequisitos antes de shaders avanzados
Checklist con ítems concretos y verificables

### 3. Estructura de código src/
Árbol de directorios + para cada clase: responsabilidad en 1-2 líneas + métodos principales

### 4. Riesgos y mitigaciones
Tabla: # | Riesgo | Probabilidad (Alta/Media/Baja) | Mitigación concreta

### 5. Recomendaciones adicionales
Cualquier insight relevante no cubierto por las preguntas anteriores, específico para este proyecto.

Sé directo. Cada recomendación debe ser accionable con el stack dado.

## Instrucción de output

**IMPORTANTE**: Deposita tu respuesta completa directamente en el siguiente archivo del proyecto:

```
docs/reviews/plan-implementacion-consulta/reportes/consulta-01-plan-implementacion-gemini.md
```

No incluyas en tu reporte el contenido de esta instrucción ni la lista de archivos que revisaste.
