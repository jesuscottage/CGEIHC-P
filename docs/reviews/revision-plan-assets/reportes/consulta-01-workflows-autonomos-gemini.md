## Veredicto general
El plan **Parcialmente** es ejecutable de forma autónoma. Aunque la infraestructura en C++ está excelentemente preparada para la validación de un agente (con capturas vía `glReadPixels` y pruebas CLI deterministas), el pipeline fallará inevitablemente en la extracción de assets debido a muros de autenticación estricta (Sketchfab, Freesound) y comandos bash destructivos para los modelos.

## Pasos que FALLARÁN sin intervención humana
| # | Fase.Paso | Razón del fallo | Solución propuesta |
|---|-----------|-----------------|-------------------|
| 1 | Fase B (WF-02) | **Sketchfab bloquea descargas anónimas**: Requiere sesión incluso para modelos CC0. `WebFetch` o `curl` recibirán un error 401/403 en lugar del archivo ZIP. | Usar modelos pre-aprobados en repositorios de GitHub (ej. `glTF-Sample-Models`), la API de Poly Haven, o cargar un `.zip` inicial en el repo. |
| 2 | Fase B (WF-02) | **Corrupción de referencias GLTF**: El script bash (Paso 6) copia solo el archivo `.gltf`/`.glb` y lo renombra. Si es un `.gltf`, perderá la conexión con su `.bin` obligatorio y la carpeta de texturas, causando un crash en Assimp. | Copiar y mover la carpeta extraída **completa** hacia `assets/models/`, y usar la ruta del `.gltf` original en el código C++. |
| 3 | Fase C (WF-03) | **Freesound y Pixabay (Fallbacks)**: Freesound requiere login obligatorio. Pixabay, aunque no requiere login, está protegido fuertemente por Cloudflare, bloqueando peticiones `curl` automatizadas de agentes. | Descargar audios desde Wikimedia Commons, GitHub, o el Internet Archive (`archive.org`), que tienen APIs amigables con bots. |
| 4 | Fase D (D.2) | **Dependencia fantasma (Blender CLI)**: La mitigación de FPS propone "simplificar con Blender CLI", pero Blender no forma parte de los prerrequisitos documentados en `INFRAESTRUCTURA.md`. | Si un modelo excede la cuota poligonal y baja los FPS, el agente debe regresar de inmediato a la geometría procedural original. |

## Restricciones de sitios web (verificado)
| Sitio | ¿Descarga sin login? | ¿Rate limit? | Notas |
|-------|---------------------|--------------|-------|
| ambientcg.com | **Sí** | Sí (Flexible) | Provee enlaces directos a ZIPs a través de su API o web estática, amigable con bots. |
| Sketchfab | **No** | API Estricta | Todo intento de `curl` a links de descarga resultará en denegación de servicio. |
| freesound.org | **No** | Estricto | Requiere autenticación OAuth2 o login de sesión en navegador. |
| Poly Haven | **Sí** | Sí | Excelente para agentes. Descargas directas de texturas y HDRI sin bloqueo de bots. |
| Pixabay (Audio) | Sí (pero...) | Cloudflare | Las peticiones automatizadas (curl/WebFetch) frecuentemente son bloqueadas por desafíos CAPTCHA. |

## Orden de ejecución
El orden lógico es bueno, pero **no aprovecha totalmente el paralelismo** permitido (2 agentes simultáneos). 
- **Sugerencia de mejora**: Dado que las descargas en la Fase B (Modelos) y Fase C (Audio) toman mucho tiempo en peticiones web, el Agente 1 debería encargarse exclusivamente de descargar y descomprimir en bloque todo el contenido en background (Fase A, B y C), mientras el Agente 2 inicia la programación e integración en C++ con los primeros assets que vayan aterrizando en las carpetas. 

## Assets faltantes
- **Texturas del Globo Terráqueo (M5)**: El plan menciona descargar un modelo `globe.glb` (MD07), pero si este falla o se ve mal, el código procedural actual requiere texturas de océano y tierra para mapearse sobre la esfera procedural si se decide mantener el fallback.
- **Botón UI / Interact**: En la narrativa se menciona "UI blip" y "[E] Explorar", pero no hay assets visuales planificados para enriquecer este elemento de ImGui (como un ícono `.png` cargado en el atlas).

## Riesgos no contemplados
| Riesgo | Probabilidad | Impacto | Mitigación |
|--------|-------------|---------|------------|
| **Texturas embebidas vs externas** | Alta | Alto | El cargador `Model.h` soporta embebidas, pero falla si un GLTF usa rutas absolutas. Se debe priorizar buscar archivos `.glb` empacados y autocontenidos en todo momento. |
| **Coordenadas UV faltantes** | Alta | Medio | Si un modelo no tiene UVs generadas, fallará el mapeo de textura en `Model.h`. Instruir al agente que aplique un material Unlit (Color plano) en ese caso específico. |
| **Tamaño/Escala impredecible** | Alta | Alto | Modificar la escala a ciegas basándose en capturas 2D tomará muchos ciclos de prueba y error. El agente debería imprimir en consola el *Bounding Box* del modelo (min/max X, Y, Z) al cargarlo con Assimp para auto-calcular un factor de escala inicial. |
| **Normales invertidas / Backface Culling** | Media | Medio | Algunos modelos CC0 tienen polígonos invertidos. Si el agente nota el modelo invisible u oscuro, desactivar `glEnable(GL_CULL_FACE)` temporalmente en esa iteración. |

## Recomendaciones finales
1. **Corregir la descompresión de modelos**: Forzar al agente a buscar estrictamente archivos `.glb` (que contienen todo) o mover la carpeta entera del `.gltf` (para preservar `.bin` y texturas).
2. **Cambiar las fuentes de descarga**: Sustituir Sketchfab por GitHub (repositorios con `.glb` crudos) y Freesound/Pixabay por Wikimedia Commons para evitar que el bot se estanque con errores 403.
3. **Calcular Escala Automática**: Modificar el paso de "ajustar escala" instruyendo al agente a implementar un `AABB` básico con Assimp que imprima sus dimensiones y ajuste el factor de `glm::scale` en el primer intento.
4. **Fallback Rápido**: Ordenar al agente que si el modelo pesa más de 10 MB o baja los FPS tras 2 intentos de carga, haga `git checkout` de ese archivo y pase al siguiente modelo usando el fallback procedural instantáneamente.
5. **Separar Descarga de Integración**: Modificar los Workflows para ejecutar un "Script Descargador" inicial que acumule todos los recursos posibles en `app/assets/` primero, permitiendo que la fase C++ no sufra por latencias de red.
