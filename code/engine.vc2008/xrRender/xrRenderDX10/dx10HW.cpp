// dx10HW.cpp: implementation of the DX10 specialisation of CHW.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop
#include <VersionHelpers.h>
#pragma warning(disable:4995)
#include <d3dx9.h>
#include <dxgi1_4.h>
#pragma warning(default:4995)
#include "../xrRender/HW.h"
#include "../../xrEngine/XR_IOConsole.h"
#include "../../xrCore/xrAPI.h"

#include "StateManager\dx10SamplerStateCache.h"
#include "StateManager\dx10StateCache.h"

struct DM1024
{
	DEVMODE		sys_mode;
	string1024	sm_buffer;
} g_dm;

ENGINE_API BOOL isGraphicDebugging;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
RENDER_API CHW HW;

CHW::CHW() : m_pAdapter(nullptr), pDevice(nullptr), m_move_window(true), pAnnotation(nullptr), m_bDX11_1(false)
{
	Device.seqAppActivate.Add(this);
	Device.seqAppDeactivate.Add(this);
}

CHW::~CHW()
{
	Device.seqAppActivate.Remove(this);
	Device.seqAppDeactivate.Remove(this);
}

//////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////
void CHW::CreateD3D()
{
	// Init g_dm
	std::memset(&g_dm, 0, sizeof(g_dm));
	g_dm.sys_mode.dmSize = sizeof(g_dm.sys_mode);
	g_dm.sys_mode.dmDriverExtra = sizeof(g_dm.sm_buffer);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &g_dm.sys_mode);

	m_bUsePerfhud = false;

	// Init pAdapter
	if (IsWindows10OrGreater())
	{
		IDXGIFactory4 * pFactory;
		CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)(&pFactory));
		pFactory->EnumAdapters1(0, &m_pAdapter);
		_RELEASE(pFactory);
	}
	else if (IsWindows8Point1OrGreater())
	{
		IDXGIFactory3 * pFactory;
		CreateDXGIFactory1(__uuidof(IDXGIFactory3), (void**)(&pFactory));
		pFactory->EnumAdapters1(0, &m_pAdapter);
		_RELEASE(pFactory);
	}
	else if (IsWindows8OrGreater())
	{
		IDXGIFactory2 * pFactory;
		CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)(&pFactory));
		pFactory->EnumAdapters1(0, &m_pAdapter);
		_RELEASE(pFactory);
	}
	else
	{
		IDXGIFactory1 * pFactory;
		CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory));
		pFactory->EnumAdapters1(0, &m_pAdapter);
		_RELEASE(pFactory);
	}
}

void CHW::DestroyD3D()
{
	_SHOW_REF("refCount:m_pAdapter", m_pAdapter);
	_RELEASE(m_pAdapter);
}

void CHW::CreateDevice(HWND m_hWnd, bool move_window)
{
	m_move_window = move_window;
	CreateD3D();

	// General - select adapter and device
	BOOL bWindowed = !psDeviceFlags.is(rsFullscreen) || strstr(Core.Params, "-editor");

	m_DriverType = Caps.bForceGPU_REF ? D3D_DRIVER_TYPE_REFERENCE : D3D_DRIVER_TYPE_HARDWARE;

	if (m_bUsePerfhud)
		m_DriverType = D3D_DRIVER_TYPE_REFERENCE;

	// Display the name of video board
	DXGI_ADAPTER_DESC1 Desc;
	R_CHK(m_pAdapter->GetDesc1(&Desc));
	//	Warning: Desc.Description is wide string
	Msg("* GPU [vendor:%X]-[device:%X]: %S", Desc.VendorId, Desc.DeviceId, Desc.Description);
	Caps.id_vendor = Desc.VendorId;
	Caps.id_device = Desc.DeviceId;

	// MatthewKush to all: Please change to DXGI_SWAP_CHAIN_DESC1 (for lots of reasons)
	DXGI_SWAP_CHAIN_DESC &sd = m_ChainDesc;
	memset(&sd, 0, sizeof(sd));
	SelectResolution(sd.BufferDesc.Width, sd.BufferDesc.Height, bWindowed);

	//	TODO: DX10: implement dynamic format selection
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // HDR10 = DXGI_FORMAT_R10G10B10A2_UNORM
	sd.BufferCount = psDeviceFlags.test(rsTripleBuffering) ? 3 : 2;

	// Multisample
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.OutputWindow = m_hWnd;
	sd.Windowed = bWindowed;
	sd.BufferDesc.RefreshRate = SelectRefresh(sd.BufferDesc.Width, sd.BufferDesc.Height, sd.BufferDesc.Format);
	//sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

	//	Additional set up
	UINT createDeviceFlags = 0;//Temp reverted (this is causing the CTD's) //D3D11_CREATE_DEVICE_SINGLETHREADED;
	if (isGraphicDebugging)
	{
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}

	// Front buffer
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	D3D_FEATURE_LEVEL pFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
	};
	HRESULT R;

	for (u32 FeatIter = 0; FeatIter < sizeof(pFeatureLevels) / sizeof(D3D_FEATURE_LEVEL); FeatIter++)
	{
		D3D_FEATURE_LEVEL &refFeature = pFeatureLevels[FeatIter];
		R = D3D11CreateDeviceAndSwapChain
		(
			nullptr, m_DriverType, nullptr, createDeviceFlags,
			&refFeature, sizeof(refFeature),
			D3D11_SDK_VERSION,
			&sd, &m_pSwapChain, &pDevice,
			&FeatureLevel, &pContext
		);

		if (!FAILED(R))
		{
			m_bDX11_1 = refFeature == D3D_FEATURE_LEVEL_11_1;
			break;
		}
	}

	if (IsWindows10OrGreater())
	{
		IDXGIDevice3 * pDXGIDevice;
		R_CHK(pDevice->QueryInterface(__uuidof(IDXGIDevice3), (void **)&pDXGIDevice));

		IDXGIAdapter3 * pDXGIAdapter;
		R_CHK(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter3), (void **)&pDXGIAdapter));

		R = pDXGIDevice->SetMaximumFrameLatency(1);
	}
	else if (IsWindows8Point1OrGreater())
	{
		IDXGIDevice2 * pDXGIDevice;
		R_CHK(pDevice->QueryInterface(__uuidof(IDXGIDevice2), (void **)&pDXGIDevice));

		IDXGIAdapter2 * pDXGIAdapter;
		R_CHK(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter2), (void **)&pDXGIAdapter));

		R = pDXGIDevice->SetMaximumFrameLatency(1);
	}
	else
	{
		IDXGIDevice1 * pDXGIDevice;
		R_CHK(pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void **)&pDXGIDevice));

		IDXGIAdapter1 * pDXGIAdapter;
		R_CHK(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter1), (void **)&pDXGIAdapter));

		R = pDXGIDevice->SetMaximumFrameLatency(1);
	}

	if (FAILED(R))
	{
		// Fatal error! Cannot create rendering device AT STARTUP !!!
		Msg("Failed to initialize graphics hardware.\n"
			"Please try to restart the game.\n"
			"CreateDevice returned 0x%08x", R);
		xrLogger::FlushLog();
		FATAL("Failed to initialize graphics hardware.\nPlease try to restart the game.");
	};
	R_CHK(R);

	// main anotation
	if (m_bDX11_1)
	{
		// Only for Win8 and later
		R_CHK(pContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**)& pAnnotation));
	}

	_SHOW_REF("* CREATE: DeviceREF:", HW.pDevice);
	//	Create render target and depth-stencil views here
	UpdateViews();

	size_t	memory = Desc.DedicatedVideoMemory;
	Msg("* Texture memory: %d M", memory / (1024 * 1024));
	FillVidModeList();
}

void CHW::DestroyDevice()
{
	//	Destroy state managers
	StateManager.Reset();
	RSManager.ClearStateArray();
	DSSManager.ClearStateArray();
	BSManager.ClearStateArray();
	SSManager.ClearStateArray();

	_SHOW_REF("refCount:pBaseZB", pBaseZB);
	_RELEASE(pBaseZB);

	_SHOW_REF("refCount:pBaseRT", pBaseRT);
	_RELEASE(pBaseRT);
	//	Must switch to windowed mode to release swap chain
	if (!m_ChainDesc.Windowed) m_pSwapChain->SetFullscreenState(FALSE, NULL);
	_SHOW_REF("refCount:m_pSwapChain", m_pSwapChain);
	_RELEASE(m_pSwapChain);

	_RELEASE(pContext);
	_SHOW_REF("refCount:DeviceREF:", HW.pDevice);
	_RELEASE(HW.pDevice);

	// main anotation
	_RELEASE(pAnnotation);

	DestroyD3D();

#ifndef _EDITOR
	FreeVidModeList();
#endif
}

//////////////////////////////////////////////////////////////////////
// Resetting device
//////////////////////////////////////////////////////////////////////
void CHW::Reset(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC &sd = m_ChainDesc;

	bool bWindowed = !psDeviceFlags.is(rsFullscreen) || strstr(Core.Params, "-editor");

	sd.Windowed		= bWindowed;
	sd.BufferCount	= psDeviceFlags.test(rsTripleBuffering) ? 3 : 2;

	m_pSwapChain->SetFullscreenState(!bWindowed, NULL);

	DXGI_MODE_DESC	&desc = m_ChainDesc.BufferDesc;

	SelectResolution(desc.Width, desc.Height, bWindowed);
	desc.RefreshRate = SelectRefresh(desc.Width, desc.Height, desc.Format);

	CHK_DX(m_pSwapChain->ResizeTarget(&desc));

	_SHOW_REF("refCount:pBaseZB", pBaseZB);
	_SHOW_REF("refCount:pBaseRT", pBaseRT);

	_RELEASE(pBaseZB);
	_RELEASE(pBaseRT);

	CHK_DX(m_pSwapChain->ResizeBuffers(
		sd.BufferCount,
		desc.Width,
		desc.Height,
		desc.Format,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		)
	);

	UpdateViews();
}

void CHW::ResizeWindowProc(WORD h, WORD w)
{
	Reset(NULL);
}


void CHW::SelectResolution(u32 &dwWidth, u32 &dwHeight, BOOL bWindowed)
{
	FillVidModeList();

	if (bWindowed)
	{
		dwWidth = psCurrentVidMode[0];
		dwHeight = psCurrentVidMode[1];
	}
	else
	{
		string64 buff;
		xr_sprintf(buff, sizeof(buff), "%dx%d", psCurrentVidMode[0], psCurrentVidMode[1]);

		if (_ParseItem(buff, vid_mode_token) == u32(-1)) //not found
		{ //select safe
			xr_sprintf(buff, sizeof(buff), "vid_mode %s", vid_mode_token[0].name);
			Console->Execute(buff);
		}

		dwWidth = psCurrentVidMode[0];
		dwHeight = psCurrentVidMode[1];
	}
}

//	TODO: DX10: check if we need these
DXGI_RATIONAL CHW::SelectRefresh(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt)
{
	DXGI_RATIONAL	res;

	res.Numerator = 60;
	res.Denominator = 1;

	float CurrentFreq = 60.0f;

	if (ps_r_RefreshHZ == 0)
	{
		return res;
	}
	else if (ps_r_RefreshHZ == 1)
	{
		res.Numerator = 120;
		return res;
	}
	else
	{
		xr_vector<DXGI_MODE_DESC>	modes;

		IDXGIOutput *pOutput;
		m_pAdapter->EnumOutputs(0, &pOutput);
		VERIFY(pOutput);

		UINT num = 0;
		DXGI_FORMAT format = fmt;
		UINT flags = 0;

		// Get the number of display modes available
		pOutput->GetDisplayModeList(format, flags, &num, 0);

		// Get the list of display modes
		modes.resize(num);
		pOutput->GetDisplayModeList(format, flags, &num, &modes.front());

		_RELEASE(pOutput);

		for (u32 i = 0; i<num; ++i)
		{
			DXGI_MODE_DESC &desc = modes[i];

			if ((desc.Width == dwWidth) && (desc.Height == dwHeight))
			{
				VERIFY(desc.RefreshRate.Denominator);
				float TempFreq = float(desc.RefreshRate.Numerator) / float(desc.RefreshRate.Denominator);
				if (TempFreq > CurrentFreq)
				{
					CurrentFreq = TempFreq;
					res = desc.RefreshRate;
				}

				// Select desktop frequency
				if (TempFreq == g_dm.sys_mode.dmDisplayFrequency)
				{
					res = desc.RefreshRate;
					break;
				}
			}
		}

		return res;
	}
}

void CHW::OnAppActivate()
{
	if (m_pSwapChain && !m_ChainDesc.Windowed)
	{
		ShowWindow(m_ChainDesc.OutputWindow, SW_RESTORE);
		m_pSwapChain->SetFullscreenState(TRUE, NULL);
	}
}

void CHW::OnAppDeactivate()
{
	if (m_pSwapChain && !m_ChainDesc.Windowed)
	{
		m_pSwapChain->SetFullscreenState(FALSE, NULL);
		ShowWindow(m_ChainDesc.OutputWindow, SW_MINIMIZE);
	}
}


bool CHW::IsFormatSupported(DXGI_FORMAT fmt)
{
	u32 FormatSupport;
	HRESULT hr = pDevice->CheckFormatSupport(fmt, &FormatSupport);;
	if (FAILED(hr))
		return false;

	return true;
}

struct _uniq_mode
{
	_uniq_mode(LPCSTR v) :_val(v) {}
	LPCSTR _val;
	bool operator() (LPCSTR _other) { return !stricmp(_val, _other); }
};

#ifndef _EDITOR
void CHW::FreeVidModeList()
{
	for (int i = 0; vid_mode_token[i].name; i++)
	{
		xr_free(vid_mode_token[i].name);
	}
	xr_free(vid_mode_token);
	vid_mode_token = NULL;
}

void CHW::FillVidModeList()
{
	if (vid_mode_token != NULL)
		return;

	xr_vector<LPCSTR>	_tmp;
	xr_vector<DXGI_MODE_DESC>	modes;

	IDXGIOutput *pOutput;
	m_pAdapter->EnumOutputs(0, &pOutput);
	VERIFY(pOutput);

	UINT num = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;//Don't touch
	UINT flags = 0;

	// Get the number of display modes available
	pOutput->GetDisplayModeList(format, flags, &num, 0);

	// Get the list of display modes
	modes.resize(num);
	pOutput->GetDisplayModeList(format, flags, &num, &modes.front());

	_RELEASE(pOutput);

	for (u32 i = 0; i<num; ++i)
	{
		DXGI_MODE_DESC &desc = modes[i];
		string32		str;

		xr_sprintf(str, sizeof(str), "%dx%d", desc.Width, desc.Height);

		if (_tmp.end() != std::find_if(_tmp.begin(), _tmp.end(), _uniq_mode(str)))
			continue;

		_tmp.push_back(NULL);
		_tmp.back() = xr_strdup(str);
	}

	u32 _cnt = _tmp.size() + 1;

	vid_mode_token = xr_alloc<xr_token>(_cnt);

	vid_mode_token[_cnt - 1].id = -1;
	vid_mode_token[_cnt - 1].name = NULL;

#ifdef DEBUG
	Msg("Available video modes[%d]:", _tmp.size());
#endif // DEBUG
	for (u32 i = 0; i<_tmp.size(); ++i)
	{
		vid_mode_token[i].id = i;
		vid_mode_token[i].name = _tmp[i];
#ifdef DEBUG
		Msg("[%s]", _tmp[i]);
#endif // DEBUG
	}
}

void CHW::UpdateViews()
{
	DXGI_SWAP_CHAIN_DESC &sd = m_ChainDesc;
	HRESULT R;

	ID3DTexture2D *pBuffer;
	R = m_pSwapChain->GetBuffer(0, __uuidof(ID3DTexture2D), (LPVOID*)&pBuffer);
	R_CHK(R);

	R = pDevice->CreateRenderTargetView(pBuffer, nullptr, &pBaseRT);
	pBuffer->Release();
	R_CHK(R);

	//	Create Depth/stencil buffer
	//	HACK: DX10: hard depth buffer format
	ID3DTexture2D* pDepthStencil = NULL;
	D3D_TEXTURE2D_DESC descDepth;
	descDepth.Width = sd.BufferDesc.Width;
	descDepth.Height = sd.BufferDesc.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D_USAGE_DEFAULT;
	descDepth.BindFlags = D3D_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	R = pDevice->CreateTexture2D(&descDepth /* Texture desc */, NULL /* Initial data */, &pDepthStencil); // [out] Texture
	R_CHK(R);

	//	Create Depth/stencil view
	R = pDevice->CreateDepthStencilView(pDepthStencil, NULL, &pBaseZB);
	R_CHK(R);

	_RELEASE(pDepthStencil);
}
#endif
