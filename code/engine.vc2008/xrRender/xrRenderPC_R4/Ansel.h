#pragma once
#include "../../xrEngine/CameraBase.h"
#include "../../xrEngine/CameraManager.h"
#include "../../xrEngine/Effector.h"
#include "../../xrEngine/xr_object.h"
#include "../../xrEngine/IGame_AnselSDK.h"

class AnselCamera : public CCameraBase
{
public:
	AnselCamera(CObject* p, u32 flags);
};

class AnselCameraEffector : public CEffectorCam
{
public:
	AnselCameraEffector();
	BOOL ProcessCam(SCamEffectorInfo& info) override;
};

struct AnselManagerCallbackHandler;

class xrAnsel : public IGameAnsel, public CObject, public pureFrame
{
	friend struct AnselManagerCallbackHandler;

public:
	xrAnsel();
	virtual ~xrAnsel();

	virtual bool Load() override;
	void Unload();
	bool Init();
	void OnFrame() override;

protected:

	AnselCamera					 Camera;
	AnselCameraEffector			 Effector;
	CTimer						 Timer;
	float						 fTimeDelta;
	AnselManagerCallbackHandler* pCallbackHandler;
};