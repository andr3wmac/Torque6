$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_deferred_color, 0);
SAMPLER2D(s_deferred_light, 1);

void main()
{
	vec4 deferred_color = vec4(texture2D(s_deferred_color, v_texcoord0).rgb, 1.0);
    vec4 light_color = vec4(texture2D(s_deferred_light, v_texcoord0).rgb, 1.0);
	gl_FragColor = vec4(deferred_color.rgb * light_color.rgb, 1.0);
}
