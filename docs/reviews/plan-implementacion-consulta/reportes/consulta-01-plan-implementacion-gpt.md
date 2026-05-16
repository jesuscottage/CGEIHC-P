# 1. Fases de desarrollo

| Fase | Nombre                      | Objetivo                                                                                        | Criterio de salida                                                          |
| ---- | --------------------------- | ----------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------- |
| 0    | Bootstrap del proyecto      | Tener build reproducible en VS2022 con todas las dependencias integradas por CMake FetchContent | `cmake --build` genera `.exe` limpio en Windows sin copiar DLLs manualmente |
| 1    | Core OpenGL mínimo          | Crear ventana GLFW, contexto OpenGL 3.3 Core, loop principal y render básico                    | Se renderiza un cubo/textura con cámara FPS y deltaTime estable             |
| 2    | Sistema de aplicación       | Implementar `AppState`, input, cámara FPS, timing y scene loop                                  | Cambio funcional entre TITLE → LOADING → MUSEUM                             |
| 3    | Pipeline de render básico   | Shader manager, mesh simple, VAO/VBO/EBO, texturas y matrices MVP                               | Se renderizan múltiples meshes con transform independientes                 |
| 4    | Asset pipeline              | Integrar Assimp GLTF, caché de modelos/texturas y resource manager                              | Un GLTF complejo carga correctamente con materiales/texturas                |
| 5    | Arquitectura del museo      | Construir layout T invertida, suelo, paredes y navegación                                       | Usuario puede recorrer todo el museo sin glitches de cámara                 |
| 6    | Sistema modular interactivo | Implementar `ModuloBase` + triggers + estados + LERP                                            | Los 7 módulos responden con tecla `E` y reset instantáneo                   |
| 7    | Animación GLTF              | Integrar animación skeletal para oso polar idle loop                                            | Idle animation del oso corre continuamente sin afectar FPS                  |
| 8    | Iluminación avanzada        | Implementar Blinn-Phong + Fresnel + niebla exponencial                                          | Toda la escena usa lighting estable y consistente                           |
| 9    | Shaders especiales          | Agua procedural, skybox cubemap y shader unlit                                                  | Agua animada + skybox funcional + letreros visibles                         |
| 10   | Sistema de partículas       | Nieve billboard CPU→GPU dinámica                                                                | 80–120 partículas estables a FPS objetivo                                   |
| 11   | UI y texto                  | Dear ImGui + stb_truetype + overlays narrativos                                                 | `[E] Explorar`, title screen y créditos funcionan                           |
| 12   | Audio                       | OpenAL Soft + audio ambiental + fuentes 3D                                                      | Audio posicional estable y sincronizado                                     |
| 13   | Integración narrativa       | Flujo completo inicio → recorrido → cierre                                                      | Usuario puede completar narrativa de inicio a fin                           |
| 14   | Optimización y QA           | Resolver leaks, stutter y validación final                                                      | Build final estable durante 15+ min continuos                               |

---

# 2. Prerequisitos antes de shaders avanzados

Checklist obligatoria antes de tocar Blinn-Phong/Fresnel/agua:

* [ ] GLFW crea contexto OpenGL 3.3 Core correctamente
* [ ] GLAD inicializa sin errores
* [ ] `glEnable(GL_DEPTH_TEST)` funcionando
* [ ] Cámara FPS estable con deltaTime
* [ ] Sistema MVP correcto:

  * Model
  * View
  * Projection
* [ ] Shader loader:

  * compilación
  * linking
  * logs de error legibles
* [ ] VAO/VBO/EBO reutilizables
* [ ] Texturas 2D cargadas con stb_image
* [ ] Mesh simple renderizado con textura
* [ ] Assimp importa GLTF correctamente
* [ ] Coordenadas UV correctas
* [ ] Normales correctas
* [ ] Render de múltiples objetos por frame
* [ ] ResourceManager evita recargar texturas/modelos
* [ ] Sistema de transforms estable
* [ ] DeltaTime desacoplado de FPS
* [ ] Input GLFW desacoplado del render
* [ ] Skybox básico renderiza sin lighting
* [ ] Uniform system funcional:

  * matrices
  * vec3
  * float
  * arrays
* [ ] Estado OpenGL limpio:

  * depth test
  * blending
  * culling
* [ ] ImGui coexistiendo sin romper depth state
* [ ] Frame pacing estable (>60 FPS objetivo)

No implementar shaders avanzados antes de que todo esto funcione.

---

# 3. Estructura de código `src/`

```text
src/
├── main.cpp
├── core/
│   ├── Application.h/.cpp
│   ├── AppState.h
│   ├── Window.h/.cpp
│   ├── Input.h/.cpp
│   ├── Time.h/.cpp
│   └── Config.h
│
├── graphics/
│   ├── Renderer.h/.cpp
│   ├── Shader.h/.cpp
│   ├── Texture.h/.cpp
│   ├── Cubemap.h/.cpp
│   ├── Material.h/.cpp
│   ├── Mesh.h/.cpp
│   ├── Model.h/.cpp
│   ├── CameraFPS.h/.cpp
│   ├── Framebuffer.h/.cpp
│   └── particles/
│       ├── SnowSystem.h/.cpp
│       └── BillboardRenderer.h/.cpp
│
├── animation/
│   ├── Animator.h/.cpp
│   ├── Skeleton.h/.cpp
│   ├── Bone.h/.cpp
│   ├── AnimationClip.h/.cpp
│   ├── Keyframe.h
│   └── Lerp.h
│
├── modules/
│   ├── ModuloBase.h/.cpp
│   ├── ModuloIceberg.h/.cpp
│   ├── ModuloPolarBear.h/.cpp
│   ├── ModuloSeaLevel.h/.cpp
│   ├── ModuloWindTurbine.h/.cpp
│   ├── ModuloElectricCar.h/.cpp
│   ├── ModuloTree.h/.cpp
│   └── ModuloGlobalConnections.h/.cpp
│
├── scene/
│   ├── Scene.h/.cpp
│   ├── MuseumScene.h/.cpp
│   ├── Entity.h/.cpp
│   ├── Transform.h
│   ├── Environment.h/.cpp
│   ├── SignSystem.h/.cpp
│   └── TriggerZone.h/.cpp
│
├── assets/
│   ├── ResourceManager.h/.cpp
│   ├── ModelLoader.h/.cpp
│   ├── TextureLoader.h/.cpp
│   ├── FontAtlas.h/.cpp
│   └── AudioLoader.h/.cpp
│
├── ui/
│   ├── UIManager.h/.cpp
│   ├── TitleScreen.h/.cpp
│   ├── CreditsScreen.h/.cpp
│   └── HUD.h/.cpp
│
├── audio/
│   ├── AudioEngine.h/.cpp
│   ├── AudioSource.h/.cpp
│   ├── AudioBuffer.h/.cpp
│   └── Listener.h/.cpp
│
├── utils/
│   ├── Logger.h/.cpp
│   ├── FileUtils.h/.cpp
│   ├── MathUtils.h
│   └── GLDebug.h/.cpp
│
└── shaders/
    ├── standard.vert
    ├── standard.frag
    ├── water.vert
    ├── water.frag
    ├── skybox.vert
    ├── skybox.frag
    ├── unlit.vert
    └── unlit.frag
```

---

## Clases principales

## `Application`

Responsabilidad:

* Lifecycle principal
* Estados globales
* Main loop

Métodos:

```cpp
void init();
void run();
void shutdown();

void update(float dt);
void render();
void changeState(AppState newState);
```

---

## `Renderer`

Responsabilidad:

* Orden global de render
* Estado OpenGL
* Draw calls

Métodos:

```cpp
void beginFrame();
void renderScene(Scene* scene);
void renderSkybox();
void renderTransparent();
void endFrame();
```

Orden recomendado:

1. Skybox
2. Opaque geometry
3. Agua
4. Partículas
5. ImGui

---

## `Shader`

Responsabilidad:

* Compilación GLSL
* Uniform API

Métodos:

```cpp
bool load(vertexPath, fragmentPath);

void use();

void setMat4();
void setVec3();
void setFloat();
void setInt();
```

---

## `ResourceManager`

CRÍTICO para evitar retrabajos.

Responsabilidad:

* Cargar una vez
* Cachear
* Reusar

Métodos:

```cpp
Model* loadModel(path);
Texture* loadTexture(path);
Shader* loadShader(name);

void clear();
```

Usar:

```cpp
std::unordered_map<std::string, Model*>
```

---

## `ModuloBase`

La pieza más importante de arquitectura.

Responsabilidad:

* Trigger
* Estados
* Timer
* Interacción

Estructura:

```cpp
enum class EstadoModulo {
    INACTIVO,
    ANIMANDO,
    COMPLETADO
};
```

```cpp
class ModuloBase {
protected:
    glm::vec3 position;
    float triggerRadius;

    float t;
    float duration;

    EstadoModulo estado;

public:
    virtual void update(float dt);
    virtual void render();

    virtual void animate(float normalizedT)=0;
    virtual void reset();

    bool isPlayerInside(glm::vec3 playerPos);
    void activate();
};
```

Patrón:

```cpp
t += dt;
float alpha = clamp(t / duration, 0, 1);

animate(alpha);

if(alpha >= 1.0f)
    estado = COMPLETADO;
```

---

## `MuseumScene`

Responsabilidad:

* Instanciar módulos
* Layout del museo
* Update narrativo

Métodos:

```cpp
void init();
void update(float dt);
void render();

ModuloBase* getActiveModule();
```

Debe mantener:

```cpp
std::vector<std::unique_ptr<ModuloBase>>
```

---

## `Animator`

Responsabilidad:

* Skeletal animation GLTF

Métodos:

```cpp
void update(float dt);
std::vector<glm::mat4> getFinalBoneMatrices();
```

IMPORTANTE:
No generalizar demasiado. Solo necesitan:

* 1 clip loop
* 1 modelo animado

---

## `SnowSystem`

Responsabilidad:

* Partículas nieve CPU side
* Upload dinámico VBO

Métodos:

```cpp
void update(float dt);
void uploadGPU();
void render();
```

Partícula:

```cpp
struct SnowParticle {
    glm::vec3 pos;
    float speed;
};
```

---

## `FontAtlas`

Responsabilidad:

* stb_truetype
* Rasterización TTF → textura OpenGL

Métodos:

```cpp
bool loadTTF(path);
GLuint buildAtlas();
void drawText3D();
```

NO implementar sistema tipográfico complejo.

Solo:

* ASCII
* atlas fijo
* texto baked

---

## `AudioEngine`

Responsabilidad:

* OpenAL device/context
* Sources/buffers

Métodos:

```cpp
void init();
void playAmbient();
void play3D();
void updateListener(CameraFPS&);
```

---

# 4. Riesgos y mitigaciones

| # | Riesgo                                             | Probabilidad | Mitigación concreta                                                                                                    |
| - | -------------------------------------------------- | ------------ | ---------------------------------------------------------------------------------------------------------------------- |
| 1 | Assimp GLTF + skeletal animation inconsistente     | Alta         | Implementar primero carga estática GLTF. Después agregar animación SOLO al oso polar. No intentar sistema genérico AAA |
| 2 | stb_truetype genera texto borroso o UV incorrectas | Media        | Usar atlas 1024x1024 baked una sola vez con Roboto-Regular.ttf a tamaño fijo                                           |
| 3 | ImGui rompe estados OpenGL                         | Alta         | Restaurar estados después de ImGui: depth test, blending, cull face, shader activo                                     |
| 4 | OpenAL Soft falla en runtime Windows               | Media        | Copiar `OpenAL32.dll` junto al `.exe` desde build script CMake                                                         |
| 5 | FetchContent rompe builds en VS2022                | Alta         | Congelar commits/tags exactos de cada dependencia. No usar branches latest                                             |

---

# 5. Recomendaciones adicionales

### 1. Congelar assets temprano

No cambiar modelos/texturas después de fase 6.

Cambiar assets tarde rompe:

* escalas
* pivotes
* animaciones
* colisiones trigger
* UVs

---

### 2. Estandarizar unidades desde el inicio

Convención fija:

* 1 unidad = 1 metro
* Y up
* grados para gameplay
* radianes solo internamente GLM

---

### 3. Separar “update” de “render”

Nunca mezclar lógica con draw calls.

Estructura correcta:

```cpp
update(dt);
render();
```

Evita bugs temporales en animaciones LERP.

---

### 4. No usar herencia profunda

Solo:

```cpp
ModuloBase -> ModuloEspecifico
```

Nada más.

Evitar arquitectura tipo engine comercial.

---

### 5. Instrumentar logs desde el día 1

Agregar:

```cpp
GLDebug
```

Y:

```cpp
glGetError()
```

en debug builds.

Shaders GLSL fallarán constantemente al inicio.

---

### 6. Resolver transparencia temprano

Necesitan transparencia para:

* nieve
* agua
* líneas M5
* UI
* billboards

Definir orden desde fase 8:

```text
opaque → transparent back-to-front
```

---

### 7. No optimizar prematuramente

Objetivo académico:

* estabilidad
* claridad
* entregable funcional

No implementar:

* ECS
* multithreading
* deferred rendering
* PBR
* job systems

---

### 8. Validar triggers visualmente

Durante desarrollo:

* renderizar círculos wireframe de trigger
* mostrar estado módulo debug ImGui

Ahorrará muchas horas.

---

### 9. Mantener shaders pequeños

`standard.frag` será el más complejo.

NO meter:

* sombras
* SSR
* PBR
* IBL
* normal mapping complejo

Blinn-Phong + Fresnel + fog es suficiente para la rúbrica.

---

### 10. Orden ideal de integración final

No integrar todo simultáneamente.

Orden recomendado:

```text
museo estático
→ módulos
→ lighting
→ skybox
→ agua
→ partículas
→ UI
→ audio
→ narrativa final
```

Ese orden minimiza retrabajos y facilita debugging.
