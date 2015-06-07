$input v_position

/*
 * Copyright 2013-2014 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <torque6.sc>

void main()
{
    // Variance ShadowMaps
    float depth = v_position.z / v_position.w * 0.5 + 0.5;
    float depthSq = depth * depth;

    gl_FragColor = vec4(packHalfFloat(depth), packHalfFloat(depthSq));
}
