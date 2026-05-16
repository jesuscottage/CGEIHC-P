# CLAUDE.md — CGEIHC-P

> **Proyecto**: Calentamiento global en el polo norte
> **Materia**: Computación Gráfica e Interacción Humano-Computadora (CGEIHC)
> **Institución**: UNAM, Facultad de Ingeniería — Semestre 2026-2
> **Docente**: Dr. Sergio Teodoro Vite
> **Idioma principal**: Español (ver `.claude/rules/idioma.md`)

---

## Contexto del Proyecto

Sistema gráfico interactivo 3D que simula un **museo virtual educativo en el Polo Norte** sobre el calentamiento global. El usuario recorre dos caminos narrativos desde una zona de bienvenida hasta una zona de reflexión final.

**Stack técnico**: C++ + OpenGL 3.3 Core Profile + GLFW + GLAD + GLM + Assimp + stb_image + Dear ImGui + OpenAL + CMake

Ver `INFRAESTRUCTURA.md` para el stack completo y `PENDIENTES.md` para el estado actual y próximos pasos.

---

## Equipo

| Nombre | Cuenta |
|--------|--------|
| Hernández Cabañas Jesús | 422062224 |
| Navarro Nuño Juan Pablo | 319086779 |
| Macias Niño Carmen Violeta | 318285450 |
| Martinez Jimenez Israel | 312032184 |

---

## Estructura del Repositorio

```
CGEIHC-P/
├── CLAUDE.md               # Este archivo — instrucciones para Claude
├── README.md               # Descripción del proyecto
├── PENDIENTES.md           # Estado actual y próximos pasos (SIEMPRE revisar)
├── INFRAESTRUCTURA.md      # Stack técnico completo y configuración del entorno
├── .claude/
│   ├── rules/              # Reglas obligatorias (idioma, organización, calidad, seguridad)
│   ├── agents/             # Agentes (investigador)
│   ├── skills/             # Skills disponibles
│   ├── commands/           # /commit y otros comandos
│   └── context/
│       └── reminders.md    # Contexto auto-inyectado al inicio de sesión
├── docs/
│   ├── knowledge/          # Base de conocimiento
│   │   ├── INDEX.md        # OBLIGATORIO actualizar al agregar docs
│   │   └── technology/     # Especificaciones, idea, materiales del curso, stack
│   └── plans/              # Planes de acción con checkboxes
├── src/                    # Código fuente C++ (a crear en Fase 1)
├── shaders/                # Shaders GLSL (a crear en Fase 2+)
├── assets/                 # Modelos 3D, texturas, audio (a poblar)
├── scripts/                # Scripts de automatización (build, screenshot, export)
└── active/                 # Artefactos temporales (gitignored)
```

---

## Documentos Críticos (Leer Siempre)

| Archivo | Contenido |
|---------|-----------|
| `PENDIENTES.md` | Estado actual, prioridad inmediata, próximos pasos |
| `INFRAESTRUCTURA.md` | Stack completo, arquitectura del motor, pipeline de assets |
| `docs/knowledge/INDEX.md` | Índice de toda la base de conocimiento |
| `docs/knowledge/technology/especificaciones-proyecto.md` | Rúbricas, entregables, requisitos técnicos |
| `docs/knowledge/technology/idea-bocetos-storyboard.md` | Concepto narrativo, modelos, animaciones, materiales |
| `docs/knowledge/technology/materiales-curso.md` | Syllabus, bibliografía, shaders de ejemplo del profesor |

---

## Requisitos Técnicos Obligatorios (Rúbrica)

1. **OpenGL 3.3+ Core Profile** con GLSL
2. **Al menos 2 técnicas de iluminación**: Blinn-Phong + Fresnel
3. **Al menos 1 técnica de environment mapping**: cubemap/skybox + reflections
4. **3 tipos de animación**:
   - Básica (transformaciones directas)
   - Keyframe (LERP/SLERP)
   - Procedural (generativa, sin fotogramas manuales)
5. **Recorrido guiado narrativo** con punto inicial y final definidos
6. **Interacciones** contextuales con la temática
7. **Fluidez** adecuada (fps estables)

---

## Reglas del Proyecto

| Regla | Archivo | Descripción |
|-------|---------|-------------|
| Organización | `.claude/rules/organizacion.md` | Estructura de directorios obligatoria |
| Idioma | `.claude/rules/idioma.md` | Todo en español con acentos correctos |
| Seguridad | `.claude/rules/seguridad.md` | Sin secrets hardcodeados |
| Calidad | `.claude/rules/calidad.md` | Convenciones de código, no over-engineering |

---

## Skills Útiles para Este Proyecto

| Skill | Cuándo usarla |
|-------|--------------|
| `/generar-informe` | Reporte de estado del proyecto |
| `/agent-review` | Revisar código OpenGL/GLSL implementado |
| `/prompt-contract` | Antes de implementar cualquier módulo no trivial |
| `/consultar-conocimiento` | Consultar especificaciones, idea, materiales del curso |
| `/corregir-ortografia` | Antes de entregar reportes en markdown |
| `/revision-sistematica` | Investigación del estado del arte para el reporte |
| `/consultar-gemini` | Consultas de arquitectura o decisiones técnicas difíciles |
| `/diagram-generator` | Generar diagramas de arquitectura del sistema |

---

## Workflow de Desarrollo Autónomo (Claude Code)

Para que Claude pueda trabajar de forma autónoma con retroalimentación visual:

```
1. Claude modifica código C++/GLSL
2. Bash: cmake --build build && .\build\Debug\CGEIHC.exe (en background)
3. Bash: powershell -File scripts/screenshot.ps1 (capturar pantalla)
4. Read: active/screenshot.png (Claude ve el resultado 3D)
5. Iterar basándose en el resultado visual
```

Scripts disponibles en `scripts/`:
- `build-and-run.ps1` — Build con CMake y ejecutar
- `screenshot.ps1` — Capturar screenshot de la ventana OpenGL

---

## Gestión de Conocimiento

- **Investigación y hallazgos**: `docs/knowledge/` con índice en `INDEX.md`
- **Subcategoría activa**: `technology/`
- **Consultar**: `/consultar-conocimiento [tema]`
- **Siempre actualizar INDEX.md** al agregar o eliminar documentos

## Planes

- **Planes de acción**: `docs/plans/` con checkboxes
- **Estado actual**: `PENDIENTES.md` en la raíz (referencia rápida)

---

## Contexto del Curso

- **Shaders de referencia**: `13_wavesAnimation-fresnel.vs/.fs` (del profesor, usar como base para el océano)
- **Referencia principal**: [learnopengl.com](https://learnopengl.com/)
- **Plataforma del curso**: [cursos.computer-graphics.com.mx](https://cursos.computer-graphics.com.mx/)
- **Bibliografía**: Hearn & Baker (2006), Angel & Shreiner (2011), de Vries (2020)
