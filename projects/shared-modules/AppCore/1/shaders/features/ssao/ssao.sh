vec4 ssao_blur(vec2 _uv, vec2 _dir, sampler2D _occlusionTexture, sampler2D _normalTexture)
{
	float weights[9] =
	{
		0.013519569015984728,
		0.047662179108871855,
		0.11723004402070096,
		0.20116755999375591,
		0.240841295721373,
		0.20116755999375591,
		0.11723004402070096,
		0.047662179108871855,
		0.013519569015984728
	};
	float indices[9] = { -4.0, -3.0, -2.0, -1.0, 0.0, +1.0, +2.0, +3.0, +4.0 };
	vec2  step       = _dir / u_viewRect.zw;

	vec3 normal[9];
	normal[0] = normalize(decodeNormalUint(texture2D(_normalTexture, _uv + (indices[0] * step)).xyz));
	normal[1] = normalize(decodeNormalUint(texture2D(_normalTexture, _uv + (indices[1] * step)).xyz));
	normal[2] = normalize(decodeNormalUint(texture2D(_normalTexture, _uv + (indices[2] * step)).xyz));
	normal[3] = normalize(decodeNormalUint(texture2D(_normalTexture, _uv + (indices[3] * step)).xyz));
	normal[4] = normalize(decodeNormalUint(texture2D(_normalTexture, _uv + (indices[4] * step)).xyz));
	normal[5] = normalize(decodeNormalUint(texture2D(_normalTexture, _uv + (indices[5] * step)).xyz));
	normal[6] = normalize(decodeNormalUint(texture2D(_normalTexture, _uv + (indices[6] * step)).xyz));
	normal[7] = normalize(decodeNormalUint(texture2D(_normalTexture, _uv + (indices[7] * step)).xyz));
	normal[8] = normalize(decodeNormalUint(texture2D(_normalTexture, _uv + (indices[8] * step)).xyz));

	float total_weight      = 1.0;
	float discard_threshold = 0.85;

	int i;
	for( i = 0; i < 9; ++i )
	{
		if( dot(normal[i], normal[4]) < discard_threshold )
		{
			total_weight -= weights[i];
			weights[i] = 0.0;
		}
	}

	vec4 res = vec4_splat(0.0);
	for( i = 0; i < 9; ++i )
	{
		res += texture2D(_occlusionTexture, _uv + indices[i] * step) * weights[i];
	}

	res /= total_weight;
	return res;
}
