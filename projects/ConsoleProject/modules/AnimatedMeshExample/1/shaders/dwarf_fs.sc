$input v_wpos, v_texcoord0
#include <bgfx_shader.sh>
#include <common.sh>

SAMPLER2D(Texture0, 0);
//uniform vec4 lightPosRadius[4];
//uniform vec4 lightColorAttn[4];

vec3 calcLight(vec3 wpos, vec3 light_pos, vec3 light_color, float light_radius, float light_attn)
{
	vec3 lp = light_pos - wpos;
	float attn = 1.0 - smoothstep(light_attn, 1.0, length(lp) / light_radius);
	vec3 rgb = light_color * attn;
	return rgb;
}

void main()
{
	//vec3 lightColor;
	//lightColor =  calcLight(v_wpos, lightPosRadius[0].xyz, lightColorAttn[0].xyz, lightPosRadius[0].w, lightColorAttn[0].w);
	//lightColor += calcLight(v_wpos, lightPosRadius[1].xyz, lightColorAttn[1].xyz, lightPosRadius[1].w, lightColorAttn[1].w);
	//lightColor += calcLight(v_wpos, lightPosRadius[2].xyz, lightColorAttn[2].xyz, lightPosRadius[2].w, lightColorAttn[2].w);
	//lightColor += calcLight(v_wpos, lightPosRadius[3].xyz, lightColorAttn[3].xyz, lightPosRadius[3].w, lightColorAttn[3].w);

    //vec4 color = toLinear(texture2D(Texture0, v_texcoord0));
	//gl_FragColor.xyz = max(vec3_splat(0.05), lightColor.xyz) * color.xyz;
	//gl_FragColor.w = 1.0;
    //gl_FragColor = toGamma(gl_FragColor);

    vec4 color = texture2D(Texture0, v_texcoord0);
    gl_FragColor = vec4(color.rgb, 1.0);
}
