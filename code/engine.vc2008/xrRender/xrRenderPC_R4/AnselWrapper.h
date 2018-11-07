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

class AnselManager : public CGameAnsel, public CObject, public pureFrame
{
	void*					pAnselModule;
	AnselCamera				Camera;
	AnselCameraEffector		Effector;
	CTimer					Timer;
	float					fTimeDelta;

public:
	AnselManager();

	virtual bool Load();
	void Unload();
	bool Init() const;
	void OnFrame() override;
};