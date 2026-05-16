# Pendientes del Proyecto — CGEIHC-P

> Proyecto: Calentamiento global en el polo norte
> Última actualización: 2026-05-15
> Estado general: **Fase de planificación / inicialización del entorno**

---

## Estado Actual

La primera entrega (Idea + Boceto + Storyboard + Blueprints + Modelado + Rigging + Materiales + Animación + Iluminación/Texturizado + Renderizado + Sonido + Interacción) fue entregada el **06/05/2026**.

El proyecto está en la transición hacia la **fase de implementación técnica** (configuración del entorno + código + assets).

---

## Prioridad Inmediata

- [ ] **Leer las especificaciones de la materia completas** (docx) para confirmar fechas de entrega intermedias
- [ ] **Configurar el entorno de desarrollo** (Visual Studio 2022 + CMake + GLAD + GLFW + GLM + Assimp)
- [ ] **Crear el repositorio del código fuente** (separado de este repositorio de organización, o subcarpeta `src/`)
- [ ] **Prototipo 0**: ventana OpenGL con cámara free-fly funcionando

---

## Fase 1 — Infraestructura y Prototipo Base

- [ ] Configurar CMakeLists.txt con FetchContent (GLFW, GLAD, GLM, Assimp, stb_image, ImGui)
- [ ] Implementar módulo de ventana (GLFW + contexto OpenGL 3.3 Core)
- [ ] Implementar cámara free-fly (WASD + mouse + delta time)
- [ ] Implementar cargador de shaders GLSL (compilación + linking en runtime)
- [ ] Renderizar primitiva de prueba (cubo, plano)
- [ ] Implementar cargador de texturas (stb_image)
- [ ] Implementar carga de modelos básica (Assimp, OBJ simple)

---

## Fase 2 — Escenario Base (Entorno Ártico)

- [ ] Modelar / obtener entorno base en Blender:
  - [ ] Terreno nevado (plano subdividido + displacement map)
  - [ ] Skybox polar (cubemap)
  - [ ] Superficie oceánica (plane para shader procedural)
  - [ ] Iceberg(s) iniciales
- [ ] Exportar assets a formatos compatibles con Assimp (OBJ/FBX/GLTF)
- [ ] Integrar assets en la escena OpenGL
- [ ] Implementar shader Blinn-Phong básico
- [ ] Implementar iluminación base de la escena (sol ártico, luz ambiental)

---

## Fase 3 — Shaders Avanzados y Materiales

- [ ] Shader de hielo (Fresnel, transparencia parcial, reflejos especulares)
- [ ] Shader de océano procedural (Perlin Noise en vertex shader, ondas dinámicas)
- [ ] Environment mapping en agua y superficies reflectivas
- [ ] Normal maps en hielo, nieve y rocas
- [ ] Shader de niebla volumétrica
- [ ] Efectos de post-procesado: Bloom, corrección de color

---

## Fase 4 — Estructura del Museo y Navegación

- [ ] Implementar zonas/módulos del museo (trigger zones por proximidad)
- [ ] Diseñar layout de la escena: vestíbulo → dos caminos → zona final
- [ ] Modelar carteles informativos y elementos de UI en el entorno
- [ ] Implementar sistema de triggers (detección de proximidad del jugador)
- [ ] Implementar HUD (Dear ImGui): temperatura, año, indicadores de ruta

---

## Fase 5 — Animaciones (3 tipos requeridos)

- [ ] **Animación básica**: turbinas girando, vehículos en movimiento
- [ ] **Keyframe (LERP/SLERP)**:
  - [ ] Iceberg derritiéndose (intacto → fragmentado)
  - [ ] Apertura de módulos informativos
  - [ ] Animaciones de fauna (ciclos de movimiento)
- [ ] **Procedural**:
  - [ ] Sistema de partículas de nieve
  - [ ] Partículas de humo/metano (ruta problemática)
  - [ ] Ondas del océano (ya en shader Fase 3)

---

## Fase 6 — Fauna y Personajes

- [ ] Obtener/modelar animales árticos:
  - [ ] Oso polar (con rigging y caminata)
  - [ ] Foca
  - [ ] Pingüino
  - [ ] Otros (morsa, aves, peces)
- [ ] Importar animaciones de rigging via Assimp (bone animations)
- [ ] Integrar en la escena con triggers narrativos

---

## Fase 7 — Audio

- [ ] Configurar OpenAL Soft + libsndfile
- [ ] Obtener efectos de sonido (licencias libres):
  - [ ] Viento ártico, crujidos de hielo, océano
  - [ ] Fauna ártica
  - [ ] Sonidos industriales (ruta problemática)
  - [ ] Música de fondo (ruta sostenible)
- [ ] Implementar audio 3D posicional (atenuación por distancia)
- [ ] Disparar sonidos en eventos de interacción

---

## Fase 8 — Pulido y Optimización

- [ ] Frustum culling
- [ ] Hardware instancing (rocas, partículas)
- [ ] Revisión de rendimiento (target: >30 fps en hardware modesto)
- [ ] Corrección de bugs visuales (con RenderDoc si necesario)
- [ ] Pruebas con usuarios (al menos 1 ronda de feedback)

---

## Entregables Finales

- [ ] **Software ejecutable** (.exe con InstallForge)
  - [ ] `README.txt` con instrucciones de instalación y uso
  - [ ] `manual.pdf` con descripción del sistema
- [ ] **Reporte en español** (20-35 páginas, PDF)
  - [ ] Resumen, introducción con estado del arte (APA)
  - [ ] Metodología, experimentos, resultados, conclusiones individuales
- [ ] **Reporte en inglés** (misma estructura)
- [ ] **Video demostrativo** (3-5 min, MP4/H.264, narrado)
- [ ] **Presentación oral y digital**

---

## Notas y Decisiones

- El código fuente **no se entrega** — se revisa en sesiones de avances
- Empaquetador: **InstallForge** (gratis)
- Editor de video: **DaVinci Resolve** (gratis, recomendado por el profesor)
- Referencias: usar **Mendeley o Zotero** para formato APA
- Todo sube a **Google Drive** del equipo (enlace en Moodle)

---

## Recursos Útiles

- **learnopengl.com** — Tutorial principal de referencia
- **cursos.computer-graphics.com.mx** — Plataforma del curso (Moodle)
- **Shaders de referencia del profesor**: `13_wavesAnimation-fresnel.vs/.fs`
- **Texturas CC0**: ambientcg.com, polyhaven.com, 3dtextures.me
- **Generación de modelos IA**: meshy.ai
