#version 410 core

in vec3 fPosition;
in vec2 fTexCoords;
in vec3 fNormal;

out vec4 fColor;

// texture
uniform sampler2D diffuseTexture;

//lighting
uniform vec3 waveLightDir;
uniform vec3 waveLightColor;
uniform sampler2D shadowMap;

in vec4 fragPosLightSpace;
in vec4 fragPosWorld;

//components
vec3 ambient;
float ambientStrength = 0.1f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(fNormal);

    //normalize light direction
    vec3 lightDirN = normalize(waveLightDir);

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * waveLightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * waveLightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    specular = specularStrength * specCoeff * waveLightColor;
}

float computeShadow()
{
    // perform perspective divide
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;

    // Get depth of current fragment from light's perspective
    float currentDepth = normalizedCoords.z;

    // Check whether current frag pos is in shadow
    float bias = 0.005f;
    float shadow = (currentDepth - bias) > closestDepth ? 1.0f : 0.0f;

    if (normalizedCoords.z > 1.0f)
    return 0.0f;

    return shadow;
}

float computeFog()
{
    float fogDensity = 0.3f;

    float fragmentDistance = length(fragPosLightSpace);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

    return clamp(fogFactor, 0.0f, 1.0f);
}

void main()
{
    computeDirLight();

    float shadow = computeShadow();

    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;

    //compute final vertex color
    vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);

    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.0f, 0.0f, 0.2f, 1.0f);

    fColor = fogColor * (1 - fogFactor) + vec4(color, 1.0f) * fogFactor;

}
