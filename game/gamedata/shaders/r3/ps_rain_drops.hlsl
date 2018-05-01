#include "common.h"
#include "MrProperCommon.h"

//////////////////////////////////////////////////////////////////////////////////////////
//pixel
float4 main(AntiAliasingStruct INStruct) : SV_Target
{
	float4 rain_drops_distortion = s_rain_drops0.Sample(/*smp_nofilter*/ smp_rtlinear, INStruct.texCoord0);
	float2 texcoord_offset = (rain_drops_distortion.xy - (127.h / 255.h)) * def_distort; //fix newtral offset
	float2 texcoord = INStruct.texCoord0 + texcoord_offset * rain_drops_params0.y;
	float3 scene = s_base0.Sample(/*smp_nofilter*/ smp_rtlinear, texcoord);
	return float4(scene, 1.f);
}