#pragma once
#include "../xrEngine/feel_touch.h"

//this class implements only denie functionality
class GlobalFeelTouch : public Feel::Touch
{
public:
							GlobalFeelTouch();
	virtual					~GlobalFeelTouch();

	virtual void			feel_touch_update			(Fvector& P, float	R);

			bool			is_object_denied			(CObject const * O);
}; //class GlobalFeelTouch
