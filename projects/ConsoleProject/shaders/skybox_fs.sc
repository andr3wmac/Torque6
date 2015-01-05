$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLERCUBE(u_texCube, 0);
uniform mat4 u_mtx;

void main()
{
	vec3 dir = vec3(v_texcoord0*2.0 - 1.0, 1.0);
	dir = normalize(mul(u_mtx, vec4(dir, 0.0) ).xyz);
	gl_FragColor = vec4(textureCube(u_texCube, dir).xyz, 1.0);
}
