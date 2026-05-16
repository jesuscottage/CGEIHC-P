# Expectativas Visuales — Lo Que el Usuario Debe Ver

> Este documento describe EXACTAMENTE lo que se espera ver en pantalla en cada posición y momento.
> Cada sección es un "punto de control" que debe coincidir con la realidad del render.
> Si hay CUALQUIER diferencia, es un bug que debe corregirse.

---

## PC-01: Pantalla de Título (frame 0-120, antes de presionar ESPACIO)

**Posición de cámara**: (0, 1.7, 2.0), mirando norte (yaw=90, pitch=0)
**Estado**: AppState::TITULO

**Lo que DEBE verse**:
- Fondo azul oscuro casi negro (glClearColor 0.02, 0.04, 0.12)
- Panel semitransparente centrado con:
  - Título "NUESTRO MUNDO" en azul claro
  - Subtítulo "Polo Norte - Calentamiento Global"
  - Texto descriptivo del museo
  - Instrucción "[ESPACIO] para iniciar" en amarillo
- NO se debe ver geometría 3D (solo el overlay ImGui sobre fondo oscuro)
- NO se deben ver artifacts, parpadeo ni bordes raros

---

## PC-02: Vestíbulo — Primera Vista del Usuario (frame ~130, justo al iniciar)

**Posición de cámara**: (0, 1.7, 2.0), mirando norte (yaw=90, pitch=0)
**Estado**: AppState::JUGANDO

**Lo que DEBE verse**:
- **Suelo**: superficie texturizada (concreto gris) que se extiende en todas direcciones, claramente visible bajo los pies. El suelo debe ser SÓLIDO — no se debe ver agua debajo.
- **Paredes**: a izquierda y derecha, dos muros altos (~6m) con textura de concreto claro, que forman un pasillo ancho. Deben verse como paredes de un edificio real.
- **Techo**: visible arriba como una superficie gris uniforme que cubre el espacio interior.
- **Fondo (norte)**: mirando al frente se deben ver los dos corredores (izquierdo y derecho) divergiendo. Entre ellos, se ve el skybox (cielo nublado) a través de la abertura central.
- **Iluminación**: la escena debe estar bien iluminada — no oscura. La luz hemisférica ártica ilumina desde arriba. Las superficies deben tener sombras suaves, no ser completamente planas.
- **Nieve**: partículas blancas cayendo suavemente en el campo de visión.
- **Agua**: visible SOLO a través de aberturas en las paredes o mirando hacia fuera — NO debajo del suelo.
- **Panel debug**: esquina superior izquierda con FPS, posición, yaw/pitch.

**Problemas probables**:
- Si el suelo no se ve: el quad del suelo puede estar culled (normal incorrecta) o la textura es transparente
- Si se ve agua en vez de suelo: el suelo no se renderiza y el agua (Y=-0.3) es visible por debajo
- Si las paredes son grises planos: la textura no cargó (fallback a baseColor)
- Si está muy oscuro: la dirección de la luz o la iluminación hemisférica están mal configuradas

---

## PC-03: Corredor Izquierdo — Vista hacia M1_IZQ (caminando por corredor izq)

**Posición de cámara**: (-12, 1.7, 9.5), mirando norte (yaw=90, pitch=0)

**Lo que DEBE verse**:
- **Suelo**: concreto texturizado, continuo bajo los pies
- **Paredes**: izquierda (pared exterior X=-20) y derecha (pared interior X=-4), ambas texturizadas
- **Techo**: cubriendo el corredor (gris)
- **Plataforma M1_IZQ**: disco circular azulado/helado (textura de hielo) en el centro del corredor, ligeramente elevado del suelo
- **Iceberg**: bloque ancho azul-blanco sobre la plataforma (visible como un témpano de hielo rectangular pero más ancho que alto)
- **Letreros**: panel de color azul flotando arriba (~5m) del módulo
- **Skybox**: visible a lo lejos a través de la abertura del corredor
- **Al fondo**: se ven M2_IZQ y M3_IZQ más adelante en el corredor
- **Panel narrativo**: si estamos dentro del radio (5m), debe aparecer "Deshielo del Ártico" con opción [E]

---

## PC-04: M1_IZQ Animando (después de presionar E, ~5 segundos)

**Posición de cámara**: (-12, 1.7, 9.5)
**Estado**: M1_IZQ.animT ≈ 0.5

**Lo que DEBE verse**:
- El iceberg ha reducido su tamaño a ~50% del original
- El color ha cambiado de blanco-azul a un azul más oscuro
- El trozo lateral flotante ha desaparecido o está casi invisible
- La plataforma de hielo sigue visible debajo
- Barra de progreso ~50% en el panel narrativo

---

## PC-05: M2_IZQ — Oso Polar (corredor izq, módulo 2)

**Posición de cámara**: (-12, 1.7, 24.5), mirando norte

**Lo que DEBE verse**:
- **Disco de hielo**: gran disco blanco/helado sobre la plataforma
- **Oso polar**: figura reconocible de oso (cuerpo blanco, cabeza, patas) parado sobre el disco
- **Foca cercana**: a la izquierda o derecha del corredor, una forma alargada gris oscuro a nivel del suelo
- Cuando se activa: el disco encoge y el oso "pierde su hábitat"

---

## PC-06: M3_IZQ — Ciudades Inundadas (corredor izq, módulo 3)

**Posición de cámara**: (-12, 1.7, 39.5), mirando norte

**Lo que DEBE verse**:
- **3 edificios**: modelos 3D reales (Kenney) con forma de edificios de oficinas, ventanas rectangulares, color gris claro. Deben verse como edificios reconocibles — NO como bloques gigantes que cubren toda la pantalla.
- **Escala**: cada edificio debe tener ~3-5m de alto (no cubrir toda la vista)
- **Disposición**: los 3 edificios deben estar separados, visibles individualmente alrededor de la plataforma
- Al activar: agua azul sube desde abajo cubriendo gradualmente los edificios

---

## PC-07: M1_DER — Turbina Eólica (corredor derecho, módulo 1)

**Posición de cámara**: (12, 1.7, 9.5), mirando norte

**Lo que DEBE verse**:
- **Turbina**: poste vertical gris (~6m), góndola en la parte superior, 3 palas largas
- La turbina debe ser CLARAMENTE visible desde la posición de la cámara (no microscópica)
- Al activar: las palas empiezan a girar, acelerando progresivamente

---

## PC-08: M2_DER — Auto Eléctrico (corredor der, módulo 2)

**Posición de cámara**: (12, 1.7, 24.5), mirando norte

**Lo que DEBE verse**:
- **Auto**: modelo 3D real (Kenney sedan) con forma reconocible de coche
- El auto debe estar SOBRE la plataforma, a nivel del suelo, con escala razonable (~3m largo)
- El auto debe ser visible de frente o costado — NO invisible ni microscópico
- Al activar: el auto se mueve sinusoidalmente a lo largo de Z

---

## PC-09: M3_DER — Árbol (corredor der, módulo 3)

**Posición de cámara**: (12, 1.7, 39.5), mirando norte

**Lo que DEBE verse**:
- Antes de activar: apenas visible (semilla diminuta)
- Durante animación: árbol cónico verde (modelo Kenney) con tronco marrón, creciendo progresivamente
- Al final: árbol completo de ~3-4m de alto, verde brillante, claramente visible

---

## PC-10: M5 — Globo Terráqueo (sala final)

**Posición de cámara**: (0, 1.7, 59.5), mirando norte

**Lo que DEBE verse**:
- **Globo**: forma reconocible como representación de la Tierra (azul = océano, verde = continentes)
- El globo gira constantemente (~18°/s)
- Elevado ~3m sobre el suelo
- Al activar: líneas/discos de acuerdos aparecen secuencialmente alrededor del globo
- Al completar: se activa pantalla de cierre

---

## PC-11: Vista Exterior (mirando hacia fuera del museo)

**Cualquier posición mirando hacia el borde del museo**

**Lo que DEBE verse**:
- **Océano Ártico**: plano de agua con olas sinusoidales, color turquesa-azul
- **Skybox**: cielo nublado (cubemap de 6 caras)
- **Efecto Fresnel**: el agua refleja más el cielo a ángulos rasantes (horizonte)
- **Espuma**: líneas blancas en las crestas de las olas
- **Transición suave**: la niebla desvanece los objetos lejanos

---

## PC-12: Mirando Directamente al Suelo (pitch=-60 a -90, CUALQUIER posición dentro del museo)

**Lo que DEBE verse**:
- El frame ENTERO cubierto por la textura de concreto gris del suelo
- CERO agua azul o turquesa visible
- CERO skybox (cielo nublado) visible
- La textura debe mostrar variación de detalle (no un gris plano uniforme)
- Si se ven las plataformas circulares de hielo, deben ser de un tono diferente al suelo

**Si NO se ve el suelo**: Bug de winding order → los índices del floorMesh están invertidos (CW en vez de CCW para OpenGL). Fix: invertir índices en Museum.h:buildFloor().

---

## PC-13: Mirando Directamente al Techo (pitch=+45 a +90, CUALQUIER posición dentro del museo)

**Lo que DEBE verse**:
- Superficie gris uniforme cubriendo la parte superior del frame
- NO debe verse cielo nublado a través del techo (excepto en aberturas intencionadas)
- La iluminación debe producir un gradiente suave (más claro en las paredes que en el centro)

---

## PC-14: Acercarse a una Pared (distancia ~0.5m de cualquier pared)

**Lo que DEBE verse**:
- La textura de concreto claro debe ser visible con detalle a corta distancia
- Se deben ver variaciones en la textura (no un color plano gris uniforme)
- La iluminación debe producir un gradiente vertical (más claro arriba, más oscuro abajo)
- Si es color plano uniforme sin variación → la textura no cargó (fallback a baseColor)

---

## PC-02b: Vestíbulo Mirando Hacia Atrás (yaw=270, entrada del museo)

**Posición**: (0, 1.7, 5.0), mirando sur (hacia la entrada)

**Lo que DEBE verse**:
- La pared frontal del museo (Z=-5) cerrando la entrada
- Textura de concreto en la pared
- Suelo texturizado debajo
- A los lados: las paredes laterales del vestíbulo

---

## Reglas Generales (TODA la escena, TODO el tiempo)

1. **El suelo SIEMPRE debe ser visible** dentro del museo — nunca debe "faltar"
2. **Las paredes deben parecer paredes** — con textura, iluminadas, no bloques planos
3. **Los modelos 3D deben tener escala humana** — edificios 3-5m, autos 2-3m, árboles 3-4m
4. **Nada debe ser completamente negro** — la iluminación hemisférica garantiza luz mínima
5. **Los letreros deben ser discretos** — no ocupar un porcentaje grande de la vista
6. **La interfaz ImGui debe ser legible** — fondo semitransparente, texto contrastante
7. **La nieve debe ser sutil** — no obstruir la vista, apenas perceptible
8. **El agua solo debe verse fuera del museo** — no a través del suelo
9. **Ningún objeto interactivo debe ocupar <2% de la pantalla** desde la posición de test
10. **El objeto principal de cada módulo debe ocupar >15% del alto de pantalla**
11. **El museo debe ser reconocible como espacio arquitectónico interior** — no como espacio abstracto
