#include "stdafx.h"
#include "Ansel.h"
#include "../../xrEngine/IGame_Actor.h"
#include "../../xrEngine/IGame_Persistent.h"
#include "../../xrEngine/IGame_Level.h"
#include <AnselSDK/AnselSDK.h>

static bool bInited = false;

AnselCamera::AnselCamera(CObject* p, u32 flags) : CCameraBase(p, flags) 
{

}

AnselCameraEffector::AnselCameraEffector() : CEffectorCam(cefAnsel, FLT_MAX) 
{

}

struct AnselManagerCallbackHandler
{
	AnselManagerCallbackHandler(xrAnsel* InManager)
		: pManager(InManager)
	{}
	xrAnsel* pManager = nullptr;

	ansel::StartSessionStatus StartSession(ansel::SessionConfiguration& pSessionConf)
	{
		if (!g_pGameLevel)
			return ansel::kDisallowed;

		pManager->bIsActive = true;

		if (g_pGamePersistent->m_pMainMenu->IsActive())
			g_pGamePersistent->m_pMainMenu->Activate(false);

		pSessionConf.maximumFovInDegrees = 140;

		Device.Pause(TRUE, TRUE, TRUE, "Nvidia Ansel");

		g_pGameLevel->Cameras().AddCamEffector(new AnselCameraEffector());
		Device.seqFrame.Add(pManager, REG_PRIORITY_CAPTURE);

		CCameraBase* C = nullptr;

		if (g_actor)
		{
			C = g_actor->cam_Active();
		}
		else
		{
			Log("! Failed to find camera for Ansel");
			return ansel::kDisallowed;
		}

		pManager->Camera.Set(C->Position(), C->Direction(), C->vNormal);
		pManager->Camera.f_fov = C->Fov();
		pManager->Camera.f_aspect = C->Aspect();
		return ansel::kAllowed;
	}
	
	void StopSession()
	{
		pManager->bIsActive = false;
		Device.Pause(FALSE, FALSE, FALSE, "Nvidia Ansel");
		g_pGameLevel->Cameras().RemoveCamEffector(cefAnsel);
		Device.seqFrame.Remove(pManager);
	}
};


BOOL AnselCameraEffector::ProcessCam(SCamEffectorInfo& info)
{
	info.dont_apply = false;

	static ansel::Camera camera;
	static nv::Vec3 right = { info.r.x, info.r.y, info.r.z };
	static nv::Vec3 up = { info.n.x, info.n.y, info.n.z };
	static nv::Vec3 forward = { info.d.x, info.d.y, info.d.z };

	ansel::rotationMatrixVectorsToQuaternion(right, up, forward, camera.rotation);

	camera.fov				 = info.fFov;
	camera.aspectRatio		 = info.fAspect;
	camera.farPlane			 = info.fFar;
	camera.projectionOffsetX = pGameAnsel->offsetX;
	camera.projectionOffsetY = pGameAnsel->offsetY;
	camera.nearPlane		 = pGameAnsel->viewportNear;

	ansel::updateCamera(camera);
	ansel::quaternionToRotationMatrixVectors(camera.rotation, right, up, forward);

	info.fFov					= camera.fov;
	info.fAspect				= camera.aspectRatio;
	info.fFar					= camera.farPlane;
	pGameAnsel->offsetX			= camera.projectionOffsetX;
	pGameAnsel->offsetY			= camera.projectionOffsetY;
	pGameAnsel->viewportNear	= camera.nearPlane;

	info.d.set(forward.x, forward.y, forward.z);
	info.n.set(up.x, up.y, up.z);
	info.r.set(right.x, right.y, right.z);
	return TRUE;
}

xrAnsel::xrAnsel() : Camera(this, 0), fTimeDelta(EPS_S)
{
	Timer.Start();
	pCallbackHandler = new AnselManagerCallbackHandler(this);
}

xrAnsel::~xrAnsel()
{
	delete pCallbackHandler;
	pCallbackHandler = nullptr;
}

bool xrAnsel::Load()
{
	return Init();
}

void xrAnsel::Unload()
{}

bool xrAnsel::Init()
{
	if (!bInited && ansel::isAnselAvailable())
	{
		ansel::Configuration config;
		config.titleNameUtf8 = u8"X-Ray Oxygen";
		config.right = { 1, 0, 0 };
		config.up = { 0, 1, 0 };
		config.forward = { 0, 0, 1 };
		config.fovType = ansel::kVerticalFov;
		// XXX: Support camera move
		config.isCameraTranslationSupported = false;
		config.rotationalSpeedInDegreesPerSecond = 220.0f;
		config.translationalSpeedInWorldUnitsPerSecond = 50.0f;
		config.captureLatency = 0;
		config.captureSettleLatency = 0;
		config.gameWindowHandle = Device.m_hWnd;
		config.userPointer = pCallbackHandler;

		config.startSessionCallback = [](ansel::SessionConfiguration& conf, void* context) -> ansel::StartSessionStatus
		{
			AnselManagerCallbackHandler* pOurHandler = reinterpret_cast<AnselManagerCallbackHandler*>(context);
			return pOurHandler->StartSession(conf);
		};

		config.stopSessionCallback = [](void* context)
		{
			AnselManagerCallbackHandler* pOurHandler = reinterpret_cast<AnselManagerCallbackHandler*>(context);
			pOurHandler->StopSession();
		};

		config.startCaptureCallback = nullptr;
		config.stopCaptureCallback = nullptr;
		const ansel::SetConfigurationStatus status = ansel::setConfiguration(config);
		switch (status)
		{
		case ansel::kSetConfigurationSuccess:
			Log("Nvidia Ansel is supported and used");
			return true;
		case ansel::kSetConfigurationIncompatibleVersion:
			Log("! Nvidia Ansel: incompatible version");
			break;
		case ansel::kSetConfigurationIncorrectConfiguration:
			Log("! Nvidia Ansel: incorrect configuration");
			break;

		case ansel::kSetConfigurationSdkNotLoaded:
			Log("! Nvidia Ansel: SDK wasn't loaded");
			break;
		}
		bInited = true;
	}
	else
		Log("! Nvidia Ansel:: failed to load AnselSDKxx.dll");

	return false;
}

void xrAnsel::OnFrame()
{
	const float previousFrameTime = Timer.GetElapsed_sec();
	Timer.Start();
	fTimeDelta = 0.3f * fTimeDelta + 0.7f * previousFrameTime;
	clamp(fTimeDelta, EPS_S, 0.1f);
	Device.fTimeDelta = fTimeDelta; // fake, to update cam (problem with fov)
	g_pGameLevel->Cameras().UpdateFromCamera(&Camera);
	Device.fTimeDelta = 0.0f; // fake, to update cam (problem with fov)
}

