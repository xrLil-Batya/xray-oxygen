#include "common.h"

struct 	v2p
{
	float2 	tc0		: TEXCOORD0;	// base
	float4	c		: COLOR0;		// diffuse
#ifdef USE_SOFT_PARTICLES
	float4	tctexgen: TEXCOORD1;
#endif
	float	fog		: FOG;		// fog
};

//	Must be less than view near
#define	DEPTH_EPSILON 0.1f
//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
float4 main(v2p I) : COLOR
{
	float4 result		= I.c * tex2D(s_base, I.tc0);

	if (result.w <= 0.00001f)
		discard;
	
	// Igor: additional depth test
#ifdef USE_SOFT_PARTICLES
	float4	_P			= tex2Dproj(s_position, I.tctexgen);
	float	spaceDepth	= _P.z - I.tctexgen.z - DEPTH_EPSILON;
	if (spaceDepth < -2.0f*DEPTH_EPSILON)
		spaceDepth = 100000.0f; // Skybox doesn't draw into position buffer
		
	result *= Contrast(saturate(spaceDepth*1.3f), 2.0f);
#endif
	result.w *= I.fog*I.fog; // skyloader: fog fix
	
	return result;
}
