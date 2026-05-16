---
title: Especificaciones del Proyecto Final — CGEIHC 2026-2
date: 2026-05-15
category: technology
tags: [opengl, proyecto-final, rubrica, entregables, especificaciones]
status: complete
---

# Especificaciones del Proyecto Final — CGEIHC 2026-2

**Materia**: Computación Gráfica e Interacción Humano-Computadora
**Institución**: UNAM, Facultad de Ingeniería
**Docente**: Dr. Sergio Teodoro Vite
**Grupo**: Teoría 2
**Semestre**: 2026-2

## Equipo

| Nombre | Cuenta |
|--------|--------|
| Hernández Cabañas Jesús | 422062224 |
| Navarro Nuño Juan Pablo | 319086779 |
| Macias Niño Carmen Violeta | 318285450 |
| Martinez Jimenez Israel | 312032184 |

## Objetivo General

El alumno demuestra, mediante un problema práctico, la aplicación de los conceptos de computación gráfica para generar un sistema gráfico de calidad con análisis y diseño rigurosos.

## Requisitos Técnicos Obligatorios

- **API gráfica**: OpenGL 3.x o superior con GLSL
- **Tipo de sistema**: Interactivo, 3D, tiempo real
- **Iluminación**: Al menos **2 técnicas**:
  - Phong (materiales opacos)
  - Fresnel (materiales translúcidos y metálicos)
- **Texturizado de ambiente**: Al menos **1 técnica**:
  - Cube mapping, environment mapping o sphere mapping
- **Animaciones** — **3 tipos obligatorios**:
  1. Básica (transformaciones directas)
  2. Basada en keyframes (LERP/SLERP entre estados)
  3. Procedural (generativa, sin definir cada fotograma)
- **Pipeline gráfico completo**: Modelado geométrico → color → texturizado → iluminación → sombreado → animación → simulación → interacción → renderizado

## Entregables y Pesos

| Entregable | Porcentaje |
|------------|------------|
| Software funcional ejecutable | 50% |
| Reporte en español | 15% |
| Reporte en inglés | 15% |
| Video demostrativo | 10% |
| Presentación oral y digital | 10% |

## Rúbrica del Ejecutable (detalle)

| Criterio | Puntos |
|----------|--------|
| Empaquetado en setup.exe (InstallForge) | Obligatorio |
| Manual PDF con instrucciones claras | 0.5 |
| Instalación correcta | 0.5 |
| Ejecución sin errores | 0.5 |
| Funcional y coherente con manual | 0.5 |
| Recorrido guiado narrativo con punto inicial y final | **4.0** |
| Interacciones contextuales con la temática | **3.0** |
| Fluidez del programa (fps adecuados) | 1.0 |
| **Total** | **10.0** |

## Rúbrica del Reporte

| Criterio | Puntos |
|----------|--------|
| Portada formal | Obligatorio |
| Resumen con palabras clave | Obligatorio |
| Referencias en formato APA | Obligatorio |
| Estado del arte en introducción (análisis económico, social y ético) | 1.0 |
| Metodología con algoritmos, diagramas de arquitectura, segmentos de código | 3.0 |
| Experimentos (con figuras tituladas y referenciadas) | 3.0 |
| Resultados y discusión comparativa | 1.0 |
| Conclusiones individuales | 1.0 |
| Video demostrativo enlazado | 1.0 |
| **Total** | **10.0** |

### Estructura del Reporte

- Portada: título, fecha, autores (apellido paterno primero)
- Hoja de evidencias con número(s) de cuenta
- Resumen (1 párrafo)
- Introducción con citas APA
- Metodología práctica
- Experimentos
- Resultados
- Conclusiones individuales (por integrante)
- Referencias APA
- Enlace a video demostrativo (3-5 minutos)

**Formato**: 20-35 páginas, fuente 12pt, interlineado 1.5, PDF

## Rúbrica del Video

| Criterio | Puntos |
|----------|--------|
| Duración de 3 a 5 minutos | 0.5 |
| Formato MP4 (H.264) | 0.5 |
| Buena calidad de video | 1.0 |
| Nombre del proyecto e integrantes | 0.5 |
| Narración de la idea y características principales | **6.0** |
| Tomas del software funcionando | 1.0 |
| Textos descriptivos de características | 0.5 |
| **Total** | **10.0** |

## Metodología de Desarrollo

Basada en prototipos (adaptación OpenAI, Agosto 2024):

1. **Recolección de requisitos y planificación**
2. **Diseño conceptual** (bocetos, wireframes, flujo de interacción)
3. **Prototipo inicial funcional** + pruebas internas
4. **Validación con usuarios** + recolección de retroalimentación
5. **Iteración y mejoras** (ciclo continuo)
6. **Implementación completa** + pruebas exhaustivas
7. **Validación final y entrega**

## Entrega Final

- Software `.exe` empaquetado con **InstallForge**
- `README.txt` con instrucciones de instalación y uso
- **NO se entrega código fuente** (se revisa en sesiones de avances)
- Video en **MP4 / H.264**, editado con DaVinci Resolve (recomendado)
- Todo subido a Google Drive (enlace en Moodle)

## Recursos del Curso

- Plataforma: [cursos.computer-graphics.com.mx](https://cursos.computer-graphics.com.mx/)
- Referencia OpenGL: [learnopengl.com](https://learnopengl.com/)
- Software de modelado: [blender.org](https://www.blender.org/)
