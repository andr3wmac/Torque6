$input v_color0
#include <torque6.sc>

void main()
{
    gl_FragColor = encodeRGBE8(v_color0.rgb);
}
