#include "stdafx.h"
#include "ISpectreRenderLib.h"
#include "../xrEngine/Engine.h"
#include "../xrEngine/xr_input.h"
#include "../xrEngine/x_ray.h"
#include "../xrCDB/ISpatial.h"

extern ENGINE_API xr_vector<xr_token> vid_quality_token;

MANAGED_RENDER_API void xrRenderInit()
{
	//psDeviceFlags.set(rsR4, true);
	strcat(Core.Params, "-r4");
	vid_quality_token.emplace_back(xr_token("renderer_r4", 0));
	Engine.External.InitializeRenderer();
	Device.ConnectToRender();
	Device.m_pRender->SetupGPU(false, false, false);
}

// This function required to initialize static variables. LoadLibrary do not initialize them for some reason
MANAGED_RENDER_API void DummyFunction()
{
	//XRay::Log::Warning( gcnew System::String(SecondaryCombine_Invoker.Name));
	XRay::Log::Info("X-Ray Spectre Render Library was loaded");
}
