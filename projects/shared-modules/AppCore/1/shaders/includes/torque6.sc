#include "bgfx_shader.sh"
#include "shaderlib.sh"

// Lighting Equations - Point Light
vec3 calcPointLight(vec3 wpos, vec3 light_pos, vec3 light_color, float light_radius, float light_attn)
{
	vec3 lp = light_pos - wpos;
	float attn = 1.0 - smoothstep(light_attn, 1.0, length(lp) / light_radius);
	vec3 rgb = light_color * attn;
	return rgb;
}

// Lighting Equations - Directional Light
vec3 calcDirectionalLight(vec3 _normal, vec3 _lightDir, vec3 _lightColor)
{
    // Calculate the amount of light on this pixel.
    float lightIntensity = clamp(dot(_normal, _lightDir), 0.0, 1.0);

    // Calculate light color
    return _lightColor * lightIntensity;
}
