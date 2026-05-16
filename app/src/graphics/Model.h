#pragma once
#include "glad/glad.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <iostream>

// Un submesh de modelo con textura propia
struct ModelMesh {
    Mesh     mesh;
    Texture  diffuse;
    glm::vec3 baseColor{1.0f};
    bool     hasTexture = false;
};

class Model {
public:
    std::vector<ModelMesh> meshes;
    std::string directory;
    bool loaded = false;

    bool load(const std::string& path) {
        Assimp::Importer importer;
        unsigned int flags = aiProcess_Triangulate
                           | aiProcess_FlipUVs
                           | aiProcess_GenSmoothNormals
                           | aiProcess_CalcTangentSpace;

        // Intentar carga directa primero
        const aiScene* scene = importer.ReadFile(path, flags);

        // Si falla (ej: paths Unicode en Windows), cargar desde memoria
        std::vector<char> fileData;
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            FILE* fp = fopen(path.c_str(), "rb");
            if (!fp) {
                std::cerr << "[Model] No se pudo abrir: " << path << "\n";
                return false;
            }
            fseek(fp, 0, SEEK_END);
            long sz = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            fileData.resize(sz);
            fread(fileData.data(), 1, sz, fp);
            fclose(fp);

            // Determinar hint de formato por extensión
            std::string ext = path.substr(path.find_last_of('.'));
            scene = importer.ReadFileFromMemory(fileData.data(), sz, flags, ext.c_str());
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                std::cerr << "[Model] Error Assimp (memoria): " << importer.GetErrorString() << "\n";
                return false;
            }
        }
        directory = path.substr(0, path.find_last_of("/\\"));
        processNode(scene->mRootNode, scene);
        loaded = true;
        std::cout << "[Model] Cargado: " << path << " (" << meshes.size() << " submeshes)\n";
        return true;
    }

    // Dibujar con el shader ya activo (caller setea model matrix)
    // Si overrideColor != nullptr, forzar ese color en vez del material del modelo
    void draw(Shader& shader, const glm::vec3* overrideColor = nullptr) const {
        for (const auto& mm : meshes) {
            if (overrideColor) {
                shader.setBool("useTexture", false);
                shader.setVec3("baseColor", *overrideColor);
            } else if (mm.hasTexture) {
                mm.diffuse.bind(0);
                shader.setInt("diffuseMap", 0);
                shader.setBool("useTexture", true);
            } else {
                shader.setBool("useTexture", false);
                shader.setVec3("baseColor", mm.baseColor);
            }
            mm.mesh.draw();
        }
    }

    void free() {
        for (auto& mm : meshes) {
            mm.mesh.free();
            mm.diffuse.free();
        }
        meshes.clear();
    }

private:
    void processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
            processMesh(scene->mMeshes[node->mMeshes[i]], scene);
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene);
    }

    void processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex v;
            v.position  = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            v.normal    = mesh->HasNormals()
                        ? glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z}
                        : glm::vec3{0,1,0};
            v.texCoords = mesh->mTextureCoords[0]
                        ? glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}
                        : glm::vec2{0,0};
            vertices.push_back(v);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            auto& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        ModelMesh mm;
        mm.mesh.setup(vertices, indices);

        // Textura difusa del material
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

            // Color base del material
            aiColor3D color(1.f,1.f,1.f);
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            mm.baseColor = {color.r, color.g, color.b};

            // Textura difusa
            if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString texPath;
                mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);

                // GLTF puede tener texturas embebidas
                const aiTexture* embedded = scene->GetEmbeddedTexture(texPath.C_Str());
                if (embedded) {
                    // Textura embebida: cargar desde memoria
                    int w, h, nch;
                    stbi_set_flip_vertically_on_load(0);
                    unsigned char* data = stbi_load_from_memory(
                        (unsigned char*)embedded->pcData,
                        embedded->mWidth, &w, &h, &nch, 0);
                    if (data) {
                        GLenum fmt = (nch == 4) ? GL_RGBA : GL_RGB;
                        glGenTextures(1, &mm.diffuse.id);
                        glBindTexture(GL_TEXTURE_2D, mm.diffuse.id);
                        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
                        glGenerateMipmap(GL_TEXTURE_2D);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        stbi_image_free(data);
                        mm.hasTexture = true;
                        mm.diffuse.width = w; mm.diffuse.height = h;
                    }
                } else {
                    // Textura en archivo externo
                    std::string fullPath = directory + "/" + texPath.C_Str();
                    if (mm.diffuse.load(fullPath, false))
                        mm.hasTexture = true;
                }
            }
        }

        if (!mm.hasTexture) {
            mm.diffuse.loadWhite();
        }

        meshes.push_back(std::move(mm));
    }
};
