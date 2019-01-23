#include "stdafx.h"
#include "../../xrEngine/IGame_Persistent.h"
#include "../../xrEngine/Environment.h"
#include "../../xrEngine/Rain.h"

void CRenderTarget::PhaseRainDrops()
{
	float wetness = Environment().eff_Rain->GetCurrViewEntityWetness();
	if (wetness < EPS_L)
		return;

	//#TODO: RZ to self: improve rain drops effect.
	// 1. Check if actor is _really_ under rain.
	// 2. Use wet/dry timers for proper effect accumulation.
	// 3. Add layered distortion effect to simulate water flowing, streaks, etc.

	// Pass 0
#ifdef USE_DX11
	ref_rt outRT = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;

	RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_Generic_2, s_rain_drops->E[0]);
	HW.pContext->CopyResource(outRT->pTexture->surface_get(), rt_Generic_2->pTexture->surface_get());
#else
	RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_Color, s_rain_drops->E[0]);
#endif
}
