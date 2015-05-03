$input v_color0, v_texcoord0, v_sspos

#include <torque6.sc>

uniform vec4 singleLightPosRadius;
uniform vec4 singleLightColorAttn;

SAMPLER2D(s_depth, 0);

void main()
{
    vec3 sspos          = v_sspos.xyz / v_sspos.w;
    vec2 uv_coords      = toUVSpace( sspos );
	float deviceDepth   = texture2D(s_depth, uv_coords).x;
	float depth         = toClipSpaceDepth(deviceDepth);

	vec3 clip = vec3(sspos.xy, depth);
    vec3 dwpos = clipToWorld(u_invViewProj, clip);

    vec3 color = calcPointLight(dwpos, singleLightPosRadius.xyz, singleLightColorAttn.xyz, singleLightPosRadius.w, singleLightColorAttn.w);
    gl_FragColor = vec4(color, 1.0);
}
