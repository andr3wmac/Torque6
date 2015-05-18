// Generic Includes
#include "bgfx_shader.sh"
#include "shaderlib.sh"

// Shared Lighting Equations.
#include "lighting.sh"

// Billboard Creation in Vertex Shader
vec4 createBillboard(mat4 modelTransform, vec3 modelPos, vec3 billboardPos, float billboardSize)
{

#if BGFX_SHADER_LANGUAGE_HLSL
    modelTransform[0] += vec4(0, 0, 0, billboardPos.x);
    modelTransform[1] += vec4(0, 0, 0, billboardPos.y);
    modelTransform[2] += vec4(0, 0, 0, billboardPos.z);
#else
    modelTransform[3] += vec4(billboardPos.x, billboardPos.y, billboardPos.z, 0);
#endif

    mat4 modelView = mul(u_view, modelTransform);
    return mul(u_proj, mul(modelView, vec4(0.0, 0.0, 0.0, 1.0)) - vec4(modelPos.x * billboardSize, modelPos.z * billboardSize, 0.0, 0.0));
}

// Space Conversion Functions
vec2 toUVSpace( vec3 screenspacePos )
{
    vec2 outPos = ( screenspacePos.xy + 1.0 ) / 2.0;
#if BGFX_SHADER_LANGUAGE_HLSL
    outPos.y = 1.0 - outPos.y;
#endif
    return outPos;
}

vec2 toClipSpace( vec2 uvPos )
{
    vec2 outPos = uvPos;

#if BGFX_SHADER_LANGUAGE_HLSL
    outPos.y = 1.0 - outPos.y;
#endif

    outPos = ( outPos.xy * 2.0 ) - 1.0;
    return outPos;
}

float toClipSpaceDepth(float _depthTextureZ)
{
#if BGFX_SHADER_LANGUAGE_HLSL
    return _depthTextureZ;
#else
    return _depthTextureZ * 2.0 - 1.0;
#endif // BGFX_SHADER_LANGUAGE_HLSL
}

vec3 clipToWorld(mat4 _invViewProj, vec3 _clipPos)
{
    vec4 wpos = mul(_invViewProj, vec4(_clipPos, 1.0) );
    return wpos.xyz / wpos.w;
}
