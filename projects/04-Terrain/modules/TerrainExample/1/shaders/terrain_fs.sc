$input v_position, v_texcoord0

#include <torque6.sc>

SAMPLER2D(Texture0, 0);
SAMPLER2D(Texture1, 1);
SAMPLER2D(Texture2, 2);

void main()
{
    vec4 layer0 = texture2D(Texture0, v_texcoord0 * 20);
    vec4 layer1 = texture2D(Texture1, v_texcoord0 * 20);
    vec4 layer2 = texture2D(Texture2, v_texcoord0 * 20);

    float height = v_position.y / 10.0;
	//gl_FragColor = float4(height + v_texcoord0.x, height + v_texcoord0.y, height, 1.0);
    
    gl_FragColor = float4(mix(layer2.rgb, layer1.rgb, height), 1.0);
}
