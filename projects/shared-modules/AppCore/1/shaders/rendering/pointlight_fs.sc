$input v_color0, v_texcoord0, v_sspos

#include <torque6.sc>

uniform vec4 singleLightPos;
uniform vec4 singleLightColor;
uniform vec4 singleLightParams;
uniform vec4 u_camPos;

SAMPLER2D(Texture0, 0); // Depth
SAMPLER2D(Texture1, 1); // Normals
SAMPLER2D(Texture2, 2); // Material Info

#include <lighting.sh>

void main()
{
    vec3 sspos          = v_sspos.xyz / v_sspos.w;
    vec2 uv_coords      = toUVSpace( sspos );
    float deviceDepth   = texture2D(Texture0, uv_coords).x;
    float depth         = toClipSpaceDepth(deviceDepth);

    vec3 clip = vec3(sspos.xy, depth);
    vec3 wpos = clipToWorld(u_invViewProj, clip);

    // Normals
    vec3 normal = decodeNormalUint(texture2D(Texture1, uv_coords).xyz);

    // View Direction
    vec3 viewDir = normalize(u_camPos.xyz - wpos);

    // Material Parameters
    vec4  matInfo   = texture2D(Texture2, uv_coords);
    float roughness = matInfo.g;

    vec3 color = calcPointLight(wpos, viewDir, normal, roughness, singleLightPos.xyz, singleLightColor.xyz, singleLightParams);
    gl_FragColor = vec4(color, 1.0);
}
