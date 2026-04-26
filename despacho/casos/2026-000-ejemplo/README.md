# Caso 2026-000 — Ejemplo de referencia

Este NO es un caso real. Es un esqueleto que muestra la estructura completa
de una carpeta de caso, con archivos de ejemplo que ilustran qué va en
cada subcarpeta.

> El correlativo `000` está reservado para este ejemplo. Los casos reales
> empiezan en `2026-001`.

## Cómo se ve cuando está poblado

```
2026-000-ejemplo/
├── caso.yml                          ← metadatos
├── 00-input/
│   ├── instrucciones-v1.md           ← lo que el abogado pidió por chat para v1
│   ├── instrucciones-v2.md           ← lo que pidió para v2
│   └── adjuntos/                     ← documentos del cliente
├── 01-plantilla/
│   └── plantilla-origen.docx         ← copia de la plantilla, NO editar
├── 02-borradores/
│   ├── v1-2026-04-25-1430.docx
│   ├── v1-2026-04-25-1430.diff.md
│   ├── v2-2026-04-25-1530.docx
│   └── v2-2026-04-25-1530.diff.md
└── 04-final/                         ← sólo aparece al cerrar el caso
    ├── final.docx
    └── final.pdf
```

## Reglas

- **`01-plantilla/` no se toca**: la plantilla de origen queda como
  evidencia de qué se usó como base.
- **Cada borrador es inmutable**: los borradores numerados (`v1`, `v2`,
  `v3`...) nunca se editan; cada cambio crea un `v{N+1}`.
- **`04-final/` sólo existe al cerrar el caso**: es la versión definitiva.
- **Cierre del caso = publicación en `entregables/`**: la skill copia la
  versión final a `despacho/entregables/{AAAA}/{MM}/`.
