#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;
in vec4 fragPosWorld;

out vec4 fColor;

//lighting
uniform    vec3 lightDir;
uniform    vec3 lightColor;

//texture
uniform sampler2D shadowMap;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform vec3 cameraPos;

vec3 ambient;
float ambientStrength = 0.5f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;
float shadow;

vec3 buoyLightEye = vec3(-50.0f, 0.0f, 55.0f);
uniform vec3 buoyLightColor;
vec3 buoyLightDirection = vec3(0.0f, 0.0f, 1.0f);

void computeLightComponents()
{
    vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin

    //transform normal
    vec3 normalEye = normalize(fNormal);

    //compute light direction
    vec3 lightDirN = normalize(lightDir);

    //compute view direction
    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflection = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
    specular = specularStrength * specCoeff * lightColor;
}

vec3 computeLampLight(vec3 lampPosition, vec3 direction, float outerCutOffAngle, float lampLinear, float lampQuadratic, vec3 lightPosColor)
{
    float shininessLamp = 15.0f;
    float constant = 1.0f;

    float cutOff = cos(radians(180.5f));
    float outerCutOff = cos(radians(outerCutOffAngle));

    vec3 viewPos = vec3(0.0f);
    vec3 viewDir = normalize(viewPos - fragPosWorld.xyz);
    vec3 lightDirLampLocal = normalize(lampPosition - fragPosWorld.xyz);

    // Diffuse shading
    float diff = max(dot(fNormal, lightDirLampLocal), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDirLampLocal, fNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessLamp);

    // Attenuation
    float distance = length(lampPosition - fragPosWorld.xyz);
    float attenuation = 1.0f / (constant + lampLinear * distance + lampQuadratic * (distance * distance));

    // Spotlight intensity
    float theta = dot(lightDirLampLocal, normalize(-direction));
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    // Combine results
    vec3 ambientLamp = (texture(diffuseTexture, fTexCoords).rgb) * lightPosColor;
    vec3 diffuseLamp = diff * vec3(texture(diffuseTexture, fTexCoords)) * lightPosColor;
    vec3 specularLamp = spec * vec3(texture(specularTexture, fTexCoords)) * lightPosColor;
    ambientLamp *= attenuation; //* intensity;
    diffuseLamp *= attenuation; //* intensity;
    specularLamp *= attenuation; //* intensity;

    return (ambientLamp + diffuseLamp + specularLamp);

}

float computeFog()
{
    float fogDensity = 0.05f;

    if(cameraPos.y < -9.0) {
        fogDensity = 0.2f;
    }

    float fragmentDistance = length(fragPosLightSpace);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

    return clamp(fogFactor, 0.0f, 1.0f);
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


void main()
{
    computeLightComponents();

    vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange

	vec3 buoyLight = computeLampLight(buoyLightEye, buoyLightDirection, 35.0f, 0.07f, 0.17f, buoyLightColor);

    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;
    specular *= texture(specularTexture, fTexCoords).rgb;

    shadow = computeShadow();
    vec3 color = min((ambient + (3.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
    vec3 finalColor = min(color + buoyLight, 1.0f);

    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

    if(cameraPos.y < -7.0) {
        fogColor = vec4(0.0f, 0.2f, 1.0f, 1.0f);
    }


    fColor = fogColor * (1 - fogFactor) + vec4(finalColor, 1.0f) * fogFactor;
}
