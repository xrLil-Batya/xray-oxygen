#include "common.h"

half NormalizeDepth(half Depth)
{
	return saturate(Depth / 100);
}

half4 main(p_screen I) : COLOR
{
	half3 	scene 		= tex2D(s_image, I.tc0.xy).xyz;
	half 	sceneDepth 	= tex2D(s_position, I.tc0.xy).z;
	half 	RESDepth 	= saturate(sceneDepth / 100) * 1000; // normalize depth

	half4	outColor 	= half4(scene*(1 - RESDepth), RESDepth);
	return outColor;
}
