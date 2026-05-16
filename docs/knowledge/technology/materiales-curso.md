---
title: Materiales del Curso — CGEIHC 2026-2
date: 2026-05-15
category: technology
tags: [curso, opengl, computacion-grafica, teoria, temas, bibliografia]
status: complete
---

# Materiales del Curso — CGEIHC 2026-2

**Materia**: Computer Graphics and Human-Computer Interaction
**Profesor**: PhD Sergio Teodoro-Vite
**Institución**: UNAM, Facultad de Ingeniería
**Fecha de presentación**: 24 enero 2025

---

## Descripción del Curso

Curso sobre conceptos fundamentales de computación gráfica en 2D y 3D. Tópicos clave: pipeline gráfico, modelado geométrico, posicionamiento de cámara, técnicas de iluminación, animación e interacción. Los estudiantes desarrollan aplicaciones gráficas interactivas en tiempo real usando software y hardware especializado.

**Objetivo**: Integrar técnicas fundamentales de computación gráfica para explorar áreas de aplicación, incluyendo diseño de interfaces para interacción humano-computadora.

---

## Temas del Curso (Syllabus)

1. Introduction to Computer Graphics (Part I & II)
2. Introduction to 2D Graphics (Part I & II)
3. Graphics Pipeline (Part I & II)
4. Geometric Modelling (2D y 3D)
5. Color Theory and Lighting Models
6. Texturing
7. Animation Principles
8. Interaction Techniques and User Interfaces
9. Human-Computer Interaction
10. Simulación (incluye shader de ondas con Fresnel)

---

## Archivos de Materiales Disponibles

| Archivo | Tema |
|---------|------|
| `00 - Course Presentation.pdf` | Presentación general del curso |
| `01 - Introduction to Computer Graphics Part I.pdf` | Introducción CG parte 1 |
| `01 - Introduction to Computer Graphics Part II.pdf` | Introducción CG parte 2 |
| `02 - Introduction to 2D Graphics Part I.pdf` | Gráficos 2D parte 1 |
| `02 - Introduction to 2D Graphics Part II.pdf` | Gráficos 2D parte 2 |
| `03 - Graphics Pipeline Part I.pdf` | Pipeline gráfico parte 1 |
| `03 - Graphics Pipeline Part II.pdf` | Pipeline gráfico parte 2 |
| `04 - Geometric Modelling.pdf` | Modelado geométrico |
| `05_Modelos de color e Iluminación.pdf` | Color e iluminación (Blinn-Phong, Fresnel) |
| `06_Texturizado.pdf` | Texturizado (UV, normal maps, environment mapping) |
| `07_Principios de Animación.pdf` | Animación (keyframes, LERP, SLERP, procedural) |
| `08_Simulación.pdf` | Simulación física |
| `09_Human Computer Interaction.pdf` | HCI e interfaces |
| `13_wavesAnimation-fresnel.fs` | Shader de fragmento: animación de olas + Fresnel |
| `13_wavesAnimation-fresnel.vs` | Shader de vértice: animación de olas + Fresnel |

> **Nota**: Los materiales están en `C:\Users\herna\Downloads\drive-download-20260516T033212Z-3-001\`
> Para consultar un tema específico, leer el PDF correspondiente.

---

## Bibliografía Oficial

1. Hearn, D., and Baker, M. P. (2006). *Computer Graphics with OpenGL 3/E*. Pearson Educación.
2. Angel, E., and Shreiner, D. (2011). *Interactive Computer Graphics: A Top-Down Approach with Shader-Based OpenGL*. Pearson Education.
3. ANGEL, Edward. *Interactive Computer Graphics: A Top-Down Approach with OpenGL*, Third edition. Boston Addison-Wesley, 2002.
4. de Vries, J. (2020). *Learn OpenGL: Learn Modern OpenGL Graphics Programming in a Step-by-step Fashion*. Kendall and Welling.

---

## Plataforma y Recursos

- **Moodle del curso**: [cursos.computer-graphics.com.mx](https://cursos.computer-graphics.com.mx/)
- **Referencia OpenGL**: [learnopengl.com](https://learnopengl.com/)
- **Blender** (modelado oficial del curso): [blender.org](https://www.blender.org/)

---

## Evaluación del Curso

| Componente | Porcentaje |
|------------|------------|
| Primer parcial | 20% (50% examen + 50% pre-propuesta proyecto) |
| Segundo parcial | 20% (50% examen + 50% propuesta proyecto) |
| Asistencia | 10% |
| Laboratorio | 30% |
| Proyecto Final | 20% |

---

## Shaders de Ejemplo del Curso

El curso incluye shaders de referencia para animación de olas con efecto Fresnel:
- `13_wavesAnimation-fresnel.vs` — Vertex shader (desplazamiento de vértices, ondas)
- `13_wavesAnimation-fresnel.fs` — Fragment shader (Fresnel, reflejos, transparencia)

Estos son **directamente aplicables** al océano del proyecto (simulación del agua del Ártico).

---

## Perfil del Profesor

PhD Sergio Teodoro-Vite:
- Ingeniero en Computación (FI, UNAM)
- Maestría y Doctorado en Ciencias e Ingeniería de la Computación (IIMAS, UNAM)
- Líneas de investigación: sistemas interactivos, realidad virtual, realidad aumentada, simulación física, metaverso y serious games
- Campo principal: simulación en salud
