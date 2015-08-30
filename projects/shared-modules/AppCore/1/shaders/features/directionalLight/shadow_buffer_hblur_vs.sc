$input a_position, a_texcoord0
$output v_texcoord0, v_position1, v_position2, v_position3, v_position4

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <torque6.sc>

uniform vec4 u_blurParams;
#define u_xOffset u_blurParams.z

void main()
{
	float offset = u_viewTexel.x*u_xOffset;

	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	v_texcoord0 = a_texcoord0;

	v_position1 = vec4(a_texcoord0.x - offset*1.0, a_texcoord0.y,
					   a_texcoord0.x + offset*1.0, a_texcoord0.y
					  );
	v_position2 = vec4(a_texcoord0.x - offset*2.0, a_texcoord0.y,
					   a_texcoord0.x + offset*2.0, a_texcoord0.y
					  );
	v_position3 = vec4(a_texcoord0.x - offset*3.0, a_texcoord0.y,
					   a_texcoord0.x + offset*3.0, a_texcoord0.y
					  );
	v_position4 = vec4(a_texcoord0.x - offset*4.0, a_texcoord0.y,
					   a_texcoord0.x + offset*4.0, a_texcoord0.y
					  );
}
