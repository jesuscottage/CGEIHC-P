---
title: Plan de mejoras visuales post-revisión
date: 2026-05-27
status: activo
prioridad: alta
---

# Plan de Mejoras Visuales — Post-revisión del Profesor

> **Observación del profesor**: "Mejorar: texturizado, iluminación, materiales, el piso, animaciones básicas."

## Flujo de trabajo por mejora

Cada mejora sigue este ciclo:

```
1. Claude explica qué vamos a mejorar y dónde se ve en la app
2. El usuario entra a la app, verifica el estado actual, aprueba
3. Decisiones conjuntas (texturas, parámetros, assets necesarios)
4. Claude implementa cambios en código
5. El usuario verifica resultado final, aprueba o da retroalimentación
6. Se documenta en la bitácora (entregables/bitacora-mejoras.md)
```

---

## Mejora 1 — Textura en paredes y techo

### Qué mejorar
Las paredes y el techo del museo son color sólido gris sin textura. Las paredes usan `baseColor = (0.72, 0.76, 0.80)` y el techo `(0.55, 0.62, 0.70)`. Aunque `wallTex` se carga desde `textures/floor.png`, la textura actual no es apropiada para paredes. El techo tiene `useTexture=false` hardcodeado.

### Dónde verlo
- Abrir la app → mirar cualquier pared lateral (especialmente en los corredores)
- Mirar hacia arriba (techo completamente plano, sin detalle)

### Archivos a modificar
- `app/src/scene/Museum.h` — líneas 93-94 (carga de texturas), líneas 129-136 (render techo)
- `app/assets/textures/` — agregar texturas nuevas (concreto, hielo para paredes)

### Decisiones a tomar juntos
- Qué textura usar para paredes (concreto claro, hielo bruñido, bloques de hielo)
- Qué textura usar para techo (metal, hielo translúcido, o la misma que paredes)
- El usuario busca/proporciona las texturas CC0 o Claude sugiere opciones

### Cambios técnicos
1. Agregar `ceilTex` al `Museum` (actualmente no existe)
2. En `buildFloor()` — asegurar UVs correctas en el mesh del techo
3. En `draw()` — activar `useTexture=true` para el techo y asignar `ceilTex`
4. Reemplazar la textura de paredes por una apropiada
5. Ajustar escala de UVs si el tiling es muy evidente

### Riesgo
Bajo. Solo cambia texturas y un booleano. No afecta geometría ni lógica.

---

## Mejora 2 — Point lights en exhibiciones

### Qué mejorar
El museo tiene solo luz hemisférica global + un sol direccional. Las exhibiciones se ven igual que los pasillos vacíos. Falta iluminación dramática tipo spotlight sobre cada módulo.

### Dónde verlo
- Caminar por cualquier corredor → todo se ve uniformemente iluminado
- Comparar una exhibición activa vs un pasillo vacío → misma luz

### Archivos a modificar
- `app/shaders/standard.frag` — agregar uniforms de point lights
- `app/src/main.cpp` — líneas ~830-835 (setup de uniforms de iluminación)

### Decisiones a tomar juntos
- Cuántos point lights (1 por módulo = 7, o solo 3-4 clave)
- Color de las luces (blanco cálido, o color por módulo)
- Intensidad y radio de atenuación

### Cambios técnicos
1. En `standard.frag`: agregar array `uniform vec3 pointLightPos[N]` + `pointLightColor[N]`
2. En el loop del fragment shader: acumular contribución difusa + especular de cada point light
3. En `main.cpp`: setear posiciones (centro de cada módulo, Y=5m) y colores antes de cada frame
4. Atenuación: `1.0 / (1.0 + 0.09*d + 0.032*d*d)` (modelo cuadrático estándar)

### Riesgo
Medio. Modificar el fragment shader puede afectar el aspecto de toda la escena. Hay que calibrar intensidades para que no quemen ni oscurezcan.

---

## Mejora 3 — Materiales diferenciados (shininess/fresnel)

### Qué mejorar
Un solo valor `shininess=48` y un solo `fresnel0` para todo. Hielo, metal, paredes, agua — todos reflejan igual. El hielo debería ser brillante (128+), las paredes mate (16-32), el metal muy reflectivo (64+).

### Dónde verlo
- Mirar el piso de hielo y una pared → mismo brillo especular
- El globo metálico (M5) y una pared → misma reflexión

### Archivos a modificar
- `app/src/main.cpp` — línea 832 (`shininess = 48`)
- `app/src/scene/Museum.h` — antes de cada draw call, setear shininess/fresnel apropiados

### Decisiones a tomar juntos
- Tabla de valores: hielo, paredes, metal, modelos

### Cambios técnicos
1. Antes de `floorMesh.draw()`: `shininess=128`, `fresnel0=(0.04)`
2. Antes de paredes: `shininess=16`, `fresnel0=(0.03)`
3. Antes de techo: `shininess=8`, `fresnel0=(0.02)`
4. Antes de plataformas: `shininess=96`, `fresnel0=(0.05)`
5. En ModuleScene: valores por tipo de objeto (turbina metálica=64, edificios=16, agua=256)

### Riesgo
Bajo. Solo cambia uniforms float/vec3 antes de cada draw call. No modifica geometría.

---

## Mejora 4 — Fix texturas modelos Kenney (colormap.png)

### Qué mejorar
Los modelos GLB de Kenney (pino, auto, edificios A/B/C, globo) buscan `Textures/colormap.png` que no existe. Los modelos se renderizan con color override sólido en lugar de sus texturas originales.

### Dónde verlo
- Los edificios en M3_IZQ son bloques blancos/grises sin detalle
- El auto en M2_DER es un bloque azul sólido
- Los pinos son verdes uniformes

### Archivos a modificar
- `app/assets/models/Textures/` — crear carpeta y agregar `colormap.png`
- O modificar `Model.h` para buscar la textura en la ruta correcta

### Decisiones a tomar juntos
- Descargar la textura `colormap.png` original de Kenney (es CC0, viene con los packs)
- O extraerla del GLB si está embebida pero la ruta relativa falla

### Cambios técnicos
1. Identificar de qué pack de Kenney vienen los modelos
2. Descargar `colormap.png` y colocarla en `app/assets/models/Textures/`
3. Verificar que los 6 modelos la encuentren automáticamente

### Riesgo
Bajo. Solo agregar un archivo de textura. No cambia código.

---

## Mejora 5 — Humo semitransparente (alpha blending)

### Qué mejorar
El humo de la fábrica (M3_DER) se dibuja como discos opacos oscuros flotando sobre las chimeneas. Parecen platos, no humo.

### Dónde verlo
- Ir a M3_DER (Captura de Carbono) → los discos oscuros sobre las chimeneas

### Archivos a modificar
- `app/src/scene/ModuleScene.h` — función `drawFactory()` / sección de humo
- `app/shaders/standard.frag` — posiblemente agregar soporte de alpha

### Decisiones a tomar juntos
- Opacidad del humo (0.3-0.5 sugerido)
- Si usar `standard.frag` con alpha o crear un shader unlit con alpha

### Cambios técnicos
1. Antes de dibujar humo: `glEnable(GL_BLEND)`, `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`
2. Modificar `FragColor` para incluir alpha: `FragColor = vec4(color, alpha)`
3. Agregar uniform `float alpha` al shader (o usar un flag)
4. Después de dibujar humo: `glDisable(GL_BLEND)` (restaurar estado)
5. Opcionalmente: hacer que el humo suba suavemente con `time`

### Riesgo
Bajo-Medio. El blending requiere ordenar objetos transparentes después de los opacos, pero si solo son los discos de humo es manejable.

---

## Mejora 6 — Easing en animaciones

### Qué mejorar
Todas las animaciones usan `animT` lineal (0→1 a velocidad constante). El movimiento se siente robótico. Un ease-out cúbico haría que el inicio sea rápido y el final suave (más natural).

### Dónde verlo
- Activar cualquier módulo con E → la animación avanza a velocidad constante sin aceleración/desaceleración

### Archivos a modificar
- `app/src/scene/ModuleScene.h` — función `draw()`, donde se pasa `animT`
- O directamente en `Museum.h` donde se calcula `animT`

### Decisiones a tomar juntos
- Tipo de easing: ease-out (rápido→lento), ease-in-out (lento→rápido→lento), u otro
- Si aplicar a todos los módulos o selectivamente

### Cambios técnicos
1. Agregar función helper:
   ```cpp
   float easeOutCubic(float t) { return 1.0f - powf(1.0f - t, 3.0f); }
   ```
2. En `ModuleScene::draw()`: usar `easeOutCubic(animT)` en lugar de `animT` directamente
3. No modificar la lógica de `animT` en Museum (el timer lineal sigue igual, solo se transforma al usarlo)

### Riesgo
Bajo. Solo transforma un float antes de pasarlo a las funciones de draw. No cambia lógica.

---

## Mejora 7 — Bobbing en fauna

### Qué mejorar
Los animales (lobos, pingüinos) se deslizan por el suelo sin movimiento vertical. Falta oscilación sutil tipo "caminata" (subir-bajar rítmico).

### Dónde verlo
- Observar un pingüino patrullando → se desliza perfectamente horizontal

### Archivos a modificar
- `app/src/scene/DecoScene.h` — función `patrol()` (línea ~218) y los draw calls de fauna

### Decisiones a tomar juntos
- Amplitud del bobbing (0.05m-0.15m sugerido)
- Frecuencia (sincronizada con velocidad de patrulla)

### Cambios técnicos
1. En `patrol()` o en cada draw de fauna: agregar `pos.y += amplitude * abs(sin(t * freq))`
2. Usar `abs(sin())` para que siempre sea positivo (el animal sube y baja, nunca baja del suelo)
3. Frecuencia proporcional a velocidad: caminata lenta = bobbing lento

### Riesgo
Bajo. Solo modifica la posición Y de los modelos de fauna. No afecta colisiones ni otra lógica.

---

## Mejora 8 — Detail map en piso

### Qué mejorar
El piso usa una sola textura `ice.png` con UVs de 8×16 tiles. El patrón repetitivo es visible a media distancia.

### Dónde verlo
- Mirar el piso desde un corredor → el patrón se repite visiblemente

### Archivos a modificar
- `app/shaders/standard.frag` — agregar segunda textura de detalle
- `app/src/scene/Museum.h` — bindear textura de detalle adicional
- `app/assets/textures/` — agregar textura de detalle (ruido, grietas, nieve)

### Decisiones a tomar juntos
- Textura de detalle a usar (noise, snow detail, scratch marks)
- Escala del detail map (3× - 5× más detallado que la textura base)
- Ya existe `snow_detail.png` en assets que podría servir

### Cambios técnicos
1. Agregar `uniform sampler2D detailMap` y `uniform bool useDetail` al fragment shader
2. En el shader: `albedo *= mix(vec3(1.0), texture(detailMap, TexCoords * detailScale).rgb, 0.3)`
3. En Museum: bindear textura de detalle en unidad 1 solo para el piso
4. Para el resto de objetos: `useDetail=false`

### Riesgo
Medio. Modifica el fragment shader global. Hay que asegurar que `useDetail=false` para todo lo demás.

---

## Mejora 9 — Plataformas diferenciadas

### Qué mejorar
Las plataformas circulares de cada exhibición usan la misma textura `ice.png` que el piso. No resaltan visualmente.

### Dónde verlo
- Mirar hacia abajo en cualquier módulo → el disco se confunde con el piso

### Archivos a modificar
- `app/src/scene/Museum.h` — sección de render de plataformas (líneas 157-169)
- `app/assets/textures/` — opcionalmente agregar textura de plataforma

### Decisiones a tomar juntos
- Color o textura diferente (más oscuro, borde iluminado, color por módulo)
- O usar el labelColor de cada módulo como tinte

### Cambios técnicos
1. Opción A: tinte de color — `baseColor = mod.labelColor * 0.3 + iceColor * 0.7`
2. Opción B: textura diferente (metal, piedra pulida)
3. Opción C: borde brillante — anillo exterior con color del módulo

### Riesgo
Bajo. Solo cambia color/textura de los discos. No afecta geometría ni lógica.

---

## Mejora 10 — Idle sutil en módulos

### Qué mejorar
Los objetos de cada exhibición están completamente estáticos hasta que el usuario presiona E. Un movimiento sutil (balanceo, respiración, rotación lenta) los haría más atractivos y vivos.

### Dónde verlo
- Acercarse a cualquier módulo sin activar → el modelo está congelado en su pose inicial

### Archivos a modificar
- `app/src/scene/ModuleScene.h` — cada función `drawXxx()`, agregar movimiento con `time` cuando `animT == 0`

### Decisiones a tomar juntos
- Qué tipo de idle por módulo (rotación lenta, balanceo, flotación)
- Amplitud (sutil, 1-3 grados o 0.05-0.1m)

### Cambios técnicos
1. En cada draw function: si `animT < 0.01` (no activado), aplicar transformación sutil con `time`:
   - Iceberg: flotar Y ±0.1m con `sin(time)`
   - Turbina: girar muy lento (~1 RPM)
   - Auto: nada (estático tiene sentido)
   - Globo: rotación lenta (ya existe parcialmente)
2. No afecta la animación principal — al activar, el idle se reemplaza

### Riesgo
Bajo. Movimiento condicional solo cuando `animT ≈ 0`. No interfiere con animación principal.

---

## Orden sugerido de implementación

| Orden | Mejora | Justificación |
|-------|--------|--------------|
| 1 | **M3: Materiales diferenciados** | Cambio más rápido, impacto inmediato en toda la escena |
| 2 | **M4: Fix colormap.png** | Solo agregar un archivo, 6 modelos mejoran automáticamente |
| 3 | **M6: Easing animaciones** | Una función de 1 línea transforma todas las animaciones |
| 4 | **M7: Bobbing fauna** | Una línea extra en patrol(), mejora toda la fauna |
| 5 | **M1: Textura paredes/techo** | Impacto visual enorme, requiere elegir texturas |
| 6 | **M9: Plataformas diferenciadas** | Cambio rápido de color/textura |
| 7 | **M2: Point lights** | Mayor complejidad (shader + uniforms), pero mayor impacto en iluminación |
| 8 | **M5: Humo semitransparente** | Requiere blending, riesgo medio |
| 9 | **M8: Detail map piso** | Modifica shader global, requiere calibración |
| 10 | **M10: Idle módulos** | Prioridad baja, solo polish |

> **Estrategia**: empezar por cambios de bajo riesgo y alto impacto (M3, M4, M6, M7), después los que requieren assets nuevos (M1, M9), y al final los que modifican el shader principal (M2, M5, M8).
