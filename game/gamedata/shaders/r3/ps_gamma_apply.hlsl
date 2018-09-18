#include "common.h"

uniform texture2D s_gamma_lut;

float4 main(p_screen I) : SV_Target
{
	float3 image  = s_image.Load(int3(I.hpos.xy, 0), 0).xyz;

	// Compute the 1D LUT lookup scale/offset factor
	const float lutSize = 256.0f;
	float scale = (lutSize - 1.0f) / lutSize;
	float offset = 1.0f / (2.0f * lutSize);
	
	// apply
	float3 color = float3(s_gamma_lut.Sample(smp_rtlinear, scale * image.x + offset).x,
						  s_gamma_lut.Sample(smp_rtlinear, scale * image.y + offset).y,
						  s_gamma_lut.Sample(smp_rtlinear, scale * image.z + offset).z);
	
	return float4(color, 1.0f);
}
