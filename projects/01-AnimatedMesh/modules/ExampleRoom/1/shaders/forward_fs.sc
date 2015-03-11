$input v_texcoord0, v_wpos, v_normal

#include <torque6.sc>
#include <forwardLighting.sc>

SAMPLER2D(Texture0, 0);

void main()
{
    // Sample texture.
    vec4 color = texture2D(Texture0, v_texcoord0);

    // Compute forward lighting.
    vec3 lightColor = computeForwardLighting(v_wpos.xyz, v_normal.xyz);

    // Output combined result.
    gl_FragColor = vec4(color.rgb * lightColor.rgb, 1.0);
}
