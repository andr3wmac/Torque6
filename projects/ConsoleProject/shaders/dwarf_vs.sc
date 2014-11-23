$input a_position, a_texcoord0, a_indices, a_weights
$output v_texcoord0

#include <bgfx_shader.sh>

void main()
{
    // GPU Skinning
    mat4 BoneTransform =    mul(u_model[int(a_indices[0])], a_weights[0]);
    //BoneTransform +=        mul(u_model[int(a_indices[1])], a_weights[1]);
    //BoneTransform +=        mul(u_model[int(a_indices[2])], a_weights[2]);
    //BoneTransform +=        mul(u_model[int(a_indices[3])], a_weights[3]);    

    vec4 PosL = mul(BoneTransform, vec4(a_position, 1.0));
    gl_Position = mul(u_modelViewProj, PosL);

    //mat4 testProj = mul(u_viewProj, u_model[0]);
    //gl_Position = mul(testProj, vec4(a_position, 1.0));

	//gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
	v_texcoord0 = a_texcoord0;
}
