#include	"stdafx.h"
#include	"animation.h"

using	namespace animation;

const channal_rule channels::rules[channels::max] = {{lerp, lerp}, {lerp, lerp}, {lerp, add}, {lerp, add}};

channels::channels()
{
	init( );
}

void channels::init()
{
	memset(factors, 1.f, max * sizeof(*factors));
}

void channels::set_factor(u16 channel, float factor)
{
	VERIFY(channel < max);
	factors[channel] = factor;
}