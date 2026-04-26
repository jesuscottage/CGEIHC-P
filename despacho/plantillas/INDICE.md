# Índice de plantillas

> Última actualización: 2026-04-25
>
> Catálogo de plantillas reutilizables del despacho. Para agregar una nueva,
> coloca el `.docx`/`.pdf` en esta carpeta (o en una subcarpeta si te ayuda
> a organizar), crea su `.meta.yml` hermano, y agrega una fila en la tabla
> de abajo.

---

## Plantillas disponibles

| ID | Archivo | Título | Tags | Última revisión |
|----|---------|--------|------|------------------|
| — | — | (sin plantillas todavía) | — | — |

---

## Cómo agregar una plantilla

1. Coloca el archivo en esta carpeta:
   ```
   plantillas/amparo-indirecto-v1.docx
   ```
2. Crea el `.meta.yml` hermano (copia y adapta `_templates/meta.yml.template`):
   ```
   plantillas/amparo-indirecto-v1.meta.yml
   ```
3. Agrega una fila en la tabla de arriba con un ID corto único.
4. Si la plantilla aún tiene datos de cliente embebidos, **no la
   versiones en git** — primero límpiala o sustituye los datos por
   marcadores como `{{NOMBRE_QUEJOSO}}`.

---

## Tags sugeridos

Sin imponer una taxonomía rígida, estos tags suelen ser útiles para
filtrar plantillas:

- Por materia: `amparo`, `civil`, `mercantil`, `laboral`, `familiar`,
  `penal`, `fiscal`, `administrativo`
- Por tipo de documento: `demanda`, `escrito`, `contrato`, `convenio`,
  `poder`, `acuerdo`, `recurso`
- Por etapa procesal: `inicial`, `respuesta`, `pruebas`, `alegatos`,
  `sentencia`, `apelación`

Usa los que tengan sentido para cada plantilla. La búsqueda se hará por
tags, no por carpetas.
