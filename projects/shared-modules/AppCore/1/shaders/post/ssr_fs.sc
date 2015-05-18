$input v_texcoord0

#include <torque6.sc>

SAMPLER2D(Texture0, 0); // Depth
SAMPLER2D(Texture1, 1); // Color
SAMPLER2D(Texture2, 2); // Normals

void main()
{
    vec3 normal = normalize(decodeNormalUint(texture2D(Texture2, v_texcoord0).xyz));
    gl_FragColor = vec4(normal.rgb, 1.0);
}
