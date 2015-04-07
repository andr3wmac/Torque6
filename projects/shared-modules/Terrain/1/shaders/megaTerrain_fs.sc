$input v_position, v_texcoord0

#include <torque6.sc>

SAMPLER2D(Texture0, 0);

void main()
{
    vec4 megaTexture = texture2D(Texture0, v_texcoord0); 
    gl_FragColor = vec4(v_texcoord0.x, v_texcoord0.y, 0.0, 1.0);
}
