$input v_texcoord0, v_position1, v_position2, v_position3, v_position4

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "common.sh"

SAMPLER2D(Texture0, 0);

void main()
{
    gl_FragColor = blur9(Texture0, v_texcoord0, v_position1, v_position2, v_position3, v_position4);
}
