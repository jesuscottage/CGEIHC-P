# Consulta 01 — Auditoría del Plan Maestro de Desarrollo (Gemini)

> Proyecto CGEIHC-P — Calentamiento global en el polo norte
> Tipo: Auditoría de plan de implementación C++/OpenGL
> Interfaz: Gemini Antigravity (tienes acceso completo al repositorio)

---

## Tu rol

Eres un **arquitecto senior de software especializado en motores gráficos en tiempo real** con amplia experiencia en proyectos C++17 + OpenGL 3.3 y en el desarrollo asistido por agentes de IA. Tu tarea es auditar un plan de desarrollo de 12 fases para un proyecto académico universitario.

---

## Contexto del proyecto

Museo virtual educativo 3D en el Polo Norte sobre calentamiento global. Proyecto final universitario (UNAM, Computación Gráfica e Interacción Humano-Computadora, semestre 2026-2).

**Particularidad clave**: el plan está diseñado para ser ejecutado en su totalidad por **Claude Code** de forma autónoma, incluyendo codificación, verificación visual de resultados 3D (a través de screenshots), iteración y corrección de errores. La simpleza y la viabilidad autónoma son criterios tanto o más importantes que la sofisticación técnica.

---

## Archivos a revisar

Por favor, lee los siguientes archivos del proyecto antes de responder:

### Plan a auditar
- `docs/plans/plan-maestro-desarrollo.md` — **documento principal a auditar**

### Contexto del proyecto
- `INFRAESTRUCTURA.md` — stack técnico completo y justificaciones
- `blueprints/09-orden-desarrollo.md` — orden de desarrollo previo (base del plan)

### Especificaciones del proyecto
- `blueprints/01-vision-narrativa.md` — narrativa, módulos y textos
- `blueprints/02-layout-escenario.md` — layout del museo en T invertida, dimensiones
- `blueprints/03-catalogo-modelos.md` — modelos 3D, fauna, letreros, skybox
- `blueprints/04-interaccion-controles.md` — controles FPS, triggers, AppState
- `blueprints/05-materiales-texturas.md` — materiales, shaders, texturas
- `blueprints/06-animaciones.md` — 3 tipos de animación (básica/keyframe/procedural)
- `blueprints/07-audio.md` — audio con miniaudio (header-only)
- `blueprints/08-hud-ui.md` — ImGui HUD, pantallas de título y cierre

---

## Preguntas de auditoría

Responde las siguientes preguntas **en orden**, siendo específico y concreto:

### 1. Orden de fases — ¿Es óptimo?

Analiza las 12 fases (0-11) del plan maestro. ¿El orden minimiza retrabajos y bloqueos? ¿Hay alguna tarea que debería estar en una fase diferente? ¿Alguna fase está demasiado cargada o demasiado vacía?

Sé específico: señala la fase y la tarea concreta si hay un problema.

### 2. Pipeline de verificación visual autónoma — ¿Es robusto?

El plan propone usar `glReadPixels + stb_image_write` para que Claude capture screenshots automáticamente al frame 120 y los lea con la herramienta Read. Evalúa:
- ¿Es suficiente para que un agente detecte errores visuales 3D (geometría, texturas, iluminación, posición)?
- ¿Hay casos de error visual que un screenshot en el frame 120 no capturaría?
- ¿Hay herramientas o técnicas adicionales que deberían añadirse al pipeline?
- ¿El protocolo de "3 iteraciones → error diferido" es razonable?

### 3. Tareas faltantes o en la fase incorrecta

¿Hay tareas críticas que el plan no menciona? ¿Hay tareas que están en la fase incorrecta y podrían causar retrabajos? ¿Hay tareas sobredimensionadas que podrían simplificarse?

### 4. Criterios de salida — ¿Son verificables autónomamente?

Para cada fase, el plan define un "criterio de salida" con checkboxes. Un agente de IA debe poder verificar si el criterio está cumplido. ¿Los criterios son suficientemente específicos? ¿Algún criterio es ambiguo o no verificable sin intervención humana?

### 5. Gestión de riesgos — ¿Hay riesgos no cubiertos?

El plan cubre 5 riesgos técnicos con mitigaciones. ¿Hay riesgos importantes que el plan no menciona? ¿Alguna mitigación propuesta es incorrecta o insuficiente para el stack dado (C++17 + OpenGL 3.3 + Assimp + miniaudio)?

---

## Restricciones del proyecto (importantes para la auditoría)

- **Proyecto académico**: complejidad acotada. No implementar: ECS, sombras dinámicas, PBR completo, multithreading, deferred rendering, IBL, normal mapping complejo.
- **Solo 2 niveles de herencia**: `ModuloBase → ModuloEspecifico`
- **Ejecución autónoma por Claude Code**: toda tarea debe ser implementable sin explicaciones adicionales al agente
- **Plataforma**: Windows 11 + Visual Studio 2022 Community + CMake 3.20+
- **Stack fijo**: C++17 + OpenGL 3.3 Core + GLFW 3.4 + GLM 1.0.1 + Assimp v5.4.3 + ImGui v1.91.6 + miniaudio 0.11+ + stb headers

---

## Formato de respuesta

Responde en español. Para cada pregunta:

```
### [Número y título de la pregunta]

**Veredicto**: [Aceptable / Correcciones menores / Correcciones mayores]

**Análisis**: [tu análisis]

**Correcciones sugeridas** (si aplica):
- [corrección 1]
- [corrección 2]
```

Al final, un **Resumen ejecutivo** con:
- Las 3-5 correcciones más importantes (ordenadas por impacto)
- Un veredicto global del plan (listo para implementar / necesita ajustes menores / necesita revisión mayor)

---

## Instrucción de entrega

Deposita tu respuesta directamente en el archivo:
```
docs/reviews/auditoria-plan-desarrollo/reportes/consulta-01-auditoria-plan-gemini.md
```
