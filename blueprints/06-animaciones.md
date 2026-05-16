# Doc 06 — Animaciones

> Estado: COMPLETO
> Última actualización: 2026-05-15

---

## Clasificación por Tipo (Rúbrica)

| Tipo | Animaciones que lo cubren |
|------|--------------------------|
| **Básica** — transformaciones directas | Rotación constante del globo (M5), aceleración de aspas de turbina (M1 der) |
| **Keyframe** — LERP/SLERP | Scale lerp iceberg (M1 izq), iceDisc lerp (M2 izq), waterY lerp (M3 izq), emissive lerp + carZ lerp (M2 der), treeScale lerp (M3 der), alpha lerp secuencial de líneas (M5) |
| **Procedural** — generativa sin fotogramas | Shader de ondas del agua (función seno + tiempo), partículas de nieve del entorno, partículas CO₂ en espiral (M3 der), billboard particles de la turbina (M1 der) |

---

## Animaciones Básicas

### Globo terráqueo — M5
- Rotación Y constante en loop desde el primer fotograma
- Velocidad fija: ~15°/s
- Se actualiza cada frame: `globe.rotY += 15.0f * deltaTime`

### Aspas de turbina — M1 der
- En reposo: `rotSpeed = 0`
- Al presionar E: `rotSpeed` sube de 0 → 120°/s mediante LERP durante ~5 s
- Cada frame: `blades.rotY += rotSpeed * deltaTime`

---

## Animaciones Keyframe (LERP)

Todas usan la misma estructura:

```cpp
t += deltaTime / duracion;          // avanza t de 0 a 1
t = clamp(t, 0.0f, 1.0f);
valor = mix(valorInicial, valorFinal, t);
```

| Módulo | Variable | Inicial | Final | Duración |
|--------|----------|---------|-------|----------|
| M1 izq — iceberg | `vec3 scale` | `(1.0, 1.0, 1.0)` | `(0.3, 0.1, 0.3)` | ~10 s |
| M2 izq — disco de hielo | `float iceScale` (XZ) | `1.0` | `0.2` | ~8 s |
| M3 izq — nivel del agua | `float waterY` | `-1.0` | `+2.5` | ~12 s |
| M1 der — turbina (rotSpeed) | `float rotSpeed` | `0.0` | `120.0` °/s | ~5 s |
| M2 der — faros del auto | `float emissive` | `0.0` | `1.0` | ~2 s |
| M2 der — posición auto | `float carZ` | zA | zB → zA | ~6 s (ida y vuelta) |
| M3 der — árbol | `float treeScale` | `0.05` | `1.0` | ~8 s |
| M5 — líneas de conexión | `float lineAlpha[N]` | `0.0` | `1.0` | ~0.5 s por línea, en secuencia |

**Reset**: al salir del trigger, todos los valores vuelven al estado inicial **instantáneamente** (`t = 0`).

**Interpolación de rotaciones**: LERP de ángulo euler para todas las rotaciones — sin SLERP de quaterniones.

---

## Animaciones Procedurales

### Ondas del agua (shader)

Aplicado al plano de agua en M1 izq y M3 izq. Basado en los shaders del profesor (`13_wavesAnimation-fresnel.vs/.fs`):

```glsl
// Vertex shader
float wave = amplitude * sin(frequency * position.x + time)
           + amplitude * sin(frequency * position.z + time * 0.7);
gl_Position.y += wave;
```

| Parámetro | Valor |
|-----------|-------|
| Amplitud | 0.08 m |
| Frecuencia | 2.0 |
| Uniform `time` | Actualizado cada frame con `glfwGetTime()` |

---

### Partículas de nieve (entorno)

80–120 partículas activas en toda la escena, siempre corriendo.

```cpp
// Update por partícula cada frame
p.y -= p.speed * deltaTime;                       // caída
p.x += sin(p.phase + time * 0.5f) * 0.01f;       // drift suave
if (p.y < 0.0f) {                                 // reciclar
    p.y = 8.0f;
    p.x = randomRange(-20, 20);
    p.z = randomRange(-20, 20);
}
```

- Billboards de 0.05 m, color blanco, alpha ~0.6
- Sin textura — punto blanco sólido en el fragment shader

---

### Partículas de CO₂ — M3 der (La Tierra respira)

30–40 partículas activas sólo mientras el módulo está en `ANIMANDO` o `COMPLETADO`.

Movimiento en espiral convergente hacia el árbol:

```cpp
float angle = i * GOLDEN_ANGLE + time;
float radius = lerp(MAX_RADIUS, 0.0f, t_modulo);  // se cierra con el módulo
p.x = arbol.x + radius * cos(angle);
p.z = arbol.z + radius * sin(angle);
p.y = lerp(4.0f, 0.5f, t_particula);              // cae hacia el árbol
```

Cuando una partícula llega al centro (radio < 0.1 m), reaparece en el borde exterior.

---

### Billboard particles — M1 der (La energía del planeta)

20–30 partículas activas sólo cuando `rotSpeed > 0`.

```cpp
p.position += p.velocity * deltaTime;   // velocidad radial desde turbina
p.alpha -= 0.3f * deltaTime;            // fade out
if (p.alpha <= 0.0f) resetParticula();  // reaparece cerca de la turbina
```

- Color blanco, alpha decreciente desde 0.8 → 0
- Sin textura — quad blanco con alpha blending

---

## Animación del Oso Polar

- Clip "idle" del GLTF corre en loop permanente desde el primer fotograma
- Al presionar E, el oso continúa su idle sin interrupción — E solo anima el disco de hielo (XZ scale)
- Sin animación adicional al presionar E

---

## Deformación del Suelo

El heightmap del suelo (±0.3 m, Doc 02) es **estático** — calculado una vez al generar el mesh, sin actualización en tiempo real. Las ondas dinámicas quedan cubiertas por el shader de agua de los módulos.

---

## Notas de Implementación

- Cada módulo tiene su propio `float t` que avanza en `update()` cuando el estado es `ANIMANDO`
- El `t` de cada módulo se guarda en la estructura del módulo: `struct Modulo { float t; EstadoModulo estado; ... }`
- Las partículas de nieve se actualizan en CPU y se suben a GPU con `glBufferSubData` cada frame (VBO dinámico)
- El uniform `time` del shader de agua se pasa con `glUniform1f(loc, (float)glfwGetTime())`
- La secuencia de líneas en M5 usa un array de `t` independientes: `lineT[i]` empieza a avanzar cuando `lineT[i-1] >= 0.5`
