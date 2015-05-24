$input v_color0, v_texcoord0, v_sspos

#include <torque6.sc>

SAMPLER2D(Texture0, 0);

void main()
{
    vec4 color = texture2D(Texture0, v_texcoord0);
    gl_FragColor = encodeRGBE8(color.rgb);
}
