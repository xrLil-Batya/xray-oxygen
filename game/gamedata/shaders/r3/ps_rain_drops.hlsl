#include "common.h"

Texture2D s_rain_drops0;
uniform float2 rain_drops_params0;

float4 main(p_screen I) : SV_Target
{
	float4 rain_drops_distortion = s_rain_drops0.Sample(smp_rtlinear, I.tc0);
	float2 texcoord_offset = (rain_drops_distortion.xy - (127.f / 255.f)) * def_distort; //fix newtral offset
#ifdef RAIN_DROPS_DEBUG
	float2 texcoord = I.tc0 + texcoord_offset * rain_drops_params0.y;
#else
	float2 texcoord = I.tc0 + texcoord_offset * rain_drops_params0.x;
#endif
	float3 scene = s_image.Sample(smp_rtlinear, texcoord);
	return float4(scene, 1.f);
}
