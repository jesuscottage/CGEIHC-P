#pragma once
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    unsigned int indexCount  = 0;
    unsigned int vertexCount = 0;

    // Inicializar desde vértices e índices
    void setup(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
        indexCount  = (unsigned int)indices.size();
        vertexCount = (unsigned int)vertices.size();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                     vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                     indices.data(), GL_STATIC_DRAW);

        // layout 0: position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, position));
        // layout 1: normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, normal));
        // layout 2: texCoords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, texCoords));

        glBindVertexArray(0);
    }

    void draw() const {
        glBindVertexArray(VAO);
        if (indexCount > 0)
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);
    }

    void free() {
        if (VAO) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
        if (VBO) { glDeleteBuffers(1, &VBO); VBO = 0; }
        if (EBO) { glDeleteBuffers(1, &EBO); EBO = 0; }
    }
};

// ─────────────────────────────────────────────
// Generadores de geometría primitiva
// ─────────────────────────────────────────────

// Plano horizontal (XZ) centrado en el origen, tamaño 2*halfSize x 2*halfSize
// UV va de 0 a uvRepeat (para tiling de textura)
inline Mesh makePlane(float halfSize = 1.0f, float uvRepeat = 1.0f) {
    float h = halfSize;
    std::vector<Vertex> verts = {
        {{-h, 0, -h}, {0,1,0}, {0,         0        }},
        {{ h, 0, -h}, {0,1,0}, {uvRepeat,  0        }},
        {{ h, 0,  h}, {0,1,0}, {uvRepeat,  uvRepeat }},
        {{-h, 0,  h}, {0,1,0}, {0,         uvRepeat }},
    };
    std::vector<unsigned int> idx = {0,1,2, 0,2,3};
    Mesh m; m.setup(verts, idx); return m;
}

// Disco horizontal (plataforma circular) — N segmentos, centrado en origen
// El borde inferior está en Y=0; height es el grosor hacia arriba
inline Mesh makeDisc(float radius = 1.0f, int segments = 24, float height = 0.1f) {
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    // Centro superior
    verts.push_back({{0,height,0},{0,1,0},{0.5f,0.5f}});
    // Centro inferior
    verts.push_back({{0,0,0},{0,-1,0},{0.5f,0.5f}});

    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * glm::two_pi<float>();
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        // Cara superior
        verts.push_back({{x,height,z},{0,1,0},{0.5f+cos(angle)*0.5f, 0.5f+sin(angle)*0.5f}});
        // Cara inferior
        verts.push_back({{x,0,z},{0,-1,0},{0.5f+cos(angle)*0.5f, 0.5f+sin(angle)*0.5f}});
        // Cara lateral
        verts.push_back({{x,height,z},{x/radius,0,z/radius},{(float)i/segments,1}});
        verts.push_back({{x,0,z},{x/radius,0,z/radius},{(float)i/segments,0}});
    }

    // Índices cara superior (fan)
    for (int i = 0; i < segments; i++) {
        unsigned int b = 2 + i * 4;
        idx.insert(idx.end(), {0, b+4, b});
    }
    // Índices cara inferior (fan — invertido)
    for (int i = 0; i < segments; i++) {
        unsigned int b = 2 + i * 4;
        idx.insert(idx.end(), {1, b+1, b+5});
    }
    // Índices lados
    for (int i = 0; i < segments; i++) {
        unsigned int b = 2 + i * 4;
        unsigned int s = b + 2; // lateral actual
        idx.insert(idx.end(), {s, s+1, s+5, s, s+5, s+4});
    }

    Mesh m; m.setup(verts, idx); return m;
}

// Cubo unitario centrado en el origen (1x1x1)
inline Mesh makeCube() {
    std::vector<Vertex> verts = {
        // Cara -Z (back)
        {{-0.5f,-0.5f,-0.5f},{0,0,-1},{0,0}}, {{ 0.5f,-0.5f,-0.5f},{0,0,-1},{1,0}},
        {{ 0.5f, 0.5f,-0.5f},{0,0,-1},{1,1}}, {{-0.5f, 0.5f,-0.5f},{0,0,-1},{0,1}},
        // Cara +Z (front)
        {{-0.5f,-0.5f, 0.5f},{0,0,1},{0,0}}, {{ 0.5f,-0.5f, 0.5f},{0,0,1},{1,0}},
        {{ 0.5f, 0.5f, 0.5f},{0,0,1},{1,1}}, {{-0.5f, 0.5f, 0.5f},{0,0,1},{0,1}},
        // Cara -X (left)
        {{-0.5f, 0.5f, 0.5f},{-1,0,0},{1,0}}, {{-0.5f, 0.5f,-0.5f},{-1,0,0},{1,1}},
        {{-0.5f,-0.5f,-0.5f},{-1,0,0},{0,1}}, {{-0.5f,-0.5f, 0.5f},{-1,0,0},{0,0}},
        // Cara +X (right)
        {{ 0.5f, 0.5f, 0.5f},{1,0,0},{1,0}}, {{ 0.5f, 0.5f,-0.5f},{1,0,0},{1,1}},
        {{ 0.5f,-0.5f,-0.5f},{1,0,0},{0,1}}, {{ 0.5f,-0.5f, 0.5f},{1,0,0},{0,0}},
        // Cara -Y (bottom)
        {{-0.5f,-0.5f,-0.5f},{0,-1,0},{0,1}}, {{ 0.5f,-0.5f,-0.5f},{0,-1,0},{1,1}},
        {{ 0.5f,-0.5f, 0.5f},{0,-1,0},{1,0}}, {{-0.5f,-0.5f, 0.5f},{0,-1,0},{0,0}},
        // Cara +Y (top)
        {{-0.5f, 0.5f,-0.5f},{0,1,0},{0,1}}, {{ 0.5f, 0.5f,-0.5f},{0,1,0},{1,1}},
        {{ 0.5f, 0.5f, 0.5f},{0,1,0},{1,0}}, {{-0.5f, 0.5f, 0.5f},{0,1,0},{0,0}},
    };
    std::vector<unsigned int> idx;
    for (int f = 0; f < 6; f++) {
        unsigned int b = f * 4;
        idx.insert(idx.end(), {b,b+1,b+2, b,b+2,b+3});
    }
    Mesh m; m.setup(verts, idx); return m;
}
