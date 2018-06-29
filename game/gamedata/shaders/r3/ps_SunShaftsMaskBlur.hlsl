#include "common.h"
#include "Gauss.h" //gaussian blur
#include "MrProperCommon.h"

#define MASK_BLUR_SAMPLES int(6)
//////////////////////////////////////////////////////////////////////////////////////////
//
half4 main(OptimizeAA INStruct) : SV_Target
{
	half4 outColor = half4(0,0,0,0);
	for (int i = 1; i < MASK_BLUR_SAMPLES; i++)
	{
		outColor += Gauss(sMask, INStruct.texCoord0.xy, i, true);
	}
	outColor /= MASK_BLUR_SAMPLES;
	return outColor;
}