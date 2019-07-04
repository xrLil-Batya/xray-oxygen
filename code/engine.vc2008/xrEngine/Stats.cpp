#include "stdafx.h"
#include "GameFont.h"
#pragma hdrstop

#include "../xrcdb/ISpatial.h"
#include "IGame_Persistent.h"
#include "IGame_Level.h"
#include "render.h"
#include "xr_object.h"
#include "psapi.h"
#include <Wbemidl.h>
#include <comdef.h>
#include <timeapi.h>
#include "NvGPUTransferee.h"
#include "AMDGPUTransferee.h"

#include "../Include/xrRender/DrawUtils.h"

int		g_ErrorLineCount	= 15;
Flags32 g_stats_flags		= {0};

// stats
DECLARE_RP(Stats);

//#GIPERION: Perhaps we need a general header with most used colors for all systems, not only debug?
//Format: Alpha, Red, Green, Blue
enum DebugTextColor : DWORD
{
    DTC_RED = 0xFFF0672B,
    DTC_YELLOW = 0xFFF6D434,
    DTC_GREEN = 0xFF67F92E,
    DTC_BLUE = 0xFF0000FF,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL g_bDisableRedText	= FALSE;
CStats::CStats	()
{
	fFPS				= 30.f;
	fRFPS				= 30.f;
    fLastDisplayedFPS   = 30.f;
	fTPS				= 0;
	pFont				= 0;
	fMem_calls			= 0;
	RenderDUMP_DT_Count = 0;
	Device.seqRender.Add		(this,REG_PRIORITY_LOW-1000);
}

CStats::~CStats()
{
	Device.seqRender.Remove		(this);
	xr_delete		(pFont);
}

void _draw_cam_pos(CGameFont* pFont)
{
	float sz		= pFont->GetHeight();
	pFont->SetHeightI(0.02f);
	pFont->SetColor	(0xffffffff);
	pFont->Out		(10, 600, "CAMERA POSITION:  [%3.2f,%3.2f,%3.2f]",VPUSH(Device.vCameraPosition));
	pFont->SetHeight(sz);
}

void CStats::Show() 
{
	// Stop timers
	{
		EngineTOTAL.FrameEnd		();

		// Giperion Engine specific timers
		Engine_RenderFrame.FrameEnd();
		Engine_ApplicationFrame.FrameEnd();
		Engine_InputFrame.FrameEnd();
		Engine_MenuFrame.FrameEnd();
		Engine_PersistanceFrame.FrameEnd();
		Engine_PersistanceFrame_Begin.FrameEnd();
		Engine_PersistanceFrame_EnvAndSpectre.FrameEnd();
		Engine_PersistanceFrame_ParticlePlay.FrameEnd();
		Engine_PersistanceFrame_ParticleDestroy.FrameEnd();
		Engine_PersistanceFrame_Scheduler.FrameEnd();
		Engine_PersistanceFrame_WeatherAndDOF.FrameEnd();

		Physics.FrameEnd			();	
		ph_collision.FrameEnd		();
		ph_core.FrameEnd			();
		Animation.FrameEnd			();	
		AI_Think.FrameEnd			();
		AI_Range.FrameEnd			();
		AI_Path.FrameEnd			();
		AI_Node.FrameEnd			();
		AI_Vis.FrameEnd				();
		AI_Vis_Query.FrameEnd		();
		AI_Vis_RayTests.FrameEnd	();
		
		RenderTOTAL.FrameEnd		();
		Render_CRenderRender_render_main.FrameEnd			();
		RenderCALC_HOM.FrameEnd		();
		RenderDUMP.FrameEnd			();	
		RenderDUMP_RT.FrameEnd		();
		RenderDUMP_SKIN.FrameEnd	();	
		RenderDUMP_Wait.FrameEnd	();	
		Render_CRenderRender_WaitForFrame.FrameEnd	();	
		RenderDUMP_HUD.FrameEnd		();	
		RenderDUMP_Glows.FrameEnd	();	
		RenderDUMP_Lights.FrameEnd	();	
		RenderDUMP_WM.FrameEnd		();	
		RenderDUMP_DT_VIS.FrameEnd	();	
		RenderDUMP_DT_Render.FrameEnd();	
		RenderDUMP_DT_Cache.FrameEnd();
		RenderDUMP_Pcalc.FrameEnd	();	
		RenderDUMP_Scalc.FrameEnd	();	
		RenderDUMP_Srender.FrameEnd	();	

		// Giperion Render specific Frame end section
		Render_CRenderRender.FrameEnd();
		Render_CRenderRender_MainMenu.FrameEnd();
		Render_CRenderRender_ScenePrepare.FrameEnd();
		Render_CRenderRender_DeferPart0.FrameEnd();
		Render_CRenderRender_LightVisibility.FrameEnd();
		Render_CRenderRender_ItemUIWallmarks.FrameEnd();
		Render_CRenderRender_FlushOcclusion.FrameEnd();
		Render_CRenderRender_MSAA_Rain.FrameEnd();
		Render_CRenderRender_Sun.FrameEnd();
		Render_CRenderRender_LightRender.FrameEnd();
		Render_CRenderRender_Combine.FrameEnd();
		Render_Combine_Begin.FrameEnd();
		Render_Combine_Sky.FrameEnd();
		Render_Combine_Cloud.FrameEnd();
		Render_Combine_Combine1.FrameEnd();
		Render_Combine_ForwardRendering.FrameEnd();
		
		Sound.FrameEnd				();
		Input.FrameEnd				();
		clRAY.FrameEnd				();	
		clBOX.FrameEnd				();
		clFRUSTUM.FrameEnd			();
		
		TEST0.FrameEnd				();
		TEST1.FrameEnd				();
		TEST2.FrameEnd				();
		TEST3.FrameEnd				();

		g_SpatialSpace->stat_insert.FrameEnd		();
		g_SpatialSpace->stat_remove.FrameEnd		();
		g_SpatialSpacePhysic->stat_insert.FrameEnd	();
		g_SpatialSpacePhysic->stat_remove.FrameEnd	();
	}

	// calc FPS & TPS
	if (fRawFrameDeltaTime > EPS_S) {
		float fps  = 1.f / fRawFrameDeltaTime;
		float fOne = 0.3f;
		float fInv = 1.f-fOne;
		fFPS = fInv*fFPS + fOne*fps;

		if (RenderTOTAL.result>EPS_S) {
			u32	rendered_polies = Device.m_pRender->GetCacheStatPolys();
			fTPS = fInv*fTPS + fOne*float(rendered_polies)/(RenderTOTAL.result*1000.f);
			//fTPS = fInv*fTPS + fOne*float(RCache.stat.polys)/(RenderTOTAL.result*1000.f);
			fRFPS= fInv*fRFPS+ fOne*1000.f/RenderTOTAL.result;
		}
	}
	{
		float mem_count		= float	(Memory.stat_calls);
		if (mem_count>fMem_calls)	fMem_calls	=	mem_count;
		else						fMem_calls	=	.9f*fMem_calls + .1f*mem_count;
		Memory.stat_calls	= 0		;
	}

	////////////////////////////////////////////////
	int frm = 2000;
	div_t ddd = div(Device.dwFrame,frm);
	if( ddd.rem < frm/2.0f ){
		pFont->SetColor	(0xFFFFFFFF	);
		pFont->OutSet	(0,0);
		pFont->OnRender	();
	}

	CGameFont& F = *pFont;
	float		f_base_size	= 0.01f;
				F.SetHeightI	(f_base_size);

	// Show them
	if (psDeviceFlags.test(rsStatistic))
	{
		static float	r_ps		= 0;
		static float	b_ps		= 0;
		r_ps						= .99f*r_ps + .01f*(clRAY.count/clRAY.result);
		b_ps						= .99f*b_ps + .01f*(clBOX.count/clBOX.result);

		CSound_stats				snd_stat;
		::Sound->statistic			(&snd_stat,0);
		F.SetColor	(0xFFFFFFFF	);

		F.OutSet	(0,0);
		F.OutNext	("FPS/RFPS:    %3.1f/%3.1f",fFPS,fRFPS);
		F.OutNext	("TPS:         %2.2f M",	fTPS);
		m_pRender->OutData1(F);
#ifdef DEBUG
		F.OutSkip	();
#ifdef FS_DEBUG
		F.OutNext	("mapped:      %d",			g_file_mapped_memory);
		F.OutSkip	();
#endif
		m_pRender->OutData2(F);
#endif
		m_pRender->OutData3(F);
		F.OutSkip	();

#define GetPercentOf(local, global) (100.0f * float(local) / float (global.result))

		F.OutNext	("*** ENGINE:  %2.2fms",EngineTOTAL.result);
		F.OutNext	(" -> Render      : %2.2fms, %2.1f%%", Engine_RenderFrame.result,				GetPercentOf(Engine_RenderFrame.result, EngineTOTAL));
		F.OutNext	(" -> Application : %2.2fms, %2.1f%%", Engine_ApplicationFrame.result,			GetPercentOf(Engine_ApplicationFrame.result, EngineTOTAL));
		F.OutNext	(" -> Input       : %2.2fms, %2.1f%%", Engine_InputFrame.result,				GetPercentOf(Engine_InputFrame.result, EngineTOTAL));
		F.OutNext	(" -> Menu        : %2.2fms, %2.1f%%", Engine_MenuFrame.result,					GetPercentOf(Engine_MenuFrame.result, EngineTOTAL));
		F.OutNext	(" -> Persistence : %2.2fms, %2.1f%%", Engine_PersistanceFrame.result,			GetPercentOf(Engine_PersistanceFrame.result, EngineTOTAL));
		F.OutNext	(" -> -> Begin                : %2.2fms, %2.1f%%", Engine_PersistanceFrame_Begin.result,		   GetPercentOf(Engine_PersistanceFrame_Begin.result, Engine_PersistanceFrame));
		F.OutNext	(" -> -> Environment & Spectre: %2.2fms, %2.1f%%", Engine_PersistanceFrame_EnvAndSpectre.result,   GetPercentOf(Engine_PersistanceFrame_EnvAndSpectre.result, Engine_PersistanceFrame));
		F.OutNext	(" -> -> Playing Particles    : %2.2fms, %2.1f%%", Engine_PersistanceFrame_ParticlePlay.result,    GetPercentOf(Engine_PersistanceFrame_ParticlePlay.result, Engine_PersistanceFrame));
		F.OutNext	(" -> -> Destroying Particles : %2.2fms, %2.1f%%", Engine_PersistanceFrame_ParticleDestroy.result, GetPercentOf(Engine_PersistanceFrame_ParticleDestroy.result, Engine_PersistanceFrame));
		F.OutNext	(" -> -> Scheduler            : %2.2fms, %2.1f%%", Engine_PersistanceFrame_Scheduler.result,	   GetPercentOf(Engine_PersistanceFrame_Scheduler.result, Engine_PersistanceFrame));
		F.OutNext	(" -> -> Weather and DOF      : %2.2fms, %2.1f%%", Engine_PersistanceFrame_WeatherAndDOF.result,   GetPercentOf(Engine_PersistanceFrame_WeatherAndDOF.result, Engine_PersistanceFrame));
		F.OutSkip();

		F.OutNext	("Memory:      %2.2fa",fMem_calls);
		F.OutNext	("uParticles:  Qstart[%d] Qactive[%d] Qdestroy[%d]",	Particles_starting,Particles_active,Particles_destroy);
		F.OutNext	("spInsert:    o[%.2fms, %2.1f%%], p[%.2fms, %2.1f%%]",	
			g_SpatialSpace->stat_insert.result, 
			GetPercentOf(g_SpatialSpace->stat_insert.result, EngineTOTAL),	
			g_SpatialSpacePhysic->stat_insert.result, 
			GetPercentOf(g_SpatialSpacePhysic->stat_insert.result, EngineTOTAL));
		F.OutNext	("spRemove:    o[%.2fms, %2.1f%%], p[%.2fms, %2.1f%%]",	
			g_SpatialSpace->stat_remove.result, 
			GetPercentOf(g_SpatialSpace->stat_remove.result, EngineTOTAL),	
			g_SpatialSpacePhysic->stat_remove.result, 
			GetPercentOf(g_SpatialSpacePhysic->stat_remove.result, EngineTOTAL));
		F.OutNext	("Physics:     %2.2fms, %2.1f%%",Physics.result, GetPercentOf(Physics.result, EngineTOTAL));
		F.OutNext	("  collider:  %2.2fms", ph_collision.result);	
		F.OutNext	("  solver:    %2.2fms, %d",ph_core.result,ph_core.count);	
		F.OutNext	("aiThink:     %2.2fms, %d",AI_Think.result,AI_Think.count);	
		F.OutNext	("  aiRange:   %2.2fms, %d",AI_Range.result,AI_Range.count);
		F.OutNext	("  aiPath:    %2.2fms, %d",AI_Path.result,AI_Path.count);
		F.OutNext	("  aiNode:    %2.2fms, %d",AI_Node.result,AI_Node.count);
		F.OutNext	("aiVision:    %2.2fms, %d",AI_Vis.result,AI_Vis.count);
		F.OutNext	("  Query:     %2.2fms",	AI_Vis_Query.result);
		F.OutNext	("  RayCast:   %2.2fms",	AI_Vis_RayTests.result);
		F.OutSkip	();
								   
        F.OutNext	("*** RENDER:  %2.2fms",RenderTOTAL.result);
        F.OutNext	(" -> CRender::Render()     : %2.2fms, %2.1f%%",	Render_CRenderRender.result,			    GetPercentOf(Render_CRenderRender.result, RenderTOTAL));
        F.OutNext	(" -> -> CRender::render_menu(): %2.2fms, %2.1f%%",	Render_CRenderRender_MainMenu.result,       GetPercentOf(Render_CRenderRender_MainMenu.result, RenderTOTAL));
        F.OutNext	(" -> -> Scene prepare         : %2.2fms, %2.1f%%",	Render_CRenderRender_WaitForFrame.result,   GetPercentOf(Render_CRenderRender_WaitForFrame.result, RenderTOTAL));
        F.OutNext	(" -> -> Wait for frame        : %2.2fms, %2.1f%%",	Render_CRenderRender_ScenePrepare.result,   GetPercentOf(Render_CRenderRender_ScenePrepare.result, RenderTOTAL));
        F.OutNext	(" -> -> Occlusion, renderables: %2.2fms, %2.1f%%",	Render_CRenderRender_render_main.result,    GetPercentOf(Render_CRenderRender_render_main.result, RenderTOTAL));
        F.OutNext	(" -> -> DEFER_PART0           : %2.2fms, %2.1f%%",	Render_CRenderRender_DeferPart0.result,     GetPercentOf(Render_CRenderRender_DeferPart0.result, RenderTOTAL));
        F.OutNext	(" -> -> Lights visibility     : %2.2fms, %2.1f%%",	Render_CRenderRender_LightVisibility.result,GetPercentOf(Render_CRenderRender_LightVisibility.result, RenderTOTAL));
        F.OutNext	(" -> -> Item UI and Wallmarks : %2.2fms, %2.1f%%",	Render_CRenderRender_ItemUIWallmarks.result,GetPercentOf(Render_CRenderRender_ItemUIWallmarks.result, RenderTOTAL));
        F.OutNext	(" -> -> Light flush occlussion: %2.2fms, %2.1f%%",	Render_CRenderRender_FlushOcclusion.result, GetPercentOf(Render_CRenderRender_FlushOcclusion.result, RenderTOTAL));
        F.OutNext	(" -> -> MSAA and Rain FX(DX11): %2.2fms, %2.1f%%",	Render_CRenderRender_MSAA_Rain.result,		GetPercentOf(Render_CRenderRender_MSAA_Rain.result, RenderTOTAL));
        F.OutNext	(" -> -> Sun                   : %2.2fms, %2.1f%%",	Render_CRenderRender_Sun.result,			GetPercentOf(Render_CRenderRender_Sun.result, RenderTOTAL));
        F.OutNext	(" -> -> Lights rendering      : %2.2fms, %2.1f%%",	Render_CRenderRender_LightRender.result,	GetPercentOf(Render_CRenderRender_LightRender.result, RenderTOTAL));
        F.OutNext	(" -> -> PostProcess, combine  : %2.2fms, %2.1f%%",	Render_CRenderRender_Combine.result,		GetPercentOf(Render_CRenderRender_Combine.result, RenderTOTAL));
        F.OutNext	(" -> -> -> Begin              : %2.2fms, %2.1f%%", Render_Combine_Begin.result,				GetPercentOf(Render_Combine_Begin.result, Render_CRenderRender_Combine));
        F.OutNext	(" -> -> -> Sky                : %2.2fms, %2.1f%%", Render_Combine_Sky.result,					GetPercentOf(Render_Combine_Sky.result, Render_CRenderRender_Combine));
        F.OutNext	(" -> -> -> Cloud              : %2.2fms, %2.1f%%", Render_Combine_Cloud.result,				GetPercentOf(Render_Combine_Cloud.result, Render_CRenderRender_Combine));
        F.OutNext	(" -> -> -> Combine1           : %2.2fms, %2.1f%%", Render_Combine_Combine1.result,				GetPercentOf(Render_Combine_Combine1.result, Render_CRenderRender_Combine));
        F.OutNext	(" -> -> -> Forward Render     : %2.2fms, %2.1f%%", Render_Combine_ForwardRendering.result,		GetPercentOf(Render_Combine_ForwardRendering.result, Render_CRenderRender_Combine));
        F.OutNext	(" -> Presenter	            : %2.2fms, %2.1f%%",	Render_End.result,							GetPercentOf(Render_End.result, RenderTOTAL));

        float UnprofiledPartsOfRender = RenderTOTAL.result - Render_CRenderRender.result - Render_End.result;
        F.OutNext	(" -> Other                 : %2.2fms, %2.1f%%", UnprofiledPartsOfRender, GetPercentOf(UnprofiledPartsOfRender, RenderTOTAL));
		F.OutSkip	();

		F.OutNext	("  HOM:       %2.2fms, %d", RenderCALC_HOM.result, RenderCALC_HOM.count);

#if 0
		F.OutNext	("  Skeletons: %2.2fms, %d",Animation.result,		Animation.count);
		F.OutNext	("R_DUMP:      %2.2fms, %2.1f%%",RenderDUMP.result, GetPercentOf(RenderDUMP.result, RenderTOTAL));
		F.OutNext	("  Wait-L:    %2.2fms",RenderDUMP_Wait.result);	
		F.OutNext	("  Wait-S:    %2.2fms", Render_CRenderRender_WaitForFrame.result);
		F.OutNext	("  Skinning:  %2.2fms",RenderDUMP_SKIN.result);	
		F.OutNext	("  DT_Vis/Cnt:%2.2fms/%d",RenderDUMP_DT_VIS.result,RenderDUMP_DT_Count);	
		F.OutNext	("  DT_Render: %2.2fms",RenderDUMP_DT_Render.result);	
		F.OutNext	("  DT_Cache:  %2.2fms",RenderDUMP_DT_Cache.result);	
		F.OutNext	("  Wallmarks: %2.2fms, %d/%d - %d",RenderDUMP_WM.result,RenderDUMP_WMS_Count,RenderDUMP_WMD_Count,RenderDUMP_WMT_Count);
		F.OutNext	("  Glows:     %2.2fms",RenderDUMP_Glows.result);	
		F.OutNext	("  Lights:    %2.2fms, %d",RenderDUMP_Lights.result,RenderDUMP_Lights.count);
		F.OutNext	("  RT:        %2.2fms, %d",RenderDUMP_RT.result,RenderDUMP_RT.count);
		F.OutNext	("  HUD:       %2.2fms",RenderDUMP_HUD.result);	
		F.OutNext	("  P_calc:    %2.2fms",RenderDUMP_Pcalc.result);
		F.OutNext	("  S_calc:    %2.2fms",RenderDUMP_Scalc.result);
		F.OutNext	("  S_render:  %2.2fms, %d",RenderDUMP_Srender.result,RenderDUMP_Srender.count);
#endif
		F.OutSkip	();


		F.OutNext	("*** SOUND:   %2.2fms",Sound.result);
		F.OutNext	("  TGT/SIM/E: %d/%d/%d",  snd_stat._rendered, snd_stat._simulated, snd_stat._events);
		F.OutNext	("  HIT/MISS:  %d/%d",  snd_stat._cache_hits, snd_stat._cache_misses);
		F.OutSkip	();
		F.OutNext	("Input:       %2.2fms",Input.result);
		F.OutNext	("clRAY:       %2.2fms, %d, %2.0fK",clRAY.result,		clRAY.count,r_ps);
		F.OutNext	("clBOX:       %2.2fms, %d, %2.0fK",clBOX.result,		clBOX.count,b_ps);
		F.OutNext	("clFRUSTUM:   %2.2fms, %d",		clFRUSTUM.result,	clFRUSTUM.count	);
		F.OutSkip	();
		
		F.OutSkip	();

		F.OutSkip	();
		F.OutNext	("TEST 0:      %2.2fms, %d",TEST0.result,TEST0.count);
		F.OutNext	("TEST 1:      %2.2fms, %d",TEST1.result,TEST1.count);
		F.OutNext	("TEST 2:      %2.2fms, %d",TEST2.result,TEST2.count);
		F.OutNext	("TEST 3:      %2.2fms, %d",TEST3.result,TEST3.count);

		F.OutSkip	();
		F.OutNext	("qpc[%3d]",CPU::qpc_counter);
		CPU::qpc_counter	=	0		;
		F.OutSkip	();
		m_pRender->OutData4(F);
		//////////////////////////////////////////////////////////////////////////
		// Renderer specific
		F.SetHeightI						(f_base_size);
		F.OutSet						(200,0);
		Render->Statistics				(&F);

		//////////////////////////////////////////////////////////////////////////
		// Game specific
		F.SetHeightI						(f_base_size);
		F.OutSet						(400,0);
		g_pGamePersistent->Statistics	(&F);

		//////////////////////////////////////////////////////////////////////////
		// process PURE STATS
		F.SetHeightI						(f_base_size);
		seqStats.Process				(rp_Stats);
		pFont->OnRender					();

		//////////////////////////////////////////////////////////////////////////
		// Environment stats
		if (psDeviceFlags.test(rsEnviromentInfo))
		{
			if (g_pGameLevel && g_pGameLevel->bReady)
			{
				F.SetHeightI(f_base_size);
				F.OutSet(800, 0);
				Environment().Statistics(&F);
			}
		}
	};

	if( psDeviceFlags.test(rsDrawFPS) )
    {
        //On every 25 frame, update last known fps
        if ((Core.dwFrame % 25) == 0)
        {
            fLastDisplayedFPS = fFPS;
        }
        float sz = pFont->GetHeight();
        pFont->SetHeightI(0.018f);

        if (fLastDisplayedFPS > 50.0f)			pFont->SetColor(DebugTextColor::DTC_GREEN);
        else if (fLastDisplayedFPS > 30.0f)		pFont->SetColor(DebugTextColor::DTC_YELLOW);
        else									pFont->SetColor(DebugTextColor::DTC_RED);
        
        const char* FPSFormat = "FPS: %0.0f";
        //If game paused, engine not updating deltaTime variable, so FPS variable is freezed to last value
        if (Device.Paused())
        {
            FPSFormat = "LAST KNOWN FPS: %0.0f";
        }

        pFont->Out(10, 10, FPSFormat, fLastDisplayedFPS);
        pFont->SetHeight(sz);
		pFont->OnRender					();
	}

	if (psDeviceFlags.test(rsHWInfo))
	{
        if ((Core.dwFrame % 25) == 0)
        {
			if (CNvReader::bSupport)
				NVGPULoad = NvData.GetPercentActive();

			if (CAMDReader::bAMDSupportADL)
				AMDGPULoad = AMDData.GetPercentActive();

		    // init all variables
		    MEMORYSTATUSEX mem;
		    PROCESS_MEMORY_COUNTERS_EX pmc;
		    SYSTEM_INFO sysInfo;
		
		    // Getting info about memory
		    mem.dwLength = sizeof(MEMORYSTATUSEX);
		    GlobalMemoryStatusEx((&mem));
	
		    AvailableMem = (float)mem.ullAvailPhys;	// how much phys mem available
		    AvailableMem /= (1024 * 1024);	
		    AvailablePageFileMem = (float)mem.ullAvailPageFile;	// how much pagefile mem available
		    AvailablePageFileMem /= (1024 * 1024);

		    // Getting info by request
		    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(PROCESS_MEMORY_COUNTERS_EX));
		    GetSystemInfo(&sysInfo);
	
			PhysMemoryUsedPercent = (float)mem.dwMemoryLoad;
			PageFileMemUsedByApp = (float)pmc.PagefileUsage;
		    PageFileMemUsedByApp /= (1024 * 1024);

		    // Counting CPU load
            CPU::Info.getCPULoad(cpuLoad);
            cpuBefore = cpuLoad;

			CPU::Info.MTCPULoad();
        }

        pFont->SetHeightI(0.018f);

        if (AvailableMem < 512 || AvailablePageFileMem < 1596)
            pFont->SetColor(DebugTextColor::DTC_RED);
        else if (AvailableMem < 768 || AvailablePageFileMem < 2048)
            pFont->SetColor(DebugTextColor::DTC_YELLOW);
        else
            pFont->SetColor(DebugTextColor::DTC_GREEN);

        // Draw all your stuff
        pFont->Out(10, 25, "MEM_AVAILABLE: %0.0fMB", AvailableMem);				// Physical memory available
        pFont->Out(10, 40, "PAGE_AVAILABLE: %0.0fMB", AvailablePageFileMem);	// Pagefile memory available
        pFont->Out(10, 55, "PAGE_APPUSED: %0.0fMB", PageFileMemUsedByApp);		// Physical memory used by app

        if (cpuLoad > 80.0 || PhysMemoryUsedPercent > 80.0)			pFont->SetColor(DebugTextColor::DTC_RED);
        else if (cpuLoad > 60.0 || PhysMemoryUsedPercent > 60.0)	pFont->SetColor(DebugTextColor::DTC_YELLOW);
        else														pFont->SetColor(DebugTextColor::DTC_GREEN);

		pFont->Out(10, 70, "CPU_LOAD: %0.0f", cpuLoad);							// CPU load
		pFont->Out(10, 85, "MEM_USED: %0.0f", PhysMemoryUsedPercent);			// Total Phys. memory load (%)

		// get MT Load
		float dwScale = 100;
		for (size_t i = 0; i < CPU::Info.m_dwNumberOfProcessors; i++)
		{
			if (CPU::Info.fUsage[i] >= 60) pFont->SetColor(DebugTextColor::DTC_RED);
			pFont->Out(10, dwScale, "CPU%d: %0.0f", i, CPU::Info.fUsage[i]);
			dwScale += 15;
		}

		if (CAMDReader::bAMDSupportADL)
		{
			pFont->SetColor(DebugTextColor::DTC_BLUE);
			pFont->Out(10, dwScale, "GPU Used: %d", AMDGPULoad);
		}
		else if(CNvReader::bSupport)
		{
			pFont->SetColor(DebugTextColor::DTC_BLUE);
			pFont->Out(10, dwScale, "GPU Used: %d", NVGPULoad);
		}
        pFont->OnRender();
	}
	
	if( psDeviceFlags.test(rsCameraPos) ){
		_draw_cam_pos					(pFont);
		pFont->OnRender					();
	};
#ifdef DEBUG
	//////////////////////////////////////////////////////////////////////////
	// Show errors
	if (!g_bDisableRedText)
	{
		xrCriticalSectionGuard guard(errorsGuard);
		if (!errors.empty())
		{
			F.SetColor(color_rgba(255, 16, 16, 191));
			F.OutSet(200, 0);
			F.SetHeightI(f_base_size);
			for (u32 it = (u32)std::max(int(0), (int)errors.size() - g_ErrorLineCount); it < errors.size(); it++)
				F.OutNext("%s", errors[it].c_str());
			F.OnRender();
		}
	}
#endif

	{
		EngineTOTAL.FrameStart		();	

		// Giperion Engine specific timers
		Engine_RenderFrame.FrameStart();
		Engine_ApplicationFrame.FrameStart();
		Engine_InputFrame.FrameStart();
		Engine_MenuFrame.FrameStart();
		Engine_PersistanceFrame.FrameStart();
		Engine_PersistanceFrame_Begin.FrameStart();
		Engine_PersistanceFrame_EnvAndSpectre.FrameStart();
		Engine_PersistanceFrame_ParticlePlay.FrameStart();
		Engine_PersistanceFrame_ParticleDestroy.FrameStart();
		Engine_PersistanceFrame_Scheduler.FrameStart();
		Engine_PersistanceFrame_WeatherAndDOF.FrameStart();

		Physics.FrameStart			();	
		ph_collision.FrameStart		();
		ph_core.FrameStart			();
		Animation.FrameStart		();	
		AI_Think.FrameStart			();
		AI_Range.FrameStart			();
		AI_Path.FrameStart			();
		AI_Node.FrameStart			();
		AI_Vis.FrameStart			();
		AI_Vis_Query.FrameStart		();
		AI_Vis_RayTests.FrameStart	();
		
		RenderTOTAL.FrameStart		();
		Render_CRenderRender_render_main.FrameStart		();
		RenderCALC_HOM.FrameStart	();
		RenderDUMP.FrameStart		();	
		RenderDUMP_RT.FrameStart	();
		RenderDUMP_SKIN.FrameStart	();	
		RenderDUMP_Wait.FrameStart	();	
		Render_CRenderRender_WaitForFrame.FrameStart();	
		RenderDUMP_HUD.FrameStart	();	
		RenderDUMP_Glows.FrameStart	();	
		RenderDUMP_Lights.FrameStart();	
		RenderDUMP_WM.FrameStart	();	
		RenderDUMP_DT_VIS.FrameStart();	
		RenderDUMP_DT_Render.FrameStart();	
		RenderDUMP_DT_Cache.FrameStart();	
		RenderDUMP_Pcalc.FrameStart	();	
		RenderDUMP_Scalc.FrameStart	();	
		RenderDUMP_Srender.FrameStart();	

		// Giperion Render specific
		Render_CRenderRender.FrameStart();
		Render_CRenderRender_MainMenu.FrameStart();
		Render_CRenderRender_ScenePrepare.FrameStart();
		Render_CRenderRender_DeferPart0.FrameStart();
		Render_CRenderRender_LightVisibility.FrameStart();
		Render_CRenderRender_ItemUIWallmarks.FrameStart();
		Render_CRenderRender_FlushOcclusion.FrameStart();
		Render_CRenderRender_MSAA_Rain.FrameStart();
		Render_CRenderRender_Sun.FrameStart();
		Render_CRenderRender_LightRender.FrameStart();
		Render_CRenderRender_Combine.FrameStart();
		Render_Combine_Begin.FrameStart();
		Render_Combine_Sky.FrameStart();
		Render_Combine_Cloud.FrameStart();
		Render_Combine_Combine1.FrameStart();
		Render_Combine_ForwardRendering.FrameStart();
		
		Sound.FrameStart			();
		Input.FrameStart			();
		clRAY.FrameStart			();	
		clBOX.FrameStart			();
		clFRUSTUM.FrameStart		();
		
		TEST0.FrameStart			();
		TEST1.FrameStart			();
		TEST2.FrameStart			();
		TEST3.FrameStart			();

		g_SpatialSpace->stat_insert.FrameStart		();
		g_SpatialSpace->stat_remove.FrameStart		();

		g_SpatialSpacePhysic->stat_insert.FrameStart();
		g_SpatialSpacePhysic->stat_remove.FrameStart();

	
	}
	dwSND_Played = dwSND_Allocated = 0;
	Particles_starting = Particles_active = Particles_destroy = 0;
	

}

void	_LogCallback				(LPCSTR string)
{
	if (string && '!' == string[0] && ' ' == string[1])
	{
		xrCriticalSectionGuard guard(Device.Statistic->errorsGuard);
		Device.Statistic->errors.emplace_back	(string);
	}
}

void CStats::OnDeviceCreate			()
{
	g_bDisableRedText				= strstr(Core.Params,"-xclsx")?TRUE:FALSE;

	pFont	= xr_new<CGameFont>		("stat_font", CGameFont::fsDeviceIndependent);

#ifdef DEBUG
	if (!g_bDisableRedText)
	{
		xrLogger::AddLogCallback(_LogCallback);
	}
#endif
}

void CStats::OnDeviceDestroy		()
{
	xrLogger::RemoveLogCallback(_LogCallback);
	xr_delete	(pFont);
}

void CStats::OnRender				()
{
#ifdef DEBUG
	if (g_stats_flags.is(st_sound)){
		CSound_stats_ext				snd_stat_ext;
		::Sound->statistic				(0,&snd_stat_ext);
		auto	_I = snd_stat_ext.items.begin();
		auto	_E = snd_stat_ext.items.end();
		for (;_I!=_E;_I++){
			const CSound_stats_ext::SItem& item = *_I;
			if (item._3D)
			{
				m_pRender->SetDrawParams(&*Device.m_pRender);
				DU->DrawCross			(item.params.position, 0.5f, 0xFF0000FF, true );
				if (g_stats_flags.is(st_sound_min_dist))
					DU->DrawSphere		(Fidentity, item.params.position, item.params.min_distance, 0x400000FF,	0xFF0000FF, true, true);
				if (g_stats_flags.is(st_sound_max_dist))
					DU->DrawSphere		(Fidentity, item.params.position, item.params.max_distance, 0x4000FF00,	0xFF008000, true, true);
				
				xr_string out_txt		= (out_txt.size() && g_stats_flags.is(st_sound_info_name)) ? item.name.c_str():"";

				if (item.game_object)
				{
					if (g_stats_flags.is(st_sound_ai_dist))
						DU->DrawSphere	(Fidentity, item.params.position, item.params.max_ai_distance, 0x80FF0000,0xFF800000,true,true);
					if (g_stats_flags.is(st_sound_info_object)){
						out_txt			+= "  (";
						out_txt			+= item.game_object->cNameSect().c_str();
						out_txt			+= ")";
					}
				}
				if (g_stats_flags.is_any(st_sound_info_name|st_sound_info_object) && item.name.size())
					DU->OutText			(item.params.position, out_txt.c_str(),0xFFFFFFFF,0xFF000000);
			}
		}
	}
#endif
}
