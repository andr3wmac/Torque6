SAMPLER2D(ShadowMap, 0);

uniform vec4 dirLightDirection;
uniform vec4 dirLightColor;
uniform vec4 dirLightAmbient;
uniform vec4 lightPosRadius[4];
uniform vec4 lightColorAttn[4];

#include <shadows.sc>

vec3 computeForwardLighting(vec3 pos, vec3 normal, vec3 view, vec4 shadowcoord)
{
    // Temp: Fixed input color
    vec3 color = vec3(1.0, 1.0, 1.0);

    // Total Light Color
    vec3 lightColor;

    // Directional Light
    lightColor =  calcDirectionalLight(color, normal, 0.5, 0.5, view, dirLightDirection, dirLightColor, dirLightAmbient);

    // Point Lights
    lightColor += calcPointLight(pos, normal, view, lightPosRadius[0].xyz, lightColorAttn[0].xyz, lightPosRadius[0].w, lightColorAttn[0].w);
    lightColor += calcPointLight(pos, normal, view, lightPosRadius[1].xyz, lightColorAttn[1].xyz, lightPosRadius[1].w, lightColorAttn[1].w);
    lightColor += calcPointLight(pos, normal, view, lightPosRadius[2].xyz, lightColorAttn[2].xyz, lightPosRadius[2].w, lightColorAttn[2].w);
    lightColor += calcPointLight(pos, normal, view, lightPosRadius[3].xyz, lightColorAttn[3].xyz, lightPosRadius[3].w, lightColorAttn[3].w);

    // Compute shadow coverage.
    float shadowMapBias = 0.000;
    vec2 texelSize = vec2_splat(1.0/512.0);
    float visibility = PCF(ShadowMap, shadowcoord, shadowMapBias, texelSize);
    lightColor = lightColor * visibility;

    // Ambient
    lightColor += dirLightAmbient;

    return lightColor;
}
