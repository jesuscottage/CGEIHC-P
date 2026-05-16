#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>

// Variables globales para el callback del cursor (evita conflicto de user pointer con Window)
static float g_mouseLastX = 640.0f;
static float g_mouseLastY = 360.0f;
static glm::vec2 g_mouseDeltaAccum{0.0f};
static bool g_mouseFirstMove = true;

static void cursorPosCallback(GLFWwindow*, double x, double y) {
    float fx = (float)x, fy = (float)y;
    if (g_mouseFirstMove) {
        g_mouseLastX = fx; g_mouseLastY = fy;
        g_mouseFirstMove = false;
    }
    g_mouseDeltaAccum.x += fx - g_mouseLastX;
    g_mouseDeltaAccum.y  = g_mouseLastY - fy; // Y invertida: arriba = positivo
    g_mouseLastX = fx; g_mouseLastY = fy;
}

class Input {
public:
    void init(GLFWwindow* window) {
        win = window;
        glfwSetCursorPosCallback(window, cursorPosCallback);
    }

    // Llamar al FINAL de cada frame
    void update() {
        prevKeys = currKeys;
        currKeys.clear();
        static const int KEYS[] = {
            GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D,
            GLFW_KEY_E, GLFW_KEY_ESCAPE, GLFW_KEY_F12,
            GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_LEFT_SHIFT
        };
        for (int k : KEYS)
            currKeys[k] = (glfwGetKey(win, k) == GLFW_PRESS);

        // Capturar delta y resetear acumulador
        frameDelta = g_mouseDeltaAccum;
        g_mouseDeltaAccum = glm::vec2(0.0f);
    }

    bool isPressed(int key)     const {
        auto it = currKeys.find(key);
        return it != currKeys.end() && it->second;
    }
    bool isJustPressed(int key) const {
        auto c = currKeys.find(key), p = prevKeys.find(key);
        bool cur = (c != currKeys.end() && c->second);
        bool pre = (p != prevKeys.end() && p->second);
        return cur && !pre;
    }

    glm::vec2 getMouseDelta() const { return frameDelta; }

    void captureMouse() { glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED); g_mouseFirstMove = true; }
    void releaseMouse() { glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }

private:
    GLFWwindow* win = nullptr;
    std::unordered_map<int, bool> currKeys, prevKeys;
    glm::vec2 frameDelta{0.0f};
};
