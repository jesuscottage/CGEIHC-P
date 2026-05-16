#version 330 core
// skybox.frag — Fase 7A: fragment shader del skybox cubemap

in vec3 TexCoords; // dirección del rayo hacia el texel de la cubemap

out vec4 FragColor;

uniform samplerCube skybox;

void main() {
    FragColor = texture(skybox, TexCoords);
}
