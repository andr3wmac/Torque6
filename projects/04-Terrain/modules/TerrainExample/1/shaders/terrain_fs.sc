$input v_position, v_texcoord0

#include <torque6.sc>

SAMPLER2D(Texture0, 0);

uniform vec2 focusPoint;
uniform vec3 cascadeSize;

void main()
{
    vec2 focus_dist = abs(focusPoint - v_texcoord0.xy);

    // Trick to avoid branching.
    vec3 cascade_check = vec3(focus_dist.x > cascadeSize.x || focus_dist.y > cascadeSize.x,
                              focus_dist.x > cascadeSize.y || focus_dist.y > cascadeSize.y,
                              focus_dist.x > cascadeSize.z || focus_dist.y > cascadeSize.z);

    int cascade = int(dot(vec3(1.0, 1.0, 1.0), cascade_check));
    int next_cascade = max(cascade + 1, 3);

    vec2 cascade_uvs[4];
    cascade_uvs[0] = vec2(0.0, 0.0) + (v_texcoord0.xy - (focusPoint - cascadeSize.x)) * (1.0 / (cascadeSize.x * 2.0)) * 0.5;
    cascade_uvs[1] = vec2(0.5, 0.0) + (v_texcoord0.xy - (focusPoint - cascadeSize.y)) * (1.0 / (cascadeSize.y * 2.0)) * 0.5;
    cascade_uvs[2] = vec2(0.0, 0.5) + (v_texcoord0.xy - (focusPoint - cascadeSize.z)) * (1.0 / (cascadeSize.z * 2.0)) * 0.5;
    cascade_uvs[3] = vec2(0.5, 0.5) + (v_texcoord0.xy * 0.5);

    // Sample MegaTexture twice and blend.
    vec4 sample0 = texture2D(Texture0, cascade_uvs[cascade]); 
    vec4 sample1 = texture2D(Texture0, cascade_uvs[next_cascade]); 
    vec4 blended = mix(sample0, sample1, 0.25);

    // Output blended result.
    gl_FragColor = vec4(blended.rgb, 1.0);

    // DEBUG:
    //vec4 debug = texture2D(Texture0, v_texcoord0.xy);
    //gl_FragColor = vec4(debug.rgb, 1.0); 
}
