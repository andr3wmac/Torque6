/*
 * Copyright 2013-2014 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

// Cascaded ShadowMaps
float texcoordInRange(vec2 _texcoord)
{
    bool inRange = all(greaterThan(_texcoord, vec2_splat(0.0)))
                && all(lessThan   (_texcoord, vec2_splat(1.0)))
                 ;

    return float(inRange);
}

// Variance ShadowMaps
float VSM(sampler2D _sampler, vec4 _shadowCoord, float _bias, float _depthMultiplier, float _minVariance)
{
    vec2 texCoord = _shadowCoord.xy/_shadowCoord.w;

    bool outside = any(greaterThan(texCoord, vec2_splat(1.0)))
                || any(lessThan   (texCoord, vec2_splat(0.0)))
                 ;

    if (outside)
    {
        return 1.0;
    }

    float receiver = (_shadowCoord.z-_bias)/_shadowCoord.w * _depthMultiplier;
    vec4 rgba = texture2D(_sampler, texCoord);
    vec2 occluder = vec2(unpackHalfFloat(rgba.rg), unpackHalfFloat(rgba.ba)) * _depthMultiplier;

    if (receiver < occluder.x)
    {
        return 1.0;
    }

    float variance = max(occluder.y - (occluder.x*occluder.x), _minVariance);
    float d = receiver - occluder.x;

    float visibility = variance / (variance + d*d);

    return visibility;
}

// Variance ShadowMaps Blur
vec4 blur9VSM(sampler2D _sampler, vec2 _uv0, vec4 _uv1, vec4 _uv2, vec4 _uv3, vec4 _uv4)
{
#define _BLUR9_WEIGHT_0 1.0
#define _BLUR9_WEIGHT_1 0.9
#define _BLUR9_WEIGHT_2 0.55
#define _BLUR9_WEIGHT_3 0.18
#define _BLUR9_WEIGHT_4 0.1
#define _BLUR9_NORMALIZE (_BLUR9_WEIGHT_0+2.0*(_BLUR9_WEIGHT_1+_BLUR9_WEIGHT_2+_BLUR9_WEIGHT_3+_BLUR9_WEIGHT_4) )
#define BLUR9_WEIGHT(_x) (_BLUR9_WEIGHT_##_x/_BLUR9_NORMALIZE)

	vec2 blur;
	vec4 val;
	val = texture2D(_sampler, _uv0) * BLUR9_WEIGHT(0);
	blur = vec2(unpackHalfFloat(val.rg), unpackHalfFloat(val.ba));
	val = texture2D(_sampler, _uv1.xy) * BLUR9_WEIGHT(1);
	blur += vec2(unpackHalfFloat(val.rg), unpackHalfFloat(val.ba));
	val = texture2D(_sampler, _uv1.zw) * BLUR9_WEIGHT(1);
	blur += vec2(unpackHalfFloat(val.rg), unpackHalfFloat(val.ba));
	val = texture2D(_sampler, _uv2.xy) * BLUR9_WEIGHT(2);
	blur += vec2(unpackHalfFloat(val.rg), unpackHalfFloat(val.ba));
	val = texture2D(_sampler, _uv2.zw) * BLUR9_WEIGHT(2);
	blur += vec2(unpackHalfFloat(val.rg), unpackHalfFloat(val.ba));
	val = texture2D(_sampler, _uv3.xy) * BLUR9_WEIGHT(3);
	blur += vec2(unpackHalfFloat(val.rg), unpackHalfFloat(val.ba));
	val = texture2D(_sampler, _uv3.zw) * BLUR9_WEIGHT(3);
	blur += vec2(unpackHalfFloat(val.rg), unpackHalfFloat(val.ba));
	val = texture2D(_sampler, _uv4.xy) * BLUR9_WEIGHT(4);
	blur += vec2(unpackHalfFloat(val.rg), unpackHalfFloat(val.ba));
	val = texture2D(_sampler, _uv4.zw) * BLUR9_WEIGHT(4);
	blur += vec2(unpackHalfFloat(val.rg), unpackHalfFloat(val.ba));

	return vec4(packHalfFloat(blur.x), packHalfFloat(blur.y));
}
