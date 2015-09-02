$input v_texcoord0

#include <torque6.sc>
#include <shadows.sc>

uniform mat4 u_sceneInvViewProjMat;

SAMPLER2D(Texture0, 0); // Depth

// Cascaded ShadowMap
uniform mat4 u_cascadeMtx0;
uniform mat4 u_cascadeMtx1;
uniform mat4 u_cascadeMtx2;
uniform mat4 u_cascadeMtx3;

SAMPLER2D(Texture1, 1); // ShadowMap Cascade 0
SAMPLER2D(Texture2, 2); // ShadowMap Cascade 1
SAMPLER2D(Texture3, 3); // ShadowMap Cascade 2
SAMPLER2D(Texture4, 4); // ShadowMap Cascade 3

void main()
{
    // World-Space Position
    float deviceDepth   = texture2D(Texture0, v_texcoord0).x;
    float depth         = toClipSpaceDepth(deviceDepth);
    vec3  clip          = vec3(toClipSpace(v_texcoord0), depth);
    vec3  wpos          = clipToWorld(u_sceneInvViewProjMat, clip);

    // ShadowMap Coordinates
    vec4 posOffset = vec4(wpos, 1.0);
    vec4 csmcoord1 = mul(u_cascadeMtx0, posOffset);
    vec4 csmcoord2 = mul(u_cascadeMtx1, posOffset);
    vec4 csmcoord3 = mul(u_cascadeMtx2, posOffset);
    vec4 csmcoord4 = mul(u_cascadeMtx3, posOffset);

    vec2 texcoord1 = csmcoord1.xy/csmcoord1.w;
    vec2 texcoord2 = csmcoord2.xy/csmcoord2.w;
    vec2 texcoord3 = csmcoord3.xy/csmcoord3.w;
    vec2 texcoord4 = csmcoord4.xy/csmcoord4.w;

    bool selection0 = all(lessThan(texcoord1, vec2_splat(0.99))) && all(greaterThan(texcoord1, vec2_splat(0.01)));
    bool selection1 = all(lessThan(texcoord2, vec2_splat(0.99))) && all(greaterThan(texcoord2, vec2_splat(0.01)));
    bool selection2 = all(lessThan(texcoord3, vec2_splat(0.99))) && all(greaterThan(texcoord3, vec2_splat(0.01)));
    bool selection3 = all(lessThan(texcoord4, vec2_splat(0.99))) && all(greaterThan(texcoord4, vec2_splat(0.01)));

    // Hard-Coded Setting (TEMP)
    float _bias = 0.0030;
    float _depthMultiplier = 10000.0;
    float _minVariance = 0.010;

    // Shadows
    float visibility;
    vec3 colorCoverage;
    if (selection0)
    {
        float coverage = texcoordInRange(csmcoord1.xy/csmcoord1.w) * 0.4;
        colorCoverage = vec3(-coverage, coverage, -coverage);

        //visibility = VSM(Texture1, csmcoord1, _bias, _depthMultiplier, _minVariance);
        visibility = ESM(Texture1, csmcoord1, _bias, _depthMultiplier);
        //visibility = EVSM(Texture1, csmcoord1);
    }
    else if (selection1)
    {
        float coverage = texcoordInRange(csmcoord2.xy/csmcoord2.w) * 0.4;
        colorCoverage = vec3(coverage, coverage, -coverage);

        //visibility = VSM(Texture2, csmcoord2, _bias, _depthMultiplier, _minVariance);
        visibility = ESM(Texture2, csmcoord2, _bias, _depthMultiplier);
        //visibility = EVSM(Texture2, csmcoord2);
    }
    else if (selection2)
    {
        float coverage = texcoordInRange(csmcoord3.xy/csmcoord3.w) * 0.4;
        colorCoverage = vec3(-coverage, -coverage, coverage);

        //visibility = VSM(Texture3, csmcoord3, _bias, _depthMultiplier, _minVariance);
        visibility = ESM(Texture3, csmcoord3, _bias, _depthMultiplier);
        //visibility = EVSM(Texture3, csmcoord3);
    }
    else //selection3
    {
        float coverage = texcoordInRange(csmcoord4.xy/csmcoord4.w) * 0.4;
        colorCoverage = vec3(coverage, -coverage, -coverage);

        //visibility = VSM(Texture4, csmcoord4, _bias, _depthMultiplier, _minVariance);
        visibility = ESM(Texture4, csmcoord4, _bias, _depthMultiplier);
        //visibility = EVSM(Texture4, csmcoord4);
    }

    // Output
    gl_FragColor = vec4(visibility, visibility, visibility, 1.0);
}
