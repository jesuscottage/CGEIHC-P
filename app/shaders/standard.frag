#version 330 core
// standard.frag — Fase 6: Blinn-Phong completo + Fresnel (Schlick) + luz hemisférica

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform bool      useTexture;
uniform vec3      baseColor;

// Luz principal — sol ártico bajo en el horizonte
uniform vec3  lightDir;    // dirección DE la luz (apunta desde la escena hacia el sol)
uniform vec3  lightColor;
uniform vec3  viewPos;

// Parámetros de material
uniform float shininess;   // brillo especular (16–256)
uniform vec3  fresnel0;    // reflectancia en incidencia normal (0.04 para no-metales)

// Niebla
uniform float fogDensity;
uniform vec3  fogColor;

// Fresnel de Schlick: reflectancia en ángulos rasantes
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    vec3 albedo  = useTexture ? texture(diffuseMap, TexCoords).rgb : baseColor;
    vec3 norm    = normalize(Normal);
    vec3 ldir    = normalize(-lightDir); // vector hacia la luz
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(ldir + viewDir);

    // ── Luz hemisférica de ambiente ───────────────────────────────────────
    // Simula cielo ártico (azul frío arriba) + albedo del hielo reflejado (abajo).
    // hemi=1 para superficies mirando al cielo, hemi=0 mirando al suelo.
    float hemi      = 0.5 + 0.5 * dot(norm, vec3(0.0, 1.0, 0.0));
    vec3 skyColor   = vec3(0.42f, 0.52f, 0.68f); // azul ártico
    vec3 groundColor= vec3(0.52f, 0.58f, 0.62f); // blanco-gris del hielo
    vec3 ambLight   = mix(groundColor, skyColor, hemi) * 0.62;

    // ── Difuso Blinn-Phong ────────────────────────────────────────────────
    float NdotL  = max(dot(norm, ldir), 0.0);
    vec3  diffuse = NdotL * lightColor;

    // ── Especular Blinn-Phong + Fresnel ───────────────────────────────────
    float NdotH  = max(dot(norm, halfDir), 0.0);
    float spec   = pow(NdotH, shininess);
    float VdotH  = max(dot(viewDir, halfDir), 0.0);
    vec3  fresnelTerm = fresnelSchlick(VdotH, fresnel0);

    // ── Combinación ────────────────────────────────────────────────────────
    vec3 color = albedo  * (ambLight + diffuse)
               + fresnelTerm * spec * lightColor * 0.55;

    // ── Niebla exponencial cuadrática ────────────────────────────────────
    float fogDist   = length(viewPos - FragPos);
    float fogFactor = 1.0 - clamp(exp(-pow(fogDensity * fogDist, 2.0)), 0.0, 1.0);
    color = mix(color, fogColor, fogFactor);

    FragColor = vec4(color, 1.0);
}
