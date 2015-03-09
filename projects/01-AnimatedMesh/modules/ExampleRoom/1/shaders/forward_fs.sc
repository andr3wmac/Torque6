$input v_texcoord0, v_wpos, v_normal

#include <torque6.sc>

SAMPLER2D(Texture0, 0);

uniform vec3 dirLightDirection;
uniform vec3 dirLightColor;

uniform vec4 lightPosRadius[4];
uniform vec4 lightColorAttn[4];

void main()
{
    // Texture Color
    vec4 color = texture2D(Texture0, v_texcoord0);

    // Total Light Color
	vec3 lightColor;

    // Directional Light
    lightColor =  calcDirectionalLight(v_normal.xyz, dirLightDirection, dirLightColor);

    // Point Lights
	lightColor += calcPointLight(v_wpos, lightPosRadius[0].xyz, lightColorAttn[0].xyz, lightPosRadius[0].w, lightColorAttn[0].w);
	lightColor += calcPointLight(v_wpos, lightPosRadius[1].xyz, lightColorAttn[1].xyz, lightPosRadius[1].w, lightColorAttn[1].w);
	lightColor += calcPointLight(v_wpos, lightPosRadius[2].xyz, lightColorAttn[2].xyz, lightPosRadius[2].w, lightColorAttn[2].w);
	lightColor += calcPointLight(v_wpos, lightPosRadius[3].xyz, lightColorAttn[3].xyz, lightPosRadius[3].w, lightColorAttn[3].w);

    gl_FragColor = vec4(color.rgb * lightColor.rgb, 1.0);
}
