#include "common.h"

#include "lmodel.h"
#include "hmodel.h"
#include "BRDF.hlsl"
uniform	Texture2D s_half_depth;

#include "ps_ssao.hlsl"
#ifdef USE_HBAO
#include "ps_ssao_hbao.hlsl"
#endif                                 

struct _input
{
	float4	tc0		: TEXCOORD0;	// tc.xy, tc.w = tonemap scale
	float2	tcJ		: TEXCOORD1;	// jitter coords
	float4	pos2d	: SV_Position;
};

struct _out
{
	float4	low		: SV_Target0;
	float4	high	: SV_Target1;
};

//	TODO:	DX10: Replace Sample with Load
#ifndef MSAA_OPTIMIZATION
[earlydepthstencil]
_out main (_input I)
#else
[earlydepthstencil]
_out main (_input I, uint iSample : SV_SAMPLEINDEX)
#endif
{
	gbuffer_data gbd = gbuffer_load_data(GLD_P(I.tc0, I.pos2d, ISAMPLE));
	
	// Sample the buffers:
	float4	P		= float4(gbd.P, gbd.mtl);			// position.(mtl or sun)
	float4	N		= float4(gbd.N, gbd.hemi);			// normal.hemi
	float4	D		= float4(gbd.C, gbd.gloss);			// rgb.gloss
#ifndef USE_MSAA
#	ifdef GBUFFER_OPTIMIZATION
	float4	L		= s_accumulator[int2(I.pos2d.xy)];	// diffuse.specular
	// MatthewKush to all: If non-MSAA, this is better (but only if the sampler uses is smp_nofilter)
#	else
	float4	L		= s_accumulator.Load(int3(I.tc0 * screen_res.xy, 0));
#	endif
#else
#	ifdef GBUFFER_OPTIMIZATION
	float4	L		= s_accumulator.Load(int3(I.pos2d.xy, 0), ISAMPLE);	// diffuse.specular
#	else
	float4	L		= s_accumulator.Load(int3(I.tc0 * screen_res.xy, 0), ISAMPLE);
#	endif
#endif

#ifdef USE_SUPER_SPECULAR
	float	ds		= dot(D.rgb, 1.0f/3.0f);
			D.w		= max(D.w, ds*ds/8.0f);
#endif

#ifdef FORCE_GLOSS
			D.w		= FORCE_GLOSS;
#endif

#ifdef USE_GAMMA_22
			D.rgb	= (D.rgb*D.rgb); // pow(2.2)
#endif

	// static sun
	float	mtl		= P.w;

#ifdef USE_R2_STATIC_SUN
	float	sun_occ	= P.w*2;
			mtl		= xmaterial;
			L	   += Ldynamic_color * sun_occ * plight_infinity(mtl, P.xyz, N.xyz, Ldynamic_dir);
#endif
	// hemisphere
	float3 hdiffuse, hspecular;

	//  Calculate SSAO
#ifdef USE_MSAA
#	ifdef GBUFFER_OPTIMIZATION
	int2	texCoord = I.pos2d;
#	else
	int2	texCoord = int2(I.tc0 * screen_res.xy);
#	endif
#endif
	
/*#ifdef USE_SSAO_BLUR	
#	ifndef USE_MSAA
	float	occ = ssao_blur_ps(I.tc0);
#	else
	float   occ = ssao_blur_ps( texCoord, ISAMPLE );
#	endif*/

#ifdef USE_HBAO
	float	occ = calc_hbao(P.z, N, I.tc0, I.pos2d);
#else
	float	occ = calc_ssao(CS_P(P, N, I.tc0, I.tcJ, I.pos2d, ISAMPLE));
#endif
	hmodel(hdiffuse, hspecular, mtl, N.w, D.w, P.xyz, N.xyz);
	hdiffuse	*= occ;
	hspecular	*= occ;
		float3 _tsView = -normalize(P.xyz);
		float3 _tsLight = -Ldynamic_dir.xyz;
		
	float4	light	= float4(L.rgb + hdiffuse, L.w);
	float roughness = 1.0 - D.w;
	roughness = roughness * roughness;
	float real_ruff = roughness * roughness;
	float3 _F0 = float3(0.04,0.04,0.04);
//float	NdotV = dot( N, _tsView );
float3 BRDF_Light = ComputeBRDF_Full( N, _tsView, _tsLight, real_ruff, _F0, roughness, D.rgb, true, true );	
	BRDF_Light.z += hdiffuse;
	BRDF_Light.x += hspecular;
	
	float4	C		= D*light;						// rgb.gloss * light(diffuse.specular)
	float3	spec	= C.www * L.rgb + hspecular;	// replicated specular

	float3	color	= C.rgb + spec;

	// here should be distance fog
	float3	pos			= P.xyz;
	float	dist		= length(pos);
	float	fog			= saturate(dist*fog_params.w + fog_params.x);
			color		= lerp(color,fog_color,fog);
	float	skyblend	= saturate(fog*fog);

	float	tm_scale	= I.tc0.w; // interpolated from VS

#ifdef USE_SUPER_SPECULAR
			color		= spec - hspecular;
#endif
	_out o;
	tonemap(o.low, o.high, color, tm_scale);
	o.low.a		= skyblend;
	o.high.a	= skyblend;

	return o;
}
