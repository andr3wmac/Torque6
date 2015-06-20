$input v_texcoord0

#include <torque6.sc>

uniform mat4 u_sceneViewMat;
uniform mat4 u_sceneProjMat;
uniform mat4 u_sceneInvProjMat;
uniform mat4 u_sceneViewProjMat;
uniform mat4 u_sceneInvViewProjMat;
uniform vec4 u_camPos;

SAMPLER2D(Texture0, 0); // Color
SAMPLER2D(Texture1, 1); // Normals
SAMPLER2D(Texture2, 2); // Material Info
SAMPLER2D(Texture3, 3); // Depth
SAMPLER2D(Texture4, 4); // Post Input

void main()
{
    // World-Space Position
    float deviceDepth   = texture2D(Texture3, v_texcoord0).x;
    float depth         = toClipSpaceDepth(deviceDepth);
    vec3  clip          = vec3(toClipSpace(v_texcoord0), depth);
    vec3  wpos          = clipToWorld(u_sceneInvViewProjMat, clip);

    // Color
    vec3 color = decodeRGBE8(texture2D(Texture0, v_texcoord0));

    // Normals
    vec3 normal = decodeNormalUint(texture2D(Texture1, v_texcoord0).xyz);

    // View Direction
    vec3  viewDir    = normalize(u_camPos.xyz - wpos);
    float ndotv      = clamp(dot(normal, viewDir), 0.0, 1.0);
    vec3  reflectDir = normalize(2.0 * ndotv * normal - viewDir); // reflect(v, n);
    reflectDir = reflectDir * 2.0;
    float reflectLen = length(reflectDir);

    // Used to fade out as the angle approaches the screen.
    float front_fade = 1.0 - clamp(dot(reflectDir, viewDir), 0.0, 1.0); 

    // Backbuffer Input
    vec3 backbuffer = decodeRGBE8(texture2D(Texture4, v_texcoord0)).rgb;

    // Step as many times as it takes to find a hit.
    vec3  curr_wpos         = wpos;
    curr_wpos.x += noise(wpos.x);
    curr_wpos.y += noise(wpos.y);
    curr_wpos.z += noise(wpos.z);

    float prev_linear_depth = toLinearDepth(0.1, 2000.0, depth);
    vec3  reflected_color   = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < 32; ++i)
    {
        // Step in worldspace.
        curr_wpos += reflectDir;

        // Calculate UV of current worldspace position.
        vec4 curr_sspos = mul(u_sceneViewProjMat, vec4(curr_wpos, 1.0));
        vec2 curr_uv    = toUVSpace(curr_sspos.xyz / curr_sspos.w);

        // Sample linear depth at UV.
        float sample_linear_depth = getLinearDepth(Texture3, curr_uv, 0.1, 2000.0);

        // Determine the difference in linear depth.
        float diff = prev_linear_depth - sample_linear_depth; 
        if ( diff > 0.0 && diff < reflectLen )
        {
            float edge_fade = 1.0 - clamp(distance(curr_uv, vec2(0.5, 0.5)) * 2.0, 0.0, 1.0);
            reflected_color = decodeRGBE8(texture2D(Texture4, curr_uv)).rgb * edge_fade * front_fade;
            break;
        } 
    
        // Calculate and store linear depth from current screenspace position.
        float curr_depth    = curr_sspos.z / curr_sspos.w;
        prev_linear_depth   = toLinearDepth(0.1, 2000.0, curr_depth);
    }

    backbuffer += reflected_color;
    gl_FragColor = encodeRGBE8(backbuffer);
}
