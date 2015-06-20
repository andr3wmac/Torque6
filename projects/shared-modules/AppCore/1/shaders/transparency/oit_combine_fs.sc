$input v_texcoord0

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <torque6.sc>

SAMPLER2D(Texture0, 0); // Backbuffer
SAMPLER2D(Texture1, 1); // Transparency Accumulation
SAMPLER2D(Texture2, 2); // Transparency Weight

void main()
{
    vec3  color      = decodeRGBE8(texture2D(Texture0, v_texcoord0));
    vec4  accum      = texture2D(Texture1, v_texcoord0);
	float opacity    = texture2D(Texture2, v_texcoord0).x;
    vec4  tColor     = vec4(accum.rgb / clamp(accum.a, 1e-4, 5e4), opacity);

    // Blend.
    vec3 finalColor = (color.rgb * opacity) + (tColor.rgb * (1.0 - opacity));  
    //vec3 finalColor = vec3(opacity, opacity, opacity);

	gl_FragColor = encodeRGBE8(finalColor);
}
