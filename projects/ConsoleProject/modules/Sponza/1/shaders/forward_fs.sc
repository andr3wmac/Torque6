$input v_texcoord0, v_normal

#include <bgfx_shader.sh>
#include <common.sh>

SAMPLER2D(Texture0, 0);
uniform vec3 dirLightDirection;
uniform vec3 dirLightColor;

void main()
{
    // Invert the light direction for calculations.
    vec3 lightDir = vec3(1, 1, 0);

    // Calculate the amount of light on this pixel.
    vec3 lightIntensity = saturate(dot(v_normal.xyz, lightDir));

    // Calculate light color
    vec3 lightColor = dirLightColor * lightIntensity;

    // Texture Sample
    vec4 color = texture2D(Texture0, v_texcoord0);

    //gl_FragColor = vec4(dirLightColor, 1.0);

    gl_FragColor = vec4(color.rgb * lightIntensity.rgb, 1.0);
}
