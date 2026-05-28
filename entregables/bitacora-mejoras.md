# Bitácora de Mejoras Visuales — Proyecto "Nuestro Mundo: Polo Norte"

> **Materia**: Computación Gráfica e Interacción Humano-Computadora (CGEIHC)
> **Semestre**: 2026-2 — UNAM, Facultad de Ingeniería
> **Docente**: Dr. Sergio Teodoro Vite
> **Fecha de inicio**: 2026-05-27

## Equipo

| Nombre | Cuenta |
|--------|--------|
| Hernández Cabañas Jesús | 422062224 |
| Navarro Nuño Juan Pablo | 319086779 |
| Macias Niño Carmen Violeta | 318285450 |
| Martinez Jimenez Israel | 312032184 |

---

## Contexto

Tras la revisión del proyecto funcional por parte del profesor, se recibió la siguiente observación:

> "Mejorar: texturizado, iluminación, materiales, el piso, animaciones básicas."

Se realizó un diagnóstico visual completo del proyecto ejecutando la aplicación y capturando screenshots desde cada zona del museo (vestíbulo, M1-M3 izquierdo, M1-M3 derecho, M5) en múltiples ángulos (frente, piso, techo, animación intermedia, animación final).

### Hallazgos del diagnóstico

| Área | Estado antes | Problema detectado |
|------|-------------|-------------------|
| Paredes | Color gris sólido `(0.72, 0.76, 0.80)` | Sin textura — superficies planas sin detalle |
| Techo | Color gris-azul sólido | Sin textura — completamente plano |
| Iluminación | Hemisférica uniforme | Sin luces puntuales, exhibiciones sin spotlight |
| Materiales | Shininess global único | Hielo, metal, paredes — todos reflejan igual |
| Modelos Kenney | Color override sólido | Texturas `colormap.png` no encontradas |
| Humo fábrica | Discos opacos oscuros | Sin alpha blending — parecen platos, no humo |
| Animaciones | Interpolación lineal | Sin easing — movimiento robótico |
| Fauna | Deslizamiento estático | Sin bobbing vertical al patrullar |
| Piso | Textura ice.png funcional | Tiling repetitivo, sin detail map |
| Plataformas | Misma textura que el piso | No se distinguen visualmente |

---

## Registro de mejoras

<!-- Cada mejora se documenta con: fecha, descripción, decisiones tomadas, cambios realizados, resultado -->

### Mejora 1 — Textura en paredes y techo

**Estado**: Descartada
**Motivo**: Tras revisión manual del proyecto por el equipo, las paredes y el techo se ven aceptables con las texturas actuales. No se justifica el cambio.

---

### Mejora 2 — Point lights en exhibiciones

**Estado**: Implementada
**Fecha**: 2026-05-27

**Decisiones tomadas**:
- 8 point lights: 7 en módulos (M1-M3 izquierdo, M1-M3 derecho, M5) + 1 en vestíbulo (spawn)
- Color: blanco cálido `(0.95, 0.85, 0.65)` con intensidad sutil (`× 0.45`)
- Posición: centro de cada módulo a Y=5m (1m debajo del techo de 6m)
- Atenuación cuadrática estándar: `1 / (1 + 0.09d + 0.032d²)`, radio efectivo ~12m
- Iluminación ambiental reducida de `× 1.0` a `× 0.70` para que las luces puntuales destaquen
- Fill light reducido de `0.35` a `0.25` para mayor contraste

**Archivos modificados**:
- `app/shaders/standard.frag` — agregado array de 8 point lights con cálculo Blinn-Phong + Fresnel por cada luz puntual; reducida iluminación hemisférica y fill light
- `app/src/main.cpp` — seteo de uniforms `pointLightPos[]` y `pointLightColor[]` usando posiciones de módulos + luz adicional en vestíbulo

**Resultado**: Las exhibiciones ahora tienen un resplandor cálido que las distingue de los pasillos. La reducción del ambiente genera contraste entre zonas iluminadas y oscuras. FPS estables.

**Corrección adicional 1 — Estalactitas pegadas al techo**: Las estalactitas de hielo (pirámides invertidas en el corredor izquierdo) estaban flotando a Y=4.8m, 1.2m debajo del techo. Se movieron a Y=6.0m para que cuelguen directamente del techo. Verificado visualmente.
- `app/src/scene/DecoScene.h` — posición Y de estalactitas de `4.8` a `6.0`

**Corrección adicional 2 — Letreros opacos desde todos los ángulos**: Los letreros 3D (cruces de color sobre cada módulo) se veían transparentes desde algunos ángulos debido al face culling de OpenGL. Se desactivó el culling para los letreros (`glDisable(GL_CULL_FACE)` antes de `drawSigns`, `glEnable` después) para que todas las caras exteriores sean siempre visibles.
- `app/src/main.cpp` — `glDisable(GL_CULL_FACE)` envolviendo `museum.drawSigns()`

**Corrección adicional 3 — Orientación del oso polar (M2_IZQ)**: El modelo `polar_bear.glb` aparecía con la cabeza apuntando al suelo. Se corrigió agregando una rotación de -90° en el eje X para que el animal quede parado correctamente sobre sus patas.
- `app/src/scene/ModuleScene.h` — `glm::rotate(m, glm::radians(-90.f), {1,0,0})` en `drawIceFloe()`

**Corrección adicional 4 — Turbina eólica traslapando letrero (M1_DER)**: Las aspas de la turbina eólica se traslapaban con el letrero 3D del módulo (Y=4.2m). Se bajó la turbina de `c.y - 3.8` a `c.y - 5.5`, alejando el hub del rotor 1.7m más abajo del letrero.
- `app/src/scene/ModuleScene.h` — offset Y de turbina de `-3.8` a `-5.5`

**Ajuste adicional — Luz ambiental reducida**: La iluminación ambiental hemisférica se redujo de `× 0.70` a `× 0.55` para que los point lights de las exhibiciones contrasten más con los pasillos.
- `app/shaders/standard.frag` — multiplicador de `ambLight` de `0.70` a `0.55`

**Corrección adicional 5 — Reset automático de animaciones**: Al completarse una animación (`animT >= 1.0`), la barra de progreso mostraba "Animación completada" y se quedaba visible indefinidamente, obstruyendo la vista. Se agregó un timer de 2 segundos post-completado tras el cual el módulo se resetea a su estado original (`animT = 0`, `animActive = false`) y la barra desaparece. Esto permite al usuario volver a activar la animación con E.
- `app/src/main.cpp` — variable `completionTimer` + lógica de reset cuando `completionTimer >= 2.0f`

**Corrección adicional 6 — Pinturas de galería al revés**: Las texturas de las pinturas se cargaban con `flip = false`, lo que causaba que las imágenes aparecieran invertidas verticalmente (el origen del PNG es arriba-izquierda, pero OpenGL espera abajo-izquierda). Se cambió a `flip = true` para que las pinturas se muestren con la orientación correcta.
- `app/src/scene/GalleryScene.h` — `mTex[i].load(path, false)` cambiado a `mTex[i].load(path, true)`

**Corrección adicional 7 — Auto eléctrico orientado al movimiento (M2_DER)**: El modelo del auto Kenney se movía sinusoidalmente en el eje Z pero estaba rotado 90° en Y, lo que hacía que pareciera desplazarse de lado. Se eliminó la rotación de 90° para que el auto mire en la dirección de su movimiento (adelante y atrás).
- `app/src/scene/ModuleScene.h` — eliminado `glm::rotate(m, glm::radians(90.f), {0,1,0})` en `drawElectricCar()`

**Corrección adicional 8 — Humo de fábrica solo con activación (M3_DER)**: El humo de las chimeneas del módulo Captura de Carbono se dibujaba siempre, incluso antes de que el usuario activara la animación con E. Se condicionó a `t > 0` para que solo aparezca cuando la animación está en curso.
- `app/src/scene/ModuleScene.h` — `drawSmoke()` envuelto en `if (t > 0.0f)` dentro de `drawTree()`

**Corrección adicional 9 — Agua de ciudades inundadas (M3_IZQ)**: El cubo azul que simula el agua tenía problemas de visibilidad por face culling (algunas caras no se veían desde ciertos ángulos) y era demasiado pequeño (7m) para cubrir todos los edificios. Se desactivó el culling para el cubo de agua y se aumentó el tamaño de 7×7m a 12×12m.
- `app/src/scene/ModuleScene.h` — `glDisable(GL_CULL_FACE)` para el agua + escala de `{7, 0.16, 7}` a `{12, 0.16, 12}`

**Corrección adicional 10 — Globo terráqueo procedural (M5)**: El módulo Acuerdos por el Clima usaba un modelo DamagedHelmet (`globe.glb`) que no representaba la Tierra. Se reemplazó por una esfera UV procedural (`makeSphere()` en Mesh.h, 24 stacks × 32 slices) con océano azul, 5 masas continentales verdes (Europa/África, Asia, Norteamérica, Sudamérica, Australia), casquetes polares blancos e inclinación axial de 23.4°. La animación ahora está controlada por `animT`: a t=0 la esfera es pequeña y estática; a t=1 alcanza tamaño completo y rota a 30°/s.
- `app/src/graphics/Mesh.h` — nueva función `makeSphere()`
- `app/src/scene/ModuleScene.h` — `mSphere` como miembro + reescritura completa de `drawGlobe()` con esfera, continentes y polos procedurales

**Corrección adicional 11 — Animación de deshielo del Ártico (M1_IZQ)**: El módulo Deshielo del Ártico solo encogía el iceberg, sin efecto visual de agua derretida. Se agregó un charco de agua procedural (cubo azul con culling desactivado) que crece en radio (0.5m → 6m) y altura (0.02m → 0.20m) conforme `animT` avanza. Esto hace visible el efecto de deshielo: el iceberg se derrite y el agua sube.
- `app/src/scene/ModuleScene.h` — bloque `if (t > 0.05f)` al final de `drawIceberg()` con cubo de agua creciente

**Corrección adicional 12 — Paredes del final de los corredores**: Las paredes interiores (X=±4.25) se extendían de Z=10 a Z=70, bloqueando visualmente el paso hacia la sala M5 (que comienza en Z=60). Se acortaron a Z=10–Z=60 (longitud de 60 a 50). También se eliminaron las paredes traseras de los corredores en Z=65 que impedían la conexión visual con M5. Ahora los tres pasillos (izquierdo, central y derecho) convergen sin obstrucción en la sala final.
- `app/src/scene/Museum.h` — paredes interiores: centro Z de `40` a `35`, longitud de `60` a `50`; eliminadas las dos paredes traseras de corredor en Z=65

---

**Corrección adicional 13 — Iceberg no visible (M1_IZQ)**: El modelo `iceberg.glb` se escalaba demasiado (factor 5.0 × 2.5 = 12.5 en X), haciendo que la cámara quedara dentro del iceberg y no se viera. Se redujo la escala inicial de `5.0` a `1.8` para que el iceberg sea visible desde la posición del jugador (~4.5m × 3.6m × 3.96m en mundo).
- `app/src/scene/ModuleScene.h` — escala en `drawIceberg()` de `glm::mix(5.0f, 0.3f, t)` a `glm::mix(1.8f, 0.15f, t)`

**Corrección adicional 14 — Letreros morados uniformes**: Los letreros 3D de cada módulo tenían colores individuales (azul, verde, amarillo, etc.). Se cambió a morado fijo `(0.55, 0.15, 0.75)` para todos los módulos, mejorando la identidad visual y visibilidad.
- `app/src/scene/Museum.h` — `baseColor` de `mod.labelColor` a `glm::vec3(0.55f, 0.15f, 0.75f)` en `drawSigns()`

---

### Mejora 3 — Materiales diferenciados (shininess/fresnel)

**Estado**: Pendiente

---

### Mejora 4 — Fix texturas modelos Kenney (colormap.png)

**Estado**: Pendiente

---

### Mejora 5 — Humo semitransparente (alpha blending)

**Estado**: Pendiente

---

### Mejora 6 — Easing en animaciones

**Estado**: Pendiente

---

### Mejora 7 — Bobbing en fauna

**Estado**: Pendiente

---

### Mejora 8 — Detail map en piso

**Estado**: Pendiente

---

### Mejora 9 — Plataformas diferenciadas

**Estado**: Pendiente

---

### Mejora 10 — Idle sutil en módulos

**Estado**: Pendiente

---

## Conclusiones

*(Se completará al finalizar todas las mejoras.)*
