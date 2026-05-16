### 1. Orden de fases — ¿Es óptimo?

**Veredicto**: Correcciones menores

**Análisis**:
El orden general es bueno y está claramente pensado para minimizar dependencia circular. La progresión `core → render → assets → layout → interacción → shaders → efectos → UI → integración → QA` es correcta para un proyecto autónomo por agente.

Hay varias decisiones especialmente acertadas:

* Introducir screenshots automáticos desde Fase 1 es excelente. Evita “desarrollo ciego”.
* Posponer shaders avanzados hasta después de tener módulos funcionales reduce muchísimo el retrabajo.
* Separar Assimp (Fase 3) del layout (Fase 4) también está bien: desacopla carga de assets de lógica espacial.
* El orden de render definido en Fase 7 evita problemas clásicos de depth/transparencia.

Los problemas son más de granularidad y dependencia implícita que de arquitectura.

**Correcciones sugeridas**:

* Mover parte del “render order” de Fase 7 a Fase 2.

  * El concepto “pipeline de render con capas” debería existir desde el inicio aunque todavía no haya skybox/agua/partículas.
  * Si no, Claude puede construir render loops monolíticos difíciles de refactorizar después.
  * Basta con introducir:

    ```cpp
    renderOpaque();
    renderTransparent();
    renderUI();
    ```

* Adelantar una versión mínima de `ResourceManager` a Fase 2.

  * Actualmente aparece hasta Fase 3.
  * Pero shaders y texturas ya necesitan caché básica antes.
  * Si no, Claude probablemente duplicará cargas y luego habrá retrabajo.

* Dividir Fase 7 en dos subfases lógicas:

  * 7A: Skybox + niebla
  * 7B: Agua + Fresnel
  * El shader de agua es el elemento técnicamente más frágil del proyecto.

* Fase 9 está ligeramente sobrecargada.

  * UI, texto, estados y audio juntos son demasiados subsistemas.
  * Especialmente Dear ImGui + stb_truetype + miniaudio en una sola fase puede producir debugging cruzado.
  * Recomendable:

    * Fase 9: UI + texto
    * Fase 10: audio + narrativa/integración

* Fase 11 debería incluir explícitamente:

  * “Detección de fugas de recursos OpenGL”
  * “Verificación de shutdown limpio”
  * Porque Claude tiende a olvidar `glDelete*()` y `ma_engine_uninit()`.

---

### 2. Pipeline de verificación visual — ¿Es suficiente?

**Veredicto**: Correcciones menores

**Análisis**:
Para un proyecto académico autónomo, el pipeline es sorprendentemente sólido. La combinación:

1. Build automático
2. Ejecución determinista
3. Screenshot fijo
4. Análisis visual iterativo

es exactamente el tipo de ciclo que hace viable el desarrollo por agente.

Detectará correctamente:

* geometría rota,
* texturas ausentes,
* matrices MVP incorrectas,
* objetos fuera de posición,
* iluminación completamente negra,
* UVs invertidas,
* skybox roto,
* clipping grave,
* partículas invisibles,
* fallos de escala.

También es muy buena idea cerrar automáticamente la app después del screenshot. Reduce estados colgados y procesos zombies.

Lo que NO detecta bien:

1. Problemas temporales

   * Animaciones incorrectas
   * easing roto
   * LERP congelado
   * partículas no persistentes
   * reset defectuoso

2. Problemas interactivos

   * trigger no activado
   * tecla E ignorada
   * estados AppState mal conectados
   * navegación rota

3. Problemas dependientes de cámara

   * objetos visibles desde un ángulo pero no otro
   * culling incorrecto
   * precision z-fighting

4. Problemas de profundidad/transparencia

   * orden incorrecto de blending
   * agua renderizada antes que skybox
   * depth mask mal restaurado

5. Performance

   * el screenshot no mide FPS reales sostenidos.

**Correcciones sugeridas**:

* Agregar “scripted camera paths”.

  * Muy importante.
  * Ejemplo:

    ```cpp
    if (AUTO_TEST_MODE) {
        camera.position = lerp(path[t]);
    }
    ```
  * Permite screenshots deterministas desde múltiples zonas.

* Generar 3 screenshots:

  * frame 60
  * frame 120
  * frame 240
  * Captura animaciones y errores temporales.

* Añadir overlay debug opcional:

  ```cpp
  drawText("FPS: 58");
  drawText("DrawCalls: 143");
  drawText("ActiveModule: M2");
  ```

  Claude puede leer esto visualmente.

* Guardar logs OpenGL automáticamente:

  ```cpp
  glDebugMessageCallback(...)
  ```

  ya está bien planteado, pero faltó persistencia:

  ```txt
  active/gl_log.txt
  ```

* Añadir modo wireframe automático:

  ```cpp
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  ```

  Una captura wireframe detecta:

  * mallas corruptas,
  * normales degeneradas,
  * índices malos,
  * geometría invisible.

* Muy recomendable:
  guardar un JSON simple de estado:

  ```json
  {
    "fps": 57,
    "camera": [x,y,z],
    "moduleActive": "M3",
    "particles": 96
  }
  ```

  Porque no todo debe verificarse visualmente.

---

### 3. Tareas faltantes o sobredimensionadas

**Veredicto**: Correcciones menores

**Análisis**:
El plan está bastante bien dimensionado para un proyecto universitario. De hecho, tiene mejor control de alcance que muchos proyectos reales.

Lo que está correctamente simplificado:

* Sin ECS
* Sin PBR
* Sin multithreading
* Sin deferred
* Sin físicas complejas
* Sin skeletal animation
* Sin editor

Eso es exactamente correcto para OpenGL 3.3 académico.

Las partes potencialmente sobredimensionadas:

1. Fresnel en múltiples shaders

   * Está bien mantenerlo simple.
   * No intentar aproximaciones físicas complejas.

2. CO₂ espiral procedural

   * Riesgo moderado para poco valor académico.
   * Puede convertirse en debugging visual innecesario.

3. Skybox cubemap manual

   * Viable, pero sensible a:

     * orientación,
     * handedness,
     * seams.
   * Aun así, aceptable.

Faltan algunas tareas importantes:

1. Gestión explícita de estados OpenGL

   * Falta una política clara:

     ```cpp
     glEnable(GL_DEPTH_TEST);
     glEnable(GL_CULL_FACE);
     glBlendFunc(...);
     ```
   * Claude suele introducir estados globales inconsistentes.

2. Política de coordenadas

   * Muy importante para agentes.
   * Debe definirse:

     * Y-up
     * metros reales
     * handedness
     * forward = -Z

3. Asset naming convention

   * Faltó:

     ```txt
     assets/models/
     assets/textures/
     assets/audio/
     ```
   * Agentes autónomos se benefician muchísimo de convenciones rígidas.

4. Fallback visuales

   * Si una textura falla:

     * checkerboard magenta
   * Si un modelo falla:

     * cubo placeholder.
   * Esto acelera muchísimo debugging autónomo.

5. Validación de Assimp

   * Faltó:

     ```cpp
     if(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
     ```
   * Es crítico.

**Correcciones sugeridas**:

* Simplificar CO₂ procedural si el calendario aprieta.
* Añadir “OpenGLState.h” o política documentada de estados.
* Añadir placeholders automáticos.
* Definir convención espacial global desde Fase 1.

---

### 4. Criterios de salida — ¿Son verificables por un agente?

**Veredicto**: Correcciones menores

**Análisis**:
En general están muy bien redactados para automatización. Mucho mejor que criterios subjetivos tipo “se ve bien”.

Los mejores:

* “cubo texturizado con matrices MVP correctas”
* “screenshot automático funcional”
* “modelo GLTF renderizado sin warnings”
* “flujo TITULO→JUGANDO→... sin crashes”

Son objetivos y observables.

Los ambiguos:

* “cámara FPS estable”

  * ¿estable = sin jitter?
  * ¿sin NaN?
  * ¿sin drift?

* “specular visible en ángulos oblicuos”

  * Muy subjetivo visualmente.

* “Fresnel visible”

  * También subjetivo.

* “museo navegable”

  * No define colisiones mínimas.

* “build estable 15+ min”

  * No define métricas.

**Correcciones sugeridas**:

* Reemplazar:

  * “cámara FPS estable”
    por:

  ```txt
  Movimiento WASD sin jitter visible; pitch limitado; deltaTime estable; sin inversión de ejes.
  ```

* Reemplazar:

  * “specular visible”
    por:

  ```txt
  Highlight especular observable en screenshot lateral con shininess=32.
  ```

* Reemplazar:

  * “Fresnel visible”
    por:

  ```txt
  Bordes de agua/objetos muestran incremento de intensidad dependiente del ángulo de visión.
  ```

* Reemplazar:

  * “museo navegable”
    por:

  ```txt
  Jugador puede recorrer vestíbulo y ambos corredores sin atravesar límites AABB.
  ```

* Reemplazar:

  * “estable 15+ min”
    por:

  ```txt
  Sin crecimiento continuo de memoria, sin crashes, FPS >45 promedio.
  ```

---

### 5. Riesgos no cubiertos

**Veredicto**: Correcciones menores

**Análisis**:
El plan cubre bien riesgos gráficos y de complejidad. Los huecos principales son de robustez operativa.

Riesgos importantes no mencionados:

1. Dependencia de DLLs Assimp

   * Ya mitigado parcialmente con post-build copy.
   * Bien planteado.

2. Context loss / GL state corruption

   * Claude puede dejar estados globales inconsistentes entre draw calls.

3. Coordenadas inconsistentes entre Blender/GLTF/OpenGL

   * Riesgo MUY común.
   * Especialmente:

     * escala ×100,
     * modelos invertidos,
     * Z-up vs Y-up.

4. Transparencia

   * El agua + partículas + letreros es el punto más peligroso del render pipeline.

5. Fugas GPU

   * Especialmente:

     * texturas,
     * VAOs,
     * cubemap,
     * framebuffers futuros.

6. Unicode/texto

   * stb_truetype puede romper caracteres acentuados si no se define rango glyph correctamente.
   * Importante porque el proyecto está en español.

7. Tiempo de compilación

   * FetchContent + Assimp puede ser pesado para iteraciones frecuentes.

8. Deadlocks de estado narrativo

   * AppState + módulos + triggers pueden entrar en estados imposibles si no hay reset explícito.

**Correcciones sugeridas**:

* Añadir checklist obligatorio por frame:

  ```cpp
  glBindVertexArray(0);
  glUseProgram(0);
  ```

  al final de debug builds.

* Definir:

  ```txt
  Todos los assets exportados:
  - escala 1.0
  - Y-up
  - metros
  - forward -Z
  ```

* Añadir watchdog:

  ```cpp
  if(frame > MAX_TEST_FRAMES) exit(0);
  ```

  para evitar loops infinitos.

* Añadir modo “headless test path”.

  * Cámara automática + activación automática de módulos.

* Añadir charset explícito para stb_truetype:

  ```txt
  ASCII + acentos españoles + signos ¿¡
  ```

---

## Resumen ejecutivo

Las 5 correcciones más importantes son:

1. Añadir verificación temporal y multi-screenshot

   * Un solo screenshot al frame 120 no basta para animaciones, triggers y estados.

2. Definir convenciones globales estrictas

   * Y-up, escala, forward, rutas assets, estados OpenGL.
   * Esto reduce muchísimo errores autónomos.

3. Separar parcialmente Fase 9

   * UI/texto y audio deberían desacoplarse.

4. Introducir debugging persistente

   * `gl_log.txt`, overlay debug y JSON de estado mejoran enormemente la capacidad autónoma de Claude.

5. Formalizar criterios ambiguos

   * “estable”, “visible”, “navegable” deben traducirse a métricas observables.

**Veredicto global**:
El plan es sólido, realista y sorprendentemente bien adaptado para ejecución autónoma por un agente de IA. La arquitectura está correctamente simplificada para un proyecto académico OpenGL 3.3 y evita casi todas las trampas típicas de sobreingeniería. Las correcciones necesarias son principalmente de verificabilidad automática, robustez operativa y reducción de ambigüedad, no de rediseño técnico.
