#pragma once
#include "glad/glad.h"
#include "stb_image.h"
#include <string>
#include <iostream>

class Texture {
public:
    unsigned int id = 0;
    int width = 0, height = 0;

    // Cargar desde archivo (stb_image)
    bool load(const std::string& path, bool flip = true) {
        stbi_set_flip_vertically_on_load(flip ? 1 : 0);
        int nch;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nch, 0);
        if (!data) {
            std::cerr << "[Texture] No se pudo cargar: " << path << "\n";
            return false;
        }
        GLenum fmt = (nch == 4) ? GL_RGBA : (nch == 3) ? GL_RGB : GL_RED;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
        return true;
    }

    // Textura blanca 1x1 (placeholder cuando no hay textura)
    void loadWhite() {
        unsigned char px[4] = {255, 255, 255, 255};
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void bind(unsigned int slot = 0) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void free() {
        if (id) { glDeleteTextures(1, &id); id = 0; }
    }
};
