#include "common.h"
#include "shared\waterconfig.h"

struct v2p
{
	float2	tbase	: TEXCOORD0;
	float2	tdist0	: TEXCOORD1;
	float2	tdist1	: TEXCOORD2;
#ifdef USE_SOFT_WATER
#ifdef NEED_SOFT_WATER
	float4	tctexgen: TEXCOORD3;
#endif
#endif
	float4	hpos	: SV_Position;
};

Texture2D s_distort;
#define POWER 0.5f
//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
float4 main(v2p I) : SV_Target
{
	float4	t_base		= s_base.Sample(smp_base, I.tbase);

	float2	t_d0		= s_distort.Sample(smp_base, I.tdist0);
	float2	t_d1		= s_distort.Sample(smp_base, I.tdist1);
	float2	distort		= (t_d0 + t_d1) * 0.5f; // average
	float2	zero		= float2(0.5f, 0.5f);
	float2	faded		= lerp(distort, zero, t_base.a);

	//	Igor: additional depth test
#ifdef USE_SOFT_WATER
#ifdef NEED_SOFT_WATER
	float2	PosTc		= I.tctexgen.xy/I.tctexgen.z;
	gbuffer_data gbd	= gbuffer_load_data(PosTc, I.hpos);

	float4	_P			= float4(gbd.P, gbd.mtl);
	float	waterDepth	= _P.z - I.tctexgen.z;
	float	alphaDistort = saturate(5*waterDepth);
			faded		= lerp(zero, faded, alphaDistort);
#endif	//	NEED_SOFT_WATER
#endif	//	USE_SOFT_WATER & NEED_SOFT_WATER
	float2	faded_bx2	= (faded*2-1)*W_DISTORT_POWER;
	float	faded_dot	= dot(float3(faded_bx2, 0.0f), 0.75f);
	float	alpha		= abs(faded_dot);
			faded		= faded*POWER - 0.5f*POWER + 0.5f;

#ifdef	NEED_SOFT_WATER
	return  float4(faded, 0.0f, alpha);
#else
	return  float4(faded, 0.08f, alpha);
#endif
}
