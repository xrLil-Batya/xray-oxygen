#include "stdafx.h"

#include "../Include/xrRender/DrawUtils.h"
#include "render.h"
#include "../xrcdb/xrxrc.h"

extern XRCDB_API bool *cdb_bDebug;

void SetupGPU(IRenderDeviceRender *pRender)
{
	const BOOL bForceGPU_SW =		!!strstr(Core.Params, "-gpu_sw");
	const BOOL bForceGPU_NonPure =	!!strstr(Core.Params, "-gpu_nopure");
	const BOOL bForceGPU_REF =		!!strstr(Core.Params, "-gpu_ref");

	pRender->SetupGPU(bForceGPU_SW, bForceGPU_NonPure, bForceGPU_REF);
}

void CRenderDevice::_SetupStates()
{
	// General Render States
	mView.identity();
	mProject.identity();
	mFullTransform.identity();

	vCameraPosition.set		(0,0,0);
	vCameraDirection.set	(0,0,1);
	vCameraTop.set			(0,1,0);
	vCameraRight.set		(1,0,0);

	m_pRender->SetupStates();
}

void CRenderDevice::_Create(LPCSTR shName)
{
	Memory.mem_compact();

	// after creation
	b_is_Ready = TRUE;
	_SetupStates();

	m_pRender->OnDeviceCreate(shName);

	dwFrame = 0;
}

void CRenderDevice::ConnectToRender()
{
	if (!m_pRender) { m_pRender = RenderFactory->CreateRenderDeviceRender(); }
}

void CRenderDevice::Create(bool bIsEditor)
{
	if (b_is_Ready) { return; }		// prevent double call
	Statistic = xr_new<CStats>();

#ifdef	DEBUG
	cdb_clRAY = &Statistic->clRAY;				// total: ray-testing
	cdb_clBOX = &Statistic->clBOX;				// total: box query
	cdb_clFRUSTUM = &Statistic->clFRUSTUM;		// total: frustum query
	cdb_bDebug = &bDebug;
#endif

	if (!m_pRender) { m_pRender = RenderFactory->CreateRenderDeviceRender(); }
	SetupGPU(m_pRender);
	Log("Starting RENDER device...");

#ifdef _EDITOR
	psCurrentVidMode[0]	= dwWidth;
	psCurrentVidMode[1] = dwHeight;
#endif // #ifdef _EDITOR

	fFOV				= 90.f;
	fASPECT				= 1.f;

	m_pRender->Create(m_hWnd, dwWidth, dwHeight, fWidth_2, fHeight_2, !bIsEditor);

	if (!bIsEditor || editor())
		UpdateWindowPropStyle(GetCurrentWindowPropStyle());

	string_path			fname; 
	FS.update_path		(fname,"$game_data$","shaders.xr");

	//////////////////////////////////////////////////////////////////////////
	_Create				(fname);

	PreCache			(0, false, false);
}
