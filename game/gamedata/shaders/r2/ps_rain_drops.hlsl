#include "common.h"

uniform sampler2D s_droplets;
uniform float4 rain_params; // x=world wetness, y=camera wetness, z=rain density

float4 main(p_screen I) : COLOR
{
	float4	rain_drops_distortion 	= tex2D(s_droplets, I.tc0);
	float2	texcoord_offset 		= (rain_drops_distortion.xy - (127.0f / 255.0f)) * def_distort; // fix neutral offset
	float2	texcoord 				= I.tc0 + texcoord_offset * saturate(rain_params.y);
	float3	scene 					= tex2D(s_image, texcoord);
	
	return float4(scene, 1.0f);
}
