#pragma once
#include "glad/glad.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <cstdio>

#ifndef ASSETS_DIR
#define ASSETS_DIR "assets/"
#endif

// Quad vertical en el plano YZ, normal apuntando +X, centrado en el origen.
// mirrorU=true invierte el eje U para que la imagen no aparezca espejada
// en paredes donde el observador mira en dirección -X.
inline Mesh makeWallQuad(float w, float h, bool mirrorU = false) {
    float hw = w * 0.5f, hh = h * 0.5f;
    float u0 = mirrorU ? 1.f : 0.f;
    float u1 = mirrorU ? 0.f : 1.f;
    std::vector<Vertex> verts = {
        {{ 0.f, -hh, -hw }, { 1, 0, 0 }, { u0, 0.f }},
        {{ 0.f, -hh,  hw }, { 1, 0, 0 }, { u1, 0.f }},
        {{ 0.f,  hh,  hw }, { 1, 0, 0 }, { u1, 1.f }},
        {{ 0.f,  hh, -hw }, { 1, 0, 0 }, { u0, 1.f }},
    };
    std::vector<unsigned int> idx = { 0, 1, 2, 0, 2, 3 };
    Mesh m; m.setup(verts, idx); return m;
}

// ─────────────────────────────────────────────────────────────────────────────
// GalleryScene — pinturas enmarcadas en los corredores del museo
//
// Corredor izquierdo (X ∈ [-20, -4.25]):
//   - Pinturas 01-06 (consecuencias del calentamiento global)
//   - Ambas paredes muestran las mismas pinturas
//
// Corredor derecho (X ∈ [4.25, 20]):
//   - Pinturas 07-12 (soluciones y esperanza)
//   - Ambas paredes muestran las mismas pinturas
// ─────────────────────────────────────────────────────────────────────────────
class GalleryScene {
public:
    static constexpr int N    = 12;    // número total de pinturas únicas
    static constexpr float CW = 1.80f; // ancho del lienzo (m)
    static constexpr float CH = 1.35f; // alto del lienzo (m)
    static constexpr float FB = 0.08f; // grosor del marco (m)
    static constexpr float CY = 2.80f; // centro vertical de las pinturas (m)

    Texture mTex[N];
    Mesh    mCanvas;        // lienzo UV normal (corredor izquierdo)
    Mesh    mCanvasMirr;    // lienzo UV espejado (corredor derecho)
    Mesh    mFrameH;        // tira horizontal del marco (arriba/abajo)
    Mesh    mFrameV;        // tira vertical del marco (izquierda/derecha)

    void init() {
        mCanvas     = makeWallQuad(CW,          CH,          false);
        mCanvasMirr = makeWallQuad(CW,          CH,          true);
        mFrameH     = makeWallQuad(CW + 2*FB,   FB,          false);
        mFrameV     = makeWallQuad(FB,           CH,          false);

        for (int i = 0; i < N; i++) {
            char path[256];
            std::snprintf(path, sizeof(path),
                          ASSETS_DIR "textures/gallery/pintura_%02d.png", i + 1);
            mTex[i].load(path, false);
        }
    }

    void draw(Shader& sh) {
        // Posiciones Z de las pinturas a lo largo de los corredores (10-60 m)
        const float ZS[] = { 13.f, 21.f, 29.f, 37.f, 45.f, 53.f };
        const int   NZ   = 6;
        const glm::vec3 FC = { 0.22f, 0.13f, 0.06f }; // color del marco (marrón oscuro)
        const float PI   = glm::pi<float>();

        glDisable(GL_CULL_FACE); // los quads son de una sola cara — desactivar culling

        // ── Corredor izquierdo ── pinturas 01-06 (consecuencias) ──────────────
        for (int i = 0; i < NZ; i++) {
            // Pared exterior izquierda (X=-20, normal +X, observador mira +X): sin espejo
            drawPainting(sh, { -19.7f, CY, ZS[i] }, 0.f,  i,     false, FC);
            // Pared interior izquierda (X=-4.25, normal -X, observador mira -X): sin espejo
            drawPainting(sh, {  -4.85f, CY, ZS[i] }, PI,   i,     false, FC);
        }

        // ── Corredor derecho ── pinturas 07-12 (soluciones) ───────────────────
        for (int i = 0; i < NZ; i++) {
            // Pared interior derecha (X=+4.25, normal +X, observador mira -X): CON espejo
            drawPainting(sh, {  4.85f, CY, ZS[i] }, 0.f,  6 + i, true,  FC);
            // Pared exterior derecha (X=+20, normal -X, observador mira +X): CON espejo
            drawPainting(sh, { 19.7f,  CY, ZS[i] }, PI,   6 + i, true,  FC);
        }

        glEnable(GL_CULL_FACE);
    }

    void free() {
        for (auto& t : mTex) t.free();
        mCanvas.free();
        mCanvasMirr.free();
        mFrameH.free();
        mFrameV.free();
    }

private:
    // Dibuja una pintura enmarcada en la posición y orientación dadas.
    // yRot=0 → pintura mira +X ; yRot=π → pintura mira -X
    // mirror=true usa UV espejado para corregir la orientación horizontal.
    void drawPainting(Shader& sh, glm::vec3 pos, float yRot,
                      int texIdx, bool mirror, const glm::vec3& fc) {
        glm::mat4 base = glm::rotate(
            glm::translate(glm::mat4(1.f), pos),
            yRot, { 0.f, 1.f, 0.f });

        // ── Lienzo ────────────────────────────────────────────────────────────
        mTex[texIdx].bind(0);
        sh.setInt("diffuseMap", 0);
        sh.setBool("useTexture", true);
        sh.setMat4("model", base);
        (mirror ? mCanvasMirr : mCanvas).draw();

        // ── Marco (cuatro tiras de color sólido) ──────────────────────────────
        sh.setBool("useTexture", false);
        sh.setVec3("baseColor", fc);

        // Tira superior
        sh.setMat4("model", glm::translate(base, { 0.f,  CH * 0.5f + FB * 0.5f, 0.f }));
        mFrameH.draw();
        // Tira inferior
        sh.setMat4("model", glm::translate(base, { 0.f, -(CH * 0.5f + FB * 0.5f), 0.f }));
        mFrameH.draw();
        // Tira izquierda (en espacio local del quad, "izquierda" = Z negativo)
        sh.setMat4("model", glm::translate(base, { 0.f, 0.f, -(CW * 0.5f + FB * 0.5f) }));
        mFrameV.draw();
        // Tira derecha
        sh.setMat4("model", glm::translate(base, { 0.f, 0.f,  CW * 0.5f + FB * 0.5f }));
        mFrameV.draw();
    }
};
