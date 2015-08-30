$input v_color0, v_texcoord0, v_sspos

#include <torque6.sc>
#include <shadows.sc>

uniform vec4 dirLightDirection;
uniform vec4 dirLightColor;
uniform mat4 u_sceneInvViewProjMat;
uniform vec4 u_camPos;

SAMPLER2D(Texture0, 0); // Normals
SAMPLER2D(Texture1, 1); // Material Info
SAMPLER2D(Texture2, 2); // Depth
SAMPLER2D(Texture3, 3); // Shadows

#include <lighting.sh>

void main()
{
    // World-Space Position
    float deviceDepth   = texture2D(Texture2, v_texcoord0).x;
    float depth         = toClipSpaceDepth(deviceDepth);
    vec3  clip          = vec3(toClipSpace(v_texcoord0), depth);
    vec3  wpos          = clipToWorld(u_sceneInvViewProjMat, clip);

    // Normals
    vec3 normal = decodeNormalUint(texture2D(Texture0, v_texcoord0).xyz);

    // View Direction
    vec3 viewDir = normalize(u_camPos.xyz - wpos);

    // Material Info
    vec4 matInfo = texture2D(Texture1, v_texcoord0);

    // Shadows
    float visibility = texture2D(Texture3, v_texcoord0).x;

    // Directional Light
    vec3 lightColor = calcDirectionalLight(viewDir,
                                           normal,
                                           matInfo.g, // Roughness
                                           dirLightDirection.xyz, 
                                           dirLightColor.rgb, 
                                           visibility);

    // Output
    gl_FragColor = vec4(lightColor, 1.0);
}
