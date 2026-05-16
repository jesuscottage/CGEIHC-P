#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform bool      useTexture;
uniform vec3      baseColor;   // color plano si no hay textura

// Luz direccional (sol ártico bajo en el horizonte)
uniform vec3 lightDir;    // dirección normalizada hacia la luz
uniform vec3 lightColor;
uniform vec3 viewPos;

// Control de niebla
uniform float fogDensity;
uniform vec3  fogColor;

void main() {
    // ── Albedo ────────────────────────────────────
    vec3 albedo = useTexture ? texture(diffuseMap, TexCoords).rgb : baseColor;

    // ── Blinn-Phong básico ─────────────────────────
    vec3 norm     = normalize(Normal);
    vec3 ldir     = normalize(-lightDir); // hacia la luz
    vec3 viewDir  = normalize(viewPos - FragPos);
    vec3 halfDir  = normalize(ldir + viewDir);

    float ambient  = 0.35;
    // Luz de rebote del suelo ártico: superficies que miran hacia abajo
    // reciben luminosidad reflejada del hielo
    float bounce   = 0.18 * max(dot(norm, vec3(0.0, -1.0, 0.0)), 0.0);
    float diff     = max(dot(norm, ldir), 0.0);
    float spec     = pow(max(dot(norm, halfDir), 0.0), 32.0);

    vec3 color = albedo * ((ambient + bounce) + diff * lightColor)
               + lightColor * spec * 0.3;

    // ── Niebla exponencial cuadrática ──────────────
    float fogDist   = length(viewPos - FragPos);
    float fogFactor = 1.0 - clamp(exp(-pow(fogDensity * fogDist, 2.0)), 0.0, 1.0);
    color = mix(color, fogColor, fogFactor);

    FragColor = vec4(color, 1.0);
}
