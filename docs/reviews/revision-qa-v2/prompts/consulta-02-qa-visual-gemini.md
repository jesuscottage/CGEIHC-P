# Prompt para Gemini — Revisión del Plan QA Visual v2

## Rol

Eres un **QA Lead senior de aplicaciones 3D interactivas** con experiencia en:
- Testing visual de motores gráficos (Unity, Unreal, OpenGL custom engines)
- Detección de defectos de rendering (z-fighting, culling, UV mapping, iluminación)
- Diseño de test plans para presentaciones académicas/demos
- UX de aplicaciones 3D en primera persona

## Contexto Crítico

Este es un proyecto académico de Computación Gráfica (UNAM). Es un museo virtual 3D en C++/OpenGL 3.3 sobre calentamiento global en el Polo Norte. El usuario recorre el museo en primera persona (cámara FPS).

**PROBLEMA**: Se realizó una QA superficial que NO detectó defectos graves que un usuario humano ve de inmediato al ejecutar:
- El suelo no se renderiza correctamente (se ve el agua debajo)
- Las paredes parecen bloques grises sin contexto
- Los modelos 3D tienen escalas incorrectas
- No se entiende que es un "museo" — parece un espacio abstracto
- Los letreros son líneas diagonales que cruzan la pantalla

El agente Claude Code tiene visión multimodal (puede ver screenshots PNG) pero falló en la QA porque no comparó contra expectativas concretas.

## Archivos a Revisar

1. `docs/plans/expectativas-visuales.md` — Descripción detallada de lo que el usuario DEBE ver en cada posición del museo (11 puntos de control)
2. `docs/plans/plan-qa-v2.md` — Nuevo plan QA basado en comparar expectativas vs screenshots reales

También lee estos archivos para contexto técnico:
- `app/src/scene/Museum.h` — Geometría del museo (suelo, paredes, techo, plataformas)
- `app/src/scene/ModuleScene.h` — Modelos 3D y animaciones de los 7 módulos
- `app/src/main.cpp` — Render loop, orden de dibujado, configuración de shaders
- `app/shaders/standard.frag` — Shader de iluminación Blinn-Phong + Fresnel

## Preguntas a Evaluar

### (a) Completitud de las expectativas
- ¿Las 11 descripciones de PC cubren todo lo que un usuario vería al recorrer el museo?
- ¿Faltan posiciones de cámara importantes? (ej: mirando hacia abajo al suelo, mirando al techo, mirando hacia atrás, transiciones entre corredores)
- ¿Los detalles de cada PC son suficientes para detectar los bugs que el usuario reportó?

### (b) Robustez del plan QA
- ¿La metodología de comparar expectativa vs screenshot es suficiente?
- ¿El orden de prioridades es correcto?
- ¿Faltan pasos en el flujo de diagnóstico?
- ¿Cómo debería el agente verificar que algo "se ve bonito" y no solo "se renderiza"?

### (c) Expectativas faltantes
- ¿Qué ve el usuario al mirar DIRECTAMENTE al suelo (pitch=-90)?
- ¿Qué ve al mirar al techo?
- ¿Qué ve en las TRANSICIONES entre vestíbulo y corredores?
- ¿Qué ve al estar ENTRE dos módulos (no dentro del radio de ninguno)?
- ¿Qué ve al final del corredor antes de entrar a la sala M5?

### (d) Diagnóstico
- ¿Las 4 técnicas de aislamiento son suficientes?
- ¿Qué técnicas adicionales sugeriría un QA de gráficos 3D?
- ¿Cómo detectaría el agente problemas de UVs, normales invertidas, o blend order?

## Formato de Respuesta

```markdown
## Evaluación General
[¿El plan QA v2 detectaría los bugs que el usuario reportó? Sí/No + razón]

## Expectativas Faltantes (críticas)
| # | Posición/Situación | Qué debería describirse | Por qué importa |
|---|-------------------|------------------------|----------------|

## Mejoras al Plan QA
| # | Mejora | Impacto | Implementación |
|---|--------|---------|----------------|

## Técnicas de Diagnóstico Adicionales
[Lista de técnicas que faltan]

## Preguntas de "Sensibilidad Estética"
[Cómo puede un agente sin gustos humanos evaluar si algo es "bonito"]

## Top 5 Recomendaciones
1. ...
```
