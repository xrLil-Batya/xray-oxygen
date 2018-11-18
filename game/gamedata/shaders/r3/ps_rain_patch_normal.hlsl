#include "common.h"
#include "lmodel.h"
#include "shadow.h"

Texture3D s_water;
Texture2D s_waterFall;
float4 RainDensity;	//	float
//float4	WorldX;	//	Float3	
//float4	WorldZ;	//	Float3	


float3	GetNVNMap(Texture3D s_texture, float2 tc, float time)
{
	// Unpack NVidia normal map
//	float4 water = s_water.SampleLevel( smp_linear, float3(tc, time), 0 ) - 0.5;
//	float4 water = s_water.Sample( smp_linear, float3(tc, time)) - 0.5;
	float4 water = s_texture.SampleBias(smp_base, float3(tc, time), -3.0f) - 0.5f;

	// Swizzle
	water.x = water.w;

	// Renormalize (*2) and scale (*3)
	water.xz *= 6;

//	water.y = sqrt(1-water.x*water.x-water.z*water.z);
	water.y = 0.1f;

	return water.xyz;
}


float3	GetWaterNMap( Texture2D s_texture, float2 tc)
{
	//	Unpack normal map
//	float4 water = s_texture.SampleLevel( smp_base, float3(tc, time), 0 );
	float4 water = s_texture.Sample(smp_base, tc);
	water.xyz = (water.xzy - 0.5f)*2.0f;

//	water.xyz *= 3;
//	water.xyz *= 0.3;

	water.y = 0.1f;

	return water.xyz;
}


#ifndef ISAMPLE
#define ISAMPLE 0
#endif

#ifdef MSAA_OPTIMIZATION
float4 main (float2 tc : TEXCOORD0, float2 tcJ : TEXCOORD1, float4 Color : COLOR, float4 pos2d : SV_Position, uint iSample : SV_SAMPLEINDEX) : SV_Target
#else
float4 main (float2 tc : TEXCOORD0, float2 tcJ : TEXCOORD1, float4 Color : COLOR, float4 pos2d : SV_Position) : SV_Target
#endif
{
#ifdef MSAA_OPTIMIZATION
	gbuffer_data gbd = gbuffer_load_data(tc, pos2d, iSample);
#else
	gbuffer_data gbd = gbuffer_load_data(tc, pos2d, ISAMPLE);
#endif

	float4	_P 			= float4(gbd.P, 1.0f);
	float3	_N 			= normalize(gbd.N);
	float3	D 			= gbd.C;		// rgb	//.gloss

	float4	PS 			= mul(m_shadow,  _P);

	float3	WorldP 		= mul(m_sunmask, _P);
	float3	WorldN 		= mul(m_sunmask, _N);

	// Read rain projection with some jitter. Also adding pixel normal 
	// factor to jitter to make rain strips more realistic.
	float 	s 			= shadow_rain(PS, WorldP.xz - WorldN.xz);
	
	// Apply distance falloff
	//float	fAtten = 1 - smoothstep( 10, 30, length( _P.xyz ));
	// Using fixed fallof factors according to float16 depth coordinate precision.
	float	fAtten 		= 1 - smoothstep(5, 25, _P.z);
			s 		   *= fAtten*fAtten;

	// Apply rain density
			s		   *= RainDensity.x;

	float	fIsUp		= -dot(Ldynamic_dir.xyz, _N);
			s  		   *= saturate(fIsUp*10+(10*0.5f)+0.5f);
			fIsUp 		= max(0, fIsUp);

	float	fIsX 		= WorldN.x;
	float	fIsZ 		= WorldN.z;

	float3	waterSplash	= GetNVNMap(s_water, WorldP.xz, timers.x*3.0);
//	float3	waterFallX 	= GetWaterNMap( s_waterFall, float2(WorldP.x, WorldP.y-timers.x), 0.5 );
//	float3	waterFallZ 	= GetWaterNMap( s_waterFall, float2(WorldP.z, WorldP.y-timers.x), 0.5 );

	float3	tc1 		= WorldP / 2.0f;

	float	fAngleFactor	= 0.1f*ceil((1.0f - fIsUp)*10.0f);
	// Just slow down effect.
			fAngleFactor   *= 0.5f;

	float 	fTmp 		= tc1.y + timers.x*fAngleFactor;
	float3	waterFallX 	= GetWaterNMap(s_waterFall, float2(tc1.z, fTmp));
	float3	waterFallZ 	= GetWaterNMap(s_waterFall, float2(tc1.x, fTmp));

	float2	IsDir 		= normalize(float2(fIsZ, fIsX));

	float3	waterFall 	= GetWaterNMap(s_waterFall, float2(dot(tc1.xz, IsDir), tc1.y+timers.x));

	float3	water 		= waterSplash*(fIsUp*s);
			water 	   += waterFallX.yxz*(abs(fIsX)*s);
			water 	   += waterFallZ.zxy*(abs(fIsZ)*s);
	// Translate NM to view space
			water.xyz 	= mul(m_V, water.xyz);
	
			_N 		   += water.xyz;
			_N 			= normalize(_N);
			
			s 		   *= dot(D.xyz, float3(0.33f, 0.33f, 0.33f));
//			s 		   *= 0.5f*fIsUp+0.5f;

	return float4(_N, s);
}
