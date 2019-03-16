#include "stdafx.h"
#pragma hdrstop

#include "../xrRender/HW.h"
#include "../../xrEngine/XR_IOConsole.h"
#include "../../xrCore/xrAPI.h"

#include "../xrRenderDX10/StateManager/dx10SamplerStateCache.h"
#include "../xrRenderDX10/StateManager/dx10StateCache.h"
#include "../xrRenderDX10/StateManager/dx10StateManager.h"

struct DM1024
{
	DEVMODE		sys_mode;
	string1024	sm_buffer;
} g_dm;

void	fill_vid_mode_list(CHW* _hw);
void	free_vid_mode_list();

CHW HW;

CHW::CHW() : m_pAdapter(0), pDevice(nullptr), m_move_window(true)
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
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CHW::CreateD3D()
{
	// Init g_dm
	std::memset(&g_dm, 0, sizeof(g_dm));
	g_dm.sys_mode.dmSize = sizeof(g_dm.sys_mode);
	g_dm.sys_mode.dmDriverExtra = sizeof(g_dm.sm_buffer);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &g_dm.sys_mode);

	m_bUsePerfhud = false;

	VkInstanceCreateInfo inst_info = {
		 VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		 NULL,
		 NULL,
		 0,
		 NULL,
		 0,
		 NULL,
	};

	VkInstance inst;
	u32 dev_count = 0;
	VkResult err = vkCreateInstance(&inst_info, NULL, &inst);
	err = vkEnumeratePhysicalDevices(inst, &dev_count, m_pAdapter);
}

void CHW::DestroyD3D()
{
	xr_delete(m_pAdapter);
}

void CHW::CreateDevice(HWND m_hWnd, bool move_window)
{
	m_move_window = move_window;
	CreateD3D();

	// General - select adapter and device
	BOOL  bWindowed = !psDeviceFlags.is(rsFullscreen);

	VkSwapchainKHR &sd = m_ChainDesc;
	memset(&sd, 0, sizeof(sd));

	//SelectResolution(sd.BufferDesc.Width, sd.BufferDesc.Height, bWindowed);
}

void CHW::DestroyDevice()
{
	//	Destroy state managers
	StateManager.Reset();
	RSManager.ClearStateArray();
	DSSManager.ClearStateArray();
	BSManager.ClearStateArray();
	SSManager.ClearStateArray();

	xr_delete(pBaseZB);
	xr_delete(pBaseRT);

	//	Must switch to windowed mode to release swap chain
	//if (!m_ChainDesc.Windowed) m_pSwapChain->SetFullscreenState(FALSE, NULL);

	xr_delete(m_pSwapChain);
	xr_delete(pContext);
	xr_delete(HW.pDevice);

	DestroyD3D();
	free_vid_mode_list();
}

//////////////////////////////////////////////////////////////////////
// Resetting device
//////////////////////////////////////////////////////////////////////
void CHW::Reset(HWND hwnd)
{
	VkSwapchainKHR &cd = m_ChainDesc;

	BOOL	bWindowed = !psDeviceFlags.is(rsFullscreen);

}

void CHW::SelectResolution(u32 &dwWidth, u32 &dwHeight, BOOL bWindowed)
{
	//fill_vid_mode_list(this);

	if (bWindowed)
	{
		dwWidth = psCurrentVidMode[0];
		dwHeight = psCurrentVidMode[1];
	}
	else //check
	{
		string64					buff;
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


u32 CHW::SelectRefresh(u32 dwWidth, u32 dwHeight, VkFormat fmt)
{
	
}

void CHW::OnAppActivate()
{
	if (m_pSwapChain && !psDeviceFlags.is(rsFullscreen))
	{
		//ShowWindow(m_ChainDesc.OutputWindow, SW_RESTORE);
		//m_pSwapChain->SetFullscreenState(TRUE, NULL);
	}
}

void CHW::OnAppDeactivate()
{
	if (m_pSwapChain && !psDeviceFlags.is(rsFullscreen))
	{
		//m_pSwapChain->SetFullscreenState(FALSE, NULL);
		//ShowWindow(m_ChainDesc.OutputWindow, SW_MINIMIZE);
	}
}

struct _uniq_mode
{
	_uniq_mode(LPCSTR v) :_val(v) {}
	LPCSTR _val;
	bool operator() (LPCSTR _other) { return !stricmp(_val, _other); }
};

void free_vid_mode_list()
{
	for (int i = 0; vid_mode_token[i].name; i++)
	{
		xr_free(vid_mode_token[i].name);
	}
	xr_free(vid_mode_token);
	vid_mode_token = NULL;
}

void CHW::UpdateViews()
{
}