$input v_texcoord0

#include <torque6.sc>
#include "ssao.sh"

SAMPLER2D(Texture0, 0); // Occlusion
SAMPLER2D(Texture1, 1); // Normals

void main()
{
    gl_FragColor = ssao_blur(v_texcoord0, vec2(1.0, 0.0), Texture0, Texture1);
}
