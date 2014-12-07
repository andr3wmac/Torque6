$input v_color0
#include <bgfx_shader.sh>

uniform vec4 lightColor[4];

void main()
{
	gl_FragColor = vec4(lightColor[0].xyz, 1.0);
}
