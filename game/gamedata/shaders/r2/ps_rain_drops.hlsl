// mrnmwar -> SGM 3.0
#include "common.h"

uniform sampler2D s_droplets;
uniform float4 rain_drops_params0;

float4 main(p_screen I) : COLOR
{
	float4 rain_drops_distortion = tex2D(s_droplets, I.tc0);
	float2 texcoord_offset = (rain_drops_distortion.xy - (127.f / 255.f))*def_distort;//fix newtral offset
#ifdef RAIN_DROPS_DEBUG
	float2 texcoord = I.tc0 + texcoord_offset * rain_drops_params0.y;
#else
	float2 texcoord = I.tc0 + texcoord_offset * rain_drops_params0.x;
#endif
	float3 scene = tex2D(s_image, texcoord);
	return float4(scene, 1.0f);
}