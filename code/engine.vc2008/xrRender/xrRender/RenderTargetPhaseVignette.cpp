#include "stdafx.h"

void CRenderTarget::PhaseVignette()
{
	// Pass 0
#ifdef USE_DX11
	ref_rt outRT = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;

	RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_Generic_2, s_vignette->E[0]);
	HW.pContext->CopyResource(outRT->pTexture->surface_get(), rt_Generic_2->pTexture->surface_get());
#else
	RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_Color, s_vignette->E[0]);
#endif
}
