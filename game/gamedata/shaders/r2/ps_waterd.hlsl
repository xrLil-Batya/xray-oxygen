#include "common.h"
#include "shared\waterconfig.h"

struct v2p
{
	float2	tbase		: TEXCOORD0;  // base
	float2	tdist0		: TEXCOORD1;  // d0
	float2	tdist1		: TEXCOORD2;  // d1
#ifdef NEED_SOFT_WATER
	float4	tctexgen	: TEXCOORD3;
#endif
};

uniform sampler2D	s_distort;

#define POWER 0.5f

float4 main(v2p I) : COLOR
{
	float4	t_base		= tex2D(s_base, I.tbase);

	float2	t_d0		= tex2D(s_distort, I.tdist0);
	float2	t_d1		= tex2D(s_distort, I.tdist1);
	float2	distort		= (t_d0 + t_d1) * 0.5f; // average
	float2	zero		= float2(0.5f, 0.5f);
	float2	faded		= lerp(distort, zero, t_base.a);

#ifdef NEED_SOFT_WATER
	float4	_P			= tex2Dproj(s_position, I.tctexgen);
	float	waterDepth	= _P.z - I.tctexgen.z;
	float	alphaDistort= saturate(5*waterDepth);
			faded		= lerp(zero, faded, alphaDistort);
#endif // NEED_SOFT_WATER

	float2	faded_bx2	= (faded*2-1)*W_DISTORT_POWER;
	float	faded_dot	= dot(float3(faded_bx2, 0), 0.75f);
	float	alpha		= abs(faded_dot);
			faded		= faded*POWER - 0.5f*POWER + 0.5f;

	return float4(faded, 0.0f, alpha);
}
