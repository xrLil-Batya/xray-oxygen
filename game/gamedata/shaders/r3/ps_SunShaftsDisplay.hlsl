#include "common.h"
#include "MrProperCommon.h"

float4 blendSoftLight(float4 a, float4 b)
{
	float4 c = 2 * a * b + a * a * (1 - 2 * b);
	float4 d = sqrt(a) * (2 * b - 1) + 2 * a * (1 - b);
	
	return ( b < 0.5 )? c : d;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
float4 main(OptimizeAA INStruct) : SV_Target
{
	float4 cScreen = sScene.Sample(smp_rtlinear,INStruct.texCoord0.xy);
	float4 cSunShafts = sSunShafts.Sample(smp_rtlinear, INStruct.texCoord0.xy);
	
	float fShaftsMask = saturate(1.00001-cSunShafts.w) * SSParamsDISPLAY.y * 2;
	
	//normalize suncolor
	float4 sunColor = float4(1,1,1,1);
	sunColor.xyz = normalize(L_sun_color.xyz);
	
	float4 outColor = cScreen+cSunShafts.xyzz * SSParamsDISPLAY.x * sunColor * (1-cScreen);
	outColor = blendSoftLight(outColor, sunColor * fShaftsMask * 0.5 + 0.5);
	
	return outColor;
}
