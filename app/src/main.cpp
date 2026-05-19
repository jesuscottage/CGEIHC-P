// main.cpp — Fase 10: Integración narrativa completa (título + recorrido + cierre)

#include "glad/glad.h"
#include "core/Window.h"
#include "core/Input.h"
#include "core/Time.h"
#include "graphics/CameraFPS.h"
#include "graphics/Shader.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/Model.h"
#include "graphics/Skybox.h"
#include "scene/Museum.h"
#include "scene/ModuleScene.h"
#include "scene/SnowSystem.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include "stb_image_write.h"
#include "miniaudio.h"

#ifndef ACTIVE_DIR
#define ACTIVE_DIR "active/"
#endif
#ifndef SHADERS_DIR
#define SHADERS_DIR "shaders/"
#endif
#ifndef ASSETS_DIR
#define ASSETS_DIR "assets/"
#endif

// ─────────────────────────────────────────────
// AppState
// ─────────────────────────────────────────────
enum class AppState { TITULO, JUGANDO, CIERRE, SALIR };

// ─────────────────────────────────────────────
// Test Mode CLI
// ─────────────────────────────────────────────
struct TestConfig {
    bool enabled        = false;
    std::string testModule    = "";
    int  activateAtFrame      = 60;
};

// Posiciones de cámara para test mode: 3.5m al sur del centro de cada módulo
// para que el objeto sea visible y la cámara esté dentro del trigger (radio 5m)
static const std::map<std::string, glm::vec3> MODULE_POSITIONS = {
    {"VESTIBULO",{  0.0f, 1.7f,  2.0f}},
    {"M1_IZQ",   {-12.0f, 1.7f,  9.5f}},
    {"M2_IZQ",   {-12.0f, 1.7f, 24.5f}},
    {"M3_IZQ",   {-12.0f, 1.7f, 39.5f}},
    {"M1_DER",   { 12.0f, 1.7f,  9.5f}},
    {"M2_DER",   { 12.0f, 1.7f, 24.5f}},
    {"M3_DER",   { 12.0f, 1.7f, 39.5f}},
    {"M5",       {  0.0f, 1.7f, 59.5f}},
};

TestConfig parseArgs(int argc, char** argv) {
    TestConfig cfg;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--test-module" && i + 1 < argc)
            { cfg.enabled = true; cfg.testModule = argv[++i]; }
        else if (std::string(argv[i]) == "--activate-frame" && i + 1 < argc)
            cfg.activateAtFrame = std::stoi(argv[++i]);
    }
    return cfg;
}

// ─────────────────────────────────────────────
// Screenshot + JSON
// ─────────────────────────────────────────────
static void saveScreenshot(const char* filename, int w, int h) {
    std::vector<unsigned char> pixels(w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    stbi_flip_vertically_on_write(true);
    std::string path = std::string(ACTIVE_DIR) + filename;
    stbi_write_png(path.c_str(), w, h, 3, pixels.data(), w * 3);
}

static void saveStateJSON(int frame, float fps, const glm::vec3& cam,
                          const std::string& mod, float modT)
{
    std::ofstream f(std::string(ACTIVE_DIR) + "state.json");
    f << "{\n"
      << "  \"frame\": " << frame << ",\n"
      << "  \"fps\": " << fps << ",\n"
      << "  \"camera\": [" << cam.x << ", " << cam.y << ", " << cam.z << "],\n"
      << "  \"activeModule\": \"" << mod << "\",\n"
      << "  \"moduleT\": " << modT << "\n"
      << "}\n";
}

// ─────────────────────────────────────────────
// Colisión museo — layout en T
// ─────────────────────────────────────────────
static void applyCollision(glm::vec3& pos, float prevX) {
    const float R = 0.35f;

    if (pos.z < -4.5f + R) pos.z = -4.5f + R;
    if (pos.z > 74.5f - R) pos.z = 74.5f - R;

    // Si el jugador está fuera del ancho del vestíbulo (X=±9.25), no puede retroceder a Z<9.8
    if (pos.z < 9.8f && (pos.x < -9.25f || pos.x > 9.25f))
        pos.z = 9.8f;

    if (pos.z < 9.8f) {
        // Vestíbulo: solo paredes exteriores
        if (pos.x < -9.6f + R) pos.x = -9.6f + R;
        if (pos.x >  9.6f - R) pos.x =  9.6f - R;
    } else if (pos.z < 60.0f) {
        // Corredores: paredes exteriores
        if (pos.x < -19.5f + R) pos.x = -19.5f + R;
        if (pos.x >  19.5f - R) pos.x =  19.5f - R;
        // Paredes interiores en X=±4.25 (grosor 0.5):
        //   cara interior (da al centro) en X=±4.0
        //   cara exterior (da al corredor) en X=±4.5
        if (prevX > 4.5f) {
            if (pos.x < 4.5f + R) pos.x = 4.5f + R;   // corredor der → bloqueado en cara exterior
        } else if (prevX < -4.5f) {
            if (pos.x > -4.5f - R) pos.x = -4.5f - R; // corredor izq → bloqueado en cara exterior
        } else {
            // centro (o dentro de la pared): bloqueado por caras interiores
            if (pos.x >  4.0f - R) pos.x =  4.0f - R;
            if (pos.x < -4.0f + R) pos.x = -4.0f + R;
        }
    } else {
        // Sala M5: solo paredes exteriores
        if (pos.x < -19.5f + R) pos.x = -19.5f + R;
        if (pos.x >  19.5f - R) pos.x =  19.5f - R;
    }
}

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────
int main(int argc, char** argv) {
    TestConfig testCfg = parseArgs(argc, argv);

    Window window;
    if (!window.init(1280, 720, "Nuestro Mundo — Polo Norte")) return -1;

    Input input;
    input.init(window.handle);

    Time time;
    CameraFPS camera;
    camera.position = glm::vec3(0.0f, 1.7f, 2.0f); // dentro del vestíbulo
    camera.yaw = 90.0f;

    if (testCfg.enabled) {
        auto it = MODULE_POSITIONS.find(testCfg.testModule);
        if (it != MODULE_POSITIONS.end())
            camera.position = it->second;
    }
    input.captureMouse();

    // ── Audio (miniaudio) ─────────────────────
    // El motor de audio se inicializa; si no hay archivos en assets/audio/
    // las llamadas a play_sound simplemente retornan error sin crashear.
    ma_engine audioEngine;
    bool audioOk = (ma_engine_init(nullptr, &audioEngine) == MA_SUCCESS);
    if (audioOk) {
        ma_engine_play_sound(&audioEngine, ASSETS_DIR "audio/ambient.mp3", nullptr);
    }

    // Handle para la narración (permite detenerla con Tab)
    ma_sound narratorSound;
    bool    narratorInit = false;

    // ── ImGui ─────────────────────────────────
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.handle, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    // Fuente: usar default de ImGui (evitar problemas con paths Unicode en Windows)
    // Roboto-Regular.ttf disponible en assets/fonts/ para empaquetado final

    // ── Shaders ───────────────────────────────
    Shader stdShader, unlitShader, skyboxShader, waterShader;
    if (!stdShader.load(SHADERS_DIR "standard.vert", SHADERS_DIR "standard.frag")) return -1;
    if (!unlitShader.load(SHADERS_DIR "unlit.vert",  SHADERS_DIR "unlit.frag"))    return -1;
    if (!skyboxShader.load(SHADERS_DIR "skybox.vert", SHADERS_DIR "skybox.frag"))  return -1;
    if (!waterShader.load(SHADERS_DIR "water.vert",   SHADERS_DIR "water.frag"))   return -1;

    // ── Museo ─────────────────────────────────
    Museum museum;
    museum.init();

    // ── Escenas animadas de módulos ────────────
    ModuleScene moduleScene;
    moduleScene.init();

    // ── Skybox ártico ─────────────────────────
    Skybox skybox;
    skybox.init();

    // ── Plano de agua exterior ─────────────────
    // Grid 200×200m centrado en (0, -0.3, 35) — océano Ártico rodeando el museo
    Mesh waterMesh = makeGrid(200.f, 200.f, 60, 60, 0.04f);

    // ── Sistema de nieve ───────────────────────
    SnowSystem snow;
    snow.init();

    // ── Texturas placeholder ───────────────────
    Texture whiteTex;
    whiteTex.loadWhite();

    // ── Uniforms globales ──────────────────────
    const glm::vec3 LIGHT_DIR   = glm::normalize(glm::vec3(-0.2f, -0.7f,  0.3f));
    const glm::vec3 LIGHT_COLOR = glm::vec3(1.0f,  0.97f, 0.90f);
    const glm::vec3 FOG_COLOR   = glm::vec3(0.72f, 0.80f, 0.88f);
    const float     FOG_DENSITY = 0.006f; // reducida para ver más lejos dentro del museo

    // En test mode se salta la pantalla de título para que las activaciones
    // de módulos por frame ocurran desde el frame 0 en estado JUGANDO
    AppState state     = testCfg.enabled ? AppState::JUGANDO : AppState::TITULO;
    double   titleTimer = glfwGetTime();

    int    frameCount    = 0;
    double fpsTimer      = glfwGetTime();
    int    fpsFrames     = 0;
    float  currentFPS    = 60.0f;
    double cierreTimer   = 0.0;   // momento en que se entró al estado CIERRE
    int    modulesCompleted = 0;  // cuántos módulos han llegado a animT=1.0

    // Multi-ángulo: frente, suelo, techo, animación media, animación final
    const int   CAPTURE_FRAMES[] = {60, 90, 120, 300, 600};
    const float CAPTURE_PITCH[]  = {0.f, -80.f, 60.f, 0.f, 0.f};
    const char* CAPTURE_NAMES[]  = {
        "front.png", "floor.png", "ceil.png", "anim_mid.png", "anim_end.png"
    };

    std::string activeModuleName = "ninguno";
    float       activeModuleT    = 0.0f;

    // Módulo actualmente activado (nullptr si ninguno)
    ModuleInfo* activeModulePtr  = nullptr;

    // ── Textos narrativos por módulo ──────────────────────────────────────
    static const std::map<std::string, std::string> MODULE_TEXTOS = {
        {"M1_IZQ",
         "Deshielo del Ártico\n\n"
         "El Ártico está perdiendo más del 13% de su superficie de hielo marino cada "
         "década, un ritmo sin precedentes en miles de años. Las temperaturas en esta "
         "región aumentan hasta cuatro veces más rápido que en el resto del planeta, "
         "derritiendo glaciares milenarios que tardarían siglos en formarse de nuevo.\n\n"
         "Este deshielo eleva el nivel global del mar, amenazando comunidades costeras "
         "en todos los continentes. Además, altera las corrientes oceánicas que "
         "regulan el clima de Europa y América, provocando fenómenos meteorológicos "
         "extremos cada vez más frecuentes e intensos."},
        {"M2_IZQ",
         "Fauna en Peligro\n\n"
         "El oso polar, símbolo del Ártico, depende del hielo marino para cazar focas "
         "y reproducirse. Con menos hielo cada año, estos animales deben nadar "
         "distancias cada vez mayores, agotando sus reservas energéticas. Se estima "
         "que dos tercios de la población mundial de osos polares podría desaparecer "
         "antes del año 2050 si el calentamiento continúa al ritmo actual.\n\n"
         "Morsas, ballenas bowhead, zorros árticos y cientos de especies migratorias "
         "también se ven afectadas. La pérdida de un eslabón en esta cadena alimentaria "
         "tiene consecuencias en cascada para todo el ecosistema ártico."},
        {"M3_IZQ",
         "Ciudades Inundadas\n\n"
         "Si las emisiones globales no se reducen drásticamente, el nivel del mar "
         "podría subir entre 0.5 y 1.3 metros para el año 2100. Ciudades como Miami, "
         "Nueva Orleans, Tokio, Ámsterdam, Mumbai y Shanghái enfrentarían inundaciones "
         "permanentes que harían inviable la vida de millones de personas.\n\n"
         "Se calcula que más de 680 millones de personas viven en zonas costeras de "
         "baja elevación. El desplazamiento masivo que provocaría esta crisis generaría "
         "conflictos por recursos, colapso de infraestructuras y pérdidas económicas "
         "que superarían los 14 billones de dólares anuales a nivel mundial."},
        {"M1_DER",
         "Energía Eólica\n\n"
         "Los aerogeneradores modernos pueden abastecer de energía completamente limpia "
         "a miles de hogares sin emitir un solo gramo de CO₂ durante su operación. "
         "La energía eólica es hoy una de las fuentes de electricidad más baratas del "
         "mundo, con costos que han caído más del 70% en la última década.\n\n"
         "En 2023, la capacidad eólica global superó los 1,000 gigavatios, suficiente "
         "para abastecer a más de 700 millones de hogares. Países como Dinamarca ya "
         "generan más del 50% de su electricidad con viento, demostrando que una "
         "transición energética completa es técnica y económicamente posible."},
        {"M2_DER",
         "Auto Eléctrico\n\n"
         "El transporte representa casi el 25% de las emisiones globales de CO₂, y "
         "los automóviles de gasolina y diésel son los principales responsables. "
         "Sustituirlos por vehículos eléctricos alimentados con energía renovable "
         "podría eliminar hasta el 80% de esas emisiones, transformando radicalmente "
         "la huella de carbono del sector.\n\n"
         "Las ventas de autos eléctricos crecieron un 35% en 2023 y se espera que "
         "superen a los de combustión interna antes de 2035. Además de reducir "
         "emisiones, los vehículos eléctricos disminuyen la contaminación del aire "
         "en ciudades, salvando cientos de miles de vidas cada año."},
        {"M3_DER",
         "Captura de Carbono\n\n"
         "Las tecnologías de captura directa de aire, conocidas como DAC, extraen "
         "CO₂ directamente de la atmósfera y lo comprimen para almacenarlo de forma "
         "permanente en formaciones geológicas profundas. Aunque todavía en etapa de "
         "escalamiento, estas plantas representan una herramienta crítica para "
         "revertir el daño ya causado al clima.\n\n"
         "Combinadas con la reforestación masiva y la reducción de emisiones, la "
         "captura de carbono podría ayudar a alcanzar la neutralidad de carbono antes "
         "de 2050. Islandia ya opera la planta DAC más grande del mundo, capturando "
         "equivalente a lo que emiten miles de automóviles cada año."},
        {"M5",
         "Acuerdos por el Clima\n\n"
         "El Acuerdo de París, firmado en 2015, une a 196 naciones en un compromiso "
         "histórico: limitar el calentamiento global a 1.5°C por encima de los niveles "
         "preindustriales. Este umbral es crítico: superarlo significaría consecuencias "
         "irreversibles para ecosistemas, comunidades y economías de todo el planeta.\n\n"
         "Cada nación presenta sus propios planes de reducción de emisiones, y cada "
         "cinco años se revisan y fortalecen esos compromisos. La ciencia, la "
         "tecnología y la voluntad política colectiva son los tres pilares sobre los "
         "que descansa la única salida posible: un futuro donde el Ártico vuelva a "
         "congelarse, las ciudades permanezcan en pie y las especies encuentren "
         "un hogar donde vivir."},
    };

    // ── Estado del popup narrativo ────────────────────────────────────────
    bool        popupVisible = false;
    std::string popupTitulo;
    std::string popupTexto;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    while (state != AppState::SALIR) {
        time.update();
        float dt = time.getDeltaTime();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ── Update ───────────────────────────────
        switch (state) {
            case AppState::TITULO:
                if (glfwGetTime() - titleTimer > 2.0 ||
                    glfwGetKey(window.handle, GLFW_KEY_SPACE) == GLFW_PRESS ||
                    glfwGetKey(window.handle, GLFW_KEY_ENTER) == GLFW_PRESS)
                    state = AppState::JUGANDO;
                break;

            case AppState::JUGANDO: {
                float prevX = camera.position.x;
                camera.processInput(input, dt);
                // Colisión museo
                applyCollision(camera.position, prevX);
                // Multi-ángulo QA: forzar pitch para capturas
                if (testCfg.enabled) {
                    for (int i = 0; i < 5; i++) {
                        if (frameCount == CAPTURE_FRAMES[i]) {
                            camera.pitch = CAPTURE_PITCH[i];
                            printf("[QA] frame=%d pitch=%.1f\n", frameCount, CAPTURE_PITCH[i]);
                        }
                        if (frameCount == CAPTURE_FRAMES[i] + 1 && CAPTURE_PITCH[i] != 0.f)
                            camera.pitch = 0.f;
                    }
                }

                museum.update(dt);
                snow.update(dt, (float)glfwGetTime());

                // Detectar módulo cercano
                ModuleInfo* near = museum.getNearModule(camera.position);

                // Activar animación con E (cuando hay módulo cercano)
                if (near && input.isJustPressed(GLFW_KEY_E)) {
                    near->activate();
                    activeModulePtr = near;
                    // Abrir popup narrativo
                    auto it = MODULE_TEXTOS.find(near->id);
                    if (it != MODULE_TEXTOS.end()) {
                        popupTitulo  = near->label;
                        popupTexto   = it->second;
                        popupVisible = true;
                    }
                    // Reproducir narración con handle para poder detenerla
                    if (audioOk) {
                        if (narratorInit) {
                            ma_sound_stop(&narratorSound);
                            ma_sound_uninit(&narratorSound);
                            narratorInit = false;
                        }
                        std::string ruta = std::string(ASSETS_DIR) + "audio/narr_" + near->id + ".mp3";
                        if (ma_sound_init_from_file(&audioEngine, ruta.c_str(), 0,
                                nullptr, nullptr, &narratorSound) == MA_SUCCESS) {
                            ma_sound_start(&narratorSound);
                            narratorInit = true;
                        }
                    }
                }
                // Cerrar popup con Tab y detener narración
                if (popupVisible && input.isJustPressed(GLFW_KEY_TAB)) {
                    popupVisible = false;
                    if (narratorInit) {
                        ma_sound_stop(&narratorSound);
                        ma_sound_uninit(&narratorSound);
                        narratorInit = false;
                    }
                }
                // Test mode: simular E en el frame configurado
                if (testCfg.enabled && frameCount == testCfg.activateAtFrame && near) {
                    near->activate();
                    activeModulePtr = near;
                }

                // Actualizar estado para JSON/ImGui
                if (activeModulePtr) {
                    activeModuleName = activeModulePtr->id;
                    activeModuleT    = activeModulePtr->animT;
                }

                if (glfwGetKey(window.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                    state = AppState::SALIR;

                // Contar módulos completados para activar el cierre narrativo
                {
                    int done = 0;
                    for (const auto& m : museum.modules)
                        if (m.animT >= 1.0f) done++;
                    modulesCompleted = done;
                    // CIERRE desactivado para demo libre — salir solo con ESC
                }
                break;
            }
            case AppState::CIERRE:
                // Pantalla de cierre durante 5 segundos antes de salir
                if (glfwGetTime() - cierreTimer > 5.0 ||
                    glfwGetKey(window.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                    state = AppState::SALIR;
                break;
            case AppState::SALIR:  break;
        }

        // ── ImGui overlays ────────────────────────

        // Pantalla de título
        if (state == AppState::TITULO) {
            float pw = 560.0f, ph = 240.0f;
            ImGui::SetNextWindowPos(
                ImVec2((window.width() - pw) * 0.5f, (window.height() - ph) * 0.45f),
                ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(pw, ph), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.75f);
            ImGui::Begin("Titulo", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoSavedSettings);
            ImGui::SetCursorPosX(pw * 0.08f);
            ImGui::TextColored(ImVec4(0.55f, 0.82f, 1.0f, 1.0f),
                "NUESTRO MUNDO");
            ImGui::Spacing();
            ImGui::SetCursorPosX(pw * 0.05f);
            ImGui::TextColored(ImVec4(0.8f, 0.92f, 1.0f, 1.0f),
                "Polo Norte  -  Calentamiento Global");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::TextWrapped(
                "Recorre un museo virtual en el Artico y descubre las "
                "consecuencias del calentamiento global y las soluciones "
                "que la humanidad puede adoptar.");
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.4f, 1.0f),
                "  [ ESPACIO ]  para iniciar el recorrido");
            ImGui::End();
        }

        // Pantalla de cierre
        if (state == AppState::CIERRE) {
            float pw = 560.0f, ph = 220.0f;
            ImGui::SetNextWindowPos(
                ImVec2((window.width() - pw) * 0.5f, (window.height() - ph) * 0.45f),
                ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(pw, ph), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.80f);
            ImGui::Begin("Cierre", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoSavedSettings);
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.6f, 1.0f),
                "Gracias por tu visita");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::TextWrapped(
                "El calentamiento global es una realidad que afecta a nuestro "
                "planeta hoy. Cada decision importa.");
            ImGui::Spacing();
            ImGui::TextWrapped(
                "Modulos completados: %d / %d",
                modulesCompleted, (int)museum.modules.size());
            ImGui::Spacing();
            float t = (float)(glfwGetTime() - cierreTimer) / 5.0f;
            ImGui::ProgressBar(glm::clamp(t, 0.0f, 1.0f),
                ImVec2(-1.0f, 0.0f), "Cerrando...");
            ImGui::End();
        }

        if (state == AppState::JUGANDO) {
            // ── Panel debug (esquina superior izq) ──────────────────────
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(280, 115), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.50f);
            ImGui::Begin("Debug", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoSavedSettings);
            ImGui::Text("FPS: %.1f  Frame: %d", currentFPS, frameCount);
            ImGui::Text("Cam: (%.1f, %.1f, %.1f)",
                camera.position.x, camera.position.y, camera.position.z);
            ImGui::Text("Yaw: %.1f  Pitch: %.1f", camera.yaw, camera.pitch);
            ImGui::End();

            // ── Panel narrativo (parte inferior central) ─────────────────
            // Muestra info del módulo cercano + barra de progreso si activo
            ModuleInfo* near = museum.getNearModule(camera.position);
            if (near || activeModulePtr) {
                float pw = 480.0f, ph = near ? 90.0f : 70.0f;
                ImGui::SetNextWindowPos(
                    ImVec2((window.width() - pw) * 0.5f, window.height() - ph - 20),
                    ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2(pw, ph), ImGuiCond_Always);
                ImGui::SetNextWindowBgAlpha(0.70f);
                ImGui::Begin("Narrativo", nullptr,
                    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoSavedSettings);

                if (near && !near->animActive) {
                    ImGui::TextColored(ImVec4(0.7f,0.9f,1.0f,1.0f),
                        "[ %s ]", near->label.c_str());
                    ImGui::Text("Presiona  E  para activar la animacion");
                } else if (activeModulePtr) {
                    ImGui::TextColored(ImVec4(0.4f,1.0f,0.6f,1.0f),
                        "[ %s ]  - Animando", activeModulePtr->label.c_str());
                    ImGui::ProgressBar(activeModuleT, ImVec2(-1.0f, 0.0f));
                    if (activeModuleT >= 1.0f)
                        ImGui::TextColored(ImVec4(1.0f,0.9f,0.4f,1.0f),
                            "Animacion completada");
                }
                ImGui::End();
            }

            // ── Popup narrativo ──────────────────────────────────────────
            if (popupVisible) {
                float pw = 680.0f, ph = 380.0f;
                ImGui::SetNextWindowPos(
                    ImVec2((window.width()  - pw) * 0.5f,
                           (window.height() - ph) * 0.35f),
                    ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2(pw, ph), ImGuiCond_Always);
                ImGui::SetNextWindowBgAlpha(0.90f);
                ImGui::Begin("PopupNarrativo", nullptr,
                    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoSavedSettings);

                ImGui::TextColored(ImVec4(0.4f, 0.85f, 1.0f, 1.0f),
                    "%s", popupTitulo.c_str());
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::SetNextItemWidth(-1.0f);
                ImGui::PushTextWrapPos(pw - 20.0f);
                ImGui::TextUnformatted(popupTexto.c_str());
                ImGui::PopTextWrapPos();
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.55f, 0.55f, 0.55f, 1.0f),
                    "[ TAB ] Cerrar");

                ImGui::End();
            }
        }

        // ── Render ───────────────────────────────
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 proj = camera.getProjectionMatrix(window.aspectRatio());

        if (state == AppState::TITULO || state == AppState::CIERRE) {
            glClearColor(0.02f, 0.04f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } else {
            // Fondo — cielo ártico
            glClearColor(0.45f, 0.60f, 0.75f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Shader estándar — uniforms globales
            stdShader.use();
            stdShader.setMat4("view",        view);
            stdShader.setMat4("projection",  proj);
            stdShader.setVec3("viewPos",     camera.position);
            stdShader.setVec3("lightDir",    LIGHT_DIR);
            stdShader.setVec3("lightColor",  LIGHT_COLOR);
            stdShader.setFloat("fogDensity", FOG_DENSITY);
            // Fresnel + shininess (valores ártico: hielo semisólido)
            stdShader.setFloat("shininess",  48.0f);
            stdShader.setVec3("fresnel0",    glm::vec3(0.06f, 0.07f, 0.09f));
            stdShader.setVec3("fogColor",    FOG_COLOR);

            // Render del museo (suelo, techo, paredes, plataformas)
            museum.draw(stdShader, camera.position);

            // Escenas animadas de los módulos interactivos
            float totalTime = (float)glfwGetTime();
            for (const auto& mod : museum.modules) {
                moduleScene.draw(stdShader, mod.id, mod.center, mod.animT, totalTime);
            }

            // Fauna decorativa estática
            moduleScene.drawFauna(stdShader);

            // Letreros 3D de color sobre cada módulo
            museum.drawSigns(stdShader);

            // ── Agua ártica (semitransparente — blend antes del skybox) ──────
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE); // no escribir en depth — transparente

            waterShader.use();
            waterShader.setMat4("view",        view);
            waterShader.setMat4("projection",  proj);
            waterShader.setVec3("viewPos",     camera.position);
            waterShader.setVec3("lightDir",    LIGHT_DIR);
            waterShader.setVec3("lightColor",  LIGHT_COLOR);
            waterShader.setFloat("fogDensity", FOG_DENSITY);
            waterShader.setVec3("fogColor",    FOG_COLOR);
            waterShader.setFloat("waveAmp",    0.18f);
            waterShader.setFloat("waveSpeed",  0.9f);
            waterShader.setFloat("waveFreq",   0.35f);
            waterShader.setFloat("time",       totalTime);
            // Model: centrado en (0, -0.6, 35) — suficientemente bajo para que
            // las olas (max +0.34) nunca superen el suelo (Y=0)
            glm::mat4 waterModel = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(0.0f, -0.6f, 35.0f));
            waterShader.setMat4("model", waterModel);
            waterMesh.draw();

            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);

            // ── Partículas de nieve (billboards, unlit, blend) ──────────
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE); // el quad es de doble cara

            snow.draw(unlitShader, view, proj);

            glEnable(GL_CULL_FACE);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);

            // Skybox — renderizar AL FINAL para aprovechar el early-z
            // (el truco xyww hace que todos los fragmentos del skybox tengan z=1.0)
            skybox.draw(skyboxShader, view, proj);
        }

        // ImGui siempre al final
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Capturas ANTES de swap (glReadPixels del back buffer renderizado)
        for (int i = 0; i < 5; i++) {
            if (testCfg.enabled && frameCount == CAPTURE_FRAMES[i])
                saveScreenshot(CAPTURE_NAMES[i], window.width(), window.height());
        }

        window.swapBuffers();
        window.pollEvents();
        if (testCfg.enabled && frameCount == 600)
            saveStateJSON(frameCount, currentFPS, camera.position, activeModuleName, activeModuleT);

        if (testCfg.enabled && frameCount >= 620) state = AppState::SALIR;
        if (state == AppState::SALIR) window.close();

        fpsFrames++;
        double now = glfwGetTime();
        if (now - fpsTimer >= 1.0) {
            currentFPS = (float)fpsFrames / (float)(now - fpsTimer);
            fpsFrames = 0; fpsTimer = now;
        }

        input.update();
        frameCount++;
    }

    museum.free();
    moduleScene.free();
    skybox.free();
    waterMesh.free();
    snow.free();
    whiteTex.free();
    if (narratorInit) { ma_sound_stop(&narratorSound); ma_sound_uninit(&narratorSound); }
    if (audioOk) ma_engine_uninit(&audioEngine);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    window.terminate();
    return 0;
}
