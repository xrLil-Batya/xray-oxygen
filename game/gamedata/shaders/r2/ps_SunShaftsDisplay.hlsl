#include "common.h"
#include "MrProperCommon.h" // [FX to mrmnwar] Cheking includes, please

float4 blendSoftLight(float4 a, float4 b)
{
	float4 c = 2 * a * b + a * a * (1 - 2 * b);
	float4 d = sqrt(a) * (2 * b - 1) + 2 * a * (1 - b);
	
	return ( b < 0.5 )? c : d;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
half4		main		(	AntiAliasingStruct	INStruct	)		:		COLOR
{
	half4 outColor;
	half4 cScreen = tex2D(sScene,INStruct.texCoord0.xy);
	half4 cSunShafts = tex2D(sSunShafts,INStruct.texCoord0.xy);
	
	//half fShaftsMask = saturate(1.00001-cSunShafts.w)*BlendParams.x*2;
	half fShaftsMask=saturate(1.00001-cSunShafts.w)*SSParamsDISPLAY.y*2;
	

	
	//normalize suncolor
	float4 sunColor = float4(1,1,1,1);
	sunColor.xyz = normalize(L_sun_color.xyz);
	

	//outColor = cScreen+cSunShafts.xyzz*SSIntensity.x*sunColor*(1-cScreen);
	outColor=cScreen+cSunShafts.xyzz*SSParamsDISPLAY.x*sunColor*(1-cScreen);
	outColor = blendSoftLight(outColor,sunColor*fShaftsMask*0.5+0.5);
	
	return outColor;
}
