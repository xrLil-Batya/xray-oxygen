#include "stdafx.h"

void	CRenderTarget::phase_accumulator()
{
	// Targets
	if (dwAccumulatorClearMark==Device.dwFrame)	{
		// normal operation - setup
		if (RImplementation.o.fp16_blend)	u_setrt	(rt_Accumulator,		NULL,NULL,HW.pBaseZB);
		else								u_setrt	(rt_Accumulator_temp,	NULL,NULL,HW.pBaseZB);
	} else {
		// initial setup
		dwAccumulatorClearMark				= Device.dwFrame;

		// clear
		u_setrt								(rt_Accumulator,		NULL,NULL,HW.pBaseZB);
		reset_light_marker();
		u32		clr4clear					= color_rgba(0,0,0,0);	// 0x00
		RCache.Clear						(0L, nullptr, D3DCLEAR_TARGET, clr4clear, 1.0f, 0L);

		// Stencil	- draw only where stencil >= 0x1
		RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);
		RCache.set_CullMode					(CULL_NONE);
		RCache.set_ColorWriteEnable			();
	}
}

void	CRenderTarget::phase_vol_accumulator()
{
	if (!m_bHasActiveVolumetric)
	{
		m_bHasActiveVolumetric = true;

		u_setrt(rt_Volumetric, nullptr, nullptr, HW.pBaseZB);
		u32 clr4clearVol = color_rgba(0, 0, 0, 0);	// 0x00
		RCache.Clear(0L, nullptr, D3DCLEAR_TARGET, clr4clearVol, 1.0f, 0L);
	}
	else
		u_setrt(rt_Volumetric, nullptr, nullptr, HW.pBaseZB);

	RCache.set_Stencil							(FALSE);
	RCache.set_CullMode							(CULL_NONE);
	RCache.set_ColorWriteEnable					();
}
