#include "common.h"
#include "Gauss.h"//gaussian blur
#include "MrProperCommon.h" // [FX to mrmnwar] Cheking includes, please

//#define USE_MASK_BLUR//smoothed mask
#define MASK_BLUR_SAMPLES int(6)
//////////////////////////////////////////////////////////////////////////////////////////
//
half4		main		(	AntiAliasingStruct	INStruct	)		:		COLOR
{

	half4 outColor = half4(0,0,0,0);
//	#ifdef USE_MASK_BLUR
	for (int i = 1; i < MASK_BLUR_SAMPLES; i++)
	//for (int i = 0; i < MASK_BLUR_SAMPLES; i++)
	{
		/*				sampler2D,texCoord,			factor,				optimize*/

		outColor += Gauss(sMask,INStruct.texCoord0.xy,i,true);
	}
	outColor/=MASK_BLUR_SAMPLES;
//	#else//USE_MASK_BLUR
//	outColor = tex2D(sMask,INStruct.texCoord0.xy);
//	#endif//USE_MASK_BLUR
	return outColor;
}
