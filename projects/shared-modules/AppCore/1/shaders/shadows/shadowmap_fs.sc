$input v_position

/*
 * Copyright 2013-2014 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <torque6.sc>

void main()
{
	float depth = v_position.z/v_position.w * 0.5 + 0.5;
	gl_FragColor = packFloatToRgba(depth);
}
