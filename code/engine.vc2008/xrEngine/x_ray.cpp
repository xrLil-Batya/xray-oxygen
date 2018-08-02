//-----------------------------------------------------------------------------
// File: x_ray.cpp
//
// Programmers:
//	Oles		- Oles Shishkovtsov
//	AlexMX		- Alexander Maksimchuk
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"
#include "ILoadingScreen.h"
#include "xr_input.h"
#include "xr_ioconsole.h"
#include "x_ray.h"
#include "std_classes.h"
#include "GameFont.h"
#include "../xrPlay/resource.h"
#include "LightAnimLibrary.h"
#include "../xrcdb/ispatial.h"
#include "Text_Console.h"
#include <process.h>
#include <locale.h>

#include "../FrayBuildConfig.hpp"
//---------------------------------------------------------------------
ENGINE_API CInifile* pGameIni = nullptr;
volatile bool g_bIntroFinished = false;
ENGINE_API BOOL isGraphicDebugging = FALSE; //#GIPERION: Graphic debugging
ENGINE_API BOOL g_appLoaded = FALSE;

#ifdef MASTER_GOLD
#	define NO_MULTI_INSTANCES
#endif // #ifdef MASTER_GOLD

//---------------------------------------------------------------------
// 2446363
// umbt@ukr.net
//////////////////////////////////////////////////////////////////////////
struct _SoundProcessor : public pureFrame
{
	virtual void _BCL OnFrame()
	{
		Device.Statistic->Sound.Begin();
		::Sound->update(Device.vCameraPosition, Device.vCameraDirection, Device.vCameraTop);
		Device.Statistic->Sound.End();
	}
}	SoundProcessor;

//////////////////////////////////////////////////////////////////////////
// global variables
ENGINE_API	CApplication*	pApp			= NULL;
static		HWND			logoWindow		= NULL;

			void			doBenchmark		(LPCSTR name);
ENGINE_API	bool			g_bBenchmark	= false;
string512	g_sBenchmarkName;


ENGINE_API	string512		g_sLaunchOnExit_params;
ENGINE_API	string512		g_sLaunchOnExit_app;
ENGINE_API	string_path		g_sLaunchWorkingFolder;
// -------------------------------------------
// startup point
void InitEngine()
{
	Engine.Initialize();
	while (!g_bIntroFinished)	
		Sleep(100);
	Device.Initialize();
}

ENGINE_API void InitSettings	()
{
	string_path fname;
	FS.update_path(fname, "$game_config$", "system.ltx");

	pSettings = xr_new<CInifile>(fname, TRUE);
	CHECK_OR_EXIT(!pSettings->sections().empty(), make_string("Cannot find file %s.\nReinstalling application may fix this problem.", fname));

	FS.update_path(fname, "$game_config$", "game.ltx");
	pGameIni = xr_new<CInifile>(fname, TRUE);
	CHECK_OR_EXIT(!pGameIni->sections().empty(), make_string("Cannot find file %s.\nReinstalling application may fix this problem.", fname));
}

ENGINE_API void InitConsole	()
{
	Console = xr_new<CConsole>();
	Console->Initialize();

	xr_strcpy(Console->ConfigFile,"user.ltx");
	if (strstr(Core.Params,"-ltx ")) 
	{
		string64 c_name;
		sscanf(strstr(Core.Params,"-ltx ")+5,"%[^ ] ",c_name);
		xr_strcpy(Console->ConfigFile,c_name);
	}
}

ENGINE_API void InitInput()
{
	BOOL bCaptureInput = !strstr(Core.Params, "-i");

	pInput = xr_new<CInput>(bCaptureInput);
}

void destroyInput()
{
	xr_delete(pInput);
}

ENGINE_API void InitSound1()
{
	CSound_manager_interface::_create(0);
}

ENGINE_API void InitSound2()
{
	CSound_manager_interface::_create(1);
}

inline void destroySound()
{
	CSound_manager_interface::_destroy();
}

void destroySettings()
{
	CInifile** s = (CInifile**)(&pSettings);
	xr_delete(*s);
	xr_delete(pGameIni);
}

void destroyConsole()
{
	Console->Execute("cfg_save");
	Console->Destroy();
	xr_delete(Console);
}

void destroyEngine()
{
	Device.Destroy();
	Engine.Destroy();
}

void execUserScript()
{
	Console->Execute("default_controls");
	Console->ExecuteScript(Console->ConfigFile);
}

void slowdownthread	( void* )
{
	for (;;)
	{
		if (Device.Statistic->fFPS < 30)	Sleep(1);
		if (Device.mt_bMustExit)			return;
		if (!pSettings)						return;
		if (!Console)						return;
		if (!pInput)						return;
		if (!pApp)							return;
	}
}
void CheckPrivilegySlowdown		( )
{
#ifdef DEBUG
	if (strstr(Core.Params, "-slowdown"))
	{
		thread_spawn(slowdownthread, "slowdown", 0, 0);
	}
	if (strstr(Core.Params, "-slowdown2x"))
	{
		thread_spawn(slowdownthread, "slowdown", 0, 0);
		thread_spawn(slowdownthread, "slowdown", 0, 0);
}
#endif // DEBUG
}

void Startup()
{
	InitSound1		();
	execUserScript	();
	InitSound2		();

	// ...command line for auto start
	{
		LPCSTR	pStartup = strstr(Core.Params, "-start ");
		if (pStartup)				
			Console->Execute(pStartup + 1);
	}
	{
		LPCSTR	pStartup = strstr(Core.Params, "-load ");
		if (pStartup)				
			Console->Execute(pStartup + 1);
	}

	if (strstr(Core.Params, "-$"))
	{
		string256                buf, cmd, param;
		sscanf(strstr(Core.Params, "-$")
			+ 2,
			"%[^ ] %[^ ] ",
			cmd,
			param);
		strconcat(sizeof(buf),
			buf,
			cmd,
			" ",
			param);
		Console->Execute(buf);
	}
	/////////////////////////////////////////////
	// Initialize APP
	ShowWindow(Device.m_hWnd, SW_SHOWNORMAL);
	/////////////////////////////////////////////
	Device.Create();
	LALib.OnCreate();
	pApp = xr_new<CApplication>();
	g_pGamePersistent = (IGame_Persistent*)NEW_INSTANCE(CLSID_GAME_PERSISTANT);
	g_SpatialSpace = xr_new<ISpatial_DB>();
	g_SpatialSpacePhysic = xr_new<ISpatial_DB>();
	/////////////////////////////////////////////
	// Destroy LOGO
	if (!strstr(Core.Params, "-nologo"))
	{
		DestroyWindow(logoWindow);
		logoWindow = NULL;
	}
	/////////////////////////////////////////////
	// Main cycle
	Memory.mem_usage();
	Device.Run();
	/////////////////////////////////////////////
	// Destroy APP
	xr_delete(g_SpatialSpacePhysic);
	xr_delete(g_SpatialSpace);
	DEL_INSTANCE(g_pGamePersistent);
	xr_delete(pApp);
	Engine.Event.Dump();
	/////////////////////////////////////////////
	// Destroying
	destroyInput();
	if (!g_bBenchmark)
	{
		destroySettings();
	}
	LALib.OnDestroy();
	if (!g_bBenchmark)
		destroyConsole();
	else
		Console->Destroy();
	/////////////////////////////////////////////
	destroySound();
	destroyEngine();
}

static INT_PTR CALLBACK logDlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) 
	{
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		DestroyWindow(hw);
		break;
	case WM_COMMAND:
		if (LOWORD(wp) == IDCANCEL)
			DestroyWindow(hw);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
extern void	testbed	(void);

// video
#define dwStickyKeysStructSize sizeof( STICKYKEYS )
#define dwFilterKeysStructSize sizeof( FILTERKEYS )
#define dwToggleKeysStructSize sizeof( TOGGLEKEYS )

struct damn_keys_filter 
{
	BOOL bScreenSaverState;

	// Sticky & Filter & Toggle keys

	STICKYKEYS StickyKeysStruct;
	FILTERKEYS FilterKeysStruct;
	TOGGLEKEYS ToggleKeysStruct;

	DWORD dwStickyKeysFlags, dwFilterKeysFlags, dwToggleKeysFlags;

	damn_keys_filter()
	{
		// Screen saver stuff
		bScreenSaverState = FALSE;

		// Saveing current state
		SystemParametersInfoA(SPI_GETSCREENSAVEACTIVE, 0, (PVOID)&bScreenSaverState, 0);

		if (bScreenSaverState)		
			SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, 0);		// Disable screensaver

		dwStickyKeysFlags = 0;
		dwFilterKeysFlags = 0;
		dwToggleKeysFlags = 0;

		ZeroMemory(&StickyKeysStruct, dwStickyKeysStructSize);
		ZeroMemory(&FilterKeysStruct, dwFilterKeysStructSize);
		ZeroMemory(&ToggleKeysStruct, dwToggleKeysStructSize);

		StickyKeysStruct.cbSize = dwStickyKeysStructSize;
		FilterKeysStruct.cbSize = dwFilterKeysStructSize;
		ToggleKeysStruct.cbSize = dwToggleKeysStructSize;

		// Saving current state
		SystemParametersInfoA(SPI_GETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
		SystemParametersInfoA(SPI_GETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
		SystemParametersInfoA(SPI_GETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);

		if ( StickyKeysStruct.dwFlags & SKF_AVAILABLE ) 
		{
			// Disable StickyKeys feature
			dwStickyKeysFlags = StickyKeysStruct.dwFlags;
			StickyKeysStruct.dwFlags = 0;
			SystemParametersInfoA(SPI_SETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
		}

		if ( FilterKeysStruct.dwFlags & FKF_AVAILABLE ) 
		{
			// Disable FilterKeys feature
			dwFilterKeysFlags = FilterKeysStruct.dwFlags;
			FilterKeysStruct.dwFlags = 0;
			SystemParametersInfoA(SPI_SETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
		}

		if ( ToggleKeysStruct.dwFlags & TKF_AVAILABLE ) 
		{
			// Disable FilterKeys feature
			dwToggleKeysFlags = ToggleKeysStruct.dwFlags;
			ToggleKeysStruct.dwFlags = 0;
			SystemParametersInfoA(SPI_SETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);
		}
	}

	~damn_keys_filter	()
	{
		if (bScreenSaverState)
			// Restoring screen saver
			SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, 0);

		if (dwStickyKeysFlags) 
		{
			// Restore StickyKeys feature
			StickyKeysStruct.dwFlags = dwStickyKeysFlags;
			SystemParametersInfoA(SPI_SETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
		}

		if (dwFilterKeysFlags) 
		{
			// Restore FilterKeys feature
			FilterKeysStruct.dwFlags = dwFilterKeysFlags;
			SystemParametersInfoA(SPI_SETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
		}

		if (dwToggleKeysFlags) 
		{
			// Restore FilterKeys feature
			ToggleKeysStruct.dwFlags = dwToggleKeysFlags;
			SystemParametersInfoA(SPI_SETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);
		}

	}
};

#undef dwStickyKeysStructSize
#undef dwFilterKeysStructSize
#undef dwToggleKeysStructSize

#include "xr_ioc_cmd.h"

ENGINE_API int RunApplication(LPCSTR commandLine)
{
	if (!IsDebuggerPresent())
	{
		size_t HeapFragValue = 2;
		HeapSetInformation(GetProcessHeap(), HeapCompatibilityInformation, &HeapFragValue, sizeof(HeapFragValue));
	}

	gMainThreadId = GetCurrentThreadId();
	Debug.set_mainThreadId(gMainThreadId);
	// Check for another instance
#ifdef NO_MULTI_INSTANCES
#define STALKER_PRESENCE_MUTEX "Local\\STALKER-COP"

	HANDLE hCheckPresenceMutex = OpenMutex(READ_CONTROL, FALSE, STALKER_PRESENCE_MUTEX);
	if (hCheckPresenceMutex == NULL)
	{
		hCheckPresenceMutex = CreateMutex(NULL, FALSE, STALKER_PRESENCE_MUTEX);	// New mutex
		if (hCheckPresenceMutex == NULL)
			return 2;
	}
	else
	{
		CloseHandle(hCheckPresenceMutex);		// Already running
		return 1;
	}
#endif

	//////////////////////////////////////////
	// Title window
	//////////////////////////////////////////
	HWND logoInsertPos = HWND_TOPMOST;
	if (IsDebuggerPresent())
	{
		logoInsertPos = HWND_NOTOPMOST;
	}
	//////////////////////////////////////////
	if (!strstr(Core.Params, "-nologo"))
	{
		logoWindow = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_STARTUP), 0, logDlgProc);
		HWND logoPicture = GetDlgItem(logoWindow, IDC_STATIC_LOGO);
		RECT logoRect;
		//////////////////////////////////////////
		GetWindowRect(logoPicture, &logoRect);
		SetWindowPos(logoWindow, logoInsertPos, 0,
					 0, logoRect.right - logoRect.left,
					 logoRect.bottom - logoRect.top,
					 SWP_NOMOVE | SWP_SHOWWINDOW);
		UpdateWindow(logoWindow);
	}
	//////////////////////////////////////////
	// AVI
	g_bIntroFinished = true;

	g_sLaunchOnExit_app[0] = 0;
	g_sLaunchOnExit_params[0] = 0;


	InitSettings();

	if (strstr(Core.Params, "-renderdebug"))
	{
		isGraphicDebugging = TRUE;
	}

	// Adjust player & computer name for Asian
	if (pSettings->line_exist("string_table", "no_native_input")) 
	{
		xr_strcpy(Core.UserName, sizeof(Core.UserName), "Player");
		xr_strcpy(Core.CompName, sizeof(Core.CompName), "Computer");
	}

	{
		FPU::m24r();
		InitEngine();
		InitInput();
		InitConsole();

		Engine.External.Initialize();

		Startup();
		Core._destroy();
#ifdef NO_MULTI_INSTANCES		
		// Delete application presence mutex
		CloseHandle(hCheckPresenceMutex);
#endif
	}
	return 0;
}

LPCSTR _GetFontTexName (LPCSTR section)
{
	static char* tex_names[] = { "texture800","texture","texture1600" };
	int def_idx		= 1;	//default 1024x768
	int idx			= def_idx;

	u32 h = Device.dwHeight;

	if (h <= 600)		
		idx = 0;
	else if (h < 1024)	
		idx = 1;
	else 			
		idx = 2;
	
	while(idx>=0)
	{
		if( pSettings->line_exist(section,tex_names[idx]))
			return pSettings->r_string(section,tex_names[idx]);
		--idx;
	}
	return pSettings->r_string(section,tex_names[def_idx]);
}

void _InitializeFont(CGameFont*& F, LPCSTR section, u32 flags)
{
	LPCSTR font_tex_name = _GetFontTexName(section);
	R_ASSERT(font_tex_name);

	LPCSTR sh_name = pSettings->r_string(section, "shader");
	if (!F) 
	{
		F = xr_new<CGameFont>(sh_name, font_tex_name, flags);
	}
	else
		F->Initialize(sh_name, font_tex_name);

	if (pSettings->line_exist(section, "size")) 
	{
		float sz = pSettings->r_float(section, "size");
		if (flags&CGameFont::fsDeviceIndependent)	F->SetHeightI(sz);
		else										F->SetHeight(sz);
	}
	if (pSettings->line_exist(section, "interval"))
		F->SetInterval(pSettings->r_fvector2(section, "interval"));

}

CApplication::CApplication()
{
	dwLoadReference	= 0;

	max_load_stage = 0;

	// events
	eQuit						= Engine.Event.Handler_Attach("KERNEL:quit",this);
	eStart						= Engine.Event.Handler_Attach("KERNEL:start",this);
	eStartLoad					= Engine.Event.Handler_Attach("KERNEL:load",this);
	eDisconnect					= Engine.Event.Handler_Attach("KERNEL:disconnect",this);
	eConsole					= Engine.Event.Handler_Attach("KERNEL:console",this);

	// levels
	Level_Current				= u32(-1);
	Level_Scan					( );

	// Font
	pFontSystem					= NULL;

	// Register us
	Device.seqFrame.Add			(this, REG_PRIORITY_HIGH+1000);
	Device.seqFrameMT.Add		(&SoundProcessor);

	Console->Show				( );

	// App Title
	loadingScreen = nullptr;
}

CApplication::~CApplication()
{
	Console->Hide();

	// font
	xr_delete(pFontSystem);

	Device.seqFrameMT.Remove(&SoundProcessor);
	Device.seqFrame.Remove(this);


	// events
	Engine.Event.Handler_Detach(eConsole, this);
	Engine.Event.Handler_Detach(eDisconnect, this);
	Engine.Event.Handler_Detach(eStartLoad, this);
	Engine.Event.Handler_Detach(eStart, this);
	Engine.Event.Handler_Detach(eQuit, this);
	
}

extern CRenderDevice Device;
ENGINE_API int ps_rs_loading_stages = 0;

void CApplication::OnEvent(EVENT E, u64 P1, u64 P2)
{
	if (E == eQuit)
	{
		PostQuitMessage	(0);
		
		for (u32 i=0; i<Levels.size(); i++)
		{
			xr_free(Levels[i].folder);
			xr_free(Levels[i].name);
		}
	}
	else if(E == eStart) 
	{
		LPSTR		op_server		= LPSTR	(P1);
		LPSTR		op_client		= LPSTR	(P2);
		Level_Current				= u32(-1);
		R_ASSERT	(!g_pGameLevel);
		R_ASSERT	(g_pGamePersistent);
		{		
			Console->Execute("main_menu off");
			Console->Hide();
//!			this line is commented by Dima
//!			because I don't see any reason to reset device here
			//-----------------------------------------------------------
			g_pGamePersistent->PreStart		(op_server);
			//-----------------------------------------------------------
			g_pGameLevel					= (IGame_Level*)NEW_INSTANCE(CLSID_GAME_LEVEL);
			pApp->LoadBegin					(); 
			g_pGamePersistent->Start		(op_server);
			g_pGameLevel->net_Start			(op_server,op_client);
			pApp->LoadEnd					(); 
		}
		xr_free							(op_server);
		xr_free							(op_client);
	} 
	else if (E==eDisconnect) 
	{

		if (g_pGameLevel) 
		{
			Console->Hide			();
			g_pGameLevel->net_Stop	();
			DEL_INSTANCE			(g_pGameLevel);
			Console->Show			();
			
			if( (FALSE == Engine.Event.Peek("KERNEL:quit")) &&(FALSE == Engine.Event.Peek("KERNEL:start")) )
			{
				Console->Execute("main_menu off");
				Console->Execute("main_menu on");
			}
		}
		R_ASSERT			(0!=g_pGamePersistent);
		g_pGamePersistent->Disconnect();
	}
	else if (E == eConsole)
	{
		LPSTR command				= (LPSTR)P1;
		Console->ExecuteCommand		( command, false );
		xr_free						(command);
	}
}

static	CTimer	phase_timer;

void CApplication::LoadBegin()
{
	dwLoadReference++;
	if (1==dwLoadReference)	
	{
		g_appLoaded			= FALSE;
		_InitializeFont		(pFontSystem,"ui_font_letterica18_russian",0);

		phase_timer.Start	();
		load_stage			= 0;
	}
}

void CApplication::LoadEnd()
{
    VERIFY(dwLoadReference != 0);
	dwLoadReference--;
	if (0 == dwLoadReference)
	{
		g_appLoaded = TRUE;
	}
}

void CApplication::SetLoadingScreen(ILoadingScreen* newScreen)
{
	if (loadingScreen)
    {
        Log("! Trying to create new loading screen, but there is already one..");
        DestroyLoadingScreen();
    }

	loadingScreen = newScreen;
}

void CApplication::DestroyLoadingScreen()
{
	xr_delete(loadingScreen);
}

ENGINE_API void CApplication::LoadDraw		()
{
	if(g_appLoaded)				return;
	Device.dwFrame				+= 1;

	if(!Device.Begin () )		return;
	load_draw_internal			();
	Device.End					();
}

void CApplication::LoadForceFinish()
{
	if (loadingScreen)
		loadingScreen->ForceFinish();
}

void CApplication::SetLoadStageTitle(const char* _ls_title)
{
	if (ps_rs_loading_stages && loadingScreen)
		 loadingScreen->SetStageTitle(_ls_title);
}

void CApplication::LoadTitleInt(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	if (loadingScreen)
		loadingScreen->SetStageTip(str1, str2, str3);
}
void CApplication::LoadStage()
{
	VERIFY(dwLoadReference);
	Msg("* phase time: %d ms",phase_timer.GetElapsed_ms());
	phase_timer.Start();
	
	max_load_stage = 19;

	LoadDraw();

	++load_stage;
}

void CApplication::LoadSwitch	()
{
}

// Sequential
void CApplication::OnFrame	( )
{
	Engine.Event.OnFrame			();
	g_SpatialSpace->update			();
	g_SpatialSpacePhysic->update	();
	if (g_pGameLevel)				
		g_pGameLevel->SoundEvent_Dispatch	( );
}

void CApplication::Level_Append		(LPCSTR folder)
{
	string_path	N1,N2,N3,N4;
	strconcat	(sizeof(N1),N1,folder,"level");
	strconcat	(sizeof(N2),N2,folder,"level.ltx");
	strconcat	(sizeof(N3),N3,folder,"level.geom");
	strconcat	(sizeof(N4),N4,folder,"level.cform");
	if	(
		FS.exist("$game_levels$",N1)		&&
		FS.exist("$game_levels$",N2)		&&
		FS.exist("$game_levels$",N3)		&&
		FS.exist("$game_levels$",N4)	
		)
	{
		sLevelInfo			LI;
		LI.folder			= xr_strdup(folder);
		LI.name				= 0;
		Levels.push_back	(LI);
	}
}

void CApplication::Level_Scan()
{
	for (u32 i=0; i<Levels.size(); i++)
	{
		xr_free(Levels[i].folder);
		xr_free(Levels[i].name);
	}
	Levels.clear	();

	xr_vector<char*>* folder			= FS.file_list_open		("$game_levels$",FS_ListFolders|FS_RootOnly);
	
	for (u32 i=0; i<folder->size(); ++i)	
		Level_Append((*folder)[i]);
	
	FS.file_list_close		(folder);
}

void gen_logo_name(string_path& dest, LPCSTR level_name, int num)
{
	strconcat	(sizeof(dest), dest, "intro\\intro_", level_name);
	
	u32 len = xr_strlen(dest);
	if(dest[len-1]=='\\')
		dest[len-1] = 0;

	string16 buff;
	xr_strcat(dest, sizeof(dest), "_");
	xr_strcat(dest, sizeof(dest), itoa(num+1, buff, 10));
}

void CApplication::Level_Set(u32 L)
{
	if (L >= Levels.size())	return;
	char* lvl_fld = FS.get_season_folder(Levels[L].folder);

    if (lvl_fld != nullptr)
    {
        Msg("level load from %s", lvl_fld);
        FS.get_path("$level$")->_set(lvl_fld);
    }

	static string_path			path;

	if (Level_Current != L)
	{
		path[0] = 0;

		Level_Current = L;

		int count = 0;
		while (true)
		{
			string_path			temp2;
			gen_logo_name(path, Levels[L].folder, count);
			if (FS.exist(temp2, "$game_textures$", path, ".dds") || FS.exist(temp2, "$level$", path, ".dds"))
				count++;
			else
				break;
		}

		if (count)
		{
			int num = ::Random.randI(count);
			gen_logo_name(path, Levels[L].folder, num);
		}
	}

	if (path[0] && loadingScreen)
		loadingScreen->SetLevelLogo(path);

}

int CApplication::Level_ID(LPCSTR name, LPCSTR ver, bool bSet)
{
	int result = -1;
	bool arch_res					= false;

	for(auto it = FS.m_archives.begin(); it != FS.m_archives.end(); ++it)
	{
		CLocatorAPI::archive& A		= *it;
		if(A.hSrcFile==NULL)
		{
			LPCSTR ln = A.header->r_string("header", "level_name");
			LPCSTR lv = A.header->r_string("header", "level_ver");
			if ( 0==_stricmp(ln,name) && 0==_stricmp(lv,ver) )
			{
				FS.LoadArchive(A);
				arch_res = true;
			}
		}
	}

	if( arch_res )
		Level_Scan							();
	
	string256		buffer;
	strconcat		(sizeof(buffer),buffer,name,"\\");
	for (u32 I=0; I<Levels.size(); ++I)
	{
		if (0==stricmp(buffer,Levels[I].folder))	
		{
			result = int(I);	
			break;
		}
	}

	if(bSet && result!=-1)
		Level_Set(result);

	if( arch_res )
		g_pGamePersistent->OnAssetsChanged	();

	return result;
}

CInifile*  CApplication::GetArchiveHeader(LPCSTR name, LPCSTR ver)
{
	for(auto it = FS.m_archives.begin(); it!=FS.m_archives.end(); ++it)
	{
		CLocatorAPI::archive& A		= *it;

		LPCSTR ln = A.header->r_string("header", "level_name");
		LPCSTR lv = A.header->r_string("header", "level_ver");
		if(!_stricmp(ln,name) && 0==_stricmp(lv, ver))
		{
			return A.header;
		}
	}
	return nullptr;
}

void CApplication::LoadAllArchives()
{
	if( FS.load_all_unloaded_archives() )
	{
		Level_Scan							();
		g_pGamePersistent->OnAssetsChanged	();
	}
}
#pragma optimize("g", off)
void CApplication::load_draw_internal()
{
	if (loadingScreen)
		loadingScreen->Update(load_stage, max_load_stage);
}
