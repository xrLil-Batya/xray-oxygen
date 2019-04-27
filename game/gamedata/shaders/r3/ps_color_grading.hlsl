#include "common.h"

uniform Texture2D color_chart_sampler;

float4 main(p_screen IN) : SV_Target
{
    float3 cImage =  s_image.Load(int3(IN.hpos.xy, 0), 0);
    float lutWidth = 16.0f;
    float2 lutOffset = float2(0.5f / 256.0f, 0.5f / lutWidth);
    float lutScale = (lutWidth - 1.0) / lutWidth; 
	//saturate to make sure in 0-1 range
	cImage = saturate(cImage);

	float3 lutTC = cImage * lutScale;
	
	float blue = floor(cImage.b * 14.9999f) / lutWidth;
	float frac = (cImage.b - blue) * lutWidth;

	lutTC.x = blue + cImage.r * lutScale / lutWidth;
	float3 sample1 = color_chart_sampler.Sample(smp_nofilter, lutOffset + lutTC.xy).rgb;
	
	lutTC.x += 1.0f / 16.0f;
	float3 sample2 = color_chart_sampler.Sample(smp_nofilter, lutOffset + lutTC.xy).rgb;

	cImage = lerp(sample1, sample2, frac);
	
	return float4(cImage, 1);
};