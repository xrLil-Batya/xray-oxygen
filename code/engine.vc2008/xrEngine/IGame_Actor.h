#pragma once
#include "xr_object.h"

class CIActor
{
public:
	virtual ~CIActor() = default;

		    void StopAnyMove() {}

	virtual void IR_OnMouseMove(int x, int y) {}
	virtual void IR_OnKeyboardPress(int dik) {}
	virtual void IR_OnKeyboardRelease(int dik) {}
	virtual void IR_OnKeyboardHold(int dik) {}
	virtual void IR_OnMouseWheel(int direction) {}
public:

	//режим подбирания предметов
	bool m_bPickupMode;

	u32	mstate_wishful;
	u32	mstate_old;
	u32	mstate_real;
};
extern CIActor* g_actor;