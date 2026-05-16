#pragma once
// Skybox.h — Fase 7A: cubemap ártico procedural + render con depth trick

#include "glad/glad.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <cstdint>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// Vértices de un cubo unitario centrado en el origen (36 verts, sin índices)
// ─────────────────────────────────────────────────────────────────────────────
static const float SKYBOX_VERTS[] = {
    // cara -X (izquierda)
    -1.f,  1.f, -1.f,   -1.f, -1.f, -1.f,   -1.f, -1.f,  1.f,
    -1.f, -1.f,  1.f,   -1.f,  1.f,  1.f,   -1.f,  1.f, -1.f,
    // cara +X (derecha)
     1.f, -1.f, -1.f,    1.f, -1.f,  1.f,    1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,    1.f,  1.f, -1.f,    1.f, -1.f, -1.f,
    // cara -Y (abajo)
    -1.f, -1.f,  1.f,   -1.f, -1.f, -1.f,    1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,    1.f, -1.f,  1.f,   -1.f, -1.f,  1.f,
    // cara +Y (arriba)
    -1.f,  1.f, -1.f,   -1.f,  1.f,  1.f,    1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,    1.f,  1.f, -1.f,   -1.f,  1.f, -1.f,
    // cara -Z (frente)
    -1.f, -1.f, -1.f,    1.f, -1.f, -1.f,    1.f,  1.f, -1.f,
     1.f,  1.f, -1.f,   -1.f,  1.f, -1.f,   -1.f, -1.f, -1.f,
    // cara +Z (atrás)
    -1.f, -1.f,  1.f,   -1.f,  1.f,  1.f,    1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,    1.f, -1.f,  1.f,   -1.f, -1.f,  1.f,
};

// ─────────────────────────────────────────────────────────────────────────────
// Clase Skybox
// ─────────────────────────────────────────────────────────────────────────────
class Skybox {
public:
    GLuint vao      = 0;
    GLuint vbo      = 0;
    GLuint cubemap  = 0;

    // ── Generación del cubemap ártico procedural ──────────────────────────
    // Cada cara (64×64 px, GL_RGB, uint8) se llena con un gradiente:
    //   - Cara +Y (techo): zenith azul-oscuro → horizonte
    //   - Cara -Y (suelo): blanco-gris del hielo
    //   - Caras laterales: gradiente vertical horizonte → zenith
    // Los colores son un cielo ártico despejado.
    // ─────────────────────────────────────────────────────────────────────
    void init() {
        // VAO / VBO
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOX_VERTS), SKYBOX_VERTS, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);

        // Cubemap
        glGenTextures(1, &cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

        const int SZ = 64;
        std::vector<uint8_t> face((size_t)(SZ * SZ * 3));

        // GLenum targets para las 6 caras del cubemap
        const GLenum targets[6] = {
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,   // +X derecha
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,   // -X izquierda
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,   // +Y arriba
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,   // -Y abajo
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,   // +Z frente
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,   // -Z atrás
        };

        // Colores clave del cielo ártico
        // Zenith: azul profundo ártico
        const float zenithR = 0.10f, zenithG = 0.22f, zenithB = 0.55f;
        // Horizonte: azul-claro ártico / blanco-azulado
        const float horizR  = 0.62f, horizG  = 0.78f, horizB  = 0.92f;
        // Suelo (hielo): blanco-grisáceo con leve tono azul
        const float groundR = 0.82f, groundG = 0.88f, groundB = 0.92f;

        for (int f = 0; f < 6; f++) {
            for (int row = 0; row < SZ; row++) {
                for (int col = 0; col < SZ; col++) {
                    float r, g, b;

                    if (f == 2) {
                        // +Y cara superior — zenith puro (azul profundo)
                        // gradiente radial desde centro (zenith) hacia borde (horizonte)
                        float dx = (col - SZ * 0.5f) / (SZ * 0.5f);
                        float dy = (row - SZ * 0.5f) / (SZ * 0.5f);
                        float dist = std::min(1.0f, std::sqrt(dx*dx + dy*dy));
                        r = zenithR + (horizR - zenithR) * dist;
                        g = zenithG + (horizG - zenithG) * dist;
                        b = zenithB + (horizB - zenithB) * dist;
                    }
                    else if (f == 3) {
                        // -Y cara inferior — hielo/suelo ártico
                        // leve variación de ruido simulado con patrón chequereo suave
                        float nx = (float)col / SZ;
                        float ny = (float)row / SZ;
                        float noise = 0.04f * std::sin(nx * 17.3f) * std::cos(ny * 13.7f);
                        r = groundR + noise;
                        g = groundG + noise * 0.8f;
                        b = groundB + noise * 0.5f;
                    }
                    else {
                        // Caras laterales — gradiente vertical zenith→horizonte
                        // row=0 es la fila superior (+Y), row=SZ-1 es inferior (-Y)
                        float t = (float)row / (SZ - 1); // 0=arriba, 1=abajo

                        // parte superior: zenith
                        // parte media: horizonte puro en t=0.55
                        // parte inferior: horizonte ligeramente más cálido
                        if (t < 0.55f) {
                            float tt = t / 0.55f;
                            // curva de suavizado (easeInOut cuadrático)
                            tt = tt * tt * (3.0f - 2.0f * tt);
                            r = zenithR + (horizR - zenithR) * tt;
                            g = zenithG + (horizG - zenithG) * tt;
                            b = zenithB + (horizB - zenithB) * tt;
                        } else {
                            // horizonte hasta suelo
                            float tt = (t - 0.55f) / 0.45f;
                            tt = tt * tt * (3.0f - 2.0f * tt);
                            r = horizR + (groundR - horizR) * tt * 0.3f;
                            g = horizG + (groundG - horizG) * tt * 0.3f;
                            b = horizB + (groundB - horizB) * tt * 0.3f;
                        }

                        // Añadir aurora boreal tenue en las caras laterales
                        // Aparece en la parte superior (t < 0.35) como banda verde-turquesa
                        if (t > 0.05f && t < 0.35f) {
                            float auroraT = (t - 0.05f) / 0.30f;
                            // envolvente de campana centrada en t=0.18
                            float dist = std::abs(auroraT - 0.43f);
                            float envelope = std::max(0.0f, 1.0f - dist * 3.5f);
                            // Variación horizontal con patrón sinusoidal
                            float hx = (float)col / SZ;
                            float aurStr = envelope * 0.08f *
                                (0.5f + 0.5f * std::sin(hx * 9.4f + (float)f * 1.7f));
                            r += aurStr * 0.05f;
                            g += aurStr * 1.0f;
                            b += aurStr * 0.55f;
                        }
                    }

                    // Clamp y convertir a uint8
                    r = std::min(1.0f, std::max(0.0f, r));
                    g = std::min(1.0f, std::max(0.0f, g));
                    b = std::min(1.0f, std::max(0.0f, b));

                    int idx = (row * SZ + col) * 3;
                    face[idx + 0] = (uint8_t)(r * 255.0f);
                    face[idx + 1] = (uint8_t)(g * 255.0f);
                    face[idx + 2] = (uint8_t)(b * 255.0f);
                }
            }
            glTexImage2D(targets[f], 0, GL_RGB8, SZ, SZ, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, face.data());
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    // ── Render del skybox ─────────────────────────────────────────────────
    // Llamar DESPUÉS del museo. El truco xyww del vertex shader garantiza
    // z_NDC = 1.0, por lo que el skybox queda detrás de toda la geometría.
    void draw(Shader& sh, const glm::mat4& view, const glm::mat4& proj) {
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        sh.use();
        // Quitar la traslación de la cámara — solo rotación
        glm::mat4 viewNoTrans = glm::mat4(glm::mat3(view));
        sh.setMat4("view",       viewNoTrans);
        sh.setMat4("projection", proj);
        sh.setInt("skybox",      0);

        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    void free() {
        if (vao)     { glDeleteVertexArrays(1, &vao);    vao     = 0; }
        if (vbo)     { glDeleteBuffers(1, &vbo);         vbo     = 0; }
        if (cubemap) { glDeleteTextures(1, &cubemap);    cubemap = 0; }
    }

};
