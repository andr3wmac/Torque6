$input v_normal, v_texcoord0

#include <torque6.sc>

void main()
{
    gl_FragData[0] = encodeRGBE8(vec3(0.5, 0.5, 0.5));
    gl_FragData[1] = vec4(encodeNormalUint(normalize(v_normal)), 1.0);
    gl_FragData[2] = vec4(0.5, 0.5, 0.5, 0.0);
}
