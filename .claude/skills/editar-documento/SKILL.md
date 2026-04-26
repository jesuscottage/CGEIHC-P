---
name: editar-documento
description: Adapta una plantilla del despacho a un caso específico mediante instrucciones en lenguaje natural por el chat. Genera DOCX + PDF versionados, registra cada iteración en su carpeta de caso, y mantiene trazabilidad completa. Triggers en español "edita el documento", "modifica el caso", "abre un caso nuevo", "cierra el caso", o invocación directa "/editar-documento".
allowed-tools: Read, Write, Edit, Bash, Glob, Grep
---

# Skill: /editar-documento

> Skill principal del despacho. Adapta plantillas a casos concretos a
> partir de instrucciones libres del abogado en el chat.

## Pre-lectura obligatoria (orden estricto)

Antes de hacer cualquier otra cosa:

1. Lee **completo** `docs/knowledge/methodology/lecciones-aprendidas-skills.md`.
   Sin esto se repiten errores ya corregidos.
2. Si la instrucción menciona un tipo de documento con síntesis específica
   (`docs/knowledge/methodology/sintesis-{tipo}.md`), léela también.
3. Activa el entorno técnico ejecutando `source setup/env-tools.sh` antes
   del primer comando que use Word, Tesseract o Java.

## Detección del modo

Lee el mensaje del usuario y clasifica como uno de tres modos:

| Modo | Pistas en el mensaje |
|------|----------------------|
| **A — Apertura** | "abre", "nuevo caso", "empieza un caso", menciona un cliente y una plantilla pero no un ID |
| **B — Edición** | menciona un ID de caso existente (`2026-NNN-...`) y describe cambios concretos |
| **C — Cierre** | "cierra", "este es el final", "publica el entregable", "termina el caso" |

Si no es claro, pregunta **una sola vez** antes de actuar. No asumas.

---

## Modo A — Apertura de caso

### Input esperado del chat

> "Abre un caso para Juan García López, materia mercantil, usando la plantilla `arrendamiento-comercial-v2`."

### Pasos

1. Verifica que la plantilla exista: `Glob despacho/plantillas/{nombre}*`.
   Si no existe, lista las plantillas disponibles y pide al usuario
   confirmar el nombre correcto.
2. Ejecuta:
   ```bash
   python .claude/skills/editar-documento/scripts/abrir_caso.py \
     --cliente "Juan García López" \
     --materia "mercantil" \
     --plantilla "arrendamiento-comercial-v2"
   ```
3. El script devuelve JSON con `id`, `carpeta`, `plantilla_copiada`.
4. Reporta al usuario el ID asignado y pregunta qué cambios aplicar
   (esto inicia el siguiente turno en Modo B).

> **No generes `v1` en este modo**. La apertura sólo crea la carpeta y
> copia la plantilla. La primera versión surge de las instrucciones
> que el abogado dará en el siguiente mensaje.

---

## Modo B — Edición sobre caso existente

### Input esperado

> "En el caso `2026-001`, cambia el nombre del arrendatario a Juan García, actualiza la fecha al 25 de abril de 2026, y agrega una cláusula de rescisión por mora superior a 30 días."

### Pasos

1. **Lee el contexto del caso**:
   - `despacho/casos/{id}/caso.yml` (metadatos)
   - El último borrador en `02-borradores/v{N}-*.docx` o, si N=0,
     `01-plantilla/plantilla-origen.docx`.
   - Para inspeccionar contenido del DOCX usa el helper:
     ```bash
     python .claude/skills/editar-documento/scripts/utils.py extraer-texto \
       despacho/casos/{id}/02-borradores/v{N}-...docx
     ```
2. **Parsea las instrucciones del chat** y construye una lista de
   cambios estructurada en YAML (esquema en `aplicar_cambios.py --help`).
   Tipos disponibles:
   - `sustitucion_texto` — buscar y reemplazar literal
   - `actualizacion_fecha` — actualizar fecha en encabezado, cuerpo o pie
   - `insercion_parrafo` — agregar párrafo nuevo en posición específica
   - `eliminacion_parrafo` — quitar un párrafo
   - `rescritura_seccion` — reescribir una sección completa
   - `formato` — bold/italic/indent puntual
   - `agregar_tabla` / `modificar_tabla` — operaciones sobre tablas
3. **Antes de ejecutar**, valida que ninguna instrucción cae en una
   convención intocable (ver sección 2.2 del PLAN.md):
   - Citas textuales no se "corrigen".
   - Numeración de cláusulas no se renumera automáticamente.
   - Marcadores `{{X}}` sólo se sustituyen si la instrucción lo pide.
   - Pies de firma del abogado no se alteran.
   Si una instrucción es ambigua o riesgosa, **pregunta antes de actuar**.
4. **Aplica los cambios**:
   ```bash
   python .claude/skills/editar-documento/scripts/aplicar_cambios.py \
     --caso 2026-001-garcia-lopez \
     --cambios /tmp/cambios.yml
   ```
5. **Genera PDF** vía Word COM:
   ```bash
   python .claude/skills/editar-documento/scripts/convertir_pdf.py \
     --docx despacho/casos/.../v{N+1}-...docx
   ```
6. **Valida ortografía** (no bloqueante, sólo informativo):
   ```bash
   python .claude/skills/editar-documento/scripts/validar_ortografia.py \
     --docx despacho/casos/.../v{N+1}-...docx
   ```
7. **Genera el `.diff.md`** legible con la lista de cambios y el reporte
   de ortografía. Lo escribe el script `aplicar_cambios.py` o tú directo
   con la información que tienes.
8. **Registra las instrucciones literales** del chat en
   `despacho/casos/{id}/00-input/instrucciones-v{N+1}.md` con timestamp.
9. **Reporta al chat** con el formato estándar (ver "Salida al chat" abajo).

### Salida al chat (formato estándar)

```
✅ Caso {id} — borrador v{N+1} generado.

Cambios aplicados:
  1. [descripción corta]
  2. [descripción corta]
  ...

Archivos:
  - DOCX: [ruta]
  - PDF:  [ruta]
  - Diff: [ruta]

⚠️ Validación ortográfica: N observaciones (no bloqueantes). Ver diff.

¿Reviso algo más o procedo a cierre?
```

---

## Modo C — Cierre de caso

### Input esperado

> "Cierra el caso `2026-001`, este es el final."

### Pasos

1. **Confirma con el usuario antes de actuar**:
   > "Voy a cerrar el caso `2026-001` con el borrador `v3`. ¿Confirmas?"
   Sólo procede si responde sí.
2. Ejecuta:
   ```bash
   python .claude/skills/editar-documento/scripts/cerrar_caso.py \
     --caso 2026-001-garcia-lopez
   ```
3. El script:
   - Copia el último borrador a `04-final/final.docx`.
   - Regenera el PDF → `04-final/final.pdf`.
   - Publica copia en `despacho/entregables/{AAAA}/{MM}/`.
   - Actualiza `despacho/entregables/INDICE.md`.
   - Actualiza `caso.yml` con `estado: cerrado` y `fechas.cierre`.
4. Reporta al usuario las rutas finales del entregable publicado.

---

## Captura de lecciones (post-ejecución, todos los modos)

Al final de cada ejecución exitosa, evalúa:

> ¿Surgió algún patrón generalizable que podría aplicar a futuros casos?

**Sí registrar** en `lecciones-aprendidas-skills.md`:
- Sesgos del modelo (confusiones recurrentes entre términos jurídicos).
- Ambigüedades de instrucciones que se repiten ("actualiza la fecha" → ¿cuál?).
- Convenciones del abogado validadas tras varias iteraciones.
- Limitaciones técnicas con workaround conocido.

**No registrar**:
- Correcciones específicas a un documento concreto.
- Datos de clientes (nombres, RFC, fechas, montos).
- Cualquier PII u información identificable.

Si una entrada existente cubre el patrón, **incrementa** el contador
"detectado en N casos" en lugar de duplicar.

---

## Manejo de errores comunes

| Situación | Comportamiento |
|---|---|
| Plantilla no existe | Listar plantillas disponibles del INDICE y pedir al usuario el nombre correcto |
| Word ocupado al convertir PDF | El script reintenta 2 veces y luego usa LibreOffice headless como fallback. Reportar al usuario qué motor se usó |
| Instrucción ambigua ("mejora el documento") | Preguntar al usuario qué entiende por "mejorar" antes de aplicar nada |
| Caso ya cerrado | Bloquear edición. Sugerir: "puedo reabrirlo si confirmas" (acción explícita) |
| ID de caso mal escrito | Hacer fuzzy match contra carpetas de `casos/` y proponer la coincidencia más cercana |
| Borrador previo corrupto | Conservar `v_N` intacto, **no escribir** `v_{N+1}` parcial |

---

## Referencias

- Plan completo: [`docs/plans/skill-editar-documento/PLAN.md`](../../../docs/plans/skill-editar-documento/PLAN.md)
- Plan maestro de arquitectura: [`docs/plans/plan-maestro-arquitectura-archivos.md`](../../../docs/plans/plan-maestro-arquitectura-archivos.md)
- Lecciones aprendidas: [`docs/knowledge/methodology/lecciones-aprendidas-skills.md`](../../../docs/knowledge/methodology/lecciones-aprendidas-skills.md)
- Guía de operación del despacho: [`despacho/README.md`](../../../despacho/README.md)
