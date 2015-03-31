$input v_texcoord0

#include <bgfx_shader.sh>

#if BGFX_SHADER_LANGUAGE_HLSL
    #define FXAA_HLSL_3 1
#else
    #define FXAA_GLSL_130 1
#endif

#define FXAA_PC 1
#define FXAA_GREEN_AS_LUMA 1
#define FXAA_QUALITY__PRESET 20

#include <nvidia_fxaa.sc>

SAMPLER2D(s_input, 0);

void main()
{
    vec3 antialiased_color = FxaaPixelShader(v_texcoord0.xy, s_input, u_viewTexel.xy, 1.0, 0.125, 0.0625);

    vec4 colorSample = texture2D(s_input, v_texcoord0);
    gl_FragColor = vec4(antialiased_color, colorSample.a);
}
