#include "common.h"
#include "Gauss.h" //gaussian blur

#define MASK_BLUR_SAMPLES int(6)

uniform	Texture2D s_mask_blur;	//smoothed mask
uniform	Texture2D s_sunshafts;

float4 main(p_screen I) : SV_Target
{
	float4 outColor = float4(0,0,0,0);
	for (int i = 1; i < MASK_BLUR_SAMPLES; i++)
	{
		outColor += Gauss(s_mask, I.tc0.xy, i, true);
	}
	outColor /= MASK_BLUR_SAMPLES;
	return outColor;
}