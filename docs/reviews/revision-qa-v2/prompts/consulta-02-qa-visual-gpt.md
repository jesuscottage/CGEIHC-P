# Prompt para ChatGPT — Revisión del Plan QA Visual v2

## Rol

Eres un **QA Lead senior de aplicaciones 3D interactivas** con experiencia en testing visual de motores gráficos, detección de defectos de rendering, y UX de aplicaciones 3D en primera persona.

## Contexto

Proyecto académico: museo virtual 3D en C++/OpenGL 3.3 sobre calentamiento global en el Polo Norte. El usuario recorre en primera persona con cámara FPS (WASD + mouse).

**PROBLEMA GRAVE**: Se hizo una QA que pasó todos los tests, pero cuando el usuario ejecutó la app vio:
- Suelo invisible (se veía el agua debajo en vez del piso del museo)
- Paredes como bloques grises abstractos sin contexto
- Modelos 3D con escalas incorrectas (edificios gigantes, autos invisibles)
- Letreros como líneas diagonales cruzando la pantalla
- No se entendía que era un "museo"

El agente QA (Claude Code) tiene visión multimodal — puede ver screenshots PNG — pero su QA fue superficial: tomó screenshots, los miró, y dijo "OK" sin comparar contra lo que DEBERÍA verse.

## Documentos a Revisar

### Documento 1: Expectativas Visuales (11 puntos de control)

Resume cada punto de control del museo:

**PC-01 Título**: Fondo oscuro + panel ImGui con título/instrucciones
**PC-02 Vestíbulo**: Suelo de concreto texturizado + paredes altas + techo + corredores al frente + nieve
**PC-03 Corredor Izq**: Suelo continuo + paredes laterales + plataforma de hielo + iceberg + letreros
**PC-04 M1_IZQ animando**: Iceberg encogiéndose + color cambiando + barra progreso
**PC-05 M2_IZQ**: Disco de hielo + oso polar + foca cercana
**PC-06 M3_IZQ**: 3 edificios GLB reconocibles + agua subiendo
**PC-07 M1_DER**: Turbina eólica visible (~6m) + palas girando
**PC-08 M2_DER**: Auto GLB reconocible + movimiento sinusoidal
**PC-09 M3_DER**: Árbol GLB creciendo de semilla a completo
**PC-10 M5**: Globo azul/verde girando + líneas de acuerdos
**PC-11 Exterior**: Océano con olas + skybox nublado + Fresnel

Reglas generales: suelo SIEMPRE visible, nada completamente negro, letreros discretos, agua solo fuera del museo.

### Documento 2: Plan QA v2

Metodología por PC: PREPARAR → CAPTURAR → COMPARAR → DIAGNOSTICAR → CORREGIR → ITERAR

Técnicas de diagnóstico:
1. Desactivar cull face (detectar normales invertidas)
2. Forzar color rojo (aislar problemas de textura vs geometría)
3. Cámara extrema (vista cenital para verificar posiciones)
4. Escala ×10 (localizar componentes invisibles)

## Preguntas

### (a) ¿Las expectativas detectarían los bugs que el usuario reportó?
- Suelo invisible → ¿PC-02 lo detectaría?
- Modelos mal escalados → ¿PC-06/PC-08 lo detectarían?
- "No se entiende que es museo" → ¿hay algún PC que evalúe esto?

### (b) ¿Qué le falta al plan?
- ¿Hay posiciones de cámara que faltan? (mirar al suelo, al techo, girar 360°)
- ¿El agente debería tomar screenshots rotando la cámara 45° en cada dirección?
- ¿Debería haber un "test de primer frame" donde se evalúa la primera impresión?

### (c) ¿Cómo puede un agente sin gustos humanos evaluar "bonito"?
- ¿Qué métricas objetivas puede usar? (cobertura de color, contraste, filling ratio)
- ¿Debería comparar contra imágenes de referencia de otros museos 3D?

### (d) ¿Qué técnicas de diagnóstico faltan?
- ¿Wireframe rendering para ver la geometría real?
- ¿Depth buffer visualization?
- ¿Normal visualization (colorear pixeles según la normal)?

## Formato de Respuesta

```markdown
## Evaluación
[¿El plan v2 habría detectado los bugs del usuario? Sí/No + gaps]

## Expectativas Faltantes
| # | Qué falta | Por qué es crítico |

## Mejoras al Plan QA
| # | Mejora | Cómo implementar |

## Cómo Evaluar "Bonito" Objetivamente
[Métricas + técnicas]

## Técnicas de Diagnóstico Adicionales
[Lista]

## Top 5 Cambios para Plan Infalible
1. ...
```
