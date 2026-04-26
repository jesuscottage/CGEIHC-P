---
title: Plan — Skill /editar-documento
date: 2026-04-25
estado: propuesta — pendiente de aprobación
ámbito: skill principal del despacho — adaptación de plantillas a casos específicos vía instrucciones por chat
plan_maestro: ../plan-maestro-arquitectura-archivos.md
---

# Skill `/editar-documento` — Plan de construcción

> **Resumen en una línea**: skill que recibe instrucciones en lenguaje
> natural por el chat, las aplica sobre la última versión del documento
> de un caso, genera DOCX + PDF nuevos vía Word, y registra todo para
> trazabilidad.

---

## 1. Objetivo y criterios de éxito

### Objetivo

Reemplazar el ciclo manual del abogado:

> plantilla → impresión → anotación a lápiz → secretaria edita → reimpresión → revisión

Por un ciclo asistido:

> plantilla → instrucciones por chat → DOCX + PDF generados → revisión → (iterar)

### Criterios de éxito

La skill se considera lista cuando:

1. **Apertura**: el abogado puede abrir un caso nuevo con una sola
   instrucción en chat ("abre un caso para Juan García usando la plantilla
   de arrendamiento") y obtiene el ID asignado.
2. **Iteración rápida**: cada ronda de cambios produce un DOCX + PDF en
   menos de 60 segundos para documentos de hasta 20 páginas.
3. **Fidelidad de formato**: el DOCX generado preserva fuentes, tablas,
   encabezados, numeración y márgenes de la plantilla original — no
   "se ve diferente".
4. **Trazabilidad completa**: cada borrador tiene su `.diff.md` legible
   en español que un humano puede leer sin abrir el documento.
5. **Cero pérdida de trabajo**: ningún borrador anterior se sobrescribe
   jamás. Si una iteración sale mal, el abogado puede volver al `v_{N-1}`.
6. **Cierre limpio**: una sola instrucción ("cierra el caso") publica el
   final en `entregables/`, actualiza `INDICE.md` y deja todo trazable.

### Lo que NO hace esta skill (delimitación)

- No redacta documentos desde cero — sólo adapta plantillas existentes.
  Para redacción desde cero, irá una skill especializada (p. ej.
  `/redactar-amparo`) con su propia investigación previa.
- No interpreta documentos escaneados de la contraparte — eso lo hace
  una utilidad separada de OCR + extracción.
- No firma electrónicamente ni envía a juzgados. Sólo produce
  entregables listos para que el abogado los firme y entregue.
- No "mejora" el contenido por iniciativa propia. Sólo aplica los cambios
  que el abogado pide explícitamente.

---

## 2. Conocimiento técnico mínimo

> Esta skill es **adaptativa, no generativa**: parte de plantillas que
> el abogado ya validó y sólo aplica los cambios que él pide. Por eso
> no requiere investigación previa exhaustiva (a diferencia de skills
> generativas como `/redactar-amparo`).
>
> Lo que sigue son los gotchas y convenciones que la implementación
> debe conocer desde el primer día — embebidos aquí para no inflar la
> investigación. El conocimiento profundo se acumula en
> `docs/knowledge/methodology/lecciones-aprendidas-skills.md` con el
> uso real.

### 2.1. Gotchas de `python-docx` a tener presentes

| # | Riesgo | Cómo evitarlo |
|---|--------|---------------|
| 1 | Editar `cell.text = "x"` borra el formato de la celda | Usar `cell.paragraphs[0].runs[i].text = "x"` para preservar fuente, negrita, alineación |
| 2 | Texto continuo está partido en múltiples `runs` (Word optimiza internamente) | Antes de buscar/sustituir, **normalizar runs**: concatenar runs adyacentes con mismo formato, o usar regex sobre `paragraph.text` y luego mapear a runs |
| 3 | Insertar párrafo en posición específica (no al final) | `paragraph._p.addprevious(new_p)` — la API pública sólo añade al final |
| 4 | Encabezados y pies de página viven en `section.header.paragraphs`, no en el body | Aplicar cambios también ahí cuando la instrucción es "actualiza la fecha" — la fecha suele estar en el header |
| 5 | Numeración automática se rompe al cambiar el `style` del párrafo | No tocar el style — sólo modificar `runs[i].text` |
| 6 | Saltos de página están en propiedades del párrafo o como `<w:br type="page">` | Al duplicar/mover párrafos, preservar `paragraph_format.page_break_before` |
| 7 | Estilos personalizados de la plantilla no se heredan al agregar párrafos nuevos | Copiar el `style` explícitamente: `new_p.style = source_p.style` |
| 8 | Comments, track changes y campos dinámicos (`{ DATE }`) no son manejados bien por python-docx | Para esos casos, usar Word COM (pywin32) que sí los entiende, o copiar vía XML directo |
| 9 | Tablas anidadas y celdas combinadas (`merged cells`) requieren iteración cuidadosa | No usar índices fijos — iterar `for row in table.rows: for cell in row.cells` y filtrar duplicados por `cell._tc` |
| 10 | Sustituir un marcador `{{X}}` puede fallar si está partido entre runs | Workaround: para marcadores, primero reconstruir cada `paragraph.text`, hacer la sustitución sobre el string completo, y reescribir borrando los runs viejos y creando uno nuevo (perdiendo formato fino — aceptable para marcadores porque suelen ir en texto plano) |

### 2.2. Convenciones intocables del documento jurídico

Esto la skill **nunca debe alterar por iniciativa propia**, sólo si la
instrucción lo pide explícitamente:

- **Citas textuales** de jurisprudencia, ley, tratados, doctrina:
  jamás cambiar puntuación, acentos, mayúsculas, ni "corregir" lo que
  parece un typo — puede ser textual de la fuente.
- **Numeración de cláusulas y secciones**: nunca renumerar
  automáticamente. El abogado puede tener referencias cruzadas
  ("conforme a la cláusula séptima..."). Insertar una cláusula nueva
  no debe renumerar las posteriores salvo instrucción expresa.
- **Fechas en letra** vs **numéricas**: respetar el formato del
  documento. Si el documento usa "veinticinco de abril del año dos mil
  veintiséis", no convertir a "25/04/2026" salvo instrucción.
- **Encabezados procesales** ("C. JUEZ DÉCIMO DE LO CIVIL..."):
  mantener mayúsculas, abreviaturas y puntuación originales.
- **Firmas, sellos y espacios reservados** (líneas en blanco para
  firma manual): no rellenar, no quitar.
- **Marcadores de plantilla** (`{{NOMBRE_QUEJOSO}}`, `[CLIENTE]`,
  etc.): sustituir sólo si la instrucción menciona el dato. No
  asumir.
- **Pies de firma con datos profesionales** del abogado (cédula,
  número de patente, domicilio del despacho): nunca alterar a menos
  que la instrucción lo pida.

> Esta lista es semilla. Conforme aparezcan otras convenciones se
> registran en `lecciones-aprendidas-skills.md` bajo "Convenciones del
> abogado validadas".

---

## 3. Flujo de la skill

### 3.1. Entrada (input desde chat)

Tres modos de invocación:

#### Modo A — Apertura de caso nuevo

```
Abre un caso nuevo para Juan García López, materia mercantil,
usando la plantilla "arrendamiento-comercial-v2".
```

#### Modo B — Edición sobre caso existente

```
En el caso 2026-001, cambia el nombre del arrendatario a Juan García,
actualiza la fecha al 25 de abril de 2026, y agrega una cláusula de
rescisión por mora superior a 30 días.
```

#### Modo C — Cierre de caso

```
Cierra el caso 2026-001, este es el final.
```

> **No hay flags ni sintaxis especial**. La skill detecta el modo
> leyendo el mensaje. Si ambiguo, pregunta una sola vez.

### 3.2. Pre-carga de contexto (siempre, en este orden)

1. **`docs/knowledge/methodology/lecciones-aprendidas-skills.md`** —
   patrones de error generales acumulados. Sin esto se repiten errores
   ya corregidos.
2. Si hay sintesís específica para el tipo de documento
   (`docs/knowledge/methodology/sintesis-{tipo}.md`), también se carga.
   Para esta skill general no es obligatoria.
3. Si es Modo B o C: leer `casos/{id}/caso.yml` y el último borrador
   en `02-borradores/`.
4. Si es Modo A: leer `plantillas/INDICE.md` para localizar la plantilla
   y luego su `.meta.yml`.

### 3.3. Procesamiento

#### Modo A — Apertura

1. Validar que la plantilla exista en `despacho/plantillas/`.
2. Asignar correlativo anual: leer la lista de casos del año, asignar
   el siguiente número (`2026-001`, `2026-002`, ...).
3. Generar slug del cliente (kebab-case, sin acentos, máx. 30 chars).
4. Crear esqueleto: `casos/{id}/{00-input,01-plantilla,02-borradores}/`.
5. Copiar plantilla a `01-plantilla/plantilla-origen.docx`.
6. Crear `caso.yml` desde `_templates/caso.yml.template`.
7. **No genera v1 todavía** — espera la primera tanda de instrucciones.
8. Responder al chat con el ID del caso y la ruta.

#### Modo B — Edición

1. Identificar el último borrador `v_N` (o la plantilla si N=0).
2. **Parsear instrucciones**: Claude analiza el mensaje del chat y
   produce una "lista de cambios" estructurada (ver sección 4.2).
3. **Aplicar cambios** sobre una copia del v_N usando python-docx
   (operaciones: substitución de texto, inserción de párrafo,
   modificación de cláusula, ajuste de formato puntual).
4. Guardar como `02-borradores/v{N+1}-{AAAA-MM-DD}-{HHMM}.docx`.
5. **Generar PDF** vía docx2pdf (Word COM) →
   `02-borradores/v{N+1}-{AAAA-MM-DD}-{HHMM}.pdf`.
6. **Validar ortografía** del documento generado con
   `language_tool_python` (modo es). Reportar errores en el `.diff.md`,
   no bloquear (el abogado decide si corrige).
7. **Generar `.diff.md`** legible: lista en español de qué cambió,
   sección por sección.
8. **Registrar instrucciones**: copiar el texto literal del mensaje
   del abogado a `00-input/instrucciones-v{N+1}.md` con timestamp.
9. **Actualizar `caso.yml`**: agregar entrada al array `borradores`,
   actualizar `ultima_modificacion`.
10. **Captura de lecciones (post-ejecución)**: si en el proceso surgió
    algún patrón generalizable (instrucción ambigua que se repitió,
    error del modelo detectado, convención del abogado validada),
    agregarlo a `lecciones-aprendidas-skills.md` siguiendo las reglas
    de filtrado.

#### Modo C — Cierre

1. Confirmar con el abogado: "voy a cerrar el caso `{id}` con el
   borrador `v_N`. ¿Confirmas?". Sólo si responde sí.
2. Copiar `v_N.docx` → `04-final/final.docx`.
3. Regenerar PDF → `04-final/final.pdf`.
4. Copiar a `entregables/{AAAA}/{MM}/{caso-id}-final.{docx,pdf}`.
5. Agregar fila a `entregables/INDICE.md`.
6. Actualizar `caso.yml`: `estado: cerrado`, `fechas.cierre: hoy`.
7. Responder con la ruta del entregable publicado.

### 3.4. Salida al chat

Estructurada y consistente:

```
✅ Caso 2026-001-garcia-lopez — borrador v3 generado.

Cambios aplicados:
  1. Sustitución del nombre del arrendatario (3 ocurrencias)
  2. Actualización de fecha en encabezado y pie
  3. Cláusula nueva agregada después del punto 7

Archivos:
  - DOCX: despacho/casos/2026-001-garcia-lopez/02-borradores/v3-2026-04-25-1635.docx
  - PDF:  despacho/casos/2026-001-garcia-lopez/02-borradores/v3-2026-04-25-1635.pdf
  - Diff: despacho/casos/2026-001-garcia-lopez/02-borradores/v3-2026-04-25-1635.diff.md

⚠️ Validación ortográfica: 2 observaciones (no bloqueantes). Ver diff.

¿Reviso algo más o procedo a cierre?
```

---

## 4. Diseño técnico (resumen)

> El detalle vive en [`01-arquitectura-tecnica.md`](01-arquitectura-tecnica.md).

### 4.1. Reparto de responsabilidades

| Componente | Responsabilidad |
|---|---|
| **SKILL.md** | Define disparadores, pre-carga obligatoria, orquesta el flujo |
| **Claude (LLM)** | Parsea instrucciones, decide qué cambios aplicar, genera el `.diff.md` legible, evalúa lecciones aprendidas. Aplica los gotchas y convenciones de sección 2 al decidir qué tocar y qué no |
| **Scripts Python** | Operaciones mecánicas: leer/escribir DOCX, aplicar cambios deterministas, convertir a PDF, ejecutar LanguageTool, asignar correlativo de caso |
| **Word (COM)** | Conversión final DOCX → PDF (preferencia del usuario); también para edición de documentos con campos dinámicos o track changes (gotcha #8 sección 2.1) |

### 4.2. "Lista de cambios" (formato intermedio)

Claude convierte instrucciones libres en una lista estructurada antes de
ejecutar. Esto permite:
- Auditoría: el `.diff.md` se construye desde esta lista.
- Determinismo: la misma lista aplicada al mismo input produce el mismo
  output.
- Validación previa: si una operación es ambigua, la skill puede pedir
  aclaración antes de modificar nada.

Esquema:

```yaml
cambios:
  - tipo: sustitucion_texto
    objetivo: "Pedro Pérez"
    nuevo: "Juan García"
    alcance: documento_completo
    case_sensitive: true

  - tipo: actualizacion_fecha
    objetivo_regex: "fecha del encabezado"
    nueva_fecha: "25 de abril de 2026"

  - tipo: insercion_parrafo
    despues_de: "Cláusula séptima"
    contenido: "Octava. Rescisión por mora superior a 30 días..."

  - tipo: rescritura_seccion
    seccion: "Cláusula 5"
    motivo: "el abogado pidió versión más estricta"
    nuevo_contenido: |
      [texto completo de la nueva cláusula]
```

Tipos previstos (extensibles):
- `sustitucion_texto`
- `actualizacion_fecha`
- `insercion_parrafo`
- `eliminacion_parrafo`
- `rescritura_seccion`
- `formato` (bold/italic/indent)
- `agregar_tabla` / `modificar_tabla`

### 4.3. Scripts Python a construir

Vivirán en `.claude/skills/editar-documento/scripts/`:

| Script | Responsabilidad |
|---|---|
| `abrir_caso.py` | Crea esqueleto de caso, asigna ID anual, copia plantilla |
| `aplicar_cambios.py` | Toma DOCX origen + YAML de cambios → genera DOCX modificado |
| `convertir_pdf.py` | DOCX → PDF vía docx2pdf (Word COM); fallback a LibreOffice si Word ocupado |
| `validar_ortografia.py` | Extrae texto del DOCX, lo pasa por language_tool_python, devuelve observaciones |
| `cerrar_caso.py` | Copia a `04-final/`, publica en `entregables/`, actualiza INDICE |
| `utils.py` | Helpers: slug, asignación correlativo, lectura/escritura caso.yml |

### 4.4. Manejo de errores

| Situación | Comportamiento |
|---|---|
| Plantilla no existe | Pedir confirmación de nombre, mostrar plantillas disponibles del INDICE |
| Word ocupado al convertir PDF | Reintentar 2 veces, luego usar LibreOffice headless como fallback (ya instalado) |
| Instrucción ambigua | Preguntar al abogado **antes** de aplicar cambios, no asumir |
| Error de ortografía | Reportar pero no bloquear — el abogado decide |
| Caso ya cerrado | Bloquear edición — proponer "reabrir caso" como acción explícita |
| Borrador corrupto | Conservar `v_N` intacto, no escribir `v_{N+1}` parcial |

---

## 5. Estructura `SKILL.md` propuesta

Vivirá en `.claude/skills/editar-documento/SKILL.md`.

```markdown
---
name: editar-documento
description: Adapta una plantilla del despacho a un caso específico mediante
  instrucciones en lenguaje natural por el chat. Genera DOCX + PDF
  versionados, registra cada iteración, y mantiene trazabilidad completa.
  Triggers: "edita", "modifica el documento", "abre un caso nuevo",
  "cierra el caso", "/editar-documento".
allowed-tools: Read, Write, Edit, Bash, Glob, Grep
---

# Skill: /editar-documento

## Pre-lectura obligatoria (orden estricto)

1. Lee `docs/knowledge/methodology/lecciones-aprendidas-skills.md`
   completo. No saltes esto — patrones generales que evitan repetir
   errores.
2. Si existe síntesis específica para el tipo de documento
   (`docs/knowledge/methodology/sintesis-{tipo}.md`), léela.

## Detección del modo

Lee el mensaje del usuario y clasifica como:
- **Apertura** si menciona "abrir caso", "nuevo caso", "empezar caso"
- **Edición** si menciona un ID de caso existente y describe cambios
- **Cierre** si dice "cierra", "este es el final", "publica el entregable"

Si no es claro, pregunta una sola vez antes de actuar.

## Flujo por modo

[Detalles operativos por modo — ver PLAN.md sección 3.3]

## Captura de lecciones (post-ejecución)

Al final, evalúa si surgió un patrón generalizable. Si lo hay y respeta
el filtro de privacidad (sin PII, aplicable a futuros casos), agrégalo
a `docs/knowledge/methodology/lecciones-aprendidas-skills.md`.
```

---

## 6. Casos de prueba

| # | Escenario | Resultado esperado |
|---|-----------|--------------------|
| 1 | Abrir caso con plantilla existente | Carpeta `2026-NNN-...` creada, plantilla copiada, sin v1 todavía |
| 2 | Editar caso recién abierto con 3 sustituciones simples | v1 generado, .diff.md con 3 entradas, PDF abierto en Word |
| 3 | Editar caso con cláusula nueva + reescritura | v2 generado preservando formato de la plantilla |
| 4 | Iterar 5 veces en el mismo caso | v1..v5 todos presentes, ninguno sobrescrito |
| 5 | Pedir cambio ambiguo ("mejora la redacción") | Skill pregunta qué se considera mejorar antes de actuar |
| 6 | Cerrar caso | final.docx + final.pdf en `04-final/`, copia en entregables/, INDICE actualizado |
| 7 | Intentar editar caso cerrado | Bloqueado con mensaje claro |
| 8 | Plantilla con tabla compleja | Tabla preservada en v1 sin distorsión |
| 9 | Word abierto por el abogado al pedir conversión | Reintenta, luego usa LibreOffice headless |
| 10 | Plantilla con falta de tildes | LanguageTool reporta, no bloquea |

---

## 7. Checklist de implementación

### Fase A — Scaffolding técnico
- [ ] Crear `.claude/skills/editar-documento/SKILL.md`
- [ ] Crear `.claude/skills/editar-documento/scripts/` con stubs de los
      6 scripts y tests unitarios mínimos por cada uno
- [ ] Implementar `utils.py` (slug, correlativo, caso.yml read/write)
- [ ] Implementar `abrir_caso.py` y probar Modo A end-to-end

### Fase B — Núcleo de edición
- [ ] Definir formato YAML de "lista de cambios" (sección 4.2)
- [ ] Implementar `aplicar_cambios.py` cubriendo los 7 tipos previstos,
      respetando los gotchas de la sección 2.1
- [ ] Implementar `convertir_pdf.py` con fallback Word→LibreOffice
- [ ] Implementar `validar_ortografia.py`
- [ ] Generación de `.diff.md` desde la lista de cambios

### Fase C — Cierre y publicación
- [ ] Implementar `cerrar_caso.py`
- [ ] Auto-actualización de `entregables/INDICE.md` y `caso.yml`

### Fase D — Pruebas integradas
- [ ] Crear plantilla de prueba `ejemplo-plantilla-v1.docx` en
      `despacho/plantillas/` con casos representativos: tabla, encabezado,
      marcadores `{{...}}`, cláusulas numeradas, cita textual entrecomillada
- [ ] Ejecutar los 10 casos de prueba de la sección 6
- [ ] Verificar que las convenciones intocables (sección 2.2) se respetan

### Fase E — Captura de lecciones
- [ ] Verificar que la skill agrega entradas a
      `lecciones-aprendidas-skills.md` después de ejecuciones reales
- [ ] Verificar que respeta el filtro de PII (probar con un caso que
      contenga datos sensibles y revisar que NO los registre)

---

## 8. Decisiones pendientes (para validar antes de Fase B)

1. **Formato de los IDs de plantilla**: ¿el abogado los recordará por
   nombre completo (`arrendamiento-comercial-v2`) o necesitamos un
   buscador semántico? Recomendación: arrancar con nombre completo +
   búsqueda fuzzy por keywords como fallback.
2. **Manejo de cambios masivos** (más de 20 sustituciones en una
   instrucción): ¿procesarlos en lote o pedir confirmación intermedia?
   Recomendación: lote, mostrar resumen al final.
3. **Versiones rama** (¿qué pasa si el abogado pide "vuelve al v3 y desde
   ahí cambia X"?): ¿generamos un v_N+1 lineal o creamos rama explícita?
   Recomendación: lineal por ahora — el `.diff.md` permite reconstruir
   cualquier versión intermedia. Branching sólo si surge la necesidad.
4. **Confirmación previa a aplicar cambios**: ¿siempre, sólo cambios
   destructivos, o nunca? Recomendación: nunca para sustituciones
   simples; siempre para reescrituras de sección completa.
5. **Idempotencia de la apertura de caso**: si el abogado intenta abrir
   un caso para un cliente que ya tiene caso abierto, ¿reutilizamos o
   creamos uno nuevo? Recomendación: avisar y preguntar.

---

## 9. Sub-planes

Por ahora ninguno separado. Si la implementación revela que algún
componente necesita más profundidad arquitectónica, se crea entonces
el sub-plan correspondiente en esta misma carpeta.

Para skills futuras especializadas (`/redactar-amparo`, etc.) sí se
mantiene la regla de investigación previa exhaustiva, porque ahí el
contenido se genera y la fidelidad al canon jurídico es crítica. Esa
regla sigue vigente en la memoria
[`feedback_skills_jurídicas.md`](../../../.claude/projects/c--Users-herna-Downloads-Jes-s-Juan-Pablo/memory/feedback_skills_jur%C3%ADdicas.md).
