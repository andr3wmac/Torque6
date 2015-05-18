$input v_texcoord0

#include <torque6.sc>

SAMPLER2D(Texture0, 0); // Color
SAMPLER2D(Texture1, 1); // Material Info
SAMPLER2D(Texture2, 2); // Light Buffer

void main()
{
    vec3 deferred_color = decodeRGBE8(texture2D(Texture0, v_texcoord0));
    vec4 mat_info       = texture2D(Texture1, v_texcoord0);
    vec3 light_color    = texture2D(Texture2, v_texcoord0).rgb;

    float metallic = mat_info.r;
    float emissive = mat_info.a;

    vec3 diffuse = (deferred_color * (1.0 - metallic)) * light_color;
    vec3 spec = (light_color * metallic);

    gl_FragColor = encodeRGBE8((diffuse + spec) * (1.0 - emissive) + (deferred_color * emissive));
}
