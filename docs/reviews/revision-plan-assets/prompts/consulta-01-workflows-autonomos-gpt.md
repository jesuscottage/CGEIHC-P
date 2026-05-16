# Prompt para ChatGPT — Revisión del Plan de Integración de Assets

## Rol

Eres un **auditor técnico senior de pipelines de assets 3D y automatización**, con experiencia en:
- Descarga automatizada de recursos CC0 (Sketchfab API, ambientcg, polyhaven, freesound)
- Integración de modelos GLTF en motores C++/OpenGL con Assimp
- Workflows autónomos de agentes de código (Claude Code, Cursor, Copilot Workspace)
- Restricciones de APIs web (rate limits, autenticación, CAPTCHAs, descarga programática)

## Contexto del Proyecto

Proyecto académico de Computación Gráfica (UNAM, C++17 + OpenGL 3.3 Core Profile). El motor 3D ya funciona completamente con geometría procedural. Se necesita integrar assets reales para mejorar la calidad visual.

**Herramientas del agente Claude Code**:
- `WebSearch` — buscar en internet (Google-like)
- `WebFetch` — obtener contenido de URLs, convierte HTML a markdown (NO descarga binarios directamente; para binarios usa Bash+curl)
- `Bash` — ejecutar comandos shell en Windows (Git Bash): `curl`, `unzip`, `mv`, `cmake`, `ffmpeg` (si está instalado)
- `Read` — leer archivos locales (incluido ver imágenes PNG/JPG)
- `Write` / `Edit` — crear/modificar archivos de código

**Restricción crítica**: El agente NO puede interactuar con GUIs, NO puede hacer login en sitios web, NO puede resolver CAPTCHAs, NO puede hacer clicks en botones de descarga que requieran JavaScript interactivo.

## Plan a Revisar

```markdown
# Plan Maestro — Integración de Assets y Entregables Finales

## Inventario de Assets Necesarios

### Texturas (8 archivos): suelo, paredes, hielo, nieve, water normal, metal, madera, asfalto
- Fuente: ambientcg.com (CC0, 1K resolution)
- Workflow: WebSearch → WebFetch (obtener URL ZIP) → curl ZIP → unzip → copiar Color map → limpiar

### Skybox (6 caras PNG): arctic sky cubemap
- Fuente: polyhaven.com HDRI → cubemap
- Workflow: buscar cubemap pre-cortado o HDRI → descargar → convertir si necesario

### Modelos 3D (10 modelos): iceberg, oso polar, edificios, turbina, auto, árbol, globo, foca, zorro, gaviota
- Fuente: Sketchfab CC0 GLTF
- Workflow: WebSearch → encontrar modelo → descargar GLTF/GLB → integrar con Assimp

### Audio (5 archivos): viento ártico, crujido hielo, activación UI, olas, música cierre
- Fuente: freesound.org CC0 / pixabay.com
- Workflow: WebSearch → encontrar sonido → curl descarga directa → mover a assets/audio/

### Fuente (1 archivo): Roboto-Regular.ttf
- Fuente: Google Fonts CDN
- Workflow: curl URL directa → guardar en assets/fonts/

## Workflows Clave

### WF-01 (Texturas desde ambientcg):
1. WebSearch("ambientcg.com {material} 1K")
2. WebFetch(url) → extraer URL del ZIP
3. curl -L -o /tmp/tex.zip {url}
4. unzip → copiar *_Color.jpg → limpiar

### WF-02 (Modelos desde Sketchfab):
1. WebSearch("sketchfab {modelo} CC0 free GLTF")
2. WebFetch(url_modelo) → verificar CC0, obtener link descarga
3. curl -L -o /tmp/model.zip {url}
4. unzip → copiar .glb → limpiar
5. Modificar C++ para cargar modelo con Assimp

### WF-03 (Audio):
1. WebSearch("pixabay sound effects {término}")
2. WebFetch → obtener URL directa
3. curl → guardar en app/assets/audio/

### WF-04 (Integración en código):
1. Agregar Model member + load() en init()
2. Reemplazar geometría procedural por model.draw()
3. Build + ejecutar + screenshot → verificar visualmente

## Fallbacks
- Si no se puede descargar modelo → mantener geometría procedural
- Si sitio requiere login → buscar alternativa sin login
- Si FPS baja de 30 → usar modelo más simple

## Orden: Fuente → Skybox → Texturas → Modelos → Audio → Verificación → Reportes → Empaquetado
```

## Preguntas a Evaluar

1. **¿Puede el agente ejecutar cada workflow SIN intervención humana?**
   - ¿ambientcg.com sirve ZIPs sin login ni JavaScript?
   - ¿Sketchfab permite descarga de GLB/GLTF sin API key ni login para modelos CC0?
   - ¿freesound.org permite descarga sin autenticación?
   - ¿pixabay.com permite descarga directa de audio con curl?
   - ¿polyhaven.com tiene API directa para cubemaps?

2. **¿El orden de ejecución es óptimo?** ¿Se puede paralelizar más?

3. **¿Faltan assets importantes** para la narrativa (el proyecto es un museo sobre calentamiento global en el Polo Norte)?

4. **¿Los fallbacks son realistas** para una entrega académica?

5. **¿Qué riesgos técnicos no están contemplados?** (ej: modelos con escala gigante, texturas embebidas en GLB que Assimp no extrae, normales invertidas, modelos sin UVs)

## Formato de Respuesta

```markdown
## Veredicto general
[¿El plan es ejecutable de forma completamente autónoma? Sí/No/Parcialmente + razón]

## Pasos que FALLARÁN
| # | Workflow | Paso | Razón | Solución |
|---|---------|------|-------|----------|

## Análisis de sitios web
| Sitio | Descarga sin login | Descarga con curl | CAPTCHAs/JS requerido | Alternativa viable |
|-------|--------------------|-------------------|-----------------------|-------------------|

## Mejoras al orden de ejecución
[Sugerencias]

## Assets faltantes
[Lista]

## Riesgos técnicos no contemplados
| Riesgo | Solución |
|--------|----------|

## Top 5 recomendaciones para éxito autónomo
1. ...
2. ...
3. ...
4. ...
5. ...
```
