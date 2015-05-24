vec3  a_position  : POSITION0;
vec3  a_normal    : NORMAL0;
vec4  a_tangent   : TANGENT0;
vec4  a_bitangent : BINORMAL0;
vec4  a_color0    : COLOR0;
vec4  a_color1    : COLOR1;
vec2  a_texcoord0 : TEXCOORD0;
vec2  a_texcoord1 : TEXCOORD1;
vec4  a_weight    : BLENDWEIGHT0;
ivec4 a_indices   : BLENDINDICES0;

vec4 i_data0     : TEXCOORD7;
vec4 i_data1     : TEXCOORD6;
vec4 i_data2     : TEXCOORD5;
vec4 i_data3     : TEXCOORD4;
vec4 i_data4     : TEXCOORD3;

vec4 v_color0      : COLOR0    = vec4(0.0, 0.0, 0.0, 1.0);
vec4 v_color1      : COLOR1    = vec4(0.0, 0.0, 0.0, 1.0);

vec3 v_normal      : NORMAL0   = vec3(0.0, 0.0, 1.0);
vec3 v_tangent     : TANGENT0  = vec3(1.0, 0.0, 0.0);
vec3 v_bitangent   : BINORMAL0 = vec3(0.0, 1.0, 0.0);

vec2 v_texcoord0   : TEXCOORD0 = vec2(0.0, 0.0);
vec3 v_wpos        : TEXCOORD1 = vec3(0.0, 0.0, 0.0);
vec4 v_sspos       : TEXCOORD2 = vec4(0.0, 0.0, 0.0, 0.0);
vec3 v_dir         : TEXCOORD3 = vec3(0.0, 0.0, 0.0);
vec4 v_position    : TEXCOORD4 = vec4(0.0, 0.0, 0.0, 0.0);
vec4 v_shadowcoord : TEXCOORD5 = vec4(0.0, 0.0, 0.0, 0.0);

vec4 v_position1 : POSITION1 = vec4(0.0, 0.0, 0.0, 0.0);
vec4 v_position2 : POSITION2 = vec4(0.0, 0.0, 0.0, 0.0);
vec4 v_position3 : POSITION3 = vec4(0.0, 0.0, 0.0, 0.0);
vec4 v_position4 : POSITION4 = vec4(0.0, 0.0, 0.0, 0.0);
