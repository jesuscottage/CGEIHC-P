// main.cpp — Fase 5: Animaciones LERP en módulos interactivos

#include "glad/glad.h"
#include "core/Window.h"
#include "core/Input.h"
#include "core/Time.h"
#include "graphics/CameraFPS.h"
#include "graphics/Shader.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/Model.h"
#include "scene/Museum.h"
#include "scene/ModuleScene.h"

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
    {"M1_IZQ", {-12.0f, 1.7f,  9.5f}},
    {"M2_IZQ", {-12.0f, 1.7f, 24.5f}},
    {"M3_IZQ", {-12.0f, 1.7f, 39.5f}},
    {"M1_DER", { 12.0f, 1.7f,  9.5f}},
    {"M2_DER", { 12.0f, 1.7f, 24.5f}},
    {"M3_DER", { 12.0f, 1.7f, 39.5f}},
    {"M5",     {  0.0f, 1.7f, 59.5f}},
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

    // ── ImGui ─────────────────────────────────
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.handle, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // ── Shaders ───────────────────────────────
    Shader stdShader, unlitShader;
    if (!stdShader.load(SHADERS_DIR "standard.vert", SHADERS_DIR "standard.frag")) return -1;
    if (!unlitShader.load(SHADERS_DIR "unlit.vert",  SHADERS_DIR "unlit.frag"))    return -1;

    // ── Museo ─────────────────────────────────
    Museum museum;
    museum.init();

    // ── Escenas animadas de módulos ────────────
    ModuleScene moduleScene;
    moduleScene.init();

    // ── Texturas placeholder ───────────────────
    Texture whiteTex;
    whiteTex.loadWhite();

    // ── Uniforms globales ──────────────────────
    const glm::vec3 LIGHT_DIR   = glm::normalize(glm::vec3(-0.3f, -0.8f,  0.2f));
    const glm::vec3 LIGHT_COLOR = glm::vec3(1.0f,  0.95f, 0.85f);
    const glm::vec3 FOG_COLOR   = glm::vec3(0.65f, 0.75f, 0.85f);
    const float     FOG_DENSITY = 0.012f;

    // En test mode se salta la pantalla de título para que las activaciones
    // de módulos por frame ocurran desde el frame 0 en estado JUGANDO
    AppState state     = testCfg.enabled ? AppState::JUGANDO : AppState::TITULO;
    double   titleTimer = glfwGetTime();

    int    frameCount = 0;
    double fpsTimer   = glfwGetTime();
    int    fpsFrames  = 0;
    float  currentFPS = 60.0f;

    const int   CAPTURE_FRAMES[] = {60, 300, 600};
    const char* CAPTURE_NAMES[]  = {
        "screenshot_f060.png", "screenshot_f300.png", "screenshot_f600.png"
    };

    std::string activeModuleName = "ninguno";
    float       activeModuleT    = 0.0f;

    // Módulo actualmente activado (nullptr si ninguno)
    ModuleInfo* activeModulePtr  = nullptr;

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
                camera.processInput(input, dt);
                // AABB: límites del museo
                camera.clampPosition(-19.5f, 19.5f, -4.5f, 74.5f);

                museum.update(dt);

                // Detectar módulo cercano
                ModuleInfo* near = museum.getNearModule(camera.position);

                // Activar animación con E (cuando hay módulo cercano)
                if (near && input.isJustPressed(GLFW_KEY_E)) {
                    near->activate();
                    activeModulePtr = near;
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
                break;
            }
            case AppState::CIERRE: state = AppState::SALIR; break;
            case AppState::SALIR:  break;
        }

        // ── ImGui debug ───────────────────────────
        if (state == AppState::JUGANDO) {
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(280, 130), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.55f);
            ImGui::Begin("Debug", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoSavedSettings);
            ImGui::Text("FPS: %.1f  Frame: %d", currentFPS, frameCount);
            ImGui::Text("Cam: (%.1f, %.1f, %.1f)",
                camera.position.x, camera.position.y, camera.position.z);
            ImGui::Text("Yaw: %.1f  Pitch: %.1f", camera.yaw, camera.pitch);
            ModuleInfo* near = museum.getNearModule(camera.position);
            if (near) ImGui::Text("Cerca: %s  [E para activar]", near->id.c_str());
            else      ImGui::Text("Sin modulo cercano");
            ImGui::Text("Activo: %s  t=%.2f", activeModuleName.c_str(), activeModuleT);
            ImGui::End();
        }

        // ── Render ───────────────────────────────
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 proj = camera.getProjectionMatrix(window.aspectRatio());

        if (state == AppState::TITULO) {
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
            stdShader.setVec3("fogColor",    FOG_COLOR);

            // Render del museo (suelo, techo, paredes, plataformas)
            museum.draw(stdShader, camera.position);

            // Escenas animadas de los módulos interactivos
            float totalTime = (float)glfwGetTime();
            for (const auto& mod : museum.modules) {
                moduleScene.draw(stdShader, mod.id, mod.center, mod.animT, totalTime);
            }
        }

        // ImGui siempre al final
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swapBuffers();
        window.pollEvents();

        // Capturas
        for (int i = 0; i < 3; i++)
            if (frameCount == CAPTURE_FRAMES[i])
                saveScreenshot(CAPTURE_NAMES[i], window.width(), window.height());
        if (frameCount == 600)
            saveStateJSON(frameCount, currentFPS, camera.position, activeModuleName, activeModuleT);

        if (frameCount >= 600) state = AppState::SALIR;
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
    whiteTex.free();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    window.terminate();
    return 0;
}
