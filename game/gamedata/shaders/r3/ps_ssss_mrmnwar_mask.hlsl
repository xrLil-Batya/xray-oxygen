#include "common.h"

float NormalizeDepth(float Depth)
{
	return saturate(Depth / 100.f);
}

float4 main(p_screen I) : SV_Target
{
	float4 	scene 		= s_image.Sample(smp_rtlinear, I.tc0.xy);
#ifndef USE_MSAA
	float 	sceneDepth 	= s_position.Sample(smp_nofilter, I.tc0.xy).z;
#else
	float 	sceneDepth 	= s_position.Load(int3(I.tc0.xy * screen_res.xy, 0), 0).z;
#endif
	float 	RESDepth 	= NormalizeDepth(sceneDepth) * 1000;
	
	float4 	outColor 	= float4(scene.xyz*(1 - RESDepth),RESDepth);
	return outColor;
}