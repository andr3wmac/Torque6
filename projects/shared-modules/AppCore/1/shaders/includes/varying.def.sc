vec4 v_color0      : COLOR0    = vec4(0.0, 0.0, 0.0, 1.0);
vec4 v_color1      : COLOR1    = vec4(0.0, 0.0, 0.0, 1.0);
vec2 v_texcoord0   : TEXCOORD0 = vec2(0.0, 0.0);
vec3 v_normal      : TEXCOORD1 = vec3(0.0, 1.0, 0.0);
vec3 v_wpos        : TEXCOORD2 = vec3(0.0, 0.0, 0.0);
vec4 v_sspos       : TEXCOORD3 = vec4(0.0, 0.0, 0.0, 0.0);
vec3 v_dir         : TEXCOORD4 = vec3(0.0, 0.0, 0.0);
vec4 v_position    : TEXCOORD5 = vec4(0.0, 0.0, 0.0, 0.0);
vec4 v_shadowcoord : TEXCOORD6 = vec4(0.0, 0.0, 0.0, 0.0);

vec3 a_position  : POSITION;
vec3 a_normal    : NORMAL0;
vec4 a_color0    : COLOR0;
vec4 a_color1    : COLOR1;
vec2 a_texcoord0 : TEXCOORD0;
vec2 a_texcoord1 : TEXCOORD1;

ivec4 a_indices  : BLENDINDICES0;
vec4 a_weights   : BLENDWEIGHT0;

vec4 i_data0     : TEXCOORD3;
vec4 i_data1     : TEXCOORD4;
vec4 i_data2     : TEXCOORD5;
vec4 i_data3     : TEXCOORD6;
vec4 i_data4     : TEXCOORD7;
