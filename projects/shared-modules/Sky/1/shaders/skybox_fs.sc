$input v_dir

#include <torque6.sc>

SAMPLERCUBE(Texture0, 0);

void main()
{
	gl_FragColor = encodeRGBE8(toLinear(textureCube(Texture0, v_dir).xyz));
}
