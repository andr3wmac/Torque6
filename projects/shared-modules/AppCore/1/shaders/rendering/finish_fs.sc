$input v_texcoord0

#include <torque6.sc>

SAMPLER2D(Texture0, 0);

void main()
{
    vec4 colorSample = texture2D(Texture0, v_texcoord0);
    gl_FragColor = vec4(colorSample.rgb, 1.0);
}
