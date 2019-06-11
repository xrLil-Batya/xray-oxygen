#include "stdafx.h"
#include "../../xrEngine/IGame_Persistent.h"
#include "../../xrEngine/Environment.h"
#include "../../xrEngine/Rain.h"

u64 dwEntry2Game = 0ull;

//#TODO: RZ to self: improve rain drops effect.
// 1. Check if actor is _really_ under rain.
// 2. Use wet/dry timers for proper effect accumulation.
// 3. Add layered distortion effect to simulate water flowing, streaks, etc.

void CRenderTarget::PhaseRainDrops()
{
	if (Device.dwPrecacheFrame)
		dwEntry2Game = Device.dwFrame + 400;

	float wetness = Environment().eff_Rain->GetCurrViewEntityWetness();
	if (wetness < EPS_L || dwEntry2Game > Device.dwFrame)
	{
		// [dwEntry2Game] - For a smooth wet start in rainy weather
		return; // Skip
	}

	// Pass 0
	ref_rt outRT = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;

	RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_Generic_2, s_rain_drops->E[0]);
	HW.pContext->CopyResource(outRT->pTexture->surface_get(), rt_Generic_2->pTexture->surface_get());
}
