# Doc 05 — Materiales y Texturas

> Estado: COMPLETO
> Última actualización: 2026-05-15

---

## Material del Suelo (Hielo/Nieve)

Material PBR simple con texturas CC0 de ambientcg.com:

| Canal | Valor |
|-------|-------|
| Albedo | Blanco-azulado pálido (#d8eaf5) con variación sutil — textura tileable |
| Normal map | Superficie ligeramente irregular, grietas suaves |
| Roughness | ~0.85 — hielo mate, no brillante en vista directa |
| Brillo especular | Manejado por Fresnel en el fragment shader (no por specular map) |

Aplica a todo el plano de suelo. Los corredores y el vestíbulo usan material idéntico — la diferenciación visual la dan las flechas y los letreros, no el piso.

---

## Material de las Plataformas de Módulos

| Canal | Valor |
|-------|-------|
| Albedo superficie | Blanco con tinte cyan (#e0f7fa) |
| Normal map | Más suave que el suelo — superficie casi plana |
| Roughness | ~0.85 (igual que el suelo) |
| Borde emisivo | Color blanco-azulado, intensidad baja, **estático** — siempre encendido |

El borde emisivo no cambia de color al entrar el jugador — es un indicador visual permanente de zona interactiva.

---

## Entorno Atmosférico

### Niebla

Niebla exponencial aplicada en el fragment shader:

```glsl
float fogFactor = exp(-fogDensity * distance);
fragColor = mix(fogColor, sceneColor, fogFactor);
```

| Parámetro | Valor |
|-----------|-------|
| Color | Blanco-gris muy pálido (#e8eef2) |
| Densidad | Baja (~0.008) — difumina el horizonte sin afectar la visibilidad cercana |
| Efecto | Une el skybox con el suelo en el horizonte, da profundidad a la escena |

### Partículas de Nieve

Implementación mínima — pocas partículas, no obstruyen la vista:

| Parámetro | Valor |
|-----------|-------|
| Geometría | Quads billboards de ~0.05 m, siempre orientados hacia la cámara |
| Material | `unlit` — punto blanco semitransparente (alpha ~0.6), sin textura |
| Cantidad | ~80–120 partículas activas en toda la escena |
| Movimiento | Caída suave en Y (velocidad aleatoria por partícula) + drift leve en X con seno |
| Distribución | Volumen de ~40 × 40 m centrado en el jugador (se reciclan al pasar el suelo) |
| Reset | Cuando una partícula llega a y = 0, reaparece en y = 8 m en posición aleatoria dentro del volumen |

---

## Shader de Agua

Base: shaders del profesor `13_wavesAnimation-fresnel.vs/.fs` adaptados.

| Canal | Valor |
|-------|-------|
| Albedo | Azul-gris oscuro (#1a3a5c) |
| Normal map | Textura de agua CC0 de ambientcg.com (opcional, mejora el detalle) |
| Animación vértices | Desplazamiento Y con función seno: `y += amplitude * sin(freq * x + time)` |
| Fresnel | Borde agua más opaco en ángulo oblicuo, más transparente en vista directa |

El mismo shader se reutiliza en:
- **M1 izq**: plano de agua base — ondas activas al presionar E (iceberg se derrite)
- **M3 izq**: plano de agua que sube — ondas siempre activas mientras el agua sube

---

## Letreros Informativos

Material `unlit` — el letrero siempre tiene el mismo brillo, independientemente de la iluminación de la escena.

| Elemento | Valor |
|----------|-------|
| Fondo | Azul oscuro semi-opaco (#0d2137, alpha ~0.85) — se ve ligeramente el modelo detrás |
| Texto | Blanco puro (#ffffff), rasterizado con stb_truetype a textura OpenGL |
| Borde | Línea cyan (#00bcd4) de 2 px — parte de la textura generada en CPU |
| Renderizado | Alpha blending activado (`GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`) |
| Iluminación | Ninguna — shader unlit |

---

## Fauna y Modelos Decorativos

Los modelos de fauna usan los materiales que vienen con su asset GLTF de Sketchfab (albedo + normal map incluidos). No se modifican — se cargan y usan directamente.

---

## Resumen de Técnicas de Iluminación

| Técnica | Aplicación | Requisito rúbrica |
|---------|-----------|-------------------|
| Blinn-Phong | Todos los modelos 3D de la escena | ✅ Técnica 1 |
| Fresnel | Agua (shader del profesor) + bordes del hielo | ✅ Técnica 2 |
| Emisivo | Bordes de plataformas, faros del auto (M2 der) | Complementario |

---

## Fuentes de Texturas CC0

| Textura | Fuente |
|---------|--------|
| Hielo/nieve (albedo + normal) | ambientcg.com |
| Agua (normal map) | ambientcg.com o polyhaven.com |
| Skybox HDR ártico con aurora | polyhaven.com |
| Assets de fauna/módulos | Incluidos en los GLTF de Sketchfab |

---

## Notas de Implementación

- Todas las texturas del entorno se cargan con stb_image al inicio y se almacenan como `GLuint`
- Las texturas de los letreros se generan en CPU con stb_truetype y se suben con `glTexImage2D` — una vez por módulo al arranque
- El shader de agua es compartido entre M1 izq y M3 izq — se instancia con los mismos uniforms, cambiando sólo `waterY` por módulo
- Las partículas de nieve usan un único VAO con posiciones actualizadas en CPU cada frame (no compute shader)
