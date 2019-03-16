#include "common.h"

struct _in
{
	float4 P	: POSITION;
	float2 tc	: TEXCOORD0;
};

p_screen main (_in v)
{
	p_screen o;

	o.hpos 	= v.P;		// xform, input in world coords
	o.tc0 	= v.tc;				// copy tc

	return o;
}