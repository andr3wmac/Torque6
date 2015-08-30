$input v_texcoord0

#include <torque6.sc>
#include "dlaa.sh"

SAMPLER2D(Texture0, 0); // Backbuffer

void main()
{
    gl_FragColor = firsPassEdgeDetect(Texture0, v_texcoord0);
}
