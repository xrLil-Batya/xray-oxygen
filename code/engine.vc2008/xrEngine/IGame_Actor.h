#pragma once
#include "xr_object.h"

class ENGINE_API CIActor
{
public:
	virtual ~CIActor() = default;

		    void StopAnyMove() {}

	virtual void IR_OnMouseMove(int x, int y) {}
	virtual void IR_OnKeyboardPress(int dik) {}
	virtual void IR_OnKeyboardRelease(int dik) {}
	virtual void IR_OnKeyboardHold(int dik) {}
	virtual void IR_OnMouseWheel(int direction) {}

	virtual BOOL g_Alive() { return false; }
public:

	//режим подбирания предметов
	bool m_bPickupMode;

	u32	mstate_wishful;
	u32	mstate_old;
	u32	mstate_real;
};

extern ENGINE_API CIActor* g_actor;
extern ENGINE_API int psActorSleepTime;