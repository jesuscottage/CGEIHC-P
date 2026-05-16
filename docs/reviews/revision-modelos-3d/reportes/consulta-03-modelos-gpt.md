## Fuentes Verificadas

| Fuente                                                                                                   | Sin login? | Con curl? | Modelos relevantes                              | URL                                                       |
| -------------------------------------------------------------------------------------------------------- | ---------- | --------- | ----------------------------------------------- | --------------------------------------------------------- |
| [Poly Haven](https://polyhaven.com?utm_source=chatgpt.com)                                               | Sí         | Sí        | iceberg, roca, nieve, algunas estructuras       | CC0 total, GLTF/GLB descargable directo ([Poly Haven][1]) |
| [Kenney.nl](https://kenney.nl/assets?utm_source=chatgpt.com)                                             | Sí         | Sí        | panel solar, turbina, props lowpoly, naturaleza | Excelente para packs compactos                            |
| [Quaternius](https://quaternius.com?utm_source=chatgpt.com)                                              | Sí         | Sí        | animales, naturaleza, props                     | CC0, glTF incluido en muchos packs ([Quaternius][2])      |
| [Poly Pizza](https://poly.pizza?utm_source=chatgpt.com)                                                  | Sí         | Sí        | animales individuales GLTF                      | Mucho contenido CC0 de Quaternius ([Poly Pizza][3])       |
| [Khronos glTF Sample Assets](https://github.com/KhronosGroup/glTF-Sample-Assets?utm_source=chatgpt.com)  | Sí         | Sí        | esfera/globo, ejemplos PBR                      | Bueno para debugging OpenGL/PBR                           |
| [OpenGameArt](https://opengameart.org?utm_source=chatgpt.com)                                            | Sí         | Sí        | animales lowpoly CC0                            | Calidad inconsistente pero útil ([OpenGameArt.org][4])    |
| [GitHub Search](https://github.com/search?q=polar+bear+glb+cc0&type=repositories&utm_source=chatgpt.com) | Sí         | Sí        | cualquier cosa rara                             | Último recurso                                            |

---

## URLs Directas de Descarga (si existen)

| Modelo                               | URL directa comprobable                                                                                                               |
| ------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------- |
| Oso polar (sustituto: oso/wolf/yeti) | [Ultimate Animated Animal Pack](https://quaternius.com/packs/ultimateanimatedanimals.html?utm_source=chatgpt.com) ([Quaternius][5])   |
| Pájaro / gaviota                     | [Animated Animals Low Poly](https://opengameart.org/content/animated-animales-low-poly?utm_source=chatgpt.com) ([OpenGameArt.org][4]) |
| Fauna lowpoly GLTF                   | [Quaternius on Poly Pizza](https://poly.pizza/u/Quaternius?utm_source=chatgpt.com) ([Poly Pizza][3])                                  |
| Farm animal bundle GLTF              | [Farm Animal Pack GLTF Bundle](https://poly.pizza/bundle/Farm-Animal-Pack-1kUvRTPLzT?utm_source=chatgpt.com) ([Poly Pizza][6])        |
| Icebergs / rocas                     | [Poly Haven Models](https://polyhaven.com/models?utm_source=chatgpt.com) ([Poly Haven][7])                                            |
| Globo terráqueo                      | [glTF Sample Assets](https://github.com/KhronosGroup/glTF-Sample-Assets?utm_source=chatgpt.com)                                       |
| Turbina eólica                       | [Kenney Assets](https://kenney.nl/assets?utm_source=chatgpt.com)                                                                      |
| Panel solar                          | [Kenney Assets](https://kenney.nl/assets?utm_source=chatgpt.com)                                                                      |
| Roca nevada                          | [Poly Haven](https://polyhaven.com/models?utm_source=chatgpt.com)                                                                     |

---

## Respuestas Directas

### 1. ¿Quaternius funciona sin login?

Sí. Totalmente usable con `curl` y sin autenticación.
Los packs están en páginas HTML simples con botones directos de descarga. ([Quaternius][2])

Los mejores packs para ustedes:

* [Ultimate Animated Animal Pack](https://quaternius.com/packs/ultimateanimatedanimals.html?utm_source=chatgpt.com)
* [LowPoly Animated Animals](https://quaternius.itch.io/lowpoly-animated-animals?utm_source=chatgpt.com)
* [Poly Pizza Quaternius Collection](https://poly.pizza/u/Quaternius?utm_source=chatgpt.com)

Poly Pizza es especialmente útil porque:

* descarga GLTF directo
* no requiere login
* modelos individuales
* evita bajar ZIPs gigantes

Para automation pipeline es probablemente mejor que itch.io.

---

### 2. ¿Hay fuentes mejores para animales árticos?

Para CC0 + sin login + GLB fáciles:

| Fuente      | Calidad   | Conveniencia                           |
| ----------- | --------- | -------------------------------------- |
| Poly Pizza  | Muy buena | Excelente                              |
| Quaternius  | Buena     | Excelente                              |
| OpenGameArt | Variable  | Buena                                  |
| Sketchfab   | Excelente | Mala (login/licencia/API problemática) |
| CGTrader    | Variable  | Mala                                   |
| BlendSwap   | Buena     | Mala                                   |

Para animales árticos reales (polar bear / seal / arctic fox), el problema NO es técnico sino de licencia.

La mayoría de modelos buenos:

* son Royalty Free
* requieren cuenta
* no son CC0

Por eso su mejor estrategia es:

* stylized lowpoly
* no realista
* estética consistente

Eso además combina mejor con OpenGL académico.

---

## Dificultad por Modelo

| Modelo           | Dificultad | Mejor fuente             | Alternativa si no se encuentra |
| ---------------- | ---------- | ------------------------ | ------------------------------ |
| Globo terráqueo  | Fácil      | Khronos Sample Assets    | esfera UV + textura NASA       |
| Oso polar        | Difícil    | Quaternius / Poly Pizza  | oso blanco genérico / yeti     |
| Iceberg          | Muy fácil  | Poly Haven               | roca escalada azul             |
| Turbina eólica   | Fácil      | Kenney                   | modelarla manualmente          |
| Foca             | Difícil    | Poly Pizza               | pez grande estilizado          |
| Zorro ártico     | Medio      | Quaternius               | wolf blanco                    |
| Pájaro / gaviota | Fácil      | OpenGameArt / Quaternius | pigeon/birb                    |
| Panel solar      | Muy fácil  | Kenney                   | plano con textura              |
| Roca nevada      | Muy fácil  | Poly Haven               | roca + material nieve          |

---

## Packs "Todo en Uno"

Estos sí valen mucho la pena:

### Mejor opción general

[Stylized Nature MegaKit (Quaternius)](https://quaternius.itch.io/stylized-nature-megakit?utm_source=chatgpt.com)

Cubre:

* rocas
* nieve
* árboles
* props naturales
* decoración

---

### Mejor opción fauna

[Ultimate Animated Animal Pack](https://quaternius.com/packs/ultimateanimatedanimals.html?utm_source=chatgpt.com)

Aunque no tenga animales árticos exactos, sí da:

* cuadrúpedos reutilizables
* esqueletos consistentes
* estética uniforme

Muy útil para reemplazar “cubos vivos”.

---

### Mejor opción modular para museo

[Kenney Nature Kit](https://kenney.nl/assets/nature-kit?utm_source=chatgpt.com)

Ideal para:

* props ligeros
* GLB compactos
* coherencia visual
* bajo polycount

---

## ¿El orden de prioridad es correcto?

Yo cambiaría el orden a:

1. Globo terráqueo
2. Iceberg
3. Oso polar
4. Turbina
5. Roca nevada
6. Panel solar
7. Pájaro
8. Zorro
9. Foca

Razón:

* El globo arregla inmediatamente el módulo roto visualmente.
* Icebergs dan muchísimo impacto visual por poco esfuerzo.
* Turbina/panel son fáciles y comunican “energía renovable”.
* La fauna secundaria aporta menos valor académico.
* La foca es probablemente el asset más difícil de conseguir bien en CC0.

---

## Recomendaciones

1. Usen GLTF/GLB exclusivamente. Eviten OBJ+MTL.
2. Prioricen assets stylized lowpoly para mantener coherencia visual.
3. Poly Pizza probablemente será su mejor pipeline automation-friendly.
4. Eviten modelos PBR complejos si su renderer OpenGL no soporta bien metallic/roughness.
5. Para el globo: usen textura baked/simple diffuse, no materiales físicos avanzados.
6. Si un modelo no existe CC0:

   * cambien el animal
   * usen sustituto visual
   * mantengan la intención educativa
7. Mantengan todos los modelos:

   * <50k tris
   * <4K textures
   * GLB embedded
   * escala normalizada

La combinación más sólida para su proyecto sería:

* Quaternius → animales
* Poly Haven → hielo/rocas
* Kenney → props tecnológicos
* Khronos → debugging/render validation

Eso les da pipeline consistente, legalmente limpio y automatizable con `curl`.

[1]: https://polyhaven.com/?utm_source=chatgpt.com "Poly Haven"
[2]: https://quaternius.com/?utm_source=chatgpt.com "Quaternius • Free Game Assets"
[3]: https://poly.pizza/u/Quaternius?utm_source=chatgpt.com "Quaternius"
[4]: https://opengameart.org/content/animated-animales-low-poly?utm_source=chatgpt.com "Animated Animales Low Poly"
[5]: https://quaternius.com/packs/ultimateanimatedanimals.html?utm_source=chatgpt.com "Ultimate Animated Animal Pack"
[6]: https://poly.pizza/bundle/Farm-Animal-Pack-1kUvRTPLzT?utm_source=chatgpt.com "Farm Animal Pack by Quaternius | Free download"
[7]: https://polyhaven.com/models/?utm_source=chatgpt.com "Models"
