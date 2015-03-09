$input v_color0, v_texcoord0, v_sspos

#include <torque6.sc>

uniform vec4 singleLightPosRadius;
uniform vec4 singleLightColorAttn;

SAMPLER2D(s_depth, 0);

vec2 toUVSpace( float3 screenspacePos )
{
	vec2 outPos = ( screenspacePos.xy + 1.0 ) / 2.0;
#if BGFX_SHADER_LANGUAGE_HLSL
	outPos.y = 1.0 - outPos.y;
#endif
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

void main()
{
    vec3 sspos          = v_sspos.xyz / v_sspos.w;
    vec2 uv_coords      = toUVSpace( sspos );
	float deviceDepth   = texture2D(s_depth, uv_coords).x;
	float depth         = toClipSpaceDepth(deviceDepth);

	vec3 clip = vec3(sspos.xy, depth);
    vec3 dwpos = clipToWorld(u_invViewProj, clip);

    vec3 color = calcPointLight(dwpos, singleLightPosRadius.xyz, singleLightColorAttn.xyz, singleLightPosRadius.w, singleLightColorAttn.w);
    gl_FragColor = vec4(color, 1.0);
}
