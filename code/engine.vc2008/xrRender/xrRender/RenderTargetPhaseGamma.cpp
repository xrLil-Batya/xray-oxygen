#include "stdafx.h"

#include "dxRenderDeviceRender.h"

void CRenderTarget::PhaseGammaGenerateLUT()
{
	u32 Offset = 0;

	Fvector2 p0, p1;
#ifdef USE_DX11
	p0.set(0.0f, 0.0f);
	p1.set(1.0f, 1.0f);
#else
	float _w = float(Device.dwWidth);
	float _h = float(Device.dwHeight);
	p0.set(0.5f/_w, 0.5f/_h);
	p1.set((_w+0.5f)/_w, (_h+0.5f)/_h);
#endif

	/////// Create gamma LUT //////////////////
	u_setrt				(rt_GammaLUT, nullptr, nullptr, nullptr);
	RCache.set_Stencil	(FALSE);

	// Constants
	float brightness		= dxRenderDeviceRender::Instance().GetBrightness();
	float gamma				= dxRenderDeviceRender::Instance().GetGamma();
	float contrast			= dxRenderDeviceRender::Instance().GetContrast();
	Fvector color_grading	= dxRenderDeviceRender::Instance().GetBalance();

	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0.0f,	1.0f, 0.0f, 1.0f, 0, p0.x, p1.y); pv++;
	pv->set(0.0f,	0.0f, 0.0f, 1.0f, 0, p0.x, p0.y); pv++;
	pv->set(256.0f,	1.0f, 0.0f, 1.0f, 0, p1.x, p1.y); pv++;
	pv->set(256.0f,	0.0f, 0.0f, 1.0f, 0, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	// Draw COLOR
	RCache.set_Element	(s_gamma->E[0]);
	RCache.set_c		("color_params", brightness, gamma, contrast, 0.0f);
	RCache.set_c		("color_grading", color_grading.x, color_grading.y, color_grading.z, 0.0f);
	RCache.set_Geometry	(g_combine);
	RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::PhaseGammaApply()
{
	// Copy back buffer content to the rt_Generic_0
#ifdef USE_DX11
	ID3DResource* pTmpTexture = rt_Generic_0->pTexture->surface_get();
	HW.pBaseRT->GetResource(&pTmpTexture);
	HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), pTmpTexture);
	_RELEASE(pTmpTexture);
#else
	CHK_DX(HW.pDevice->StretchRect(HW.pBaseRT, nullptr, rt_Generic_0->pRT, nullptr, D3DTEXF_NONE));
	// Is this will be faster? Need another rt matching back buffer format though...
	//CHK_DX(HW.pDevice->GetRenderTargetData(HW.pBaseRT, rt_Generic_0->pRT));
#endif

	RCache.set_Z(FALSE);
	RenderScreenQuad(Device.dwWidth, Device.dwHeight, HW.pBaseRT, s_gamma->E[1]);
}

#ifdef DEBUG
void CRenderTarget::SaveGammaLUT()
{
	ID3DBlob* saved = nullptr;

#ifdef USE_DX11
	ID3DResource* pSrcTexture;
	rt_GammaLUT->pRT->GetResource(&pSrcTexture);
	VERIFY(pSrcTexture);

	CHK_DX(D3DX11SaveTextureToMemory(HW.pContext, pSrcTexture, D3DX11_IFF_PNG, &saved, 0));
	_RELEASE(pSrcTexture);
#else
	IDirect3DSurface9* pFB;
	D3DLOCKED_RECT D;

	u32 w = rt_GammaLUT->dwWidth;
	u32 h = rt_GammaLUT->dwHeight;

	CHK_DX(HW.pDevice->CreateOffscreenPlainSurface(w, h, rt_GammaLUT->fmt, D3DPOOL_SYSTEMMEM, &pFB, nullptr));
	CHK_DX(HW.pDevice->GetRenderTargetData(rt_GammaLUT->pRT, pFB));

	// Image processing (kill alpha)
	CHK_DX(pFB->LockRect(&D, nullptr, 0));
	{
		u32* pPixel = (u32*)D.pBits;
		u32* pEnd = pPixel + (w*h);

		while (pPixel != pEnd)
		{
			u32 p = *pPixel;
			*pPixel = color_xrgb(color_get_R(p), color_get_G(p), color_get_B(p));
			++pPixel;
		}
	}
	CHK_DX(pFB->UnlockRect());

	CHK_DX(D3DXSaveSurfaceToFileInMemory(&saved, D3DXIFF_PNG, pFB, nullptr, nullptr));
	_RELEASE(pFB);
#endif

	IWriter* fs = FS.w_open("$screenshots$", "GammaLUT.png");
	R_ASSERT(fs);
	fs->w(saved->GetBufferPointer(), saved->GetBufferSize());
	FS.w_close(fs);
	_RELEASE(saved);
}
#endif
