#include "common.h"

struct _in
{
	float4 	p	: POSITION;		// xy=pos, zw=tc0
	float2	tcJ	: TEXCOORD0;	// jitter coords
};

struct _out
{
	float4 	hpos:	POSITION;
#ifdef	USE_VTF
  	float4 	tc0:	TEXCOORD0;	// tc.xy, tc.w = tonemap scale
#else
  	float2 	tc0:	TEXCOORD0;	// tc.xy 
#endif
	float2	tcJ:	TEXCOORD1;	// jitter coords
};
//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
_out main (_in I)
{
	_out O;
	O.hpos = float4(I.p.x, -I.p.y, 0.0f, 1.0f);
#ifdef	USE_VTF
	float	scale 	= tex2Dlod(s_tonemap,float4(0.5f,0.5f,0.5f,0.5f)).x;
	O.tc0			= float4(I.p.zw, scale, scale);
#else
	O.tc0			= I.p.zw;
#endif
	O.tcJ			= I.tcJ;

 	return	O;
}

FXVS;
