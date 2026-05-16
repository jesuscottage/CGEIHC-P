#pragma once
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int id = 0;

    bool load(const std::string& vertPath, const std::string& fragPath) {
        std::string vert = readFile(vertPath);
        std::string frag = readFile(fragPath);
        if (vert.empty() || frag.empty()) {
            std::cerr << "[Shader] No se pudo leer: " << vertPath << " / " << fragPath << "\n";
            return false;
        }
        unsigned int vs = compile(GL_VERTEX_SHADER,   vert.c_str());
        unsigned int fs = compile(GL_FRAGMENT_SHADER, frag.c_str());
        if (!vs || !fs) return false;

        id = glCreateProgram();
        glAttachShader(id, vs);
        glAttachShader(id, fs);
        glLinkProgram(id);

        int ok; char log[512];
        glGetProgramiv(id, GL_LINK_STATUS, &ok);
        if (!ok) {
            glGetProgramInfoLog(id, 512, nullptr, log);
            std::cerr << "[Shader] Link error: " << log << "\n";
            id = 0; return false;
        }
        glDeleteShader(vs);
        glDeleteShader(fs);
        return true;
    }

    void use()  const { glUseProgram(id); }
    void unuse()const { glUseProgram(0);  }

    void setBool (const std::string& n, bool  v)         const { glUniform1i (loc(n), (int)v); }
    void setInt  (const std::string& n, int   v)         const { glUniform1i (loc(n), v);       }
    void setFloat(const std::string& n, float v)         const { glUniform1f (loc(n), v);       }
    void setVec2 (const std::string& n, const glm::vec2& v)const { glUniform2fv(loc(n), 1, glm::value_ptr(v)); }
    void setVec3 (const std::string& n, const glm::vec3& v)const { glUniform3fv(loc(n), 1, glm::value_ptr(v)); }
    void setMat4 (const std::string& n, const glm::mat4& v)const { glUniformMatrix4fv(loc(n), 1, GL_FALSE, glm::value_ptr(v)); }

private:
    GLint loc(const std::string& n) const { return glGetUniformLocation(id, n.c_str()); }

    static std::string readFile(const std::string& path) {
        std::ifstream f(path);
        if (!f.is_open()) return "";
        std::stringstream ss; ss << f.rdbuf();
        return ss.str();
    }

    static unsigned int compile(GLenum type, const char* src) {
        unsigned int s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        int ok; char log[512];
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            glGetShaderInfoLog(s, 512, nullptr, log);
            std::cerr << "[Shader] Compile error (" << (type == GL_VERTEX_SHADER ? "vert" : "frag") << "):\n" << log << "\n";
            return 0;
        }
        return s;
    }
};
