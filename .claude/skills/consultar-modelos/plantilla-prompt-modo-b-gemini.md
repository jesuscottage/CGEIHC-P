# Plantilla de Prompt — Modo B: Gemini (gemini.google.com, contexto completo)

> **Uso**: Prompt para Gemini vía Antigravity. Tiene acceso completo al proyecto.
> Se le pasan paths de archivos para que los investigue directamente — no se embebe contenido.
> Se le indica que deposite su reporte directamente en el archivo de salida.

---

## Plantilla completa

````markdown
# {TITULO}

## Tu rol

Eres un **{ROL}** con amplia experiencia en **{DOMINIO}**.

## Proyecto

**CGEIHC-P** — {DESCRIPCION_BREVE}

## Contexto

{CONTEXTO}

## Documentos de referencia

A continuación se incluye el contenido completo de los documentos relevantes del proyecto:

{DOCUMENTOS_EMBEBIDOS}

## Tarea

{TAREA}

## Restricciones

{RESTRICCIONES}

## Formato de respuesta

{FORMATO}

## Instrucción de output

Proporciona tu reporte completo directamente en tu respuesta, formateado en Markdown.
El usuario lo copiará al archivo: `{RUTA_REPORTE}-gemini.md`
````

---

## Variables

| Variable | Descripción |
|---|---|
| `{TITULO}` | Título descriptivo de la consulta |
| `{ROL}` | Rol experto asignado (ver tabla de roles en SKILL.md) |
| `{DOMINIO}` | Área de expertise específica |
| `{DESCRIPCION_BREVE}` | Una línea del proyecto (default: "Museo virtual educativo 3D sobre calentamiento global en el Polo Norte — proyecto final universitario C++/OpenGL, UNAM CGEIHC 2026-2") |
| `{CONTEXTO}` | Situación actual, decisiones ya tomadas, qué se necesita resolver |
| `{DOCUMENTOS_EMBEBIDOS}` | Contenido completo de blueprints u otros docs relevantes, cada uno con `### blueprints/NN-nombre.md` como encabezado |
| `{TAREA}` | Qué se espera exactamente del modelo — preguntas específicas |
| `{RESTRICCIONES}` | Stack fijo (no negociable), limitaciones académicas, tiempo, etc. |
| `{FORMATO}` | Estructura esperada de la respuesta (usar formatos predefinidos de SKILL.md) |
| `{RUTA_REPORTE}` | Path base del archivo de reporte. Ejemplo: `docs/reviews/plan-implementacion-consulta/reportes/consulta-01-plan-implementacion` |

### Notas sobre los documentos embebidos

- Cada documento debe estar precedido por su ruta como encabezado `###`
- Incluir el contenido completo del documento dentro de bloques de código markdown
- Gemini 2.5 Pro puede manejar contextos muy largos — priorizar completitud sobre brevedad
- Si hay documentos muy largos, incluirlos igual — Gemini los procesará correctamente
