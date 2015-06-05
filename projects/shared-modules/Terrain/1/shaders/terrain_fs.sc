$input v_position, v_texcoord0, v_normal

#include <torque6.sc>

SAMPLER2D(Texture0, 0);

uniform vec4 focusPoint;
uniform vec4 cascadeSize;

void main()
{
    vec2 focus_dist = abs(focusPoint.xy - v_texcoord0.xy);

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

    float cascade_blend[4];
    cascade_blend[0] = 0.25;
    cascade_blend[1] = 0.25;
    cascade_blend[2] = 0.25;
    cascade_blend[3] = 0.0;

    // Sample MegaTexture twice and blend.
    vec4 sample0 = texture2D(Texture0, cascade_uvs[cascade]); 
    vec4 sample1 = texture2D(Texture0, cascade_uvs[next_cascade]); 
    vec4 blended = mix(sample0, sample1, cascade_blend[cascade]);

    // Deferred: Color
    gl_FragData[0] = encodeRGBE8(sample0.rgb);

    // Deferred: Normals
    vec3 wnormal = normalize(v_normal).xyz;
    gl_FragData[1] = vec4(encodeNormalUint(wnormal), 1.0);

    // Deferred: MatInfo
    gl_FragData[2] = vec4(0.0, 1.0, 0.0, 0.0);

    // DEBUG:
    //vec4 debug = texture2D(Texture0, v_texcoord0.xy);
    //gl_FragColor = vec4(debug.rgb, 1.0); 
}
