# Recordatorios de Contexto — CGEIHC-P

> Auto-cargado al inicio de cada sesión.
> Última actualización: 2026-05-15

---

## Proyecto

**Nombre**: Calentamiento global en el polo norte
**Tipo**: Proyecto final de Computación Gráfica e Interacción Humano-Computadora (CGEIHC)
**Institución**: UNAM, Facultad de Ingeniería — Semestre 2026-2
**Docente**: Dr. Sergio Teodoro Vite
**Stack**: C++17 + OpenGL 3.3 Core + GLFW + GLAD + GLM + Assimp + stb_image + ImGui + OpenAL + CMake

## Lo Primero que Debes Hacer en Cada Sesión

1. Leer `PENDIENTES.md` — estado actual y prioridad inmediata
2. Leer `INFRAESTRUCTURA.md` si vas a trabajar en código o assets
3. Consultar `docs/knowledge/INDEX.md` para navegar la base de conocimiento

## Archivos Críticos

| Archivo | Propósito |
|---------|-----------|
| `PENDIENTES.md` | Estado actual, tareas prioritarias, pendientes por fase |
| `INFRAESTRUCTURA.md` | Stack completo, arquitectura del motor, pipeline de assets |
| `docs/knowledge/technology/especificaciones-proyecto.md` | Rúbricas y requisitos técnicos obligatorios |
| `docs/knowledge/technology/idea-bocetos-storyboard.md` | Concepto narrativo, modelos, animaciones requeridas |
| `docs/knowledge/technology/materiales-curso.md` | Syllabus, shaders del profesor, bibliografía |

## Requisitos Técnicos Obligatorios (Rúbrica)

- OpenGL 3.3+ Core Profile + GLSL
- 2+ técnicas de iluminación: **Blinn-Phong** (opaco) + **Fresnel** (hielo, agua)
- 1+ environment mapping: cubemap skybox + reflejos
- 3 tipos de animación: **básica** + **keyframe (LERP/SLERP)** + **procedural**
- Recorrido narrativo con punto inicial y final definidos
- Interacciones contextuales con la temática

## Reglas (resumen)

- **Organización** (`organizacion.md`): estructura de directorios obligatoria
- **Idioma** (`idioma.md`): todo en español, acentos siempre correctos
- **Calidad** (`calidad.md`): no over-engineering, solo lo pedido

## Workflow Autónomo (Claude + OpenGL)

```
Modificar código → cmake --build build → ejecutar .exe →
screenshot.ps1 → leer active/screenshot.png → iterar
```

## Skills Más Útiles

| Skill | Cuándo |
|-------|--------|
| `/prompt-contract` | Antes de implementar cualquier módulo |
| `/agent-review` | Al terminar una implementación no trivial |
| `/consultar-conocimiento` | Para revisar specs, idea o materiales del curso |
| `/generar-informe` | Reportes de avance para el equipo o sesiones |
| `/corregir-ortografia` | Antes de entregas de reportes |

## Fuentes de Texturas CC0

- ambientcg.com, polyhaven.com, 3dtextures.me, texturelabs.org, 3dtextures.me

## Referencia Rápida del Profesor

- Shaders ejemplo: `13_wavesAnimation-fresnel.vs/.fs` (usar como base para océano)
- Referencia OpenGL: learnopengl.com
- Modelado: blender.org
