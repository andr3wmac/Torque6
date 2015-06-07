$input v_texcoord0, v_position1, v_position2, v_position3, v_position4

/*
 * Copyright 2013-2014 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <torque6.sc>
#include <shadows.sc>
SAMPLER2D(Texture0, 0);

void main()
{
	gl_FragColor = blur9VSM(Texture0, v_texcoord0, v_position1, v_position2, v_position3, v_position4);
}

