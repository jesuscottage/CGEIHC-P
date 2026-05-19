#pragma once
// Museum.h — Fase 9: geometría del museo + letreros 3D de color por módulo
// Layout en T invertida:
//   Vestíbulo:      X∈[-10,10],  Z∈[-5,10]
//   Corredor izq:   X∈[-20,-4],  Z∈[5,65]
//   Corredor der:   X∈[4,20],    Z∈[5,65]
//   Sala M5:        X∈[-20,20],  Z∈[60,75]
// Coordenadas: 1 unidad = 1 metro, Y=0 es el suelo, Y-up, forward=-Z

#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <cmath>

#ifndef ASSETS_DIR
#define ASSETS_DIR "assets/"
#endif

// ──────────────────────────────────────────────────────────
// Descripción de un módulo interactivo
// ──────────────────────────────────────────────────────────
struct ModuleInfo {
    std::string id;           // "M1_IZQ", "M2_DER", "M5", etc.
    glm::vec3   center;       // centro de la plataforma (Y=0)
    float       radius;       // radio del trigger de proximidad (5.0 m)
    std::string label;        // texto del letrero
    glm::vec3   labelColor;   // color del letrero 3D

    // Estado de animación
    bool  animActive  = false;
    float animT       = 0.0f; // 0.0 → 1.0 a lo largo de la duración
    float animDuration= 8.0f; // segundos

    bool isNear(const glm::vec3& camPos) const {
        float dx = camPos.x - center.x;
        float dz = camPos.z - center.z;
        return (dx*dx + dz*dz) < (radius * radius);
    }

    // Llamar cada frame cuando la animación está activa
    void update(float dt) {
        if (animActive && animT < 1.0f) {
            animT += dt / animDuration;
            if (animT > 1.0f) animT = 1.0f;
        }
    }

    void activate() { animActive = true; }
    void reset()    { animActive = false; animT = 0.0f; }
};

// ──────────────────────────────────────────────────────────
// Clase Museum: geometría + módulos + render
// ──────────────────────────────────────────────────────────
class Museum {
public:
    // Módulos ordenados: 3 izquierda, 3 derecha, 1 M5
    std::vector<ModuleInfo> modules;

    // Meshes de la estructura
    Mesh floorMesh;    // suelo completo
    Mesh ceilingMesh;  // techo de los corredores
    // Paredes almacenadas con su transform
    struct Wall { Mesh mesh; glm::mat4 transform; glm::vec3 color; };
    std::vector<Wall> walls;

    // Plataformas circulares (una por módulo)
    struct Platform { Mesh mesh; glm::vec3 pos; };
    std::vector<Platform> platforms;

    // Mesh compartido para los letreros 3D
    Mesh signCube;

    // Texturas del museo
    Texture floorTex, wallTex, iceTex;

    // AABB del recorrido (para clampear la cámara)
    glm::vec3 aabbMin{ -20.0f, 0.0f, -5.0f };
    glm::vec3 aabbMax{  20.0f, 0.0f, 75.0f };

    void init() {
        initModules();
        buildFloor();
        buildWalls();
        buildPlatforms();
        signCube = makeCube();
        // Cargar texturas (fallback: si no existe, Texture::load retorna false y se usa baseColor)
        // Paredes claras (Concrete034 claro), suelo más oscuro (Concrete040)
        floorTex.load(ASSETS_DIR "textures/wall.png");   // concreto oscuro para suelo
        wallTex.load(ASSETS_DIR "textures/floor.png");    // concreto claro para paredes
        iceTex.load(ASSETS_DIR "textures/ice.png");
    }

    // Retorna el módulo más cercano a la cámara (o nullptr)
    ModuleInfo* getNearModule(const glm::vec3& camPos) {
        for (auto& m : modules)
            if (m.isNear(camPos)) return &m;
        return nullptr;
    }

    void update(float dt) {
        for (auto& m : modules) m.update(dt);
    }

    // Render: suelo, techo, paredes, plataformas
    void draw(Shader& shader, const glm::vec3& viewPos) {
        // Suelo y techo: desactivar culling para que se vean desde ambos lados
        // (evita problemas de winding order en planos horizontales grandes)
        glDisable(GL_CULL_FACE);

        // Suelo — textura de concreto si disponible
        {
            shader.setMat4("model", glm::mat4(1.0f));
            if (floorTex.id) {
                shader.setBool("useTexture", true);
                floorTex.bind(0);
                shader.setInt("diffuseMap", 0);
            } else {
                shader.setBool("useTexture", false);
                shader.setVec3("baseColor", glm::vec3(0.82f, 0.90f, 0.96f));
            }
            floorMesh.draw();
        }

        // Techo — gris ártico
        {
            glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, CEILING_H, 0.0f));
            shader.setMat4("model", m);
            shader.setBool("useTexture", false);
            shader.setVec3("baseColor", glm::vec3(0.55f, 0.62f, 0.70f));
            ceilingMesh.draw();
        }

        // Paredes — culling desactivado (cámara interior ve cara interna)
        glDisable(GL_CULL_FACE);

        // Paredes — textura de concreto claro si disponible
        for (const auto& w : walls) {
            shader.setMat4("model", w.transform);
            if (wallTex.id) {
                shader.setBool("useTexture", true);
                wallTex.bind(0);
                shader.setInt("diffuseMap", 0);
            } else {
                shader.setBool("useTexture", false);
                shader.setVec3("baseColor", w.color);
            }
            w.mesh.draw();
        }
        glEnable(GL_CULL_FACE);

        // Plataformas — textura de hielo si disponible
        for (const auto& p : platforms) {
            glm::mat4 m = glm::translate(glm::mat4(1.0f), p.pos);
            shader.setMat4("model", m);
            if (iceTex.id) {
                shader.setBool("useTexture", true);
                iceTex.bind(0);
                shader.setInt("diffuseMap", 0);
            } else {
                shader.setBool("useTexture", false);
                shader.setVec3("baseColor", glm::vec3(0.60f, 0.80f, 0.95f));
            }
            p.mesh.draw();
        }
    }

    // Letreros 3D de color — pequeños paneles sobre cada módulo
    // Dos cubos en cruz para visibilidad desde cualquier ángulo
    void drawSigns(Shader& shader) {
        for (const auto& mod : modules) {
            float signW = (mod.id == "M5") ? 1.0f : 0.8f; // ancho del panel
            float signH = 0.35f;   // alto del panel
            float signD = 0.15f;   // grosor (antes 0.06 — ahora visible desde ángulos oblicuos)
            float signY = 4.2f;    // altura — debajo del techo (6m), discreto

            shader.setBool("useTexture", false);
            shader.setVec3("baseColor", mod.labelColor);

            // Panel 1: orientado en Z
            auto m1 = glm::scale(glm::translate(glm::mat4(1.f),
                {mod.center.x, signY, mod.center.z}),
                {signD, signH, signW});
            shader.setMat4("model", m1);
            signCube.draw();

            // Panel 2: orientado en X (cruz)
            auto m2 = glm::scale(glm::translate(glm::mat4(1.f),
                {mod.center.x, signY, mod.center.z}),
                {signW, signH, signD});
            shader.setMat4("model", m2);
            signCube.draw();
        }
    }

    void free() {
        floorMesh.free();
        ceilingMesh.free();
        signCube.free();
        for (auto& w : walls)    w.mesh.free();
        for (auto& p : platforms) p.mesh.free();
    }

private:
    static constexpr float CEILING_H  = 6.0f;  // altura del techo (metros)
    static constexpr float WALL_THICK = 0.5f;  // grosor de paredes
    static constexpr float MODULE_R   = 5.0f;  // radio de plataforma/trigger

    void initModules() {
        modules = {
            // 3 izquierda — consecuencias
            {"M1_IZQ", {-12.0f,0,13.0f}, MODULE_R, "Deshielo del Ártico",  {0.4f,0.7f,1.0f}, false,0.0f,10.0f},
            {"M2_IZQ", {-12.0f,0,28.0f}, MODULE_R, "Fauna en Peligro",     {0.3f,0.8f,0.6f}, false,0.0f, 8.0f},
            {"M3_IZQ", {-12.0f,0,43.0f}, MODULE_R, "Ciudades Inundadas",   {0.5f,0.6f,0.9f}, false,0.0f,12.0f},
            // 3 derecha — soluciones
            {"M1_DER", { 12.0f,0,13.0f}, MODULE_R, "Energía Eólica",       {1.0f,0.9f,0.3f}, false,0.0f, 5.0f},
            {"M2_DER", { 12.0f,0,28.0f}, MODULE_R, "Auto Eléctrico",       {0.3f,0.9f,0.4f}, false,0.0f, 6.0f},
            {"M3_DER", { 12.0f,0,43.0f}, MODULE_R, "Captura de Carbono",   {0.7f,0.9f,0.5f}, false,0.0f, 8.0f},
            // M5 — compartido
            {"M5",     {  0.0f,0,63.0f}, 7.0f,     "Acuerdos por el Clima",{1.0f,0.8f,0.3f}, false,0.0f,12.0f},
        };
    }

    void buildFloor() {
        // Suelo grande: cubre toda la T (vestíbulo + corredores + sala M5)
        // 80×80 m con centro en (0, 0, 35) → de Z=-5 a Z=75, X=-40 a X=40
        std::vector<Vertex> verts = {
            {{-40,0,-5}, {0,1,0}, {0,  0 }},
            {{ 40,0,-5}, {0,1,0}, {8,  0 }},
            {{ 40,0,75}, {0,1,0}, {8, 16 }},
            {{-40,0,75}, {0,1,0}, {0, 16 }},
        };
        // CCW visto desde arriba (+Y) para que sea front-face con GL_CCW default
        std::vector<unsigned int> idx = {0,2,1, 0,3,2};
        floorMesh.setup(verts, idx);

        // Techo: visto desde abajo (interior del museo)
        // Normal apunta hacia -Y. Para que la cara sea front-face vista desde abajo,
        // el winding debe ser CW desde arriba = CCW desde abajo.
        std::vector<Vertex> cVerts = {
            {{-40,0,-5}, {0,-1,0}, {0,  0 }},
            {{-40,0,75}, {0,-1,0}, {0, 16 }},
            {{ 40,0,75}, {0,-1,0}, {8, 16 }},
            {{ 40,0,-5}, {0,-1,0}, {8,  0 }},
        };
        // Probar winding opuesto para visibilidad desde abajo
        std::vector<unsigned int> cIdx = {0,1,2, 0,2,3};
        ceilingMesh.setup(cVerts, cIdx);
    }

    void buildWalls() {
        walls.clear();
        // Paredes del museo: planos XZ orientados hacia el interior
        // Se usa makeCube escalado (altura 6m, grosor 0.5m)

        auto addWall = [&](glm::vec3 pos, glm::vec3 scale, glm::vec3 color) {
            Wall w;
            w.mesh = makeCube();
            w.transform = glm::scale(glm::translate(glm::mat4(1.0f), pos), scale);
            w.color = color;
            walls.push_back(std::move(w));
        };

        glm::vec3 wallColor{0.72f, 0.76f, 0.80f}; // gris claro ártico (museo iluminado)

        // ── Vestíbulo ──────────────────────────────────────
        // Pared frontal (Z=-5, cara de entrada)
        addWall({0, 3, -5.25f}, {20, 6, 0.5f}, wallColor);
        // Paredes laterales del vestíbulo (X=±10)
        addWall({-10.25f, 3, 2.5f}, {0.5f, 6, 15}, wallColor);
        addWall({ 10.25f, 3, 2.5f}, {0.5f, 6, 15}, wallColor);

        // ── Corredor izquierdo ─────────────────────────────
        // Pared exterior izq (X=-20)
        addWall({-20.25f, 3, 35}, {0.5f, 6, 70}, wallColor);
        // Pared interior izq (X=-4)  — empieza en Z=10 (fin del vestíbulo)
        addWall({-4.25f, 3, 40}, {0.5f, 6, 60}, {wallColor.r*0.85f, wallColor.g*0.85f, wallColor.b*0.85f});
        // Pared trasera corredor izq (Z=65 lado izq)
        addWall({-12, 3, 65.25f}, {16, 6, 0.5f}, wallColor);

        // ── Corredor derecho ───────────────────────────────
        addWall({ 20.25f, 3, 35}, {0.5f, 6, 70}, wallColor);
        addWall({  4.25f, 3, 40}, {0.5f, 6, 60}, {wallColor.r*0.85f, wallColor.g*0.85f, wallColor.b*0.85f});
        addWall({ 12, 3, 65.25f}, {16, 6, 0.5f}, wallColor);

        // ── Sala M5 ────────────────────────────────────────
        // Pared trasera (Z=75)
        addWall({0, 3, 75.25f}, {40, 6, 0.5f}, wallColor);
    }

    void buildPlatforms() {
        platforms.clear();
        for (const auto& mod : modules) {
            Platform p;
            float radius = (mod.id == "M5") ? 7.0f : 5.0f;
            p.mesh = makeDisc(radius, 32, 0.08f);
            p.pos  = mod.center; // Y=0 → el disco queda en la superficie
            platforms.push_back(std::move(p));
        }
    }
};
