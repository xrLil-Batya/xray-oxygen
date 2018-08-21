#include "stdafx.h"
#include "../xrRender/ScreenshotManager.h"

void CScreenshotManager::ProcessImage(u32* pData, u32 size, bool bGammaCorrection)
{
	//#TODO: implement CScreenshotManager::ProcessImage for DX10
}

ID3DBlob* CScreenshotManager::MakeScreenshotNormal(u32 fmt)
{
	ID3DResource* pBackBufferRes = nullptr;
	ID3DBlob* pData = nullptr;

	// Copy back buffer to resource
	HW.pBaseRT->GetResource(&pBackBufferRes);
	VERIFY(pBackBufferRes);

	//#TODO: apply gamma settings to screenshot

	// Save resource to buffer and return it
	CHK_DX(D3DX10SaveTextureToMemory(pBackBufferRes, (D3DX10_IMAGE_FILE_FORMAT)fmt, &pData, 0));
	_RELEASE(pBackBufferRes);

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
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;

	// Create small texture, copy (and resize) resourse to it
	CHK_DX(HW.pDevice->CreateTexture2D(&desc, nullptr, &pSmallTexture));
	CHK_DX(D3DX10LoadTextureFromTexture(pBackBufferRes, nullptr, pSmallTexture));
	_RELEASE(pBackBufferRes);

	// Save resource to buffer and return it
	HRESULT hr = D3DX10SaveTextureToMemory(pSmallTexture, D3DX10_IFF_DDS, &pData, 0);
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
