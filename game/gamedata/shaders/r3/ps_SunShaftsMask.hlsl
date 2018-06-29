#include "common.h"
#include "MrProperCommon.h" 
//////////////////////////////////////////////////////////////////////////////////////////
float4 main(OptimizeAA INStruct) : SV_Target
{
	float sceneDepth = sPosition.Sample(smp_nofilter,INStruct.texCoord0.xy).z;
	float4 scene = sScene.Sample(smp_rtlinear,INStruct.texCoord0.xy);
	
	float RESDepth = NormalizeDepth(sceneDepth);
	RESDepth*=1000;
	
	float4 outColor = float4(scene.xyz*(1-RESDepth),RESDepth);
	return outColor;
}