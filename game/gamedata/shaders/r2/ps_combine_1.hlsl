#include "common.h"

//#define USE_SUPER_SPECULAR
//#define USE_ORIGINAL_SSAO
//#define HBAO_WORLD_JITTER

uniform	sampler2D	s_half_depth;
uniform sampler2D	fog_texture;

uniform float4 wind_params; // x = direction, y = velocity

#include "lmodel.h"
#include "hmodel.h"

#include "ps_ssao_blur.hlsl"
#include "ps_ssao.hlsl"
#include "ps_ssao_hbao.hlsl"

struct _input
{
	float4	hpos	: POSITION;
#ifdef USE_VTF
	float4	tc0		: TEXCOORD0;	// tc.xy, tc.w = tonemap scale
#else
	float2	tc0		: TEXCOORD0;	// tc.xy
#endif
	float2	tcJ		: TEXCOORD1;	// jitter coords
};

struct _out
{
	float4	low		: COLOR0;
	float4	high	: COLOR1;
};

uniform sampler1D fog_table;

_out main (_input I)
{
	// Sample the buffers:
	float4	P		= tex2D(s_position,      I.tc0);	// position.(mtl or sun)
	float4	N		= tex2D(s_normal,        I.tc0);	// normal.hemi
	float4	D		= tex2D(s_diffuse,       I.tc0);	// rgb.gloss
	float4	L		= tex2D(s_accumulator,   I.tc0);	// diffuse.specular

#ifdef USE_SUPER_SPECULAR
	float	ds		= dot(D.rgb, 1.0f/3.0f);
			D.w		= max(D.w, 	ds*ds/8.0f);
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
	float	sun_occ	= P.w * 2.0f;
			mtl		= xmaterial;
            L	   += Ldynamic_color * sun_occ * plight_infinity(mtl, P.xyz, N.xyz, Ldynamic_dir);
#endif

	// hemisphere
	float3	hdiffuse, hspecular;

	// Calculate SSAO
//#ifdef USE_SSAO_BLUR
//	float	occ		= ssao_blur_ps(I.tc0);
#ifdef USE_HBAO
	float	occ		= calc_hbao(P.z, N, I.tc0);
#else
	float	occ		= calc_ssao(P, N, I.tc0, I.tcJ);
#endif
	hmodel(hdiffuse, hspecular, mtl, N.w, D.w, P.xyz, N.xyz);
//	hmodel(hdiffuse, hspecular, mtl, 1, D.w, P.xyz, N.xyz);
//	hdiffuse*=hdiffuse;			//. high contrast hemi
//	hdiffuse*=(D.rgb*0.8 + 0.2f);	// rise texture contrast for diffuse lighting
//	hdiffuse = 0.8;
//	hdiffuse *= (occ*(D.rgb + .1f));
	hdiffuse	*= occ;
	hspecular	*= occ;

	float4	light	= float4(L.rgb + hdiffuse, L.w);
	float4	C		= D * light;														// rgb.gloss * light(diffuse.specular)
//	float3	spec	= (C.rgb*0.5f + 0.5f)*C.w + hspecular + hspecular + hspecular;	// replicated specular
	float3	spec	= C.www * L.rgb + hspecular;									// replicated specular

#ifdef USE_SUPER_SPECULAR
			spec	= (C.rgb*0.5f + 0.5f)*C.w + hspecular;
#endif
//	float3	color	= C.rgb + D.rgb*spec;
//	float3	color	= C.rgb + (D.rgb*spec+spec)/0.5f;
	float3	color	= C.rgb + spec;
//	float3	color	= C.rgb + D.rgb*spec+hspecular+hspecular;	//	More realistic and contrast specular - Ugrumiy edition

////////////////////////////////////////////////////////////////////////////////
/// For Test ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef DBG_TEST_NMAP
			color	= hdiffuse + L.rgb + N;
#endif
#ifdef DBG_TEST_NMAP_SPEC
			color 	= hdiffuse + L.rgb + N + spec;
#endif
#ifdef DBG_TEST_LIGHT
			color 	= hdiffuse + L.rgb;
#endif
#ifdef DBG_TEST_LIGHT_SPEC
			color 	= hdiffuse + L.rgb + spec;
#endif
#ifdef DBG_TEST_SPEC
			color	= spec;
#endif
////////////////////////////////////////////////////////////////////////////////

	// distance fog
	float	fog_speed	= 0.1f;
	float3	pos			= P.xyz;
	float	dist		= length(pos);
		
	// linear fog
	float	fog_factor	= saturate(dist*fog_params.w + fog_params.x);
			color		= lerp(color, fog_color, fog_factor);
	float	skyblend	= saturate(fog_factor * fog_factor);

	// exponential squared fog with texture
//	float	density		= dist * fog_color.w;
//	float	fog_factor	= 1.0f / exp(density*density);
//			fog_factor	= clamp(fog_factor, 0.0f, 1.0f);

//	float	be			= (eye_position.y - P.y) * 0.004; // 0.004 is just a factor
//	float	bi			= (eye_position.y - P.y) * 0.001; // 0.001 is just a factor
 
//	float	be			= 0.025 * smoothstep(0.0, 6.0, 32.0 - viewSpace.y);
//	float	bi			= 0.075* smoothstep(0.0, 80, 10.0 - viewSpace.y);
//	float	ext 		= exp(-dist * be);
//	float	insc 		= exp(-dist * bi);
		
//	float2	fog_tc		= I.tc0.xy * clamp(fog_factor, 0.0f, 0.5f) * 2.0f;
//			fog_tc.x	= fog_tc.x + timers.z * max(wind_params.y, 1.0f) * fog_speed;
//	float3	fog_t		= tex2D(fog_texture, fog_tc) * fog_color.xyz; 
//			fog_t		= color * ext + fog_t * (1.0f - insc);
//			color		= lerp(color, fog_t, 1.0f - fog_factor);

#ifdef DBG_TMAPPING
			color		= D.xyz;
#endif
        // final tone-mapping
#ifdef USE_VTF
	float	tm_scale	= I.tc0.w;                // interpolated from VS
#else
	float	tm_scale	= tex2D(s_tonemap, I.tc0).x;
#endif

#ifdef USE_SUPER_SPECULAR
			color		= spec - hspecular;
#endif
//		color 		= N; 					// show normals
//		color 		= N.w; 					// show precalculated AO
//		color 		= float4(occ.xxx,1.0f); // show occlusion
//		color		= occ;
//		color 		= float4(P.zzz,1.0f);	// show z-buffer
//		color 		= float4(P.www,1.0f);	// show stencil
//		color		= L;					// show diffuse lighting
//		color 		= float4(L.www,1.0f);	// show specular

	_out o;
	tonemap(o.low, o.high, color, tm_scale);
	o.low.a		= skyblend;
	o.high.a	= skyblend;

	return o;
}
