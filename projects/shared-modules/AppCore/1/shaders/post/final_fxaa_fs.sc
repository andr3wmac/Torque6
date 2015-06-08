$input v_texcoord0

#include <torque6.sc>

#define FXAA_PC 1
#define FXAA_GLSL_120 1
#define FXAA_GREEN_AS_LUMA 1
#define FXAA_QUALITY__PRESET 39
#define FXAA_FAST_PIXEL_OFFSET 0
#define FXAA_GATHER4_ALPHA 0

#include <nvidia_fxaa.sc>

SAMPLER2D(s_input, 0);

void main()
{
    vec3 antialiased_color = FxaaPixelShader(v_texcoord0.xy, s_input, u_viewTexel.xy, 1.0, 0.063, 0.0312).rgb;
    gl_FragColor = vec4(antialiased_color, 1.0);
}
