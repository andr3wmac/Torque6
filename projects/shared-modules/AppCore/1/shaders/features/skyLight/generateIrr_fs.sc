$input v_texcoord0

#include <torque6.sc>
#include <skyLight.sc>

uniform vec4 u_cubeParams;
SAMPLERCUBE(u_skyCube, 0); // Sky Cubemap

void main()
{
    vec2 currUV = ( v_texcoord0.xy * 2.0 ) - 1.0;
    vec3 currDirection = texelCoordToVec(currUV, int(u_cubeParams.x));

    vec3 totalColor = vec3(0.0, 0.0, 0.0);
    float step = 1.0 / 16.0;
    for(int side = 0; side < 6; ++side)
    {
        vec3 sideColor = vec3(0.0, 0.0, 0.0);
        float sideAccum = 0.0;

        float x = -1.0;
        float y = -1.0;
        for(; y <= 1.0; y += step)
        {
            for(; x <= 1.0; x += step)
            {
                vec3 direction = texelCoordToVec(vec2(x, y), side);
                float ndotl = max(0, dot(direction, currDirection));
                sideColor += (textureCube(u_skyCube, direction).rgb * ndotl);
                sideAccum += 1.0;
            }
        }
        totalColor += sideColor / sideAccum;
    }
    totalColor = totalColor / 6.0;
    gl_FragColor = vec4(totalColor.rgb, 1.0);
}
