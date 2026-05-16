---
name: consultar-modelos
description: Consultar a Gemini y ChatGPT como revisores externos para decisiones de arquitectura, revisiones técnicas o análisis. 3 modos según el contexto. Principio: los modelos analizan y opinan. Claude sintetiza, identifica falsos positivos, decide y ejecuta.
allowed-tools: Read, Write, Edit, Bash, Glob, Grep
---

# Skill: /consultar-modelos

> Consultar a Gemini y ChatGPT como revisores externos para decisiones, revisiones técnicas, verificaciones o análisis en el proyecto CGEIHC-P.
> **3 modos** según la profundidad del contexto a enviar.
> **Principio**: los modelos analizan y opinan. Claude sintetiza, identifica falsos positivos, decide y ejecuta.

## Uso

```
/consultar-modelos [tema o pregunta]           # → Claude elige el modo
/consultar-modelos [tema] modo-a               # → Solo Gemini (contexto completo)
/consultar-modelos [tema] modo-b               # → Gemini (completo) + ChatGPT (enfocado)
/consultar-modelos [tema] solo-gemini          # → Solo Gemini
/consultar-modelos [tema] solo-gpt             # → Solo ChatGPT
```

**Ejemplos:**
```
/consultar-modelos Plan de implementación OpenGL modo-b
/consultar-modelos ¿Cómo estructurar el sistema de módulos en C++?
/consultar-modelos Revisar arquitectura de shaders modo-b
/consultar-modelos Riesgos técnicos del stack Assimp+GLTF solo-gemini
```

---

## Modos

### Modo A — Solo Gemini (con acceso al proyecto)

| Modelo | Interfaz | Acceso | Prompt |
|--------|----------|--------|--------|
| **Gemini** | Antigravity UI | Completo al proyecto — lee archivos directamente | Paths a investigar |

- Un solo prompt con la lista de rutas a investigar.
- Gemini tiene acceso completo al repo — se le anima a explorar los archivos por su cuenta.
- El usuario pega la respuesta en el placeholder.

### Modo B — Gemini (proyecto) + ChatGPT (web, autocontenido)

| Modelo | Interfaz | Acceso | Prompt |
|--------|----------|--------|--------|
| **Gemini** | Antigravity UI | Completo al proyecto — lee archivos directamente | Paths a investigar |
| **ChatGPT** | chatgpt.com | Sin acceso al repo — necesita todo embebido | Autocontenido conciso (~128K tokens) |

- **Dos prompts separados**: Gemini recibe paths de archivos; ChatGPT recibe el contexto clave embebido.
- **Dos placeholders** para los reportes.
- Gemini deposita su reporte directamente en el archivo de salida indicado.
- El usuario copia la respuesta de ChatGPT y la pega en su placeholder.

### Modo C — Solo ChatGPT (enfocado)

| Modelo | Interfaz | Contexto |
|--------|----------|----------|
| **ChatGPT** | chatgpt.com | Enfocado — solo lo esencial |

---

## Flujo completo

### Paso 1 — Entender la consulta

Identificar:
- **Tema/pregunta central**: ¿Qué se quiere resolver o evaluar?
- **Tipo de consulta**: Decisión técnica | Revisión de código | Análisis de arquitectura | Plan de implementación | Riesgos | Otro
- **Modo**: A, B o C (si no se indica, elegir según contexto — por defecto Modo B)
- **Rol ideal**: ver tabla de roles

### Paso 2 — Recopilar contexto

Según el tipo de consulta, leer los archivos relevantes del proyecto:
- Blueprints (`blueprints/`)
- Base de conocimiento (`docs/knowledge/`)
- Planes (`docs/plans/`)
- Stack técnico (`INFRAESTRUCTURA.md`)
- Especificaciones (`docs/knowledge/technology/especificaciones-proyecto.md`)

**No sobrecargar**: solo lo que el modelo necesita para dar una respuesta informada.
**Gemini**: puede recibir todos los blueprints completos.
**ChatGPT**: recibe resúmenes y los fragmentos más relevantes.

### Paso 3 — Determinar carpeta de destino

**Si ya existe una revisión relacionada**, reutilizar su carpeta:
```
docs/reviews/{revisión-existente}/prompts/consulta-NN-tema.md
```

**Si es una consulta nueva**, crear estructura:
```
docs/reviews/{nombre-consulta}/
├── prompts/
│   ├── consulta-NN-tema-gemini.md       # Prompt para Gemini (completo)
│   └── consulta-NN-tema-gpt.md          # Prompt para ChatGPT (enfocado)
└── reportes/
    ├── consulta-NN-tema-gemini.md       # Placeholder reporte Gemini
    └── consulta-NN-tema-gpt.md          # Placeholder reporte ChatGPT
```

### Paso 4 — Generar prompts

Usar las plantillas:
- Gemini: [plantilla-prompt-modo-b-gemini.md](plantilla-prompt-modo-b-gemini.md)
- ChatGPT: [plantilla-prompt-modo-b-gpt.md](plantilla-prompt-modo-b-gpt.md)

**Checklist antes de mostrar al usuario:**
- [ ] Rol asignado (específico, no genérico)
- [ ] Contexto mínimo necesario (situación actual, decisiones previas)
- [ ] Tarea clara y acotada
- [ ] Restricciones explícitas
- [ ] Formato de respuesta definido
- [ ] Placeholders creados en `reportes/`

### Paso 5 — Crear placeholders e instrucciones al usuario

**SIEMPRE crear ambos archivos placeholder** antes de mostrar instrucciones:

```markdown
<!-- Pegar aquí la respuesta del modelo a la consulta NN -->
```

**Instrucciones Modo B:**
```
Dos prompts preparados.

Gemini:   docs/reviews/{nombre}/prompts/consulta-NN-tema-gemini.md
ChatGPT:  docs/reviews/{nombre}/prompts/consulta-NN-tema-gpt.md

Instrucciones:
1. Copia el prompt de Gemini → pégalo en gemini.google.com (usa Gemini 2.5 Pro)
   → Copia la respuesta y pégala en: docs/reviews/{nombre}/reportes/consulta-NN-tema-gemini.md
2. Copia el prompt de ChatGPT → pégalo en chatgpt.com (usa GPT-4o)
   → Copia la respuesta y pégala en: docs/reviews/{nombre}/reportes/consulta-NN-tema-gpt.md
3. Avísame cuando estén listas
```

---

## Procesamiento de respuestas

Cuando el usuario avisa que las respuestas están listas:

### Paso 6 — Leer, sintetizar e identificar falsos positivos

1. **Extraer lo valioso**: insights genuinos, datos concretos, riesgos no considerados
2. **Descartar ruido**: recomendaciones genéricas, repeticiones del contexto, obviedades
3. **Contrastar con el proyecto**: ¿la recomendación aplica dado nuestro stack C++/OpenGL, restricciones académicas y estado actual?
4. **Identificar falsos positivos**: para cada hallazgo, verificar si aplica al proyecto real. Clasificar como genuino o FP con justificación.
5. **Si se consultó a ambos**: cruzar respuestas, identificar coincidencias y discrepancias

### Paso 7 — Presentar síntesis y plan

```
## Síntesis de la consulta

[Resumen de 3-5 líneas de lo más valioso]

### Hallazgos genuinos

| # | Hallazgo | Fuente | Acción |
|---|----------|--------|--------|
| G1 | [descripción] | Gemini/GPT/ambos | [fix concreto] |

### Falsos positivos

| # | Hallazgo | Fuente | Por qué es FP |
|---|----------|--------|---------------|
| FP1 | [descripción] | Gemini/GPT | [justificación] |

## Plan de acción

1. [Acción 1] — Claude puede ejecutar
2. [Acción 2] — Requiere decisión del usuario
...

¿Procedo?
```

### Paso 8 — Ejecutar (si el usuario acepta)

Aplicar los cambios aprobados, actualizar blueprints si corresponde.

---

## Roles comunes para CGEIHC-P

| Tipo de consulta | Rol sugerido |
|---|---|
| Arquitectura del motor gráfico | Arquitecto de software senior especializado en gráficos por computadora |
| Plan de implementación OpenGL | Ingeniero senior de gráficos en tiempo real (C++/OpenGL) |
| Revisión de shaders GLSL | Especialista en shaders y pipeline gráfico OpenGL 3.3 |
| Decisión de estructura C++ | Ingeniero senior C++17 con experiencia en proyectos académicos |
| Audio OpenAL | Especialista en audio para aplicaciones interactivas (OpenAL/FMOD) |
| CMake/build systems | Especialista en build systems C++ (CMake, vcpkg, Conan) |
| Riesgos técnicos | Auditor técnico de proyectos de gráficos universitarios |
| Assets y pipeline 3D | Especialista en pipeline de assets 3D (Blender/Assimp/OpenGL) |

---

## Formatos de respuesta predefinidos

### Para plan de implementación

```
### 1. Fases de desarrollo (tabla: fase | objetivo | duración | criterio de salida)
### 2. Prerequisitos antes de avanzar a la siguiente fase
### 3. Estructura de código src/ (árbol + descripción de clases)
### 4. Riesgos y mitigaciones (tabla: riesgo | probabilidad | mitigación)
### 5. Plan de acción concreto (pasos ejecutables)
```

### Para decisión técnica

```
### 1. Análisis de la situación
### 2. Opciones viables (mín. 2, máx. 4) con pros, contras, esfuerzo
### 3. Recomendación fundamentada
### 4. Riesgos y mitigaciones
### 5. Plan de acción concreto
```

### Para revisión de código

```
Tabla: # | Severidad (Alta/Media/Baja/Info) | Archivo:Línea | Hallazgo | Fix sugerido
Después: resumen ejecutivo y patrones detectados
```
