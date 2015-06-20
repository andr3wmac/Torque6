// Based on shaders by Dario Manesku.

// Lighting Equations
vec3 fresnel(vec3 _cspec, float _dot)
{
	return _cspec + (1.0 - _cspec) * pow(1.0 - _dot, 5);
}

vec3 lit(vec3 lightColor, vec3 specColor, vec3 normal, float metalness, float glossiness, vec3 view, vec3 lightDir, float shadow)
{
	vec3 v = view;
	vec3 n = normalize(normal);
	vec3 l = normalize(lightDir);
	vec3 h = normalize(v + l);

	float ndotl = clamp(dot(n, l), 0.0, 1.0); //diff
	float ndoth = clamp(dot(n, h), 0.0, 1.0); //spec
	float vdoth = clamp(dot(v, h), 0.0, 1.0); //spec fresnel
	float ndotv = clamp(dot(n, v), 0.0, 1.0); //env spec fresnel

    vec3 kd = lightColor; // Diffuse Color
	vec3 ks = specColor;  // Specular Color

    vec3 r = 2.0 * ndotv * n - v; // reflect(v, n);

	vec3 cubeR = normalize(r);
	vec3 cubeN = n;

    float mipLevel = min((1.0 - glossiness) * 11.0 + 1.0, 8.0);

    vec3 ambient    = vec3(0.1, 0.1, 0.1);
    vec3 ambientIrr = vec3(0.1, 0.1, 0.1);
#ifdef LIGHTING_AMBIENT_CUBE
    ambient    = textureCubeLod(u_ambientCube, cubeR, mipLevel).xyz;
    ambientIrr = textureCube(u_ambientIrrCube, cubeN).xyz;
#endif

	vec3 cs = ks * metalness;
	vec3 cd = kd * (1.0 - cs);

	vec3 diff = cd;
	float pwr = exp2(glossiness * 11.0 + 1.0);
	vec3 spec = cs * pow(ndoth, pwr) * ((pwr + 8.0) / 8.0) * fresnel(cs, vdoth);

    vec3 ambspec = fresnel(cs, ndotv) * ambient;
	vec3 ambdiff = cd * ambientIrr;

    vec3 lc = (diff + (spec * 0.5)) * ndotl;
    vec3 ec = (ambdiff + ambspec);

    return (lc * shadow) + ec;
}

// Lighting Equations - Point Light
vec3 calcPointLight(vec3 wpos, vec3 normal, vec3 view, vec3 light_pos, vec3 light_color, float light_radius, float light_attn)
{
	vec3 lp = light_pos - wpos;
	float attn = 1.0 - smoothstep(light_attn, 1.0, length(lp) / light_radius);

    vec3 color = vec3(1.0, 1.0, 1.0);
    vec3 lightDir = normalize(lp);
    vec3 lc = lit(light_color, color, normal, 0.2, 0.8, view, lightDir, 1.0);

	vec3 rgb = light_color * lc * attn;
	return rgb;
}

// Lighting Equations - Directional Light
vec3 calcDirectionalLight(vec3 color, 
                          vec3 normal, 
                          float metallic, 
                          float roughness, 
                          vec3 view, 
                          vec3 lightDir, 
                          vec3 lightColor, 
                          vec3 lightAmbient,
                          float shadow)
{
    vec3 lc = lit(lightColor, color, normal, metallic, 1.0 - roughness, view, lightDir, shadow);
    return lc;
}

