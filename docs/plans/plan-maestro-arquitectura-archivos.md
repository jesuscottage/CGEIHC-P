---
title: Plan maestro — Arquitectura de archivos jurídicos
date: 2026-04-25
estado: aprobado — decisiones de sección 8 confirmadas el 2026-04-25
ámbito: organización física de plantillas, casos, entregables y artefactos generados por skills
---

# Plan maestro: Arquitectura de archivos jurídicos

> Este plan define **dónde viven los documentos** del despacho dentro del
> repositorio del proyecto, cómo se versionan, cómo se identifican los
> entregables y cómo se enlazan con las skills (especialmente la skill
> prioritaria de edición de documentos).
>
> No tocar la estructura de la plantilla Machote (`.claude/`, `docs/knowledge/`,
> `docs/plans/`, `docs/reviews/`, `scripts/`, `setup/`). Todo lo nuevo se
> agrega bajo una carpeta dedicada `despacho/` en la raíz del proyecto.

---

## 1. Principios de diseño

1. **Separación clara entre "fuente" y "derivado"**: las plantillas y los
   inputs del abogado nunca se mezclan con los archivos producidos por las
   skills.
2. **Inmutabilidad de las plantillas**: una plantilla aprobada no se edita
   in-place; se clona a la carpeta del caso y se modifica la copia.
3. **Un caso = una carpeta**: todo lo relacionado a un cliente/asunto vive
   en su propia carpeta autocontenida (plantilla origen, instrucciones,
   versiones, entregable final).
4. **Trazabilidad por convención de nombres**: el nombre del archivo basta
   para saber qué es, de qué caso, qué versión y qué fecha.
5. **Identificación rápida de entregables nuevos**: existe una zona única
   (`entregables/`) donde se publica la versión final aprobada de cada caso,
   con índice actualizado.
6. **Privacidad**: la carpeta `despacho/` se ignora por completo en git
   (datos de clientes reales) o se cifra; sólo se versionan plantillas y
   metadatos sin PII.

---

## 2. Estructura propuesta

**Ubicación**: dentro del repositorio (`despacho/` en la raíz). Esto permite
indexar los nombres y metadatos en `docs/knowledge/INDEX.md` y referenciar
documentos con rutas relativas estables desde las skills. Los datos de
clientes reales quedan excluidos de git por `.gitignore` (sólo se versionan
plantillas, índices y `README.md`).

```
despacho/
├── README.md                          # Cómo operar esta carpeta (manual corto)
│
├── plantillas/                        # Documentos originales reutilizables (estructura libre)
│   ├── INDICE.md                      # Índice maestro de plantillas
│   └── *.{docx|pdf}                   # El abogado organiza por subcarpetas si quiere; no se imponen
│
├── casos/                             # Un caso por carpeta
│   └── {AAAA}-{NNN}-{slug-cliente}/   # p. ej. 2026-001-garcia-lopez
│       ├── caso.yml                   # Metadatos: cliente, materia, fechas, plantilla origen
│       ├── 00-input/                  # Materiales de partida del caso
│       │   ├── instrucciones-vN.md    # Log de instrucciones recibidas por chat (lo escribe la skill)
│       │   └── adjuntos/              # Documentos del cliente (constancias, contratos previos)
│       ├── 01-plantilla/              # Copia de la plantilla original (NO editar aquí)
│       │   └── plantilla-origen.docx
│       ├── 02-borradores/             # Versiones generadas por la skill, numeradas
│       │   ├── v1-2026-04-25-1430.docx
│       │   ├── v1-2026-04-25-1430.diff.md   # Resumen legible de cambios respecto a plantilla
│       │   ├── v2-2026-04-25-1530.docx
│       │   └── v2-2026-04-25-1530.diff.md
│       └── 04-final/                  # Versión aprobada lista para entregar
│           ├── final.docx
│           └── final.pdf
│
├── entregables/                       # Vista plana de TODO lo finalizado
│   ├── INDICE.md                      # Tabla con todos los entregables (caso, fecha, archivo)
│   └── {AAAA}/{MM}/                   # Organizado por año/mes de finalización
│       └── {caso-id}-final.{docx|pdf} # Copia del 04-final/
│
├── archivo-historico/                 # Documentos antiguos a destilar como futuras plantillas
│   ├── INDICE.md                      # Catálogo (estructura libre debajo)
│   └── *.{docx|pdf}                   # Sin subdivisión obligatoria
│
└── _trabajo/                          # Carpeta scratch para procesos intermedios (gitignored)
    └── ocr/                           # OCR de PDFs escaneados, conversiones temporales
```

> **Nota sobre estructura libre**: tanto `plantillas/` como `archivo-historico/`
> aceptan los documentos directamente o agrupados como prefiera el abogado.
> Las skills de búsqueda y selección operan sobre los `INDICE.md` y los
> `.meta.yml`, no sobre la estructura de carpetas — así no se rompe nada si
> se reorganiza después. Eliminada la subcarpeta `03-revisiones/` y los
> archivos de audio en `00-input/` porque el abogado dará las modificaciones
> directamente por chat (decisión 3 de la sección 8).

---

## 3. Convenciones de nombres

### Plantillas
- **Patrón sugerido**: `{descripcion-corta}-v{N}.{ext}` (kebab-case, sin
  imponer categoría — el abogado nombra como le sea útil).
- Ejemplos: `amparo-indirecto-v1.docx`, `contrato-honorarios-v3.docx`,
  `escrito-promocion-v1.docx`.
- Cada plantilla tiene su archivo `.meta.yml` hermano con:
  ```yaml
  titulo: Amparo indirecto — modelo base
  autor: Juan Pablo
  fecha_creacion: 2018-03-12
  fecha_revision: 2026-04-25
  variables_detectadas: [nombre_quejoso, autoridad_responsable, acto_reclamado]
  notas: "Plantilla usada en 14 casos exitosos entre 2018 y 2025"
  tags: [amparo, indirecto]   # opcionales — sirven al INDICE para filtrar
  ```
- El campo `materia` se elimina como obligatorio; los `tags` libres lo
  reemplazan (consistente con la decisión de "formato libre").

### Casos
- **ID de caso**: `{AAAA}-{NNN}-{slug-cliente-corto}` — año + correlativo + slug.
- Ejemplo: `2026-001-garcia-lopez`, `2026-002-construcciones-norte`.
- El correlativo se asigna por orden de apertura del caso en el año.

### Borradores
- **Patrón**: `v{N}-{AAAA-MM-DD}-{HHMM}.docx`
- Ejemplo: `v3-2026-04-25-1635.docx`
- Cada borrador tiene un `.diff.md` hermano que describe en lenguaje natural
  qué cambió respecto al borrador anterior (o respecto a la plantilla, si
  es v1).

### Entregable final
- En `casos/{id}/04-final/`: siempre `final.docx` + `final.pdf`.
- En `entregables/{año}/{mes}/`: `{caso-id}-final.docx` (copia generada al
  cerrar el caso — Windows no maneja bien symlinks sin permisos elevados).

### Numeración de casos (decisión confirmada)
- Correlativo **anual reiniciado**: `2026-001`, `2026-002`, ..., `2027-001`.
- Más legible que un correlativo perpetuo y suficiente para el volumen
  esperado de un despacho individual.

---

## 4. Identificación de entregables nuevos

Tres mecanismos complementarios:

1. **Índice maestro `entregables/INDICE.md`**: tabla con columnas
   `caso-id | cliente | tipo-documento | fecha-finalización | enlace`. Se
   actualiza automáticamente cuando una skill mueve algo a `04-final/`.
2. **Carpeta cronológica `entregables/{AAAA}/{MM}/`**: permite escanear
   visualmente lo más reciente sin abrir el índice.
3. **Hook de Claude Code**: descartado para fase 1 (decisión 4). Cuando se
   implemente como mejora posterior: un `PostToolUse` que, al detectar
   escritura en `04-final/`, agregue una fila al INDICE y emita una
   notificación. Por ahora, el INDICE se actualiza desde dentro de la propia
   skill de cierre de caso.

---

## 5. Integración con skills (especialmente la de edición de documentos)

La skill prioritaria — provisionalmente `/editar-documento` — operará así:

1. **Input** (todo desde el chat de Claude Code, sin archivos intermedios):
   - ID del caso (si ya existe) o solicitud de creación de caso nuevo.
   - Identificador de plantilla a usar (nombre o tags).
   - **Instrucciones en texto libre directamente en el mensaje del chat**:
     "cambia el nombre del quejoso a Juan García, actualiza la fecha al
     25 de abril de 2026, sustituye la cita del artículo 14 por la 16...".
2. **Pre-carga de contexto** (en este orden, antes de cualquier acción):
   - **Siempre**: `docs/knowledge/methodology/lecciones-aprendidas-skills.md`
     — patrones de error generales y mejoras acumuladas que aplican a todas
     las skills jurídicas. Sin esto, se repiten errores ya corregidos.
   - **Para skills especializadas** (amparos, etc.):
     `docs/knowledge/methodology/sintesis-{tipo-documento}.md`. Sin esta
     síntesis la skill especializada **no se ejecuta** (ver memoria
     `feedback_skills_jurídicas.md`).
   - **Para edición genérica** (`/editar-documento`): la síntesis específica
     es opcional pero recomendada cuando exista.
3. **Proceso**:
   - Si es la primera ejecución del caso: copia la plantilla a
     `01-plantilla/` y crea `caso.yml`.
   - Genera el siguiente `v{N+1}` en `02-borradores/` aplicando las
     instrucciones recibidas.
   - Genera el `.diff.md` legible (resumen de cambios respecto al borrador
     anterior).
   - Registra el texto literal de las instrucciones del chat en
     `00-input/instrucciones-v{N+1}.md` con timestamp, **para trazabilidad
     y para reconstruir el razonamiento si hay disputa posterior**.
4. **Output al chat**: ruta del nuevo borrador + resumen de cambios + path
   del PDF generado vía Word COM (preferencia del usuario).
5. **Cierre del caso** (comando explícito del abogado, p. ej.
   "cierra el caso", "este es el final"): la skill copia el último borrador
   a `04-final/`, regenera el PDF, publica copia en `entregables/{año}/{mes}/`
   y actualiza `entregables/INDICE.md`.
6. **Captura de lecciones (post-ejecución)**: al detectar una corrección,
   queja o ajuste del abogado, la skill evalúa si es **generalizable** (un
   patrón que puede repetirse en otros casos o que revela un sesgo del
   modelo). Si lo es, agrega una entrada a
   `docs/knowledge/methodology/lecciones-aprendidas-skills.md` en formato
   estructurado. Si es específico de este caso (un nombre, una fecha, un
   dato propio de este cliente), **no se agrega** — sólo permanece en la
   bitácora del caso.

---

## 5.bis. Documento de mejora continua: lecciones aprendidas

Vive en `docs/knowledge/methodology/lecciones-aprendidas-skills.md` (no en
`despacho/`, porque es metodología transversal, no datos del despacho).

**Propósito**: capturar patrones generales de error y sugerencias acumuladas
durante el uso real de las skills, para que cada nueva ejecución parta de
un piso más alto y no se repitan los mismos tropiezos.

**Reglas estrictas de contenido**:

| ✅ SÍ va aquí | ❌ NO va aquí |
|---|---|
| "El modelo confunde 'recurso de revisión' con 'recurso de revocación' — verificar siempre el ordenamiento procesal aplicable" | "En el caso 2026-007, cambiar 'revisión' por 'revocación'" |
| "Cuando el abogado pide 'actualiza la fecha', se refiere a la fecha del documento, no a las fechas históricas citadas dentro del cuerpo" | "Para García López, la fecha es 25 abril 2026" |
| "PDFs escaneados pre-2010 suelen tener resolución <200 dpi y requieren `--dpi 300` en ocrmypdf" | "El PDF de Pérez Construcciones requiere reescaneo" |
| "El abogado prefiere mantener la cita textual de jurisprudencia incluso si suena redundante" | "En este escrito quitar la cita del párrafo 3" |

**Formato de entrada**:

```markdown
### [YYYY-MM-DD] Título corto del patrón

- **Qué pasaba**: descripción del error o fricción observada.
- **Por qué**: causa raíz (sesgo del modelo, ambigüedad de instrucción,
  limitación técnica, convención del abogado, etc.).
- **Cómo evitarlo**: regla concreta a aplicar en futuras ejecuciones.
- **Detectado en**: cantidad de casos donde apareció (sin nombrarlos).
```

**Ciclo de actualización**:
1. La skill detecta el patrón al final de su ejecución.
2. Antes de agregarlo, valida que **no contiene PII ni datos del caso**.
3. Si una entrada existente cubre el patrón, se **incrementa** el contador
   de "detectado en" en lugar de duplicar.
4. Periódicamente (cuando lo solicite el usuario) se condensan entradas
   redundantes.

**Cómo se garantiza que esté "a la vista"**:
- Cada `SKILL.md` jurídica debe declarar este archivo como pre-lectura
  obligatoria en su sección inicial.
- Una memoria de tipo `feedback` apunta a este archivo, asegurando que
  Claude lo cargue incluso fuera del contexto de una skill formal.
- El archivo vive bajo `docs/knowledge/`, así `/consultar-conocimiento`
  también lo encuentra.

---

## 6. Privacidad y seguridad

- Agregar a `.gitignore` del proyecto:
  ```
  /despacho/casos/
  /despacho/_trabajo/
  /despacho/archivo-historico/
  /despacho/entregables/
  ```
- **Versionar sí**: `despacho/plantillas/` (sin PII, son modelos), los
  índices `INDICE.md` (sin datos sensibles), `README.md`.
- **Versionar no**: nada que contenga datos reales de clientes.
- Considerar más adelante cifrado en reposo (BitLocker/EFS) para la
  carpeta `casos/`.

---

## 7. Checklist de implementación

### Fase 1 — Estructura base
- [ ] Crear `despacho/` con subcarpetas (`plantillas/`, `casos/`,
      `entregables/`, `archivo-historico/`, `_trabajo/`) y `.gitkeep` donde aplique
- [ ] Escribir `despacho/README.md` (manual corto, español, sin jerga técnica)
- [ ] Crear los tres `INDICE.md`: `plantillas/`, `entregables/`, `archivo-historico/`
- [ ] Actualizar `.gitignore`: ignorar `casos/`, `_trabajo/`, `archivo-historico/`,
      `entregables/`; mantener versionados los `INDICE.md`, `README.md` y `plantillas/`
- [ ] Crear esqueletos `caso.yml.template` y `meta.yml.template` con ejemplos comentados
- [ ] Generar un caso de ejemplo (`2026-000-ejemplo`) con todas las subcarpetas
      y archivos de muestra para servir de referencia visual
- [ ] Crear `docs/knowledge/methodology/lecciones-aprendidas-skills.md`
      con frontmatter, las dos secciones (errores corregidos / sugerencias
      acumuladas) y la tabla de qué sí / qué no incluir
- [ ] Registrar el archivo en `docs/knowledge/INDEX.md`

### Fase 2 — Comandos de gestión de casos
- [ ] Script "abrir caso nuevo": pide datos al usuario, asigna correlativo
      anual, crea esqueleto de carpeta, copia plantilla seleccionada
- [ ] Script "cerrar caso": copia último borrador a `04-final/`, genera PDF
      vía Word COM, publica copia en `entregables/{AAAA}/{MM}/`, actualiza INDICE

### Fase 3 — Skills
- [ ] Plan de la skill `/editar-documento` (sub-plan separado en
      `docs/plans/skill-editar-documento/`)
- [ ] Plan de la primera skill especializada — p. ej. `/redactar-amparo` —
      con su fase de investigación previa obligatoria (sub-plan separado)

### Fase 4 — Mejoras (postergadas)
- [ ] Hook `PostToolUse` para auto-actualizar INDICE de entregables
- [ ] Conversión voz → texto si en algún momento el abogado prefiere dictar
      las instrucciones en lugar de escribirlas

---

## 8. Decisiones tomadas (2026-04-25)

| # | Decisión | Implicación en este plan |
|---|----------|--------------------------|
| 1 | **Ubicación**: `despacho/` dentro del repo, gitignored, indexado en `docs/knowledge/INDEX.md` | Skills operan con rutas relativas estables. |
| 2 | **Tipos de documento**: formato libre, sin subcarpetas obligatorias | Eliminadas subcarpetas `amparos/`, `contratos/`, etc. de `plantillas/` y `archivo-historico/`. Filtrado se hace por `tags` en `.meta.yml` y por `INDICE.md`. |
| 3 | **Formato de instrucciones**: texto libre directamente en el chat | Eliminadas subcarpetas `audio/` y archivo `instrucciones.md` como input obligatorio. La skill registra las instrucciones recibidas como `00-input/instrucciones-vN.md` después de procesarlas (trazabilidad). |
| 4 | **Hook de notificación**: postergado | No se incluye en la fase 1. La actualización del INDICE la hace la propia skill de cierre. |
| 5 | **Numeración de casos**: anual reiniciado | Formato `{AAAA}-{NNN}-{slug-cliente}`. |

---

## 9. Sub-planes referenciados

Una vez aprobada la estructura, crear:

- `docs/plans/arquitectura-archivos/01-creacion-estructura-base.md`
- `docs/plans/arquitectura-archivos/02-scripts-abrir-cerrar-caso.md`
- `docs/plans/arquitectura-archivos/03-migracion-archivo-historico.md`
- `docs/plans/skill-editar-documento/00-investigacion-previa.md`
- `docs/plans/skill-editar-documento/01-diseño-skill.md`
