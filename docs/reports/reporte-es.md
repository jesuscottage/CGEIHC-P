# Nuestro Mundo: Simulación Interactiva 3D del Calentamiento Global en el Polo Norte

## Reporte Final del Proyecto

---

**Materia**: Computación Gráfica e Interacción Humano-Computadora (CGEIHC)
**Grupo**: Teoría 2
**Docente**: Dr. Sergio Teodoro Vite
**Institución**: Universidad Nacional Autónoma de México — Facultad de Ingeniería
**Semestre**: 2026-2

### Equipo

| Nombre | Número de Cuenta |
|--------|-----------------|
| Hernández Cabañas Jesús | 422062224 |
| Navarro Nuño Juan Pablo | 319086779 |
| Macias Niño Carmen Violeta | 318285450 |
| Martínez Jiménez Israel | 312032184 |

**Fecha de entrega**: Mayo 2026

---

## Resumen

Se desarrolló un sistema gráfico interactivo 3D en tiempo real que simula un museo virtual educativo ambientado en el Polo Norte, con el objetivo de concientizar sobre el calentamiento global y sus efectos en los ecosistemas árticos. El sistema fue implementado en C++17 utilizando OpenGL 3.3 Core Profile con pipeline programable (GLSL), integrando técnicas de iluminación Blinn-Phong con reflexión Fresnel, environment mapping mediante cubemap, tres tipos de animación (básica, keyframe y procedural), y un recorrido narrativo guiado. El proyecto ejecuta de forma estable a 59-61 FPS en hardware gráfico integrado (AMD Radeon), demostrando la viabilidad de aplicaciones de computación gráfica educativas con rendimiento en tiempo real.

**Palabras clave**: computación gráfica, OpenGL, tiempo real, calentamiento global, museo virtual, interacción humano-computadora.

---

## 1. Introducción

### 1.1 Contexto del problema

El calentamiento global representa una de las mayores amenazas para los ecosistemas polares. Según el Panel Intergubernamental sobre Cambio Climático (IPCC, 2023), la temperatura promedio del Ártico ha aumentado al doble de la tasa global en las últimas cuatro décadas. La extensión del hielo marino ártico ha disminuido aproximadamente un 13% por década desde 1979. Estos cambios tienen consecuencias directas sobre la fauna ártica, el nivel del mar y los patrones climáticos globales.

La educación ambiental es fundamental para generar conciencia sobre estos problemas. Sin embargo, los medios tradicionales (textos, imágenes estáticas, videos) no logran transmitir la escala y urgencia de los cambios climáticos de manera inmersiva. Los entornos 3D interactivos ofrecen una alternativa educativa que permite al usuario experimentar visualmente las consecuencias del calentamiento global.

### 1.2 Objetivo del proyecto

Diseñar e implementar un sistema gráfico interactivo 3D que simule un museo virtual en el Polo Norte, donde el usuario recorre módulos educativos sobre las causas, consecuencias y soluciones del calentamiento global, utilizando técnicas de computación gráfica en tiempo real.

### 1.3 Alcance

El sistema incluye:
- Renderizado 3D en tiempo real con OpenGL 3.3 Core Profile
- Dos rutas narrativas (consecuencias y soluciones) con 7 módulos interactivos
- Iluminación avanzada (Blinn-Phong + Fresnel)
- Environment mapping (skybox cubemap)
- Tres tipos de animación (básica, keyframe LERP, procedural)
- Shader de agua con desplazamiento de vértices
- Sistema de partículas de nieve
- Interfaz de usuario con Dear ImGui
- Audio ambiental con miniaudio

---

## 2. Marco Teórico

### 2.1 Pipeline de renderizado en OpenGL

OpenGL (Open Graphics Library) es una API estándar para renderizado de gráficos 2D y 3D acelerado por hardware. La versión 3.3 Core Profile elimina la funcionalidad fija (fixed-function pipeline) y requiere el uso de shaders programables en GLSL (OpenGL Shading Language) (de Vries, 2020).

El pipeline de renderizado moderno consta de las siguientes etapas:

1. **Vertex Shader**: transforma las coordenadas de los vértices del espacio del modelo al espacio de recorte (clip space) mediante las matrices Model, View y Projection (MVP).
2. **Rasterización**: convierte los primitivos geométricos en fragmentos.
3. **Fragment Shader**: calcula el color final de cada fragmento aplicando iluminación, texturas y efectos.
4. **Tests y blending**: profundidad (depth test), stencil y mezcla alfa (alpha blending).

### 2.2 Modelo de iluminación Blinn-Phong

El modelo Blinn-Phong (Blinn, 1977) es una aproximación eficiente de la iluminación local que descompone la reflectancia en tres componentes:

- **Ambiente**: iluminación indirecta constante que evita que las superficies en sombra sean completamente negras.
- **Difusa**: reflectancia lambertiana proporcional al coseno del ángulo entre la normal de la superficie y la dirección de la luz.
- **Especular**: reflejo brillante calculado con el vector medio (half-vector) entre la dirección de la luz y la dirección de vista.

La fórmula del componente especular Blinn-Phong es:

```
I_spec = K_s * (N · H)^n
```

donde H = normalize(L + V) es el half-vector, N es la normal, y n es el exponente de brillo.

### 2.3 Reflexión Fresnel

El efecto Fresnel describe cómo la reflectancia de una superficie varía según el ángulo de incidencia. La aproximación de Schlick (1994) proporciona una evaluación eficiente:

```
F(θ) = F₀ + (1 - F₀)(1 - cos θ)⁵
```

donde F₀ es la reflectancia a incidencia normal (0.02 para agua, 0.04 para dieléctricos). Este efecto es fundamental para superficies como agua y hielo, donde los bordes reflejan más que el centro.

### 2.4 Environment mapping con cubemaps

Un cubemap es una textura compuesta por 6 imágenes cuadradas que representan las caras de un cubo infinito alrededor de la escena. Se utiliza para:
- **Skybox**: renderizar un fondo que simula un cielo o entorno lejano.
- **Reflexiones**: mapear la dirección de reflexión sobre el cubemap para simular reflejos del entorno.

El renderizado del skybox utiliza el truco `gl_Position.xyww` en el vertex shader, que fuerza z_NDC = 1.0 (profundidad máxima), garantizando que el skybox siempre se dibuje detrás de toda la geometría (de Vries, 2020).

### 2.5 Animación en tiempo real

Se implementaron tres tipos de animación según los requisitos de la rúbrica:

1. **Animación básica**: transformaciones directas (traslación, rotación, escalado) aplicadas cada frame.
2. **Animación keyframe (LERP/SLERP)**: interpolación lineal entre estados inicial y final controlada por un parámetro t ∈ [0, 1].
3. **Animación procedural**: generada matemáticamente sin fotogramas predefinidos, usando funciones sinusoidales, ruido y derivadas.

### 2.6 Interacción humano-computadora

El diseño de la interacción sigue principios de usabilidad (Nielsen, 1994):
- **Visibilidad del estado**: HUD con FPS, posición y progreso del módulo activo.
- **Correspondencia**: controles WASD+mouse intuitivos para navegación en primera persona.
- **Prevención de errores**: colisión AABB para evitar que el usuario salga del museo.
- **Retroalimentación**: indicadores visuales de proximidad a módulos interactivos.

---

## 3. Desarrollo

### 3.1 Arquitectura del sistema

El sistema está organizado en capas modulares:

```
main.cpp (loop principal, AppState)
  ├── core/       Window, Input, Time
  ├── graphics/   Shader, Mesh, Texture, Model, CameraFPS, Skybox
  ├── scene/      Museum, ModuleScene, SnowSystem
  └── vendor/     GLAD, stb_image, miniaudio
```

**Stack técnico**:
- **Lenguaje**: C++17
- **API gráfica**: OpenGL 3.3 Core Profile
- **Ventana/eventos**: GLFW 3.4
- **Matemáticas**: GLM 1.0.1
- **Carga de modelos**: Assimp 5.4.3
- **Texturas**: stb_image (header-only)
- **UI**: Dear ImGui 1.91.6
- **Audio**: miniaudio 0.11+ (header-only)
- **Build**: CMake 3.20+ con FetchContent

### 3.2 Pipeline de renderizado implementado

El orden de renderizado por frame es:

1. **Geometría opaca**: museo (suelo texturizado, paredes texturizadas, techo, plataformas)
2. **Modelos 3D**: escenas de módulos (modelos GLB vía Assimp o geometría procedural)
3. **Fauna decorativa**: foca, zorro ártico, gaviota (geometría procedural)
4. **Letreros 3D**: paneles en cruz sobre cada módulo
5. **Agua semitransparente**: grid 200×200m con desplazamiento sinusoidal (alpha blending)
6. **Nieve**: 100 partículas billboard orientadas a cámara (alpha blending)
7. **Skybox**: cubemap con depth trick `gl_Position.xyww`
8. **ImGui**: overlays de interfaz de usuario

### 3.3 Shaders implementados

#### 3.3.1 Standard shader (Blinn-Phong + Fresnel)

El fragment shader principal implementa:
- Iluminación hemisférica ártica (luz ambiente diferenciada cielo/suelo)
- Blinn-Phong con half-vector para componente especular
- Fresnel de Schlick para bordes con F₀ = 0.04
- Niebla exponencial cuadrática con color configurable

#### 3.3.2 Water shader

El vertex shader desplaza los vértices verticalmente usando 3 capas sinusoidales:
- Capa principal: onda en dirección X
- Capa cruzada: onda en dirección Z
- Capa de alta frecuencia: onda diagonal

El fragment shader aplica:
- Mezcla de color profundo/superficial según la altura de onda
- Espuma en las crestas (WaveHeight > 0.6 × amplitud)
- Reflexión Fresnel del cielo con F₀ = (0.02, 0.025, 0.03)
- Transparencia (alpha = 0.88)

#### 3.3.3 Skybox shader

- Vertex: `gl_Position = pos.xyww` para z_NDC = 1.0
- Fragment: muestreo del cubemap con coordenadas de textura 3D

#### 3.3.4 Unlit shader

Shader sin iluminación para billboards de nieve y letreros de color.

### 3.4 Sistema de módulos narrativos

El museo contiene 7 módulos interactivos organizados en dos rutas:

**Ruta izquierda (consecuencias)**:
- **M1_IZQ**: Deshielo del iceberg (escala decrece con animT)
- **M2_IZQ**: Pérdida de hábitat del oso polar (capa de hielo encogiéndose)
- **M3_IZQ**: Inundación de ciudades (nivel de agua subiendo con modelos de edificios reales)

**Ruta derecha (soluciones)**:
- **M1_DER**: Turbina eólica (velocidad angular crece con animT)
- **M2_DER**: Auto eléctrico (movimiento sinusoidal, modelo GLB real)
- **M3_DER**: Reforestación (árbol que crece desde semilla, modelo GLB real)

**Módulo compartido**:
- **M5**: Globo terráqueo giratorio con líneas de acuerdos internacionales (modelo GLB esférico)

Cada módulo se activa con la tecla E cuando el usuario está dentro del radio de proximidad (5m). La animación progresa de t=0 a t=1 a lo largo de su duración configurada (8-12 segundos).

### 3.5 Integración de assets reales

Se integraron assets CC0 (Creative Commons Zero) de fuentes sin autenticación:

- **Texturas PBR**: 8 mapas de color de ambientcg.com (concreto, hielo, nieve, metal, madera, asfalto)
- **Skybox**: 6 caras PNG (512×512) de OpenGameArt.org (cielo nublado CC0)
- **Modelos 3D**: 6 archivos GLB de kenney.nl y KhronosGroup (árbol, auto, edificios, esfera)
- **Audio**: 3 archivos de archive.org (viento ártico, olas oceánicas, sonido de activación)
- **Fuente**: Roboto-Regular.ttf de Google Fonts (Apache 2.0)

Para resolver problemas de carga en paths con caracteres Unicode en Windows, se implementó un mecanismo de carga desde memoria (`ReadFileFromMemory` de Assimp) como fallback cuando la carga directa de archivos falla.

### 3.6 Flujo narrativo (AppState)

El sistema implementa una máquina de estados:

```
TITULO → JUGANDO → CIERRE → SALIR
```

- **TITULO**: pantalla de bienvenida con nombre del proyecto y créditos.
- **JUGANDO**: exploración libre del museo con cámara FPS, activación de módulos.
- **CIERRE**: activado cuando M5 completa su animación; pantalla de reflexión con cuenta regresiva de 5 segundos.
- **SALIR**: limpieza de recursos y cierre de la aplicación.

---

## 4. Resultados

### 4.1 Rendimiento

El sistema fue probado en hardware gráfico integrado AMD Radeon con los siguientes resultados:

| Módulo | FPS promedio | animT al frame 600 | Estado |
|--------|-------------|-------------------|--------|
| M1_IZQ | 59.97 | 0.891 | Estable |
| M2_IZQ | 59.98 | 1.000 | Estable |
| M3_IZQ | 59.10 | 0.743 | Estable |
| M1_DER | 60.96 | 1.000 | Estable |
| M2_DER | 60.95 | 1.000 | Estable |
| M3_DER | 60.97 | 1.000 | Estable |
| M5 | 60.95 | 0.744 | Estable |

Todos los módulos mantienen FPS superiores a 59, lo cual garantiza una experiencia fluida.

### 4.2 Cumplimiento de rúbrica

| Requisito | Implementación | Estado |
|-----------|---------------|--------|
| OpenGL 3.3+ Core Profile | GLSL 330 core en todos los shaders | Cumplido |
| 2+ técnicas de iluminación | Blinn-Phong + Fresnel | Cumplido |
| 1+ environment mapping | Skybox cubemap con carga de PNGs | Cumplido |
| 3 tipos de animación | Básica + Keyframe LERP + Procedural (agua, nieve) | Cumplido |
| Recorrido narrativo | Título → 2 rutas → M5 → Cierre | Cumplido |
| Interacciones contextuales | Tecla E para activar módulos por proximidad | Cumplido |
| Fluidez adecuada | 59-61 FPS estables | Cumplido |

### 4.3 Capturas de pantalla

Las capturas de pantalla se generan automáticamente mediante `glReadPixels` + `stb_image_write` en los frames 60, 300 y 600, almacenadas en el directorio `active/`.

---

## 5. Conclusiones

Se logró implementar exitosamente un museo virtual 3D interactivo sobre el calentamiento global en el Polo Norte, cumpliendo todos los requisitos técnicos de la rúbrica del curso. El sistema demuestra la viabilidad de utilizar gráficos 3D en tiempo real como herramienta educativa para temas ambientales.

Las principales contribuciones técnicas del proyecto incluyen:

1. **Pipeline de renderizado completo** con 4 shaders especializados (estándar, agua, skybox, unlit).
2. **Integración de assets CC0** con mecanismo de fallback robusto (modelos reales + geometría procedural).
3. **Sistema de módulos narrativos** con animación keyframe y activación por proximidad.
4. **Verificación autónoma** mediante screenshots programáticos y archivos de estado JSON.

Como trabajo futuro, se podría expandir el sistema con:
- Más modelos 3D detallados para cada módulo
- Audio 3D posicional por módulo
- Sombras (shadow mapping)
- Animación esquelética para la fauna
- Soporte multilingüe en la interfaz

---

## 6. Referencias

Angel, E. y Shreiner, D. (2011). *Interactive Computer Graphics: A Top-Down Approach with Shader-Based OpenGL* (6a ed.). Addison-Wesley.

Blinn, J. F. (1977). Models of light reflection for computer synthesized pictures. *ACM SIGGRAPH Computer Graphics*, 11(2), 192-198.

de Vries, J. (2020). *Learn OpenGL: Graphics Programming*. Recuperado de https://learnopengl.com/

Hearn, D. y Baker, M. P. (2006). *Computer Graphics with OpenGL* (4a ed.). Prentice Hall.

IPCC (2023). *Climate Change 2023: Synthesis Report*. Intergovernmental Panel on Climate Change.

Kenney (2024). *Nature Kit, Car Kit, City Kit (Commercial)*. Recuperado de https://kenney.nl/assets (Licencia CC0).

Nielsen, J. (1994). *Usability Engineering*. Morgan Kaufmann Publishers.

OpenGameArt (2025). *Cloudy Skyboxes*. Recuperado de https://opengameart.org/content/cloudy-skyboxes-0 (Licencia CC0).

Schlick, C. (1994). An inexpensive BRDF model for physically-based rendering. *Computer Graphics Forum*, 13(3), 233-246.

ambientCG (2026). *Free PBR Materials*. Recuperado de https://ambientcg.com/ (Licencia CC0).

---

## Anexo A: Estructura del Repositorio

```
CGEIHC-P/
├── app/
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── main.cpp
│   │   ├── core/         (Window, Input, Time)
│   │   ├── graphics/     (Shader, Mesh, Texture, Model, CameraFPS, Skybox)
│   │   ├── scene/        (Museum, ModuleScene, SnowSystem)
│   │   └── vendor/       (GLAD, stb, miniaudio)
│   ├── shaders/
│   │   ├── standard.vert / standard.frag
│   │   ├── water.vert / water.frag
│   │   ├── skybox.vert / skybox.frag
│   │   └── unlit.vert / unlit.frag
│   └── assets/
│       ├── models/       (GLB: tree, electric_car, buildings, globe)
│       ├── textures/     (PNG: floor, wall, ice, snow, metal, wood, path)
│       ├── skybox/       (6 caras PNG)
│       ├── audio/        (MP3/OGG: ambient, waves, activate)
│       └── fonts/        (Roboto-Regular.ttf)
├── docs/
│   ├── knowledge/
│   ├── plans/
│   ├── reports/
│   └── reviews/
└── build/                (generado por CMake)
```

## Anexo B: Instrucciones de Compilación

```bash
# Prerrequisitos: Visual Studio 2022, CMake 3.20+, Git
git clone https://github.com/jesuscottage/CGEIHC-P.git
cd CGEIHC-P
cmake -B build -S app -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
./build/Release/CGEIHC.exe
```

## Anexo C: Controles

| Tecla | Acción |
|-------|--------|
| W/A/S/D | Movimiento (adelante/izquierda/atrás/derecha) |
| Mouse | Orientación de la cámara |
| E | Activar módulo cercano |
| ESPACIO/ENTER | Avanzar desde pantalla de título |
| ESC | Salir de la aplicación |
