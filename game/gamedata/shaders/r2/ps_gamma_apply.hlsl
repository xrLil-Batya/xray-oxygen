#include "common.h"

uniform sampler1D s_gamma_lut;

float4 main(float2 tc0 : TEXCOORD0) : COLOR
{
	float3 image = tex2D(s_image, tc0).xyz;

	// Compute the 1D LUT lookup scale/offset factor
	const float lutSize = 256.0f;
	float scale = (lutSize - 1.0f) / lutSize;
	float offset = 1.0f / (2.0f * lutSize);
	
	// apply
	float3 color = float3(tex1D(s_gamma_lut, scale * image.x + offset).x,
						  tex1D(s_gamma_lut, scale * image.y + offset).y,
						  tex1D(s_gamma_lut, scale * image.z + offset).z);
	
	return float4(color, 1.0f);
}
