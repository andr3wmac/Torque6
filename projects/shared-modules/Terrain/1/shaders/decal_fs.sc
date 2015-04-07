$input v_position, v_texcoord0

#include <torque6.sc>

SAMPLER2D(DepthTexture, 0);

void main()
{
    vec4 depthSample = texture2D(DepthTexture, v_texcoord0); 

    //float dist = 1.0 - length(vec2(v_position.x, v_position.z));
    gl_FragColor = vec4(depthSample.rgb, 1.0);

    //if ( dist < 0.5 )
	//    gl_FragColor = vec4(0, 1, 0, 0.7);
    //else
    //    gl_FragColor = vec4(0, 0, 0, 0);
}
