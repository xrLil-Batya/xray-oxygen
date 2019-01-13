// HW.cpp: implementation of the CHW class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)
#include "HW.h"
#include "../../xrEngine/XR_IOConsole.h"

CHW HW;

struct DM1024
{
	DEVMODE		sys_mode;
	string1024	sm_buffer;
} g_dm;

#ifdef DEBUG
IDirect3DStateBlock9*	dwDebugSB = 0;
#endif

CHW::CHW() : pD3D(nullptr), pDevice(nullptr), pBaseRT(nullptr), pBaseZB(nullptr), m_move_window(true)
{
}

CHW::~CHW()
{
}

void CHW::ResizeWindowProc(WORD h, WORD w)
{
	Reset(NULL);
}

void CHW::Reset(HWND hwnd)
{
#ifdef DEBUG
	_RELEASE(dwDebugSB);
#endif
	_RELEASE(pBaseZB);
	_RELEASE(pBaseRT);

	bool bWindowed = strstr(Core.Params, "-editor") || !psDeviceFlags.is(rsFullscreen);

	SelectResolution(DevPP.BackBufferWidth, DevPP.BackBufferHeight, bWindowed);
	// Windoze
	DevPP.Windowed						= bWindowed;
	DevPP.PresentationInterval			= SelectPresentInterval();
	DevPP.BackBufferCount				= psDeviceFlags.test(rsTripleBuffering) ? 2 : 1;
	DevPP.FullScreen_RefreshRateInHz	= bWindowed ? D3DPRESENT_RATE_DEFAULT : SelectRefresh(DevPP.BackBufferWidth, DevPP.BackBufferHeight, Caps.fTarget);

	while (true)
	{
		HRESULT _hr = HW.pDevice->Reset(&DevPP);
		if (SUCCEEDED(_hr))					break;
		Msg("! ERROR: [%dx%d]: %s", DevPP.BackBufferWidth, DevPP.BackBufferHeight, Debug.error2string(_hr));
		Sleep(100);
	}

	R_CHK(pDevice->GetRenderTarget(0, &pBaseRT));
	R_CHK(pDevice->GetDepthStencilSurface(&pBaseZB));
#ifdef DEBUG
	R_CHK(pDevice->CreateStateBlock(D3DSBT_ALL, &dwDebugSB));
#endif
}

#include "../../xrCore/xrAPI.h"
void CHW::CreateD3D()
{
	std::memset(&g_dm, 0, sizeof(g_dm));
	g_dm.sys_mode.dmSize = sizeof(g_dm.sys_mode);
	g_dm.sys_mode.dmDriverExtra = sizeof(g_dm.sm_buffer);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &g_dm.sys_mode);

	this->pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	R_ASSERT2(this->pD3D, "Please install DirectX 9.0c");
}

void CHW::DestroyD3D()
{
	_RELEASE(this->pD3D);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
D3DFORMAT CHW::SelectFmtTarget()
{
	static D3DFORMAT formats[3]
	{
		D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_R8G8B8
	};

	for (int i = 0; i < 3; ++i)
	{
		D3DFORMAT fmt = formats[i];

		if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter, DevT, fmt, fmt, FALSE)))
			return fmt;
	}

	return D3DFMT_UNKNOWN;
}

D3DFORMAT CHW::SelectFmtDepthStencil(D3DFORMAT fTarget)
{
	return D3DFMT_D24S8;
}

void	CHW::DestroyDevice()
{
	_SHOW_REF("refCount:pBaseZB", pBaseZB);
	_RELEASE(pBaseZB);

	_SHOW_REF("refCount:pBaseRT", pBaseRT);
	_RELEASE(pBaseRT);
#ifdef DEBUG
	_SHOW_REF("refCount:dwDebugSB", dwDebugSB);
	_RELEASE(dwDebugSB);
#endif
#ifdef _EDITOR
	_RELEASE(HW.pDevice);
#else
	_SHOW_REF("DeviceREF:", HW.pDevice);
	_RELEASE(HW.pDevice);
#endif    
	DestroyD3D();

#ifndef _EDITOR
	FreeVidModeList();
#endif
}
void CHW::SelectResolution(u32 &dwWidth, u32 &dwHeight, BOOL bWindowed)
{
	FillVidModeList();

	if (bWindowed)
	{
		dwWidth = psCurrentVidMode[0];
		dwHeight = psCurrentVidMode[1];
	}
	else //check
	{
#ifndef _EDITOR
		string64					buff;
		xr_sprintf(buff, sizeof(buff), "%dx%d", psCurrentVidMode[0], psCurrentVidMode[1]);

		if (_ParseItem(buff, vid_mode_token) == u32(-1)) //not found
		{ //select safe
			xr_sprintf(buff, sizeof(buff), "vid_mode %s", vid_mode_token[0].name);
			Console->Execute(buff);
		}

		dwWidth = psCurrentVidMode[0];
		dwHeight = psCurrentVidMode[1];
#endif
	}
}

void CHW::CreateDevice(HWND m_hWnd, bool move_window)
{
	m_move_window = move_window;
	CreateD3D();

	BOOL  bWindowed = !psDeviceFlags.is(rsFullscreen) || strstr(Core.Params, "-editor");

	DevAdapter = D3DADAPTER_DEFAULT;
	DevT = Caps.bForceGPU_REF ? D3DDEVTYPE_REF : D3DDEVTYPE_HAL;

#ifndef	MASTER_GOLD
	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	for (UINT Adapter = 0; Adapter < pD3D->GetAdapterCount(); Adapter++) {
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT Res = pD3D->GetAdapterIdentifier(Adapter, 0, &Identifier);
		if (SUCCEEDED(Res) && (xr_strcmp(Identifier.Description, "NVIDIA PerfHUD") == 0))
		{
			DevAdapter = Adapter;
			DevT = D3DDEVTYPE_REF;
			break;
		}
	}
#endif	//	MASTER_GOLD


	// Display the name of video board
	D3DADAPTER_IDENTIFIER9	adapterID;
	R_CHK(pD3D->GetAdapterIdentifier(DevAdapter, 0, &adapterID));
	Msg("* GPU [vendor:%X]-[device:%X]: %s", adapterID.VendorId, adapterID.DeviceId, adapterID.Description);

	u16	drv_Product = HIWORD(adapterID.DriverVersion.HighPart);
	u16	drv_Version = LOWORD(adapterID.DriverVersion.HighPart);
	u16	drv_SubVersion = HIWORD(adapterID.DriverVersion.LowPart);
	u16	drv_Build = LOWORD(adapterID.DriverVersion.LowPart);
	Msg("* GPU driver: %d.%d.%d.%d", u32(drv_Product), u32(drv_Version), u32(drv_SubVersion), u32(drv_Build));

	Caps.id_vendor = adapterID.VendorId;
	Caps.id_device = adapterID.DeviceId;

	// Retreive windowed mode
	D3DDISPLAYMODE mWindowed;
	R_CHK(pD3D->GetAdapterDisplayMode(DevAdapter, &mWindowed));

	// Select back-buffer & depth-stencil format
	D3DFORMAT&	fTarget = Caps.fTarget;
	D3DFORMAT&	fDepth = Caps.fDepth;
	if (bWindowed)
	{
		fTarget		= mWindowed.Format;
		R_CHK		(pD3D->CheckDeviceType(DevAdapter, DevT, fTarget, fTarget, TRUE));
		fDepth		= SelectFmtDepthStencil(fTarget);
	}
	else
	{
		fTarget		= SelectFmtTarget();
		fDepth		= SelectFmtDepthStencil(fTarget);
	}

	if ((D3DFMT_UNKNOWN == fTarget) || (D3DFMT_UNKNOWN == fDepth))
	{
		Msg("Failed to initialize graphics hardware.\n"
			"Please try to restart the game.\n"
			"Can not find matching format for back buffer."
		);
		xrLogger::FlushLog();
		MessageBox(NULL, "Failed to initialize graphics hardware.\nPlease try to restart the game.", "Error!", MB_OK | MB_ICONERROR);
		TerminateProcess(GetCurrentProcess(), 0);
	}


	// Set up the presentation parameters
	D3DPRESENT_PARAMETERS&	P = DevPP;
	std::memset(&P, 0, sizeof(P));

#ifndef _EDITOR
	SelectResolution(P.BackBufferWidth, P.BackBufferHeight, bWindowed);
#endif

	// Back buffer
	P.BackBufferFormat				= fTarget;
	P.BackBufferCount				= psDeviceFlags.test(rsTripleBuffering) ? 2 : 1;

	// Multisample
	P.MultiSampleType				= D3DMULTISAMPLE_NONE;
	P.MultiSampleQuality			= 0;

	// Windoze
	P.Windowed						= bWindowed;
	P.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	P.hDeviceWindow					= m_hWnd;

	// Depth/stencil
	P.EnableAutoDepthStencil		= TRUE;
	P.AutoDepthStencilFormat		= fDepth;
	P.Flags							= 0;

	// Refresh rate
	P.PresentationInterval			= SelectPresentInterval();
	P.FullScreen_RefreshRateInHz	= bWindowed ? D3DPRESENT_RATE_DEFAULT : SelectRefresh(P.BackBufferWidth, P.BackBufferHeight, fTarget);

	// Create the device
	u32 GPU = SelectGPU();
	HRESULT R = HW.pD3D->CreateDevice(DevAdapter, DevT, m_hWnd, GPU | D3DCREATE_MULTITHREADED, &P, &pDevice);

	if (FAILED(R))
	{
		R = HW.pD3D->CreateDevice(DevAdapter, DevT, m_hWnd, GPU | D3DCREATE_MULTITHREADED, &P, &pDevice);
	}

	if (D3DERR_DEVICELOST == R)
	{
		// Fatal error! Cannot create rendering device AT STARTUP !!!
		Msg("Failed to initialize graphics hardware.\nPlease try to restart the game.\n CreateDevice returned 0x%08x(D3DERR_DEVICELOST)", R);
		xrLogger::FlushLog();
		MessageBox(NULL, "Failed to initialize graphics hardware.\nPlease try to restart the game.", "Error!", MB_OK | MB_ICONERROR);
		TerminateProcess(GetCurrentProcess(), 0);
	};
	R_CHK(R);

	_SHOW_REF("* CREATE: DeviceREF:", HW.pDevice);
	switch (GPU)
	{
	case D3DCREATE_SOFTWARE_VERTEXPROCESSING:	Log("* Vertex Processor: SOFTWARE"); break;
	case D3DCREATE_MIXED_VERTEXPROCESSING:		Log("* Vertex Processor: MIXED"); break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING:	Log("* Vertex Processor: HARDWARE"); break;
	case D3DCREATE_PUREDEVICE:					Log("* Vertex Processor: PURE HARDWARE"); break;
	}

	// Capture misc data
#ifdef DEBUG
	R_CHK(pDevice->CreateStateBlock(D3DSBT_ALL, &dwDebugSB));
#endif
	R_CHK(pDevice->GetRenderTarget(0, &pBaseRT));
	R_CHK(pDevice->GetDepthStencilSurface(&pBaseZB));
	u32	memory = pDevice->GetAvailableTextureMem();
	Msg("[INFO] Texture memory: %d M", memory / (1024 * 1024));
	Msg("[INFO] DDI-level: %2.1f", float(D3DXGetDriverLevel(pDevice)) / 100.f);
#ifndef _EDITOR
	FillVidModeList();
#endif
}

u32	CHW::SelectPresentInterval()
{
	if (psDeviceFlags.test(rsVSync))
		return D3DPRESENT_INTERVAL_ONE;

	return D3DPRESENT_INTERVAL_IMMEDIATE;
}

u32 CHW::SelectGPU()
{
	D3DCAPS9 caps;
	pD3D->GetDeviceCaps(DevAdapter, DevT, &caps);

	if (!Caps.bForceGPU_SW && (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
	{
		if (!Caps.bForceGPU_NonPure && (caps.DevCaps & D3DDEVCAPS_PUREDEVICE))
			return D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;

		return D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
}

u32 CHW::SelectRefresh(u32 dwWidth, u32 dwHeight, D3DFORMAT fmt)
{
	if (psDeviceFlags.is(rsRefresh60hz) || psDeviceFlags.is(rsRefresh120hz))
		return D3DPRESENT_RATE_DEFAULT;
	else
	{
		u32 selected = D3DPRESENT_RATE_DEFAULT;
		u32 count = pD3D->GetAdapterModeCount(DevAdapter, fmt);
		for (u32 I = 0; I<count; I++)
		{
			D3DDISPLAYMODE	Mode;
			pD3D->EnumAdapterModes(DevAdapter, fmt, I, &Mode);
			if (Mode.Width == dwWidth && Mode.Height == dwHeight)
			{
				if (Mode.RefreshRate>selected)
					selected = Mode.RefreshRate;

				// Select desktop refresh rate
				if (Mode.RefreshRate == g_dm.sys_mode.dmDisplayFrequency)
				{
					selected = Mode.RefreshRate;
					break;
				}
			}
		}
		return selected;
	}
}

bool CHW::IsFormatSupported(D3DFORMAT fmt, DWORD type, DWORD usage)
{
	HRESULT hr = pD3D->CheckDeviceFormat(DevAdapter, DevT, Caps.fTarget, usage, (D3DRESOURCETYPE)type, fmt);
	if (FAILED(hr))
		return false;

	return true;
}

#ifndef _EDITOR

struct _uniq_mode
{
	_uniq_mode(LPCSTR v) :_val(v) {}
	LPCSTR _val;
	bool operator() (LPCSTR _other) { return !stricmp(_val, _other); }
};

void CHW::FreeVidModeList()
{
	for (int i = 0; vid_mode_token[i].name; i++)
	{
		xr_free(vid_mode_token[i].name);
	}
	xr_free(vid_mode_token);
}

void CHW::FillVidModeList()
{
	if (vid_mode_token)	return;

	xr_vector<LPCSTR> _tmp;
	u32 cnt = pD3D->GetAdapterModeCount(DevAdapter, Caps.fTarget);

	for (u32 i = 0; i < cnt; ++i)
	{
		D3DDISPLAYMODE Mode;
		string16 str;

		pD3D->EnumAdapterModes(DevAdapter, Caps.fTarget, i, &Mode);

		xr_sprintf(str, sizeof(str), "%dx%d", Mode.Width, Mode.Height);

		if (_tmp.end() != std::find_if(_tmp.begin(), _tmp.end(), _uniq_mode(str)))
			continue;

		_tmp.push_back(xr_strdup(str));
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
#endif
