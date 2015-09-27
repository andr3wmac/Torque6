$input a_position, a_normal, a_texcoord0
$output v_normal, v_texcoord0

#include <torque6.sc>

void main()
{
    // Vertex Position
    vec4 vertPosition = vec4(a_position, 1.0);
    mat4 modelTransform = u_model[0];

    // UV Coordinates
    v_texcoord0 = a_texcoord0;

    // Normals
    v_normal = mul(modelTransform, vec4(a_normal.xyz, 0.0) ).xyz;

    gl_Position = mul(u_modelViewProj, vertPosition);
}
