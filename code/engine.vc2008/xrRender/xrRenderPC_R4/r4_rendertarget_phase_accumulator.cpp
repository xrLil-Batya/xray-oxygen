#include "stdafx.h"

void	CRenderTarget::phase_accumulator()
{
	// Targets
	if (dwAccumulatorClearMark==Device.dwFrame)	
	{
		// normal operation - setup
		RImplementation.o.dx10_msaa ? u_setrt(rt_Accumulator, NULL, NULL, rt_MSAADepth->pZRT)
									: u_setrt(rt_Accumulator, NULL, NULL, HW.pBaseZB);
	} 
	else 
	{
		// initial setup
		dwAccumulatorClearMark				= Device.dwFrame;

		// clear
		RImplementation.o.dx10_msaa ? u_setrt(rt_Accumulator, NULL, NULL, rt_MSAADepth->pZRT)
									: u_setrt(rt_Accumulator, NULL, NULL, HW.pBaseZB);

		reset_light_marker();
		
		FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		HW.pContext->ClearRenderTargetView( rt_Accumulator->pRT, ColorRGBA);

		// Stencil	- draw only where stencil >= 0x1
		RCache.set_Stencil					(TRUE,D3D11_COMPARISON_LESS_EQUAL,0x01,0xff,0x00);
		RCache.set_CullMode					(CULL_NONE);
		RCache.set_ColorWriteEnable			();
		
	}

	//	Restore viewport after shadow map rendering
	RImplementation.rmNormal();
}

void	CRenderTarget::phase_vol_accumulator()
{
	RImplementation.o.dx10_msaa ? u_setrt(rt_Volumetric, NULL, NULL, RImplementation.Target->rt_MSAADepth->pZRT)
								: u_setrt(rt_Volumetric, NULL, NULL, HW.pBaseZB);

	if (!m_bHasActiveVolumetric)
	{
		m_bHasActiveVolumetric = true;

		FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		HW.pContext->ClearRenderTargetView(rt_Volumetric->pRT, ColorRGBA);
	}

	RCache.set_Stencil							(FALSE);
	RCache.set_CullMode							(CULL_NONE);
	RCache.set_ColorWriteEnable					();
}