// Cascaded ShadowMaps
float texcoordInRange(vec2 _texcoord)
{
    bool inRange = all(greaterThan(_texcoord, vec2_splat(0.0)))
                && all(lessThan   (_texcoord, vec2_splat(1.0)))
                 ;

    return float(inRange);
}

// Offset Scaling
vec2 getShadowOffsetScales(vec3 N, vec3 L) 
{ 
    float cos_alpha = clamp(dot(N, L), 0.0, 1.0); 
    float offset_scale_N = sqrt(1.0 - cos_alpha * cos_alpha); // sin(acos(L dot N)) 
    float offset_scale_L = offset_scale_N / cos_alpha; // tan(acos(L dot N)) 

    return vec2(offset_scale_N, min(2.0, offset_scale_L)); 
}

// Optimized PCF 5x5
// Based On: http://the-witness.net/news/2013/09/shadow-mapping-summary-part-1/
float nativePCF5x5(sampler2DShadow _shadowMap, vec3 _coord, float _bias)
{
    vec4 shadowMapSize = vec4(1.0 / 2048.0, 1.0 / 2048.0, 2048.0, 2048.0);
    float depth = _coord.z - _bias;

    vec2 offset = vec2(0.5, 0.5);
    vec2 uv = (_coord.xy * shadowMapSize.zw) + offset;
    vec2 base_uv = (floor(uv) - offset) * shadowMapSize.xy;
    vec2 st = fract(uv);

    vec3 uw = vec3( 4.0 - 3.0 * st.x, 7.0, 1.0 + 3.0 * st.x );
    vec3 u  = vec3( (3.0 - 2.0 * st.x) / uw.x - 2.0, (3.0 + st.x) / uw.y, st.x / uw.z + 2.0 );
    u *= shadowMapSize.x;

    vec3 vw = vec3( 4.0 - 3.0 * st.y, 7.0, 1.0 + 3.0 * st.y );
    vec3 v  = vec3( (3.0 - 2.0 * st.y) / vw.x - 2.0, (3.0 + st.y) / vw.y, st.y / vw.z + 2.0 );
    v *= shadowMapSize.y;

    float shadow;
    float sum = 0.0f;

    vec3 accum = uw * vw.x;
    sum += accum.x * shadow2D( _shadowMap, vec3( base_uv + vec2(u.x, v.x), depth) );
    sum += accum.y * shadow2D( _shadowMap, vec3( base_uv + vec2(u.y, v.x), depth) );
    sum += accum.z * shadow2D( _shadowMap, vec3( base_uv + vec2(u.z, v.x), depth) );

    accum = uw * vw.y;
    sum += accum.x * shadow2D( _shadowMap, vec3( base_uv + vec2(u.x, v.y), depth) );
    sum += accum.y * shadow2D( _shadowMap, vec3( base_uv + vec2(u.y, v.y), depth) );
    sum += accum.z * shadow2D( _shadowMap, vec3( base_uv + vec2(u.z, v.y), depth) );

    accum = uw * vw.z;
    sum += accum.x * shadow2D( _shadowMap, vec3( base_uv + vec2(u.x, v.z), depth) );
    sum += accum.y * shadow2D( _shadowMap, vec3( base_uv + vec2(u.y, v.z), depth) );
    sum += accum.z * shadow2D( _shadowMap, vec3( base_uv + vec2(u.z, v.z), depth) );

    shadow = sum / 144.0;
    return shadow;
}
