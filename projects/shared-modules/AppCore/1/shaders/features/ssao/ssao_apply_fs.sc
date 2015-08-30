$input v_texcoord0

#include <torque6.sc>

SAMPLER2D(Texture0, 0); // Backbuffer
SAMPLER2D(Texture1, 1); // Occlusion

void main()
{
    vec3 backbuffer = decodeRGBE8(texture2D(Texture0, v_texcoord0));
    vec4 occlusion  = texture2D(Texture1, v_texcoord0);

    gl_FragColor = encodeRGBE8(backbuffer * occlusion.rgb);
}
