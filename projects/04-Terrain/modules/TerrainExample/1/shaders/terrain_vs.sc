$input a_position, a_texcoord0
$output v_position, v_texcoord0

#include <torque6.sc>
void main()
{
    v_position = vec4(a_position, 1.0);

    // Standard: UV Coordinates
    v_texcoord0 = a_texcoord0;

	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
