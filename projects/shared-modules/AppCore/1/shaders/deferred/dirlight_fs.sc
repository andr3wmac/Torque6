$input v_color0, v_texcoord0, v_sspos

#include <torque6.sc>
#include <shadows.sc>

uniform vec4 dirLightDirection;
uniform vec4 dirLightColor;
uniform vec4 dirLightAmbient;

uniform mat4 u_sceneInvViewProjMat;
uniform vec4 u_camPos;

SAMPLER2D(Texture0, 0); // Color
SAMPLER2D(Texture1, 1); // Normals
SAMPLER2D(Texture2, 2); // Material Info
SAMPLER2D(Texture3, 3); // Depth

// Cascaded ShadowMap
uniform mat4 u_cascadeMtx0;
uniform mat4 u_cascadeMtx1;
uniform mat4 u_cascadeMtx2;
uniform mat4 u_cascadeMtx3;

SAMPLER2D(Texture4, 4); // ShadowMap Cascade 0
SAMPLER2D(Texture5, 5); // ShadowMap Cascade 1
SAMPLER2D(Texture6, 6); // ShadowMap Cascade 2
SAMPLER2D(Texture7, 7); // ShadowMap Cascade 3

SAMPLERCUBE(u_ambientCube, 8);
SAMPLERCUBE(u_ambientIrrCube, 9);

#define LIGHTING_AMBIENT_CUBE
#include <lighting.sh>

void main()
{
    // World-Space Position
    float deviceDepth   = texture2D(Texture3, v_texcoord0).x;
    float depth         = toClipSpaceDepth(deviceDepth);
    vec3 clip           = vec3(toClipSpace(v_texcoord0), depth);
    vec3 wpos           = clipToWorld(u_sceneInvViewProjMat, clip);

    // Color
    vec3 color = decodeRGBE8(texture2D(Texture0, v_texcoord0));

    // Normals
    vec3 normal = decodeNormalUint(texture2D(Texture1, v_texcoord0).xyz);

    // View Direction
    vec3 viewDir = normalize(u_camPos.xyz - wpos);

    // Material Info
    vec4 matInfo = texture2D(Texture2, v_texcoord0);

    // ShadowMap Coordinates
    vec4 posOffset = vec4(wpos, 1.0);
    vec4 csmcoord1 = mul(u_cascadeMtx0, posOffset);
    vec4 csmcoord2 = mul(u_cascadeMtx1, posOffset);
    vec4 csmcoord3 = mul(u_cascadeMtx2, posOffset);
    vec4 csmcoord4 = mul(u_cascadeMtx3, posOffset);

    vec2 texelSize = vec2_splat(1.0 / 1024.0);

    vec2 texcoord1 = csmcoord1.xy/csmcoord1.w;
    vec2 texcoord2 = csmcoord2.xy/csmcoord2.w;
    vec2 texcoord3 = csmcoord3.xy/csmcoord3.w;
    vec2 texcoord4 = csmcoord4.xy/csmcoord4.w;

    bool selection0 = all(lessThan(texcoord1, vec2_splat(0.99))) && all(greaterThan(texcoord1, vec2_splat(0.01)));
    bool selection1 = all(lessThan(texcoord2, vec2_splat(0.99))) && all(greaterThan(texcoord2, vec2_splat(0.01)));
    bool selection2 = all(lessThan(texcoord3, vec2_splat(0.99))) && all(greaterThan(texcoord3, vec2_splat(0.01)));
    bool selection3 = all(lessThan(texcoord4, vec2_splat(0.99))) && all(greaterThan(texcoord4, vec2_splat(0.01)));

    // Hard-Coded Setting (TEMP)
    float _bias = 0.004;
    float _depthMultiplier = 2500.0;
    float _minVariance = 0.02;

    float visibility;
    vec3 colorCoverage;
    if (selection0)
    {
        vec4 shadowcoord = csmcoord1;

        float coverage = texcoordInRange(shadowcoord.xy/shadowcoord.w) * 0.4;
        colorCoverage = vec3(-coverage, coverage, -coverage);
        visibility = VSM(Texture4, shadowcoord, _bias, _depthMultiplier, _minVariance);
    }
    else if (selection1)
    {
        vec4 shadowcoord = csmcoord2;

        float coverage = texcoordInRange(shadowcoord.xy/shadowcoord.w) * 0.4;
        colorCoverage = vec3(coverage, coverage, -coverage);
        visibility = VSM(Texture5, shadowcoord, _bias, _depthMultiplier, _minVariance);
    }
    else if (selection2)
    {
        vec4 shadowcoord = csmcoord3;

        float coverage = texcoordInRange(shadowcoord.xy/shadowcoord.w) * 0.4;
        colorCoverage = vec3(-coverage, -coverage, coverage);
        visibility = VSM(Texture6, shadowcoord, _bias, _depthMultiplier, _minVariance);
    }
    else //selection3
    {
        vec4 shadowcoord = csmcoord4;

        float coverage = texcoordInRange(shadowcoord.xy/shadowcoord.w) * 0.4;
        colorCoverage = vec3(coverage, -coverage, -coverage);
        visibility = VSM(Texture7, shadowcoord, _bias, _depthMultiplier, _minVariance);
    }

    // Directional Light
    vec3 lightColor = calcDirectionalLight(color,
                                           normal,
                                           matInfo.r, // Metallic 
                                           matInfo.g, // Roughness
                                           viewDir,
                                           dirLightDirection.xyz, 
                                           dirLightColor.rgb, 
                                           dirLightAmbient.rgb,
                                           visibility);

    // Output
    gl_FragColor = vec4(lightColor, 1.0);
}
