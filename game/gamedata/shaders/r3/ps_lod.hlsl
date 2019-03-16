#if (defined(MSAA_ALPHATEST_DX10_1_ATOC) || defined(MSAA_ALPHATEST_DX10_1)) 
#define EXTEND_F_DEFFER
#endif

#include "common.h"
#include "sload.h"

struct vf
{
	float3	position	: TEXCOORD0;
 	float2	tc0			: TEXCOORD1;	// base0
 	float2	tc1			: TEXCOORD2;	// base1
	float4	af			: COLOR1;		// alpha&factor
	float4	hpos		: SV_Position;
};

void sample_Textures(inout float4 D, inout float4 H, float2 tc1, float2 tc0, float4 af, float2 pixeloffset)
{
	float2 ddx_tc0 = ddx(tc0);
	float2 ddy_tc0 = ddy(tc0);
	float2 ddx_tc1 = ddx(tc1);
	float2 ddy_tc1 = ddy(tc1);

	float4 D1 	= s_base.Sample(smp_base,	tc1 + pixeloffset.x * ddx_tc1 + pixeloffset.y * ddy_tc1);
	float4 D0 	= s_base.Sample(smp_base,	tc0 + pixeloffset.x * ddx_tc0 + pixeloffset.y * ddy_tc0);
	float4 H0 	= s_hemi.Sample(smp_linear, tc0 + pixeloffset.x * ddx_tc0 + pixeloffset.y * ddy_tc0);	H0.xyz = H0.rgb * 2.0f - 1.0f; // expand
	float4 H1 	= s_hemi.Sample(smp_linear, tc1 + pixeloffset.x * ddx_tc1 + pixeloffset.y * ddy_tc1);	H1.xyz = H1.rgb * 2.0f - 1.0f; // expand

	D 	= lerp(D0, D1, af.w); D.w *= af.z; // multiply alpha
	H 	= lerp(H0, H1, af.w); H.w *= af.x; // multiply hemi
}

#ifdef ATOC
float4 main(vf I) : SV_Target
{
	float4 D;
	float4 H;
	sample_Textures(D, H, I.tc1, I.tc0, I.af, float2(0.0f, 0.0f));
	D.w = (D.w - (96.0f/255.0f)*0.5f) / (1.0f - (96.0f/255.0f)*0.5f);
	return D.w;
}
#else	// ATOC
#ifdef MSAA_ALPHATEST_DX10_1_ATOC
f_deffer main(vf I, float4 pos2d : SV_Position)
#else	// MSAA_ALPHATEST_DX10_1_ATOC
f_deffer main(vf I)
#endif	// MSAA_ALPHATEST_DX10_1_ATOC
{
	f_deffer O;

	// 1. Base texture + kill pixels with low alpha
	float4 D;
	float4 H;
  
#ifndef MSAA_ALPHATEST_DX10_1
  sample_Textures(D, H, I.tc1, I.tc0, I.af, float2(0.0f, 0.0f));
#if	!(defined(MSAA_ALPHATEST_DX10_1_ATOC) || defined(MSAA_ALPHATEST_DX10_0_ATOC))
	clip(D.w - (96.0f / 255.0f));
#endif	// !(defined(MSAA_ALPHATEST_DX10_1_ATOC) || defined(MSAA_ALPHATEST_DX10_1_ATOC))
#ifdef MSAA_ALPHATEST_DX10_1_ATOC
	float alpha = (D.w - (96.0f/255.0f)*0.5f)/(1.0f - (96.0f/255.0f)*0.5f);
	uint mask = alpha_to_coverage(alpha, pos2d);
#endif	// MSAA_ALPHATEST_DX10_1_ATOC
#else	// MSAA_ALPHATEST_DX10_1
	uint mask = 0x0;

	float2 pixeloffset = MSAAOffsets[0] * (1.0f / 16.0f);
	sample_Textures(D, H, I.tc1, I.tc0, I.af, pixeloffset);

	if (D.w - (96.0f/255.0f) >= 0.0f)
		mask |= 0x1;

	[unroll]
	for(int i = 1; i < MSAA_SAMPLES; ++i)
	{
		float4 DI;
		float4 HI;
		pixeloffset = MSAAOffsets[i]*(1.0f / 16.0f);
		sample_Textures(DI, HI, I.tc1, I.tc0, I.af, pixeloffset);
		if (DI.w - (96.0f/255.0f) >= 0.0f)
			mask |= (uint(0x1) << i);
	}

	if (mask == 0x0)
		discard;
#endif	// MSAA_ALPHATEST_DX10_1
  
	float3 N 	= normalize(H.xyz);
  
  // 2. Standart output
#ifndef EXTEND_F_DEFFER
	O = pack_gbuffer(
					float4(N, H.w),
					float4(I.position + N*def_virtualh/2.0f, 0.0f),
					float4(D.x, D.y, D.z, def_gloss)); // OUT: rgb.gloss
#else
	O = pack_gbuffer(
					float4(N, H.w),
					float4(I.position + N*def_virtualh/2.0f, 0.0f),
					float4(D.x, D.y, D.z, def_gloss),
					mask); // OUT: rgb.gloss
#endif
	return O;
}
#endif	//	ATOC
