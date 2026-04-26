# Índice del archivo histórico

> Última actualización: 2026-04-25
>
> Catálogo de documentos antiguos del abogado, anteriores al uso de las
> skills. Estos documentos pueden servir como **fuente para destilar
> nuevas plantillas** una vez que se les retire la información del cliente
> original.

---

## Documentos históricos

| Archivo | Descripción | Año | Materia | Posible plantilla |
|---------|-------------|-----|---------|--------------------|
| — | (sin documentos catalogados todavía) | — | — | — |

---

## Cómo agregar un documento histórico

1. Coloca el archivo en esta carpeta. Puedes agruparlo por año, materia
   o como te resulte más cómodo — la estructura interna es libre.
2. Agrega una fila a la tabla de arriba.
3. Si crees que sirve como base para una plantilla, márcalo en la columna
   "Posible plantilla" con `sí — pendiente` o el nombre que tendría.

---

## De documento histórico a plantilla

Cuando un documento histórico va a convertirse en plantilla:

1. Cópialo a `despacho/plantillas/`.
2. Sustituye los datos del cliente original por marcadores
   (p. ej. `{{NOMBRE_DEMANDANTE}}`, `{{FECHA}}`).
3. Crea el `.meta.yml` correspondiente y regístralo en
   `plantillas/INDICE.md`.
4. Marca la fila en este índice como "→ plantilla `nombre-v1`".

> **Importante**: este directorio se ignora en git porque puede contener
> datos de clientes reales. Las plantillas limpias sí se versionan.
