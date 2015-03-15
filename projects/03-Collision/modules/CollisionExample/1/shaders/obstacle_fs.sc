$input v_texcoord0, v_wpos, v_normal

#include <torque6.sc>
#include <forwardLighting.sc>

uniform vec3 cubeColor;

void main()
{
    // Compute forward lighting.
    vec3 lightColor = computeForwardLighting(v_wpos.xyz, v_normal.xyz);

    // Output combined result.
    gl_FragColor = vec4(cubeColor.rgb * lightColor.rgb, 1.0);
}
