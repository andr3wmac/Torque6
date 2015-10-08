$input a_position, a_texcoord0
$output v_dir

#include <bgfx_shader.sh>

uniform mat4 u_mtx;

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    vec2 tex = 2.0 * a_texcoord0 - 1.0;

#if BGFX_SHADER_LANGUAGE_HLSL
    tex.y = -tex.y;
#endif

    vec3 dir = mul(u_mtx, vec4(tex, 1.0, 0.0)).xyz;
    v_dir = vec3(dir.x, dir.z, dir.y);
}
