$input a_position, a_texcoord0
$output v_texcoord0, v_position1, v_position2, v_position3, v_position4

/*
 * Copyright 2013-2014 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <torque6.sc>

uniform vec4 u_blurParams;
#define u_yOffset u_blurParams.w

void main()
{
	float offset = u_viewTexel.y*u_yOffset;

	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	v_texcoord0 = a_texcoord0;

	v_position1 = vec4(a_texcoord0.x, a_texcoord0.y - offset*1.0,
					   a_texcoord0.x, a_texcoord0.y + offset*1.0
					  );
	v_position2 = vec4(a_texcoord0.x, a_texcoord0.y - offset*2.0,
					   a_texcoord0.x, a_texcoord0.y + offset*2.0
					  );
	v_position3 = vec4(a_texcoord0.x, a_texcoord0.y - offset*3.0,
					   a_texcoord0.x, a_texcoord0.y + offset*3.0
					  );
	v_position4 = vec4(a_texcoord0.x, a_texcoord0.y - offset*4.0,
					   a_texcoord0.x, a_texcoord0.y + offset*4.0
					  );
}
