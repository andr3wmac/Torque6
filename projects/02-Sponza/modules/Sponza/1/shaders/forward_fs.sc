$input v_texcoord0, v_wpos, v_normal, v_shadowcoord

#include <torque6.sc>
#include <forwardLighting.sc>

SAMPLER2D(Texture0, 1);

void main()
{
    // Sample texture.
    vec4 color = toLinear(texture2D(Texture0, v_texcoord0));

    // Compute forward lighting.
    vec3 lightColor = computeForwardLighting(v_wpos.xyz, v_normal.xyz, v_shadowcoord);

    // Output combined result.
    gl_FragColor = vec4(toGamma(color.rgb * lightColor.rgb), 1.0);
}
