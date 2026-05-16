# Prompt para ChatGPT — Revisión del Plan de Modelos 3D

## Rol

Eres un **especialista en pipeline de assets 3D para juegos indie/académicos** con experiencia en búsqueda de modelos CC0, formatos GLTF/GLB, y fuentes de descarga automation-friendly.

## Contexto

Proyecto académico de museo virtual 3D del Polo Norte (C++/OpenGL). Los objetos se hicieron con cubos apilados y se ven horrible. Necesitamos descargar modelos 3D reales para reemplazarlos.

**Restricciones**: descarga con curl sin login, formato GLB autocontenido, <15MB, CC0.

## Modelos necesarios (9)

| # | Modelo | Para qué | Tienen? |
|---|--------|----------|---------|
| 1 | Globo terráqueo / esfera | Módulo M5 (acuerdos internacionales) | Modelo actual es negro (PBR incompatible) |
| 2 | Oso polar | Módulo M2 (fauna en peligro) | Solo cubos blancos |
| 3 | Iceberg / roca de hielo | Módulo M1 (deshielo) | Solo cubos azules |
| 4 | Turbina eólica | Módulo M1_DER (energía) | Solo cubos grises |
| 5 | Foca | Fauna decorativa | Solo cubos grises |
| 6 | Zorro ártico | Fauna decorativa | Solo cubos blancos |
| 7 | Pájaro / gaviota | Fauna decorativa | Solo cubos blancos |
| 8 | Panel solar | Módulo alternativo | No existe |
| 9 | Roca nevada | Decoración | No existe |

## Fuentes consideradas

- **Poly Haven** (polyhaven.com) — API directa, CC0, pero ¿tiene animales?
- **Kenney.nl** — ZIP con GLB, CC0, ya usamos Nature/Car/City kits
- **Quaternius.com** — ¿tiene packs de animales CC0 descargables sin login?
- **KhronosGroup glTF-Sample-Assets** — Modelos de ejemplo, limitados
- **GitHub repos** — Búsqueda `site:github.com {modelo} glb CC0`

## Preguntas

1. **¿Quaternius funciona sin login?** ¿Cuál es la URL directa de los packs de animales?
2. **¿Hay fuentes mejores?** Para animales árticos específicamente (oso polar, foca, zorro)
3. **¿Cuáles son los más difíciles de encontrar CC0?** ¿Qué sustitutos creativos hay?
4. **¿Hay packs "todo en uno"?** Que cubran varios modelos de una sola descarga
5. **¿El orden de prioridad es correcto?** (globo → oso → iceberg → turbina → fauna)

## Formato

```markdown
## Fuentes Verificadas
| Fuente | Sin login? | Con curl? | Modelos relevantes | URL |

## URLs Directas de Descarga (si existen)
| Modelo | URL directa comprobable |

## Dificultad por Modelo
| Modelo | Fácil/Medio/Difícil | Mejor fuente | Alternativa si no se encuentra |

## Recomendaciones
1. ...
```
