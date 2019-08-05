#include "stdafx.h"
#include "igame_level.h"
#include "xr_ioconsole.h"
#include "xr_ioc_cmd.h"
#include "xr_ioc_cmd_ex.h"
#include "cameramanager.h"
#include "environment.h"
#include "xr_input.h"
#include "string_table.h"

#include <regex>
#include "../Include/xrRender/RenderDeviceRender.h"

float	view_port_near_koef = 0.5f;

extern ENGINE_API float		psVisDistance;
extern ENGINE_API float		psFogDistance;
extern ENGINE_API float		psFogDensity;
extern ENGINE_API float		psFogNear;
extern ENGINE_API float		psFogFar;
extern ENGINE_API Fvector3	psFogColor;

extern XRCORE_API xr_vector<xr_token> vid_quality_token;

xr_token vid_bpp_token[ ]=
{
	{ "16",	16},
	{ "32",	32},
	{ 0,	0}
};
//-----------------------------------------------------------------------
void IConsole_Command::add_to_LRU( shared_str const& arg )
{
	if ( arg.size() == 0 || bEmptyArgsHandled )
	{
		return;
	}
	
	bool dup = ( std::find( m_LRU.begin(), m_LRU.end(), arg ) != m_LRU.end() );
	if ( !dup )
	{
		m_LRU.push_back( arg );
		if ( m_LRU.size() > LRU_MAX_COUNT )
		{
			m_LRU.erase( m_LRU.begin() );
		}
	}
}

void  IConsole_Command::add_LRU_to_tips( vecTips& tips )
{
	vecLRU::reverse_iterator	it_rb = m_LRU.rbegin();
	vecLRU::reverse_iterator	it_re = m_LRU.rend();
	for ( ; it_rb != it_re; ++it_rb )
	{
		tips.push_back( (*it_rb) );
	}
}

// =======================================================

class CCC_Quit : public IConsole_Command
{
public:
	CCC_Quit(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
//		TerminateProcess(GetCurrentProcess(),0);
		Console->Hide();
		Engine.Event.Defer("KERNEL:disconnect");
		Engine.Event.Defer("KERNEL:quit");
	}
};
//-----------------------------------------------------------------------
class CCC_DbgStrCheck : public IConsole_Command
{
public:
	CCC_DbgStrCheck(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) { g_pStringContainer.verify(); }
};

class CCC_DbgStrDump : public IConsole_Command
{
public:
	CCC_DbgStrDump(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) { g_pStringContainer.dump();}
};

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
class CCC_E_Dump : public IConsole_Command
{
public:
	CCC_E_Dump(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Engine.Event.Dump();
	}
};
class CCC_E_Signal : public IConsole_Command
{
public:
	CCC_E_Signal(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		char	Event[128],Param[128];
		Event[0]=0; Param[0]=0;
		sscanf	(args,"%[^,],%s",Event,Param);
		Engine.Event.Signal	(Event,(u64)Param);
	}
};



//-----------------------------------------------------------------------
class CCC_Help : public IConsole_Command
{
public:
	CCC_Help(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Log("- --- Command listing: start ---");
		CConsole::vecCMD_IT it;
		for (it=Console->Commands.begin(); it!=Console->Commands.end(); it++)
		{
			IConsole_Command &C = *(it->second);
			TStatus _S; C.Status(_S);
			TInfo	_I;	C.Info	(_I);
			
			Msg("%-20s (%-10s) --- %s",	C.Name(), _S, _I);
		}
		Log("Key: Ctrl + A         === Select all ");
		Log("Key: Ctrl + C         === Copy to clipboard ");
		Log("Key: Ctrl + V         === Paste from clipboard ");
		Log("Key: Ctrl + X         === Cut to clipboard ");
		Log("Key: Ctrl + Z         === Undo ");
		Log("Key: Ctrl + Insert    === Copy to clipboard ");
		Log("Key: Shift + Insert   === Paste from clipboard ");
		Log("Key: Shift + Delete   === Cut to clipboard ");
		Log("Key: Insert           === Toggle mode <Insert> ");
		Log("Key: Back / Delete          === Delete symbol left / right ");

		Log("Key: Up   / Down            === Prev / Next command in tips list ");
		Log("Key: Ctrl + Up / Ctrl + Down === Prev / Next executing command ");
		Log("Key: Left, Right, Home, End {+Shift/+Ctrl}       === Navigation in text ");
		Log("Key: PageUp / PageDown      === Scrolling history ");
		Log("Key: Tab  / Shift + Tab     === Next / Prev possible command from list");
		Log("Key: Enter  / NumEnter      === Execute current command ");
		
		Log("- --- Command listing: end ----");
	}
};


XRCORE_API void _dump_open_files(int mode);
class CCC_DumpOpenFiles : public IConsole_Command
{
public:
	CCC_DumpOpenFiles(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = FALSE; };
	virtual void Execute(LPCSTR args) {
		int _mode			= atoi_17(args);
		_dump_open_files	(_mode);
	}
};

//-----------------------------------------------------------------------
class CCC_SaveCFG : public IConsole_Command
{
public:
	CCC_SaveCFG(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) 
	{
		string_path			cfg_full_name;
		xr_strcpy			(cfg_full_name, (xr_strlen(args)>0)?args:Console->ConfigFile);

		bool b_abs_name = xr_strlen(cfg_full_name)>2 && cfg_full_name[1]==':';

		if(!b_abs_name)
			FS.update_path	(cfg_full_name, "$app_data_root$", cfg_full_name);

		//if (strext(cfg_full_name))	
		//*strext(cfg_full_name) = 0;
		//xr_strcat			(cfg_full_name,".ltx");
		
		BOOL b_allow = TRUE;
		if ( FS.exist(cfg_full_name) )
			b_allow = SetFileAttributes(cfg_full_name,FILE_ATTRIBUTE_NORMAL);

		if ( b_allow ){
			IWriter* F			= FS.w_open(cfg_full_name);
				CConsole::vecCMD_IT it;
				for (it=Console->Commands.begin(); it!=Console->Commands.end(); it++)
					it->second->Save(F);
				FS.w_close			(F);
				Msg("Config-file [%s] saved successfully",cfg_full_name);
		}else
			Msg("!Cannot store config file [%s]", cfg_full_name);
	}
};
CCC_LoadCFG::CCC_LoadCFG(LPCSTR N) : IConsole_Command(N) 
{};

void CCC_LoadCFG::Execute(LPCSTR args) 
{
		Msg("Executing config-script \"%s\"...",args);
		string_path						cfg_name;

		xr_strcpy							(cfg_name, args);
		if (strext(cfg_name))			*strext(cfg_name) = 0;
		xr_strcat							(cfg_name,".ltx");

		string_path						cfg_full_name;

		FS.update_path					(cfg_full_name, "$app_data_root$", cfg_name);
		
		if( NULL == FS.exist(cfg_full_name) )
			FS.update_path					(cfg_full_name, "$fs_root$", cfg_name);
			
		if( NULL == FS.exist(cfg_full_name) )
			xr_strcpy						(cfg_full_name, cfg_name);
		
		IReader* F						= FS.r_open(cfg_full_name);
		
		string1024						str;
		if (F!=NULL) {
			while (!F->eof()) {
				F->r_string				(str,sizeof(str));
				if(allow(str))
					Console->Execute	(str);
			}
			FS.r_close(F);
			Msg("[%s] successfully loaded.",cfg_full_name);
		} else {
			Msg("! Cannot open script file [%s]",cfg_full_name);
		}
}

CCC_LoadCFG_custom::CCC_LoadCFG_custom(LPCSTR cmd)
:CCC_LoadCFG(cmd)
{
	xr_strcpy(m_cmd, cmd);
};
bool CCC_LoadCFG_custom::allow(LPCSTR cmd)
{
	return (cmd == strstr(cmd, m_cmd) );
};

//-----------------------------------------------------------------------
class CCC_Start : public IConsole_Command
{
protected:
	xr_string parse(const xr_string &str)
	{
		std::regex Reg("\\(([^)]+)\\)");
		std::smatch results;
		R_ASSERT3(std::regex_search(str, results, Reg), "Failed parsing string: [%s]", str.c_str());
		return results[1].str().c_str();
	}
public:
	CCC_Start(const char* N) : IConsole_Command(N) {};
	void Execute(const char* args) override 
	{
		xr_string str = this->parse(args);
		Engine.Event.Defer("KERNEL:start", u64(xr_strdup(str.c_str())), u64(xr_strdup("localhost")));
	}
};

class CCC_Disconnect : public IConsole_Command
{
public:
	CCC_Disconnect(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Engine.Event.Defer("KERNEL:disconnect");
	}
};
//-----------------------------------------------------------------------
class CCC_VID_Reset : public IConsole_Command
{
public:
	CCC_VID_Reset(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		if (Device.b_is_Ready) {
			Device.Reset	();
		}
	}
};
class CCC_VidMode : public CCC_Token
{
	u32		_dummy, _w, _h;
public :
					CCC_VidMode(LPCSTR N) : CCC_Token(N, &_dummy, NULL), _w(0), _h(0) { bEmptyArgsHandled = FALSE; };
	void	Execute(LPCSTR args) override
	{ 
	
		int cnt = sscanf		(args,"%dx%d",&_w,&_h);
		if(cnt==2)
		{
			psCurrentVidMode[0] = _w;
			psCurrentVidMode[1] = _h;
		}
		else
		{
			Msg("! Wrong video mode [%s]", args);
			return;
		}
	}

	virtual bool isWideScreen()
	{
		u32 uWidth = psCurrentVidMode[0];
		u32 uHeight = psCurrentVidMode[1];
		float deltaBase = (float)uWidth / (float)uHeight;

		// 1920 / 1200 = 16:10 = 1.6
		if (deltaBase > 1.6f)
			return true;

		return false;
	}

	virtual void	Status	(TStatus& S)	
	{ 
		xr_sprintf(S,sizeof(S),"%dx%d",psCurrentVidMode[0],psCurrentVidMode[1]); 
	}
	virtual xr_token* GetToken()				{return vid_mode_token;}
	virtual void	Info	(TInfo& I)
	{	
		xr_strcpy(I,sizeof(I),"change screen resolution WxH");
	}

	virtual void	fill_tips(vecTips& tips, u32 mode)
	{
		TStatus  str, cur;
		Status( cur );

		bool res = false;
		xr_token* tok = GetToken();
		while ( tok->name && !res )
		{
			if ( !xr_strcmp( tok->name, cur ) )
			{
				xr_sprintf( str, sizeof(str), "%s  (current)", tok->name );
				tips.push_back( str );
				res = true;
			}
			tok++;
		}
		if ( !res )
		{
			tips.push_back( "---  (current)" );
		}
		tok = GetToken();
		while ( tok->name )
		{
			tips.push_back( tok->name );
			tok++;
		}
	}

};

//-----------------------------------------------------------------------
ENGINE_API u32 ps_vid_windowtype = 4;
xr_token vid_windowtype_token[] =
{
    { "windowed",               1 },
    { "windowed_borderless",    2 },
    { "fullscreen",             3 },
    { "fullscreen_borderless",  4 },
    { 0,                        0 },
};

class CCC_VidWindowType : public CCC_Token
{

public:
    CCC_VidWindowType(LPCSTR N, u32* V, xr_token* T) : CCC_Token(N, V, T) {}

    virtual void Execute(LPCSTR args) override
    {
        CCC_Token::Execute(args);

        u32 rawNewWindowType = *value;
        Device.UpdateWindowPropStyle((WindowPropStyle)rawNewWindowType);
    }
};

//-----------------------------------------------------------------------
class CCC_SND_Restart : public IConsole_Command
{
public:
	CCC_SND_Restart(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Sound->_restart();
	}
};

//-----------------------------------------------------------------------
float ps_gamma = 1.0f;
float ps_brightness = 1.0f;
float ps_contrast = 1.0f;
class CCC_Gamma : public CCC_Float
{
public:
	CCC_Gamma(LPCSTR N, float* V) : CCC_Float(N, V, 0.5f, 1.5f) {}

	virtual void Execute(LPCSTR args)
	{
		CCC_Float::Execute(args);

		Device.m_pRender->SetGamma		(ps_gamma);
		Device.m_pRender->SetBrightness	(ps_brightness);
		Device.m_pRender->SetContrast	(ps_contrast);
		Device.m_pRender->UpdateGamma	();
	}
};

Fvector3 ps_c_balance = { 1.0f, 1.0f, 1.0f };
class CCC_ColorBalance : public CCC_Vector3
{
public:
	CCC_ColorBalance(LPCSTR N, Fvector* V) : CCC_Vector3(N, V, Fvector().set(0.5f, 0.5f, 0.5f), Fvector().set(1.5f, 1.5f, 1.5f)) {}

	virtual void Execute(LPCSTR args)
	{
		CCC_Vector3::Execute(args);

		Device.m_pRender->SetBalance	(ps_c_balance);
		Device.m_pRender->UpdateGamma	();
	}
};

ENGINE_API BOOL r2_advanced_pp = FALSE;	//	advanced post process and effects
extern bool g_bRendererForced;

class CCC_Renderer : public CCC_Token
{
	typedef CCC_Token inherited;
	bool bLoaded;
public:
	CCC_Renderer(LPCSTR N) : inherited(N, nullptr, NULL)
	{
		bLoaded = false;
	};
	virtual ~CCC_Renderer()
	{
	}
	virtual void	Execute(LPCSTR args)
	{
		//	vid_quality_token must be already created!
		if (!bLoaded)
		{
			psDeviceFlags.set(rsR4, true);
			r2_advanced_pp = true;
			bLoaded = true;
		}
	}

	virtual void	Save	(IWriter *F)	
	{
		tokens = vid_quality_token.data();

		if (!g_bRendererForced)
			inherited::Save(F);
	}
	virtual xr_token* GetToken()
	{
		tokens = vid_quality_token.data();
		return inherited::GetToken();
	}

	virtual void fill_tips(vecTips& tips, u32 mode)
	{
		tips.emplace_back(GetToken()->name);
	}

	virtual void Status(TStatus& S)
	{
		xr_token* tok = tokens;
		xr_strcpy(S, tok->name);
		return;
		//xr_strcpy(S, "?");
	}
};

class CCC_soundDevice : public CCC_Token
{
	typedef CCC_Token inherited;
public:
	CCC_soundDevice(LPCSTR N) :inherited(N, &snd_device_id, NULL){};
	virtual			~CCC_soundDevice	()
	{}

	virtual void Execute(LPCSTR args)
	{
		GetToken				();
		if(!tokens)				return;
		inherited::Execute		(args);
	}

	virtual void	Status	(TStatus& S)
	{
		GetToken				();
		if(!tokens)				return;
		inherited::Status		(S);
	}

	virtual xr_token* GetToken()
	{
		tokens					= snd_devices_token;
		return inherited::GetToken();
	}

	virtual void Save(IWriter *F)	
	{
		GetToken				();
		if(!tokens)				return;
		inherited::Save			(F);
	}
};

class ENGINE_API CCC_HideConsole : public IConsole_Command
{
public		:
	CCC_HideConsole(LPCSTR N) : IConsole_Command(N)
	{
		bEmptyArgsHandled	= true;
	}

	virtual void	Execute	(LPCSTR args)
	{
		Console->Hide	();
	}
	virtual void	Status	(TStatus& S)
	{
		S[0]			= 0;
	}
	virtual void	Info	(TInfo& I)
	{	
		xr_sprintf		(I,sizeof(I),"hide console");
	}
};


ENGINE_API float psHUD_FOV_def = 0.45f;
ENGINE_API float psHUD_FOV = psHUD_FOV_def;

extern int rsDVB_Size;
extern int rsDIB_Size;

extern int g_ErrorLineCount;
extern int ps_rs_loading_stages;

ENGINE_API int ps_always_active = 0;

u32 ps_r_RefreshHZ = 0;
xr_token RefreshHZ[] =
{
	{ "60hz" , 0},
	{ "120hz" , 1},
	{ "auto" , 2},
    { 0, 0	}
};

void CCC_Register()
{
	// General
	CMD1(CCC_Help,		"help"					);
	CMD1(CCC_Quit,		"quit"					);
	CMD1(CCC_Start,		"start"					);
	CMD1(CCC_Disconnect,"disconnect"			);
	CMD1(CCC_SaveCFG,	"cfg_save"				);
	CMD1(CCC_LoadCFG,	"cfg_load"				);
	CMD1(CCC_GameLanguage, "g_game_languages");

#ifdef DEBUG
	CMD1(CCC_DbgStrCheck,	"dbg_str_check"		);
	CMD1(CCC_DbgStrDump,	"dbg_str_dump"		);

	// Events
	CMD1(CCC_E_Dump,	"e_list"				);
	CMD1(CCC_E_Signal,	"e_signal"				);

	CMD3(CCC_Mask,		"rs_clear_bb",			&psDeviceFlags,		rsClearBB);

//	CMD3(CCC_Mask,		"rs_constant_fps",		&psDeviceFlags,		rsConstantFPS			);
	CMD3(CCC_Mask,		"rs_render_statics",	&psDeviceFlags,		rsDrawStatic			);
	CMD3(CCC_Mask,		"rs_render_dynamics",	&psDeviceFlags,		rsDrawDynamic			);
#endif
	CMD3(CCC_Mask,		"rs_draw_fps",			&psDeviceFlags,		rsDrawFPS				);
	CMD3(CCC_Mask,		"rs_hw_stats",			&psDeviceFlags,		rsHWInfo				);
	CMD3(CCC_Mask,		"rs_env_stats",			&psDeviceFlags,		rsEnviromentInfo		);
	// Render device states
	CMD3(CCC_Mask,		"rs_detail",			&psDeviceFlags,		rsDetails				);

	CMD3(CCC_Mask,		"rs_triple_buffering",	&psDeviceFlags,		rsTripleBuffering		);
	CMD3(CCC_Mask,		"rs_v_sync",			&psDeviceFlags,		rsVSync					);
	CMD3(CCC_Token,		"rs_refresh_hz",		&ps_r_RefreshHZ,	RefreshHZ			);
	CMD3(CCC_Mask,		"rs_stats",				&psDeviceFlags,		rsStatistic				);
	CMD3(CCC_Mask,		"rs_stats_game",		&psDeviceFlags,		rsGameProfiler			);
	CMD3(CCC_Mask,		"rs_stats_schedule",    &psDeviceFlags,		rsScheduleProfiler		);
	CMD3(CCC_Mask,		"rs_cam_pos",			&psDeviceFlags,		rsCameraPos				);

	CMD4(CCC_Float,		"rs_vis_distance",		&psVisDistance,		0.4f,	1.0f			);
	if (strstr(Core.Params,"-fog_mixer"))
	{
	CMD4(CCC_Float,		"rs_fog_distance",		&psFogDistance,		0.1f,	2.0f			);
	CMD4(CCC_Float,		"rs_fog_density",		&psFogDensity,		0.0f,	10.0f			);
	CMD4(CCC_Float,		"rs_fog_near",			&psFogNear,			0.0f,	10.0f			);
	CMD4(CCC_Vector3,	"rs_fog_color",			&psFogColor, Fvector().set(0, 0, 0), Fvector().set(256, 256, 256));
	CMD4(CCC_Float,		"rs_fog_far",			&psFogFar,			0.0f,	10.0f			);
	}
#ifdef DEBUG
	CMD3(CCC_Mask,		"rs_occ_draw",			&psDeviceFlags,		rsOcclusionDraw			);
	CMD3(CCC_Mask,		"rs_occ_stats",			&psDeviceFlags,		rsOcclusionStats		);
#endif // DEBUG
	CMD4(CCC_Float,		"rs_near_plane", &view_port_near_koef,				0.01, 5.0);
	CMD2(CCC_Gamma,		"rs_c_gamma"			,&ps_gamma			);
	CMD2(CCC_Gamma,		"rs_c_brightness"		,&ps_brightness		);
	CMD2(CCC_Gamma,		"rs_c_contrast"			,&ps_contrast		);
	CMD2(CCC_ColorBalance, "rs_c_balance"		,&ps_c_balance		);
//	CMD4(CCC_Integer,	"rs_vb_size",			&rsDVB_Size,		32,		4096);
//	CMD4(CCC_Integer,	"rs_ib_size",			&rsDIB_Size,		32,		4096);
	CMD4(CCC_Integer,	"rs_loadingstages",		&ps_rs_loading_stages,		0, 1);

	// Texture manager	
	CMD4(CCC_Integer,	"texture_lod",			&psTextureLOD,				0,	4	);

	// General video control
	CMD1(CCC_VidMode,	    "vid_mode"			);
    CMD3(CCC_VidWindowType, "vid_windowtype",   &ps_vid_windowtype, vid_windowtype_token);
	CMD1(CCC_VID_Reset,		"vid_restart"		);
	
	// Sound
	CMD2(CCC_Float,		"snd_volume_eff",		&psSoundVEffects);
	CMD2(CCC_Float,		"snd_volume_music",		&psSoundVMusic);
	CMD4(CCC_Float,		"snd_speed_of_sound",	&psSpeedOfSound,0.2f,2.0f	);
	CMD1(CCC_SND_Restart,"snd_restart"			);
	CMD3(CCC_Mask,		"snd_acceleration",		&psSoundFlags,		ss_Hardware	);
	CMD3(CCC_Mask,		"snd_efx",				&psSoundFlags,		ss_EFX		);
	CMD4(CCC_Integer,	"snd_targets",			&psSoundTargets,	4,256		);
	CMD4(CCC_Integer,	"snd_cache_size",		&psSoundCacheSizeMB,4,512		);

#ifdef DEBUG
	CMD3(CCC_Mask,		"snd_stats",			&g_stats_flags,		st_sound	);
	CMD3(CCC_Mask,		"snd_stats_min_dist",	&g_stats_flags,		st_sound_min_dist );
	CMD3(CCC_Mask,		"snd_stats_max_dist",	&g_stats_flags,		st_sound_max_dist );
	CMD3(CCC_Mask,		"snd_stats_ai_dist",	&g_stats_flags,		st_sound_ai_dist );
	CMD3(CCC_Mask,		"snd_stats_info_name",	&g_stats_flags,		st_sound_info_name );
	CMD3(CCC_Mask,		"snd_stats_info_object",&g_stats_flags,		st_sound_info_object );

	CMD4(CCC_Integer,	"error_line_count",		&g_ErrorLineCount,	6,	1024	);
#endif // DEBUG

	// Mouse
	CMD3(CCC_Mask,		"mouse_invert",			&psMouseInvert,1);
	psMouseSens			= 0.12f;
	CMD4(CCC_Float,		"mouse_sens",			&psMouseSens,		0.001f, 0.6f);

	// Camera
	CMD2(CCC_Float,		"cam_inert",			&psCamInert);
	//CMD2(CCC_Float,		"cam_slide_inert",		&psCamSlideInert);
	CMD4(CCC_Integer,	"always_active",		&ps_always_active,	0,	1);

	CMD1(CCC_Renderer,	"renderer"				);

	CMD1(CCC_soundDevice, "snd_device"			);

	//psSoundRolloff	= pSettings->r_float	("sound","rolloff");		clamp(psSoundRolloff,			EPS_S,	2.f);
	psSoundOcclusionScale	= pSettings->r_float	("sound","occlusion_scale");clamp(psSoundOcclusionScale,	0.1f,	.5f);

	extern	int	g_Dump_Export_Obj;
	CMD4(CCC_Integer,	"net_dbg_dump_export_obj",	&g_Dump_Export_Obj, 0, 1);

#ifdef DEBUG	
	CMD1(CCC_DumpOpenFiles,		"dump_open_files");
#endif

	CMD1(CCC_HideConsole,		"hide");

#ifdef	DEBUG
	extern BOOL debug_destroy;
	CMD4(CCC_Integer, "debug_destroy", &debug_destroy, FALSE, TRUE );
#endif
};
 
void IConsole_Command::InvalidSyntax()
{
	TInfo I; Info(I);
	Msg("~ Invalid syntax in call to '%s'", cName);
	Msg("~ Valid arguments: %s", I);
}

CCC_GameLanguage::CCC_GameLanguage(LPCSTR N)
	: CCC_Token(N, (u32*)& g_Language, language_type_token)
{}

void CCC_GameLanguage::Execute(LPCSTR args)
{
	CCC_Token::Execute(args);
	Msg("[GAME] Game language changed!");
	CStringTable().ReInit(g_Language);
}

void CCC_GameLanguage::Info(TInfo& I)
{
	xr_strcpy(I, "Game language");
}
