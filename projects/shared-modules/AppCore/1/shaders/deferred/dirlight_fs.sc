$input v_color0, v_texcoord0, v_sspos

#include <torque6.sc>

uniform vec3 dirLightDirection;
uniform vec3 dirLightColor;
uniform vec3 dirLightAmbient;

SAMPLER2D(Texture0, 0); // Normals
SAMPLER2D(Texture1, 1); // Depth

void main()
{
    //vec3 sspos          = v_sspos.xyz / v_sspos.w;
    //vec2 uv_coords      = toUVSpace( sspos );
	//float deviceDepth   = texture2D(Texture1, uv_coords).x;
	//float depth         = toClipSpaceDepth(deviceDepth);

	//vec3 clip = vec3(sspos.xy, depth);
    //vec3 dwpos = clipToWorld(u_invViewProj, clip);

    // Normals
    vec3 normal = decodeNormalUint(texture2D(Texture0, v_texcoord0).xyz);

    // Directional Light
    vec3 lightColor = calcDirectionalLight(normal.xyz, dirLightDirection, dirLightColor);

    gl_FragColor = vec4(lightColor.rgb, 1.0);
}
