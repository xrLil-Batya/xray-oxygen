#include "common.h"

Texture2D s_droplets;
uniform float2 rain_drops_params0;

float4 main(p_screen I) : SV_Target
{
	float4 rain_drops_distortion = s_droplets.Sample(smp_rtlinear, I.tc0);
	float2 texcoord_offset = (rain_drops_distortion.xy - (127.0f / 255.0f)) * def_distort; // fix neutral offset
#ifdef RAIN_DROPS_DEBUG
	float2 texcoord = I.tc0 + texcoord_offset * rain_drops_params0.y;
#else
	float2 texcoord = I.tc0 + texcoord_offset * rain_drops_params0.x;
#endif
//	float3 scene = s_image.Load(int3(texcoord * screen_res.xy, 0), 0);
	float3 scene = s_image.Sample(smp_rtlinear, texcoord).xyz;
	return float4(scene, 1.0f);
}
