#version 330 core
// water.vert — Fase 7B: plano de agua con olas sinusoidales
// Basado en los materiales del curso: 13_wavesAnimation-fresnel.vs

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float WaveHeight; // para cálculo de espuma en el frag shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;       // glfwGetTime()
uniform float waveAmp;    // amplitud de las olas (metros)
uniform float waveSpeed;  // velocidad de propagación
uniform float waveFreq;   // frecuencia espacial (radians/metro)

void main() {
    vec3 pos = aPos;

    // Ola principal: dirección X
    float w1 = waveAmp * sin(pos.x * waveFreq       + time * waveSpeed);
    // Ola secundaria: dirección Z + desfase para aspecto cruzado
    float w2 = waveAmp * 0.6 * sin(pos.z * waveFreq * 0.7 + time * waveSpeed * 1.3 + 1.2);
    // Rizo de alta frecuencia muy pequeño
    float w3 = waveAmp * 0.2 * sin((pos.x + pos.z) * waveFreq * 2.5 + time * waveSpeed * 0.9);

    pos.y += w1 + w2 + w3;
    WaveHeight = w1 + w2 + w3; // relativo a la media (0)

    // Normal aproximada diferenciando el campo de alturas
    float eps = 0.3;
    float dx = waveAmp * waveFreq * cos(pos.x * waveFreq + time * waveSpeed)
             + waveAmp * 0.6 * waveFreq * 0.7 * cos((aPos.z) * waveFreq * 0.7 + time * waveSpeed * 1.3 + 1.2) * 0.0
             + waveAmp * 0.2 * (waveFreq * 2.5) * cos((aPos.x + aPos.z) * waveFreq * 2.5 + time * waveSpeed * 0.9);
    float dz = waveAmp * 0.6 * (waveFreq * 0.7) * cos(aPos.z * waveFreq * 0.7 + time * waveSpeed * 1.3 + 1.2)
             + waveAmp * 0.2 * (waveFreq * 2.5) * cos((aPos.x + aPos.z) * waveFreq * 2.5 + time * waveSpeed * 0.9);
    vec3 waveNormal = normalize(vec3(-dx, 1.0, -dz));

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    Normal    = normalMatrix * waveNormal;
    FragPos   = vec3(model * vec4(pos, 1.0));
    TexCoords = aTexCoords + vec2(time * 0.015, time * 0.008); // scroll UV

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
