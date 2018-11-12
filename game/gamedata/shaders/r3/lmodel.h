#ifndef	LMODEL_H
#define LMODEL_H

#include "common.h"


float3 Diffuse_OrenNayar( float Roughness, float NoV, float NoL, float VoH )
{
	float a = Roughness * Roughness;
	float s = a;// / ( 1.29 + 0.5 * a );
	float s2 = s * s;
	float VoL = 2 * VoH * VoH - 1;		// double angle identity
	float Cosri = VoL - NoV * NoL;
	float C1 = 1 - 0.5 * s2 / (s2 + 0.33);
	float C2 = 0.45 * s2 / (s2 + 0.09) * Cosri * ( Cosri >= 0 ? rcp( max( NoL, NoV ) ) : 1 );
	return ( C1 + C2 ) * ( 1 + Roughness * 0.5 );
}

float D_GGX( float a2, float NoH )
{
	float d = ( NoH * a2 - NoH ) * NoH + 1;	// 2 mad
	return a2 / ( PI*d*d );					// 4 mul, 1 rcp
}

float Vis_SmithJointApprox( float a2, float NoV, float NoL )
{
	float a = sqrt(a2);
	float Vis_SmithV = NoL * ( NoV * ( 1 - a ) + a );
	float Vis_SmithL = NoV * ( NoL * ( 1 - a ) + a );
	return 0.5 * rcp( Vis_SmithV + Vis_SmithL );
}
float Pow4( float x )
{
	float xx = x*x;
	return xx * xx;
}

float sqr(float x)
{
return x * x;	
}
float3	FresnelDielectric( float _cosTheta, float _fresnelStrength=1.0 ) {
	float	c = lerp( 1.0, _cosTheta, _fresnelStrength );
    float nn = 1.5;

    float f0 = pow((1 - nn)/(1 + nn), 2);
	float3	g_squared = max( 0.0, f0*f0 - 1.0 + c*c );
// 	if ( g_squared < 0.0 )
// 		return 1.0;	// Total internal reflection

	float3	g = sqrt( g_squared );

	float3	a = (g - c) / (g + c);
			a *= a;
	float3	b = (c * (g+c) - 1.0) / (c * (g-c) + 1.0);
			b = 1.0 + b*b;

	return 0.5 * a * b;
}
float3 F_Fresnel(float3 SpecularColor, float VoH)
{
	float3 SpecularColorSqrt = sqrt(min(SpecularColor, float3(0.99, 0.99, 0.99)));
	float3 n = (1 + SpecularColorSqrt) / (1 - SpecularColorSqrt);
	float3 g = sqrt(n*n + VoH*VoH - 1);
	return 0.5 * sqr((g - VoH) / (g + VoH)) * (1 + sqr(((g + VoH)*VoH - 1) / ((g - VoH)*VoH + 1)));
}
//////////////////////////////////////////////////////////////////////////////////////////
// Lighting formulas			// 
float4 plight_infinity( float m, float3 pnt, float3 normal, float3 light_direction )
{
  	float3 N			= normal;							// normal 
  	float3 V 		= -normalize	(pnt);					// vector2eye
  	float3 L 		= -light_direction;						// vector2light
  	float3 H			= normalize	(L+V);						// float-angle-vector 
//	return tex3D 		(s_material,	float3( dot(L,N), dot(H,N), m ) );		// sample material
	//return s_material.Sample( smp_material, float3( dot(L,N), dot(H,N), m ) ).xxxy;		// sample material
	//float3 H = normalize(V + L);			// 3 add, 2 mad, 4 mul, 1 rsqrt
	float NoL = saturate( dot(N, L) );		// 2 mad, 1 mul
	float NoH = saturate( dot(N, H) );		// 2 mad, 1 mul
	float VoH = saturate( dot(V, H) );		// 2 mad, 1 mul
    float NoV = dot(N, V);	
	float Roughness = saturate(1.0 - L_material.w);//0.5;
	float a2 = Pow4(Roughness);
	    // refractive index
	float GGX = D_GGX(a2, NoH);
	float Vis = Vis_SmithJointApprox(a2, NoV, NoL);
	float3 F0 = (0.04,0.04,0.04);
	float Fresnel = F_Fresnel(F0,VoH);//FresnelDielectric(VoH, 0.5).x;
	//float DFresenel = FresnelDielectric(NoL).x;
	float3 Diffuse = NoL * (1/PI);
	float Specular = GGX * Vis * Fresnel * (1/PI);
	return float4(Diffuse,Specular).xxxy;
}
/*
float plight_infinity2( float m, float3 pnt, float3 normal, float3 light_direction )
{
  	float3 N		= normal;							// normal 
  	float3 V 	= -normalize		(pnt);		// vector2eye
  	float3 L 	= -light_direction;					// vector2light
 	float3 H		= normalize			(L+V);			// float-angle-vector 
	float3 R     = reflect         	(-V,N);
	float 	s	= saturate(dot(L,R));
			s	= saturate(dot(H,N));
	float 	f 	= saturate(dot(-V,R));
			s  *= f;
	float4	r	= tex3D 			(s_material,	float3( dot(L,N), s, m ) );	// sample material
			r.w	= pow(saturate(s),4);
  	return	r	;
}
*/

float4 plight_local( float m, float3 pnt, float3 normal, float3 light_position, float light_range_rsq, out float rsqr )
{
  	float3 N		= normal;							// normal 
  	float3 L2P 	= pnt-light_position;                         		// light2point 
  	float3 V 	= -normalize	(pnt);					// vector2eye
  	float3 L 	= -normalize	((float3)L2P);					// vector2light
  	float3 H		= normalize	(L+V);						// float-angle-vector
		rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
  	float  att 	= saturate	(1 - rsqr*light_range_rsq);			// q-linear attenuate
//	float4 light	= tex3D		(s_material, float3( dot(L,N), dot(H,N), m ) ); 	// sample material
	float4 light	= s_material.Sample( smp_material, float3( dot(L,N), dot(H,N), m ) ).xxxy;		// sample material
  	return att*light;
}

//	TODO: DX10: Remove path without blending
float4 blendp( float4 value, float4 tcp)
{
//	#ifndef FP16_BLEND  
//		value 	+= (float4)tex2Dproj 	(s_accumulator, tcp); 	// emulate blend
//	#endif
	return 	value;
}

float4 blend( float4 value, float2 tc)
{
//	#ifndef FP16_BLEND  
//		value 	+= (float4)tex2D 	(s_accumulator, tc); 	// emulate blend
//	#endif
	return 	value;
}

#endif