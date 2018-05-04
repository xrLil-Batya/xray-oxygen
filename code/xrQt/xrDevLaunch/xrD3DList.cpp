/*************************************************
* OXYGEN TEAM, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 26 March, 2018
* RenderList.cpp - Checking to available render
* CreateRendererList()
*************************************************/
#include "xrMain.h"
#ifdef __cplusplus
///////////////////////////////////
#include <d3d9.h>
#include <d3d11.h>
////////////////////////////////////
#pragma comment(lib, "xrCore.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
////////////////////////////////////
#include "../../engine.vc2008/xrCore/xrCore.h"
#include <stdio.h>
#include <iostream>
///////////////////////////////////////////////
DLL_API xr_vector<xr_token> vid_quality_token;

constexpr const char* r2_name = "xrRender_R2";
constexpr const char* r3_name = "xrRender_R3";
constexpr const char* r4_name = "xrRender_R4";
/////////////////////////////////////////////////////

bool SupportsAdvancedRendering()
{
	D3DCAPS9 caps;

	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	pD3D->Release();

	u16 ps_ver_major = u16(u32(u32(caps.PixelShaderVersion)&u32(0xf << 8ul)) >> 8);

	return								!(ps_ver_major < 3);
}

bool SupportsDX10Rendering()
{
	IDXGIAdapter*  m_pAdapter;	//	pD3D equivalent
	IDXGIFactory * pFactory;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
	pFactory->EnumAdapters(0, &m_pAdapter);

	HRESULT hr = m_pAdapter->CheckInterfaceSupport(__uuidof(ID3D10Device), 0);

	pFactory->Release();
	pFactory = nullptr;
	m_pAdapter->Release();
	m_pAdapter = nullptr;

	return SUCCEEDED(hr);
}

bool SupportsDX11Rendering()
{
	// Register class
	WNDCLASSEX wcex;
	std::memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = DefWindowProc;
	wcex.hInstance = GetModuleHandle(nullptr);
	wcex.lpszClassName = "TestDX11WindowClass";

	// If class can't register
	if (!RegisterClassEx(&wcex))
	{
		Msg("* DX11: failed to register window class");
		return false;
	}

	// Create window
	HWND hWnd = CreateWindow("TestDX11WindowClass", "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);

	// If window can't create
	if (!hWnd)
	{
		Msg("* DX11: failed to create window");
		return false;
	}

	HRESULT hr = E_FAIL;
	DXGI_SWAP_CHAIN_DESC sd;

	std::memset(&sd, 0, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 640;
	sd.BufferDesc.Height = 360;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL pFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL FeatureLevel;

	ID3D11Device*           pd3dDevice = NULL;
	ID3D11DeviceContext*    pContext = NULL;
	IDXGISwapChain*         pSwapChain = NULL;

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, 0, pFeatureLevels, 1,
		D3D11_SDK_VERSION, &sd,
		&pSwapChain, &pd3dDevice,
		&FeatureLevel, &pContext);

	//#TODO: graphics pointers must delete by Release();
	if (pContext)	pContext->Release();
	if (pSwapChain) pSwapChain->Release();
	if (pd3dDevice) pd3dDevice->Release();

	DestroyWindow(hWnd);

	return SUCCEEDED(hr);
}

void CreateRendererList()
{
	if (!vid_quality_token.empty())
		return;

	xr_vector<xr_token> modes;

	// try to initialize R2
	{
		modes.push_back(xr_token("renderer_r2a", 1));
		modes.emplace_back(xr_token("renderer_r2", 2));
		if (SupportsAdvancedRendering())
			modes.emplace_back(xr_token("renderer_r2.5", 3));
	}

	// try to initialize R3
	// Restore error handling
	SetErrorMode(0);
	{
		if (SupportsDX10Rendering())
			modes.emplace_back(xr_token("renderer_r3", 4));
	}

	// try to initialize R4
	// Restore error handling
	SetErrorMode(0);
	{
		if (SupportsDX11Rendering())
			modes.emplace_back(xr_token("renderer_r4", 5));
	}

	modes.emplace_back(xr_token(nullptr, -1));

	vid_quality_token = std::move(modes);
}
#endif