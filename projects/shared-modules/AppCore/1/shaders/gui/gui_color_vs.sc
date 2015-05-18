$input a_position, a_color0
$output v_color0

/*
 * Copyright 2014 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <bgfx_shader.sh>

void main()
{
    v_color0 = a_color0;
    gl_Position = mul(u_modelViewProj, vec4(a_position.xyz, 1.0) );
}
