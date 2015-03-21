$input v_texcoord0, v_wpos, v_normal, v_shadowcoord

#include <torque6.sc>
#include <forwardLighting.sc>

void main()
{
    // Sample texture.
    vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

    // Compute forward lighting.
    vec3 lightColor = computeForwardLighting(v_wpos.xyz, v_normal.xyz, v_shadowcoord);

    // Output combined result.
    gl_FragColor = vec4(color.rgb * lightColor.rgb, 1.0);
}
