# Reporte 01 — Síntesis de Auditoría del Plan Maestro de Desarrollo

> Fecha: 2026-05-16
> Auditores externos: Gemini (Antigravity) + ChatGPT (GPT-4o)
> Sintetizado por: Claude Code
> Documento auditado: `docs/plans/plan-maestro-desarrollo.md`

---

## Resumen ejecutivo

El plan maestro fue evaluado favorablemente por ambos auditores: arquitectura correcta, stack bien dimensionado para un proyecto académico, y sin sobre-ingeniería. Las correcciones necesarias fueron de **operabilidad autónoma** (cómo Claude verifica resultados sin intervención humana), no de rediseño técnico.

Veredicto global: **ajustes menores aplicados** — el plan ya está listo para implementación.

---

## Hallazgos genuinos aplicados

| # | Hallazgo | Fuente | Acción aplicada |
|---|----------|--------|-----------------|
| G1 | Claude no puede emitir eventos de teclado en runtime; las fases 5 y 10 son inverificables sin intervención humana | Gemini + ChatGPT | Añadido **Test Mode CLI** (`--test-module M1_IZQ`) con mapa de posiciones de los 7 módulos; la app posiciona cámara y simula tecla E al frame 60 |
| G2 | Un screenshot al frame 120 (≈2 s) no captura animaciones de 5-12 s (M3 izq tarda 12 s) | Gemini + ChatGPT | Reemplazado por **multi-screenshot** en frames 60/300/600; añadido **JSON de estado** (`active/state.json`) con FPS, cámara, módulo activo y `moduleT` |
| G3 | Fase 7 demasiado cargada: Skybox + Niebla + Agua + Fresnel + pipeline de transparencias en una sola fase genera debugging cruzado imposible de aislar | Gemini + ChatGPT | Fase 7 dividida en **Fase 7A** (Skybox + Niebla + pipeline de transparencias) y **Fase 7B** (Agua procedural + Fresnel); 7B no comienza hasta que 7A tenga criterios de salida verificados |
| G4 | CWD en VS2022 puede no ser la raíz del proyecto; rutas relativas `assets/...` fallan cuando el exe se ejecuta desde `build/Debug/` | Gemini | Añadidos **CMake POST_BUILD commands** para copiar `assets/`, `shaders/` y crear `active/` junto al `.exe` |
| G5 | stb_truetype solo rasteriza ASCII 32-127 por defecto; los letreros en español (á é í ó ú ñ ¿ ¡) producirán cuadros vacíos | ChatGPT | Añadida configuración de **rangos de glifos Latin-1** (192-255) mediante `stbtt_PackFontRanges` en tarea 9.5 del plan |
| G6 | Sin overlay de debug, Claude debe inferir estado numérico solo del PNG — muy impreciso para verificar `moduleT`, FPS o posición de cámara | Gemini + ChatGPT | Añadida tarea **2.5 ImGui debug overlay** desde Fase 2: muestra FPS, `camPos`, módulo activo y `moduleT` en cada screenshot |
| G7 | Criterios de salida ambiguos: "specular visible en ángulos oblicuos", "museo navegable", "build estable 15+ min" no son verificables objetivamente por un agente | ChatGPT | Reemplazados por métricas observables: specular con shininess=32 a ~45°; AABB sin atravesar paredes; 15+ min sin crecimiento de recursos OpenGL (conteo RenderDoc entre frame 60 y 3600) |
| G8 | Falta validación `AI_SCENE_FLAGS_INCOMPLETE` de Assimp; falta política explícita de coordenadas Y-up; falta restauración de estado OpenGL al inicio del frame | ChatGPT + Gemini | Añadidos a Reglas Globales (reglas 8-10): convención de coordenadas, estructura de render loop inmutable, y restauración de estado `glEnable(GL_DEPTH_TEST/CULL_FACE)` + `glDepthMask(GL_TRUE)` |
| G9 | Sin fallback visual en Texture::loadFromFile(), un asset faltante produce negro sólido — idéntico a un error de iluminación | ChatGPT | Añadido **magenta checkerboard 8×8** como textura de fallback; permite distinguir inmediatamente "asset no encontrado" de "iluminación incorrecta" |

---

## Falsos positivos descartados

| # | Hallazgo | Fuente | Por qué es falso positivo |
|---|----------|--------|---------------------------|
| FP1 | "Mover ResourceManager a Fase 2" | ChatGPT | El plan ya incluye caché básica en Shader.h desde Fase 2. ResourceManager completo en Fase 3 es correcto: los shaders no requieren caché de modelos todavía. |
| FP2 | "Fase 9 demasiado cargada, separar UI/texto y audio en Fase 10" | ChatGPT | El stack (Dear ImGui + stb_truetype + miniaudio) está correctamente acotado. Las tres bibliotecas son header-only o tienen inicialización trivial. Separar artificialmente añade una fase sin reducir complejidad real. |
| FP3 | "Simplificar CO₂ espiral procedural" | ChatGPT | La espiral es una animación procedural simple (seno en XZ). No es más compleja que las partículas de nieve ya contempladas. No hay riesgo de sobredimensionamiento. |
| FP4 | "Añadir modo wireframe automático como screenshot adicional" | ChatGPT | Ya se contempla en la tabla de verificaciones del plan: "Activar wireframe `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)` → screenshot". No es necesario automatizarlo en el pipeline base. |
| FP5 | "Definir render pipeline con capas desde Fase 2" | ChatGPT | Aplicado como Regla Global #9 (estructura de render loop inmutable desde Fase 2), que es más efectivo que añadir una tarea específica de refactoring en Fase 2. |

---

## Estado de correcciones

Todas las correcciones G1-G9 han sido aplicadas al plan maestro:

- [x] G1 — Test Mode CLI
- [x] G2 — Multi-screenshot (frames 60/300/600) + JSON de estado
- [x] G3 — División de Fase 7 en 7A y 7B
- [x] G4 — CMake POST_BUILD para assets y shaders
- [x] G5 — stb_truetype rangos de glifos español (Latin-1)
- [x] G6 — ImGui debug overlay desde Fase 2
- [x] G7 — Criterios de salida observables en Fases 2, 4, 6, 11
- [x] G8 — Reglas Globales 8-10 + validación Assimp + tabla de correcciones de coordenadas
- [x] G9 — Textura fallback magenta checkerboard

El plan está listo para comenzar la implementación por fases.
