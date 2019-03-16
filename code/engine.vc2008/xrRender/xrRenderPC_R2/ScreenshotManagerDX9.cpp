#include "stdafx.h"
#include "../xrRender/ScreenshotManager.h"
#include "../xrRender/dxRenderDeviceRender.h"
#include "../../xrEngine/xrImage_Resampler.h"

void CScreenshotManager::ProcessImage(u32* pData, u32 size, bool bGammaCorrection)
{
	u32* pPixel = pData;
	u32* pEnd	= pPixel + size;

	if (bGammaCorrection)
	{
		D3DGAMMARAMP G = dxRenderDeviceRender::Instance().GetGammaLUT();
		for (int i = 0; i < 256; ++i)
		{
			G.red[i] /= 256;
			G.green[i] /= 256;
			G.blue[i] /= 256;
		}

		// Apply gamma correction and kill aplha
		for (; pPixel != pEnd; ++pPixel)
		{
			u32 p = *pPixel;
			*pPixel = color_xrgb(
				G.red[color_get_R(p)],
				G.green[color_get_G(p)],
				G.blue[color_get_B(p)]
			);
		}
	}
	else
	{
		// Just kill alpha
		for (; pPixel != pEnd; ++pPixel)
		{
			u32 p = *pPixel;
			*pPixel = color_xrgb(
				color_get_R(p),
				color_get_G(p),
				color_get_B(p)
			);
		}
	}
}

ID3DBlob* CScreenshotManager::MakeScreenshotNormal(u32 fmt)
{
	D3DLOCKED_RECT R;
	IDirect3DSurface9* pFB = nullptr;
	ID3DBlob* pData = nullptr;

	// Copy back buffer to temporary surface
	CHK_DX(HW.pDevice->CreateOffscreenPlainSurface(Device.dwWidth, Device.dwHeight, HW.DevPP.BackBufferFormat, D3DPOOL_SYSTEMMEM, &pFB, nullptr));
	CHK_DX(HW.pDevice->GetRenderTargetData(HW.pBaseRT, pFB));

	// Process image: kill alpha channel and optionally apply gamma settings
	CHK_DX(pFB->LockRect(&R, nullptr, D3DLOCK_NOSYSLOCK));
	bool bGammaCorrection = psDeviceFlags.test(rsFullscreen) ? ps_r_postscreen_flags.test(R_FLAG_SS_GAMMA_CORRECTION) : false;
	ProcessImage((u32*)R.pBits, Device.dwWidth*Device.dwHeight, bGammaCorrection);
	CHK_DX(pFB->UnlockRect());

	// Save surface to buffer
	CHK_DX(D3DXSaveSurfaceToFileInMemory(&pData, (D3DXIMAGE_FILEFORMAT)fmt, pFB, nullptr, nullptr));
	_RELEASE(pFB);

	return pData;
}

ID3DBlob* CScreenshotManager::MakeScreenshotForSavedGame()
{
	D3DLOCKED_RECT R;
	IDirect3DSurface9* pFB = nullptr;
	IDirect3DSurface9* pSurface	= nullptr;
	IDirect3DTexture9* pTexture	= nullptr;
	ID3DBlob* pData = nullptr;

	// Copy back buffer to temporary surface
	CHK_DX(HW.pDevice->CreateOffscreenPlainSurface(Device.dwWidth, Device.dwHeight, HW.DevPP.BackBufferFormat, D3DPOOL_SYSTEMMEM, &pFB, nullptr));
	CHK_DX(HW.pDevice->GetRenderTargetData(HW.pBaseRT, pFB));

	// Process image: kill alpha channel
	CHK_DX(pFB->LockRect(&R, nullptr, D3DLOCK_NOSYSLOCK));
	ProcessImage((u32*)R.pBits, Device.dwWidth*Device.dwHeight, false);
	CHK_DX(pFB->UnlockRect());

	// Create DXT1 texture
	CHK_DX(D3DXCreateTexture(HW.pDevice, GAMESAVE_IMAGE_SIZE, GAMESAVE_IMAGE_SIZE, 1, 0, D3DFMT_DXT1, D3DPOOL_SCRATCH, &pTexture));

	// Copy image to texture (and resize)
	CHK_DX(pTexture->GetSurfaceLevel(0, &pSurface));
	CHK_DX(D3DXLoadSurfaceFromSurface(pSurface, nullptr, nullptr, pFB, nullptr, nullptr, D3DX_DEFAULT, 0));
	_RELEASE(pSurface);
	_RELEASE(pFB);

	// Save texture to buffer
	CHK_DX(D3DXSaveTextureToFileInMemory(&pData, D3DXIFF_DDS, pTexture, nullptr));
	_RELEASE(pTexture);

	return pData;
}

//#TODO: Level map && cube screenshots don't work. xrImage_Resampler is broken?

ID3DBlob* CScreenshotManager::MakeScreenshotForCubeMap(u32 fmt)
{
	D3DLOCKED_RECT R;
	IDirect3DSurface9* pFB = nullptr;
	ID3DBlob* pData = nullptr;

	// Copy back buffer to temporary surface
	CHK_DX(HW.pDevice->CreateOffscreenPlainSurface(Device.dwWidth, Device.dwHeight, HW.DevPP.BackBufferFormat, D3DPOOL_SYSTEMMEM, &pFB, nullptr));
	CHK_DX(HW.pDevice->GetRenderTargetData(HW.pBaseRT, pFB));

	// Resample image
	CHK_DX(pFB->LockRect(&R, nullptr, D3DLOCK_NOSYSLOCK));
	u32* data = (u32*)xr_malloc(Device.dwHeight * Device.dwHeight * 4);
	imf_Process(data, Device.dwHeight, Device.dwHeight, (u32*)R.pBits, Device.dwWidth, Device.dwHeight, imf_lanczos3);
	CHK_DX(pFB->UnlockRect());
	_RELEASE(pFB);

	// Create surface to hold resampled image
	CHK_DX(HW.pDevice->CreateOffscreenPlainSurface(Device.dwHeight, Device.dwHeight, HW.DevPP.BackBufferFormat, D3DPOOL_SYSTEMMEM, &pFB, nullptr));
	CHK_DX(pFB->LockRect(&R, nullptr, D3DLOCK_NOSYSLOCK));
	R.pBits = data;
	CHK_DX(pFB->UnlockRect());

	// Save surface to buffer
	CHK_DX(D3DXSaveSurfaceToFileInMemory(&pData, D3DXIFF_BMP, pFB, nullptr, nullptr));
	_RELEASE(pFB);

	return pData;
}

ID3DBlob* CScreenshotManager::MakeScreenshotForLevelMap(u32 fmt)
{
	D3DLOCKED_RECT R;
	IDirect3DSurface9* pFB = nullptr;
	ID3DBlob* pData = nullptr;

	// Copy back buffer to temporary surface
	CHK_DX(HW.pDevice->CreateOffscreenPlainSurface(Device.dwWidth, Device.dwHeight, HW.DevPP.BackBufferFormat, D3DPOOL_SYSTEMMEM, &pFB, nullptr));
	CHK_DX(HW.pDevice->GetRenderTargetData(HW.pBaseRT, pFB));

	// Resample image
	CHK_DX(pFB->LockRect(&R, nullptr, D3DLOCK_NOSYSLOCK));
	u32* data = (u32*)xr_malloc(Device.dwHeight * Device.dwHeight * 4);
	imf_Process(data, Device.dwHeight, Device.dwHeight, (u32*)R.pBits, Device.dwWidth, Device.dwHeight, imf_lanczos3);
	CHK_DX(pFB->UnlockRect());
	_RELEASE(pFB);

	// Create surface to hold resampled image
	CHK_DX(HW.pDevice->CreateOffscreenPlainSurface(Device.dwHeight, Device.dwHeight, HW.DevPP.BackBufferFormat, D3DPOOL_SYSTEMMEM, &pFB, nullptr));
	CHK_DX(pFB->LockRect(&R, nullptr, D3DLOCK_NOSYSLOCK));
	R.pBits = data;
	CHK_DX(pFB->UnlockRect());

	// Save surface to buffer
	CHK_DX(D3DXSaveSurfaceToFileInMemory(&pData, D3DXIFF_BMP, pFB, nullptr, nullptr));
	_RELEASE(pFB);

	return pData;
}
