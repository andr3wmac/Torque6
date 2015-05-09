$input a_position, a_texcoord0, a_normal, a_indices, a_weights
$output v_wpos, v_texcoord0, v_normal

#include <torque6.sc>

void main()
{
    // Standard: Vertex Position
    vec4 vertPosition = vec4(a_position, 1.0);

    // Optional: GPU Skinning
    mat4 boneTransform =    mul(u_model[int(a_indices[0])], a_weights[0]);
    boneTransform +=        mul(u_model[int(a_indices[1])], a_weights[1]);
    boneTransform +=        mul(u_model[int(a_indices[2])], a_weights[2]);
    boneTransform +=        mul(u_model[int(a_indices[3])], a_weights[3]);    
    vertPosition =          mul(boneTransform, vertPosition);

    // Optional: World-Space Position (used for lighting)
    vec3 wpos = mul(u_model[0], vertPosition).xyz;
    v_wpos = wpos;

    // Standard: UV Coordinates
    v_texcoord0 = a_texcoord0;

    // Normals
    vec3 normal = a_normal * 2.0 - 1.0;
	vec3 wnormal = mul(u_model[0], vec4(a_normal.xyz, 0.0) ).xyz;
    v_normal = wnormal.xyz;

    // Standard: Output Final Vertex Position
    gl_Position = mul(u_modelViewProj, vertPosition);
}
