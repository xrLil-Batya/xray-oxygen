#include "stdafx.h"
#include "../xrCDB/frustum.h"
#include "xr_input.h"
#include "DirectXMathExternal.h"
/////////////////////////////////////
#define MMNOSOUND
#define MMNOMIDI
#define MMNOAUX
#define MMNOMIXER
#define MMNOJOY
/////////////////////////////////////
#include <mmsystem.h>
#include <d3dx9.h>
/////////////////////////////////////
#pragma warning(default:4995)
/////////////////////////////////////
#include "x_ray.h"
#include "render.h"
#include "XR_IOConsole.h"
/////////////////////////////////////
#include "../include/editor/ide.hpp"
/////////////////////////////////////
#include "igame_persistent.h"
/////////////////////////////////////
#pragma comment(lib, "d3dx9.lib")
/////////////////////////////////////
extern bool bEngineloaded;
ENGINE_API CRenderDevice Device;
ENGINE_API CLoadScreenRenderer load_screen_renderer;
/////////////////////////////////////
ENGINE_API BOOL g_bRendering = FALSE; 
/////////////////////////////////////
BOOL		g_bLoaded		= FALSE;
bool		g_bL			= false;
ref_light	precache_light	= nullptr;
/////////////////////////////////////

BOOL CRenderDevice::Begin	()
{
	switch (m_pRender->GetDeviceState())
	{
	case IRenderDeviceRender::dsOK:
		break;

	case IRenderDeviceRender::dsLost:
		// If the device was lost, do not render until we get it back
		Sleep(33);
		return FALSE;

	case IRenderDeviceRender::dsNeedReset:
		// Check if the device is ready to be reset
		Reset();
		break;

	case IRenderDeviceRender::dsRemoved:
		// Driver error. Bye-bye
		Debug.fatal(DEBUG_INFO, "Video card driver broken! I'm sorry...");

	default:
		R_ASSERT(0);
	}

	m_pRender->Begin();

	g_bRendering = TRUE;
	g_bL = true;

	return TRUE;
}

void CRenderDevice::Clear()
{
	m_pRender->Clear();
}

void CRenderDevice::End		()
{
	bool load_finished = false;
	if (dwPrecacheFrame)
	{
		::Sound->set_master_volume	(0.f);
		dwPrecacheFrame	--;
		if (!dwPrecacheFrame)
		{
			load_finished = true;

			if (this->dwFrame % 2)
				m_pRender->UpdateGamma();

			if (precache_light)
			{
				precache_light->set_active(false);
				precache_light.destroy();
			}

			::Sound->set_master_volume(1.f);

			m_pRender->ResourcesDestroyNecessaryTextures();
			Memory.mem_compact();

			//#HACK:
			if (g_pGamePersistent->GameType() == 1)
			{
				WINDOWINFO	wi;
				GetWindowInfo(m_hWnd, &wi);
				if (wi.dwWindowStatus != WS_ACTIVECAPTION)
					Pause(TRUE, TRUE, TRUE, "application start");
			}
		}
	}

	g_bRendering = FALSE;
	// end scene
	//	Present goes here, so call OA Frame end.
	m_pRender->End();

#	ifdef INGAME_EDITOR
		if (load_finished && m_editor)
			m_editor->on_load_finished();
#	endif // #ifdef INGAME_EDITOR

}

volatile u32 mt_Thread_marker = 0x12345678;

void mt_Thread(void *ptr)	
{
    gSecondaryThreadId = GetCurrentThreadId();

	while (true) 
	{
		// waiting for Device permission to execute
		Device.mt_csEnter.Enter	();

		if (Device.mt_bMustExit) 
		{
			Device.mt_bMustExit = FALSE; // Important!!!
			return;
		}
		// we has granted permission to execute
		mt_Thread_marker = Device.dwFrame;
 
		for (xrDelegate<void()> &refParallelDelegate : Device.seqParallel)
			refParallelDelegate();

		Device.seqParallel.clear();
		Device.seqFrameMT.Process(rp_Frame);

		// now we give control to device - signals that we are ended our work
		Device.mt_csEnter.Leave();
		// waits for device signal to continue - to start again
		Device.mt_csLeave.Enter();
		// returns sync signal to device
		Device.mt_csLeave.Leave();
	}
}

#include "igame_level.h"
void CRenderDevice::PreCache(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input)
{
	if (m_pRender->GetForceGPU_REF()) { amount = NULL; }

	dwPrecacheFrame = dwPrecacheTotal = amount;

	if (dwPrecacheFrame)
	{
		if (!precache_light && g_pGameLevel && g_loading_events.empty())
		{
			precache_light = ::Render->light_create();
			precache_light->set_shadow(false);
			precache_light->set_position(vCameraPosition);
			precache_light->set_color(255, 255, 255);
			precache_light->set_range(5.0f);
			precache_light->set_active(true);
		}

		if (b_draw_loadscreen && !load_screen_renderer.b_registered)
		{
			load_screen_renderer.start(b_wait_user_input);
		}
	}
}

ENGINE_API xr_list<LOADING_EVENT> g_loading_events;

void CRenderDevice::on_idle()
{
	if (!b_is_Ready) 
	{
		Sleep(100);
		return; 
	}

	g_bEnableStatGather = psDeviceFlags.test(rsStatistic);

	if (!g_loading_events.empty())
	{
		if (LOADING_EVENT& loadEvent = g_loading_events.front())
		{
			loadEvent();
			g_loading_events.pop_front();
		}
		pApp->LoadDraw();
		return;
	}
	else
	{
		FrameMove();
	}

	// Precache
	if (dwPrecacheFrame)
	{
		float factor = float(dwPrecacheFrame) / float(dwPrecacheTotal);
		float angle = PI_MUL_2 * factor;
		vCameraDirection.set(_sin(angle), 0, _cos(angle));	vCameraDirection.normalize();
		vCameraTop.set(0, 1, 0);
		vCameraRight.crossproduct(vCameraTop, vCameraDirection);
		mView.build_camera_dir(vCameraPosition, vCameraDirection, vCameraTop);
	}

	// Matrices
	mFullTransform.mul(mProject, mView);
	m_pRender->SetCacheXform(mView, mProject);
	D3DXMatrixInverse((D3DXMATRIX*)&mInvFullTransform, nullptr, (D3DXMATRIX*)&mFullTransform);

	vCameraPosition_saved = vCameraPosition;
	mFullTransform_saved = mFullTransform;
	mView_saved = mView;
	mProject_saved = mProject;

	// *** Resume threads
	// Capture end point - thread must run only ONE cycle
	// Release start point - allow thread to run
	mt_csLeave.Enter();
	mt_csEnter.Leave();

	Statistic->RenderTOTAL_Real.FrameStart();
	Statistic->RenderTOTAL_Real.Begin();
	if (b_is_Active)
	{
		if (Begin())
		{
			seqRender.Process(rp_Render);
			if (psDeviceFlags.test(rsCameraPos) || psDeviceFlags.test(rsStatistic)  || 
				psDeviceFlags.test(rsDrawFPS)	|| psDeviceFlags.test(rsHWInfo)		|| 
				!Statistic->errors.empty())
			{
				Statistic->Show();
			}

			//	Present goes here
			Device.Statistic->Render_End.FrameStart();
			End();
			Device.Statistic->Render_End.FrameEnd();
		}
	}
	Statistic->RenderTOTAL_Real.End();
	Statistic->RenderTOTAL_Real.FrameEnd();
	Statistic->RenderTOTAL.accum = Statistic->RenderTOTAL_Real.accum;

	// *** Suspend threads
	// Capture startup point
	// Release end point - allow thread to wait for startup point
	mt_csEnter.Enter();
	mt_csLeave.Leave();

	// Ensure, that second thread gets chance to execute anyway
	if (dwFrame != mt_Thread_marker)
	{
		for (xrDelegate<void()>& refParallelDelegate : Device.seqParallel)
			refParallelDelegate();

		Device.seqParallel.clear();
		seqFrameMT.Process(rp_Frame);
	}

	if (!b_is_Active) 
	{ 
		Sleep(1); 
	}
}

void CRenderDevice::ResizeProc(DWORD height, DWORD  width)
{
	string128 buf = {0};
	MONITORINFO mi = { 0 };
	mi.cbSize = sizeof(MONITORINFO);
	long monitor_width = 0;
	long monitor_height = 0;
	HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);

	if (GetMonitorInfo(hMonitor, &mi))
	{
		monitor_width = mi.rcMonitor.right - mi.rcMonitor.left;
		monitor_height = mi.rcMonitor.bottom - mi.rcMonitor.top;

		if ((DWORD)monitor_width >= width && (DWORD)monitor_height >= height)
		{
			xr_sprintf(buf, "%s%d%s%d", "vid_mode ", width, "x", height);

			Console->Execute(buf);
			m_pRender->Reset(m_hWnd, dwWidth, dwHeight, fWidth_2, fHeight_2);
		}
	}
	else
	{
		DWORD dwError = GetLastError();
		R_CHK(dwError);
	}
}

#ifdef INGAME_EDITOR
void CRenderDevice::message_loop_editor	()
{
	m_editor->run();
	m_editor_finalize		(m_editor);
//	xr_delete				(m_engine);
}
#endif

void CRenderDevice::message_loop()
{
#ifdef INGAME_EDITOR
	if (editor()) 
	{
		message_loop_editor	();
		return;
	}
#endif

	MSG		msg;
    PeekMessage				(&msg, nullptr, 0U, 0U, PM_NOREMOVE );
	while (msg.message != WM_QUIT) 
	{
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage	(&msg);
			continue;
		}

		on_idle				();
    }
}

int GetNumOfDisplays()
{
	int sValue			= 0;
	DISPLAY_DEVICE dc;
	dc.cb				= sizeof(dc);
	//////////////////////////////////////////
	for ( int i = 0; EnumDisplayDevicesA(nullptr, i, &dc, 0); ++i )
	{
		if (dc.StateFlags & DISPLAY_DEVICE_ACTIVE)
			sValue++;
	}
	//////////////////////////////////////////
	return sValue;
}

void CRenderDevice::Run			()
{
	BeginToWork();
	message_loop				();

	seqAppEnd.Process		(rp_AppEnd);

	// Stop Balance-Thread
	mt_bMustExit			= TRUE;
	mt_csEnter.Leave();
	while (mt_bMustExit)	
		Sleep(0);
}

void CRenderDevice::BeginToWork()
{
	g_bLoaded = FALSE;
	Log("Starting engine...");

	Msg("Value of system displays: %d.", GetNumOfDisplays());

	string128 primaryThreadName = "X-Ray: Primary thread";
	PlatformUtils.SetCurrentThreadName(primaryThreadName);

	// Startup timers and calculate timer delta
	dwTimeGlobal = 0;
	Timer_MM_Delta = 0;
	{
		u32 time_mm = timeGetTime();
		// wait for next tick
		while (timeGetTime() == time_mm);	 //-V529

		u32 time_system = timeGetTime();
		u32 time_local = TimerAsync();
		Timer_MM_Delta = time_system - time_local;
	}

	// Start all threads
	mt_csEnter.Enter();
	mt_bMustExit = FALSE;
	thread_spawn(mt_Thread, "X-RAY Secondary thread", 0, nullptr);

	// Message cycle
	seqAppStart.Process(rp_AppStart);
	m_pRender->ClearTarget();
}

void CRenderDevice::GetXrWindowRect(RECT& OutWindowRect, bool bClientRect /*= false*/) const
{
	if (m_hWnd != NULL)
	{
		if (bClientRect)
		{
			GetClientRect(m_hWnd, &OutWindowRect);
		}
		else
		{
			GetWindowRect(m_hWnd, &OutWindowRect);
		}
	}
	else
	{
		ZeroMemory(&OutWindowRect, sizeof(RECT));
	}
}

void CRenderDevice::GetXrWindowPoint(POINT& OutWindowPoint) const
{
	if (m_hWnd != NULL)
	{
		ScreenToClient(m_hWnd, &OutWindowPoint);
	}
	else
	{
		ZeroMemory(&OutWindowPoint, sizeof(POINT));
	}
}

u64 CRenderDevice::GetXrWindowStyle() const
{
	if (m_hWnd == NULL)
	{
		return 0;
	}
	return GetWindowLongPtr(m_hWnd, GWL_STYLE);
}

void CRenderDevice::UpdateWindowPropStyle(WindowPropStyle PropStyle)
{
	// Don't drawing wnd when slash is active
	if (!bEngineloaded) return;

    DWORD	dwWindowStyle		= 0;
    DWORD	dwWidthCurr				= psCurrentVidMode[0];
    DWORD	dwHeightCurr			= psCurrentVidMode[1];
    bool	bFullscreen			= psDeviceFlags.is(rsFullscreen);

    RECT	WindowBounds;
    RECT	DesktopRect;
    GetClientRect				(GetDesktopWindow(), &DesktopRect);
    switch (PropStyle)
    {
    case WPS_Windowed:
    {
        psDeviceFlags.set(rsFullscreen, false);
        dwWindowStyle = WS_OVERLAPPEDWINDOW;

        SetRect	(&WindowBounds,
				(DesktopRect.right - dwWidthCurr) / 2,
				(DesktopRect.bottom - dwHeightCurr) / 2,
				(DesktopRect.right + dwWidthCurr) / 2,
				(DesktopRect.bottom + dwHeightCurr) / 2);
    }
        break;
    case WPS_WindowedBorderless:
    {
        psDeviceFlags.set(rsFullscreen, false);
        dwWindowStyle = WS_VISIBLE;

        SetRect	(&WindowBounds,
				(DesktopRect.right - dwWidthCurr) / 2,
				(DesktopRect.bottom - dwHeightCurr) / 2, 
				(DesktopRect.right + dwWidthCurr) / 2,
				(DesktopRect.bottom + dwHeightCurr) / 2);
    }
        break;
    case WPS_FullscreenBorderless:
    {
        psDeviceFlags.set(rsFullscreen, false);

        dwWindowStyle = WS_VISIBLE;
		///////////////////////////////////////
        WindowBounds = DesktopRect;
    }
        break;
    case WPS_Fullscreen:
    {
        //special case
        psDeviceFlags.set(rsFullscreen, true);
        dwWindowStyle = WS_POPUP | WS_VISIBLE;
		WindowBounds = DesktopRect;
    }
        break;
    default:
		Msg("Render error: can't change window mode");
        break;
    }

	if(!strstr(Core.Params, "-editor"))
		SetWindowLongPtr(m_hWnd, GWL_STYLE, dwWindowStyle);

    bool bNewFullscreen = psDeviceFlags.is(rsFullscreen);

    if (!bNewFullscreen)
    {
        AdjustWindowRect(&WindowBounds, dwWindowStyle, FALSE);
		SetWindowPos(m_hWnd, HWND_NOTOPMOST,
					 WindowBounds.left, WindowBounds.top,
					 (WindowBounds.right - WindowBounds.left),
					 (WindowBounds.bottom - WindowBounds.top),
					 SWP_SHOWWINDOW | SWP_NOCOPYBITS | SWP_DRAWFRAME);
    }

    if (Device.b_is_Ready && bFullscreen != bNewFullscreen)
    {
        Reset();
    }
    else
    {
		SetForegroundWindow(m_hWnd);
    }
}

u32 app_inactive_time		= 0;
u32 app_inactive_time_start = 0;

void ProcessLoading(RP_FUNC *f);
void CRenderDevice::FrameMove()
{
	dwFrame			++;

	Core.dwFrame = dwFrame;

	dwTimeContinual	= TimerMM.GetElapsed_ms() - app_inactive_time;

	if (psDeviceFlags.test(rsConstantFPS))	{
		// 20ms = 50fps
		// 33ms = 30fps

		fTimeDelta		=	0.033f;
        Statistic->fRawFrameDeltaTime = fTimeDelta;
		fTimeGlobal		+=	0.033f;
		dwTimeDelta		=	33;
		dwTimeGlobal	+=	33;
	} 
	else 
	{
		// Timer
		float fPreviousFrameTime = Timer.GetElapsed_sec(); Timer.Start();	// previous frame
		fTimeDelta = 0.1f * fTimeDelta + 0.9f*fPreviousFrameTime;			// smooth random system activity - worst case ~7% error
        Statistic->fRawFrameDeltaTime = fTimeDelta;                         // copy unmodified fTimeDelta, for statistic purpose
		if (fTimeDelta>.1f)    
			fTimeDelta = .1f;							// limit to 15fps minimum

		if (fTimeDelta <= 0.f) 
			fTimeDelta = EPS_S + EPS_S;					// limit to 15fps minimum

		if(Paused())	
			fTimeDelta = 0.0f;

		fTimeGlobal		= TimerGlobal.GetElapsed_sec(); 
		u32	_old_global	= dwTimeGlobal;
		dwTimeGlobal = TimerGlobal.GetElapsed_ms();
		dwTimeDelta		= dwTimeGlobal-_old_global;
	}

	Statistic->EngineTOTAL.Begin	();
		ProcessLoading				(rp_Frame);
	Statistic->EngineTOTAL.End	();
}

void ProcessLoading				(RP_FUNC *f)
{
	Device.seqFrame.Process				(rp_Frame);
	g_bLoaded							= TRUE;
}

ENGINE_API BOOL bShowPauseString = TRUE;
#include "IGame_Persistent.h"

void CRenderDevice::Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason)
{
#ifdef DEBUG
    Msg("* [MSG] PAUSE bOn[%s], bTimer[%s], bSound[%s], reason: %s", bOn ? "true" : "false", bTimer ? "true" : "false", bSound ? "true" : "false", reason);
#endif
	static int snd_emitters_ = -1;

	if (g_bBenchmark)	return;

	if(bOn)
	{
		if(!Paused())						
			bShowPauseString				= 
#ifdef INGAME_EDITOR
				editor() ? FALSE : 
#endif 
#ifdef DEBUG
				!xr_strcmp(reason, "li_pause_key_no_clip")?	FALSE:
#endif
				TRUE;

		if( bTimer && (!g_pGamePersistent || g_pGamePersistent->CanBePaused()) )
		{
			g_pauseMngr.Pause				(true);
#ifdef DEBUG
			if(!xr_strcmp(reason, "li_pause_key_no_clip"))
				TimerGlobal.Pause				(FALSE);
#endif 
		}
	
		if (bSound && ::Sound) {
			snd_emitters_ =					::Sound->pause_emitters(true);
		}
	}else
	{
		if( bTimer && g_pauseMngr.Paused() )
		{
			fTimeDelta						= EPS_S + EPS_S;
			g_pauseMngr.Pause				(false);
		}
		
		if(bSound)
		{
			if(snd_emitters_>0) // avoid crash
			{
				snd_emitters_ =				::Sound->pause_emitters(false);
#ifdef DEBUG
//				Log("snd_emitters_[false]",snd_emitters_);
#endif // DEBUG
			}else {
#ifdef DEBUG
				Log("Sound->pause_emitters underflow");
#endif // DEBUG
			}
		}
	}
}

BOOL CRenderDevice::Paused()
{
	return g_pauseMngr.Paused();
};

void CRenderDevice::ProcessSingleMessage()
{
	MSG		msg;
	if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CRenderDevice::OnWM_Activate(WPARAM wParam, LPARAM lParam)
{
	u16 fActive						= LOWORD(wParam);
	extern int ps_always_active;
	const BOOL fMinimized = (BOOL)HIWORD(wParam);
	const BOOL isWndActive = (fActive != WA_INACTIVE && !fMinimized) ? TRUE : FALSE;
	const BOOL isGameActive = ps_always_active || isWndActive;

	if (isGameActive != Device.b_is_Active)
	{
		Device.b_is_Active			= isGameActive;

		if (Device.b_is_Active)	
		{
			Device.seqAppActivate.Process(rp_AppActivate);
			app_inactive_time		+= TimerMM.GetElapsed_ms() - app_inactive_time_start;

		}
		else	
		{
			app_inactive_time_start	= TimerMM.GetElapsed_ms();
			Device.seqAppDeactivate.Process(rp_AppDeactivate);
		}
	}
}

void CRenderDevice::AddSeqFrame			( pureFrame* f, bool mt )
{
		if ( mt )	
		seqFrameMT.Add	(f,REG_PRIORITY_HIGH);
	else								
		seqFrame.Add		(f,REG_PRIORITY_LOW);

}

void CRenderDevice::RemoveSeqFrame	( pureFrame* f )
{
	seqFrameMT.Remove	( f );
	seqFrame.Remove		( f );
}

CLoadScreenRenderer::CLoadScreenRenderer() : b_registered(false)
{
}

void CLoadScreenRenderer::start(bool b_user_input) 
{
	Device.seqRender.Add			(this, 0);
	b_registered					= true;
	b_need_user_input				= b_user_input;
}

void CLoadScreenRenderer::stop()
{
	if(!b_registered)				return;
	Device.seqRender.Remove			(this);
	pApp->DestroyLoadingScreen		();
	b_registered					= false;
	b_need_user_input				= false;
}

void CLoadScreenRenderer::OnRender() 
{
	pApp->load_draw_internal();
}

void CRenderDevice::CSecondVPParams::SetSVPActive(bool bState) //--#SM+#-- +SecondVP+
{
	m_bIsActive = bState;
	if (g_pGamePersistent != nullptr)
		 g_pGamePersistent->m_pGShaderConstants.m_blender_mode.z = (m_bIsActive ? 1.0f : 0.0f);
}

bool CRenderDevice::CSecondVPParams::IsSVPFrame() //--#SM+#-- +SecondVP+
{
	return IsSVPActive() && ((Device.dwFrame % m_FrameDelay) == 0);
}
