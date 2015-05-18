$input v_dir

#include <torque6.sc>

SAMPLERCUBE(u_texCube, 0);

void main()
{
	gl_FragColor = encodeRGBE8(toLinear(textureCube(u_texCube, v_dir).xyz));
}
