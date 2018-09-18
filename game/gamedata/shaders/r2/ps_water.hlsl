#include "common.h"

struct v2p
{
	float4	hpos		: POSITION;
	float2	tbase		: TEXCOORD0;  // base
	float4	tnorm		: TEXCOORD1;  // xy=nm0, zw=nm1
	float3	M1			: TEXCOORD2;
	float3	M2			: TEXCOORD3;
	float3	M3			: TEXCOORD4;
	float3	v2point		: TEXCOORD5;
	float4 	position	: TEXCOORD6;	// xyz=view pos, w=hemi occlusion
#ifdef NEED_SOFT_WATER
	float4	tctexgen	: TEXCOORD7; 	// position in screen space of current pixel
#endif
	float4	c0			: COLOR0;	// xyz=lighting, w=fog
};

uniform sampler2D	s_nmap;
uniform sampler2D	s_leaves;

#ifdef NEED_SOFT_WATER
uniform float3 water_intensity;
#endif

float4 main(v2p I) : COLOR
{
	// diffuse texture
	float4	base 	= tex2D(s_base, I.tbase);
	
	// base normals
	float3	n0		= tex2D(s_nmap, I.tnorm.xy).xyz;
	float3	n1		= tex2D(s_nmap, I.tnorm.zw).xyz;
	float3	Navg	= n0 + n1 - 1.0f;
	float3	Nw		= normalize(mul(float3x3(I.M1, I.M2, I.M3), Navg));
	float3	Nv		= normalize(mul((float3x3)m_V, Nw));
			
	float4 	Pv		= I.position;

	// reflections
	float3	v2point	= normalize (I.v2point);
	float3	vreflect= reflect(v2point, Nw);
			vreflect.y= vreflect.y*2.0f - 1.0f;     // fake remapping

	float3 env		= calc_envmap(vreflect) * 2.0f;
	
	// apply lighting to diffuse texture
	base.xyz	   *= I.c0.xyz * 2.0f;

	float3	final;

	float	fresnel			= saturate(dot(vreflect,v2point));
	float	power			= pow(fresnel, 9.0f);
	float	amount			= 0.15f + 0.25f * power;	// 1=full env, 0=no env
	float	alpha			= 0.75f + 0.25f * power;	// 1=full env, 0=no env

#ifndef NO_ENV
			env 		   *= amount;
			final 			= lerp(env, base.xyz, base.a);
#else
			final 			= base.xyz;
#endif
			final		   *= I.c0 * 2.0f;
			

#ifdef NEED_SOFT_WATER
	#ifdef NO_ENV
		alpha	*= 0.5; // make water more opaque
	#endif

	// Igor: additional depth test
	float4 _P 				= tex2Dproj(s_position, I.tctexgen);
	float waterDepth 		= _P.z - I.tctexgen.z;

	// water fog
	float	fog_exp_intens	= -4.0f;
	float	fog				= 1.0f - exp(fog_exp_intens * waterDepth);
	float3	Fc				= float3(0.1f, 0.1f, 0.1f) * water_intensity.r;
			final			= lerp(Fc, final.xyz, alpha);

	// fade water in a fog
	alpha 					= min(alpha, saturate(waterDepth));
	alpha 					= max(fog, alpha);//*I.c0.a;

	// Leaves
//	float 	calc_cos 		= dot (I.M3, v2point);
	float	calc_cos 		= -dot(float3(I.M1.z, I.M2.z, I.M3.z), v2point);
	float	calc_depth 		= saturate(waterDepth * calc_cos);
	float	fLeavesFactor 	= smoothstep(0.025f, 0.050f, calc_depth);
			fLeavesFactor  *= smoothstep(0.100f, 0.075f, calc_depth);
			
	float4	leaves			= tex2D(s_leaves, I.tbase);
			leaves.rgb	   *= water_intensity.r;
			
	final					= lerp(final, leaves.xyz, leaves.a*fLeavesFactor);
	alpha					= lerp(alpha, leaves.a, leaves.a*fLeavesFactor);
#endif	// NEED_SOFT_WATER

	// Output
	return float4(final, alpha*I.c0.a*I.c0.a);
}
