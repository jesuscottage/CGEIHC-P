# Plan Maestro — Integración de Assets y Entregables Finales

> Proyecto: Calentamiento global en el polo norte
> Fecha de creación: 2026-05-16
> Última revisión: 2026-05-16 (post-auditoría Gemini + ChatGPT)
> Estado: Pendiente
> Ejecutor: Claude Code (autónomo)

---

## Objetivo

Completar TODOS los pendientes restantes del proyecto de forma autónoma:
1. Integrar assets reales (modelos 3D, texturas, skybox, audio, fuente)
2. Adaptar el código C++ para cargar y renderizar los assets
3. Verificar visualmente cada integración
4. Escribir reportes académicos (español + inglés)
5. Preparar empaquetado InstallForge

---

## Principios del Plan

1. **Iterativo y verificable**: cada asset se descarga → integra → verifica visualmente → itera si es necesario
2. **Descarga separada de integración**: primero acumular todos los assets descargables, luego integrar en C++
3. **Solo fuentes automation-friendly**: NO usar Sketchfab, Freesound, ni sitios que requieran login/JS/CAPTCHA
4. **Solo formato GLB**: para modelos 3D, SIEMPRE preferir .glb (binario autocontenido) sobre .gltf (referencias externas)
5. **Auto-escalado por AABB**: al cargar cada modelo, calcular bounding box con Assimp y normalizar escala
6. **Fallback rápido**: si un modelo pesa >10MB o baja FPS <30 tras 2 intentos, revertir al procedural
7. **Limpieza constante**: todo archivo descargado se mueve a `app/assets/` inmediatamente
8. **Commits periódicos**: después de cada fase completada, commit atómico
9. **Máximo 2 agentes simultáneos**: nunca más de 2 subagentes en paralelo

---

## Fuentes Aprobadas (Automation-Friendly)

| Tipo | Fuente principal | Fallback | Login | curl directo |
|------|-----------------|----------|-------|-------------|
| Texturas | ambientcg.com | polyhaven.com | No | Si |
| Skybox/HDRI | polyhaven.com | — | No | Si |
| Modelos 3D | polyhaven.com | kenney.nl, GitHub repos CC0 | No | Si |
| Audio | mixkit.co | archive.org, Wikimedia Commons | No | Si |
| Fuente TTF | Google Fonts CDN | — | No | Si |

**PROHIBIDO** (requieren login/JS/anti-bot):
- Sketchfab (401/403 sin sesión)
- Freesound (OAuth2 obligatorio)
- Pixabay (Cloudflare anti-bot frecuente)

---

## Inventario de Assets Necesarios

### Texturas (8 archivos mínimo)

| ID | Uso | Fuente | Formato | Destino |
|----|-----|--------|---------|---------|
| T01 | Suelo del museo (concreto/piedra gris) | ambientcg.com | PNG 1K | `app/assets/textures/floor.png` |
| T02 | Paredes del museo (concreto claro) | ambientcg.com | PNG 1K | `app/assets/textures/wall.png` |
| T03 | Hielo/glaciar | ambientcg.com | PNG 1K | `app/assets/textures/ice.png` |
| T04 | Nieve (terreno exterior) | ambientcg.com | PNG 1K | `app/assets/textures/snow.png` |
| T05 | Agua (normal map para shader) | ambientcg.com / polyhaven | PNG 1K | `app/assets/textures/water_normal.png` |
| T06 | Metal (turbina/poste) | ambientcg.com | PNG 1K | `app/assets/textures/metal.png` |
| T07 | Madera (paneles informativos) | ambientcg.com | PNG 1K | `app/assets/textures/wood.png` |
| T08 | Asfalto/camino | ambientcg.com | PNG 1K | `app/assets/textures/path.png` |

### Skybox (6 caras)

| ID | Cara | Fuente | Formato | Destino |
|----|------|--------|---------|---------|
| S01-S06 | right, left, top, bottom, front, back | polyhaven.com (arctic HDRI → cubemap) | PNG 1K | `app/assets/skybox/{face}.png` |

### Modelos 3D (7-10 modelos)

**Formato obligatorio**: GLB (binario autocontenido — NO .gltf con archivos externos)
**Fuentes aprobadas**: polyhaven.com, kenney.nl/assets, GitHub repos con GLB crudos CC0
**Tamaño máximo**: 10MB por modelo, <100K triángulos

| ID | Modelo | Módulo | Fuente primaria | Búsqueda alternativa | Destino |
|----|--------|--------|-----------------|---------------------|---------|
| MD01 | Iceberg | M1_IZQ | polyhaven.com/models | GitHub "iceberg glb CC0" | `app/assets/models/iceberg.glb` |
| MD02 | Oso polar | M2_IZQ | polyhaven.com/models | GitHub "polar bear glb" | `app/assets/models/polar_bear.glb` |
| MD03 | Edificios/ciudad | M3_IZQ | kenney.nl (City Kit) | GitHub "low poly buildings glb" | `app/assets/models/buildings.glb` |
| MD04 | Turbina eólica | M1_DER | polyhaven.com/models | kenney.nl | `app/assets/models/wind_turbine.glb` |
| MD05 | Auto eléctrico | M2_DER | kenney.nl (Vehicle Kit) | GitHub "car glb CC0" | `app/assets/models/electric_car.glb` |
| MD06 | Árbol/vegetación | M3_DER | polyhaven.com/models | kenney.nl (Nature Kit) | `app/assets/models/tree.glb` |
| MD07 | Globo terráqueo | M5 | GitHub "earth globe glb" | polyhaven.com | `app/assets/models/globe.glb` |
| MD08 | Foca | Fauna | GitHub "seal animal glb CC0" | — (mantener procedural) | `app/assets/models/seal.glb` |
| MD09 | Zorro ártico | Fauna | GitHub "fox animal glb CC0" | — (mantener procedural) | `app/assets/models/arctic_fox.glb` |
| MD10 | Gaviota | Fauna | GitHub "seagull bird glb CC0" | — (mantener procedural) | `app/assets/models/seagull.glb` |

**Nota**: MD08-MD10 (fauna) tienen baja probabilidad de encontrarse en CC0 sin login. Si no se encuentran en 2 intentos, se mantiene la geometría procedural actual (ya funciona y cumple rúbrica).

### Audio (5 archivos)

**Fuentes aprobadas**: mixkit.co (descarga directa sin login), archive.org, Wikimedia Commons
**PROHIBIDO**: freesound.org (requiere OAuth2), pixabay.com (Cloudflare anti-bot)
**Formato**: WAV o MP3 (miniaudio soporta ambos nativamente; OGG también si está disponible)

| ID | Descripción | Fuente primaria | Búsqueda | Destino |
|----|-------------|-----------------|----------|---------|
| A01 | Viento ártico ambiental (loop) | mixkit.co | "wind ambient" | `app/assets/audio/ambient.wav` |
| A02 | Crujido de hielo | mixkit.co | "ice crack" | `app/assets/audio/ice_crack.wav` |
| A03 | Activación de módulo (UI blip) | mixkit.co | "notification pop" | `app/assets/audio/activate.wav` |
| A04 | Agua/olas suaves | mixkit.co | "ocean waves" | `app/assets/audio/waves.wav` |
| A05 | Música de cierre (tranquila) | mixkit.co | "calm ambient music" | `app/assets/audio/closing.mp3` |

### Fuente (1 archivo)

| ID | Archivo | Fuente | Destino |
|----|---------|--------|---------|
| F01 | Roboto-Regular.ttf | Google Fonts (Apache 2.0) | `app/assets/fonts/Roboto-Regular.ttf` |

---

## Fases de Ejecución

### Fase A — Infraestructura Base (fuente + skybox + texturas)

**Duración estimada**: 1 bloque de trabajo
**Dependencias**: Ninguna

#### A.1 — Descargar fuente Roboto-Regular.ttf

**Workflow**:
```
1. WebFetch: obtener URL directa de Google Fonts CDN para Roboto-Regular.ttf
2. Bash: curl -L -o "app/assets/fonts/Roboto-Regular.ttf" <URL>
3. Bash: ls -la app/assets/fonts/ (verificar tamaño > 100KB)
4. Modificar main.cpp: ImGui::GetIO().Fonts->AddFontFromFileTTF(ASSETS_DIR "fonts/Roboto-Regular.ttf", 18.0f)
5. Build + ejecutar + capturar screenshot
6. Read: active/screenshot_f060.png → verificar que el texto ImGui se ve bien
```

**Criterio de éxito**: El texto ImGui usa Roboto en vez de la fuente por defecto.

#### A.2 — Descargar skybox ártico (6 caras PNG)

**Workflow**:
```
1. WebSearch: "polyhaven.com arctic sky HDRI" o "kloofendal winter cubemap"
2. WebFetch: obtener URL de descarga directa (1K cubemap PNG)
   - Alternativa: buscar cubemaps gratuitos en humus.name o learnopengl.com resources
3. Bash: curl -L -o descarga... para cada cara (o ZIP → extraer)
4. Bash: mv archivos a app/assets/skybox/{right,left,top,bottom,front,back}.png
5. Modificar Skybox.h: cargar texturas PNG en vez de generar cubemap procedural
6. Build + ejecutar + screenshot
7. Read screenshot → verificar cielo realista
8. Si no se ve bien → ajustar orden de caras o flip vertical
```

**Criterio de éxito**: Skybox muestra cielo ártico con horizonte y nubes/aurora.

**Fallback**: Si no se encuentra cubemap pre-cortado adecuado, mantener el procedural (ya funciona bien).

#### A.3 — Descargar texturas PBR (8 texturas)

**Workflow por cada textura** (repetir T01-T08):
```
1. WebSearch: "ambientcg.com [material] 1K" (ej: "Concrete034 1K")
2. WebFetch: página de descarga → extraer URL directa del ZIP 1K
3. Bash: curl -L -o /tmp/tex.zip <URL>
4. Bash: unzip -o /tmp/tex.zip -d /tmp/tex_extract/
5. Bash: cp /tmp/tex_extract/*_Color.jpg app/assets/textures/<nombre>.png
   (o convertir con ImageMagick si es necesario)
6. Bash: rm -rf /tmp/tex.zip /tmp/tex_extract/
7. Verificar: ls -la app/assets/textures/<nombre>.png (>50KB)
```

**Integración en código** (después de descargar todas):
```
1. Modificar Museum.h: cargar texturas para suelo y paredes
2. Crear sistema simple de materiales: mapear textura → geometría
3. Build + ejecutar → screenshot → verificar que suelo y paredes tienen textura
```

**Criterio de éxito**: Suelo y paredes del museo muestran texturas reales en vez de colores planos.

---

### Fase B — Descarga masiva de modelos 3D (separada de integración)

**Duración estimada**: 1-2 bloques de trabajo
**Dependencias**: Ninguna (la descarga es independiente)
**Estrategia**: Descargar TODOS los modelos posibles primero. Integrar en Fase D.

#### B.0 — Verificar que Model.h funciona con archivos reales

**Workflow**:
```
1. Descargar un modelo simple de prueba desde Poly Haven o KhronosGroup/glTF-Sample-Models
2. Cargar con Model.h existente
3. Build + ejecutar → verificar que se renderiza
4. Si falla → debuggear Model.h con Assimp
5. Implementar cálculo de AABB (min/max XYZ) e imprimirlo en consola al cargar
```

#### B.1 — Descarga batch de modelos (WF-02 repetido)

Ejecutar WF-02 para cada modelo en orden de prioridad. **Máximo 3 intentos por modelo**.

**Prioridad de modelos** (orden de búsqueda):
1. MD04 (turbina eólica) — muy común en repos CC0, alto impacto visual
2. MD06 (árbol) — extremadamente común en cualquier fuente
3. MD05 (auto eléctrico) — kenney.nl tiene Vehicle Kit CC0
4. MD03 (edificios) — kenney.nl tiene City Kit CC0
5. MD07 (globo terráqueo) — búsqueda específica en GitHub
6. MD01 (iceberg) — más difícil de encontrar, Poly Haven puede tener rocas
7. MD02 (oso polar) — difícil en CC0 sin login
8. MD08-MD10 (fauna) — muy baja probabilidad, mantener procedural

**Regla de corte**: Si tras 3 intentos de búsqueda no se encuentra un modelo descargable → PASAR AL SIGUIENTE. No perder tiempo.

**Criterio de éxito de la fase**: Al menos 4-5 de los 10 modelos descargados como .glb en `app/assets/models/`.

**Fallback global**: La geometría procedural actual ya cumple rúbrica. Los modelos reales son mejora visual, no requisito.

---

### Fase C — Audio (5 archivos)

**Duración estimada**: 1 bloque de trabajo
**Dependencias**: Ninguna (puede ejecutarse en paralelo con Fase B)

#### C.1-C.5 — Descargar e integrar audio

**Workflow repetitivo por archivo de audio**:
```
1. WebSearch: "freesound.org [descripción] CC0" (ej: "arctic wind loop CC0")
2. WebFetch: página del sonido → verificar licencia CC0, obtener URL de descarga
   - freesound.org requiere login para descarga directa
   - Alternativa: buscar en pixabay.com/sound-effects/ (no requiere login, CC0)
   - Alternativa: usar archive.org con búsqueda de audio CC0
3. Bash: curl -L -o app/assets/audio/<destino> <URL>
4. Bash: ls -la app/assets/audio/<destino> (verificar tamaño > 10KB)
5. Ejecutar app → verificar que miniaudio reproduce sin error
```

**Integración en código**:
```
- ambient.ogg ya está referenciado en main.cpp línea 134
- Agregar ice_crack.wav y activate.wav como sonidos posicionales en módulos
- Agregar waves.ogg como sonido ambiental secundario
- Agregar closing.ogg al estado CIERRE
```

**Criterio de éxito**: La app reproduce audio ambiental al iniciar y sonidos de activación.

**Fallback**: Si freesound requiere login y no se puede descargar, buscar en:
- pixabay.com/sound-effects/ (descarga directa sin login)
- mixkit.co/free-sound-effects/ (descarga directa)
- soundbible.com (CC0)

---

### Fase D — Integración en Código C++ y Verificación Visual

**Duración estimada**: 2 bloques de trabajo
**Dependencias**: Fases A, B, C completas (assets ya descargados en app/assets/)

#### D.0 — Integrar texturas en Museum.h

**Workflow**:
```
1. Modificar Museum.h:
   - Cargar texturas de suelo y paredes en init()
   - Aplicar textura al suelo (Texture floorTex) con UV tiling
   - Aplicar textura a paredes (Texture wallTex)
2. Build + screenshot → verificar superficies texturizadas
```

#### D.1 — Integrar modelos 3D (WF-02B + WF-04 por cada modelo descargado)

**Workflow por modelo**:
```
1. Cargar modelo con Model.h → imprimir AABB en consola
2. Calcular factor de escala para que quepa en ~3-5m
3. Reemplazar geometría procedural en ModuleScene.h:
   - Cambiar drawXxx() para usar model.draw() en vez de mCube.draw()
   - Mantener la lógica de animT (escalar/rotar/trasladar según progreso)
4. Build + --test-module → screenshot → verificar
5. Si invisible: desactivar GL_CULL_FACE para ese modelo
6. Si negro/sin textura: forzar baseColor con color apropiado
7. Si FPS < 30: REVERTIR al procedural para ese modelo (git checkout del archivo)
```

**Regla de iteración**: Máximo 3 intentos de ajuste por modelo. Si no funciona → procedural.

#### D.2 — Integrar audio

**Workflow**:
```
1. Modificar main.cpp:
   - ambient.wav ya está referenciado (línea 134)
   - Agregar sonido de activación (activate.wav) en el if(near && E pressed)
   - Agregar música de cierre (closing.mp3) en AppState::CIERRE
2. Build + ejecutar → verificar en consola que miniaudio no reporta errores
```

#### D.3 — QA visual completa

**Workflow**:
```
1. Ejecutar CADA módulo con --test-module + --activate-frame 60
2. Leer state.json → verificar FPS > 30
3. Read screenshot → verificar coherencia visual
4. Si FPS < 30 en algún módulo → revertir ese modelo a procedural
```

**Criterio de éxito**: Todos los módulos > 30 FPS, assets visibles y coherentes.

#### D.4 — Commit de integración

```
git add app/assets/ app/src/
git commit -m "feat(assets): integrar modelos 3D, texturas, skybox y audio reales"
git push origin main
```

---

### Fase E — Reportes Académicos

**Duración estimada**: 1-2 bloques de trabajo
**Dependencias**: Fase D completa (necesita screenshots finales)

#### E.1 — Reporte en Español (20-35 páginas)

**Estructura APA obligatoria**:
```
docs/reports/reporte-es.md

1. Portada (título, equipo, fecha, institución)
2. Resumen / Abstract (250 palabras)
3. Introducción
   - Contexto del problema (calentamiento global)
   - Objetivo del proyecto
   - Alcance
4. Marco Teórico
   - Computación gráfica en tiempo real
   - OpenGL pipeline
   - Técnicas de iluminación (Blinn-Phong, Fresnel)
   - Animación 3D (keyframe, procedural)
   - Interacción humano-computadora
5. Desarrollo
   - Arquitectura del sistema
   - Pipeline de renderizado
   - Sistema de módulos y narrativa
   - Técnicas implementadas
   - Assets y modelado
6. Resultados
   - Screenshots del sistema final
   - Tabla de rendimiento (FPS)
   - Cumplimiento de rúbrica
7. Conclusiones
8. Referencias (formato APA, mínimo 10)
   - learnopengl.com
   - Hearn & Baker (2006)
   - Angel & Shreiner (2011)
   - de Vries (2020)
   - Papers sobre calentamiento global
9. Anexos
   - Diagramas de arquitectura
   - Código relevante
```

**Workflow**:
```
1. Crear docs/reports/reporte-es.md con estructura completa
2. Insertar screenshots de Fase D
3. Escribir contenido técnico detallado
4. Aplicar /corregir-ortografia al final
5. Verificar longitud (20-35 páginas equivalentes)
```

#### E.2 — Reporte en Inglés

**Workflow**:
```
1. Traducir reporte español a inglés
2. Adaptar terminología técnica
3. Guardar en docs/reports/report-en.md
```

---

### Fase F — Empaquetado InstallForge

**Duración estimada**: 0.5 bloques de trabajo
**Dependencias**: Fase D completa

#### F.1 — Preparar estructura de distribución

**Workflow**:
```
1. cmake --build build --config Release --parallel
2. Crear carpeta dist/ (gitignored):
   dist/
   ├── CGEIHC.exe          (desde build/Release/)
   ├── assets/             (copia completa de app/assets/)
   ├── shaders/            (copia completa de app/shaders/)
   ├── README.txt          (instrucciones de uso)
   └── manual.pdf          (resumen del sistema)
3. Verificar que el .exe funciona desde dist/ con paths relativos
4. Si falla → ajustar paths en código para que sean relativos al .exe
```

#### F.2 — Crear proyecto InstallForge

**Workflow**:
```
1. Verificar si InstallForge está instalado
2. Si no → proporcionar instrucciones de instalación al equipo
3. Crear script/config InstallForge (XML) con:
   - Nombre: "Nuestro Mundo - Polo Norte"
   - Archivos fuente: dist/*
   - Destino: C:\Program Files\NuestroMundo\
   - Acceso directo en escritorio
   - Desinstalador
4. Documentar proceso en docs/plans/empaquetado.md
```

**Nota**: InstallForge requiere GUI interactiva. Claude prepara TODOS los archivos y la configuración; si la creación del .exe del instalador requiere interacción gráfica, se documenta el procedimiento paso a paso para el equipo.

---

### Fase G — Video Demostrativo

**Duración estimada**: Parcialmente autónomo
**Dependencias**: Fase D completa

#### G.1 — Preparar guión y assets para el video

**Workflow**:
```
1. Escribir guión narrado (docs/reports/guion-video.md):
   - 0:00-0:30 — Introducción (pantalla de título, contexto)
   - 0:30-1:30 — Recorrido ruta izquierda (M1, M2, M3)
   - 1:30-2:30 — Recorrido ruta derecha (M1, M2, M3)
   - 2:30-3:30 — Módulo compartido M5 + cierre narrativo
   - 3:30-4:00 — Créditos y reflexión
2. Capturar secuencia de screenshots para cada momento del guión
3. Documentar proceso de captura de video (OBS o grabación interna)
```

**Nota**: La edición final en DaVinci Resolve requiere acción manual del equipo. Claude prepara: guión, screenshots de referencia, y texto para narración.

---

## Workflows Detallados (Referencia para Auditoría)

### WF-01: Descarga y verificación de textura desde ambientcg.com

```
INPUT: nombre_material (ej: "Concrete034"), destino (ej: "floor.png")
PASOS:
  1. WebSearch("ambientcg.com {nombre_material} 1K download")
  2. WebFetch(url_resultado, "dame la URL directa de descarga del ZIP 1K")
  3. Bash: curl -L -o /tmp/ambientcg.zip "{url_zip}"
  4. Bash: unzip -o /tmp/ambientcg.zip -d /tmp/ambientcg_extract/
  5. Bash: find /tmp/ambientcg_extract/ -name "*Color*" -o -name "*Diffuse*" | head -1
  6. Bash: cp "{archivo_color}" "app/assets/textures/{destino}"
  7. Bash: rm -rf /tmp/ambientcg.zip /tmp/ambientcg_extract/
  8. Read: app/assets/textures/{destino} → verificar que es imagen válida
OUTPUT: archivo PNG/JPG en app/assets/textures/
VERIFICACIÓN: tamaño > 50KB, formato imagen válido
FALLBACK: si ambientcg no tiene el material, buscar en polyhaven.com o 3dtextures.me
```

### WF-02: Descarga de modelo 3D (fuentes automation-friendly)

```
INPUT: término_búsqueda (ej: "iceberg low poly"), destino (ej: "iceberg.glb")
FUENTES (en orden de prioridad):
  - polyhaven.com/models (API directa, CC0, GLB)
  - kenney.nl/assets (ZIP con GLB, CC0)
  - GitHub repos con archivos .glb crudos (ej: KhronosGroup/glTF-Sample-Models)
  - archive.org (búsqueda de modelos 3D CC0)
PROHIBIDO: Sketchfab (requiere login), cgtrader (login), turbosquid (login)

PASOS:
  1. WebSearch("polyhaven.com models {término}" OR "site:github.com {término} glb CC0")
  2. WebFetch(url_resultado, "¿Tiene archivo .glb descargable? Dame URL directa del archivo")
  3. Si es Poly Haven:
     Bash: curl -L -o "app/assets/models/{destino}" "https://dl.polyhaven.org/file/ph-assets/Models/glb/{id}/{id}_1k.glb"
  4. Si es GitHub raw:
     Bash: curl -L -o "app/assets/models/{destino}" "{raw_url}"
  5. Si es ZIP (Kenney):
     Bash: curl -L -o /tmp/model.zip "{url}"
     Bash: unzip -o /tmp/model.zip -d /tmp/model_extract/
     Bash: find /tmp/model_extract/ -name "*.glb" | head -1
     Bash: cp "{archivo_glb}" "app/assets/models/{destino}"
     IMPORTANTE: copiar SOLO archivos .glb (autocontenidos). NUNCA archivos .gltf sueltos.
  6. Bash: rm -rf /tmp/model.zip /tmp/model_extract/ 2>/dev/null
  7. Bash: ls -la "app/assets/models/{destino}" (verificar tamaño < 10MB y > 1KB)
  8. Si tamaño > 10MB → DESCARTAR, buscar versión más ligera o usar fallback
OUTPUT: archivo .glb en app/assets/models/
VERIFICACIÓN: tamaño entre 1KB y 10MB, extensión .glb
FALLBACK: si no se encuentra en 3 intentos → mantener geometría procedural
```

### WF-02B: Validación y auto-escalado de modelo cargado

```
INPUT: modelo recién descargado (ej: "iceberg.glb"), módulo destino (ej: "M1_IZQ")
PASOS:
  1. Agregar código temporal en main.cpp o ModuleScene.h para cargar el modelo:
     Model testModel;
     testModel.load(ASSETS_DIR "models/{archivo}");
     // Imprimir AABB en consola:
     printf("AABB min=(%.2f,%.2f,%.2f) max=(%.2f,%.2f,%.2f)\n",
            testModel.aabbMin.x, testModel.aabbMin.y, testModel.aabbMin.z,
            testModel.aabbMax.x, testModel.aabbMax.y, testModel.aabbMax.z);
  2. Build + ejecutar → leer salida de consola
  3. Calcular factor de escala:
     - Tamaño deseado por módulo: ~3-5 metros de alto
     - Factor = tamaño_deseado / max(aabb.x, aabb.y, aabb.z)
  4. Aplicar escala en la función draw correspondiente:
     glm::scale(model_matrix, glm::vec3(factor))
  5. Build + screenshot → verificar visualmente
  6. Si modelo invisible → posible problema de normales:
     - Intentar: glDisable(GL_CULL_FACE) temporalmente para ese draw
     - Si aparece → normales invertidas, dejar cull_face desactivado para ese modelo
  7. Si modelo oscuro/negro → posible falta de UVs:
     - Forzar useTexture=false y baseColor=(color_apropiado) para ese modelo
OUTPUT: modelo renderizado correctamente con escala apropiada
CRITERIO: visible, tamaño coherente con la escena, iluminado
```

### WF-03: Descarga de audio (fuentes sin login)

```
INPUT: término_búsqueda (ej: "arctic wind"), destino (ej: "ambient.wav")
FUENTES (en orden de prioridad):
  - mixkit.co/free-sound-effects/ (descarga directa, sin login, CC0-equivalente)
  - archive.org (búsqueda de audio CC0)
  - commons.wikimedia.org (audio libre)
PROHIBIDO: freesound.org (OAuth2), pixabay.com (Cloudflare anti-bot)
FORMATOS ACEPTADOS: WAV, MP3, OGG, FLAC (miniaudio soporta todos nativamente)

PASOS:
  1. WebSearch("mixkit.co free sound effect {término}")
  2. WebFetch(url_resultado, "dame la URL directa de descarga del archivo de audio WAV/MP3")
  3. Bash: curl -L -o "app/assets/audio/{destino}" "{url_directa}"
  4. Bash: ls -la "app/assets/audio/{destino}" (verificar > 10KB)
  5. Si falla mixkit → intentar archive.org:
     WebSearch("site:archive.org {término} sound CC0 wav")
     curl desde archive.org/download/{id}/{archivo}
  6. Si falla archive.org → intentar Wikimedia:
     WebSearch("site:commons.wikimedia.org {término} sound .wav")
OUTPUT: archivo de audio en app/assets/audio/
VERIFICACIÓN: tamaño > 10KB, formato WAV/MP3/OGG
FALLBACK: si no se encuentra tras 3 fuentes → miniaudio simplemente no reproduce (fallback gracioso ya implementado en main.cpp)
```

### WF-04: Integración de asset en código C++

```
INPUT: tipo (textura/modelo/audio), archivo, componente_destino
PASOS:
  1. Read: archivo fuente C++ actual (ej: ModuleScene.h)
  2. Identificar la función placeholder que será reemplazada
  3. Agregar member variable: Model/Texture + load() en init()
  4. Reemplazar geometría procedural por model.draw() o aplicar textura
  5. Build: cmake --build build --config Debug --parallel
  6. Si error de compilación → leer error, corregir, re-build
  7. Ejecutar con --test-module pertinente
  8. Read: active/state.json → verificar FPS
  9. Read: active/screenshot_f060.png → verificar visualmente
  10. Si no se ve bien → ajustar escala/posición/rotación
  11. Iterar hasta satisfactorio
OUTPUT: código modificado, asset integrado visualmente
VERIFICACIÓN: screenshot muestra modelo real, FPS > 30
```

### WF-05: Verificación visual post-integración

```
INPUT: módulo a verificar (ej: "M1_IZQ")
PASOS:
  1. Bash: cmake --build build --config Debug --parallel
  2. Bash: ./build/Debug/CGEIHC.exe --test-module {módulo} --activate-frame 60
     (en background, max 15 segundos)
  3. Read: active/state.json → extraer FPS y animT
  4. Read: active/screenshot_f060.png → verificar:
     - ¿Se ve el modelo? (no está invisible/fuera de cámara)
     - ¿Escala correcta? (no es microscópico o gigante)
     - ¿Texturas aplicadas? (no es negro/magenta)
     - ¿Iluminación correcta? (no es completamente oscuro)
  5. Si hay problemas → diagnosticar:
     - Invisible: verificar posición, scale, normals
     - Escala: ajustar factor en matriz model
     - Sin textura: verificar path, UV coords
     - Oscuro: verificar normales del modelo
  6. Iterar (máximo 3 intentos por modelo)
OUTPUT: screenshot satisfactorio guardado en active/
CRITERIO: modelo visible, escala coherente, iluminado correctamente, FPS > 30
```

---

## Orden de Ejecución y Dependencias

```
═══════════════════ BLOQUE 1: DESCARGA MASIVA ═══════════════════
(Todas las descargas son independientes entre sí)

Fase A.1 (fuente)  ──┐
Fase A.2 (skybox)  ──┤
Fase A.3 (texturas)──┤── Ejecutar en secuencia rápida (curl es instantáneo)
Fase B.1 (modelos) ──┤
Fase C.1 (audio)   ──┘
                      │
═══════════════ BLOQUE 2: INTEGRACIÓN C++ ═══════════════════
                      ▼
Fase B.0 (test Model.h) ←── Validar que Assimp carga GLB correctamente
         │
         ▼
Fase D.0 (texturas en Museum.h)
Fase D.1 (modelos en ModuleScene.h)  ←── Con auto-escalado AABB
Fase D.2 (audio en main.cpp)
         │
         ▼
Fase D.3 (QA visual + rendimiento)
Fase D.4 (commit)
         │
═══════════════ BLOQUE 3: ENTREGABLES ═══════════════════
         ▼
Fase E (reportes)  ──┐
Fase F (empaquetado)──┤── Paralelo (E y F son independientes)
Fase G (video)     ──┘
```

**Paralelismo aprovechado**:
- Bloque 1: un agente puede descargar modelos mientras otro descarga texturas+audio
- Bloque 3: reportes y empaquetado son completamente independientes

---

## Gestión de Riesgos (Actualizada post-auditoría)

| Riesgo | Probabilidad | Impacto | Mitigación |
|--------|-------------|---------|------------|
| Modelo con escala gigante/microscópica | Alta | Alto | Auto-escalado AABB: calcular bounding box y normalizar a 3-5m |
| GLB con texturas embebidas incompatibles | Alta | Medio | Forzar `useTexture=false` + `baseColor` como fallback |
| Modelo sin UVs (textura no mapea) | Alta | Medio | Detectar en consola, aplicar color plano |
| Normales invertidas (modelo invisible con culling) | Media | Medio | Si invisible: `glDisable(GL_CULL_FACE)` para ese draw |
| Modelo muy pesado (>100K tris, >10MB) | Media | Alto | Rechazar automáticamente, buscar versión más ligera o mantener procedural |
| Poly Haven no tiene modelo específico | Media | Medio | Buscar en kenney.nl y GitHub; si no hay → procedural |
| FPS cae debajo de 30 con modelos reales | Media | Alto | Revertir modelo problemático a procedural inmediatamente |
| mixkit.co cambia estructura HTML | Baja | Medio | Fallback a archive.org |
| Assimp crash con modelo corrupto | Baja | Alto | try/catch en load(), si falla → log + procedural |
| Ejes Y-up vs Z-up inconsistentes | Media | Medio | Assimp normaliza a Y-up por defecto; verificar visualmente |

---

## Criterios de Completitud

El plan se considera **COMPLETADO** cuando:

- [ ] Todas las texturas descargadas e integradas (suelo y paredes texturizados)
- [ ] Skybox real o procedural mejorado funcionando
- [ ] Al menos 5 de 7 modelos de módulos son modelos reales (no placeholder)
- [ ] Al menos 3 archivos de audio reproduciendo correctamente
- [ ] Fuente Roboto integrada en ImGui
- [ ] QA visual: screenshots satisfactorios de todos los módulos
- [ ] QA rendimiento: todos los módulos > 30 FPS
- [ ] Reporte en español completo (20+ páginas)
- [ ] Reporte en inglés completo
- [ ] Estructura dist/ preparada para InstallForge
- [ ] Guión de video escrito
- [ ] Commit final con todos los assets

---

## Notas para Auditoría

1. **Cada descarga se documenta**: origen (URL), licencia, fecha, tamaño
2. **Cada integración se verifica**: screenshot + state.json (AABB impreso en consola)
3. **Cada fallback se justifica**: por qué no se encontró el asset ideal
4. **Los commits son atómicos**: una fase = un commit
5. **El plan es adaptable**: si un asset no se encuentra, se mantiene el placeholder (el proyecto ya funciona completamente con geometría procedural)
6. **Solo fuentes sin autenticación**: todo descargable con curl directo

---

## Registro de Auditoría Externa

### Consulta 01 — Revisión de workflows autónomos (2026-05-16)

**Revisores**: Gemini 2.5 Pro + ChatGPT GPT-4o (Modo B)
**Veredicto**: Parcialmente ejecutable → Corregido
**Cambios aplicados al plan**:
- Eliminadas fuentes que requieren login (Sketchfab, Freesound, Pixabay)
- Agregadas fuentes automation-friendly (Poly Haven, Kenney, Mixkit, archive.org)
- Añadido WF-02B (validación AABB + auto-escalado)
- Separada descarga (Bloque 1) de integración (Bloque 2)
- Formato obligatorio: .glb (NO .gltf con archivos externos)
- Añadidos 3 riesgos técnicos nuevos (normales, UVs, ejes)
- Regla de corte: máximo 3 intentos por modelo antes de fallback

**Reportes completos**: `docs/reviews/revision-plan-assets/reportes/`
