// xrRender_R2.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "../xrRender/dxRenderFactory.h"
#include "../xrRender/dxUIRender.h"
#include "../xrRender/dxDebugRender.h"

#pragma comment(lib,"xrEngine.lib")

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH	:
		//	Can't call CreateDXGIFactory from DllMain
		//if (!xrRender_test_hw())	return FALSE;
		::Render					= &RImplementation;
		::RenderFactory				= &RenderFactoryImpl;
		::DU						= &DUImpl;
		//::vid_mode_token			= inited by HW;
		UIRender					= &UIRenderImpl;
		DRender						= &DebugRenderImpl;
		xrRender_initconsole		();
		break	;
	case DLL_THREAD_ATTACH	:
	case DLL_THREAD_DETACH	:
	case DLL_PROCESS_DETACH	:
		break;
	}
	return TRUE;
}

bool _declspec(dllexport) SupportsDX11Rendering()
{
	// Register class
	WNDCLASSEX wcex;
	std::memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = DefWindowProc;
	wcex.hInstance = GetModuleHandle(nullptr);
	wcex.lpszClassName = "TestDX11WindowClass";
	if (!RegisterClassEx(&wcex))
	{
		Msg("* DX11: failed to register window class");
		return false;
	}

	// Create window
	HWND hWnd = CreateWindow("TestDX11Class", "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, 0, 0);

	DXGI_SWAP_CHAIN_DESC sd;

	if (!hWnd)
	{
		Msg("* DX11: failed to create window");
		return false;
	}

	HRESULT hr = E_FAIL;

	std::memset(&sd, 0, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 800;
	sd.BufferDesc.Height = 600;
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

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, pFeatureLevels, 1,
		D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &FeatureLevel, &pContext);

	if (FAILED(hr))
		Msg("* D3D11: device creation failed with hr=0x%08x", hr);

	if (pContext) pContext->Release();
	if (pSwapChain) pSwapChain->Release();
	if (pd3dDevice) pd3dDevice->Release();

	DestroyWindow(hWnd);

	return SUCCEEDED(hr);
}

