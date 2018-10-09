#include "stdafx.h"
#include "../../xrEngine/igame_persistent.h"
#include "../../xrEngine/environment.h"

void CRenderTarget::PhaseRainDrops()
{
	//#TODO: RZ to self: improve rain drops effect.
	// 1. Check if actor is _really_ under rain.
	// 2. Use wet/dry timers for proper effect accumulation.
	// 3. Add layered distortion effect to simulate water flowing, streaks, etc.

	float _w = float(Device.dwWidth);
	float _h = float(Device.dwHeight);

	// Consts
	CEnvDescriptorMixer& envdesc = *Environment().CurrentEnv;
	Fvector4 params = { envdesc.rain_density, droplets_power_debug, 0, 0 };
	xr_unordered_map<LPCSTR, Fvector4*> consts;
	consts.insert(std::make_pair("rain_drops_params0", &params));

	// Pass 0
#if defined(USE_DX10) || defined(USE_DX11)
	ref_rt outRT = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;

	RenderScreenQuad(_w, _h, rt_Generic_2, s_rain_drops->E[0], &consts);
	HW.pContext->CopyResource(outRT->pTexture->surface_get(), rt_Generic_2->pTexture->surface_get());
#else
	RenderScreenQuad(_w, _h, rt_Color, s_rain_drops->E[0], &consts);
#endif
}
