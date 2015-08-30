#define PIXEL_SIZE vec2(1.0/1280, 1.0/720)

vec4 sampleOffseted(sampler2D _tex, vec2 _texCoord, vec2 pixelOffset )
{
   return texture2D(_tex, _texCoord + pixelOffset * PIXEL_SIZE);
}

vec3 avg(vec3 value)
{
   static float oneThird = 1.0 / 3.0;
   return dot(value.xyz, vec3(oneThird, oneThird, oneThird) );
}

vec4 firsPassEdgeDetect( sampler2D _backbuffer, vec2 _texCoord )
{
   vec4 sCenter	   = sampleOffseted(_backbuffer, _texCoord, vec2( 0.0,  0.0) );
   vec4 sUpLeft	   = sampleOffseted(_backbuffer, _texCoord, vec2(-1.0, -1.0) );
   vec4 sUpRight   = sampleOffseted(_backbuffer, _texCoord, vec2( 1.0, -1.0) );
   vec4 sDownLeft  = sampleOffseted(_backbuffer, _texCoord, vec2(-1.0,  1.0) );
   vec4 sDownRight = sampleOffseted(_backbuffer, _texCoord, vec2( 1.0,  1.0) );
 
   vec4 diff   	   = 4.0f * abs( (sUpLeft + sUpRight + sDownLeft + sDownRight) - 4.0f * sCenter );
   float edgeMask  = avg(diff.xyz);

   return vec4(sCenter.rgb, edgeMask);
}


vec4 secondPassEdgeDetectAndBlur( sampler2D _backbuffer, vec2 _texCoord )
{
	// short edges
	vec4 sampleCenter       = sampleOffseted(_backbuffer, _texCoord.xy, vec2( 0.0,  0.0) );   
	vec4 sampleHorizNeg0    = sampleOffseted(_backbuffer, _texCoord.xy, vec2(-1.5,  0.0) );
	vec4 sampleHorizPos0    = sampleOffseted(_backbuffer, _texCoord.xy, vec2( 1.5,  0.0) ); 
	vec4 sampleVertNeg0	    = sampleOffseted(_backbuffer, _texCoord.xy, vec2( 0.0, -1.5) ); 
	vec4 sampleVertPos0	    = sampleOffseted(_backbuffer, _texCoord.xy, vec2( 0.0,  1.5) );

	vec4 sumHoriz      	    = sampleHorizNeg0 + sampleHorizPos0;
	vec4 sumVert   		    = sampleVertNeg0  + sampleVertPos0;

	vec4 diffToCenterHoriz  = abs( sumHoriz - (2.0 * sampleCenter) ) / 2.0;  
	vec4 diffToCenterVert   = abs( sumHoriz - (2.0 * sampleCenter) ) / 2.0;

	float valueEdgeHoriz 	= avg( diffToCenterHoriz.xyz );
	float valueEdgeVert  	= avg( diffToCenterVert.xyz );
	
	float edgeDetectHoriz	= saturate( (3.0 * valueEdgeHoriz) - 0.1);
	float edgeDetectVert 	= saturate( (3.0 * valueEdgeVert)  - 0.1);

	vec4 avgHoriz      	    = ( sumHoriz + sampleCenter) / 3.0;
	vec4 avgVert   		    = ( sumVert  + sampleCenter) / 3.0;

	float valueHoriz 		= avg( avgHoriz.xyz );
	float valueVert      	= avg( avgVert.xyz );

	float blurAmountHoriz	= saturate( edgeDetectHoriz / valueHoriz );
	float blurAmountVert 	= saturate( edgeDetectVert  / valueVert );

	vec4 aaResult      	    = lerp( sampleCenter,  avgHoriz, blurAmountHoriz );
	aaResult         		= lerp( aaResult,  	avgVert,  blurAmountVert );
  
	// long edges
	vec4 sampleVertNeg1	    = sampleOffseted(_backbuffer, _texCoord.xy, vec2(0.0, -3.5) ); 
	vec4 sampleVertNeg2	    = sampleOffseted(_backbuffer, _texCoord.xy, vec2(0.0, -7.5) );
	vec4 sampleVertPos1	    = sampleOffseted(_backbuffer, _texCoord.xy, vec2(0.0,  3.5) ); 
	vec4 sampleVertPos2	    = sampleOffseted(_backbuffer, _texCoord.xy, vec2(0.0,  7.5) ); 

	vec4 sampleHorizNeg1    = sampleOffseted(_backbuffer, _texCoord.xy, vec2(-3.5, 0.0) ); 
	vec4 sampleHorizNeg2    = sampleOffseted(_backbuffer, _texCoord.xy, vec2(-7.5, 0.0) );
	vec4 sampleHorizPos1    = sampleOffseted(_backbuffer, _texCoord.xy, vec2( 3.5, 0.0) ); 
	vec4 sampleHorizPos2    = sampleOffseted(_backbuffer, _texCoord.xy, vec2( 7.5, 0.0) ); 

	float pass1EdgeAvgHoriz = ( sampleHorizNeg2.a + sampleHorizNeg1.a + sampleCenter.a + sampleHorizPos1.a + sampleHorizPos2.a ) / 5.0;
	float pass1EdgeAvgVert  = ( sampleVertNeg2.a  + sampleVertNeg1.a  + sampleCenter.a + sampleVertPos1.a  + sampleVertPos2.a  ) / 5.0;
	pass1EdgeAvgHoriz    	= saturate( pass1EdgeAvgHoriz * 2.0f - 1.0f );
	pass1EdgeAvgVert 		= saturate( pass1EdgeAvgVert  * 2.0f - 1.0f );
	float longEdge   		= max( pass1EdgeAvgHoriz, pass1EdgeAvgVert);

	if ( longEdge > 0 )
	{
    	vec4 avgHorizLong       = ( sampleHorizNeg2 + sampleHorizNeg1 + sampleCenter + sampleHorizPos1 + sampleHorizPos2 ) / 5.0;
    	vec4 avgVertLong        = ( sampleVertNeg2  + sampleVertNeg1  + sampleCenter + sampleVertPos1  + sampleVertPos2  ) / 5.0;
    	float valueHorizLong    = avg(avgHorizLong.xyz);
    	float valueVertLong	    = avg(avgVertLong.xyz);

    	vec4 sampleLeft	        = sampleOffseted(_backbuffer, _texCoord.xy, vec2(-1.0,  0.0) );
    	vec4 sampleRight        = sampleOffseted(_backbuffer, _texCoord.xy, vec2( 1.0,  0.0) );
    	vec4 sampleUp  	        = sampleOffseted(_backbuffer, _texCoord.xy, vec2( 0.0, -1.0) );
    	vec4 sampleDown	        = sampleOffseted(_backbuffer, _texCoord.xy, vec2( 0.0,  1.0) );

    	float valueCenter	    = avg(sampleCenter.xyz);
    	float valueLeft  	    = avg(sampleLeft.xyz);
    	float valueRight 	    = avg(sampleRight.xyz);
    	float valueTop   	    = avg(sampleUp.xyz);
    	float valueBottom	    = avg(sampleDown.xyz);

    	vec4 diffToCenter       = valueCenter - vec4(valueLeft, valueTop, valueRight, valueBottom);   	
    	float blurAmountLeft    = saturate( 0.0 + ( valueVertLong  - valueLeft   ) / diffToCenter.x );
    	float blurAmountUp      = saturate( 0.0 + ( valueHorizLong - valueTop	) / diffToCenter.y );
    	float blurAmountRight   = saturate( 1.0 + ( valueVertLong  - valueCenter ) / diffToCenter.z );
    	float blurAmountDown    = saturate( 1.0 + ( valueHorizLong - valueCenter ) / diffToCenter.w );  	

    	vec4 blurAmounts        = vec4( blurAmountLeft, blurAmountRight, blurAmountUp, blurAmountDown );
    	blurAmounts      	    = (blurAmounts == vec4(0.0, 0.0, 0.0, 0.0)) ? vec4(1.0, 1.0, 1.0, 1.0) : blurAmounts;

    	vec4 longBlurHoriz      = lerp( sampleLeft,  sampleCenter,  blurAmounts.x );
    	longBlurHoriz    	    = lerp( sampleRight, longBlurHoriz, blurAmounts.y );
    	vec4 longBlurVert       = lerp( sampleUp,	 sampleCenter,  blurAmounts.z );
    	longBlurVert 		    = lerp( sampleDown,  longBlurVert,  blurAmounts.w );

    	aaResult     		    = lerp( aaResult,	longBlurHoriz, pass1EdgeAvgVert);
    	aaResult     		    = lerp( aaResult,	longBlurVert,  pass1EdgeAvgHoriz);
   }

   return vec4(aaResult.rgb, 1.0f);
}
