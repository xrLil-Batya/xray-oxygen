#pragma once
#include "xr_object.h"
#include "CameraBase.h"

enum EActorCameras 
{
	eacFirstEye = 0,
	eacLookAt,
	eacFreeLook,
	eacMaxCam
};

class ENGINE_API CIActor
{
protected:
	CCameraBase*   cameras[eacMaxCam];
	EActorCameras  cam_active;

public:
	virtual ~CIActor() = default;

		    void StopAnyMove() {}

	virtual void IR_OnMouseMove(int x, int y) {}
	virtual void IR_OnKeyboardPress(int dik) {}
	virtual void IR_OnKeyboardRelease(int dik) {}
	virtual void IR_OnKeyboardHold(int dik) {}
	virtual void IR_OnMouseWheel(int direction) {}

	virtual BOOL g_Alive() const { return false; }

	IC CCameraBase*	cam_Active()   { return cameras[cam_active]; }
	IC CCameraBase*	cam_FirstEye() { return cameras[eacFirstEye]; }
public:

	//режим подбирания предметов
	bool m_bPickupMode;

	u32	mstate_wishful;
	u32	mstate_old;
	u32	mstate_real;
};

extern ENGINE_API CIActor* g_actor;
extern ENGINE_API int psActorSleepTime;
