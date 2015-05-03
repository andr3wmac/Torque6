$input v_wpos, v_texcoord0, v_normal

#include <torque6.sc>

SAMPLER2D(Texture0, 0);

void main()
{
    // Color
    vec4 color = texture2D(Texture0, v_texcoord0);
    gl_FragData[0] = vec4(color.rgb, 1.0);

    // Normals
    vec3 wnormal = normalize(v_normal).xyz;
    gl_FragData[1] = vec4(encodeNormalUint(wnormal), 1.0);
}
