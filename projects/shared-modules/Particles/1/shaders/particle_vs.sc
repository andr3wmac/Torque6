$input a_position, a_texcoord0, i_data0
$output v_position, v_texcoord0

#include <torque6.sc>
void main()
{
    v_position = vec4(a_position, 1.0);

    // Standard: UV Coordinates
    v_texcoord0 = a_texcoord0;

    mat4 instModel = u_model[0];
    instModel[0] += vec4(0, 0, 0, i_data0.x);
    instModel[1] += vec4(0, 0, 0, i_data0.y);
    instModel[2] += vec4(0, 0, 0, i_data0.z);
    mat4 instModelView = mul(u_view, instModel);

	//gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    gl_Position = mul(u_proj, mul(instModelView, vec4(0.0, 0.0, 0.0, 1.0)) - vec4(a_position.x * 5, a_position.z * 5, 0.0, 0.0));
}
