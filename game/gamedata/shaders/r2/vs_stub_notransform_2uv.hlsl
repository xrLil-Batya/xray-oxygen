#include "common.h"

struct v_TL2uv
{
	float4	P		: POSITION;
	float2	tc0		: TEXCOORD0;
	float2	tc1		: TEXCOORD1; 
	float4  c0		: COLOR0;
};

struct v2p_TL2uv
{
	float4 	hpos	: POSITION;	// Clip-space position 	(for rasterization)
	float2 	tc0		: TEXCOORD0;
	float2	tc1		: TEXCOORD1;
	float4  c0		: COLOR0;
};

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_TL2uv main (v_TL2uv I)
{
	v2p_TL2uv o;

	o.hpos 	= I.P;
	o.tc0 	= I.tc0;
	o.tc1 	= I.tc1;
	o.c0 	= I.c0.bgra;	//	swizzle vertex colour

 	return o;
}
