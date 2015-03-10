$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_deferred_color, 0);
SAMPLER2D(s_deferred_light, 1);
SAMPLER2D(s_deferred_depth, 2);

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
	float deviceDepth = texture2D(s_deferred_depth,  v_texcoord0).x;
	float depth       = toClipSpaceDepth(deviceDepth);

	vec3 clip = vec3(v_texcoord0 * 2.0 - 1.0, depth);
#if BGFX_SHADER_LANGUAGE_HLSL
	clip.y = -clip.y;
#endif // BGFX_SHADER_LANGUAGE_HLSL
	vec3 wpos = clipToWorld(u_invViewProj, clip);

	vec4 deferred_color = texture2D(s_deferred_color, v_texcoord0);
    vec4 light_color = texture2D(s_deferred_light, v_texcoord0);

	gl_FragColor = vec4(deferred_color.rgb * light_color.rgb, 1.0);
}
