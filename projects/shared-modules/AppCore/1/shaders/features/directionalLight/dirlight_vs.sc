$input a_position, a_color0, a_texcoord0
$output v_color0, v_texcoord0, v_sspos

#include <torque6.sc>

void main()
{
    // Standard: Vertex Position
    vec4 vertPosition = vec4(a_position, 1.0);

    // Standard: UV Coordinates
    v_texcoord0 = a_texcoord0;

    v_sspos = mul(u_model[0], vertPosition );
    gl_Position = v_sspos;
    v_color0 = a_color0;
}
