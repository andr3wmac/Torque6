$input a_position, a_texcoord0, a_normal
$output v_texcoord0, v_wpos, v_normal, v_shadowcoord

#include <torque6.sc>

uniform mat4 u_lightMtx;
SAMPLER2D(Texture0, 1);

void main()
{
    // Standard: Vertex Position
    vec4 vertPosition = vec4(a_position, 1.0);

    // Sample texture.
    vec4 color = texture2D(Texture0, a_texcoord0);
    vertPosition.y += color.r;

    // Standard: UV Coordinates
    v_texcoord0 = a_texcoord0;

    // Optional: World-Space Position (used for lighting)
    vec3 wpos = mul(u_model[0], vertPosition).xyz;
    v_wpos = wpos;

    // Normals
    vec3 normal = a_normal * 2.0 - 1.0;
	vec3 wnormal = mul(u_model[0], vec4(a_normal.xyz, 0.0) ).xyz;
    v_normal = wnormal.xyz;

    // Shadow Map Coord, used to receive shadows
	const float shadowMapOffset = 0.001;
	vec3 posOffset = a_position + normal.xyz * shadowMapOffset;
	v_shadowcoord = mul(u_lightMtx, vec4(posOffset, 1.0));

    // Standard: Output Final Vertex Position
    gl_Position = mul(u_modelViewProj, vertPosition);
}
