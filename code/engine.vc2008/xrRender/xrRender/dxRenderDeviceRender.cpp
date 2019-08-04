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

DXGI_GAMMA_CONTROL dxRenderDeviceRender::GetGammaLUT() const
{
	return m_Gamma.GetLUT();
}

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

	dwWidth					= HW.m_ChainDesc.BufferDesc.Width;
	dwHeight				= HW.m_ChainDesc.BufferDesc.Height;

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

	SSManager.SetMaxAnisotropy	(ps_r_tf_Anisotropic);
	SSManager.SetMipLODBias		(ps_r_tf_Mipbias);
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
	dwWidth					= HW.m_ChainDesc.BufferDesc.Width;
	dwHeight				= HW.m_ChainDesc.BufferDesc.Height;

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
	//	TODO: DX11: Implement overdrawBegin
	VERIFY(!"dxRenderDeviceRender::overdrawBegin not implemented.");
}

void dxRenderDeviceRender::overdrawEnd()
{
	//	TODO: DX11: Implement overdrawEnd
	VERIFY(!"dxRenderDeviceRender::overdrawEnd not implemented.");
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

	const HRESULT Result = HW.m_pSwapChain->Present(0, DXGI_PRESENT_TEST);

	switch (Result)
	{
		// Check desktop duplication interface is valid
		case DXGI_ERROR_DEVICE_HUNG:			return dsLost;

		// Check if the device is ready to be reset
		case DXGI_ERROR_DEVICE_RESET:			return dsNeedReset;

		// Where is video card?!
		case DXGI_ERROR_DEVICE_REMOVED:			return dsRemoved;
	}

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
	RCache.OnFrameBegin		();
	RCache.set_CullMode		(CULL_CW);
	RCache.set_CullMode		(CULL_CCW);
}

void dxRenderDeviceRender::Clear()
{
	HW.pContext->ClearDepthStencilView(RCache.get_ZB(), 
		D3D_CLEAR_DEPTH|D3D_CLEAR_STENCIL, 1.0f, 0);

	if (psDeviceFlags.test(rsClearBB))
	{
		FLOAT ColorRGBA[4] = {0.0f,0.0f,0.0f,0.0f};
		HW.pContext->ClearRenderTargetView(RCache.get_RT(), ColorRGBA);
	}
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

	RCache.OnFrameEnd	();

	if (!Device.m_SecondViewport.IsSVPFrame() && !Device.m_SecondViewport.m_bCamReady) //+SecondVP+ Не выводим кадр из второго вьюпорта на экран (на практике у нас экранная картинка обновляется минимум в два раза реже) [don't flush image into display for SecondVP-frame]
		HW.m_pSwapChain->Present(psDeviceFlags.test(rsVSync) ? 1 : 0, 0);
}

void dxRenderDeviceRender::ResourcesDestroyNecessaryTextures()
{
	Resources->DestroyNecessaryTextures();
}

void dxRenderDeviceRender::ClearTarget()
{
	float ColorRGBA[4] = {0.0f,0.0f,0.0f,0.0f};
	HW.pContext->ClearRenderTargetView(RCache.get_RT(), ColorRGBA);
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

