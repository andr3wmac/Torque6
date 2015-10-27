$input v_texcoord0

#include <torque6.sc>

uniform mat4 u_sceneInvViewProjMat;
uniform vec4 u_SSAOParams; // [ Bias, Intensity, Scale, Radius ]

SAMPLER2D(Texture0, 0); // Depth
SAMPLER2D(Texture1, 1); // Normals
SAMPLER2D(Texture2, 2); // Noise

vec3 getPosition(vec2 _uv)
{
    float deviceDepth   = texture2D(Texture0, _uv).x;
    float depth         = toClipSpaceDepth(deviceDepth);
    vec3  clip          = vec3(toClipSpace(_uv), depth);
    vec3  wpos          = clipToWorld(u_sceneInvViewProjMat, clip);

    return wpos;
}

float doAmbientOcclusion(vec2 _uv, vec2 _offset, vec3 _wpos, vec3 _normal)
{
    float bias        = u_SSAOParams.x;
    float intensity   = u_SSAOParams.y;
    float scale       = u_SSAOParams.z;

    vec3 diff = getPosition(_uv + _offset) - _wpos;
    vec3 v = normalize(diff);
    float d = length(diff) * scale;
    return max(0.0, dot(_normal, v) - bias) * (1.0/(1.0 + d)) * intensity;
}

void main()
{
    vec2 dir[4];
    dir[0] = vec2(1.0, 0.0);
    dir[1] = vec2(-1.0, 0.0);
    dir[2] = vec2(0.0, 1.0);
    dir[3] = vec2(0.0, -1.0);

    float deviceDepth   = texture2D(Texture0, v_texcoord0).x;
    float depth         = toClipSpaceDepth(deviceDepth);
    vec3  clip          = vec3(toClipSpace(v_texcoord0), depth);
    vec3  position      = clipToWorld(u_sceneInvViewProjMat, clip);
    vec3  normal        = decodeNormalUint(texture2D(Texture1, v_texcoord0).xyz);

    vec3 noise = texture2D(Texture2, (v_texcoord0 * u_viewRect.zw) / vec2(4.0, 4.0)).xyz * 2.0 - 1.0;

    float ao = 0.0f;
    float radius = u_SSAOParams.w;
    int iterations = 4;
    for (int j = 0; j < iterations; ++j)
    {
      vec2 coord1 = reflect(dir[j], noise.xy) * radius;
      vec2 coord2 = vec2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);
      
      ao += doAmbientOcclusion(v_texcoord0, coord1 * 0.25, position, normal);
      ao += doAmbientOcclusion(v_texcoord0, coord2 * 0.5, position, normal);
      ao += doAmbientOcclusion(v_texcoord0, coord1 * 0.75, position, normal);
      ao += doAmbientOcclusion(v_texcoord0, coord2, position, normal);
    }
    ao /= float(iterations) * 4.0;

    gl_FragColor = vec4(vec3_splat(1.0 - ao), 1.0);
}
