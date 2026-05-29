# Índice de Conocimiento — CGEIHC-P

> Proyecto: Calentamiento global en el polo norte
> Última actualización: 2026-05-15
> Consultar: `/consultar-conocimiento [tema]`

---

## Categorías

| Carpeta | Contenido |
|---------|-----------|
| `technology/` | Especificaciones técnicas, idea, storyboard, materiales del curso, stack |

---

## Documentos

| ID | Ruta | Título | Fecha | Descripción |
|----|------|--------|-------|-------------|
| T-001 | `technology/especificaciones-proyecto.md` | Especificaciones del Proyecto Final CGEIHC 2026-2 | 2026-05-15 | Rúbricas completas, entregables, requisitos técnicos (OpenGL 3+, 2 iluminaciones, 3 tipos de animación), metodología de desarrollo por prototipos |
| T-002 | `technology/idea-bocetos-storyboard.md` | Idea, Boceto y Storyboard — Calentamiento Global en el Polo Norte | 2026-05-06 | Concepto del museo virtual, dos rutas narrativas (problemática/sostenible), boceto estructural, lista completa de modelos 3D, animaciones, materiales y efectos |
| T-003 | `technology/materiales-curso.md` | Materiales del Curso CGEIHC 2026-2 | 2026-05-15 | Syllabus completo (8 temas), bibliografía oficial, shaders de ejemplo del profesor (ondas + Fresnel), evaluación del curso |
| T-004 | `../../INFRAESTRUCTURA.md` | Infraestructura y Stack Técnico | 2026-05-15 | Stack completo: OpenGL/GLAD/GLFW/GLM/Assimp/ImGui/OpenAL, pipeline de assets, herramientas de desarrollo autónomo con Claude Code, arquitectura del motor, optimizaciones |
| T-005 | `technology/fuentes-datos-museo.md` | Investigación de Fuentes — Datos Científicos del Museo | 2026-05-15 | Hallazgos y correcciones de investigación web: hielo marino (NSIDC/PIOMAS), osos polares (IUCN), nivel del mar (USGS/NOAA/IPCC), renovables (IEA), transporte (IEA), sumideros (GCP), acuerdos (UNEP). URLs de referencia rápida. |
| T-006 | `technology/proyectos-referencia.md` | Proyectos de Referencia — Base de Código OpenGL C++ | 2026-05-16 | Catálogo de proyectos y fragmentos de código verificados por categoría: iluminación, agua, skybox, partículas, animaciones LERP, Assimp, ImGui, stb_truetype, miniaudio, museo FPS. Se llena en Fase 0B y se enriquece durante el desarrollo. |
| T-007 | `technology/auditoria-requisitos.md` | Auditoría de Cumplimiento de Requisitos | 2026-05-28 | Evaluación exhaustiva ámbito por ámbito: iluminación (5 técnicas), env mapping (2), animaciones (3 tipos), recorrido narrativo, interacciones, texturas, audio, UI, FPS. Calificación estimada: 92-95%. Pendientes: instalador, video, presentación oral. |

---

## Cómo Agregar Documentos

1. Crear el archivo en la carpeta de categoría correcta con frontmatter YAML
2. Actualizar esta tabla (ID, ruta, título, fecha, descripción)
3. El ID sigue el formato `{inicial-categoría}-{número}`
