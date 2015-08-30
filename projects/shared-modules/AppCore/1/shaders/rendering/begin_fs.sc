$input v_texcoord0

#include <torque6.sc>

SAMPLER2D(Texture0, 0);

void main()
{
    vec3 decodedColor = decodeRGBE8(texture2D(Texture0, v_texcoord0));
    gl_FragColor = vec4(decodedColor.rgb, 1.0);
}
