$input a_position, a_normal, a_texcoord0
$output v_position, v_texcoord0, v_normal

#include <torque6.sc>

void main()
{
    v_position = vec4(a_position, 1.0);
    v_texcoord0 = a_texcoord0;

    // Normals
    vec3 normal = a_normal * 2.0 - 1.0;
	vec3 wnormal = mul(u_model[0], vec4(a_normal.xyz, 0.0) ).xyz;
    v_normal = wnormal.xyz;
 
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
