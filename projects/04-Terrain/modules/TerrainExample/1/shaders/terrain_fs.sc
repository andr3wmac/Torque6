$input v_position, v_texcoord0

#include <torque6.sc>

SAMPLER2D(Texture0, 0);
//SAMPLER2D(Texture1, 1);
//SAMPLER2D(Texture2, 2);

uniform vec3 focusPoint;

void main()
{
    vec2 focus_dist = abs(focusPoint.xy - v_texcoord0.xy);

    if ( focus_dist.x < 0.1 && focus_dist.y < 0.1 )
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else if ( focus_dist.x < 0.3 && focus_dist.y < 0.3 )
        gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    else
        gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);

    //vec4 megaTexture = texture2D(Texture0, v_texcoord0); 
    //gl_FragColor = vec4(megaTexture.rgb, 1.0);
   
    //vec4 layer0 = toLinear(texture2D(Texture0, mul(v_texcoord0, 20.0)));
    //vec4 layer1 = toLinear(texture2D(Texture1, mul(v_texcoord0, 20.0)));
    //vec4 layer2 = toLinear(texture2D(Texture2, mul(v_texcoord0, 20.0)));

    //float height = v_position.y / 50.0;
	//gl_FragColor = vec4(height, height, height, 1.0);
    
    //gl_FragColor = vec4(toGamma(mix(layer2.rgb, layer1.rgb, height)), 1.0);
}
