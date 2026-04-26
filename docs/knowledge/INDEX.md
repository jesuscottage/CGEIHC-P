# Knowledge Base Index / Índice de Conocimiento

> Last updated / Última actualización: 2026-04-25
> Query: `/consultar-conocimiento [topic]` or `/query-knowledge [topic]`
> The researcher agent keeps this index updated.

---

## Categories / Categorías

| Folder | Content |
|--------|---------|
| `competitors/` | Direct and indirect competitor analysis |
| `market/` | TAM, segments, pricing benchmarks, trends |
| `methodology/` | Frameworks, best practices, standards |
| `regulations/` | GDPR, local laws, compliance |
| `technology/` | Stack, libraries, architecture decisions |

---

## Documents / Documentos

| ID | Path | Title | Date | Description |
|----|------|-------|------|-------------|
| LA-001 | `methodology/lecciones-aprendidas-skills.md` | Lecciones aprendidas — mejoras generales para skills jurídicas | 2026-04-25 | Documento vivo. Se precarga en toda skill jurídica. Captura patrones de error generales y sugerencias acumuladas; nunca contiene datos de casos. |

---

## How to add documents / Cómo agregar documentos

1. Create a file in the appropriate category folder (e.g., `docs/knowledge/technology/my-topic.md`)
2. Add YAML frontmatter:
   ```yaml
   ---
   title: Document title
   date: YYYY-MM-DD
   category: competitors|market|methodology|regulations|technology
   tags: [tag1, tag2]
   status: draft|complete|outdated
   ---
   ```
3. Write the content
4. **Update this INDEX.md** — add a row to the table above
5. If using the researcher agent, it does steps 1-4 automatically
