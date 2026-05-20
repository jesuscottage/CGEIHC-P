#pragma once
// ModuleScene.h — Geometría animada + modelos reales + fauna decorativa
// Cada módulo tiene su propia escena 3D que anima con animT (0→1).
// Los módulos con modelo GLB disponible lo usan; los demás usan geometría procedural.

#include "graphics/Mesh.h"
#include "graphics/Model.h"
#include "graphics/Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <string>
#include <cmath>

#ifndef ASSETS_DIR
#define ASSETS_DIR "assets/"
#endif

class ModuleScene {
public:
    void init() {
        mCube = makeCube();
        mDisc = makeDisc(1.0f, 24, 0.08f);
        // Modelos de módulos
        mPineModel.load(ASSETS_DIR "models/pine_snow.glb");  // Quaternius CC0 (textura embebida)
        mCarModel.load(ASSETS_DIR "models/electric_car.glb");
        mBuildingA.load(ASSETS_DIR "models/building_a.glb");
        mBuildingB.load(ASSETS_DIR "models/building_b.glb");
        mBuildingC.load(ASSETS_DIR "models/building_c.glb");
        mGlobeModel.load(ASSETS_DIR "models/globe.glb");
        mIcebergModel.load(ASSETS_DIR "models/iceberg.glb");
        mRockModel.load(ASSETS_DIR "models/rock.glb");       // Quaternius CC0 (decoración)
        mBearModel.load(ASSETS_DIR "models/polar_bear.glb"); // Quaternius Wolf CC0
        mEolicModel.load(ASSETS_DIR "models/EolicOBJ.obj");  // Turbina eólica real
        mModelSCar.load(ASSETS_DIR "models/ModelS.obj");     // Tesla Model S (reemplaza Kenney)
        mFactoryModel.load(ASSETS_DIR "models/Factory.obj"); // Fábrica — M3_DER Captura de Carbono
        // Fauna
        mFoxModel.load(ASSETS_DIR "models/fox.glb");       // KhronosGroup Fox (textura embebida)
        mBirdModel.load(ASSETS_DIR "models/seagull.glb");   // three.js Stork
        mSealModel.load(ASSETS_DIR "models/seal.glb");      // Quaternius Husky CC0
        mWhaleModel.load(ASSETS_DIR "models/whale.glb");   // Quaternius CC0
    }

    // Dibuja la escena de un módulo.
    // id:     "M1_IZQ", "M2_IZQ", etc.
    // center: posición central del módulo (Y=0)
    // animT:  progreso de la animación [0,1]
    // time:   glfwGetTime() — para rotaciones y ciclos continuos
    void draw(Shader& sh, const std::string& id, const glm::vec3& center,
              float animT, float time)
    {
        if      (id == "M1_IZQ") drawIceberg    (sh, center, animT);
        else if (id == "M2_IZQ") drawIceFloe    (sh, center, animT);
        else if (id == "M3_IZQ") drawFlood      (sh, center, animT);
        else if (id == "M1_DER") drawTurbine    (sh, center, animT, time);
        else if (id == "M2_DER") drawElectricCar(sh, center, animT, time);
        else if (id == "M3_DER") drawTree       (sh, center, animT, time);
        else if (id == "M5")     drawGlobe      (sh, center, animT, time);
    }

    void free() {
        mCube.free(); mDisc.free();
        mPineModel.free(); mCarModel.free();
        mBuildingA.free(); mBuildingB.free(); mBuildingC.free();
        mGlobeModel.free(); mIcebergModel.free(); mRockModel.free();
        mBearModel.free(); mFoxModel.free(); mBirdModel.free();
        mSealModel.free(); mWhaleModel.free();
        mEolicModel.free();
        mModelSCar.free();
        mFactoryModel.free();
    }

    // ── Fauna decorativa estática ──────────────────────────────────────────
    // Dibuja la fauna en posiciones fijas del museo.
    // Llamar una vez por frame después de dibujar los módulos.
    void drawFauna(Shader& sh) {
        // Zorro ártico (modelo Fox de KhronosGroup)
        if (mFoxModel.loaded) {
            glm::mat4 m = glm::translate(glm::mat4(1.f), {-17.5f, 0.0f, 5.5f});
            m = glm::scale(m, glm::vec3(0.015f)); // Fox model es ~100 unidades
            mdl(sh, m);
            mFoxModel.draw(sh);
        } else {
            drawArcticFox(sh, {-17.5f, 0.0f, 5.5f});
        }

        // Gaviota (three.js Stork — cigüeña blanca volando)
        if (mBirdModel.loaded) {
            glm::vec3 white(0.92f, 0.94f, 0.98f);
            glm::mat4 m = glm::translate(glm::mat4(1.f), {-8.0f, 3.5f, 31.0f});
            m = glm::scale(m, glm::vec3(0.015f)); // Stork es ~100 unidades
            mdl(sh, m);
            mBirdModel.draw(sh, &white);
        } else {
            drawSeagull(sh, {-8.0f, 3.8f, 31.0f});
        }

        // Foca (Quaternius Husky como animal ártico)
        if (mSealModel.loaded) {
            glm::vec3 gray(0.75f, 0.78f, 0.82f);
            glm::mat4 m = glm::translate(glm::mat4(1.f), {-15.0f, 0.0f, 25.0f});
            m = glm::scale(m, glm::vec3(10.0f));
            mdl(sh, m);
            mSealModel.draw(sh, &gray);
        } else {
            drawSeal(sh, {-15.0f, 0.0f, 25.0f});
        }

        // Ballena decorativa (Quaternius, fuera del corredor izq)
        if (mWhaleModel.loaded) {
            glm::vec3 blueGray(0.45f, 0.55f, 0.70f);
            glm::mat4 m = glm::translate(glm::mat4(1.f), {-18.0f, 3.5f, 50.0f});
            m = glm::rotate(m, glm::radians(25.f), {0.f, 1.f, 0.f});
            m = glm::scale(m, glm::vec3(12.0f));
            mdl(sh, m);
            mWhaleModel.draw(sh, &blueGray);
        }

        // Rocas decorativas (Quaternius, dispersas en corredores)
        if (mRockModel.loaded) {
            glm::vec3 rockCol(0.70f, 0.75f, 0.80f);
            glm::vec3 rockPositions[] = {
                {-18.0f, 0.0f, 18.0f}, {-6.0f, 0.0f, 35.0f},
                { 18.0f, 0.0f, 20.0f}, { 6.0f, 0.0f, 55.0f},
            };
            float rockScales[] = {3.0f, 2.5f, 3.5f, 2.0f};
            for (int i = 0; i < 4; i++) {
                glm::mat4 m = glm::translate(glm::mat4(1.f), rockPositions[i]);
                m = glm::rotate(m, glm::radians(i * 73.f), {0.f, 1.f, 0.f});
                m = glm::scale(m, glm::vec3(rockScales[i]));
                mdl(sh, m);
                mRockModel.draw(sh, &rockCol);
            }
        }
    }

private:
    Mesh mCube, mDisc;
    Model mPineModel, mCarModel, mBuildingA, mBuildingB, mBuildingC, mGlobeModel;
    Model mIcebergModel, mRockModel, mBearModel, mFoxModel, mBirdModel, mSealModel, mWhaleModel;
    Model mEolicModel;
    Model mModelSCar;
    Model mFactoryModel;

    // ── Fauna helpers ──────────────────────────────────────────────────────
    void drawSeal(Shader& sh, glm::vec3 pos) {
        // Cuerpo: elipsoide simulado con un cubo escalado + disco base
        col(sh, glm::vec3(0.28f, 0.25f, 0.22f)); // gris oscuro
        mdl(sh, TS(pos + glm::vec3(0, 0.22f, 0), {0.52f, 0.28f, 0.90f}));
        mCube.draw();
        // Cabeza
        col(sh, glm::vec3(0.32f, 0.28f, 0.24f));
        mdl(sh, TS(pos + glm::vec3(0, 0.42f, 0.48f), {0.28f, 0.26f, 0.28f}));
        mCube.draw();
        // Aletas traseras
        col(sh, glm::vec3(0.22f, 0.20f, 0.18f));
        mdl(sh, TS(pos + glm::vec3(-0.18f, 0.06f, -0.50f),
                    {0.14f, 0.06f, 0.30f}));
        mCube.draw();
        mdl(sh, TS(pos + glm::vec3( 0.18f, 0.06f, -0.50f),
                    {0.14f, 0.06f, 0.30f}));
        mCube.draw();
    }

    void drawArcticFox(Shader& sh, glm::vec3 pos) {
        // Cuerpo
        col(sh, glm::vec3(0.92f, 0.93f, 0.95f)); // blanco-grisáceo
        mdl(sh, TS(pos + glm::vec3(0, 0.22f, 0), {0.28f, 0.25f, 0.65f}));
        mCube.draw();
        // Cabeza
        col(sh, glm::vec3(0.88f, 0.89f, 0.92f));
        mdl(sh, TS(pos + glm::vec3(0, 0.48f, 0.36f), {0.22f, 0.22f, 0.22f}));
        mCube.draw();
        // Orejas
        col(sh, glm::vec3(0.85f, 0.85f, 0.88f));
        mdl(sh, TS(pos + glm::vec3(-0.08f, 0.70f, 0.36f), {0.06f, 0.10f, 0.04f}));
        mCube.draw();
        mdl(sh, TS(pos + glm::vec3( 0.08f, 0.70f, 0.36f), {0.06f, 0.10f, 0.04f}));
        mCube.draw();
        // Cola esponjosa
        col(sh, glm::vec3(0.96f, 0.96f, 0.98f));
        mdl(sh, TS(pos + glm::vec3(0, 0.30f, -0.45f), {0.18f, 0.18f, 0.22f}));
        mCube.draw();
    }

    void drawSeagull(Shader& sh, glm::vec3 pos) {
        // Cuerpo central
        col(sh, glm::vec3(0.95f, 0.96f, 0.98f)); // blanco
        mdl(sh, TS(pos, {0.15f, 0.12f, 0.35f}));
        mCube.draw();
        // Alas — dos planos horizontales a los lados
        col(sh, glm::vec3(0.90f, 0.92f, 0.95f));
        mdl(sh, TS(pos + glm::vec3(-0.50f, 0.02f, 0), {0.65f, 0.04f, 0.18f}));
        mCube.draw();
        mdl(sh, TS(pos + glm::vec3( 0.50f, 0.02f, 0), {0.65f, 0.04f, 0.18f}));
        mCube.draw();
        // Cabeza y pico
        col(sh, glm::vec3(0.96f, 0.96f, 0.98f));
        mdl(sh, TS(pos + glm::vec3(0, 0.10f, 0.20f), {0.10f, 0.10f, 0.10f}));
        mCube.draw();
        col(sh, glm::vec3(1.0f, 0.8f, 0.2f)); // pico amarillo
        mdl(sh, TS(pos + glm::vec3(0, 0.06f, 0.28f), {0.04f, 0.04f, 0.08f}));
        mCube.draw();
    }

    // ── Helpers ────────────────────────────────────────────────────────────
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
        auto m = glm::translate(glm::mat4(1.f), t);
        m = glm::rotate(m, glm::radians(deg), glm::normalize(ax));
        return glm::scale(m, s);
    }

    // ── M1_IZQ: Iceberg derritiéndose ──────────────────────────────────────
    // animT=0: iceberg grande e imponente
    // animT=1: casi desaparecido
    void drawIceberg(Shader& sh, glm::vec3 c, float t) {
        float sx = glm::mix(3.5f, 0.5f, t);
        float sy = glm::mix(2.8f, 0.2f, t);

        if (mIcebergModel.loaded) {
            // Roca con nieve como iceberg — forzar color azul hielo
            glm::vec3 iceCol = glm::mix(glm::vec3(0.78f, 0.92f, 1.0f), glm::vec3(0.35f, 0.60f, 0.82f), t);
            float s = glm::mix(5.0f, 0.3f, t); // grande e imponente
            glm::mat4 m = glm::translate(glm::mat4(1.f), {c.x, c.y + 0.1f, c.z});
            m = glm::scale(m, glm::vec3(s * 2.5f, s * 2.0f, s * 2.2f));
            mdl(sh, m);
            mIcebergModel.draw(sh, &iceCol);
            return;
        }
        glm::vec3 iceCol = glm::mix(
            glm::vec3(0.78f, 0.92f, 1.00f),
            glm::vec3(0.35f, 0.60f, 0.82f), t);

        // Bloque principal del iceberg
        col(sh, iceCol);
        mdl(sh, TS({c.x, c.y + sy * 0.5f, c.z}, {sx, sy, sx * 0.85f}));
        mCube.draw();

        // Arista secundaria (más ancha y baja, parcialmente sumergida)
        col(sh, iceCol * 0.78f);
        mdl(sh, TS({c.x + 0.4f, c.y + 0.06f, c.z - 0.2f}, {sx * 1.6f, 0.13f, sx * 1.6f}));
        mCube.draw();

        // Trozo lateral flotante (aparece sólo antes de la mitad)
        if (t < 0.55f) {
            float lt = 1.0f - t / 0.55f;
            col(sh, iceCol * lt);
            mdl(sh, TS({c.x + sx * 0.9f, c.y + 0.3f * lt, c.z + 0.5f},
                        {sx * 0.4f * lt, 0.6f * lt, sx * 0.4f * lt}));
            mCube.draw();
        }
    }

    // ── M2_IZQ: Capa de hielo que encoge + oso polar encima ───────────────
    // animT=0: gran capa de hielo con oso estable
    // animT=1: capa casi inexistente, oso "perdido"
    void drawIceFloe(Shader& sh, glm::vec3 c, float t) {
        float iceR = glm::mix(4.8f, 0.25f, t);

        // Disco de hielo (procedural — siempre)
        col(sh, glm::vec3(0.88f, 0.95f, 1.0f));
        glm::mat4 dm = glm::scale(T({c.x, c.y + 0.08f, c.z}), {iceR, 1.f, iceR});
        mdl(sh, dm);
        mDisc.draw();

        // Oso polar (Quaternius Wolf CC0 — blanco crema para contraste con hielo)
        if (mBearModel.loaded) {
            glm::vec3 cream(0.92f, 0.90f, 0.85f); // crema cálido, distinto del hielo azulado
            glm::mat4 m = glm::translate(glm::mat4(1.f), {c.x + 1.0f, c.y + 0.15f, c.z});
            m = glm::scale(m, glm::vec3(35.0f)); // más grande para visibilidad
            mdl(sh, m);
            mBearModel.draw(sh, &cream);
        } else {
            // Fallback procedural
            col(sh, glm::vec3(0.96f, 0.97f, 1.00f));
            mdl(sh, TS({c.x, c.y + 0.55f, c.z}, {0.5f, 0.55f, 0.75f}));
            mCube.draw();
        }
    }

    // ── M3_IZQ: Edificios que se inundan ──────────────────────────────────
    // animT=0: ciudad normal, sin agua
    // animT=1: edificios casi cubiertos
    void drawFlood(Shader& sh, glm::vec3 c, float t) {
        float waterY = glm::mix(-0.6f, 3.2f, t);

        if (mBuildingA.loaded) {
            // Edificios Kenney (sin textura embebida → forzar colores de concreto)
            float offsets[][2] = {{-3.0f, -1.5f}, {0.0f, 2.0f}, {3.0f, -0.5f}};
            Model* bldModels[] = {&mBuildingA, &mBuildingB, &mBuildingC};
            glm::vec3 bldColors[] = {
                {0.72f, 0.68f, 0.62f}, // concreto cálido
                {0.65f, 0.62f, 0.58f}, // concreto oscuro
                {0.78f, 0.74f, 0.70f}, // concreto claro
            };
            for (int i = 0; i < 3; i++) {
                glm::mat4 m = glm::translate(glm::mat4(1.f),
                    {c.x + offsets[i][0], c.y, c.z + offsets[i][1]});
                m = glm::scale(m, glm::vec3(1.8f)); // escala grande para llenar la escena
                mdl(sh, m);
                bldModels[i]->draw(sh, &bldColors[i]);
            }
        } else {
            // Fallback procedural
            struct Bld { float ox, oz, h, w; };
            Bld blds[] = {
                {-1.4f, -0.6f, 3.8f, 0.9f},
                { 0.2f,  0.7f, 2.2f, 0.8f},
                { 1.5f, -0.2f, 5.0f, 0.75f},
            };
            glm::vec3 bldCol{0.62f, 0.56f, 0.50f};
            for (auto& b : blds) {
                col(sh, bldCol);
                mdl(sh, TS({c.x + b.ox, c.y + b.h * 0.5f, c.z + b.oz}, {b.w, b.h, b.w}));
                mCube.draw();
                col(sh, glm::vec3(0.95f, 0.85f, 0.4f));
                mdl(sh, TS({c.x + b.ox, c.y + b.h - 0.35f, c.z + b.oz + b.w * 0.51f},
                            {b.w * 0.65f, 0.09f, 0.04f}));
                mCube.draw();
            }
        }

        // Plano de agua (siempre procedural)
        if (waterY > -0.5f) {
            col(sh, glm::vec3(0.14f, 0.38f, 0.80f));
            mdl(sh, TS({c.x, c.y + waterY, c.z}, {7.0f, 0.16f, 7.0f}));
            mCube.draw();
        }
    }

    // ── M1_DER: Turbina eólica que acelera ────────────────────────────────
    // animT=0: turbina parada
    // animT=1: girando a velocidad máxima
    //
    // Estructura del OBJ (EolicOBJ.obj, Blender 2020):
    //   mesh 0 Cylinder          → pala 1
    //   mesh 1 Cylinder.001      → pala 2
    //   mesh 2 Cylinder.002      → pala 3
    //   mesh 3 Roundcube         → góndola
    //   mesh 4 Cube_Cube.002     → cono/hub
    //   mesh 5 Cylinder.003      → torre
    //   mesh 6-10                → suelo/vegetación (no se dibuja en el museo)
    //
    // Hub del rotor en espacio modelo: (0, 27.85, 1.3) — eje de rotación Z.
    void drawTurbine(Shader& sh, glm::vec3 c, float t, float time) {
        if (mEolicModel.loaded) {
            const float    sc  = 0.28f;                      // escala: torre ~27.5u → ~7.7m
            const glm::vec3 hub(0.f, 27.85f, 1.3f);          // hub en espacio modelo
            float speed = glm::mix(0.0f, 2.5f, t);           // rad/s (0 → ~24 RPM)
            float angle = speed * time;                       // radianes acumulados

            // Bajar la turbina para que no sobresalga del techo (Y=6.0).
            // Con c.y=1.7 y sc=0.28 la cima queda en ~9.4 → se baja 3.8u (base a ~-2.1).
            glm::vec3 c2 = {c.x, c.y - 3.8f, c.z};

            // Partes estáticas: góndola(3), cono(4), torre(5)
            glm::mat4 mSt = glm::scale(glm::translate(glm::mat4(1.f), c2), glm::vec3(sc));
            mdl(sh, mSt);
            for (int i = 3; i <= 5; i++)
                mEolicModel.drawMesh(i, sh);

            // Palas giratorias: meshes 0,1,2 — rotación alrededor del hub en espacio modelo
            glm::mat4 mBl = glm::translate(glm::mat4(1.f), c2);
            mBl = glm::scale(mBl, glm::vec3(sc));
            mBl = glm::translate(mBl,  hub);
            mBl = glm::rotate(mBl, angle, glm::vec3(0.f, 0.f, 1.f));
            mBl = glm::translate(mBl, -hub);
            mdl(sh, mBl);
            for (int i = 0; i <= 2; i++)
                mEolicModel.drawMesh(i, sh);

            return;
        }

        // Fallback procedural (si el modelo no cargó)
        col(sh, glm::vec3(0.65f, 0.66f, 0.68f));
        mdl(sh, TS({c.x, c.y + 0.15f, c.z}, {1.2f, 0.3f, 1.2f}));
        mCube.draw();
        col(sh, glm::vec3(0.88f, 0.89f, 0.91f));
        mdl(sh, TS({c.x, c.y + 2.8f, c.z}, {0.55f, 5.3f, 0.55f}));
        mCube.draw();
        col(sh, glm::vec3(0.92f, 0.92f, 0.94f));
        mdl(sh, TS({c.x, c.y + 5.5f, c.z - 0.3f}, {0.8f, 0.55f, 1.2f}));
        mCube.draw();
        float speed = glm::mix(0.0f, 80.0f, t);
        float angle = speed * time;
        glm::vec3 bladeCol{0.95f, 0.96f, 0.98f};
        for (int i = 0; i < 3; i++) {
            float a = angle + i * 120.0f;
            glm::mat4 m = glm::translate(glm::mat4(1.f), {c.x, c.y + 5.4f, c.z - 0.7f});
            m = glm::rotate(m, glm::radians(a), {0.f, 0.f, 1.f});
            m = glm::translate(m, {0.f, 1.6f, 0.f});
            m = glm::scale(m, {0.30f, 3.0f, 0.12f});
            col(sh, bladeCol);
            mdl(sh, m);
            mCube.draw();
        }
    }

    // ── M2_DER: Tesla Model S — azul, rota sobre Y al activar animación ──────
    // animT=0: coche estático en azul eléctrico
    // animT>0: rota sobre su propio eje Y a 45°/s máximo
    void drawElectricCar(Shader& sh, glm::vec3 c, float t, float time) {
        // Azul eléctrico fijo — no depende de animT
        glm::vec3 carCol(0.08f, 0.38f, 0.90f);

        if (mModelSCar.loaded) {
            // Rotación sobre Y: proporcional a animT → 0°/s en reposo, 45°/s a t=1
            float yAngle = glm::radians(45.0f * t * time);
            glm::mat4 m = glm::translate(glm::mat4(1.f), {c.x, c.y, c.z});
            m = glm::rotate(m, yAngle, {0.f, 1.f, 0.f});
            m = glm::scale(m, glm::vec3(1.0f));
            mdl(sh, m);
            // Omitir mesh 0: plano de suelo (Plane.030_Plane.018) que tapa el piso del museo
            for (int i = 1; i < (int)mModelSCar.meshes.size(); i++)
                mModelSCar.drawMesh(i, sh, &carCol);
            return;
        }

        // Fallback: modelo Kenney azul si ModelS no cargó
        float zOff = sinf(time * glm::pi<float>() / 3.0f) * 2.4f * t;
        if (mCarModel.loaded) {
            glm::mat4 m = glm::translate(glm::mat4(1.f), {c.x, c.y + 0.05f, c.z + zOff});
            m = glm::rotate(m, glm::radians(90.f), {0.f, 1.f, 0.f});
            m = glm::scale(m, glm::vec3(0.8f));
            mdl(sh, m);
            mCarModel.draw(sh, &carCol);
            return;
        }

        // Fallback procedural
        col(sh, carCol);
        mdl(sh, TS({c.x, c.y + 0.46f, c.z + zOff}, {1.35f, 0.58f, 2.5f}));
        mCube.draw();
        col(sh, carCol * 0.88f);
        mdl(sh, TS({c.x, c.y + 0.98f, c.z + zOff + 0.15f}, {1.05f, 0.42f, 1.4f}));
        mCube.draw();
        col(sh, glm::vec3(0.10f, 0.10f, 0.12f));
        float wx[] = {-0.78f,  0.78f, -0.78f,  0.78f};
        float wz[] = {-0.88f, -0.88f,  0.88f,  0.88f};
        for (int i = 0; i < 4; i++) {
            glm::mat4 wm = glm::translate(glm::mat4(1.f),
                {c.x + wx[i], c.y + 0.18f, c.z + zOff + wz[i]});
            wm = glm::rotate(wm, glm::radians(90.f), {1.f, 0.f, 0.f});
            wm = glm::scale(wm, {0.34f, 0.34f, 0.18f});
            mdl(sh, wm);
            mDisc.draw();
        }
    }

    // ── Humo de chimenea: discos horizontales que suben, se expanden y aclaran ──
    // origin: posición mundial de la boca de la chimenea
    // phase:  desfase para que las dos chimeneas no estén sincronizadas
    void drawSmoke(Shader& sh, glm::vec3 origin, float time, float phase) {
        const int N = 7;
        for (int i = 0; i < N; i++) {
            float frac = fmodf(time * 0.35f + phase + (float)i / N, 1.0f); // [0,1] ciclo
            float yOff = frac * 2.8f;                   // sube 2.8 unidades
            float r    = 0.07f + frac * 0.50f;          // radio crece de 0.07 a 0.57
            float g    = glm::mix(0.22f, 0.80f, frac);  // gris oscuro → gris claro
            col(sh, {g, g, g});
            glm::mat4 m = glm::scale(T({origin.x, origin.y + yOff, origin.z}),
                                     {r, 0.09f, r});
            mdl(sh, m);
            mDisc.draw();
        }
    }

    // ── M3_DER: Fábrica con humo + árbol que crece ────────────────────────
    // La fábrica y su humo están siempre activos.
    // El árbol crece de animT=0 (semilla) a animT=1 (árbol completo).
    //
    // Chimeneas en espacio modelo → mundo (offset centrado, sc=0.35):
    //   Chimenea 1  (-0.06, 7.02, -3.51) → c + (-0.87, 2.70, -0.77)
    //   Chimenea 2  (-2.22, 7.14, -3.55) → c + (-1.62, 2.74, -0.78)
    void drawTree(Shader& sh, glm::vec3 c, float t, float time) {
        // Fábrica — siempre visible, centrada en su bounding box.
        // BB modelo: X[-6.56,11.39] Y[-0.69,7.14] Z[-10.29,7.66]
        // Offset de centrado: (-2.42, 0.69, 1.31) para poner base en Y=0 y centrar XZ.
        if (mFactoryModel.loaded) {
            const float sc = 0.35f;
            glm::mat4 fm = glm::translate(glm::mat4(1.f), {c.x, c.y, c.z});
            fm = glm::scale(fm, glm::vec3(sc));
            fm = glm::translate(fm, {-2.42f, 0.69f, 1.31f});
            mdl(sh, fm);
            mFactoryModel.draw(sh); // usa colores del Factory.mtl
        }

        // Humo de las dos chimeneas (siempre activo)
        glm::vec3 chimney1 = {c.x - 0.87f, c.y + 2.70f, c.z - 0.77f};
        glm::vec3 chimney2 = {c.x - 1.62f, c.y + 2.74f, c.z - 0.78f};
        drawSmoke(sh, chimney1, time, 0.0f);
        drawSmoke(sh, chimney2, time, 0.45f);

        // Árbol que crece
        float s = glm::mix(0.02f, 1.0f, t);

        if (mPineModel.loaded) {
            glm::mat4 m = glm::translate(glm::mat4(1.f), {c.x, c.y, c.z});
            m = glm::scale(m, glm::vec3(s * 2.5f));
            mdl(sh, m);
            mPineModel.draw(sh);
            return;
        }
        // Fallback procedural
        col(sh, glm::vec3(0.42f, 0.28f, 0.13f));
        mdl(sh, TS({c.x, c.y + 1.7f * s, c.z}, {0.24f * s, 3.4f * s, 0.24f * s}));
        mCube.draw();
        float layH[] = {3.6f, 2.7f, 1.9f};
        float layR[] = {1.2f, 1.7f, 2.2f};
        for (int i = 0; i < 3; i++) {
            float shade = 1.0f - i * 0.07f;
            col(sh, glm::vec3(0.18f, 0.65f, 0.22f) * shade);
            glm::mat4 m = glm::scale(T({c.x, c.y + layH[i] * s, c.z}),
                                      {layR[i] * s, 1.f, layR[i] * s});
            mdl(sh, m);
            mDisc.draw();
        }
    }

    // ── M5: Globo terráqueo girando + líneas de acuerdos ──────────────────
    // Rotación siempre activa; líneas aparecen secuencialmente con animT
    void drawGlobe(Shader& sh, glm::vec3 c, float t, float time) {
        float rot = time * 18.0f; // 18°/s constante

        if (mGlobeModel.loaded) {
            // DamagedHelmet como símbolo del planeta dañado
            glm::mat4 gm = glm::translate(glm::mat4(1.f), {c.x, c.y + 2.5f, c.z});
            gm = glm::rotate(gm, glm::radians(rot), {0.f, 1.f, 0.f});
            gm = glm::scale(gm, glm::vec3(1.0f)); // más modesto para no abrumar
            mdl(sh, gm);
            mGlobeModel.draw(sh);
        } else {
            // Fallback: cubos como globo
            col(sh, glm::vec3(0.18f, 0.48f, 0.82f));
            glm::mat4 gm = glm::rotate(T({c.x, c.y + 3.2f, c.z}),
                                        glm::radians(rot), {0.f, 1.f, 0.f});
            gm = glm::scale(gm, {2.6f, 2.6f, 2.6f});
            mdl(sh, gm);
            mCube.draw();

            col(sh, glm::vec3(0.25f, 0.60f, 0.26f));
            glm::mat4 cm = glm::rotate(T({c.x, c.y + 3.2f, c.z}),
                                        glm::radians(rot + 55.f), {0.f, 1.f, 0.f});
            cm = glm::scale(cm, {2.66f, 1.2f, 1.5f});
            mdl(sh, cm);
            mCube.draw();
        }

        // Líneas de acuerdos: discos que aparecen uno a uno con animT
        if (t > 0.05f) {
            const int nLines = 6;
            for (int i = 0; i < nLines; i++) {
                float window  = 1.0f / nLines;
                float lineT   = glm::clamp((t - i * window) / window, 0.f, 1.f);
                if (lineT < 0.01f) continue;

                float a = i * (360.f / nLines) + rot * 0.28f;
                float r = 4.2f;
                glm::vec3 lCol = glm::mix(
                    glm::vec3(1.0f, 0.85f, 0.1f),
                    glm::vec3(1.0f, 0.40f, 0.0f),
                    (float)i / (nLines - 1));

                col(sh, lCol * lineT);
                glm::mat4 lm = T({c.x + cosf(glm::radians(a)) * r,
                                   c.y + 1.5f + i * 0.3f,
                                   c.z + sinf(glm::radians(a)) * r});
                lm = glm::rotate(lm, glm::radians(90.f), {1.f, 0.f, 0.f});
                lm = glm::scale(lm, {lineT * 0.75f, lineT * 0.75f, 0.04f});
                mdl(sh, lm);
                mDisc.draw();
            }
        }
    }
};
