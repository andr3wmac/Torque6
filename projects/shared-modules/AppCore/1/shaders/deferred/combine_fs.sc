$input v_texcoord0

#include <torque6.sc>

uniform mat4 u_sceneInvViewProjMat;
uniform vec4 u_camPos;

SAMPLER2D(Texture0, 0); // Albedo
SAMPLER2D(Texture1, 1); // Normals
SAMPLER2D(Texture2, 2); // Material Info
SAMPLER2D(Texture3, 3); // Depth Buffer
SAMPLER2D(Texture4, 4); // Light Buffer

SAMPLERCUBE(u_ambientCube, 5);
SAMPLERCUBE(u_ambientIrrCube, 6);

#define LIGHTING_AMBIENT_CUBE
#include <lighting.sh>

void main()
{
    // World-Space Position
    float deviceDepth   = texture2D(Texture3, v_texcoord0).x;
    float depth         = toClipSpaceDepth(deviceDepth);
    vec3  clip          = vec3(toClipSpace(v_texcoord0), depth);
    vec3  wpos          = clipToWorld(u_sceneInvViewProjMat, clip);

    // Albedo
    vec3 albedo = decodeRGBE8(texture2D(Texture0, v_texcoord0));

    // Normals
    vec3 normal = decodeNormalUint(texture2D(Texture1, v_texcoord0).xyz);

    // View Direction
    vec3 viewDir = normalize(u_camPos.xyz - wpos);

    // Material Parameters
    vec4  matInfo   = texture2D(Texture2, v_texcoord0);
    float metallic  = matInfo.r;
    float roughness = matInfo.g;

    // Calculate surface color based on metallic value.
    vec3 reflectivity = vec3_splat(metallic);
    reflectivity = clamp(reflectivity, 0.0, 0.999);
    vec3 surfaceReflect = mix(vec3_splat(0.04), albedo, reflectivity);
    vec3 surfaceColor   = albedo * (vec3_splat(1.0) - reflectivity);

    // Direct Lighting
    vec3 lightColor = texture2D(Texture4, v_texcoord0).rgb;
    vec3 dirLight = surfaceColor * lightColor;

    // Environment Lighting
    vec3 ambLight = ambientEnvLighting(viewDir, normal, albedo, metallic, roughness);

    gl_FragColor = encodeRGBE8(dirLight + ambLight);
}
