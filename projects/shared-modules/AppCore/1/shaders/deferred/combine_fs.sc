$input v_texcoord0

#include <torque6.sc>

SAMPLER2D(s_deferred_color, 0);
SAMPLER2D(s_deferred_light, 1);

void main()
{
    vec4 deferred_color = toLinear(texture2D(s_deferred_color, v_texcoord0));
    vec4 light_color = toLinear(texture2D(s_deferred_light, v_texcoord0));
    gl_FragColor = toGamma(deferred_color * light_color);
}
