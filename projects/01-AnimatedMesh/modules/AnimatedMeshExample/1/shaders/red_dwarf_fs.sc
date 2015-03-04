$input v_wpos, v_texcoord0
#include <bgfx_shader.sh>

SAMPLER2D(Texture0, 0);
SAMPLER2D(Texture1, 1);

void main()
{
    vec4 color = texture2D(Texture0, v_texcoord0);
    vec4 color2 = texture2D(Texture1, v_texcoord0);
    color += color2;
    gl_FragColor = vec4(color.xyz, 1.0);
}
