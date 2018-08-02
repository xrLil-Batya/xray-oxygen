#include "common.h"
#include "Gauss.h" // gaussian blur

#define MASK_BLUR_SAMPLES int(6)

half4 main (p_screen I) : COLOR
{
	half4 outColor = half4(0,0,0,0);
	
	for (int i = 1; i < MASK_BLUR_SAMPLES; ++i)
	{
		outColor += Gauss(s_mask, I.tc0.xy, i, true);
	}
	outColor /= MASK_BLUR_SAMPLES;

	return outColor;
}
