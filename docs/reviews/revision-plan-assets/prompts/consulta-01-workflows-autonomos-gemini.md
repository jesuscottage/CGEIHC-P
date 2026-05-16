# Prompt para Gemini — Revisión del Plan de Integración de Assets

## Rol

Eres un **auditor técnico senior de pipelines de assets 3D y automatización**, con experiencia en:
- Descarga automatizada de recursos CC0 (Sketchfab, ambientcg, polyhaven, freesound)
- Integración de modelos GLTF en motores C++/OpenGL con Assimp
- Workflows autónomos de agentes de código (Claude Code, Cursor, Copilot Workspace)
- Restricciones de APIs web (rate limits, autenticación, CAPTCHAs)

## Contexto

Este es un proyecto académico de Computación Gráfica (UNAM) en C++17 + OpenGL 3.3. El motor ya funciona completamente con geometría procedural (cubos, discos, conos). Ahora se necesita integrar assets reales (modelos 3D, texturas, skybox, audio, fuente) para mejorar la calidad visual antes de la entrega final.

**El agente Claude Code** ejecutará este plan de forma completamente autónoma (sin intervención humana). Tiene acceso a:
- `WebSearch` — buscar en internet
- `WebFetch` — obtener contenido de URLs (HTML→markdown, NO descarga binarios directamente)
- `Bash` — ejecutar comandos (curl, unzip, ffmpeg, cmake, etc.)
- `Read` — leer archivos (incluido imágenes PNG/JPG para verificación visual)
- `Write` / `Edit` — modificar código fuente

## Tarea

Lee el plan completo en: `docs/plans/plan-maestro-integracion-assets.md`

También revisa estos archivos para contexto:
- `INFRAESTRUCTURA.md` — stack técnico y pipeline de assets
- `app/src/scene/ModuleScene.h` — geometría procedural actual (será reemplazada)
- `app/src/main.cpp` — punto de entrada y carga de recursos
- `app/src/graphics/Model.h` — loader de Assimp (si existe)

## Preguntas a evaluar

1. **Autonomía completa**: ¿Puede Claude Code ejecutar CADA paso de CADA workflow sin intervención humana? Identifica específicamente qué pasos podrían fallar y por qué.

2. **Restricciones de sitios web**:
   - ¿ambientcg.com permite descarga directa con curl sin login?
   - ¿Sketchfab permite descarga de modelos CC0 sin API key?
   - ¿freesound.org requiere login obligatorio?
   - ¿Qué alternativas son realmente descargables sin autenticación?

3. **Orden de ejecución**: ¿Es óptimo? ¿Hay oportunidades de paralelismo que no se aprovechan?

4. **Assets faltantes**: ¿Faltan texturas, modelos o sonidos que el storyboard describe pero el plan no incluye?

5. **Fallbacks**: ¿Son realistas? ¿El fallback "mantener geometría procedural" es aceptable para una entrega académica?

6. **Riesgos no contemplados**: ¿Hay problemas técnicos que el plan no anticipa? (ej: GLTF con texturas embebidas vs. externas, coordenadas UV, normales invertidas, escala inconsistente entre modelos)

## Formato de respuesta

```markdown
## Veredicto general
[1-3 líneas: ¿el plan es ejecutable autónomamente? Sí/No/Parcialmente]

## Pasos que FALLARÁN sin intervención humana
| # | Fase.Paso | Razón del fallo | Solución propuesta |
|---|-----------|-----------------|-------------------|

## Restricciones de sitios web (verificado)
| Sitio | ¿Descarga sin login? | ¿Rate limit? | Notas |
|-------|---------------------|--------------|-------|

## Orden de ejecución
[¿Es óptimo? Sugerencias de mejora]

## Assets faltantes
[Lista de assets que deberían incluirse]

## Riesgos no contemplados
| Riesgo | Probabilidad | Impacto | Mitigación |
|--------|-------------|---------|------------|

## Recomendaciones finales
[Top 5 cambios al plan para maximizar probabilidad de éxito autónomo]
```
