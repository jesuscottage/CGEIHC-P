#pragma once

#ifndef ASSETS_DIR
#define ASSETS_DIR "assets/"
#endif

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "graphics/Mesh.h"
#include "graphics/Model.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include <vector>
#include <cmath>

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
// makePyramid â€” pirÃ¡mide de 4 caras laterales, base cuadrada en Y=0, Ã¡pice en Y=height
// Normales por cara para iluminaciÃ³n Blinn-Phong correcta
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
inline Mesh makePyramid(float base = 1.0f, float height = 1.5f) {
    float h  = base * 0.5f;
    glm::vec3 apex(0.f, height, 0.f);

    // Las 4 esquinas de la base (Y=0), en orden: -Z,-X / +Z,-X / +Z,+X / -Z,+X
    glm::vec3 b0(-h, 0.f, -h);
    glm::vec3 b1(-h, 0.f,  h);
    glm::vec3 b2( h, 0.f,  h);
    glm::vec3 b3( h, 0.f, -h);

    auto faceNormal = [](glm::vec3 a, glm::vec3 b, glm::vec3 c) -> glm::vec3 {
        return glm::normalize(glm::cross(b - a, c - a));
    };

    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    // Cara frontal: b0, b1, apex  (normal apunta hacia -X)
    // Cara trasera: b2, b3, apex  (normal apunta hacia +X)
    // Cara izq:    b1, b2, apex  (normal apunta hacia +Z)
    // Cara der:    b3, b0, apex  (normal apunta hacia -Z)
    struct Face { glm::vec3 v0, v1, v2; };
    Face faces[4] = {
        { b0, b3, apex },
        { b3, b2, apex },
        { b2, b1, apex },
        { b1, b0, apex },
    };

    for (int f = 0; f < 4; f++) {
        glm::vec3 n = faceNormal(faces[f].v0, faces[f].v1, faces[f].v2);
        unsigned int base_idx = (unsigned int)verts.size();
        verts.push_back({ faces[f].v0, n, {0.0f, 0.0f} });
        verts.push_back({ faces[f].v1, n, {1.0f, 0.0f} });
        verts.push_back({ faces[f].v2, n, {0.5f, 1.0f} });
        idx.push_back(base_idx);
        idx.push_back(base_idx + 1);
        idx.push_back(base_idx + 2);
    }

    // Base cuadrada (2 triÃ¡ngulos, normal hacia abajo)
    glm::vec3 nDown(0.f, -1.f, 0.f);
    unsigned int bi = (unsigned int)verts.size();
    verts.push_back({ b0, nDown, {0.f, 0.f} });
    verts.push_back({ b1, nDown, {0.f, 1.f} });
    verts.push_back({ b2, nDown, {1.f, 1.f} });
    verts.push_back({ b3, nDown, {1.f, 0.f} });
    idx.insert(idx.end(), { bi, bi+2, bi+1,  bi, bi+3, bi+2 });

    Mesh m; m.setup(verts, idx); return m;
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
// makeOctahedron â€” octaedro regular (cristal de hielo low-poly)
// Centrado en origen: Ã¡pice superior en Y=+radius, Ã¡pice inferior en Y=-radius
// Normales por cara para iluminaciÃ³n Blinn-Phong correcta
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
inline Mesh makeOctahedron(float radius = 1.0f) {
    // 6 vÃ©rtices del octaedro regular
    glm::vec3 top   ( 0.f,  radius, 0.f);
    glm::vec3 bot   ( 0.f, -radius, 0.f);
    glm::vec3 px    ( radius, 0.f,  0.f);
    glm::vec3 nx    (-radius, 0.f,  0.f);
    glm::vec3 pz    ( 0.f,  0.f,  radius);
    glm::vec3 nz    ( 0.f,  0.f, -radius);

    // 8 triÃ¡ngulos del octaedro
    struct Tri { glm::vec3 a, b, c; };
    Tri tris[8] = {
        // Hemisferio superior
        { top, px,  pz },
        { top, pz,  nx },
        { top, nx,  nz },
        { top, nz,  px },
        // Hemisferio inferior
        { bot, pz,  px },
        { bot, nx,  pz },
        { bot, nz,  nx },
        { bot, px,  nz },
    };

    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    for (int i = 0; i < 8; i++) {
        glm::vec3 n = glm::normalize(glm::cross(tris[i].b - tris[i].a, tris[i].c - tris[i].a));
        unsigned int base_idx = (unsigned int)verts.size();
        verts.push_back({ tris[i].a, n, {0.5f, 1.0f} });
        verts.push_back({ tris[i].b, n, {0.0f, 0.0f} });
        verts.push_back({ tris[i].c, n, {1.0f, 0.0f} });
        idx.push_back(base_idx);
        idx.push_back(base_idx + 1);
        idx.push_back(base_idx + 2);
    }

    Mesh m; m.setup(verts, idx); return m;
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
// DecoScene â€” decoraciÃ³n estÃ¡tica/dinÃ¡mica del museo de calentamiento global
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class DecoScene {
public:
    void init() {
        mCube  = makeCube();
        mDisc  = makeDisc(1.0f, 20, 0.08f);
        mPyram = makePyramid(1.0f, 1.5f);
        mOcta  = makeOctahedron(1.0f);

        // Terreno exterior: plano de nieve grande rodeando el museo
        mTerrain = makeGrid(600.f, 600.f, 8, 8, 0.015f);

        // Modelos descargados
        mIgloo.load(ASSETS_DIR "models/igloo_real.obj");
        mWolf.load(ASSETS_DIR  "models/wolf.glb");
        mPole.load(ASSETS_DIR  "models/broken_pole.fbx");

        // PingÃ¼ino de alta resoluciÃ³n (generado por script)
        mPenguin.load(ASSETS_DIR "models/penguin.obj");

        // Textura de nieve para terreno exterior
        mSnowTex.load(ASSETS_DIR "textures/snow_albedo.png");
    }

    void draw(Shader& sh, float t) {
        glDisable(GL_CULL_FACE);

        drawTerrenoExterior(sh);
        drawVestibulo(sh, t);
        drawCorredorIzq(sh, t);
        drawCorredorDer(sh, t);
        drawPasillocentral(sh, t);
        drawSalaM5(sh, t);

        glEnable(GL_CULL_FACE);
    }

    void free() {
        mCube.free();
        mDisc.free();
        mPyram.free();
        mOcta.free();
        mTerrain.free();
        mIgloo.free();
        mWolf.free();
        mPole.free();
        mPenguin.free();
        mSnowTex.free();
    }

private:
    Mesh    mCube, mDisc, mPyram, mOcta, mTerrain;
    Model   mIgloo, mWolf, mPole, mPenguin;
    Texture mSnowTex;

    // â”€â”€ Helpers â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void col(Shader& sh, const glm::vec3& c) {
        sh.setBool("useTexture", false);
        sh.setVec3("baseColor", c);
    }
    void mdl(Shader& sh, const glm::mat4& m) { sh.setMat4("model", m); }

    glm::mat4 T(glm::vec3 t) {
        return glm::translate(glm::mat4(1.f), t);
    }
    glm::mat4 TS(glm::vec3 t, glm::vec3 s) {
        return glm::scale(glm::translate(glm::mat4(1.f), t), s);
    }
    glm::mat4 TRS(glm::vec3 t, float deg, glm::vec3 ax, glm::vec3 s) {
        glm::mat4 m = glm::translate(glm::mat4(1.f), t);
        m = glm::rotate(m, glm::radians(deg), glm::normalize(ax));
        return glm::scale(m, s);
    }

    // â”€â”€ Paleta de colores temÃ¡ticos â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    static constexpr glm::vec3 cHielo()      { return {0.72f, 0.88f, 0.98f}; }
    static constexpr glm::vec3 cArbolMuerto(){ return {0.28f, 0.22f, 0.18f}; }
    static constexpr glm::vec3 cSolarPanel() { return {0.08f, 0.12f, 0.45f}; }
    static constexpr glm::vec3 cArbusto()    { return {0.18f, 0.62f, 0.22f}; }
    static constexpr glm::vec3 cMadera()     { return {0.48f, 0.36f, 0.24f}; }
    static constexpr glm::vec3 cTermometro() { return {0.95f, 0.15f, 0.12f}; }
    static constexpr glm::vec3 cDorado()     { return {0.85f, 0.70f, 0.10f}; }
    static constexpr glm::vec3 cBlanco()     { return {0.96f, 0.97f, 0.98f}; }
    static constexpr glm::vec3 cPantallaAzul(){ return {0.12f, 0.38f, 0.82f}; }
    static constexpr glm::vec3 cGrisClaro()  { return {0.80f, 0.82f, 0.85f}; }
    // Colores de modelos descargados
    static constexpr glm::vec3 cIglu()       { return {0.88f, 0.93f, 0.98f}; }
    static constexpr glm::vec3 cLobo()       { return {0.75f, 0.72f, 0.68f}; }
    static constexpr glm::vec3 cPoste()      { return {0.38f, 0.36f, 0.32f}; }
    static constexpr glm::vec3 cPinguino()   { return {0.08f, 0.08f, 0.10f}; }
    static constexpr glm::vec3 cNieve()      { return {0.90f, 0.93f, 0.97f}; }
    static constexpr glm::vec3 cArbolNieve() { return {0.14f, 0.34f, 0.16f}; } // verde pino

    // â”€â”€ AnimaciÃ³n: patrulla lineal entre dos puntos â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    // Devuelve posiciÃ³n interpolada y Ã¡ngulo Y mirando hacia donde avanza.
    struct PatrolResult { glm::vec3 pos; float yDeg; };
    PatrolResult patrol(glm::vec3 a, glm::vec3 b, float speed, float phase, float t) {
        float s   = sinf(t * speed + phase);           // [-1, 1]
        float fwd = cosf(t * speed + phase);           // velocidad: >0 = va hacia b
        float k   = (s + 1.f) * 0.5f;                 // [0, 1]
        glm::vec3 pos = a + (b - a) * k;
        float yDeg = glm::degrees(atan2f(b.x - a.x, b.z - a.z));
        if (fwd < 0.f) yDeg += 180.f;
        return {pos, yDeg};
    }

    // Dibuja un pingÃ¼ino con cuerpo negro + beak naranja en su posiciÃ³n correcta
    void drawPenguinAt(Shader& sh, glm::vec3 pos, float yRot, float sc = 1.0f) {
        glm::mat4 base = TRS(pos, yRot, {0.f,1.f,0.f}, {sc,sc,sc});
        // Cuerpo (todo negro)
        if (mPenguin.loaded) {
            glm::vec3 dark = cPinguino();
            mdl(sh, base);
            mPenguin.draw(sh, &dark);
        }
        // Beak naranja: en espacio local del pingÃ¼ino estÃ¡ en (0, 0.73, 0.22),
        // apuntando +Z. Rotamos el mPyram -90Â° en X para que apunte hacia delante.
        glm::mat4 bk = glm::translate(base, {0.f, 0.73f, 0.22f});
        bk = glm::rotate(bk, glm::radians(-90.f), {1.f,0.f,0.f});
        bk = glm::scale(bk, {0.06f, 0.14f, 0.06f});
        col(sh, {1.0f, 0.55f, 0.0f});   // naranja
        mdl(sh, bk);
        mPyram.draw();
    }

    // Dibuja un lobo en pos/yRot con escala ajustada
    void drawWolfAt(Shader& sh, glm::vec3 pos, float yRot, float sc = 2.2f) {
        if (!mWolf.loaded) return;
        glm::vec3 c = cLobo();
        drawModel(sh, mWolf, TRS(pos, yRot, {0.f,1.f,0.f}, {sc,sc,sc}), c);
    }

    // â”€â”€ Sub-primitivas reutilizables â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    // Pilar de hielo: cubo (0.6 Ã— height Ã— 0.6) + octaedro en la cima
    void drawPilarHielo(Shader& sh, glm::vec3 pos, float height) {
        // Columna
        col(sh, cHielo());
        mdl(sh, TS(pos + glm::vec3(0.f, height * 0.5f, 0.f), {0.6f, height, 0.6f}));
        mCube.draw();
        // Octaedro encima
        col(sh, cHielo() * 0.92f);
        mdl(sh, TS(pos + glm::vec3(0.f, height + 0.5f, 0.f), {0.55f, 0.55f, 0.55f}));
        mOcta.draw();
    }

    // Banca: asiento + respaldo + 4 patas
    void drawBanca(Shader& sh, glm::vec3 pos, float yRot = 0.f) {
        glm::mat4 base = glm::rotate(T(pos), glm::radians(yRot), {0.f, 1.f, 0.f});

        auto lugar = [&](glm::vec3 offset, glm::vec3 s) {
            mdl(sh, glm::scale(glm::translate(base, offset), s));
            mCube.draw();
        };

        col(sh, cMadera());
        // Asiento
        lugar({0.f, 0.42f, 0.f}, {1.20f, 0.08f, 0.45f});
        // Respaldo
        lugar({0.f, 0.72f, -0.18f}, {1.20f, 0.52f, 0.07f});
        // Patas (4)
        col(sh, cMadera() * 0.80f);
        lugar({-0.50f, 0.20f, -0.16f}, {0.07f, 0.40f, 0.07f});
        lugar({ 0.50f, 0.20f, -0.16f}, {0.07f, 0.40f, 0.07f});
        lugar({-0.50f, 0.20f,  0.16f}, {0.07f, 0.40f, 0.07f});
        lugar({ 0.50f, 0.20f,  0.16f}, {0.07f, 0.40f, 0.07f});
    }

    // Ãrbol muerto: tronco + 2 ramas rotas
    void drawArbolMuerto(Shader& sh, glm::vec3 pos) {
        col(sh, cArbolMuerto());
        // Tronco
        mdl(sh, TS(pos + glm::vec3(0.f, 2.0f, 0.f), {0.25f, 4.0f, 0.25f}));
        mCube.draw();
        // Rama rota izquierda (-40Â° en Z)
        mdl(sh, TRS(pos + glm::vec3(-0.55f, 3.2f, 0.f), -40.f, {0.f, 0.f, 1.f}, {0.14f, 1.5f, 0.14f}));
        mCube.draw();
        // Rama rota derecha (+35Â° en Z)
        mdl(sh, TRS(pos + glm::vec3( 0.55f, 2.8f, 0.f),  35.f, {0.f, 0.f, 1.f}, {0.14f, 1.2f, 0.14f}));
        mCube.draw();
    }

    // Cluster de arbustos: 3 octaedros verdes agrupados
    void drawArbusto(Shader& sh, glm::vec3 pos) {
        col(sh, cArbusto());
        mdl(sh, TS(pos + glm::vec3( 0.00f, 0.55f,  0.00f), {0.65f, 0.65f, 0.65f}));
        mOcta.draw();
        col(sh, cArbusto() * 0.88f);
        mdl(sh, TS(pos + glm::vec3(-0.40f, 0.40f, -0.15f), {0.50f, 0.50f, 0.50f}));
        mOcta.draw();
        mdl(sh, TS(pos + glm::vec3( 0.38f, 0.38f,  0.18f), {0.48f, 0.48f, 0.48f}));
        mOcta.draw();
    }

    // Pilar informativo: columna + pantalla azul + octaedro decorativo
    void drawPilarInfo(Shader& sh, glm::vec3 pos) {
        col(sh, cGrisClaro());
        // Columna
        mdl(sh, TS(pos + glm::vec3(0.f, 1.5f, 0.f), {0.30f, 3.0f, 0.30f}));
        mCube.draw();
        // Pantalla azul
        col(sh, cPantallaAzul());
        mdl(sh, TS(pos + glm::vec3(0.f, 2.5f, 0.16f), {0.65f, 0.80f, 0.05f}));
        mCube.draw();
        // Octaedro decorativo encima
        col(sh, cHielo());
        mdl(sh, TS(pos + glm::vec3(0.f, 3.2f, 0.f), {0.30f, 0.30f, 0.30f}));
        mOcta.draw();
    }

    // Panel solar: poste + panel inclinado
    void drawPanelSolar(Shader& sh, glm::vec3 pos) {
        col(sh, cGrisClaro());
        // Poste
        mdl(sh, TS(pos + glm::vec3(0.f, 1.5f, 0.f), {0.12f, 3.0f, 0.12f}));
        mCube.draw();
        // Panel (inclinado 30Â° en X, azul oscuro)
        col(sh, cSolarPanel());
        mdl(sh, TRS(pos + glm::vec3(0.f, 3.1f, -0.25f), -30.f, {1.f, 0.f, 0.f}, {1.20f, 0.06f, 0.85f}));
        mCube.draw();
    }

    // Cristal de hielo pequeÃ±o: octaedro escalado
    void drawCristalHielo(Shader& sh, glm::vec3 pos, float r = 0.40f) {
        col(sh, cHielo());
        mdl(sh, TS(pos + glm::vec3(0.f, r, 0.f), {r, r * 1.4f, r}));
        mOcta.draw();
    }

    // Mancha de nieve derretida: cubo muy aplastado blanco
    void drawNieveDerretida(Shader& sh, glm::vec3 pos, float rx, float rz) {
        col(sh, cBlanco());
        mdl(sh, TS(pos + glm::vec3(0.f, 0.02f, 0.f), {rx * 2.f, 0.06f, rz * 2.f}));
        mCube.draw();
    }

    // Helper: dibujar modelo con TRS y color de override
    void drawModel(Shader& sh, Model& model, const glm::mat4& m, const glm::vec3& c) {
        mdl(sh, m);
        model.draw(sh, &c);
    }

    // Ãrbol tipo conÃ­fera nevado: tronco + 3 capas de pirÃ¡mides + nieve en cima
    void drawArbolNieve(Shader& sh, glm::vec3 pos, float escala = 1.0f) {
        // Tronco
        col(sh, cArbolMuerto());
        mdl(sh, TS(pos + glm::vec3(0.f, 1.2f * escala, 0.f),
                   {0.18f * escala, 2.4f * escala, 0.18f * escala}));
        mCube.draw();
        // 3 niveles de ramas (pirÃ¡mides verdes, de abajo hacia arriba)
        const float pyH[]  = {0.f, 1.4f, 2.4f};
        const float pyR[]  = {1.4f, 1.0f, 0.65f};
        const float pyHt[] = {1.6f, 1.3f, 1.0f};
        for (int k = 0; k < 3; k++) {
            col(sh, cArbolNieve());
            mdl(sh, TS(pos + glm::vec3(0.f, pyH[k] * escala, 0.f),
                       {pyR[k] * escala, pyHt[k] * escala, pyR[k] * escala}));
            mPyram.draw();
            // Nieve encima de cada capa
            col(sh, cNieve());
            mdl(sh, TS(pos + glm::vec3(0.f, (pyH[k] + pyHt[k] * 0.85f) * escala, 0.f),
                       {pyR[k] * 0.55f * escala, pyHt[k] * 0.18f * escala, pyR[k] * 0.55f * escala}));
            mPyram.draw();
        }
    }

    // â”€â”€ TERRENO EXTERIOR (nieve mÃ¡s allÃ¡ de las paredes) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void drawTerrenoExterior(Shader& sh) {
        // Plano de nieve grande, ligeramente bajo el nivel del suelo del museo
        glm::mat4 m = glm::translate(glm::mat4(1.f), {0.f, -0.15f, 35.f});
        sh.setMat4("model", m);
        if (mSnowTex.id) {
            sh.setBool("useTexture", true);
            mSnowTex.bind(0);
            sh.setInt("diffuseMap", 0);
        } else {
            sh.setBool("useTexture", false);
            sh.setVec3("baseColor", cNieve());
        }
        mTerrain.draw();
        sh.setBool("useTexture", false);

        // MontÃ­culos de nieve en el horizonte (cubos aplastados y pirÃ¡mides)
        const float hillPos[][2] = {
            {-80,  20}, {-120, 45}, {-90, 70}, {-110,100},
            { 80,  20}, { 120, 45}, { 90, 70}, { 110,100},
            {-40,-40},  {  0,-60},  { 40,-40},
            {-50, 130},  { 50, 130},
        };
        for (auto& hp : hillPos) {
            float x = hp[0], z = hp[1];
            float s = 12.f + (fabsf(x) + fabsf(z)) * 0.05f;
            col(sh, cNieve());
            mdl(sh, TS({x, -0.15f, z}, {s * 2.f, s * 0.55f, s * 1.5f}));
            mPyram.draw();
            col(sh, cNieve() * 0.92f);
            mdl(sh, TS({x + s * 0.6f, -0.15f, z + s * 0.3f},
                       {s * 1.4f, s * 0.35f, s * 1.1f}));
            mPyram.draw();
        }

        // Ãrboles nevados dispersos en el exterior
        const float treePos[][2] = {
            {-25,-20},{-32,-35},{25,-20},{32,-35},
            {-28,100},{28,100},{-35,80},{35,80},
            {-70, 30},{70, 30},{-65,60},{65,60},
        };
        for (auto& tp : treePos) {
            drawArbolNieve(sh, {tp[0], -0.15f, tp[1]}, 1.2f);
        }
    }

    // â”€â”€ VESTÃBULO â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void drawVestibulo(Shader& sh, float t) {
        // Podio de bienvenida central (X=0, Z=1.5)
        glm::vec3 podioPos(0.f, 0.f, 1.5f);
        // Base del podio
        col(sh, cGrisClaro());
        mdl(sh, TS(podioPos + glm::vec3(0.f, 0.20f, 0.f), {1.20f, 0.40f, 0.80f}));
        mCube.draw();
        // Columna central
        col(sh, cGrisClaro() * 0.90f);
        mdl(sh, TS(podioPos + glm::vec3(0.f, 0.80f, 0.f), {0.30f, 0.80f, 0.30f}));
        mCube.draw();
        // Panel azul (cartel de bienvenida)
        col(sh, cPantallaAzul());
        mdl(sh, TS(podioPos + glm::vec3(0.f, 1.45f, 0.f), {1.00f, 0.55f, 0.06f}));
        mCube.draw();

        // Pilares de hielo flanqueando la entrada a corredores (X=Â±7.5, Z=8.5)
        drawPilarHielo(sh, {-7.5f, 0.f, 8.5f}, 4.2f);
        drawPilarHielo(sh, { 7.5f, 0.f, 8.5f}, 4.2f);

        // Bancas laterales (X=Â±6.5, Z=5)
        drawBanca(sh, {-6.5f, 0.f, 5.0f},  90.f);
        drawBanca(sh, { 6.5f, 0.f, 5.0f}, -90.f);

        // Cristales de hielo en esquinas del vestÃ­bulo
        drawCristalHielo(sh, {-8.5f, 0.f, -2.0f}, 0.45f);
        drawCristalHielo(sh, {-8.5f, 0.f, -4.0f}, 0.35f);
        drawCristalHielo(sh, { 8.5f, 0.f, -2.0f}, 0.45f);
        drawCristalHielo(sh, { 8.5f, 0.f, -4.0f}, 0.35f);

        // IglÃº real â€” OBJ de 3dsMax; Y vertical, geometrÃ­a Â±5u â†’ escala 0.35.
        // -90Â° en X corrige el domo si miraba hacia Z.
        if (mIgloo.loaded) {
            glm::vec3 c = cIglu();
            glm::mat4 m = glm::translate(glm::mat4(1.f), {-6.f, 0.f, 4.f});
            m = glm::rotate(m, glm::radians(-90.f), {1.f,0.f,0.f});
            m = glm::rotate(m, glm::radians(30.f),  {0.f,0.f,1.f});
            m = glm::scale(m, {0.35f,0.35f,0.35f});
            mdl(sh, m);
            mIgloo.draw(sh, &c);
        }

        // Lobo patrullando el vestÃ­bulo (X: -5 â†” +5)
        {
            auto p = patrol({-5.f,0.f,5.f}, {5.f,0.f,5.f}, 0.4f, 0.f, t);
            drawWolfAt(sh, p.pos, p.yDeg);
        }

        // PingÃ¼inos flanqueando: van y vienen simÃ©tricamente
        {
            auto pL = patrol({-4.f,0.f,8.f}, {-4.f,0.f,2.f}, 0.6f, 0.f, t);
            auto pR = patrol({ 4.f,0.f,2.f}, { 4.f,0.f,8.f}, 0.6f, 0.f, t);
            drawPenguinAt(sh, pL.pos, pL.yDeg, 1.1f);
            drawPenguinAt(sh, pR.pos, pR.yDeg, 1.1f);
        }
    }

    // â”€â”€ CORREDOR IZQ (consecuencias) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void drawCorredorIzq(Shader& sh, float t) {
        // Ãrboles muertos contra pared exterior (X=-18.5)
        drawArbolMuerto(sh, {-18.5f, 0.f, 18.0f});
        drawArbolMuerto(sh, {-18.5f, 0.f, 33.0f});
        drawArbolMuerto(sh, {-18.5f, 0.f, 50.0f});

        // TermÃ³metro gigante (X=-18, Z=22)
        col(sh, cBlanco());
        mdl(sh, TS({-18.f, 3.0f, 22.f}, {0.28f, 6.0f, 0.28f}));
        mCube.draw();
        col(sh, cTermometro());
        mdl(sh, TS({-18.f, 0.40f, 22.f}, {0.70f, 0.70f, 0.70f}));
        mOcta.draw();
        col(sh, cTermometro());
        mdl(sh, TS({-18.f, 0.40f + 5.28f * 0.5f, 22.f}, {0.18f, 5.28f, 0.18f}));
        mCube.draw();

        // Estalactitas de hielo del techo (pirÃ¡mide invertida)
        auto estalactita = [&](float x, float z) {
            col(sh, cHielo());
            mdl(sh, TRS({x, 6.0f, z}, 180.f, {1.f, 0.f, 0.f}, {0.40f, 1.20f, 0.40f}));
            mPyram.draw();
        };
        estalactita(-18.0f, 15.0f);
        estalactita(-16.5f, 15.0f);
        estalactita(-18.0f, 47.0f);
        estalactita(-16.5f, 47.0f);

        // Manchas de nieve derretida en suelo
        drawNieveDerretida(sh, {-16.0f, 0.f, 20.0f}, 1.2f, 0.8f);
        drawNieveDerretida(sh, {-17.5f, 0.f, 35.0f}, 0.9f, 0.6f);
        drawNieveDerretida(sh, {-15.5f, 0.f, 52.0f}, 1.4f, 1.0f);

        // Pilares informativos en pared interior (X=-5.5)
        drawPilarInfo(sh, {-5.5f, 0.f, 20.0f});
        drawPilarInfo(sh, {-5.5f, 0.f, 50.0f});

        // Ãrboles nevados a lo largo del corredor izquierdo
        const float treeZ[] = {13.f, 20.f, 27.f, 34.f, 41.f, 48.f, 55.f};
        for (float tz : treeZ)
            drawArbolNieve(sh, {-17.5f, 0.f, tz}, 0.85f);

        // Postes rotos â€” FBX Blender en cm â†’ escala 0.01
        if (mPole.loaded) {
            glm::vec3 c = cPoste();
            drawModel(sh, mPole,
                TRS({-16.f, 0.f, 28.f},  10.f, {0.f,1.f,0.f}, {0.01f,0.01f,0.01f}), c);
            drawModel(sh, mPole,
                TRS({-17.f, 0.f, 44.f},  -8.f, {0.f,1.f,0.f}, {0.01f,0.01f,0.01f}), c);
        }

        // Lobo patrullando todo el corredor izquierdo (Z: 12 â†” 57)
        {
            auto p = patrol({-13.f,0.f,12.f}, {-13.f,0.f,57.f}, 0.25f, 1.0f, t);
            drawWolfAt(sh, p.pos, p.yDeg);
        }

        // Dos pingÃ¼inos patrullando en tramos distintos del corredor
        {
            auto p1 = patrol({-14.f,0.f,13.f}, {-14.f,0.f,32.f}, 0.5f, 0.0f, t);
            auto p2 = patrol({-14.f,0.f,55.f}, {-14.f,0.f,36.f}, 0.5f, 1.6f, t);
            drawPenguinAt(sh, p1.pos, p1.yDeg, 1.1f);
            drawPenguinAt(sh, p2.pos, p2.yDeg, 1.1f);
        }
    }

    // â”€â”€ CORREDOR DER (soluciones) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void drawCorredorDer(Shader& sh, float t) {
        // Paneles solares contra pared exterior (X=18.5)
        drawPanelSolar(sh, {18.5f, 0.f, 18.0f});
        drawPanelSolar(sh, {18.5f, 0.f, 24.0f});
        drawPanelSolar(sh, {18.5f, 0.f, 47.0f});
        drawPanelSolar(sh, {18.5f, 0.f, 53.0f});

        // Arbustos verdes
        drawArbusto(sh, {17.5f, 0.f, 32.0f});
        drawArbusto(sh, {18.0f, 0.f, 38.0f});
        drawArbusto(sh, {17.2f, 0.f, 56.0f});

        // Cristales de hielo pequeÃ±os
        drawCristalHielo(sh, {18.0f, 0.f, 30.0f}, 0.38f);
        drawCristalHielo(sh, {18.0f, 0.f, 32.0f}, 0.30f);

        // Pilares informativos en pared interior (X=5.5)
        drawPilarInfo(sh, {5.5f, 0.f, 20.0f});
        drawPilarInfo(sh, {5.5f, 0.f, 50.0f});

        // Banca
        drawBanca(sh, {18.0f, 0.f, 38.5f}, 180.f);

        // Ãrboles nevados a lo largo del corredor derecho
        const float treeZd[] = {13.f, 20.f, 28.f, 36.f, 44.f, 52.f};
        for (float tz : treeZd)
            drawArbolNieve(sh, {17.5f, 0.f, tz}, 0.85f);

        // Lobo patrullando corredor derecho (Z: 12 â†” 57), fase opuesta al izq
        {
            auto p = patrol({13.f,0.f,57.f}, {13.f,0.f,12.f}, 0.25f, 0.5f, t);
            drawWolfAt(sh, p.pos, p.yDeg);
        }

        // Dos pingÃ¼inos en el corredor derecho
        {
            auto p1 = patrol({14.f,0.f,14.f}, {14.f,0.f,33.f}, 0.45f, 0.8f, t);
            auto p2 = patrol({14.f,0.f,54.f}, {14.f,0.f,37.f}, 0.45f, 2.4f, t);
            drawPenguinAt(sh, p1.pos, p1.yDeg, 1.1f);
            drawPenguinAt(sh, p2.pos, p2.yDeg, 1.1f);
        }
    }

    // â”€â”€ PASILLO CENTRAL (Xâˆˆ[-4,4]) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void drawPasillocentral(Shader& sh, float t) {
        // Bancas en pares (Z=25 y Z=43)
        drawBanca(sh, {-1.5f, 0.f, 25.0f},   0.f);
        drawBanca(sh, { 1.5f, 0.f, 25.0f}, 180.f);
        drawBanca(sh, {-1.5f, 0.f, 43.0f},   0.f);
        drawBanca(sh, { 1.5f, 0.f, 43.0f}, 180.f);

        // Clusters de cristales de hielo a los lados (2 octaedros por cluster)
        // Z=15
        drawCristalHielo(sh, {-2.8f, 0.f, 15.0f}, 0.42f);
        drawCristalHielo(sh, {-2.4f, 0.f, 15.6f}, 0.32f);
        drawCristalHielo(sh, { 2.8f, 0.f, 15.0f}, 0.42f);
        drawCristalHielo(sh, { 2.4f, 0.f, 15.6f}, 0.32f);
        // Z=30
        drawCristalHielo(sh, {-2.8f, 0.f, 30.0f}, 0.38f);
        drawCristalHielo(sh, {-2.3f, 0.f, 30.5f}, 0.28f);
        drawCristalHielo(sh, { 2.8f, 0.f, 30.0f}, 0.38f);
        drawCristalHielo(sh, { 2.3f, 0.f, 30.5f}, 0.28f);
        // Z=45
        drawCristalHielo(sh, {-2.8f, 0.f, 45.0f}, 0.44f);
        drawCristalHielo(sh, {-2.5f, 0.f, 45.6f}, 0.30f);
        drawCristalHielo(sh, { 2.8f, 0.f, 45.0f}, 0.44f);
        drawCristalHielo(sh, { 2.5f, 0.f, 45.6f}, 0.30f);

        // PingÃ¼ino patrullando el pasillo central de extremo a extremo
        {
            auto p = patrol({0.f,0.f,12.f}, {0.f,0.f,58.f}, 0.18f, 3.0f, t);
            drawPenguinAt(sh, p.pos, p.yDeg, 1.1f);
        }
    }

    // â”€â”€ SALA M5 â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void drawSalaM5(Shader& sh, float t) {
        glm::vec3 m5center(0.f, 0.f, 63.f);
        const float bancaRadio = 11.0f;

        // 6 bancas en semicÃ­rculo (frente al mÃ³dulo M5, mirando hacia Z negativo)
        for (int i = 0; i < 6; i++) {
            // SemicÃ­rculo frontal: Ã¡ngulos de 200Â° a 340Â° para que miren al mÃ³dulo
            float angle = glm::radians(200.f + i * 28.f);
            float bx = m5center.x + bancaRadio * cosf(angle);
            float bz = m5center.z + bancaRadio * sinf(angle);
            // La banca mira hacia el centro
            float yRot = glm::degrees(atan2f(-cosf(angle), sinf(angle)));
            drawBanca(sh, {bx, 0.f, bz}, yRot);
        }

        // Pilares de hielo en la entrada de la sala (X=Â±9.5, Z=60.5) â€” mÃ¡s altos
        drawPilarHielo(sh, {-9.5f, 0.f, 60.5f}, 5.2f);
        drawPilarHielo(sh, { 9.5f, 0.f, 60.5f}, 5.2f);

        // Cristales grandes en esquinas de la sala
        drawCristalHielo(sh, {-18.5f, 0.f, 61.5f}, 0.85f);
        drawCristalHielo(sh, { 18.5f, 0.f, 61.5f}, 0.85f);
        drawCristalHielo(sh, {-18.5f, 0.f, 73.5f}, 0.80f);
        drawCristalHielo(sh, { 18.5f, 0.f, 73.5f}, 0.80f);

        // 12 marcadores dorados en anillo decorativo (radio 8.5m desde M5)
        const float markerRadio = 8.5f;
        for (int i = 0; i < 12; i++) {
            float angle = glm::radians(i * 30.f);
            float mx = m5center.x + markerRadio * cosf(angle);
            float mz = m5center.z + markerRadio * sinf(angle);
            col(sh, cDorado());
            mdl(sh, TS({mx, 0.f, mz}, {0.18f, 0.50f, 0.18f}));
            mCube.draw();
            col(sh, cDorado() * 1.1f);
            mdl(sh, TS({mx, 0.55f, mz}, {0.22f, 0.22f, 0.22f}));
            mOcta.draw();
        }

        // Arbustos decorativos (X=Â±16, Z=62)
        drawArbusto(sh, {-16.f, 0.f, 62.f});
        drawArbusto(sh, { 16.f, 0.f, 62.f});

        // Dos lobos en patrulla circular alrededor del centro de M5
        {
            float r = 9.f;
            for (int i = 0; i < 2; i++) {
                float ang = t * 0.3f + i * glm::pi<float>();
                glm::vec3 pos = {m5center.x + r * cosf(ang), 0.f,
                                 m5center.z + r * sinf(ang)};
                float yDeg = glm::degrees(ang) + 90.f; // tangente al cÃ­rculo
                drawWolfAt(sh, pos, yDeg);
            }
        }

        // Grupo de 5 pingÃ¼inos patrullando en la sala M5
        // Cada uno recorre un segmento distinto del semicÃ­rculo frontal
        const glm::vec3 pA[] = {
            {-5.f,0.f,57.f},{-2.f,0.f,56.f},{ 0.f,0.f,55.f},
            { 2.f,0.f,56.f},{ 5.f,0.f,57.f}
        };
        const glm::vec3 pB[] = {
            {-5.f,0.f,62.f},{-2.f,0.f,63.f},{ 0.f,0.f,64.f},
            { 2.f,0.f,63.f},{ 5.f,0.f,62.f}
        };
        for (int i = 0; i < 5; i++) {
            auto p = patrol(pA[i], pB[i], 0.55f, i * 0.8f, t);
            drawPenguinAt(sh, p.pos, p.yDeg, 1.1f);
        }
    }
};

