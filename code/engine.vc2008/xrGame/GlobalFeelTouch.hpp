#pragma once
#include "../xrEngine/feel_touch.h"

//this class implements only denied functionality
class GlobalFeelTouch : public Feel::Touch
{
public:
				GlobalFeelTouch		() = default;
	virtual		~GlobalFeelTouch	() = default;

	void		feel_touch_update	(Fvector& P, float R) noexcept override;
	bool		is_object_denied	(CObject const* O) noexcept;
};
