# Doc 03 — Catálogo de Modelos

> Estado: COMPLETO
> Última actualización: 2026-05-15

---

## Módulos Interactivos

Cada módulo tiene un modelo principal + un panel informativo 3D permanente.
La interacción se activa con la tecla **E** al entrar en la plataforma circular.

---

### M1 izq — El hielo que desaparece
*(Plataforma izquierda, 15 m desde el vestíbulo)*

| Campo | Valor |
|-------|-------|
| Modelo principal | Iceberg — Sketchfab CC0, formato GLTF/OBJ |
| Posición | Centro de la plataforma circular |
| Escala inicial | `vec3(1.0, 1.0, 1.0)` |
| Interacción (E) | Scale lerp → `vec3(0.3, 0.1, 0.3)` en ~10 s |
| Efecto secundario | Shader de ondas (wave animation) en el plano de agua base |
| Reset | Al salir de la plataforma, escala vuelve a `vec3(1.0)` instantáneamente |

**Variable animada**: `float t` ∈ [0, 1], actualizada en el `update()` del módulo.
```
iceberg.scale = mix(vec3(1.0), vec3(0.3, 0.1, 0.3), t);
```

---

### M2 izq — Los últimos habitantes
*(Plataforma izquierda, 30 m desde el vestíbulo)*

| Campo | Valor |
|-------|-------|
| Modelo principal | Oso polar — Sketchfab CC0, GLTF con clip de animación |
| Animación idle | Clip "idle" del GLTF, reproducción en loop desde el primer fotograma |
| Plataforma auxiliar | Disco de hielo plano (mesh propio, r = 3 m) centrado bajo el oso |
| Interacción (E) | XZ scale lerp del disco: `vec2(1.0)` → `vec2(0.2)` en ~8 s |
| Reset | Al salir, disco vuelve a `vec2(1.0)` instantáneamente |

**Variable animada**: `float iceScale` ∈ [1.0, 0.2] aplicado sólo a X y Z del disco.

---

### M3 izq — El mar que avanza
*(Plataforma izquierda, 45 m desde el vestíbulo)*

| Campo | Valor |
|-------|-------|
| Modelo principal | 3–5 edificios low-poly — Sketchfab CC0 o Poly Haven |
| Posición edificios | Agrupados en el centro de la plataforma |
| Plano de agua | Mesh plano con shader de olas (wave animation), y inicial = −1.0 |
| Interacción (E) | Y position lerp del plano: −1.0 → +2.5 en ~12 s |
| Reset | Al salir, y vuelve a −1.0 instantáneamente |

**Variable animada**: `float waterY` ∈ [−1.0, +2.5].
```
waterPlane.position.y = waterY;
```

---

### M1 der — La energía del planeta
*(Plataforma derecha, 15 m desde el vestíbulo)*

| Campo | Valor |
|-------|-------|
| Modelo principal | Turbina eólica — Sketchfab CC0, mesh separado para las aspas |
| Estado inicial | Aspas sin rotación (velocidad = 0) |
| Interacción (E) | Rotation speed lerp: 0 → MAX_RPM (≈ 120°/s) en ~5 s |
| Efecto secundario | Billboard particles (quad sprites blancos) saliendo de la turbina — representan energía limpia |
| Reset | Velocidad baja a 0 al salir |

**Variable animada**: `float rotSpeed` ∈ [0.0, 120.0] en grados/segundo.
```
blades.rotation.y += rotSpeed * deltaTime;
```

---

### M2 der — Moverse sin quemar
*(Plataforma derecha, 30 m desde el vestíbulo)*

| Campo | Valor |
|-------|-------|
| Modelo principal | Auto eléctrico low-poly — Sketchfab CC0 |
| Estado inicial | Auto detenido, faros apagados (emissive = 0) |
| Interacción (E) | Emissive lerp: 0.0 → 1.0 en ~2 s (faros) + position Z lerp: A → B → A en ~6 s |
| Reset | Emissive y posición vuelven al inicial al salir |

**Variables animadas**:
- `float emissive` ∈ [0.0, 1.0] aplicado al material de los faros
- `float carZ` ∈ [zA, zB] para el movimiento de ida y vuelta

---

### M3 der — La Tierra respira
*(Plataforma derecha, 45 m desde el vestíbulo)*

| Campo | Valor |
|-------|-------|
| Modelo principal | Árbol (pino o árbol genérico frondoso) — Sketchfab CC0 |
| Escala inicial | `vec3(0.05)` — casi invisible |
| Interacción (E) | Scale lerp: `vec3(0.05)` → `vec3(1.0)` en ~8 s |
| Efecto secundario | Partículas de CO₂ (quads con puntos grises) cayendo en espiral hacia el árbol |
| Reset | Escala y partículas vuelven al inicial al salir |

**Variable animada**: `float treeScale` ∈ [0.05, 1.0].

---

### M5 — Juntos o no funciona
*(Zona central compartida al final de ambos corredores)*

| Campo | Valor |
|-------|-------|
| Modelo principal | Globo terráqueo — Sketchfab CC0, textura de mapa político/físico |
| Animación pasiva | Rotación Y constante en loop (velocidad fija, siempre activa) |
| Elementos auxiliares | N cilindros delgados (líneas de conexión entre países), alpha inicial = 0 |
| Interacción (E) | Alpha lerp secuencial de cada cilindro: 0.0 → 1.0, uno por uno con ~0.5 s entre ellos |
| Reset | Alpha de todos los cilindros a 0 al salir |

**Variable animada**: `float lineAlpha[N]` — cada elemento con su propio `t` de avance.

---

## Fauna Decorativa

Puramente ambiental — sin trigger, sin interacción, sin animación (excepto el oso polar de M2 izq).

| Animal | Modelo | Instancias | Distribución |
|--------|--------|------------|--------------|
| Foca anillada | Sketchfab CC0, mesh simple | 4–6 | Dispersas sobre el hielo fuera del área jugable |
| Zorro ártico | Sketchfab CC0 | 2–3 | Bordes del escenario, más allá de las paredes invisibles |
| Gaviota ártica | Billboard sprite (quad de 2 triángulos, PNG con alpha) | 6–8 | Posición fija elevada (y ≈ 4–6 m), estáticas |

Todos los modelos se cargan una vez y se instancian en posiciones fijas.

---

## Letreros Informativos

Cada módulo tiene un **letrero flotante fijo** con el texto completo, siempre visible en escena.
El usuario puede leer el letrero y presionar E para ver la animación al mismo tiempo — sin restricción de visión.

| Parámetro | Valor |
|-----------|-------|
| Geometría | Plano rectangular 2 × 1.2 m |
| Orientación | **Fija** — orientado hacia el corredor de entrada del módulo (no billboard) |
| Posición | Detrás del modelo principal respecto al corredor, altura y = 1.4 m (centro del plano) |
| Textura | Generada en CPU con stb_truetype → cargada como textura OpenGL al inicio |
| Contenido | Texto completo del módulo: título + dato impactante + descripción + fuente |
| Interacción | Ninguna — siempre visible, sin trigger |

La tecla E activa **únicamente la animación** del módulo. No hay overlay de Dear ImGui para texto.

---

## Señalización en el Suelo

### Flechas del vestíbulo (2 flechas)

| Parámetro | Valor |
|-----------|-------|
| Geometría | Plano 3D de 0.8 × 0.4 m, y = 0.01 (sobre el hielo) |
| Textura | PNG con flecha + texto, fondo transparente (alpha blending) |
| Texto flecha izquierda | `"← Las consecuencias"` |
| Texto flecha derecha | `"Las soluciones →"` |
| Fuente | Asset propio generado con stb_truetype o GIMP/Inkscape |

### Flechas de continuación (en cada corredor)

Una flecha adicional a mitad de cada corredor apuntando hacia M5, sin texto.

---

## Skybox

| Parámetro | Valor |
|-----------|-------|
| Tipo | Cubemap estático (6 caras PNG) |
| Contenido | Cielo ártico despejado con aurora boreal |
| Fuente primera opción | Poly Haven (CC0, HDR) → convertir a 6 caras con `cmgen` de Filament |
| Alternativa | Shader procesal: degradado azul pálido (#cce8f4) → azul oscuro (#0a1a2e) con banda verde-aurora (#00ff88, baja opacidad) en el horizonte |
| Implementación | Shader de skybox estándar — cubemap sampler, depth trick (`gl_Position = pos.xyww`) |

La aurora en el HDR de Poly Haven es estática (fotografía) — no requiere animación de shader.

---

## Indicador de Zona Interactiva

Cuando el jugador entra en una plataforma circular:

| Elemento | Descripción |
|----------|-------------|
| En escena | Borde emisivo de la plataforma — siempre activo, define visualmente la zona |
| En pantalla | Texto `[E] Explorar` centrado en la parte inferior (Dear ImGui) |
| Visibilidad | Solo dentro del trigger de proximidad de la plataforma |
| Al presionar E | Texto `[E] Explorar` desaparece, la animación del módulo comienza |
| Al salir | Todo vuelve a estado inicial |

---

## Notas de Implementación

- Todos los modelos se buscan en formato **GLTF** primero (mejor soporte Assimp); OBJ como alternativa.
- El oso polar requiere un modelo con **clip de animación idle** en el GLTF — verificar antes de descargar.
- Las texturas de los paneles informativos se generan **una sola vez al inicio** y se almacenan como `GLuint texture`.
- Los cilindros de M5 son meshes procedurales simples generados en CPU al inicio.
- Las partículas de M1-der y M3-der son billboards simples (sin física) — posición actualizada con función seno/espiral.
- El skybox HDR se convierte offline: `cmgen --format=png --size=512 input.hdr` → los 6 PNG se incluyen en `assets/skybox/`.
