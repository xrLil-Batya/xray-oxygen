#include "common.h"

uniform sampler2D color_chart_sampler;

float4 main(p_screen IN) : COLOR
{
    float3 cImage =  tex2D(s_image, IN.tc0.xy).xyz;
    float lutWidth = 16.0;
    float2 lutOffset = float2(0.5f / 256.0f, 0.5f / lutWidth);
    float lutScale = (lutWidth - 1.0) / lutWidth; 
	//saturate to make sure in 0-1 range
	cImage = saturate(cImage);

	float3 lutTC = cImage * lutScale;
	
	float blue = floor(cImage.b * 14.9999f) / lutWidth;
	float frac = (cImage.b - blue) * lutWidth;

	lutTC.x = blue + cImage.r * lutScale / lutWidth;
	float3 sample1 = tex2D(color_chart_sampler, lutOffset + lutTC.xy).rgb;
	
	lutTC.x += 1.0f / 16.0f;
	float3 sample2 = tex2D(color_chart_sampler, lutOffset + lutTC.xy).rgb;

	cImage = lerp(sample1, sample2, frac);
	
	return float4(cImage, 1);
};