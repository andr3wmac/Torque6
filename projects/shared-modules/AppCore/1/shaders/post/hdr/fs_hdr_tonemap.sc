$input v_texcoord0, v_position1, v_position2, v_position3, v_position4

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "common.sh"

SAMPLER2D(Texture0, 0); // Color
SAMPLER2D(Texture1, 1); // Lum
SAMPLER2D(Texture2, 2); // Blur

void main()
{
    vec3 rgb = decodeRGBE8(texture2D(Texture0, v_texcoord0) );
    float lum = clamp(decodeRE8(texture2D(Texture1, v_texcoord0) ), 0.1, 0.7);

    vec3 Yxy = convertRGB2Yxy(rgb);

    float middleGray = u_tonemap.x;
    float whiteSqr   = u_tonemap.y;
    float treshold   = u_tonemap.z;
    float offset     = u_tonemap.w;

    float lp = Yxy.x * middleGray / (lum + 0.0001);
    Yxy.x = reinhard2(lp, whiteSqr);

    rgb = convertYxy2RGB(Yxy);

    vec4 blur = blur9(Texture2
                    , v_texcoord0
                    , v_position1
                    , v_position2
                    , v_position3
                    , v_position4
                    );

    rgb += 0.6 * blur.xyz;

    gl_FragColor = toGamma(vec4(rgb, 1.0));
}
