$input v_color0, v_texcoord0, v_sspos

#include <torque6.sc>
#include <shadows.sc>

uniform vec3 dirLightDirection;
uniform vec3 dirLightColor;
uniform vec3 dirLightAmbient;

uniform mat4 u_sceneViewMtx;
uniform mat4 u_sceneInvViewMtx;
uniform vec3 u_camPos;
uniform mat4 u_lightMtx;

SAMPLER2D(Texture0, 0); // Color
SAMPLER2D(Texture1, 1); // Normals
SAMPLER2D(Texture2, 2); // Material Info
SAMPLER2D(Texture3, 3); // Depth
SAMPLER2D(Texture4, 4); // ShadowMap

void main()
{
    // World-Space Position
	float deviceDepth   = texture2D(Texture3, v_texcoord0).x;
	float depth         = toClipSpaceDepth(deviceDepth);
	vec3 clip           = vec3(toClipSpace(v_texcoord0), depth);
    vec3 wpos           = clipToWorld(u_sceneInvViewMtx, clip);

    // Color
    vec3 color = decodeRGBE8(texture2D(Texture0, v_texcoord0));

    // Normals
    vec3 normal = decodeNormalUint(texture2D(Texture1, v_texcoord0).xyz);

    // View Direction
    vec3 viewDir = normalize(u_camPos - wpos);

    // Material Info
    vec4 matInfo = texture2D(Texture2, v_texcoord0);

    // Directional Light
    vec3 lightColor = calcDirectionalLight(color,
                                           normal,
                                           matInfo.r, // Metallic 
                                           matInfo.g, // Roughness
                                           viewDir,
                                           dirLightDirection, 
                                           dirLightColor, 
                                           dirLightAmbient);

    // Shadow Map Coord, used to receive shadows
	const float shadowMapOffset = 0.001;
	vec3 posOffset = wpos + normal.xyz * shadowMapOffset;
	vec4 shadowcoord = mul(u_lightMtx, vec4(posOffset, 1.0));
    float shadowMapBias = 0.000;
	vec2 texelSize = vec2_splat(1.0/512.0);
	float visibility = PCF(Texture4, shadowcoord, shadowMapBias, texelSize);
    //lightColor = lightColor * visibility;

    // Output
    gl_FragColor = vec4(lightColor, 1.0);
}
