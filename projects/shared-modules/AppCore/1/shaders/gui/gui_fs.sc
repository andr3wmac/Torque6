$input v_texcoord0
#include <bgfx_shader.sh>

SAMPLER2D(Texture0, 0);

void main()
{
    vec4 color = texture2D(Texture0, v_texcoord0);
    gl_FragColor = vec4(color.xyz, 1.0);
}
