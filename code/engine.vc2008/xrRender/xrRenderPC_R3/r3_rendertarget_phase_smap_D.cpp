#include "stdafx.h"

void	CRenderTarget::phase_smap_direct		(light* L, u32 sub_phase)
{
	//	TODO: DX10: Check thst we will never need old SMap implementation
	// Targets
	if (RImplementation.o.HW_smap)		u_setrt	(rt_smap_surf, NULL, NULL, rt_smap_depth->pZRT);
	else								VERIFY(!"Use HW SMap only for DX10!");

	HW.pDevice->ClearDepthStencilView(rt_smap_depth->pZRT, D3D10_CLEAR_DEPTH, 1.0f, 0L);

	//	Prepare viewport for shadow map rendering
	if (sub_phase!=SE_SUN_RAIN_SMAP	)
		RImplementation.rmNormal();
	else
	{
		D3D_VIEWPORT VP					=	{L->X.D.minX,L->X.D.minY,
			(L->X.D.maxX - L->X.D.minX) , 
			(L->X.D.maxY - L->X.D.minY) , 
			0,1 };
		//CHK_DX								(HW.pDevice->SetViewport(&VP));
		HW.pDevice->RSSetViewports(1, &VP);
	}

	// Stencil	- disable
	RCache.set_Stencil					( FALSE );
}

void	CRenderTarget::phase_smap_direct_tsh	(light* L, u32 sub_phase)
{
	VERIFY								(RImplementation.o.Tshadows);
	FLOAT ColorRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
	RCache.set_ColorWriteEnable			();

	//	Prepare viewport for shadow map rendering
	RImplementation.rmNormal();
	HW.pDevice->ClearRenderTargetView( RCache.get_RT(0), ColorRGBA);
}
