# Plantilla de Prompt — Modo B: ChatGPT (chatgpt.com, contexto enfocado)

> **Uso**: Prompt autocontenido para ChatGPT. Contexto resumido (~128K tokens).
> Todo el contexto relevante debe estar embebido — no tiene acceso al proyecto.
> El usuario copia la respuesta y la pega en el placeholder.

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

## Decisiones de diseño ya tomadas (no cambiar)

{DECISIONES_FIJAS}

## Documentos de referencia (resumen)

{DOCUMENTOS_RESUMIDOS}

## Tarea

{TAREA}

## Restricciones

{RESTRICCIONES}

## Formato de respuesta

{FORMATO}

## Instrucción de output

Proporciona tu reporte completo directamente en tu respuesta, formateado en Markdown.
El usuario lo copiará al archivo correspondiente del proyecto.
````

---

## Variables

| Variable | Descripción | Nota |
|---|---|---|
| `{TITULO}` | Título descriptivo | |
| `{ROL}` | Rol experto asignado | |
| `{DOMINIO}` | Área de expertise | |
| `{DESCRIPCION_BREVE}` | Una línea del proyecto | Default: "Museo virtual educativo 3D sobre calentamiento global en el Polo Norte — C++/OpenGL, UNAM CGEIHC 2026-2" |
| `{CONTEXTO}` | Situación actual, qué se necesita resolver | Más detallado que Gemini — GPT no puede investigar |
| `{DECISIONES_FIJAS}` | Stack no negociable, decisiones de diseño ya confirmadas | Solo las que podrían influir en las recomendaciones |
| `{DOCUMENTOS_RESUMIDOS}` | Resumen estructurado de los blueprints relevantes | NO el contenido completo — solo lo esencial para la tarea |
| `{TAREA}` | Qué se espera del modelo | |
| `{RESTRICCIONES}` | Stack fijo, limitaciones académicas, sin over-engineering | |
| `{FORMATO}` | Estructura esperada de la respuesta | |

### Notas sobre el contexto embebido

- **No embeber código completo** — resumir la arquitectura con pseudocódigo o descripciones
- **Priorizar**: stack, arquitectura del módulo, animaciones, shaders necesarios
- **Omitir**: contenido narrativo del museo (textos de paneles, datos científicos)
- Máximo recomendado: ~3,000 palabras de contexto para evitar truncamiento en GPT-4o
