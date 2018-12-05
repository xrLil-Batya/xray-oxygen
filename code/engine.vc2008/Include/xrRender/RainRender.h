#pragma once

class CEffectRain;

#include "../../xrCore/_sphere.h"

class IRainRender
{
public:
	virtual					~IRainRender	() {;}
	virtual void			Copy			(IRainRender &_in)		= 0;
	virtual void			Render			(CEffectRain &owner)	= 0;
	virtual const Fsphere&	GetDropBounds	() const				= 0;
};
