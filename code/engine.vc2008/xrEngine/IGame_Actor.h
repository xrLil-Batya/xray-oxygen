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
	virtual void IR_OnKeyboardPress(u8 dik) {}
	virtual void IR_OnKeyboardRelease(u8 dik) {}
	virtual void IR_OnKeyboardHold(u8 dik) {}
	virtual void IR_OnMouseWheel(int direction) {}

	virtual BOOL g_Alive() const { return false; }

	IC CCameraBase*		cam_Active()   { return cameras[cam_active]; }
	IC CCameraBase*		cam_FirstEye() { return cameras[eacFirstEye]; }
	// KD: need to know which cam active outside actor methods
	IC EActorCameras	active_cam()   { return cam_active; }
	// Alundaio: made public
	virtual	void			cam_Set(EActorCameras style);  
public:

	//режим подбирания предметов
	bool m_bPickupMode;

	u32	mstate_wishful;
	u32	mstate_old;
	u32	mstate_real;
};

extern ENGINE_API CIActor* g_actor;
extern ENGINE_API int psActorSleepTime;
