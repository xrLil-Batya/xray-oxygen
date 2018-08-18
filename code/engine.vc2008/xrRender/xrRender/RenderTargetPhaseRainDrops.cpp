#include "stdafx.h"
#include "../../xrEngine/igame_persistent.h"
#include "../../xrEngine/environment.h"

void CRenderTarget::PhaseRainDrops()
{
#pragma todo("RZ: check if actor is _really_ under rain")

	float _w = float(Device.dwWidth);
	float _h = float(Device.dwHeight);

	// Consts
	CEnvDescriptorMixer& envdesc = *g_pGamePersistent->Environment().CurrentEnv;
	Fvector4 params = { envdesc.rain_density, droplets_power_debug, 0, 0 };
	xr_unordered_map<LPCSTR, Fvector4*> consts;
	consts.insert(std::make_pair("rain_drops_params0", &params));

	// Pass 0
#if defined(USE_DX10) || defined(USE_DX11)
	RenderScreenQuad(_w, _h, rt_Generic, s_rain_drops->E[0], &consts);
	HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), rt_Generic->pTexture->surface_get());
#else
	RenderScreenQuad(_w, _h, rt_Generic_0, s_rain_drops->E[0], &consts);
#endif
}
