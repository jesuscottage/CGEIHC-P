#pragma once
#include <GLFW/glfw3.h>
#include <algorithm>

class Time {
public:
    void update() {
        double now = glfwGetTime();
        double raw = now - lastTime;
        lastTime = now;
        // Clampear dt para evitar spikes (ej. al mover la ventana)
        dt = (float)std::clamp(raw, 0.0001, 0.05);
        elapsed = (float)now;
    }

    float getDeltaTime() const { return dt; }
    float getTime()      const { return elapsed; }

private:
    double lastTime = 0.0;
    float  dt       = 0.016f;
    float  elapsed  = 0.0f;
};
