$input a_position, a_indices, a_weights
$output v_position

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

	gl_Position = mul(u_modelViewProj, vertPosition );
	v_position = gl_Position;
}
