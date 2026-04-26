# Despacho — guía rápida de operación

Esta carpeta contiene todos los documentos del despacho: las **plantillas**
reutilizables, los **casos** activos, los **entregables** finalizados y el
**archivo histórico** de documentos antiguos.

> **Antes que nada**: nunca se versiona en git ningún documento con datos
> reales de clientes. Sólo se versionan plantillas (sin PII), índices y
> este README.

---

## Estructura

```
despacho/
├── README.md            ← este archivo
├── plantillas/          ← documentos reutilizables (organización libre)
├── casos/               ← un caso por carpeta — se ignora en git
├── entregables/         ← copia plana de todos los finales — se ignora en git
├── archivo-historico/   ← documentos antiguos a destilar — se ignora en git
├── _trabajo/            ← carpeta scratch (OCR, conversiones) — se ignora
└── _templates/          ← esqueletos de caso.yml y meta.yml
```

---

## Operación día a día

### Para empezar un caso nuevo

Pídeselo a Claude tal cual:

> "Abre un caso nuevo para Juan García López, materia mercantil, usando la
> plantilla `arrendamiento-comercial-v2`."

Claude:
1. Asigna el siguiente correlativo del año (`2026-001`, `2026-002`, ...).
2. Crea la carpeta `casos/2026-NNN-garcia-lopez/` con la subestructura.
3. Copia la plantilla a `01-plantilla/`.
4. Te confirma el ID del caso para usarlo después.

### Para editar el documento de un caso

Habla con Claude en lenguaje natural:

> "En el caso `2026-001`, cambia el nombre del arrendatario a Juan García
> López, actualiza la fecha al 25 de abril de 2026, y agrega una cláusula
> de rescisión por mora superior a 30 días."

Claude:
1. Lee el contexto necesario y el último borrador.
2. Genera el siguiente borrador en `02-borradores/v{N+1}-...docx`.
3. Genera un PDF con Word.
4. Te entrega un resumen de cambios y las rutas de los archivos.

### Para iterar (varias rondas de revisión)

Sólo dile qué corregir:

> "Sustituye 'arrendador' por 'propietario' en todo el documento y agrega
> el RFC del arrendatario."

Cada iteración crea un nuevo `v{N}-...docx` sin tocar los anteriores.

### Para cerrar un caso

> "Cierra el caso `2026-001`, este es el final."

Claude:
1. Copia el último borrador a `04-final/final.docx`.
2. Genera `04-final/final.pdf`.
3. Publica copia en `entregables/2026/04/2026-001-final.docx`.
4. Actualiza `entregables/INDICE.md`.

---

## Convenciones de nombres

| Elemento | Patrón | Ejemplo |
|---|---|---|
| ID de caso | `{AAAA}-{NNN}-{slug-cliente}` | `2026-001-garcia-lopez` |
| Plantilla | `{descripcion}-v{N}.{ext}` | `amparo-indirecto-v1.docx` |
| Borrador | `v{N}-{AAAA-MM-DD}-{HHMM}.docx` | `v3-2026-04-25-1635.docx` |
| Final por caso | `04-final/final.{docx,pdf}` | `final.docx`, `final.pdf` |
| Entregable publicado | `{caso-id}-final.{docx,pdf}` | `2026-001-garcia-lopez-final.pdf` |

---

## Estructura de una carpeta de caso

```
casos/2026-001-garcia-lopez/
├── caso.yml              ← metadatos: cliente, plantilla, fechas
├── 00-input/
│   ├── instrucciones-v1.md   ← lo que pediste por chat para v1
│   ├── instrucciones-v2.md
│   └── adjuntos/             ← documentos del cliente (constancias, etc.)
├── 01-plantilla/
│   └── plantilla-origen.docx ← copia de la plantilla, NO editar aquí
├── 02-borradores/
│   ├── v1-2026-04-25-1430.docx
│   ├── v1-2026-04-25-1430.diff.md
│   ├── v2-2026-04-25-1530.docx
│   └── v2-2026-04-25-1530.diff.md
└── 04-final/                 ← sólo aparece al cerrar el caso
    ├── final.docx
    └── final.pdf
```

---

## Plantillas y archivo histórico

### `plantillas/`

Documentos modelo del abogado — se versionan en git porque son **propiedad
intelectual del despacho** y no contienen datos de clientes específicos
(o, si los contenían, ya fueron sustituidos por marcadores).

Para agregar una plantilla nueva:
1. Coloca el `.docx` en `plantillas/` (puedes usar subcarpetas si te ayuda
   a organizar; la estructura interna es libre).
2. Crea el `.meta.yml` hermano (ver [`_templates/meta.yml.template`](_templates/meta.yml.template)).
3. Agrega una fila a [`plantillas/INDICE.md`](plantillas/INDICE.md).

### `archivo-historico/`

Documentos antiguos del abogado que **podrían convertirse en plantillas**
después de limpiar PII. Mientras estén en `archivo-historico/` no son
plantillas oficiales — son materia prima.

---

## Privacidad

- `casos/`, `entregables/`, `archivo-historico/`, `_trabajo/` están en
  `.gitignore`. Nunca se suben a git.
- Sólo se versionan: `README.md`, los tres `INDICE.md`, `_templates/`,
  `plantillas/` (las plantillas en sí, sin datos de cliente).
- Si una plantilla aún tiene datos de cliente embebidos, **no se versiona**
  hasta limpiarla.

---

## Referencias

- Plan maestro: [`docs/plans/plan-maestro-arquitectura-archivos.md`](../docs/plans/plan-maestro-arquitectura-archivos.md)
- Lecciones aprendidas: [`docs/knowledge/methodology/lecciones-aprendidas-skills.md`](../docs/knowledge/methodology/lecciones-aprendidas-skills.md)
- Herramientas instaladas: [`setup/herramientas-instaladas.md`](../setup/herramientas-instaladas.md)
