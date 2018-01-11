// xrLC.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "math.h"
#include "build.h"
#include "../xrLC_Light/xrLC_GlobalData.h"

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"IMAGEHLP.LIB")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"xrCDB.lib")
#pragma comment(lib,"FreeImage.lib")
#pragma comment(lib,"xrCore.lib")
#pragma comment(lib,"xrLC_Light.lib")

#define PROTECTED_BUILD

#ifdef PROTECTED_BUILD
#	define TRIVIAL_ENCRYPTOR_ENCODER
#	define TRIVIAL_ENCRYPTOR_DECODER
#	include "../../xrEngine/trivial_encryptor.h"
#	undef TRIVIAL_ENCRYPTOR_ENCODER
#	undef TRIVIAL_ENCRYPTOR_DECODER
#endif // PROTECTED_BUILD

CBuild*	pBuild		= NULL;
u32		version		= 0;

extern void logThread();
extern volatile bool bClose;

static const char* h_str =
	"The following keys are supported / required:\n"
	"-skip	== Skip the test invalid face\n"
	"-? or -h		 == this help\n"
	"-o				 == modify build options\n"
	"-skip			 == skip crash if invalid faces exists\n"
	"-nosun			 == disable sun-lighting\n"
	"-norgb			 == disable common lightmap calculating\n"
	"-notessellation == skip tessellation for lightmap"
	"-sp<1-4>		 == the level of priority\n"
	"-f<NAME>		 == compile level in GameData\\Levels\\<NAME>\\\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

void Help()
{
	MessageBox(0,h_str,"Command line options",MB_OK|MB_ICONINFORMATION);
}

typedef int __cdecl xrOptions(b_params* params, u32 version, bool bRunBuild);

void Startup(LPSTR     lpCmdLine)
{
	
	create_global_data();
	char cmd[512],name[256];
	BOOL bModifyOptions		= FALSE;

	xr_strcpy(cmd,lpCmdLine);
	strlwr(cmd);
	if (strstr(cmd, "-?") || strstr(cmd, "-h") || !strstr(cmd, "-f"))
														{ Help(); return; }
	if (strstr(cmd, "-o"))								bModifyOptions	= TRUE;
	if (strstr(cmd, "-gi"))								g_build_options.b_radiosity		= true;
	if (strstr(cmd, "-noise"))							g_build_options.b_noise			= true;
	if (strstr(cmd, "-net"))							g_build_options.b_net_light		= true;
	if (strstr(cmd, "-skip"))							g_build_options.b_skipinvalid	= true;
	if (strstr(cmd, "-notessellation"))                 g_build_options.b_notessellation = true;

	//Added priority setting
	const char* isSp = strstr(cmd, "-sp");
	if(isSp)
		sscanf(isSp +3, "%lu", &g_build_options.Priority);
	else										
		g_build_options.Priority = 2;

	//end

	VERIFY( lc_global_data() );
	lc_global_data()->b_nosun_set						( !!strstr(cmd,"-nosun") );
	lc_global_data()->b_skiplmap_set					( !!strstr(cmd,"-norgb") );
	
	// Give a LOG-thread a chance to startup
	//_set_sbh_threshold(1920);
	InitCommonControls();
	std::thread log_thread(logThread);
	Sleep(150);
	
	// Faster FPU 
	switch (g_build_options.Priority)
	{
	case 1: SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
	case 2: SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
	case 3: SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	case 4: SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	}
	
	// Load project
	name[0]=0;				sscanf(strstr(cmd,"-f")+2,"%s",name);

	extern  HWND logWindow;
	string256				temp;
	xr_sprintf				(temp, "%s - Levels Compiler", name);
	SetWindowText			(logWindow, temp);

	string_path				prjName;
	FS.update_path			(prjName,"$game_levels$",strconcat(sizeof(prjName),prjName,name,"\\build.prj"));
	string256				phaseName;
	Phase					(strconcat(sizeof(phaseName),phaseName,"Reading project [",name,"]..."));

	string256 inf;
	IReader*	F			= FS.r_open(prjName);
	if (NULL==F){
		xr_sprintf				(inf,"Build failed!\nCan't find level: '%s'",name);
		clMsg				(inf);
		MessageBox			(logWindow,inf,"Error!",MB_OK|MB_ICONERROR);
		return;
	}

	// Version
	F->r_chunk			(EB_Version,&version);
	clMsg				("version: %d",version);
	R_ASSERT(XRCL_CURRENT_VERSION==version);

	// Header
	b_params				Params;
	F->r_chunk			(EB_Parameters,&Params);

	// Show options if needed
	if (bModifyOptions)		
	{
		Phase		("Project options...");
		HMODULE		L = LoadLibrary		("xrLC_Options.dll");
		void*		P = GetProcAddress	(L,"_frmScenePropertiesRun");
		R_ASSERT	(P);
		xrOptions*	O = (xrOptions*)P;
		int			R = O(&Params,version,false);
		FreeLibrary	(L);
		if (R==2)	{
			ExitProcess(0);
		}
	}
	
	// Conversion
	Phase					("Converting data structures...");
	pBuild					= xr_new<CBuild>();
	pBuild->Load			(Params,*F);
	FS.r_close				(F);
	
	// Call for builder
	string_path				lfn;
	CTimer	dwStartupTime;	dwStartupTime.Start();
	FS.update_path			(lfn,_game_levels_,name);
	pBuild->Run				(lfn);
	xr_delete				(pBuild);

	// Show statistic
	extern	std::string make_time(u32 sec);
	u32	dwEndTime			= dwStartupTime.GetElapsed_ms();
	xr_sprintf					(inf,"Time elapsed: %s",make_time(dwEndTime/1000).c_str());
	clMsg					("Build succesful!\n%s",inf);

	if (!strstr(cmd,"-silent"))
		MessageBox			(logWindow,inf,"Congratulation!",MB_OK|MB_ICONINFORMATION);

	// Close log
	bClose					= TRUE;
	Sleep					(500);
}

//typedef void DUMMY_STUFF (const void*,const u32&,void*);
//XRCORE_API DUMMY_STUFF	*g_temporary_stuff;
//XRCORE_API DUMMY_STUFF	*g_dummy_stuff;



int APIENTRY WinMain(HINSTANCE hInst,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
//	g_temporary_stuff	= &trivial_encryptor::decode;
//	g_dummy_stuff		= &trivial_encryptor::encode;

	// Initialize debugging
	Debug._initialize	(false);
	Core._initialize	("xrLC");
	
	if(strstr(Core.Params,"-nosmg"))
		g_using_smooth_groups = false;

	Startup				(lpCmdLine);
	Core._destroy		();
	
	return 0;
}
