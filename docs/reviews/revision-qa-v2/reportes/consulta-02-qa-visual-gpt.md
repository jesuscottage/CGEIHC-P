## Evaluación

El plan QA v2 es muchísimo mejor que una QA “tomar screenshot y decir OK”, pero todavía **NO habría garantizado detectar todos los bugs reportados por el usuario**.

Sí detectaría varios problemas graves, pero todavía le faltan:

* validaciones perceptuales/globales,
* recorridos sistemáticos de cámara,
* métricas automáticas,
* y checks de “legibilidad espacial”.

### ¿Qué bugs sí detectaría?

| Bug reportado                   | ¿El plan v2 lo detecta? | Razón                                                                                                                                  |
| ------------------------------- | ----------------------- | -------------------------------------------------------------------------------------------------------------------------------------- |
| Suelo invisible                 | Parcialmente sí         | PC-02 menciona “suelo de concreto texturizado”. Si el QA compara activamente contra expectativa, debería fallar inmediatamente.        |
| Paredes grises abstractas       | Parcialmente sí         | PC-02/03 hablan de “museo”, corredores y paredes definidas. Pero falta un criterio explícito de “arquitectura reconocible”.            |
| Escalas incorrectas             | Sí, parcialmente        | PC-06 exige “3 edificios reconocibles” y PC-08 “auto reconocible”. Si el auto es microscópico o un edificio mide 300m, debería fallar. |
| Letreros como líneas diagonales | No de forma robusta     | Solo dice “letreros discretos”; no valida legibilidad, orientación ni clipping geométrico.                                             |
| “No parece museo”               | NO                      | Falta una evaluación global de identidad visual/espacial. Los PCs validan objetos aislados, no coherencia ambiental.                   |

El problema raíz del QA original no fue solo “falta de screenshots”. Fue:

> El agente verificó presencia de píxeles, no significado visual.

Eso es crítico. Un agente multimodal puede “ver”, pero si no tiene una expectativa explícita de:

* escala,
* composición,
* continuidad espacial,
* arquitectura,
* semántica visual,
  entonces puede aceptar basura visual técnicamente renderizada.

---

## Expectativas Faltantes

| #  | Qué falta                                                   | Por qué es crítico                                                |
| -- | ----------------------------------------------------------- | ----------------------------------------------------------------- |
| 1  | “El museo debe ser reconocible como espacio arquitectónico” | Detecta el problema de “bloques abstractos”.                      |
| 2  | Validación explícita de escala humana                       | Sin referencia humana, un auto gigante puede “pasar”.             |
| 3  | Continuidad del suelo                                       | Evita agujeros, z-fighting, planos invertidos o ausencia de mesh. |
| 4  | Validación de legibilidad de letreros                       | Detecta UVs corruptos, clipping y billboards rotos.               |
| 5  | Validación de orientación de normales                       | Muchos bugs OpenGL son normales invertidas/culling.               |
| 6  | “Nada importante debe ocupar <1% de pantalla”               | Detecta objetos invisibles por escala/cámara.                     |
| 7  | Test de coherencia espacial                                 | ¿Los corredores conectan lógicamente?                             |
| 8  | Validación de iluminación mínima                            | Evita escenas negras o sin contraste.                             |
| 9  | Validación de horizon line                                  | Detecta cámaras mal posicionadas o escalas absurdas.              |
| 10 | “El agua no invade interiores”                              | Importantísimo para detectar clipping/masks incorrectas.          |
| 11 | Test de navegación humana                                   | Verificar que el usuario entiende hacia dónde avanzar.            |
| 12 | Evaluación de primer impacto visual                         | Los primeros 3 segundos son críticos para UX.                     |

---

## Mejoras al Plan QA

| #  | Mejora                      | Cómo implementar                                       |
| -- | --------------------------- | ------------------------------------------------------ |
| 1  | Capturas sistemáticas 360°  | En cada PC: yaw = 0°, 45°, 90°, 135°, 180°, etc.       |
| 2  | Capturas verticales         | Mirar suelo (-60°), horizonte (0°), techo (+60°).      |
| 3  | Test de primer frame        | Screenshot automático al iniciar app SIN mover cámara. |
| 4  | Comparación side-by-side    | Mostrar EXPECTATIVA vs REAL simultáneamente.           |
| 5  | Bounding box validation     | Verificar tamaños relativos automáticamente.           |
| 6  | Heatmap de cobertura visual | Detectar escenas vacías o negras.                      |
| 7  | QA en wireframe             | Detecta geometría inexistente o colapsada.             |
| 8  | Path QA automatizado        | Recorrido fijo con checkpoints reproducibles.          |
| 9  | Escala humana de referencia | Insertar dummy humano de 1.8m opcionalmente.           |
| 10 | Histogramas de color        | Detectar “todo gris” o “todo negro”.                   |
| 11 | Validación temporal         | Verificar animaciones frame-by-frame.                  |
| 12 | Diff perceptual             | SSIM/LPIPS entre referencia y captura.                 |
| 13 | Overlay de ejes             | Diagnóstico rápido de transforms incorrectas.          |
| 14 | Captura de depth            | Detecta clipping y planos invertidos.                  |
| 15 | Estado semántico            | “¿Se entiende qué estoy viendo?” checklist explícito.  |

---

## Cómo Evaluar "Bonito" Objetivamente

Un agente no necesita “gustos humanos”. Necesita medir propiedades correlacionadas con buena percepción visual.

### Métricas útiles

#### 1. Cobertura visual

* % pantalla ocupada por geometría útil.
* Detecta:

  * vacío,
  * escenas minimalistas accidentales,
  * objetos diminutos.

Ejemplo:

* Si el museo ocupa solo 20% de la pantalla → probable fallo de escala.

---

#### 2. Contraste local

Mide:

* diferencia luminancia entre objetos y fondo.

Detecta:

* escenas lavadas,
* objetos invisibles,
* señalética ilegible.

---

#### 3. Distribución de color

Histogramas RGB/HSV.

Detecta:

* “todo gris”,
* “todo negro”,
* texturas faltantes.

Muy útil para:

* materiales OpenGL sin textura binded.

---

#### 4. Filling ratio

Cuánto espacio visual tiene:

* suelo,
* paredes,
* props,
* cielo.

Si el suelo visible = 0%:
→ bug inmediato.

---

#### 5. Sharp edge density

Cantidad de bordes detectables.

Muy bajo:

* escena vacía,
* sin detalle,
* materiales planos.

Muy alto:

* z-fighting,
* ruido visual,
* wireframe accidental.

---

#### 6. Semantic recognizability

Usar VLM/multimodal prompts:

* “¿Esto parece un museo?”
* “¿Ves un auto?”
* “¿Hay arquitectura interior?”

Esto es MUCHO más potente que “¿hay píxeles?”.

---

#### 7. Relative scale checks

Reglas automáticas:

* Auto ≈ 1.5–2m alto
* Puerta ≈ 2–3m
* Turbina visible desde distancia media
* Oso polar > foca

Esto elimina la mayoría de errores GLTF/GLB.

---

### ¿Comparar contra referencias reales?

Sí, muchísimo.

No necesariamente contra “otros museos”, sino contra:

* renders esperados,
* mockups,
* concept art,
* screenshots gold master.

La QA visual moderna funciona muchísimo mejor con:

* “golden images”
* perceptual diff
* semantic diff

No basta con “se renderiza”.

---

## Técnicas de Diagnóstico Adicionales

Tu lista original es buena, pero faltan varias herramientas extremadamente útiles.

### Rendering Modes

* Wireframe rendering

  * Detecta geometría faltante
  * Escalas absurdas
  * Mesh degenerado

* Unlit rendering

  * Aísla problemas de iluminación

* Albedo-only mode

  * Detecta texturas incorrectas

* UV checker texture

  * Detecta UV rotos/distorsionados

---

### Buffers y visualizaciones

* Depth buffer visualization

  * Detecta clipping y z-fighting

* Normal visualization

  * RGB = XYZ normal
  * Detecta normales invertidas

* World position visualization

  * Muy útil para transforms rotos

* Shadow map debug

  * Detecta sombras corruptas

* Overdraw visualization

  * Detecta geometría superpuesta accidental

---

### Diagnóstico geométrico

* Bounding boxes visibles
* Gizmos de pivote
* Mostrar origin local
* Mostrar escala XYZ
* Mostrar ejes globales

---

### Diagnóstico de cámara

* FOV stress test
* Near/far plane visualization
* Camera collision debug

---

### Diagnóstico temporal

* Slow-motion mode
* Frame stepping
* Captura cada N frames
* Curvas de animación visibles

---

### Diagnóstico de assets

* Verificador automático GLTF:

  * escala,
  * materiales,
  * texturas faltantes,
  * normales,
  * tangentes.

---

## Top 5 Cambios para Plan Infalible

1. Agregar “QA semántica”

   * El agente debe responder:

     * “¿Esto parece un museo?”
     * “¿Entiendo qué estoy viendo?”
     * “¿Los objetos son reconocibles?”

2. Implementar capturas sistemáticas de cámara

   * 360°
   * suelo
   * techo
   * vista cenital
   * distancia larga/corta

3. Crear imágenes de referencia (“golden screenshots”)

   * Comparación perceptual automática.
   * El QA no debería depender solo de criterio textual.

4. Agregar modos debug visuales

   * Wireframe
   * normales
   * depth
   * UV checker
   * unlit

5. Definir reglas cuantitativas de escala y visibilidad

   * “ningún objeto crítico <2% pantalla”
   * “suelo visible >15%”
   * “objetos interactivos reconocibles desde 5m”
   * “arquitectura interior claramente delimitada”

El salto importante aquí es pasar de:

> “¿La app renderiza?”

a:

> “¿La experiencia visual comunica correctamente el mundo diseñado?”
