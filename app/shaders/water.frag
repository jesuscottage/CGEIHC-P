#version 330 core
// water.frag — Fase 7B: agua ártica con Fresnel + espuma + niebla

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float WaveHeight;

out vec4 FragColor;

uniform vec3  viewPos;
uniform vec3  lightDir;
uniform vec3  lightColor;
uniform float fogDensity;
uniform vec3  fogColor;
uniform float waveAmp; // para normalizar la espuma

// Color del agua ártica: azul-verde oscuro casi opaco
const vec3 WATER_DEEP    = vec3(0.04, 0.16, 0.32); // profundo
const vec3 WATER_SHALLOW = vec3(0.18, 0.42, 0.58); // superficial / cresta

// Espuma: blanco cuando la cresta supera un umbral
const vec3 FOAM_COLOR    = vec3(0.88, 0.92, 0.96);
const float FOAM_THRESH  = 0.6; // fracción de waveAmp para aparecer espuma

// Fresnel de Schlick
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    vec3 norm    = normalize(Normal);
    vec3 ldir    = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(ldir + viewDir);

    // Mezcla de color profundo/superficial según WaveHeight
    float heightFrac = clamp((WaveHeight / waveAmp + 1.0) * 0.5, 0.0, 1.0);
    vec3 waterColor  = mix(WATER_DEEP, WATER_SHALLOW, heightFrac);

    // Espuma en las crestas
    float foamFrac = clamp((WaveHeight - FOAM_THRESH * waveAmp) / (waveAmp * 0.4), 0.0, 1.0);
    waterColor = mix(waterColor, FOAM_COLOR, foamFrac * foamFrac);

    // Luz hemisférica mínima (cielo → suelo)
    float hemi     = 0.5 + 0.5 * dot(norm, vec3(0.0, 1.0, 0.0));
    vec3  ambLight = mix(vec3(0.22, 0.30, 0.38), vec3(0.35, 0.52, 0.72), hemi) * 0.55;

    // Difuso
    float NdotL  = max(dot(norm, ldir), 0.0);
    vec3  diffuse = NdotL * lightColor * 0.7;

    // Especular + Fresnel (agua = F0 ≈ 0.02-0.04)
    float NdotH  = max(dot(norm, halfDir), 0.0);
    float spec   = pow(NdotH, 128.0);
    float VdotH  = max(dot(viewDir, halfDir), 0.0);
    vec3  F0     = vec3(0.02, 0.025, 0.03);
    vec3  fresnelTerm = fresnelSchlick(VdotH, F0);

    // Fresnel de vista: a ángulos rasantes el agua se vuelve más reflectante
    float NdotV      = max(dot(norm, viewDir), 0.0);
    float viewFresnel = 1.0 - NdotV;
    viewFresnel = viewFresnel * viewFresnel * viewFresnel * viewFresnel; // ^4

    // Reflejo del cielo en la superficie (color del horizonte del skybox)
    vec3 skyReflect = mix(vec3(0.62, 0.78, 0.92), vec3(0.10, 0.22, 0.55), NdotV);

    // Combinación final
    vec3 color = waterColor * (ambLight + diffuse)
               + fresnelTerm  * spec  * lightColor * 0.9
               + skyReflect   * viewFresnel * 0.35;

    // Niebla exponencial cuadrática
    float fogDist   = length(viewPos - FragPos);
    float fogFactor = 1.0 - clamp(exp(-pow(fogDensity * fogDist, 2.0)), 0.0, 1.0);
    color = mix(color, fogColor, fogFactor);

    // Transparencia leve: alpha 0.88 (agua semitransparente)
    FragColor = vec4(color, 0.88);
}
