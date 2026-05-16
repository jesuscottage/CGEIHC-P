#pragma once
// SnowSystem.h — Fase 8: partículas de nieve como billboards procedurales
// Usa el unlit shader con baseColor blanco y quads orientados a la cámara.

#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdlib>
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────
// Struct de una partícula de nieve
// ─────────────────────────────────────────────────────────────────────────────
struct Snowflake {
    glm::vec3 pos;
    float speed;     // velocidad de caída (m/s), entre 0.25 y 0.60
    float drift;     // drift horizontal (rad/s) para movimiento sinusoidal
    float phase;     // desfase de la onda sinusoidal (rad)
    float size;      // tamaño del copo (0.03 – 0.08 m)
};

// ─────────────────────────────────────────────────────────────────────────────
// SnowSystem — gestiona 100 copos de nieve que caen dentro del museo
// ─────────────────────────────────────────────────────────────────────────────
class SnowSystem {
public:
    static constexpr int COUNT = 100;

    void init() {
        // Quad unitario centrado en el origen en el plano XY (para billboard)
        std::vector<Vertex> verts = {
            {{-0.5f,-0.5f,0}, {0,0,1}, {0,0}},
            {{ 0.5f,-0.5f,0}, {0,0,1}, {1,0}},
            {{ 0.5f, 0.5f,0}, {0,0,1}, {1,1}},
            {{-0.5f, 0.5f,0}, {0,0,1}, {0,1}},
        };
        std::vector<unsigned int> idx = {0,1,2, 0,2,3};
        mQuad.setup(verts, idx);

        // Semilla determinista para reproducibilidad
        srand(42);
        spawn(particles);
    }

    void update(float dt, float totalTime) {
        for (auto& p : particles) {
            // Caída vertical
            p.pos.y -= p.speed * dt;
            // Deriva sinusoidal horizontal
            p.pos.x += 0.06f * std::sin(totalTime * p.drift + p.phase) * dt;
            p.pos.z += 0.04f * std::cos(totalTime * p.drift + p.phase + 1.2f) * dt;

            // Reiniciar cuando toca el suelo
            if (p.pos.y < -0.05f) {
                p.pos.y = 5.8f + (rand() % 100) * 0.01f;
                p.pos.x = randomX();
                p.pos.z = randomZ();
            }
        }
    }

    // Dibuja los billboards orientados a la cámara usando el unlit shader.
    void draw(Shader& sh, const glm::mat4& view, const glm::mat4& proj) const {
        // Vectores right y up de la cámara (columnas de la inversa del view)
        glm::vec3 camRight = {view[0][0], view[1][0], view[2][0]};
        glm::vec3 camUp    = {view[0][1], view[1][1], view[2][1]};

        sh.use();
        sh.setMat4("view",       view);
        sh.setMat4("projection", proj);
        sh.setBool("useTexture", false);
        sh.setVec3("baseColor",  glm::vec3(0.95f, 0.97f, 1.0f));
        sh.setFloat("alpha",     0.82f);

        for (const auto& p : particles) {
            // Construir matriz de billboard: right y up de la cámara
            glm::mat4 m(1.0f);
            m[0] = glm::vec4(camRight * p.size, 0.0f);
            m[1] = glm::vec4(camUp    * p.size, 0.0f);
            m[2] = glm::vec4(glm::cross(camRight, camUp), 0.0f);
            m[3] = glm::vec4(p.pos, 1.0f);
            sh.setMat4("model", m);
            mQuad.draw();
        }
    }

    void free() { mQuad.free(); }

private:
    Mesh mQuad;
    std::vector<Snowflake> particles;

    // Rango de spawn: dentro de los corredores del museo
    static float randomX() {
        // Dentro de las paredes del museo (X ∈ [-19, 19])
        return -19.0f + (float)(rand() % 380) * 0.1f;
    }
    static float randomZ() {
        // A lo largo del recorrido (Z ∈ [0, 75])
        return (float)(rand() % 750) * 0.1f;
    }

    static void spawn(std::vector<Snowflake>& out) {
        out.resize(COUNT);
        for (auto& p : out) {
            p.pos.x = randomX();
            p.pos.y = (float)(rand() % 60) * 0.1f; // Y ∈ [0, 6) — distribuidos verticalmente
            p.pos.z = randomZ();
            p.speed = 0.25f + (float)(rand() % 35) * 0.01f;
            p.drift = 0.4f  + (float)(rand() % 16) * 0.1f;
            p.phase = (float)(rand() % 628) * 0.01f;
            p.size  = 0.03f + (float)(rand() % 5)  * 0.01f;
        }
    }
};
