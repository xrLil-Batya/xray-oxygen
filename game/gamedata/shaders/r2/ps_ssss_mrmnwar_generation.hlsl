#include "common.h"

#define NUM_SAMPLES int(11)

uniform float4 ssss_params;
uniform sampler2D s_mask_blur;

half4 main (p_screen I) : COLOR
{
	half	sunFar 			= 100 / sqrt(1 - L_sun_dir_w.y * L_sun_dir_w.y);
	half4	sunProj 		= mul(m_VP, float4(sunFar * L_sun_dir_w + eye_position, 1));
	half4	sunScreen 		= proj2screen(sunProj) / sunProj.w; // projective 2 screen and normalize
	half2 	sunVector		= sunScreen.xy - I.tc0.xy;
	
	half 	fSign 			= dot(-eye_direction, normalize(L_sun_dir_w));
	half 	fAspectRatio 	= 1.333h * screen_res.y / screen_res.x;
	half	sunDist 		= saturate(fSign) * saturate(1 - saturate(length(sunVector * float2(1, fAspectRatio)) * ssss_params.y));
	half2 	sunDir 			= sunVector * ssss_params.x * fSign;
	
	half4 	accum 			= half4(0, 0, 0, 0);
	
	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		half4 depth = tex2D(s_mask_blur, (I.tc0.xy + sunDir.xy*i));
		accum += depth*(1 - i/NUM_SAMPLES);
	}
	accum /= NUM_SAMPLES;

	half4	outColor 		= accum * 2 * half4(sunDist.xxx, 1);
			outColor.w 	   += 1 - saturate(saturate(fSign * 0.1h + 0.9h));
	
	return outColor;
}
