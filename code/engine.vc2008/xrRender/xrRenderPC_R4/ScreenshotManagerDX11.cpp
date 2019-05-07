#include "stdafx.h"
#include "../xrRender/dxRenderDeviceRender.h"
#include "../xrRender/ScreenshotManager.h"

void CScreenshotManager::ProcessImage(u32* pData, u32 size, bool bGammaCorrection)
{
	u32* pPixel = pData;
	u32* pEnd = pPixel + size;

	if (bGammaCorrection)
	{
		DXGI_GAMMA_CONTROL G = dxRenderDeviceRender::Instance().GetGammaLUT();
		for (u32 i = 0; i < 256; ++i)
		{
			G.GammaCurve[i].Red   /= 256;
			G.GammaCurve[i].Green /= 256;
			G.GammaCurve[i].Blue  /= 256;
		}

		// Apply gamma correction and kill aplha
		for (; pPixel != pEnd; ++pPixel)
		{
			u32 p = *pPixel;
			*pPixel = color_xrgb(
				(u32)G.GammaCurve[color_get_R(p)].Red,
				(u32)G.GammaCurve[color_get_G(p)].Green,
				(u32)G.GammaCurve[color_get_B(p)].Blue
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
	ID3DTexture2D* pSmallTexture = nullptr;
	ID3DResource* pBackBufferRes = nullptr;
	ID3DBlob* pData = nullptr;

	D3D_SUBRESOURCE_DATA TexData;
	D3D_TEXTURE2D_DESC desc;

	// Copy back buffer to resource
	HW.pBaseRT->GetResource(&pBackBufferRes);
	VERIFY(pBackBufferRes);

	std::memset(&desc, 0, sizeof(desc));
	desc.Width = Device.dwWidth;
	desc.Height = Device.dwHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
	desc.Usage = D3D_USAGE_DEFAULT;
	desc.BindFlags = D3D_BIND_SHADER_RESOURCE;
	desc.SampleDesc.Count = 1;

	// Apply gamma settings to screenshot
	//// [FX]: For the correctness of the code is not sure. DirectX i not studied. 
	std::memset(&TexData, 0, sizeof(D3D_SUBRESOURCE_DATA));
	TexData.pSysMem = (u32*)Memory.mem_alloc(Device.dwWidth * Device.dwHeight * sizeof(u32));
	TexData.SysMemPitch = desc.Width * 2;
	TexData.SysMemSlicePitch = desc.Height * TexData.SysMemPitch;

	bool bGammaCorrection = psDeviceFlags.test(rsFullscreen) ? ps_r_postscreen_flags.test(R_FLAG_SS_GAMMA_CORRECTION) : false;
	ProcessImage((u32*)TexData.pSysMem, Device.dwWidth * Device.dwHeight, bGammaCorrection);

	// Save resource to buffer and return it
	CHK_DX(HW.pDevice->CreateTexture2D(&desc, &TexData , &pSmallTexture));
	CHK_DX(D3DX11LoadTextureFromTexture(HW.pContext, pBackBufferRes, nullptr, pSmallTexture));
	CHK_DX(D3DX11SaveTextureToMemory(HW.pContext, pBackBufferRes, (D3DX11_IMAGE_FILE_FORMAT)fmt, &pData, 0)); // Use pSmallTexture for gamma

	// Cleanup
	_RELEASE(pSmallTexture);
	_RELEASE(pBackBufferRes);
	Memory.mem_free(const_cast<void*>(TexData.pSysMem));

	return pData;
}

ID3DBlob* CScreenshotManager::MakeScreenshotForSavedGame()
{
	ID3DTexture2D* pSmallTexture = nullptr;
	ID3DResource* pBackBufferRes = nullptr;
	ID3DBlob* pData = nullptr;

	// Copy back buffer to resource
	HW.pBaseRT->GetResource(&pBackBufferRes);
	VERIFY(pBackBufferRes);

	D3D_TEXTURE2D_DESC desc;
	std::memset(&desc, 0, sizeof(desc));
	desc.Width = GAMESAVE_IMAGE_SIZE;
	desc.Height = GAMESAVE_IMAGE_SIZE;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_BC1_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// Create small texture, copy (and resize) resourse to it
	CHK_DX(HW.pDevice->CreateTexture2D(&desc, nullptr, &pSmallTexture));
	CHK_DX(D3DX11LoadTextureFromTexture(HW.pContext, pBackBufferRes, nullptr, pSmallTexture));
	_RELEASE(pBackBufferRes);

	// Save resource to buffer and return it
	D3DX11SaveTextureToMemory(HW.pContext, pSmallTexture, D3DX11_IFF_DDS, &pData, 0);
	_RELEASE(pSmallTexture);

	return pData;
}

ID3DBlob* CScreenshotManager::MakeScreenshotForCubeMap(u32 fmt)
{
	//#TODO: implement cubemap screenshot.
	R_ASSERT2(false, "Not implemented.");

	return nullptr;
}

ID3DBlob* CScreenshotManager::MakeScreenshotForLevelMap(u32 fmt)
{
	//#TODO: implement level map screenshot.
	R_ASSERT2(false, "Not implemented.");

	return nullptr;
}
