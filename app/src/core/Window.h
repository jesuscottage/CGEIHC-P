#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

static void APIENTRY glDebugCallback(GLenum, GLenum, GLuint id,
    GLenum severity, GLsizei, const GLchar* msg, const void*)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
    const char* sev = (severity == GL_DEBUG_SEVERITY_HIGH)   ? "ALTO"
                    : (severity == GL_DEBUG_SEVERITY_MEDIUM) ? "MEDIO" : "BAJO";
    std::cerr << "[OpenGL " << sev << "] id=" << id << " " << msg << "\n";
}

// Callback global (no usa user pointer para evitar conflicto con Input)
static void framebufferResizeCallback(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

class Window {
public:
    GLFWwindow* handle = nullptr;

    bool init(int w, int h, const std::string& title) {
        if (!glfwInit()) { std::cerr << "GLFW init falló\n"; return false; }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        handle = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
        if (!handle) { std::cerr << "No se pudo crear ventana\n"; glfwTerminate(); return false; }
        glfwMakeContextCurrent(handle);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "GLAD init falló\n"; return false;
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugCallback, nullptr);

        glfwSetFramebufferSizeCallback(handle, framebufferResizeCallback);

        std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
        std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
        return true;
    }

    int width()  const { int w, h; glfwGetFramebufferSize(handle, &w, &h); return w; }
    int height() const { int w, h; glfwGetFramebufferSize(handle, &w, &h); return h; }
    float aspectRatio() const { return (float)width() / (float)height(); }

    bool shouldClose() const { return glfwWindowShouldClose(handle); }
    void swapBuffers()  const { glfwSwapBuffers(handle); }
    void pollEvents()   const { glfwPollEvents(); }
    void close()              { glfwSetWindowShouldClose(handle, GLFW_TRUE); }

    void terminate() {
        if (handle) { glfwDestroyWindow(handle); handle = nullptr; }
        glfwTerminate();
    }
};
