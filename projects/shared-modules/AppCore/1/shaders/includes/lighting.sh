// Frostbite Attenuation
// Source: http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
float smoothDistanceAtt( float squaredDistance, float invSqrAttRadius )
{
    float factor        = squaredDistance * invSqrAttRadius;
    float smoothFactor  = clamp(1.0 - factor * factor, 0.0, 1.0);

    return smoothFactor * smoothFactor;
}

float getDistanceAtt( vec3 unormalizedLightVector, float invSqrAttRadius )
{
    float sqrDist       = dot(unormalizedLightVector, unormalizedLightVector);
    float attenuation   = 1.0 / (max(sqrDist, 0.01*0.01));
    attenuation         *= smoothDistanceAtt(sqrDist, invSqrAttRadius);

    return attenuation;
}

float getAngleAtt( vec3 normalizedLightVector, vec3 lightDir, float lightAngleScale, float lightAngleOffset )
{
    float cd            = dot(lightDir, normalizedLightVector);
    float attenuation   = clamp(cd * lightAngleScale + lightAngleOffset, 0.0, 1.0);
    attenuation         *= attenuation;

    return attenuation;
}

// GGX Specular
// Source: http://www.filmicworlds.com/images/ggx-opt/optimized-ggx.hlsl
float G1V(float dotNV, float k)
{
    return 1.0 / (dotNV * (1.0 - k) + k);
}

vec3 specularGGX(vec3 N, vec3 V, vec3 L, float roughness, float F0)
{
    float alpha = roughness * roughness;

    vec3 H = normalize(V + L);

    float dotNL = clamp(dot(N,L), 0.0, 1.0);
    float dotNV = clamp(dot(N,V), 0.0, 1.0);
    float dotNH = clamp(dot(N,H), 0.0, 1.0);
    float dotLH = clamp(dot(L,H), 0.0, 1.0);

    // D
    float alphaSqr = alpha * alpha;
    float pi = 3.14159;
    float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
    float D = alphaSqr / (pi * denom * denom);

    // F
    float dotLH5 = pow(1.0 - dotLH, 5);
    float F = F0 + (1.0 - F0) * (dotLH5);

    // V
    float k = alpha / 2.0f;
    float vis = G1V(dotNL, k) * G1V(dotNV, k);

    vec3 specular = vec3_splat(dotNL * D * F * vis);
    return specular;
}

vec3 directLighting(vec3 _view, vec3 _normal, float _roughness, vec3 _lightDir, vec3 _lightColor, float _shadow)
{
	vec3 v = _view;
	vec3 n = normalize(_normal);
	vec3 l = normalize(_lightDir);

    // Lambert Diffuse
    float ndotl = clamp(dot(n, l), 0.0, 1.0);
    vec3 diffuse = _lightColor * ndotl;

    // GGX Specular
    vec3 specular = specularGGX(n, v, l, _roughness, 1.0);

    // Combine.
    return (diffuse + (specular * _lightColor)) * vec3_splat(_shadow); 
}

// Lighting Equations - Point Light
vec3 calcPointLight(vec3 _worldPos, vec3 _view, vec3 _normal, float _roughness, vec3 _lightPos, vec3 _lightColor, vec4 _lightParams)
{
	vec3  lp            = _lightPos - _worldPos;
    vec3  lightDir      = normalize(lp);
    float invSquareAttn = _lightParams.x;
	float intensity     = _lightParams.y;

    float attn = 1.0;
    attn *= getDistanceAtt(lp, invSquareAttn);
    attn *= getAngleAtt(lightDir, lightDir, 1.0, 0.0);

    vec3 lc = directLighting(_view, _normal, _roughness, lightDir, _lightColor, attn);
	return lc * intensity;
}

// Lighting Equations - Directional Light
vec3 calcDirectionalLight(vec3 _view, vec3 _normal, float _roughness, vec3 _lightDir, vec3 _lightColor, float _shadow)
{
    vec3 lc = directLighting(_view, _normal, _roughness, _lightDir, _lightColor, _shadow);
    return lc;
}

// Based on shaders by Dario Manesku
// Source: https://github.com/dariomanesku/cmftStudio/

// Lighting Equations - Fresnel
vec3 fresnel(vec3 _reflectivity, float _dot, float _strength)
{
	return _reflectivity + (1.0 - _reflectivity) * pow(1.0 - _dot, 5.0) * _strength;
}

// Lighting Equations - Ambient Environment
vec3 ambientEnvLighting(vec3 view, 
                        vec3 normal,
                        vec3 albedo,
                        float metallic, 
                        float roughness)
{
    vec3  v     = view;
    vec3  n     = normalize(normal);
    float ndotv = clamp(dot(n, v), 0.0, 1.0);

    // Reflectivity
    vec3 reflectivity = vec3_splat(metallic);
    reflectivity = clamp(reflectivity, 0.0, 0.999);

    // Surface Values
    vec3  surfaceReflect = mix(vec3_splat(0.04), albedo, reflectivity);
    vec3  surfaceColor   = albedo * (vec3_splat(1.0) - reflectivity);
    float surfaceGloss   = 1.0 - roughness;
    vec3  surfaceFresnel = fresnel(surfaceReflect, ndotv, surfaceGloss);

    // Radiance (specular) and Irradiance (diffuse)
    vec3 radiance   = vec3(0.1, 0.1, 0.1);
    vec3 irradiance = vec3(0.1, 0.1, 0.1);

#ifdef LIGHTING_AMBIENT_CUBE
    vec3  r     = 2.0 * ndotv * n - v; // reflect(v, n);
    vec3  cubeR = normalize(r);
    vec3  cubeN = n;
    float lod   = roughness * 6.0;

    cubeR.yz = vec2(cubeR.z, cubeR.y * -1.0); // Adjust to Y-Up
    radiance   = textureCubeLod(u_radianceCube, cubeR, lod).xyz;
    cubeN.yz = vec2(cubeN.z, cubeN.y * -1.0); // Adjust to Y-Up
    irradiance = textureCube(u_irradianceCube, cubeN).xyz;
#endif

    vec3 ambdiff = surfaceColor * irradiance;
    vec3 ambspec = surfaceFresnel * radiance;

    return ambdiff + ambspec;
}

