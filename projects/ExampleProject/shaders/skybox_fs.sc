$input v_dir

#include <bgfx_shader.sh>

SAMPLERCUBE(u_texCube, 0);

void main()
{
	gl_FragColor = vec4(textureCube(u_texCube, v_dir).xyz, 1.0);
}
