#include "stdafx.h"
#include "dxRenderDeviceRender.h"

#include "ResourceManager.h"

dxRenderDeviceRender::dxRenderDeviceRender() : Resources(0)
{
	;
}

void dxRenderDeviceRender::Copy(IRenderDeviceRender &_in)
{
	*this = *(dxRenderDeviceRender*)&_in;
}

#ifdef USE_DX11
DXGI_GAMMA_CONTROL dxRenderDeviceRender::GetGammaLUT() const
{
	return m_Gamma.GetLUT();
}
#else
D3DGAMMARAMP dxRenderDeviceRender::GetGammaLUT() const
{
	return m_Gamma.GetLUT();
}
#endif

float dxRenderDeviceRender::GetGamma() const
{
	return m_Gamma.GetGamma();
}

float dxRenderDeviceRender::GetBrightness() const
{
	return m_Gamma.GetBrightness();
}

float dxRenderDeviceRender::GetContrast() const
{
	return m_Gamma.GetContrast();
}

Fvector dxRenderDeviceRender::GetBalance() const
{
	return m_Gamma.GetBalance();
}

void dxRenderDeviceRender::SetGamma(float val)
{
	m_Gamma.SetGamma(val);
}

void dxRenderDeviceRender::SetBrightness(float val)
{
	m_Gamma.SetBrightness(val);
}

void dxRenderDeviceRender::SetContrast(float val)
{
	m_Gamma.SetContrast(val);
}

void dxRenderDeviceRender::SetBalance(float r, float g, float b)
{
	m_Gamma.SetBalance(r, g, b);
}

void dxRenderDeviceRender::SetBalance(Fvector &C)
{
	m_Gamma.SetBalance(C);
}

void dxRenderDeviceRender::UpdateGamma()
{
	extern bool bNeedUpdateGammaLUT;
	m_Gamma.Update();
	bNeedUpdateGammaLUT = true;
}

void dxRenderDeviceRender::OnDeviceDestroy( BOOL bKeepTextures)
{
	m_WireShader.destroy();
	m_SelectionShader.destroy();

	Resources->OnDeviceDestroy( bKeepTextures);
	RCache.OnDeviceDestroy();
}

void dxRenderDeviceRender::ValidateHW()
{
	HW.Validate();
}

void dxRenderDeviceRender::DestroyHW()
{
	xr_delete					(Resources);
	HW.DestroyDevice			();
}

void  dxRenderDeviceRender::Reset( HWND hWnd, u32 &dwWidth, u32 &dwHeight, float &fWidth_2, float &fHeight_2)
{
#ifdef DEBUG
	_SHOW_REF("*ref -CRenderDevice::ResetTotal: DeviceREF:",HW.pDevice);
#endif // DEBUG	
	if (Resources)
	{
		Resources->reset_begin();
		Memory.mem_compact();
		HW.Reset(hWnd);
	}

#ifdef USE_DX11
	dwWidth					= HW.m_ChainDesc.BufferDesc.Width;
	dwHeight				= HW.m_ChainDesc.BufferDesc.Height;
#else
	dwWidth					= HW.DevPP.BackBufferWidth;
	dwHeight				= HW.DevPP.BackBufferHeight;
#endif

	fWidth_2				= float(dwWidth/2);
	fHeight_2				= float(dwHeight/2);

	if (Resources)
	{
		Resources->reset_end();
	}

#ifdef DEBUG
	_SHOW_REF("*ref +CRenderDevice::ResetTotal: DeviceREF:",HW.pDevice);
#endif // DEBUG
}

void dxRenderDeviceRender::SetupStates()
{
	HW.Caps.Update();

#ifdef USE_DX11
	SSManager.SetMaxAnisotropy	(ps_r_tf_Anisotropic);
	SSManager.SetMipLODBias		(ps_r_tf_Mipbias);
#else

	for (u32 i = 0; i < HW.Caps.raster.dwStages; i++)
	{
		CHK_DX(HW.pDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, ps_r_tf_Anisotropic));
		CHK_DX(HW.pDevice->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&ps_r_tf_Mipbias))));
		CHK_DX(HW.pDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
		CHK_DX(HW.pDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
		CHK_DX(HW.pDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));
	}
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_DITHERENABLE,		TRUE				));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_COLORVERTEX,		TRUE				));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_ZENABLE,			TRUE				));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_SHADEMODE,			D3DSHADE_GOURAUD	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_CULLMODE,			D3D11_CULL_BACK			));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_ALPHAFUNC,			D3D11_COMPARISON_GREATER		));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_LOCALVIEWER,		TRUE				));

	CHK_DX(HW.pDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS,	FALSE			));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_NORMALIZENORMALS,		TRUE			));

	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_SOLID		));

	// ******************** Fog parameters
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGCOLOR,			0					));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_RANGEFOGENABLE,	FALSE				));
	if (HW.Caps.bTableFog)	{
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGTABLEMODE,	D3DFOG_LINEAR		));
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGVERTEXMODE,	D3DFOG_NONE			));
	} else {
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGTABLEMODE,	D3DFOG_NONE			));
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR		));
	}

#endif
}

void dxRenderDeviceRender::OnDeviceCreate(LPCSTR shName)
{
	// Signal everyone - device created
	RCache.OnDeviceCreate		();
	m_Gamma.Update				();
	Resources->OnDeviceCreate	(shName);
	::Render->create			();
	Device.Statistic->OnDeviceCreate	();

    m_WireShader.create("editor\\wire");
    m_SelectionShader.create("editor\\selection");

    DUImpl.OnDeviceCreate();
}

void dxRenderDeviceRender::Create( HWND hWnd, u32 &dwWidth, u32 &dwHeight, float &fWidth_2, float &fHeight_2, bool move_window)
{
	HW.CreateDevice		(hWnd, move_window);
#ifdef USE_DX11
	dwWidth					= HW.m_ChainDesc.BufferDesc.Width;
	dwHeight				= HW.m_ChainDesc.BufferDesc.Height;
#else
	dwWidth					= HW.DevPP.BackBufferWidth;
	dwHeight				= HW.DevPP.BackBufferHeight;
#endif
	fWidth_2			= float(dwWidth/2)			;
	fHeight_2			= float(dwHeight/2)			;
	Resources			= xr_new<CResourceManager>		();
}

void dxRenderDeviceRender::SetupGPU(BOOL bForceGPU_SW, BOOL bForceGPU_NonPure, BOOL bForceGPU_REF)
{
	HW.Caps.bForceGPU_SW = bForceGPU_SW;
	HW.Caps.bForceGPU_NonPure = bForceGPU_NonPure;
	HW.Caps.bForceGPU_REF = bForceGPU_REF;
}

void dxRenderDeviceRender::overdrawBegin()
{
#ifdef USE_DX11
	//	TODO: DX11: Implement overdrawBegin
	VERIFY(!"dxRenderDeviceRender::overdrawBegin not implemented.");
#else
	// Turn stenciling
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILENABLE,		TRUE			));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILFUNC,		D3D11_COMPARISON_ALWAYS	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILREF,		0				));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILMASK,		0x00000000		));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILWRITEMASK,	0xffffffff		));

	// Increment the stencil buffer for each pixel drawn
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILFAIL,		D3D11_STENCIL_OP_KEEP		));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILPASS,		D3D11_STENCIL_OP_INCR_SAT	));

	if (1==HW.Caps.SceneMode)		
	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILZFAIL,	D3D11_STENCIL_OP_KEEP		)); }	// Overdraw
	else 
	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILZFAIL,	D3D11_STENCIL_OP_INCR_SAT	)); }	// ZB access
#endif
}

void dxRenderDeviceRender::overdrawEnd()
{
#ifdef USE_DX11
	//	TODO: DX11: Implement overdrawEnd
	VERIFY(!"dxRenderDeviceRender::overdrawEnd not implemented.");
#else
	// Set up the stencil states
	CHK_DX	(HW.pDevice->SetRenderState( D3DRS_STENCILZFAIL,		D3D11_STENCIL_OP_KEEP	));
	CHK_DX	(HW.pDevice->SetRenderState( D3DRS_STENCILFAIL,		D3D11_STENCIL_OP_KEEP	));
	CHK_DX	(HW.pDevice->SetRenderState( D3DRS_STENCILPASS,		D3D11_STENCIL_OP_KEEP	));
	CHK_DX	(HW.pDevice->SetRenderState( D3DRS_STENCILFUNC,		D3D11_COMPARISON_EQUAL		));
	CHK_DX	(HW.pDevice->SetRenderState( D3DRS_STENCILMASK,		0xff				));

	// Set the background to black
	RCache.Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 0, 0), 1.0f, 0);

	// Draw a rectangle wherever the count equal I
	RCache.OnFrameEnd	();
	CHK_DX	(HW.pDevice->SetFVF( FVF::F_TL ));

	// Render gradients
	for (int I=0; I<12; I++ ) 
	{
		u32	_c	= I*256/13;
		u32	c	= D3DCOLOR_XRGB(_c,_c,_c);

		FVF::TL	pv[4];
		pv[0].set(float(0),			float(Device.dwHeight),	c,0,0);			
		pv[1].set(float(0),			float(0),			c,0,0);					
		pv[2].set(float(Device.dwWidth),	float(Device.dwHeight),	c,0,0);	
		pv[3].set(float(Device.dwWidth),	float(0),			c,0,0);

		CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,		I	));
		CHK_DX(HW.pDevice->DrawPrimitiveUP	( D3DPT_TRIANGLESTRIP,	2,	pv, sizeof(FVF::TL) ));
	}
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILENABLE,		FALSE ));
#endif
}

void dxRenderDeviceRender::DeferredLoad(BOOL E)
{
	Resources->DeferredLoad(E);
}

void dxRenderDeviceRender::ResourcesDeferredUpload()
{
	Resources->DeferredUpload();
}

void dxRenderDeviceRender::ResourcesStoreNecessaryTextures()
{
	dxRenderDeviceRender::Instance().Resources->StoreNecessaryTextures();
}

dxRenderDeviceRender::DeviceState dxRenderDeviceRender::GetDeviceState()
{
	HW.Validate();

#ifndef USE_DX11
	HRESULT	_hr = HW.pDevice->TestCooperativeLevel();

	if (FAILED(_hr))
	{
		// If the device was lost, do not render until we get it back
		if (D3DERR_DEVICELOST == _hr)
			return dsLost;

		// Check if the device is ready to be reset
		if (D3DERR_DEVICENOTRESET == _hr)
			return dsNeedReset;
	}
#endif

	return dsOK;
}

BOOL dxRenderDeviceRender::GetForceGPU_REF()
{
	return HW.Caps.bForceGPU_REF;
}

u32 dxRenderDeviceRender::GetCacheStatPolys()
{
	return RCache.stat.polys;
}

void dxRenderDeviceRender::Begin()
{
#ifndef USE_DX11
	CHK_DX					(HW.pDevice->BeginScene());
#endif
	RCache.OnFrameBegin		();
	RCache.set_CullMode		(CULL_CW);
	RCache.set_CullMode		(CULL_CCW);

#ifndef USE_DX11
	if (HW.Caps.SceneMode)	
		overdrawBegin	();
#endif
}

void dxRenderDeviceRender::Clear()
{
#ifdef USE_DX11
	HW.pContext->ClearDepthStencilView(RCache.get_ZB(), 
		D3D_CLEAR_DEPTH|D3D_CLEAR_STENCIL, 1.0f, 0);

	if (psDeviceFlags.test(rsClearBB))
	{
		FLOAT ColorRGBA[4] = {0.0f,0.0f,0.0f,0.0f};
		HW.pContext->ClearRenderTargetView(RCache.get_RT(), ColorRGBA);
	}
#else
	u32 flags = D3DCLEAR_ZBUFFER;
	if (psDeviceFlags.is(rsClearBB))	flags |= D3DCLEAR_TARGET;
	if (HW.Caps.bStencil)				flags |= D3DCLEAR_STENCIL;

	RCache.Clear(0, nullptr, flags, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
#endif
}

bool bNeedUpdateGammaLUT = true;

void dxRenderDeviceRender::End()
{
	VERIFY	(HW.pDevice);
	ScopeStatTimer endTimer(Device.Statistic->Render_End);

	if (!psDeviceFlags.is(rsFullscreen))
	{
		// Generate gamma LUT if needed
		if (bNeedUpdateGammaLUT)
		{
			PIX_EVENT(GAMMA_GENERATE_LUT);
			RImplementation.Target->PhaseGammaGenerateLUT();
			bNeedUpdateGammaLUT = false;
		}
		PIX_EVENT(GAMMA_APPLY);
		RImplementation.Target->PhaseGammaApply();
	}

#ifndef USE_DX11
	if (HW.Caps.SceneMode)	
		overdrawEnd();
#endif

	RCache.OnFrameEnd	();

#ifdef USE_DX11
	if (!Device.m_SecondViewport.IsSVPFrame() && !Device.m_SecondViewport.m_bCamReady) //+SecondVP+ Не выводим кадр из второго вьюпорта на экран (на практике у нас экранная картинка обновляется минимум в два раза реже) [don't flush image into display for SecondVP-frame]
		HW.m_pSwapChain->Present(psDeviceFlags.test(rsVSync) ? 1 : 0, 0);
#else
	CHK_DX(HW.pDevice->EndScene());
	if (!Device.m_SecondViewport.IsSVPFrame() && !Device.m_SecondViewport.m_bCamReady) //+SecondVP+ Не выводим кадр из второго вьюпорта на экран (на практике у нас экранная картинка обновляется минимум в два раза реже) [don't flush image into display for SecondVP-frame]
		HW.pDevice->Present(NULL, NULL, NULL, NULL);
#endif
}

void dxRenderDeviceRender::ResourcesDestroyNecessaryTextures()
{
	Resources->DestroyNecessaryTextures();
}

void dxRenderDeviceRender::ClearTarget()
{
#ifdef USE_DX11
	FLOAT ColorRGBA[4] = {0.0f,0.0f,0.0f,0.0f};
	HW.pContext->ClearRenderTargetView(RCache.get_RT(), ColorRGBA);
#else
	RCache.Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
#endif
}

void dxRenderDeviceRender::SetCacheXform(const Fmatrix &mView, const Fmatrix &mProject)
{
	RCache.set_xform_view(mView);
	RCache.set_xform_project(mProject);
}

bool dxRenderDeviceRender::HWSupportsShaderYUV2RGB()
{
	u32		v_dev	= CAP_VERSION(HW.Caps.raster_major, HW.Caps.raster_minor);
	u32		v_need	= CAP_VERSION(2,0);
	return (v_dev>=v_need);
}

void  dxRenderDeviceRender::OnAssetsChanged()
{
    Resources->m_textures_description.UnLoad();
    Resources->m_textures_description.Load();
}

void dxRenderDeviceRender::ResizeWindowProc(WORD h, WORD w)
{
	HW.ResizeWindowProc(h, w);
}
