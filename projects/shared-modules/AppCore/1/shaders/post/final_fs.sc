$input v_texcoord0

#include <torque6.sc>

SAMPLER2D(s_input, 0);

void main()
{
    vec4 colorSample = texture2D(s_input, v_texcoord0);
    gl_FragColor = vec4(colorSample.rgb, 1.0);
}
