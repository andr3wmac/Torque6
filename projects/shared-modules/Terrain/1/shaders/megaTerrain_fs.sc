$input v_position, v_texcoord0

#include <torque6.sc>

SAMPLER2D(Texture0, 0);
SAMPLER2D(Texture1, 1);
SAMPLER2D(Texture2, 2);

uniform vec4 focusPoint;
uniform vec4 cascadeSize;
uniform vec4 layerScale;

void main()
{
    // Calculate cascade coordinate.
    vec2 blend_coord;
    if ( v_texcoord0.x < 0.5 && v_texcoord0.y < 0.5 )
    {
        blend_coord = v_texcoord0 * 2.0;
        blend_coord = (focusPoint.xy - cascadeSize.x) + (blend_coord * (cascadeSize.x * 2.0));
    }
    if ( v_texcoord0.x >= 0.5 && v_texcoord0.y < 0.5 )
    {
        blend_coord.x = (v_texcoord0.x - 0.5) * 2.0;
        blend_coord.y = (v_texcoord0.y * 2.0);
        blend_coord = (focusPoint.xy - cascadeSize.y) + (blend_coord * (cascadeSize.y * 2.0));
    }
    if ( v_texcoord0.x < 0.5 && v_texcoord0.y >= 0.5 )
    {
        blend_coord.x = (v_texcoord0.x * 2.0);
        blend_coord.y = (v_texcoord0.y - 0.5) * 2.0;
        blend_coord = (focusPoint.xy - cascadeSize.z) + (blend_coord * (cascadeSize.z * 2.0));
    }
    if ( v_texcoord0.x >= 0.5 && v_texcoord0.y >= 0.5 )
    {
        blend_coord.x = (v_texcoord0.x - 0.5) * 2.0;
        blend_coord.y = (v_texcoord0.y - 0.5) * 2.0;
    }
    
    // Blend Map
    vec4 blendSample = texture2D(Texture0, blend_coord);

    // Layer 0 : Grass
    vec4 layer0Sample = texture2D(Texture1, blend_coord * layerScale.x);

    // Layer 1 : Snow
    vec4 layer1Sample = texture2D(Texture2, blend_coord * layerScale.y);

    // Output Blended
    gl_FragColor = vec4((layer0Sample.rgb * blendSample.r) + (layer1Sample.rgb * blendSample.g), 1.0);
}
