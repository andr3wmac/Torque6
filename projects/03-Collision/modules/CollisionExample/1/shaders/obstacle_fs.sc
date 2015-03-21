$input v_texcoord0, v_wpos, v_normal, v_shadowcoord

#include <torque6.sc>
#include <forwardLighting.sc>

uniform vec3 cubeColor;

void main()
{
    // Compute forward lighting.
    vec3 lightColor = computeForwardLighting(v_wpos.xyz, v_normal.xyz, v_shadowcoord);

    // Output combined result.
    gl_FragColor = vec4(cubeColor.rgb * lightColor.rgb, 1.0);
}
