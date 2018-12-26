#include "common.h"
#include "sload.h"

#define	USE_4_DETAIL

#if defined(USE_TDETAIL) && defined(USE_4_DETAIL)
#define USE_4_BUMP
#endif

#ifdef USE_4_BUMP
f_deffer main(p_bumped I)
#else
f_deffer main(p_flat I)
#endif
{
	f_deffer O;

 	// diffuse
	float4	D	= tbase(I.tcdh); // IN:  rgb.a
	float4	L 	= s_lmap.Sample(smp_base, I.tcdh);
	float	G	= def_gloss;

#ifdef USE_TDETAIL
#ifdef USE_4_DETAIL
	float4	mask= s_mask.Sample(smp_base, I.tcdh);
	float	mag	= dot(mask, 1.0f);
			mask= mask / mag;

	float3	d_R	= s_dt_r.Sample(smp_base, I.tcdbump) * mask.r;
	float3	d_G	= s_dt_g.Sample(smp_base, I.tcdbump) * mask.g;
	float3	d_B	= s_dt_b.Sample(smp_base, I.tcdbump) * mask.b;
	float3	d_A	= s_dt_a.Sample(smp_base, I.tcdbump) * mask.a;
	float3	dt	= d_R + d_G + d_B + d_A;
		D.rgb	= 2.0f * D.rgb * dt;

#ifdef USE_4_BUMP
 	 float4	n_Rt = s_dn_r.Sample(smp_linear, I.tcdbump).wzyx;
	 float4	n_Gt = s_dn_g.Sample(smp_linear, I.tcdbump).wzyx;
	 float4	n_Bt = s_dn_b.Sample(smp_linear, I.tcdbump).wzyx;
	 float4	n_At = s_dn_a.Sample(smp_linear, I.tcdbump).wzyx;
	 
 	 float3	n_R = (n_Rt-0.5f) * mask.r; float g_R = n_Rt.w * mask.r;
	 float3	n_G = (n_Gt-0.5f) * mask.g; float g_G = n_Gt.w * mask.g;
	 float3	n_B = (n_Bt-0.5f) * mask.b; float g_B = n_Bt.w * mask.b;
	 float3	n_A = (n_At-0.5f) * mask.a; float g_A = n_At.w * mask.a;

	 float3	mix		= n_R + n_G + n_B + n_A;
			mix.z  *= 0.5f; // Make bump twice as contrast (fake, remove me if possible)
	 float3	N		= mul(float3x3(I.M1, I.M2, I.M3), mix.xyz);
	 	 	G 		= g_R + g_G + g_B + g_A;
#else	// USE_4_BUMP
	float3	N 		= I.N.xyz;
#endif	// USE_4_BUMP

#else	// USE_4_DETAIL
	D.rgb			= 2.0f * D.rgb * s_detail.Sample(smp_base, I.tcdbump).rgb;
#endif	// USE_4_DETAIL
#else	// USE_TDETAIL
	float3	N 	= I.N.xyz;
#endif	// USE_TDETAIL

	// hemi, sun, material
	float 	ms	= xmaterial;

  // 2. Standart output
  float4     Ne = float4	(normalize(N), D.w);
  O				= pack_gbuffer( 
								Ne,	// hemi
								float4(I.position.xyz + Ne.xyz*def_virtualh/2.0f, ms),
								float4(D.x, D.y, D.z, G)); // OUT: rgb.gloss

  return O;
}
