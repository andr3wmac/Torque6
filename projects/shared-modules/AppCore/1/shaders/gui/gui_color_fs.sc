$input v_color0
#include <torque6.sc>

void main()
{
    //gl_FragColor = encodeRGBE8(v_color0.rgb);
    gl_FragColor = vec4(v_color0.rgb, 1.0);
}
