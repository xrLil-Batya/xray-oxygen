#include "common.h"

struct v2p
{
 	float2	tc: TEXCOORD0;	// base & distort
};

Texture2D s_distort;

float4 main (v2p_TL I) : SV_Target
{
	float2 	distort	= s_distort.Load(int3(I.HPos.xy, 0), 0);
	float2	offset	= (distort.xy - 127.0f/255.0f)*def_distort;
	float3	image 	= s_base.Sample(smp_rtlinear, I.Tex0 + offset);

	return float4(image, 1.0f);
}
