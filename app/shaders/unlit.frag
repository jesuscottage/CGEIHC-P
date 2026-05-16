#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform bool      useTexture;
uniform vec3      baseColor;
uniform float     alpha;

void main() {
    vec4 color = useTexture ? texture(diffuseMap, TexCoords) : vec4(baseColor, alpha);
    if (color.a < 0.05) discard;
    FragColor = color;
}
