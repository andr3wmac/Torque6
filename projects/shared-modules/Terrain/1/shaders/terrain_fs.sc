$input v_position, v_texcoord0

#include <torque6.sc>

void main()
{
    float height = v_position.y / 50.0;
	gl_FragColor = float4(height + v_texcoord0.x, height + v_texcoord0.y, height, 1.0);
}
