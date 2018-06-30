#include "common.h"
#include "MrProperCommon.h"

#define NUM_SAMPLES int(11)
uniform float3 eye_direction;
//////////////////////////////////////////////////////////////////////////////////////////
//
float4 main(OptimizeAA INStruct) : SV_Target
{
	float sunFar = 100 / sqrt(1-L_sun_dir_w.y * L_sun_dir_w.y);
	float4 sunProj = mul(m_VP,float4(sunFar*L_sun_dir_w+eye_position,1));
	float4 sunScreen = proj2screen(sunProj)/sunProj.w;//projective 2 screen and normalize
	
	float fSign = dot(-eye_direction,normalize(L_sun_dir_w));
	float2 sunVector =(sunScreen.xy - INStruct.texCoord0.xy);
	float fAspectRatio = 1.333*screen_res.y/screen_res.x;
	
	float sunDist = saturate(fSign)*saturate(1-saturate(length(sunVector*float2(1,fAspectRatio))*SSParams.y));
	float2 sunDir = (sunScreen.xy-INStruct.texCoord0.xy);
	
	float4 accum = float4(0,0,0,0);
	sunDir.xy*=SSParams.x*fSign;

	[unroll]
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		float4 Depth = sMaskBlur.Sample(smp_rtlinear,(INStruct.texCoord0.xy+sunDir.xy*i));
		accum+=Depth*(1-i/NUM_SAMPLES);
	}

	accum /= NUM_SAMPLES;
	
	float4 outColor = accum * 2 * float4(sunDist.xxx, 1);
	outColor.w += 1 - saturate(saturate(fSign*0.1 + 0.9));
	
	return outColor;
}
