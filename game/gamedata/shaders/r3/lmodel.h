#ifndef	LMODEL_H
#define LMODEL_H

#include "common.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Lighting formulas			// 
float4 plight_infinity( float m, float3 pnt, float3 normal, float3 light_direction )
{
  	float3 N			= normal;							// normal 
  	float3 V 		= -normalize	(pnt);					// vector2eye
  	float3 L 		= -light_direction;						// vector2light
  	float3 H			= normalize	(L+V);						// float-angle-vector 
	float NdotL = dot(N,L);
	float specular = dot(H,N);
	specular = pow(specular,64);	
	return float4(NdotL.xxx,specular); //this seemns faster but I need feedback
	
	//s_material.Sample( smp_material, float3( dot(L,N), dot(H,N), m ) ).xxxy;		// sample material
}

float4 plight_local(float m, float3 pnt, float3 normal, float3 light_position, float light_range_rsq, out float rsqr)
{

	float3 N		= normal;										// normal 
	float3 L2P 	= pnt - light_position;                         	// light2point 
	float3 V 		= -normalize	(pnt);							// vector2eye
	float3 L 		= -normalize	((float3)L2P);					// vector2light
	float3 H		= normalize	(L+V);								// half-angle-vector
		   rsqr		= dot		(L2P,L2P);							// distance 2 light (squared)
	float  att 		= saturate	(1 - rsqr*light_range_rsq);			// q-linear attenuate

	float4 light = s_material.Sample(smp_material, float3(dot(L, N), dot(H, N), m)).xxxy;
	
  return float4(att * light.xxx,0) + att*float4(light.www * (Ldynamic_color.xyz * Ldynamic_color.xyz),light.w);
}

//	TODO: DX10: Remove path without blending
float4 blendp( float4 value, float4 tcp)
{
	return 	value;
}

float4 blend( float4 value, float2 tc)
{
	return 	value;
}

#endif