#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "core/Input.h"

class CameraFPS {
public:
    glm::vec3 position{0.0f, 1.7f, 0.0f}; // altura de ojos: 1.7 m
    float yaw   = -90.0f; // mira hacia -Z por defecto
    float pitch = 0.0f;

    // Configuración
    float moveSpeed       = 4.0f;   // m/s
    float mouseSensitivity = 0.1f;
    float fov             = 60.0f;  // grados
    float nearPlane       = 0.1f;
    float farPlane        = 200.0f;

    void processInput(Input& input, float dt) {
        glm::vec3 front = getFront();
        glm::vec3 right = glm::normalize(glm::cross(front, worldUp));

        if (input.isPressed(GLFW_KEY_W)) position += front * moveSpeed * dt;
        if (input.isPressed(GLFW_KEY_S)) position -= front * moveSpeed * dt;
        if (input.isPressed(GLFW_KEY_A)) position -= right * moveSpeed * dt;
        if (input.isPressed(GLFW_KEY_D)) position += right * moveSpeed * dt;

        // Y siempre fija a 1.7 m (no hay gravedad)
        position.y = 1.7f;

        // Mouse
        glm::vec2 delta = input.getMouseDelta();
        yaw   += delta.x * mouseSensitivity;
        pitch += delta.y * mouseSensitivity;
        pitch  = std::clamp(pitch, -80.0f, 80.0f);
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + getFront(), worldUp);
    }

    glm::mat4 getProjectionMatrix(float aspect) const {
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }

    // Mantener la cámara dentro de los límites AABB del museo
    void clampPosition(float minX, float maxX, float minZ, float maxZ) {
        position.x = std::clamp(position.x, minX, maxX);
        position.z = std::clamp(position.z, minZ, maxZ);
    }

    glm::vec3 getFront() const {
        return glm::normalize(glm::vec3{
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        });
    }

private:
    glm::vec3 worldUp{0.0f, 1.0f, 0.0f};
};
