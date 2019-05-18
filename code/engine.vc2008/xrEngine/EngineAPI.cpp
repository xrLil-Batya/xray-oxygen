#include "stdafx.h"
#include "EngineAPI.h"
#include "../xrcdb/xrXRC.h"
#include "XR_IOConsole.h"
#include "xr_ioc_cmd.h"
#include "spectre/Spectre.h"

extern XRCORE_API xr_vector<xr_token> vid_quality_token;

constexpr const char* r4_name = "xrRender_R4";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngineAPI::CEngineAPI	()
{
	hGame			= 0;
	hRender			= 0;
	pCreate			= 0;
	pDestroy		= 0;
}

CEngineAPI::~CEngineAPI()
{
	vid_quality_token.clear();
}

bool g_bRendererForced;

ENGINE_API bool is_enough_address_space_available	()
{
	SYSTEM_INFO		system_info;
	GetSystemInfo	( &system_info );
	return			(*(u32*)&system_info.lpMaximumApplicationAddress) > 0x90000000;
}

void CEngineAPI::InitializeRenderer()
{
	g_bRendererForced = true;

	// If we failed to load render,
	// then try to fallback to lower one.
	CCC_LoadCFG_custom pTmp("renderer ");
	pTmp.Execute(Console->ConfigFile);
	g_bRendererForced = false;

	if (psDeviceFlags.test(rsR4))
	{
		// try to initialize R4
		Msg("Loading DLL: %s", r4_name);
		hRender = LoadLibrary(r4_name);
		if (!hRender)
		{
			// try to load R4
			Msg("! ...Failed - incompatible hardware/pre-Vista OS.");
			psDeviceFlags.set(rsR2, true);
		}
	}
}

void CEngineAPI::Initialize(void)
{
	//////////////////////////////////////////////////////////////////////////
	// render
	InitializeRenderer();

	if (!hRender && vid_quality_token[0].id != -1)
	{
		// if engine failed to load renderer
		// but there is at least one available
		// then try again
		string32 buf;
		xr_sprintf(buf, "renderer %s", vid_quality_token[0].name);
		Console->Execute(buf);

		// Second attempt
		InitializeRenderer();
	}

	if (!hRender)
		R_CHK(GetLastError());

	R_ASSERT2(hRender, "Can't load renderer");

	Device.ConnectToRender();

	// Game
	{
		LPCSTR g_name = "xrGame";
		if (strstr(Core.Params, "-debug_game"))
			g_name = "xrGame_debug";

		Msg				("Loading DLL: %s",g_name);
		hGame			= LoadLibrary	(g_name);
		if (!hGame)
		{
			R_CHK(GetLastError());
			return;
		}

		R_ASSERT3		(hGame, "Game DLL raised exception during loading or there is no game DLL at all", g_name);
		pCreate			= (Factory_Create*)GetProcAddress(hGame,"xrFactory_Create");	R_ASSERT(pCreate);
		pDestroy		= (Factory_Destroy*)GetProcAddress(hGame,"xrFactory_Destroy");	R_ASSERT(pDestroy);
	}
}

void CEngineAPI::Destroy	(void)
{
	if (hGame)				{ FreeLibrary(hGame);	hGame	= nullptr; }
	if (hRender)			{ FreeLibrary(hRender); hRender = nullptr; }
	pCreate					= 0;
	pDestroy				= 0;
	Engine.Event._destroy	();
	XRC.r_clear_compact		();
}

DLL_Pure::~DLL_Pure()
{
	SpectreEngineClient::DestroyProxyObject(this);
}
