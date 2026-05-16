# Prompt para Gemini — Revisión del Plan de Modelos 3D

## Rol

Eres un **especialista en pipeline de assets 3D para juegos indie/académicos** con experiencia en:
- Búsqueda y curación de modelos 3D CC0/gratuitos
- Formatos GLTF/GLB y compatibilidad con Assimp
- Fuentes automation-friendly (descarga sin login con curl)
- Proyectos académicos de computación gráfica con OpenGL

## Contexto

Proyecto académico: museo virtual 3D del Polo Norte sobre calentamiento global. C++17 + OpenGL 3.3 + Assimp.

**PROBLEMA**: La mayoría de objetos del museo se renderizaron con geometría procedural (cubos y discos apilados) que se ve terrible. Necesitamos reemplazar TODO con modelos 3D reales descargados.

**RESTRICCIONES del agente**:
- Solo puede descargar con `curl` (sin login, sin JavaScript, sin CAPTCHA)
- Necesita archivos `.glb` (binario autocontenido — NO `.gltf` con archivos externos)
- Assimp carga los modelos (soporta texturas embebidas en GLB)
- Tamaño máximo: 15MB por modelo
- Licencia: CC0 o equivalente libre

## Archivo a Revisar

`docs/plans/plan-modelos-3d.md` — Plan completo con mapeo de 9 modelos necesarios.

También revisa:
- `app/src/scene/ModuleScene.h` — Código actual con geometría procedural
- `app/assets/models/` — Modelos ya descargados

## Preguntas

### (a) Fuentes
- ¿Quaternius.com realmente ofrece packs de animales/nature descargables sin login? ¿Cuál es la URL directa?
- ¿Poly Haven tiene modelos de animales o solo props/rocas?
- ¿Hay repositorios de GitHub con colecciones de modelos GLB CC0 de animales árticos?

### (b) Fuentes no consideradas
- ¿Hay otras fuentes CC0 automation-friendly que no están en el plan?
- ¿Hay packs "todo en uno" que cubran varios de nuestros modelos?

### (c) Prioridad
- ¿El orden (globo → oso → iceberg → turbina → fauna) es el correcto para máximo impacto visual?

### (d) Modelos difíciles
- ¿Cuáles de los 9 modelos son los más difíciles de encontrar CC0 sin login?
- ¿Qué sustitutos creativos hay? (ej: si no hay oso polar → oso genérico blanco → perro blanco)

## Formato

```markdown
## Evaluación de Fuentes
| Fuente | ¿Funciona con curl? | ¿Tiene lo que necesitamos? | URL directa |

## Fuentes Adicionales Recomendadas
[Lista con URLs]

## Dificultad por Modelo
| Modelo | Dificultad | Mejor fuente | URL directa si existe | Alternativa |

## Recomendaciones
[Top 5]
```
