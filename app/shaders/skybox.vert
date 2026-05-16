#version 330 core
// skybox.vert — Fase 7A: vertex shader del skybox cubemap
// El truco xyww hace que el fragmento tenga z_NDC = 1.0 (profundidad máxima)
// para que la geometría del skybox quede detrás de todo lo demás.

layout(location = 0) in vec3 aPos;

out vec3 TexCoords; // dirección del rayo hacia el texel de la cubemap

uniform mat4 view;        // sin la componente de traslación (solo rotación)
uniform mat4 projection;

void main() {
    TexCoords   = aPos;
    vec4 pos    = projection * view * vec4(aPos, 1.0);
    // Profundidad máxima: z/w = 1.0 (detrás de todo el museo)
    gl_Position = pos.xyww;
}
