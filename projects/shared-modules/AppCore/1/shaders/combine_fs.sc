$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_deferred_color, 0);
SAMPLER2D(s_deferred_light, 1);

void main()
{
    vec4 deferred_color = texture2D(s_deferred_color, v_texcoord0);
    vec4 light_color = texture2D(s_deferred_light, v_texcoord0);
    gl_FragColor = deferred_color * light_color;
}
