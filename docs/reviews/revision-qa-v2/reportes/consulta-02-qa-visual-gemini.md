## Evaluación General
**No, no detectaría los defectos graves de manera autónoma**. Aunque el plan utiliza visión multimodal y tiene descripciones detalladas, asume que un agente (IA) puede interpretar conceptos abstractos de renderizado ("escala humana", "bonito", "se ve agua debajo") con la misma intuición espacial y estética que un humano, basándose únicamente en capturas horizontales (pitch=0). Al tener la cámara a Y=1.7m mirando al frente, el área de pantalla ocupada por el suelo es mínima en perspectiva, lo que dificulta que la IA distinga entre un shader plano gris, el agua renderizada debajo, o una ausencia total de geometría del suelo si el agua coincide con los tonos del skybox. Además, no evalúa las normales invertidas ni el render order de transparencias (el agua se dibuja con depth mask `false`, lo que podría ocultar errores).

## Expectativas Faltantes (críticas)
| # | Posición/Situación | Qué debería describirse | Por qué importa |
|---|-------------------|------------------------|----------------|
| 1 | Mirando directo al suelo (Y=1.7, pitch=-90) | El frame entero debe estar cubierto por la textura de concreto. Cero agua azul visible. | Revela instantáneamente el culling del `floorMesh` por su Winding Order incorrecto (actualmente está en CW, por lo que desaparece visto desde arriba). |
| 2 | Mirando directo al techo (Y=1.7, pitch=90) | Cubierta uniformemente gris cerrando el nivel superior. No debe verse el cielo nublado. | Verifica que el plano del techo tiene las normales/winding order correctos y que la luz hemisférica se interpola bien. |
| 3 | Letreros interactivos desde un lateral | Se debe ver texto legible o paneles rectangulares nítidos flotando. | En `Museum.h`, los letreros se renderizan escalando cubos asimétricamente. Vistos de lado, pueden colapsarse visualmente en "líneas diagonales" (z-fighting o fallas de perspectiva). |
| 4 | Acerarse a una pared a distancia = 0.5m | La textura de `wall.png` debe tener detalles y variaciones de luz claras. | Ayuda a la IA a distinguir entre un fallback de `baseColor` gris liso y una textura cargada correctamente, eliminando falsos positivos. |

## Mejoras al Plan QA
| # | Mejora | Impacto | Implementación |
|---|--------|---------|----------------|
| 1 | **Pruebas Multi-Ángulo (Pitch/Yaw)** | Alto | Añadir paso para capturar 3 fotos por posición de cámara: Frente (pitch=0), Abajo (pitch=-90), Arriba (pitch=+90). |
| 2 | **Screenshot de Referencia ("Goldens")** | Crítico | En lugar de descripciones textuales subjetivas, usar imágenes de referencia previamente validadas (Golden Master) y pedirle a la IA que encuentre diferencias estructurales. |
| 3 | **Bounding Box de Referencia** | Medio | Para la escala ("humana"), renderizar en modo debug un wireframe de un cubo de 1x1x1m o 2x2x2m al lado del modelo 3D para que el agente tenga una regla de medición visual. |
| 4 | **Render Pass de Geometría Plana (Albedo)** | Alto | Realizar capturas desactivando temporalmente la niebla, las partículas de nieve y la luz ambiental para aislar problemas de carga de texturas y materiales. |

## Técnicas de Diagnóstico Adicionales
1. **Invertir Culling Temporalmente:** En lugar de solo `glDisable(GL_CULL_FACE)`, forzar `glFrontFace(GL_CW)` y `glFrontFace(GL_CCW)` en pasadas alternas para que el agente vea físicamente cómo aparece/desaparece la geometría e identifique problemas de Winding Order (como el de `floorMesh` cuyos vértices definen un vector normal hacia `-Y` pero se observan desde `+Y`).
2. **Wireframe Mode (`glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)`):** Fundamental para debuggear escalas incorrectas, letreros que parecen "líneas diagonales" y confirmar si las paredes son un solo quad estirado o están subdivididas.
3. **Normales como Color (Shader Debug):** Crear un `debug.frag` que haga `FragColor = vec4(Normal * 0.5 + 0.5, 1.0)`. Si la IA ve un plano negro o tonos incongruentes, sabe que las normales están mal calculadas (crucial para iluminación Blinn-Phong).
4. **Depth Buffer Visualization:** Renderizar el buffer de profundidad linealizado a escala de grises. Esto ayuda al agente a detectar Z-fighting, y problemas entre el agua, el suelo y los letreros 3D (Z-order).

## Preguntas de "Sensibilidad Estética"
Para que un agente sin gustos humanos pueda evaluar si algo es "bonito" o parece "un museo en lugar de un espacio abstracto", se debe parametrizar la estética en **métricas técnicas cuantificables** en su instrucción:
- **Contraste de Frecuencias:** Pedirle a la IA que detecte ruido y patrones. Un muro "bloque gris plano" tendrá una varianza de color de 0 píxeles vecinos. Un muro texturizado mostrará varianza. 
- **Volumen de Iluminación:** Solicitar a la IA que detecte gradientes. Una pared correctamente iluminada debe mostrar un gradiente desde el techo (hemi = azul oscuro) al suelo (hemi = reflejo del piso). Si la pared tiene exactamente el mismo valor RGB en la parte superior e inferior, la iluminación es "plana" (no es "bonita").
- **Proporción Pantalla-Objeto:** Definir la escala en porcentaje de píxeles en la pantalla. "El modelo Kenney 3D de árbol en (pitch=0, distancia=3m) debe ocupar al menos el 40% del alto vertical de la pantalla. Si ocupa el 2%, es diminuto".
- **Identificabilidad:** Usar Zero-Shot Classification. Preguntar ciegamente a la IA: "¿Qué tipo de edificio u objeto es el de la imagen?". Si la IA responde "un pasillo abstracto de bloques", falla la prueba; si responde "las ruinas o pasillo de una instalación/museo de concreto", pasa.

## Top 5 Recomendaciones
1. **Corregir el Winding Order del suelo:** En `Museum.h` (Línea 245), `std::vector<unsigned int> idx = {0,1,2, 0,2,3};` define un quad CW visto desde arriba, provocando que se lo lleve el back-face culling de OpenGL (por defecto CCW). Se debe invertir a `{0,2,1, 0,3,2}` para que su normal apunte a `+Y` y sea visible.
2. **Añadir Goldens de Pitch -90 y +90:** Modificar la rutina `--test-module` en `main.cpp` para obligar al agente a rotar la cámara y revisar piso/techo explícitamente en el pipeline.
3. **Reemplazar validación subjetiva con métricas:** Cambiar frases como "escala razonable" por "debe ocupar entre un 30% y un 50% de la altura de la imagen en la captura f300".
4. **Implementar render mode Debug_Normals:** Exponer un flag `--debug-normals` o `--wireframe` que el agente active automáticamente tras registrar una falla de textura/iluminación, reduciendo su dependencia a especular el código.
5. **Revisar las matrices en drawSigns():** Los escalados extremos en `Museum.h` `glm::vec3(0.06f, 0.65f, halfW * 2)` crean paneles bidimensionales infinitamente finos. Vista desde el centro, la cámara los pilla casi de canto y sin texturas, pareciendo meras "líneas cruzadas". Utilizar quads texturizados orientados tipo billboarding evitará anomalías de rasterizado en ángulos oblicuos.
