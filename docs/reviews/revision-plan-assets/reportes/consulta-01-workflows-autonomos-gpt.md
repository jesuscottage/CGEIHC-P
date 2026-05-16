## Veredicto general

**Parcialmente ejecutable de forma autónoma.**

El plan es sólido para:

* ambientCG
* Poly Haven
* Google Fonts
* algunas descargas directas con curl

Pero **fallará de forma consistente en Sketchfab, Freesound y parcialmente Pixabay** si el agente depende exclusivamente de scraping web + curl sin autenticación humana.

El principal problema no es técnico de C++/OpenGL, sino de:

* autenticación web
* URLs efímeras
* descargas protegidas por sesión
* JavaScript dinámico
* rate limits
* anti-bot

El pipeline sí puede automatizarse casi completamente si:

1. se eliminan dependencias de Sketchfab/Freesound login-required,
2. se usa un repositorio de assets “automation-friendly”,
3. se agregan pasos de validación y normalización de modelos.

---

## Pasos que FALLARÁN

| #  | Workflow          | Paso                                   | Razón                                                | Solución                                                         |
| -- | ----------------- | -------------------------------------- | ---------------------------------------------------- | ---------------------------------------------------------------- |
| 1  | WF-02 Sketchfab   | `WebFetch → obtener link descarga`     | Sketchfab usa JS dinámico + URLs firmadas            | Usar Sketchfab Data API oficial con token o reemplazar Sketchfab |
| 2  | WF-02 Sketchfab   | `curl model.zip`                       | Muchas descargas requieren sesión/login              | Migrar a Poly Haven + Kenney + OpenGameArt                       |
| 3  | WF-03 Freesound   | descarga directa                       | Freesound requiere autenticación OAuth/API           | Reemplazar con Pixabay/ Mixkit / ZapSplat-free                   |
| 4  | WF-03 Pixabay     | curl directo                           | URLs pueden ser temporales o JS-generated            | Extraer URL MP3 final desde HTML o usar otro sitio               |
| 5  | WF-01 ambientCG   | “extraer URL ZIP”                      | El HTML puede cambiar                                | Usar naming convention/API estable                               |
| 6  | WF-04 Integración | Assimp load() universal                | Algunos GLB tienen texturas embebidas incompatibles  | Pipeline de conversión previa con gltf-transform o Blender CLI   |
| 7  | WF-04 Integración | reemplazar procedural por model.draw() | Escalas/orígenes inconsistentes                      | Añadir normalization step                                        |
| 8  | Skybox            | HDRI → cubemap                         | Conversión HDR a cubemap no trivial sin herramientas | Descargar cubemap ya preparado                                   |
| 9  | Audio             | formatos variados                      | OpenAL/SDL_mixer puede no soportar MP3 según build   | Convertir automáticamente a WAV/OGG                              |
| 10 | Modelos           | GLTF arbitrario                        | Algunos modelos tienen millones de triángulos        | Pre-validar polycount                                            |

---

## Análisis de sitios web

| Sitio                                                                                | Descarga sin login | Descarga con curl | CAPTCHAs/JS requerido | Alternativa viable    |
| ------------------------------------------------------------------------------------ | ------------------ | ----------------- | --------------------- | --------------------- |
| [ambientCG](https://ambientcg.com?utm_source=chatgpt.com)                            | Sí                 | Sí                | No                    | Excelente opción      |
| [Poly Haven](https://polyhaven.com?utm_source=chatgpt.com)                           | Sí                 | Sí                | No                    | Excelente opción      |
| [Sketchfab](https://sketchfab.com?utm_source=chatgpt.com)                            | Parcial            | Poco confiable    | Sí frecuente          | Evitar para autonomía |
| [Freesound](https://freesound.org?utm_source=chatgpt.com)                            | No práctico        | No práctico       | OAuth/API             | Evitar                |
| [Pixabay](https://pixabay.com?utm_source=chatgpt.com)                                | Parcial            | Parcial           | Algo de JS            | Usable con cuidado    |
| [Google Fonts](https://fonts.google.com?utm_source=chatgpt.com)                      | Sí                 | Sí                | No                    | Excelente             |
| [Kenney Assets](https://kenney.nl/assets?utm_source=chatgpt.com)                     | Sí                 | Sí                | No                    | Muy recomendado       |
| [OpenGameArt](https://opengameart.org?utm_source=chatgpt.com)                        | Parcial            | Variable          | A veces               | Bueno como fallback   |
| [Mixkit Sound Effects](https://mixkit.co/free-sound-effects/?utm_source=chatgpt.com) | Sí                 | Sí                | Poco JS               | Mejor que Freesound   |
| [HDRMAPS Freebies](https://hdrmaps.com/freebies/?utm_source=chatgpt.com)             | Sí                 | Sí                | No                    | Alternativa HDRI      |

---

## Mejoras al orden de ejecución

El orden actual no es óptimo para minimizar riesgos.

Orden recomendado:

```text
1. Infraestructura pipeline
2. Fuente + audio
3. Texturas
4. Skybox
5. Modelos simples
6. Validación automática de modelos
7. Integración OpenGL/Assimp
8. Optimización FPS
9. Screenshots automáticos
10. Empaquetado final
```

Mejor paralelización:

| Paralelizable                         | Comentario     |
| ------------------------------------- | -------------- |
| Texturas + audio + fuente             | Independientes |
| Skybox + texturas                     | Independientes |
| Descarga modelos + integración Assimp | En paralelo    |
| Conversión assets + build             | Pipeline batch |

También falta una fase crítica:

```text
NORMALIZACIÓN DE ASSETS
```

Debe ocurrir antes de integrar modelos.

Ejemplo:

* centrar pivote
* escalar a metros
* generar tangentes
* validar UVs
* convertir texturas
* reducir polígonos

---

## Assets faltantes

Para un museo narrativo sobre calentamiento global en el Polo Norte faltan varios assets importantes:

### Ambientales

* hielo roto / ice cracks decals
* partículas de nieve
* aurora boreal
* niebla polar
* océano ártico
* chunks de hielo flotando

### Narrativos

* paneles informativos
* señales científicas
* mapas climáticos
* pantallas/multimedia
* gráficos de temperatura
* barriles/polución/basura industrial

### Vida salvaje

* morsa
* ballena
* pingüinos (aunque no son del Ártico; evitar error geográfico)
* peces
* aves adicionales

### Arquitectónicos

* estación científica
* antenas
* generadores
* interiores museo

### Audio ambiental

* viento interior/exterior
* metal crujiendo
* pasos en nieve
* agua derritiéndose
* sonidos de maquinaria

---

## Riesgos técnicos no contemplados

| Riesgo                                        | Solución                                |
| --------------------------------------------- | --------------------------------------- |
| Modelos gigantes o microscópicos              | Normalizar bounding box automáticamente |
| Ejes Y-up/Z-up inconsistentes                 | Conversión de coordenadas               |
| GLB con texturas embebidas                    | Extraer con Assimp o convertir antes    |
| Materiales PBR incompatibles con renderer     | Fallback a albedo-only                  |
| Normales invertidas                           | Recalcular normales                     |
| Modelos sin UVs                               | Detectar y fallback procedural          |
| Assets demasiado pesados                      | Decimation automática                   |
| Tangentes ausentes                            | Generarlas offline                      |
| Texturas 4K enormes                           | Downscale automático                    |
| Nombres de archivos con espacios/unicode      | Sanitización                            |
| Licencias mal clasificadas                    | Verificación explícita CC0              |
| Rate limiting                                 | Cache local                             |
| Cambios HTML en sitios                        | Evitar scraping frágil                  |
| Animaciones GLTF no soportadas                | Ignorar canales animation               |
| Assimp crash con modelos corruptos            | Validación previa                       |
| Materiales metálicos/roughness no compatibles | Bake simplificado                       |
| Assets duplicados                             | Hash deduplication                      |
| OpenGL state pollution por modelos            | Renderer state isolation                |
| Audio sample rate incompatible                | ffmpeg normalize                        |
| Cubemap orientation incorrecta                | Validación visual automática            |
| Modelos con cientos de meshes                 | Merge opcional                          |

---

## Top 5 recomendaciones para éxito autónomo

1. Reemplazar Sketchfab completamente como fuente principal.

Usar:

* [Poly Haven](https://polyhaven.com?utm_source=chatgpt.com)
* [Kenney Assets](https://kenney.nl/assets?utm_source=chatgpt.com)
* [OpenGameArt](https://opengameart.org?utm_source=chatgpt.com)

Sketchfab no es confiable para automatización sin login/API.

2. Añadir un pipeline offline de normalización.

Ejemplo:

```bash
gltf-transform optimize model.glb out.glb
```

o Blender CLI:

```bash
blender -b -P normalize.py
```

Esto resolverá:

* escala
* tangentes
* normals
* triangulación
* compresión

3. Cambiar el workflow “buscar cualquier asset” por “whitelist curada”.

En vez de:

```text
buscar iceberg
```

usar:

```json
{
  "iceberg": "https://..."
}
```

La automatización es muchísimo más estable.

4. Implementar validación automática antes de integrar.

El pipeline debería rechazar automáticamente:

* > 100k triángulos
* sin UVs
* sin texturas
* formatos incompatibles
* bounding boxes absurdas

5. Mantener geometría procedural como fallback primario.

Para una entrega académica:

* estabilidad > fidelidad visual
* un modelo roto puede romper toda la demo
* procedural garantiza presentación funcional

El fallback procedural es correcto y muy realista.

---

## Evaluación final realista

| Área                         | Probabilidad de éxito autónomo |
| ---------------------------- | ------------------------------ |
| Texturas ambientCG           | 95%                            |
| Skybox Poly Haven            | 85%                            |
| Fonts                        | 100%                           |
| Audio Pixabay/Mixkit         | 70%                            |
| Sketchfab models             | 30%                            |
| Integración Assimp básica    | 80%                            |
| Pipeline completo end-to-end | 60–70%                         |

Conclusión práctica:

El proyecto puede automatizarse exitosamente si:

* se elimina dependencia de Sketchfab/Freesound,
* se usan assets previamente curados,
* se añade normalización offline,
* se simplifica el pipeline PBR.

Sin esos cambios, el agente probablemente quedará bloqueado en descargas protegidas o modelos incompatibles.
