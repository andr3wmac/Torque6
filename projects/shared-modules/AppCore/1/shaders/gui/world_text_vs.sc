$input a_position, a_color0, a_texcoord0
$output v_color0, v_texcoord0, v_position

#include <torque6.sc>

uniform vec4 u_time;
uniform vec4 textParams;

void main()
{
    // Standard: Vertex Position
    vec4 vertPosition = vec4(a_position, 1.0);

    // Standard: UV Coordinates
    v_texcoord0 = a_texcoord0 + vec2(textParams.x * u_time.x, textParams.y * u_time.x);

#if BGFX_SHADER_LANGUAGE_GLSL
    v_texcoord0.y = 1.0 - v_texcoord0.y;
#endif

    // Vertex Color
    v_color0 = a_color0;

    // OIT
    v_position = mul(u_modelViewProj, vertPosition);

    // Output position
    gl_Position = v_position;
}
