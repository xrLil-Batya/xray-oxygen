#if ( defined(MSAA_ALPHATEST_DX10_1_ATOC) || defined(MSAA_ALPHATEST_DX10_1) ) 
#define EXTEND_F_DEFFER
#endif

#include "common.h"
#include "sload.h"

#ifdef ATOC

float4 main(p_flat I) : SV_Target
{
	float4	D 	= tbase(I.tcdh);
			D.w = (D.w - def_aref * 0.5f) / (1.0f - def_aref * 0.5f);
	return D;
}

#else	// ATOC

#ifdef MSAA_ALPHATEST_DX10_1_ATOC
f_deffer main(p_flat I, float4 pos2d : SV_Position)
#else	// MSAA_ALPHATEST_DX10_1_ATOC
f_deffer main(p_flat I)
#endif	// MSAA_ALPHATEST_DX10_1_ATOC
{
	f_deffer O;

	// 1. Base texture + kill pixels with low alpha
#if !defined(MSAA_ALPHATEST_DX10_1)
	float4	D 		= tbase(I.tcdh);

#if	!(defined(MSAA_ALPHATEST_DX10_1_ATOC) || defined(MSAA_ALPHATEST_DX10_0_ATOC))
	clip(D.w - def_aref);
#endif	// !(defined(MSAA_ALPHATEST_DX10_1_ATOC) || defined(MSAA_ALPHATEST_DX10_1_ATOC))
#ifdef MSAA_ALPHATEST_DX10_1_ATOC
	float	alpha	= (D.w - def_aref * 0.5f) / (1.0f - def_aref * 0.5f);
	uint	mask	= alpha_to_coverage(alpha, pos2d);
#endif	// MSAA_ALPHATEST_DX10_1_ATOC

#else
	uint mask = 0x0;
	float2	ddx_base 	= ddx(I.tcdh);
	float2	ddy_base 	= ddy(I.tcdh);

	float2	pixeloffset = MSAAOffsets[0]*(1.0f/16.0f);
	float2	texeloffset = pixeloffset.x * ddx_base + pixeloffset.y * ddy_base;
      	
	float4	D 			= tbase(I.tcdh + texeloffset);
	
	if (D.w - def_aref >= 0.0f)
		mask |= 0x1;
	
	[unroll] for( int i = 1; i < MSAA_SAMPLES; ++i )
	{
		pixeloffset = MSAAOffsets[i]*(1.0f/16.0f);
		texeloffset = pixeloffset.x * ddx_base + pixeloffset.y * ddy_base;
		float4 DI 	= tbase(I.tcdh + texeloffset);
		if (DI.w - def_aref >= 0.0f)
			mask |= (uint(0x1) << i);
	}
	
	if (mask == 0x0)
		discard;
#endif

#ifdef USE_TDETAIL
	D.rgb = 2.0f * D.rgb * s_detail.Sample(smp_base, I.tcdbump).rgb;
#endif

	// hemi, sun, material
	float 	ms	= xmaterial;
#ifdef USE_LM_HEMI
	float4	lm 	= s_hemi.Sample(smp_rtlinear, I.lmh);
	float 	h  	= get_hemi(lm);
#else
	float 	h	= I.position.w;
#endif

	// 2. Standart output
	float4 Ne     = float4(normalize((float3)I.N.xyz), h);
#if !(defined(MSAA_ALPHATEST_DX10_1_ATOC) || defined(MSAA_ALPHATEST_DX10_1))
	O = pack_gbuffer(
					Ne,
					float4(I.position.xyz + Ne.xyz * def_virtualh/2.0f, ms),
					float4(D.rgb, def_gloss));
#else
	O = pack_gbuffer(
  					Ne,
					float4(I.position.xyz + Ne.xyz * def_virtualh/2.0f, ms),
					float4(D.rgb, def_gloss),
					mask );
#endif
	return O;
}
#endif	// ATOC
