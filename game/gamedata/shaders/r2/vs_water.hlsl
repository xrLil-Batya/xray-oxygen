#include "common.h"
#include "shared\waterconfig.h"
#include "shared\watermove.h"

struct vert
{
	float4	P		: POSITION;        // (float,float,float,1)
	float4	N		: NORMAL;        // (nx,ny,nz,hemi occlusion)
	float4	T		: TANGENT;
	float4	B		: BINORMAL;
	float4	color	: COLOR0;        // (r,g,b,dir-occlusion)
	float2	uv		: TEXCOORD0;        // (u0,v0)
};
struct v2p
{
	float4	hpos		: POSITION;
	float2	tbase		: TEXCOORD0;  // base
	float4	tnorm		: TEXCOORD1;  // xy=nm0, zw=nm1
	float3	M1			: TEXCOORD2;
	float3	M2			: TEXCOORD3;
	float3	M3			: TEXCOORD4;
	float3	v2point		: TEXCOORD5;
	float4 	position	: TEXCOORD6;
#ifdef NEED_SOFT_WATER
	float4	tctexgen	: TEXCOORD7; // position in screen space of current pixel
#endif
	float4	c0			: COLOR0;	// xyz=lighting, w=fog
};

uniform float4 ogse_c_rain; // x - rmap dist, y - flood level, z - wet level, w - rain intensity
uniform float4 c_depth;


v2p main(vert v)
{
	v2p o;

	float4 P = v.P; // world
	
#ifdef NEED_PUDDLES
	P.xyz = mul(m_W, P);
	float speed = PUDDLES_GROW_SPEED;
	P.y += saturate(ogse_c_rain.x*ogse_c_rain.w*speed)*c_depth.x;
	P.w = 1.0f;
#endif

#ifndef WATER_NO_MOVE
	P = watermove(v.P);
#endif

	float3 P_v		= mul(m_V, P);
	o.position		= float4(P_v.xyz, v.N.w);
    o.v2point 		= P.xyz - eye_position;

#if defined(NEED_PUDDLES) || defined(WATER_NO_MOVE)
	o.tbase = v.uv;
	float3 N = v.N.xyz;
	float3 T = v.T.xyz;
	float3 B = v.B.xyz;
#else
    o.tbase = unpack_tc_base(v.uv,v.T.w,v.B.w); // copy tc
	float3 N = unpack_bx2(v.N.xyz); // just scale (assume normal in the -.5f, .5f)
	float3 T = unpack_bx2(v.T.xyz); //
	float3 B = unpack_bx2(v.B.xyz); //
#endif

#ifndef WATER_NO_MOVE
    o.tnorm.xy = watermove_tc(o.tbase*W_DISTORT_BASE_TILE_0, P.xz, W_DISTORT_AMP_0);
    o.tnorm.zw = watermove_tc(o.tbase*W_DISTORT_BASE_TILE_1, P.xz, W_DISTORT_AMP_1);
#else
	o.tnorm = o.tbase.xyxy;
#endif

	// Calculate the 3x3 transform from tangent space to eye-space
	// TangentToEyeSpace = object2eye * tangent2object
	//                   = object2eye * transpose(object2tangent) (since the inverse of a rotation is its transpose)
	float3x3 xform = mul((float3x3)m_W, float3x3(
                                                T.x,B.x,N.x,
                                                T.y,B.y,N.y,
                                                T.z,B.z,N.z
						));
	// The pixel shader operates on the bump-map in [0..1] range
	// Remap this range in the matrix, anyway we are pixel-shader limited :)
	// ...... [ 2  0  0  0]
	// ...... [ 0  2  0  0]
	// ...... [ 0  0  2  0]
	// ...... [-1 -1 -1  1]
	// issue: strange, but it's slower :(
	// issue: interpolators? dp4? VS limited? black magic?

	// Feed this transform to pixel shader
	o.M1 = xform[0];
	o.M2 = xform[1];
	o.M3 = xform[2];

	float3 L_rgb	= v.color.xyz;							// precalculated RGB lighting
	float3 L_hemi 	= v_hemi(N) * v.N.w;					// hemisphere
	float3 L_sun 	= v_sun(N) * v.color.w;					// sun
	float3 L_final 	= L_rgb + L_hemi + L_sun + L_ambient.xyz;

	o.hpos = mul(m_VP, P);                        // xform, input in world coords
	float fog = saturate(calc_fogging(P));

	o.c0 = float4(L_final, fog);

//	Igor: for additional depth dest
#ifdef NEED_SOFT_WATER
	o.tctexgen = mul(m_texgen, P);
	o.tctexgen.z = P_v.z;
#endif
	return o;
}
