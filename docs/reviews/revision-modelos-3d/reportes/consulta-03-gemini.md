## Evaluación de Fuentes
| Fuente | ¿Funciona con curl? | ¿Tiene lo que necesitamos? | URL directa |
|--------|---------------------|----------------------------|-------------|
| **Quaternius** | **Parcialmente**. Sus descargas suelen redirigir a Google Drive o Itch.io. `curl -L` puede fallar si requiere confirmación de virus en Drive para archivos grandes. | **Sí** (Animales, Naturaleza), pero vienen en ZIPs masivos con múltiples formatos, no un `.glb` individual. | `https://quaternius.com/packs/ultimateanimatedanimals.html` (requiere scraping para el link del ZIP) |
| **Poly Haven** | **Sí**. API directa y predecible. | **No para animales**. Excelente para el iceberg (rocas) y decoración, pero su catálogo de fauna es prácticamente nulo (enfocado en props inanimados). | `https://dl.polyhaven.org/file/ph-assets/Models/glb/rock_snow_02/rock_snow_02_1k.glb` |
| **GitHub** | **Sí**. Usando las raw URLs. | **Inconsistente**. No hay un gran repositorio centralizado de "animales árticos CC0", dependerá de repositorios sueltos o forks de OpenGameArt. | Depende del repositorio. |

## Fuentes Adicionales Recomendadas
- **Kenney - Animal Kit**: Esta es la joya oculta que falta en tu plan. Tiene un oso polar, un zorro (que puedes teñir de blanco en el shader) y pájaros. Todo low-poly, CC0 y en un solo ZIP predecible.
  - URL directa del ZIP: `https://kenney.nl/data/downloads/animal-kit.zip`
- **Kenney - Nature Kit** (Para el Iceberg/Rocas): Ya lo estás usando para el árbol, pero también tiene rocas y formaciones de nieve que te ahorrarán buscar en Poly Haven.
  - URL directa del ZIP: `https://kenney.nl/data/downloads/nature-kit.zip`
- **OpenGameArt (vía mirrors de GitHub)**: Para buscar cosas muy específicas, aunque es mejor priorizar Kenney por consistencia visual.

## Dificultad por Modelo
| Modelo | Dificultad | Mejor fuente | URL directa si existe | Alternativa |
|--------|------------|--------------|-----------------------|-------------|
| Globo terráqueo | Media | GitHub / Khronos | `https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/Earth/glTF-Binary/Earth.glb` (Hay que verificar si Assimp lo lee bien) | Esfera procedural con una textura de mapa terrestre aplicada en el shader. |
| Oso polar | Baja | Kenney Animal Kit | (Dentro del ZIP de Kenney) `bear.glb` | Perro blanco (`dog.glb`) del mismo pack. |
| Iceberg | Baja | Kenney / Poly Haven | (Poly Haven) `rock_boulder_dry_1k.glb` | Cubo procedural modificado con ruido Perlin en el shader de vértices. |
| Turbina eólica | Alta | GitHub / Quaternius | N/A (difícil hallar una suelta) | Dejar la procedural, pero reemplazar las aspas por un modelo de "tabla" o "ala". |
| Foca | Muy Alta | Quaternius / OGA | N/A | Pingüino (`penguin.glb` de Kenney Animal Kit) o pez genérico. |
| Zorro | Baja | Kenney Animal Kit | (Dentro del ZIP de Kenney) `fox.glb` | Perro huski o lobo. |
| Pájaro / Gaviota | Baja | Kenney Animal Kit | (Dentro del ZIP de Kenney) `owl.glb` o `parrot.glb` (teñido de blanco) | Avión de papel (fácil de hacer proceduralmente con 1 pirámide). |

## Recomendaciones
1. **Cambio de Estrategia a "Packs"**: En lugar de hacer 9 peticiones curl individuales (que pueden fallar), haz que el agente descargue 1 o 2 ZIPs masivos de Kenney/Quaternius al inicio, los descomprima en `/tmp/` y extraiga los `.glb` necesarios. Esto reduce drásticamente los puntos de fallo de red.
2. **Coherencia Visual (Low-Poly)**: Poly Haven ofrece modelos fotorrealistas (PBR) mientras que el resto de tu museo parece ser low-poly/flat-shaded (Kenney, Quaternius). Mezclar un iceberg hiperrealista de Poly Haven con un oso low-poly de Kenney se verá visualmente chocante. Quédate con Kenney para todo.
3. **El Orden de Prioridad es Correcto**: El globo terráqueo (M5) es crítico porque es el centro del museo y las normales/UVs de una esfera procedural son difíciles de hacer bien a mano. El oso polar (M2) es el ancla emocional del museo.
4. **Simplifica la Turbina**: Encontrar una turbina eólica animable en `.glb` (que tenga las aspas separadas para girarlas con tu variable `animT`) es casi imposible. Mantén la turbina procedural (es perfectamente aceptable en low-poly) y enfócate en los modelos orgánicos (animales).
5. **Cuidado con las Texturas Embebidas**: Asegúrate de que tu agente pase el flag `aiProcess_FlipUVs` en Assimp si los modelos de Kenney/Quaternius se renderizan con colores extraños, y valida que tu `ModuleScene.h` lea correctamente los materiales del `.glb` en lugar de sobrescribirlos con tu función `col()`.
