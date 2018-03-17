#include "common.h"
#include "MrProperCommon.h"
uniform half droplets_power;//	weather control
uniform sampler2D s_droplets;//	du/dv map

///////////////////////////////////////////////////////////
//pixel
half4		main	(	AntiAliasingStruct INStruct	)	:	COLOR	
{
	half2 droplets = tex2D(s_droplets,INStruct.texCoord0.xy).xy;
	
	half2 offset = (droplets.xy - (127.0h/255.0h)) * def_distort;
	float2 tc = INStruct.texCoord0.xy + offset * droplets_power;
	
	half3 image = tex2D(s_image,tc).xyz;
	return half4(image,1);
}