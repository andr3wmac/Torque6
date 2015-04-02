$input v_position, v_texcoord0

#include <torque6.sc>

void main()
{
    vec2 divided_uv = floor(v_texcoord0.xy * 2) * 0.5;
    //gl_FragColor = vec4(divided_uv.x, divided_uv.y, 0.0, 1.0);
    gl_FragColor = vec4(v_texcoord0.x, v_texcoord0.y, 0.0, 1.0);
}
