#include "stdafx.h"
#pragma hdrstop

#include "../xrRender/ResourceManager.h"
#include "../xrRender/dxRenderDeviceRender.h"

CRT::CRT			()
{
	pSurface		= NULL;
	pRT				= NULL;
	pZRT			= NULL;
#ifdef USE_DX11
	pUAView			= NULL;
#endif
	dwWidth			= 0;
	dwHeight		= 0;
	fmt				= DXGI_FORMAT_UNKNOWN;
}
CRT::~CRT			()
{
	destroy			();

	// release external reference
	DEV->_DeleteRT	(this);
}

#ifdef USE_DX11
void CRT::create	(LPCSTR Name, u32 w, u32 h, DXGI_FORMAT f, u32 SampleCount, bool useUAV )
#else
void CRT::create	(LPCSTR Name, u32 w, u32 h,	D3DFORMAT f, u32 SampleCount )
#endif
{
	if (pSurface)	return;

	R_ASSERT	(HW.pDevice && Name && Name[0] && w && h);
	_order		= CPU::GetCLK()	;

	dwWidth		= w;
	dwHeight	= h;
	fmt			= f;

	// Check width-and-height of render target surface
	if (w>D3D_REQ_TEXTURE2D_U_OR_V_DIMENSION)		return;
	if (h>D3D_REQ_TEXTURE2D_U_OR_V_DIMENSION)		return;

	// Select usage
	u32 usage	= 0;
	if (DXGI_FORMAT_R24G8_TYPELESS ==fmt)					usage = D3DUSAGE_DEPTHSTENCIL;
	else if (DXGI_FORMAT_D24_UNORM_S8_UINT ==fmt)			usage = D3DUSAGE_DEPTHSTENCIL;
	else if (DXGI_FORMAT_D16_UNORM ==fmt)					usage = D3DUSAGE_DEPTHSTENCIL;
	else if (DXGI_FORMAT_D32_FLOAT_S8X24_UINT ==fmt)		usage = D3DUSAGE_DEPTHSTENCIL;
	else if (DXGI_FORMAT_R16_TYPELESS ==fmt)				usage = D3DUSAGE_DEPTHSTENCIL;
	else if (DXGI_FORMAT_R32_TYPELESS ==fmt)				usage = D3DUSAGE_DEPTHSTENCIL;
	else if ((D3DFORMAT)MAKEFOURCC('D','F','2','4') == fmt)	usage = D3DUSAGE_DEPTHSTENCIL;
	else													usage = D3DUSAGE_RENDERTARGET;

	if (fmt == DXGI_FORMAT_D24_UNORM_S8_UINT)
	{
		fmt = DXGI_FORMAT_R24G8_TYPELESS;
		usage = D3DUSAGE_DEPTHSTENCIL;
	}

	bool	bUseAsDepth = (usage == D3DUSAGE_RENDERTARGET)?false:true;

	// Try to create texture/surface
	DEV->Evict				();

	D3D_TEXTURE2D_DESC desc;
    std::memset(&desc,0,sizeof(desc));
	desc.Width = dwWidth;
	desc.Height = dwHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = fmt;
	desc.SampleDesc.Count = SampleCount;
	desc.Usage = D3D_USAGE_DEFAULT;
   if( SampleCount <= 1 )
	   desc.BindFlags = D3D_BIND_SHADER_RESOURCE | (bUseAsDepth ? D3D_BIND_DEPTH_STENCIL : D3D_BIND_RENDER_TARGET);
   else
   {
      desc.BindFlags = (bUseAsDepth ? D3D_BIND_DEPTH_STENCIL : (D3D_BIND_SHADER_RESOURCE | D3D_BIND_RENDER_TARGET));
      if( RImplementation.o.dx10_msaa_opt )
      {
         desc.SampleDesc.Quality = UINT(D3D_STANDARD_MULTISAMPLE_PATTERN);
      }
   }

#ifdef USE_DX11
	if (HW.FeatureLevel>=D3D_FEATURE_LEVEL_11_0 && !bUseAsDepth && SampleCount == 1 && useUAV )
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
#endif

	CHK_DX( HW.pDevice->CreateTexture2D( &desc, NULL, &pSurface ) );
	HW.stats_manager.increment_stats_rtarget( pSurface );
	// OK
#ifdef DEBUG
	Msg			("* created RT(%s), %dx%d, format = %x samples = %d",Name,w,h, f, SampleCount );
#endif // DEBUG

	if (bUseAsDepth)
	{
		D3D_DEPTH_STENCIL_VIEW_DESC	ViewDesc;
        std::memset(&ViewDesc,0,sizeof(ViewDesc));

		ViewDesc.Format = DXGI_FORMAT_UNKNOWN;
		if( SampleCount <= 1 )
		{
			ViewDesc.ViewDimension = D3D_DSV_DIMENSION_TEXTURE2D;
		}
		else
		{
			ViewDesc.ViewDimension = D3D_DSV_DIMENSION_TEXTURE2DMS;
			ViewDesc.Texture2DMS.UnusedField_NothingToDefine = 0;
		}

		ViewDesc.Texture2D.MipSlice = 0;
		switch (desc.Format)
		{
		case DXGI_FORMAT_R24G8_TYPELESS:
			ViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
			ViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			break;
		}

		CHK_DX( HW.pDevice->CreateDepthStencilView( pSurface, &ViewDesc, &pZRT) );
	}
	else
		CHK_DX( HW.pDevice->CreateRenderTargetView( pSurface, 0, &pRT ) );

#ifdef USE_DX11
	if (HW.FeatureLevel>=D3D_FEATURE_LEVEL_11_0 && !bUseAsDepth &&  SampleCount == 1 && useUAV)
    {
	    D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
        std::memset( &UAVDesc, 0, sizeof( D3D11_UNORDERED_ACCESS_VIEW_DESC ) );
		UAVDesc.Format = fmt;
		UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		UAVDesc.Buffer.FirstElement = 0;
		UAVDesc.Buffer.NumElements = dwWidth * dwHeight;
		CHK_DX( HW.pDevice->CreateUnorderedAccessView( pSurface, &UAVDesc, &pUAView ) );
    }
#endif

	pTexture	= DEV->_CreateTexture	(Name);
	pTexture->surface_set(pSurface);
}

void CRT::destroy		()
{
	if (pTexture._get())	{
		pTexture->surface_set	(0);
		pTexture				= NULL;
	}
	_RELEASE	(pRT		);
	_RELEASE	(pZRT		);
	
	HW.stats_manager.decrement_stats_rtarget( pSurface );
	_RELEASE	(pSurface	);
#ifdef USE_DX11
	_RELEASE	(pUAView);
#endif
}
void CRT::reset_begin	()
{
	destroy		();
}
void CRT::reset_end		()
{
	create		(*cName,dwWidth,dwHeight,fmt);
}

#ifdef USE_DX11
void resptrcode_crt::create(LPCSTR Name, u32 w, u32 h, DXGI_FORMAT f, u32 SampleCount, bool useUAV )
{
	_set			(DEV->_CreateRT(Name,w,h,f, SampleCount, useUAV ));
}
#else
void resptrcode_crt::create(LPCSTR Name, u32 w, u32 h, D3DFORMAT f, u32 SampleCount)
{
	_set			(DEV->_CreateRT(Name,w,h,f, SampleCount));
}
#endif