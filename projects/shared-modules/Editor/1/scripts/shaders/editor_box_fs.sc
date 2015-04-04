$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

void main()
{
    vec4 base_color = vec4(1.0, 1.0, 1.0, 1.0);

    float x_val = 0.5 - v_texcoord0.x;
    x_val = abs(x_val);
    x_val = clamp(x_val, 0.45, 0.5);
    x_val = x_val - 0.45;
    x_val = x_val * 20;

    float y_val = 0.5 - v_texcoord0.y;
    y_val = abs(y_val);
    y_val = clamp(y_val, 0.45, 0.5);
    y_val = y_val - 0.45;
    y_val = y_val * 20;

    float val1 = clamp(x_val + y_val, 0, 1);
    float val2 = clamp(x_val * y_val, 0, 1);
    float val = val1 - val2;
	gl_FragColor = base_color * val;
}
