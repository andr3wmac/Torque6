$input a_position

#include <torque6.sc>

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
}
