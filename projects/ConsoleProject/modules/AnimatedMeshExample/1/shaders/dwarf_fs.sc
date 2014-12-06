$input v_wpos, v_texcoord0
#include <bgfx_shader.sh>
#include <common.sh>

SAMPLER2D(Texture0, 0);
uniform vec4 lightPosRadius[4];
uniform vec4 lightColorAttn[4];

vec3 calcLight(int _idx, vec3 _wpos)
{
	vec3 lp = lightPosRadius[_idx].xyz - _wpos;
	float attn = 1.0 - smoothstep(lightColorAttn[_idx].w, 1.0, length(lp) / lightPosRadius[_idx].w);
	vec3 rgb = lightColorAttn[_idx].xyz * attn;
	return rgb;
}

void main()
{
	vec3 lightColor;
	lightColor =  calcLight(0, v_wpos);
	lightColor += calcLight(1, v_wpos);
	lightColor += calcLight(2, v_wpos);
	lightColor += calcLight(3, v_wpos);

    vec4 color = toLinear(texture2D(Texture0, v_texcoord0));
	gl_FragColor.xyz = max(vec3_splat(0.05), lightColor.xyz) * color.xyz;
	gl_FragColor.w = 1.0;
}
