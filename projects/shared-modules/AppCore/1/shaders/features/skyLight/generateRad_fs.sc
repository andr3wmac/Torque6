$input v_texcoord0

#include <torque6.sc>
#include <skyLight.sc>

uniform vec4 u_cubeParams;
SAMPLERCUBE(u_skyCube, 0); // Sky Cubemap

void main()
{
    // TODO: GGX importance sampling

    vec2 currUV         = ( v_texcoord0.xy * 2.0 ) - 1.0;
    vec3 currDirection  = texelCoordToVec(currUV, int(u_cubeParams.x));
    vec3 outputColor    = textureCube(u_skyCube, currDirection).rgb;
    gl_FragColor        = vec4(outputColor, 1.0);
}
