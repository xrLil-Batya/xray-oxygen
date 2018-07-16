#include "stdafx.h"
#include "../xrEngine/xr_ioconsole.h"
#include "../xrEngine/xr_ioc_cmd.h"
#include "../xrEngine/customhud.h"
#include "../xrEngine/fdemorecord.h"
#include "../xrEngine/fdemoplay.h"
#include "xrMessages.h"
#include "xrserver.h"
#include "level.h"
#include "ai_debug.h"
#include "alife_simulator.h"
#include "game_cl_base.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "actor.h"
#include "Actor_Flags.h"
#include "customzone.h"
#include "script_engine.h"
#include "script_engine_space.h"
#include "script_process.h"
#include "xrServer_Objects.h"
#include "ui/UIMainIngameWnd.h"
#include "../xrphysics/iphworld.h"
#include "string_table.h"
#include "autosave_manager.h"
#include "ai_space.h"
#include "ai/monsters/BaseMonster/base_monster.h"
#include "../xrEngine/date_time.h"
#include "UIGame.h"
#include "ui/UIActorMenu.h"
#include "ui/UIStatic.h"
#include "zone_effector.h"
#include "GameTask.h"
#include "MainMenu.h"
#include "saved_game_wrapper.h"
#include "level_graph.h"
#include "cameralook.h"
#include "character_hit_animations_params.h"
#include "inventory_upgrade_manager.h"
#include "../xrCore/FS.h"
#include "../xrCore/LocatorAPI.h"

#include "ai_debug_variables.h"
#include "../xrphysics/console_vars.h"
#ifdef DEBUG
#	include "PHDebug.h"
#	include "ui/UIDebugFonts.h" 
#	include "game_graph.h"
#	include "CharacterPhysicsSupport.h"
#endif // DEBUG
#include "HudItem.h"
#include "../xrEngine/xr_ioc_cmd_ex.h"

string_path		g_last_saved_game;

#ifdef DEBUG
	extern float air_resistance_epsilon;
#endif // #ifdef DEBUG

extern	u64		g_qwStartGameTime;
extern 	u32 	hud_adj_mode;

extern  EGameLanguage g_Language;
ENGINE_API extern  float   psHUD_FOV_def;
extern	float	psSqueezeVelocity;
extern	int		psLUA_GCSTEP;

extern	int		x_m_x;
extern	int		x_m_z;
extern	BOOL	net_sv_control_hit		;
extern	int		g_dwInputUpdateDelta	;
#ifdef DEBUG
extern	BOOL	g_ShowAnimationInfo		;
#endif // DEBUG
extern	ESingleGameDifficulty g_SingleGameDifficulty;
extern	BOOL	g_show_wnd_rect2			;
//-----------------------------------------------------------
extern	float	g_fTimeFactor;
extern	BOOL	b_toggle_weapon_aim;
//extern  BOOL	g_old_style_ui_hud;

extern float	g_smart_cover_factor;
extern int		g_upgrades_log;
extern float	g_smart_cover_animation_speed_factor;

extern	BOOL	g_ai_use_old_vision;
float			g_aim_predict_time = 0.44f;
int				g_keypress_on_start	= 1;

ENGINE_API extern float	g_console_sensitive;

//-----------------------------------------------------------

		BOOL	g_bCheckTime			= FALSE;
		int		net_cl_inputupdaterate	= 50;
#ifdef DEBUG
		Flags32	dbg_net_Draw_Flags		= {0};
#endif

#ifdef DEBUG
		BOOL	g_bDebugNode			= FALSE;
		u32		g_dwDebugNodeSource		= 0;
		u32		g_dwDebugNodeDest		= 0;
extern	BOOL	g_bDrawBulletHit;
extern	BOOL	g_bDrawFirstBulletCrosshair;

		float	debug_on_frame_gather_stats_frequency	= 0.f;
#endif
#ifdef DEBUG 
extern LPSTR	dbg_stalker_death_anim;
extern BOOL		b_death_anim_velocity;
extern XRPHYSICS_API BOOL death_anim_debug;
extern BOOL		dbg_imotion_draw_skeleton;
extern BOOL		dbg_imotion_draw_velocity;
extern BOOL		dbg_imotion_collide_debug;
extern float	dbg_imotion_draw_velocity_scale;
#endif
int g_AI_inactive_time = 0;

Flags32 g_extraFeatures;

// g_spawn
class CCC_Spawn : public IConsole_Command {
public:
	CCC_Spawn(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR args) {
		if (!g_pGameLevel) return;


		if (!pSettings->section_exist(args))
		{
			Msg("! Section [%s] isn`t exist...", args);
			return;
		}

		char	Name[128];	Name[0] = 0;
		sscanf(args, "%s", Name);
		Fvector pos = Actor()->Position();
		pos.y += 3.0f;
		Level().g_cl_Spawn(Name, 0xff, M_SPAWN_OBJECT_LOCAL, pos);
	}
	virtual void	Info(TInfo& I)
	{
		strcpy(I, "name,team,squad,group");
	}
};
// g_spawn 

class CCC_Spawn_to_inventory : public IConsole_Command {
public:
	CCC_Spawn_to_inventory(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR args) {
		if (!g_pGameLevel)
		{
			Log("Error: No game level!");
			return;
		}

		if (!pSettings->section_exist(args))
		{
			Msg("! Section [%s] isn`t exist...", args);
			return;
		}

		char	Name[128];	Name[0] = 0;
		sscanf(args, "%s", Name);

		Level().spawn_item(Name, Actor()->Position(), false, Actor()->ID());
	}
	virtual void	Info(TInfo& I)
	{
		strcpy(I, "name,team,squad,group");
	}
};

class CCC_Giveinfo : public IConsole_Command {
public:
	CCC_Giveinfo(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR info_id) {
		if (!g_pGameLevel) return;

		char	Name[128];	Name[0] = 0;
		CActor* actor = smart_cast<CActor*>(Level().CurrentEntity());
		if (actor)
			actor->OnReceiveInfo(info_id);

	}
};

class CCC_Disinfo : public IConsole_Command {
public:
	CCC_Disinfo(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR info_id) {
		if (!g_pGameLevel) return;

		char	Name[128];	Name[0] = 0;
		CActor* actor = smart_cast<CActor*>(Level().CurrentEntity());
		if (actor)
			actor->OnDisableInfo(info_id);

	}
};

class CCC_GameLanguage : public CCC_Token 
{
public:
	CCC_GameLanguage(LPCSTR N) : CCC_Token(N, (u32*)&g_Language, language_type_token) {};
	virtual void Execute(LPCSTR args) 
	{
		CCC_Token::Execute(args);
		if (g_pGamePersistent && g_pGamePersistent->m_pMainMenu)
			MainMenu()->Activate(false);
		Msg("[GAME] Game language changed!");
		CStringTable().ReInit(g_Language);
		if (g_pGamePersistent && g_pGamePersistent->m_pMainMenu)
			MainMenu()->Activate(true);
	}
	virtual void Info(TInfo& I)
	{
		xr_strcpy(I, "Game language");
	}
};

// console commands
class CCC_GameDifficulty : public CCC_Token {
public:
	CCC_GameDifficulty(LPCSTR N) : CCC_Token(N,(u32*)&g_SingleGameDifficulty,difficulty_type_token)  {};
	virtual void Execute(LPCSTR args) {
		CCC_Token::Execute(args);
		if (g_pGameLevel && Level().game)
		{
			Level().game->OnDifficultyChanged	();
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		xr_strcpy(I,"game difficulty"); 
	}
};

#ifdef DEBUG
class CCC_ALifePath : public IConsole_Command {
public:
	CCC_ALifePath(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if (!ai().get_level_graph())
			Msg("! there is no graph!");
		else {
			int id1=-1, id2=-1;
			sscanf(args ,"%d %d",&id1,&id2);
			if ((-1 != id1) && (-1 != id2))
				if (std::max(id1,id2) > (int)ai().game_graph().header().vertex_count() - 1)
					Msg("! there are only %d vertexes!",ai().game_graph().header().vertex_count());
				else if (std::min(id1,id2) < 0)
						Msg("! invalid vertex number (%d)!", std::min(id1,id2));
			else
				Msg("! not enough parameters!");
		}
	}
};
#endif // DEBUG

class CCC_ALifeTimeFactor : public IConsole_Command
{
public:
	CCC_ALifeTimeFactor(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR args) 
	{
		if (!g_pGameLevel)
			return;

		float id1 = 0.0f;
		sscanf(args, "%f", &id1);
		if (id1 < EPS_L)
			Msg("Invalid time factor! (%.4f)", id1);
		else
			Level().SetGameTimeFactor(id1);
	}
	
	virtual void	Save(IWriter *F) {};
	virtual void	Status(TStatus& S)
	{	
		if (!g_pGameLevel)
			return;

		float v = Level().GetGameTimeFactor();
		xr_sprintf(S, sizeof(S), "%3.5f", v);
		while (xr_strlen(S) && ('0' == S[xr_strlen(S) - 1]))
			S[xr_strlen(S) - 1] = 0;
	}
	virtual void	Info(TInfo& I)
	{	
		if (!g_pGameLevel)
			return;

		float v = Level().GetGameTimeFactor();
		xr_sprintf(I, sizeof(I), " value = %3.5f", v);
	}
	virtual void	fill_tips(vecTips& tips, u32 mode)
	{
		if (!g_pGameLevel)
			return;

		float v = Level().GetGameTimeFactor();

		TStatus str;
		xr_sprintf(str, sizeof(str), "%3.5f  (current)  [0.0,1000.0]", v);
		tips.push_back(str);
		IConsole_Command::fill_tips(tips, mode);
	}
};

class CCC_ALifeSwitchDistance : public IConsole_Command 
{
public:
	CCC_ALifeSwitchDistance(LPCSTR N) : IConsole_Command(N) { };

	virtual void Execute(LPCSTR args)
	{
		if (ai().get_alife())
		{
			float id1 = 0.0f;
			sscanf(args, "%f", &id1);
			if (id1 < 2.0f)
				Msg("Invalid online distance! (%.4f)", id1);
			else
			{
				NET_Packet		P;
				P.w_begin(M_SWITCH_DISTANCE);
				P.w_float(id1);
				Level().Send(P);
			}
		}
	}
};

class CCC_ALifeProcessTime : public IConsole_Command
{
public:
	CCC_ALifeProcessTime(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR args) 
	{
		if (ai().get_alife())
		{
			int id1 = 0;
			sscanf(args ,"%d",&id1);
			if (id1 < 1)
				Msg("Invalid process time! (%d)",id1);
			else
				Level().Server->game->alife().set_process_time(id1);
		}
	}

};


class CCC_ALifeObjectsPerUpdate : public IConsole_Command
{
public:
	CCC_ALifeObjectsPerUpdate(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR args) 
	{
		if (ai().get_alife())
		{
			int id1 = 0;
			sscanf(args ,"%d",&id1);
			Level().Server->game->alife().objects_per_update(id1);
		}
	}
};

class CCC_ALifeSwitchFactor : public IConsole_Command
{
public:
	CCC_ALifeSwitchFactor(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR args) 
	{
		if (ai().get_alife())
		{
			float id1 = 0;
			sscanf(args ,"%f",&id1);
			clamp(id1,.1f,1.f);
			Level().Server->game->alife().set_switch_factor(id1);
		}
	}
};

//-----------------------------------------------------------------------
class CCC_DemoRecord : public IConsole_Command
{
public:

	CCC_DemoRecord(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args) 
	{
		if (g_pGameLevel)
		{
			Console->Hide();

			LPSTR			fn_;
			STRCONCAT(fn_, args, ".xrdemo");
			string_path		fn;
			FS.update_path(fn, "$game_saves$", fn_);

			g_pGameLevel->Cameras().AddCamEffector(xr_new<CDemoRecord>(fn));
		}
		else
			Msg("! There are no level(s) started");
	}
};

class CCC_DemoRecordSetPos : public CCC_Vector3
{
	static Fvector p;
public:

	CCC_DemoRecordSetPos(LPCSTR N) : CCC_Vector3( N, &p, Fvector().set( -FLT_MAX, -FLT_MAX, -FLT_MAX ),Fvector().set( FLT_MAX, FLT_MAX, FLT_MAX ) ) {};
	virtual void Execute(LPCSTR args) 
	{
		CDemoRecord::GetGlobalPosition( p );
		CCC_Vector3::Execute(args);
		CDemoRecord::SetGlobalPosition( p );
	}
	virtual void	Save	(IWriter *F)	{;}

};
Fvector CCC_DemoRecordSetPos::p = {0,0,0};

class CCC_DemoPlay : public IConsole_Command
{
public:
	CCC_DemoPlay(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args)
	{
		if (g_pGameLevel)
		{
			Console->Hide();
			string_path fn;
			u32 loops = 0;
			LPSTR comma = strchr(const_cast<LPSTR>(args), ',');
			if (comma)
			{
				loops = atoi(comma + 1);
				*comma = 0;	//. :)
			}
			strconcat(sizeof(fn), fn, args, ".xrdemo");
			FS.update_path(fn, "$game_saves$", fn);
			g_pGameLevel->Cameras().AddCamEffector(xr_new<CDemoPlay>(fn, 1.0f, loops));
		}
		else 
			Msg("! There are no level(s) started");
	}
};

// helper functions --------------------------------------------

bool valid_saved_game_name(LPCSTR file_name)
{
	LPCSTR		I = file_name;
	LPCSTR		E = file_name + xr_strlen(file_name);
	for ( ; I != E; ++I) {
		if (!strchr("/\\:*?\"<>|^()[]%",*I))
			continue;

		return	(false);
	};

	return		(true);
}

void get_files_list( xr_vector<shared_str>& files, LPCSTR dir, LPCSTR file_ext )
{
	VERIFY( dir && file_ext );
	files.clear();

	FS_Path* P = FS.get_path( dir );
	P->m_Flags.set( FS_Path::flNeedRescan, TRUE );
	FS.m_Flags.set( CLocatorAPI::flNeedCheck, TRUE );
	FS.rescan_pathes();

	LPCSTR fext;
	STRCONCAT( fext, "*", file_ext );

	FS_FileSet  files_set;
	FS.file_list( files_set, dir, FS_ListFiles, fext );
	u32 len_str_ext = xr_strlen( file_ext );

    auto itb = files_set.begin();
    auto ite = files_set.end();

	for( ; itb != ite; ++itb )
	{
		LPCSTR fn_ext = (*itb).name.c_str();
		VERIFY( xr_strlen(fn_ext) > len_str_ext );
		string_path fn;
		strncpy_s( fn, sizeof(fn), fn_ext, xr_strlen(fn_ext)-len_str_ext );
		files.push_back( fn );
	}
	FS.m_Flags.set( CLocatorAPI::flNeedCheck, FALSE );
}

#include "UIGame.h"

class CCC_ALifeSave : public IConsole_Command
{
public:
	CCC_ALifeSave(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {

		if (!g_actor || !Actor()->g_Alive())
		{
			Msg("cannot make saved game because actor is dead :(");
			return;
		}

		if (Actor()->HasInfo("cant_game_save_now"))
		{
			SDrawStaticStruct* _s = GameUI()->AddCustomStatic("game_saved", true);
			_s->wnd()->TextItemControl()->SetText(CStringTable().translate("st_cant_game_save_now").c_str());
			return;
		}

		string_path	S, S1;
		S[0] = 0;
		strncpy_s(S, sizeof(S), args, _MAX_PATH - 1);

#ifdef DEBUG
		CTimer timer;
		timer.Start();
#endif

		if (!xr_strlen(S)) 
		{
			strconcat(sizeof(S), S, Core.UserName, " - ", "quicksave");
			NET_Packet net_packet;
			net_packet.w_stringZ(S);
			net_packet.w_u8(0);
            if (ai().get_alife())
                Level().Server->game->alife().save(net_packet);
			//Level().Send(net_packet);
		}
		else 
		{
			if (!valid_saved_game_name(S))
			{
				Msg("! Save failed: invalid file name - %s", S);
				return;
			}

			NET_Packet net_packet;
			net_packet.w_stringZ(S);
			net_packet.w_u8(1);
            if (ai().get_alife())
                Level().Server->game->alife().save(net_packet);
			//Level().Send(net_packet);
		}
#ifdef DEBUG
		Msg("Game save overhead  : %f milliseconds", timer.GetElapsed_sec()*1000.f);
#endif
		SDrawStaticStruct* _s = GameUI()->AddCustomStatic("game_saved", true);
		LPSTR save_name;
		STRCONCAT(save_name, CStringTable().translate("st_game_saved").c_str(), ": ", S);
		_s->wnd()->TextItemControl()->SetText(save_name);

		xr_strcat(S, ".dds");
		FS.update_path(S1, "$game_saves$", S);

#ifdef DEBUG
		timer.Start();
#endif
		MainMenu()->Screenshot(IRender_interface::SM_FOR_GAMESAVE, S1);

#ifdef DEBUG
		Msg("Screenshot overhead : %f milliseconds", timer.GetElapsed_sec() * 1000.f);
#endif

	}//virtual void Execute

	virtual void fill_tips(vecTips& tips, u32 mode)
	{
		get_files_list(tips, "$game_saves$", SAVE_EXTENSION);
	}

};//CCC_ALifeSave

class CCC_ALifeLoadFrom : public IConsole_Command {
public:
	CCC_ALifeLoadFrom(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute( LPCSTR args )
	{
		string_path				saved_game;
		strncpy_s				(saved_game, sizeof(saved_game), args, _MAX_PATH - 1 );

		if (!ai().get_alife()) {
			Log						("! ALife simulator has not been started yet");
			return;
		}

		if (!xr_strlen(saved_game)) {
			Log						("! Specify file name!");
			return;
		}

		if (!CSavedGameWrapper::saved_game_exist(saved_game)) {
			Msg						("! Cannot find saved game %s",saved_game);
			return;
		}

		if (!CSavedGameWrapper::valid_saved_game(saved_game)) {
			Msg						("! Cannot load saved game %s, version mismatch or saved game is corrupted",saved_game);
			return;
		}

		if ( !valid_saved_game_name(saved_game) )
		{
			Msg						("! Cannot load saved game %s, invalid file name",saved_game);
			return;
		}

		if(MainMenu()->IsActive())
			MainMenu()->Activate(false);

		if (Device.Paused())
			Device.Pause			(FALSE, TRUE, TRUE, "CCC_ALifeLoadFrom");

		NET_Packet					net_packet;
		net_packet.w_begin			(M_LOAD_GAME);
		net_packet.w_stringZ		(saved_game);
		Level().Send				(net_packet);
	}
	
	virtual void fill_tips			(vecTips& tips, u32 mode)
	{
		get_files_list				(tips, "$game_saves$", SAVE_EXTENSION);
	}

};//CCC_ALifeLoadFrom

class CCC_LoadLastSave : public IConsole_Command {
public:
					CCC_LoadLastSave	(LPCSTR N) : IConsole_Command(N)
	{
		bEmptyArgsHandled		= true;
	}

	virtual void	Execute				(LPCSTR args)
	{
		string_path				saved_game = "";
		if ( args )
		{
			strncpy_s			(saved_game, sizeof(saved_game), args, _MAX_PATH - 1 );
		}

		
		if (saved_game && *saved_game)
		{
			xr_strcpy				(g_last_saved_game,saved_game);
			return;
		}

		if (!*g_last_saved_game) {
			Msg					("! cannot load last saved game since it hasn't been specified");
			return;
		}

		if (!CSavedGameWrapper::saved_game_exist(g_last_saved_game)) {
			Msg						("! Cannot find saved game %s",g_last_saved_game);
			return;
		}

		if (!CSavedGameWrapper::valid_saved_game(g_last_saved_game)) {
			Msg						("! Cannot load saved game %s, version mismatch or saved game is corrupted",g_last_saved_game);
			return;
		}

		if ( !valid_saved_game_name(g_last_saved_game) )
		{
			Msg						("! Cannot load saved game %s, invalid file name",g_last_saved_game);
			return;
		}

		LPSTR					command;
		if (ai().get_alife()) {
			STRCONCAT			(command, "load ", g_last_saved_game);
			Console->Execute	(command);
			return;
		}

		STRCONCAT				(command, "start server(", g_last_saved_game, "/single/alife/load)");
		Console->Execute		(command);
	}
	
	virtual void	Save				(IWriter *F)
	{
		if (!*g_last_saved_game)
			return;

		F->w_printf				("%s %s\r\n",cName,g_last_saved_game); 
	}
};

class CCC_FlushLog : public IConsole_Command {
public:
	CCC_FlushLog(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		FlushLog();
		Msg		("* Log file has been saved successfully!");
	}
};

class CCC_ClearLog : public IConsole_Command {
public:
	CCC_ClearLog(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR) {
		LogFile->clear	();
		FlushLog				();
		Msg						("* Log file has been cleaned successfully!");
	}
};

class CCC_FloatBlock : public CCC_Float {
public:
	CCC_FloatBlock(LPCSTR N, float* V, float _min=0, float _max=1) :
	  CCC_Float(N,V,_min,_max)
	  {};

	  virtual void	Execute	(LPCSTR args)
	  {
#ifdef _DEBUG
		  CCC_Float::Execute(args);
#else
		  if (!g_pGameLevel)	CCC_Float::Execute(args);
		  else 					Msg ("! Command disabled for this type of game");
#endif
	  }
};



class CCC_Net_CL_InputUpdateRate : public CCC_Integer {
protected:
	int		*value_blin;
public:
	CCC_Net_CL_InputUpdateRate(LPCSTR N, int* V, int _min=0, int _max=999) :
	  CCC_Integer(N,V,_min,_max),
		  value_blin(V)
	  {};

	  virtual void	Execute	(LPCSTR args)
	  {
		  CCC_Integer::Execute(args);
		  if ((*value_blin > 0) && g_pGameLevel)
		  {
			  g_dwInputUpdateDelta = 1000/(*value_blin);
		  };
	  }
};


#ifdef DEBUG

class CCC_DrawGameGraphAll : public IConsole_Command {
public:
				 CCC_DrawGameGraphAll	(LPCSTR N) : IConsole_Command(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute				(LPCSTR args)
	{
		if (!ai().get_level_graph())
			return;

		ai().level_graph().setup_current_level	(-1);
	}
};

class CCC_DrawGameGraphCurrent : public IConsole_Command {
public:
				 CCC_DrawGameGraphCurrent	(LPCSTR N) : IConsole_Command(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute					(LPCSTR args)
	{
		if (!ai().get_level_graph())
			return;

		ai().level_graph().setup_current_level	(
			ai().level_graph().level_id()
		);
	}
};

class CCC_DrawGameGraphLevel : public IConsole_Command {
public:
				 CCC_DrawGameGraphLevel	(LPCSTR N) : IConsole_Command(N)
	{
	}

	virtual void Execute					(LPCSTR args)
	{
		if (!ai().get_level_graph())
			return;

		if (!*args) {
			ai().level_graph().setup_current_level	(-1);
			return;
		}

		const GameGraph::SLevel	*level = ai().game_graph().header().level(args,true);
		if (!level) {
			Msg				("! There is no level %s in the game graph",args);
			return;
		}

		ai().level_graph().setup_current_level	(level->id());
	}
};

class CCC_DumpInfos : public IConsole_Command {
public:
	CCC_DumpInfos	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute				(LPCSTR args) {
		CActor* A =smart_cast<CActor*>(Level().CurrentEntity());
		if(A)
			A->DumpInfo();
	}
	virtual void	Info	(TInfo& I)		
	{
		xr_strcpy(I,"dumps all infoportions that actor have"); 
	}
};
class CCC_DumpTasks : public IConsole_Command {
public:
	CCC_DumpTasks	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute				(LPCSTR args) {
		CActor* A =smart_cast<CActor*>(Level().CurrentEntity());
		if(A)
			A->DumpTasks();
	}
	virtual void	Info	(TInfo& I)		
	{
		xr_strcpy(I,"dumps all tasks that actor have"); 
	}
};
#include "map_manager.h"
class CCC_DumpMap : public IConsole_Command {
public:
	CCC_DumpMap	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute				(LPCSTR args) {
		Level().MapManager().Dump();
	}
	virtual void	Info	(TInfo& I)		
	{
		xr_strcpy(I,"dumps all currentmap locations"); 
	}

};

#include "alife_graph_registry.h"
class CCC_DumpCreatures : public IConsole_Command {
public:
	CCC_DumpCreatures	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute				(LPCSTR args) {
		
		typedef CSafeMapIterator<ALife::_OBJECT_ID,CSE_ALifeDynamicObject>::_REGISTRY::const_iterator const_iterator;

		const_iterator I = ai().alife().graph().level().objects().begin();
		const_iterator E = ai().alife().graph().level().objects().end();
		for ( ; I != E; ++I) {
			CSE_ALifeCreatureAbstract *obj = smart_cast<CSE_ALifeCreatureAbstract *>(I->second);
			if (obj) {
				Msg("\"%s\",",obj->name_replace());
			}
		}		

	}
	virtual void	Info	(TInfo& I)		
	{
		xr_strcpy(I,"dumps all creature names"); 
	}

};



class CCC_DebugFonts : public IConsole_Command {
public:
	CCC_DebugFonts (LPCSTR N) : IConsole_Command(N) {bEmptyArgsHandled = true; }
	virtual void Execute				(LPCSTR args) 
	{
		xr_new<CUIDebugFonts>()->ShowDialog(true);		
	}
};

class CCC_DebugNode : public IConsole_Command {
public:
	CCC_DebugNode(LPCSTR N) : IConsole_Command(N)  { };

	virtual void Execute(LPCSTR args) {

		string128 param1, param2;
		VERIFY( xr_strlen(args) < sizeof(string128) );

		_GetItem(args,0,param1,' ');
		_GetItem(args,1,param2,' ');

		u32 value1;
		u32 value2;
		
		sscanf(param1,"%u",&value1);
		sscanf(param2,"%u",&value2);
		
		if ((value1 > 0) && (value2 > 0)) {
			g_bDebugNode		= TRUE;
			g_dwDebugNodeSource	= value1;
			g_dwDebugNodeDest	= value2;
		} else {
			g_bDebugNode = FALSE;
		}
	}
};

class CCC_ShowMonsterInfo : public IConsole_Command {
public:
				CCC_ShowMonsterInfo(LPCSTR N) : IConsole_Command(N)  { };

	virtual void Execute(LPCSTR args) {

		string128 param1, param2;
		VERIFY( xr_strlen(args) < sizeof(string128) );

		_GetItem(args,0,param1,' ');
		_GetItem(args,1,param2,' ');

		CObject			*obj = Level().Objects.FindObjectByName(param1);
		CBaseMonster	*monster = smart_cast<CBaseMonster *>(obj);
		if (!monster)	return;
		
		u32				value2;
		
		sscanf			(param2,"%u",&value2);
		monster->set_show_debug_info (u8(value2));
	}
};

void PH_DBG_SetTrackObject();
extern string64 s_dbg_trace_obj_name;
class CCC_DbgPhTrackObj : public CCC_String {
public:
	CCC_DbgPhTrackObj( LPCSTR N ) : CCC_String( N, s_dbg_trace_obj_name, sizeof(s_dbg_trace_obj_name) )  { };
	virtual void Execute(LPCSTR args/**/) {
		CCC_String::Execute( args );
			if(!xr_strcmp(args,"none"))
			{
				ph_dbg_draw_mask1.set(ph_m1_DbgTrackObject,FALSE);
				return;
			}
			ph_dbg_draw_mask1.set(ph_m1_DbgTrackObject,TRUE);
			PH_DBG_SetTrackObject();
		}
};
#endif

class CCC_PHIterations : public CCC_Integer {
public:
		CCC_PHIterations(LPCSTR N) :
		CCC_Integer(N,&phIterations,15,50)
		{};
	  virtual void	Execute	(LPCSTR args)
	  {
		  CCC_Integer::Execute	(args);
		  if( physics_world() )
				 physics_world()->StepNumIterations( phIterations );
	  }
};

class CCC_PHGravity : public IConsole_Command {
public:
		CCC_PHGravity(LPCSTR N) :
		IConsole_Command(N)
		{};
	  virtual void	Execute	(LPCSTR args)
	  {
		  if( !physics_world() )	
			  return;
		  
		  physics_world()->SetGravity(float(atof(args)));
	  }
	  virtual void	Status	(TStatus& S)
	{	
		if(physics_world())
			xr_sprintf	(S,"%3.5f",physics_world()->Gravity());
		else
			xr_sprintf	(S,"%3.5f",default_world_gravity);
		while	(xr_strlen(S) && ('0'==S[xr_strlen(S)-1]))	S[xr_strlen(S)-1] = 0;
	}
	
};

class CCC_PHFps : public IConsole_Command {
public:
	CCC_PHFps(LPCSTR N) :
	  IConsole_Command(N)
	  {};
	  virtual void	Execute	(LPCSTR args)
	  {
		  float				step_count = (float)atof(args);
#ifndef DEBUG
		  clamp				(step_count,50.f,200.f);
#endif
		  ph_console::ph_step_time = 1.f/step_count;
		  if(physics_world())
			 physics_world()->SetStep(ph_console::ph_step_time);
	  }
	  virtual void	Status	(TStatus& S)
	  {	
		 	xr_sprintf	(S,"%3.5f",1.f/ph_console::ph_step_time);	  
	  }

};

#include "game_graph.h"
struct CCC_JumpToLevel : public IConsole_Command {
	CCC_JumpToLevel(LPCSTR N) : IConsole_Command(N)  {};

	virtual void Execute(LPCSTR level)
	{
		if ( !ai().get_alife() )
		{
			Msg				("! ALife simulator is needed to perform specified command!");
			return;
		}

		GameGraph::LEVEL_MAP::const_iterator	I = ai().game_graph().header().levels().begin();
		GameGraph::LEVEL_MAP::const_iterator	E = ai().game_graph().header().levels().end();
		for ( ; I != E; ++I )
			if ( !xr_strcmp((*I).second.name(),level) )
			{
				ai().alife().jump_to_level(level);
				return;
			}
		Msg							("! There is no level \"%s\" in the game graph!",level);
	}

	virtual void	Save	(IWriter *F)	{};
	virtual void	fill_tips(vecTips& tips, u32 mode)
	{
		if ( !ai().get_alife() )
		{
			Msg				("! ALife simulator is needed to perform specified command!");
			return;
		}

		GameGraph::LEVEL_MAP::const_iterator	itb = ai().game_graph().header().levels().begin();
		GameGraph::LEVEL_MAP::const_iterator	ite = ai().game_graph().header().levels().end();
		for ( ; itb != ite; ++itb )
		{
			tips.push_back( (*itb).second.name() );
		}
	}

};

class CCC_Script : public IConsole_Command {
public:
	CCC_Script(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute( LPCSTR args )
	{
		if ( !xr_strlen(args) )
		{
			Log("* Specify script name!");
		}
		else
		{
			// rescan pathes
			FS_Path* P = FS.get_path("$game_scripts$");
			P->m_Flags.set	(FS_Path::flNeedRescan,TRUE);
			FS.rescan_pathes();
			// run script
			if (ai().script_engine().script_process(ScriptEngine::eScriptProcessorLevel))
				ai().script_engine().script_process(ScriptEngine::eScriptProcessorLevel)->add_script(args,false,true);
		}
	}

	virtual void Status( TStatus& S )
	{
		xr_strcpy( S, "<script_name> (Specify script name!)" );
	}
	virtual void Save( IWriter* F ) {}

	virtual void fill_tips( vecTips& tips, u32 mode )
	{
		get_files_list( tips, "$game_scripts$", ".script" );
	}

};

class CCC_ScriptCommand : public IConsole_Command {
public:
	CCC_ScriptCommand	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	
	virtual void	Execute				(LPCSTR args)
	{
		if (!xr_strlen(args))
			Log("* Specify string to run!");
		else {
			if (ai().script_engine().script_process(ScriptEngine::eScriptProcessorLevel)) {
				ai().script_engine().script_process(ScriptEngine::eScriptProcessorLevel)->add_script(args,true,true);
				return;
			}

			string4096 S;
			shared_str m_script_name = "console command";
			xr_sprintf(S,"%s\n",args);
			int l_iErrorCode = luaL_loadbuffer(ai().script_engine().lua(),S,xr_strlen(S),"@console_command");
			if (!l_iErrorCode) 
			{
				l_iErrorCode = lua_pcall(ai().script_engine().lua(),0,0,0);
				if (l_iErrorCode) 
				{
					ai().script_engine().print_output	(ai().script_engine().lua(),*m_script_name,l_iErrorCode);
					return;
				}
			}

			ai().script_engine().print_output	(ai().script_engine().lua(),*m_script_name,l_iErrorCode);
		}
	}

	virtual void Status( TStatus& S )
	{
		xr_strcpy( S, "<script_name.function()> (Specify script and function name!)" );
	}
	virtual void Save( IWriter* F ) {}

	virtual void fill_tips( vecTips& tips, u32 mode )
	{
		if ( mode == 1 )
		{
			get_files_list( tips, "$game_scripts$", ".script" );
			return;
		}

		IConsole_Command::fill_tips( tips, mode );
	}
};

class CCC_TimeFactor : public IConsole_Command {
public:
					CCC_TimeFactor	(LPCSTR N) : IConsole_Command(N) {}
	virtual void	Execute			(LPCSTR args)
	{
		float				time_factor = (float)atof(args);
		clamp				(time_factor,EPS,1000.f);
		Device.time_factor	(time_factor);
		    psSpeedOfSound	= time_factor;
	}
	virtual void	Status			(TStatus &S)
	{
		xr_sprintf	(S,sizeof(S),"%f",Device.time_factor());
	}

	virtual void	Info	(TInfo& I)
	{
		xr_strcpy				(I,"[0.001 - 1000.0]");
	}
	
	virtual void	fill_tips(vecTips& tips, u32 mode)
	{
		TStatus  str;
		xr_sprintf( str, sizeof(str), "%3.3f  (current)  [0.001 - 1000.0]", Device.time_factor() );
		tips.push_back( str );
		IConsole_Command::fill_tips( tips, mode );
	}
};

#include "GamePersistent.h"


class CCC_MainMenu : public IConsole_Command {
public:
	CCC_MainMenu(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {

		bool bWhatToDo = TRUE;
		if( 0==xr_strlen(args) ){
			bWhatToDo = !MainMenu()->IsActive();
		};

		if( EQ(args,"on")||EQ(args,"1") )
			bWhatToDo = TRUE;

		if( EQ(args,"off")||EQ(args,"0") )
			bWhatToDo = FALSE;

		MainMenu()->Activate( bWhatToDo );
	}
};

struct CCC_StartTimeSingle : public IConsole_Command {
	CCC_StartTimeSingle(LPCSTR N) : IConsole_Command(N) {};
	virtual void	Execute	(LPCSTR args)
	{
		u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
		sscanf				(args,"%d.%d.%d %d:%d:%d.%d",&year,&month,&day,&hours,&mins,&secs,&milisecs);
		year				= std::max(year,(u32)1);
		month				= std::max(month,(u32)1);
		day					= std::max(day,(u32)1);
		g_qwStartGameTime	= generate_time	(year,month,day,hours,mins,secs,milisecs);

		if (!g_pGameLevel)
			return;

		if (!Level().Server)
			return;

		if (!Level().Server->game)
			return;

		Level().SetGameTimeFactor(g_qwStartGameTime,g_fTimeFactor);
	}

	virtual void	Status	(TStatus& S)
	{
		u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
		split_time	(g_qwStartGameTime, year, month, day, hours, mins, secs, milisecs);
		xr_sprintf		(S,"%d.%d.%d %d:%d:%d.%d",year,month,day,hours,mins,secs,milisecs);
	}
};

struct CCC_TimeFactorSingle : public CCC_Float {
	CCC_TimeFactorSingle(LPCSTR N, float* V, float _min=0.f, float _max=1.f) : CCC_Float(N,V,_min,_max) {};

	virtual void	Execute	(LPCSTR args)
	{
		CCC_Float::Execute	(args);
		
		if (!g_pGameLevel)
			return;

		if (!Level().Server)
			return;

		if (!Level().Server->game)
			return;

		Level().SetGameTimeFactor(g_fTimeFactor);
	}

};

#ifdef DEBUG
class CCC_RadioGroupMask2;
class CCC_RadioMask :public CCC_Mask
{
	CCC_RadioGroupMask2		*group;
public:
	CCC_RadioMask(LPCSTR N, Flags32* V, u32 M):
	  CCC_Mask(N,V,M)
	 {
		group=NULL;
	 }
		void	SetGroup	(CCC_RadioGroupMask2		*G)
	{
		group=G													;
	}
virtual	void	Execute		(LPCSTR args)						;
	
IC		void	Set			(BOOL V)
	  {
		  value->set(mask,V)									;
	  }

};

class CCC_RadioGroupMask2 
{
	CCC_RadioMask *mask0;
	CCC_RadioMask *mask1;
public:
	CCC_RadioGroupMask2(CCC_RadioMask *m0,CCC_RadioMask *m1)
	  {
		mask0=m0;mask1=m1;
		mask0->SetGroup(this);
		mask1->SetGroup(this);
	  }
	void	Execute	(CCC_RadioMask& m,LPCSTR args)
	{
		BOOL value=m.GetValue();
		if(value)
		{
			mask0->Set(!value);mask1->Set(!value);
		}
		m.Set(value);
	}
};


void	CCC_RadioMask::Execute	(LPCSTR args)
{
	CCC_Mask::Execute(args);
	VERIFY2(group,"CCC_RadioMask: group not set");
	group->Execute(*this,args);
}

#define CMD_RADIOGROUPMASK2(p1,p2,p3,p4,p5,p6)		\
{\
static CCC_RadioMask x##CCC_RadioMask1(p1,p2,p3);		Console->AddCommand(&x##CCC_RadioMask1);\
static CCC_RadioMask x##CCC_RadioMask2(p4,p5,p6);		Console->AddCommand(&x##CCC_RadioMask2);\
static CCC_RadioGroupMask2 x##CCC_RadioGroupMask2(&x##CCC_RadioMask1,&x##CCC_RadioMask2);\
}

struct CCC_DbgBullets : public CCC_Integer {
	CCC_DbgBullets(LPCSTR N, int* V, int _min=0, int _max=999) : CCC_Integer(N,V,_min,_max) {};

	virtual void	Execute	(LPCSTR args)
	{
		extern FvectorVec g_hit[];
		g_hit[0].clear();
		g_hit[1].clear();
		g_hit[2].clear();
		CCC_Integer::Execute	(args);
	}
};
#endif

#include "attachable_item.h"
#include "attachment_owner.h"
#include "InventoryOwner.h"
#include "Inventory.h"
class CCC_TuneAttachableItem : public IConsole_Command
{
public:
	CCC_TuneAttachableItem(LPCSTR N) :IConsole_Command(N) {};
	virtual void Execute(LPCSTR args)
	{
		if (!g_pGameLevel)
			return;

		if (CAttachableItem::m_dbgItem)
		{
			CAttachableItem::m_dbgItem = NULL;	
			Msg("CCC_TuneAttachableItem switched to off");
			return;
		};

		CObject* obj			= Level().CurrentViewEntity();	VERIFY(obj);
		shared_str ssss			= args;

		CAttachmentOwner* owner = smart_cast<CAttachmentOwner*>(obj);
		CAttachableItem* itm	= owner->attachedItem(ssss);
		if(itm)
		{
			CAttachableItem::m_dbgItem = itm;
		}else
		{
			CInventoryOwner* iowner = smart_cast<CInventoryOwner*>(obj);
			PIItem active_item = iowner->m_inventory->ActiveItem();
			if(active_item && active_item->object().cNameSect()==ssss )
				CAttachableItem::m_dbgItem = active_item->cast_attachable_item();
		}

		if(CAttachableItem::m_dbgItem)
			Msg("CCC_TuneAttachableItem switched to ON for [%s]",args);
		else
			Msg("CCC_TuneAttachableItem cannot find attached item [%s]",args);
	}

	virtual void	Info	(TInfo& I)
	{	
		xr_sprintf(I,"allows to change bind rotation and position offsets for attached item, <section_name> given as arguments");
	}
};

#ifdef DEBUG
class CCC_Crash : public IConsole_Command {
public:
	CCC_Crash(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		VERIFY3					(false,"This is a test crash","Do not post it as a bug");
		int						*pointer = 0;
		*pointer				= 0;
	}
};

class CCC_DumpModelBones : public IConsole_Command {
public:
	CCC_DumpModelBones	(LPCSTR N) : IConsole_Command(N)
	{
	}
	
	virtual void Execute(LPCSTR arguments)
	{
		if (!arguments || !*arguments) {
			Msg					("! no arguments passed");
			return;
		}

		LPCSTR					name;

		if (0==strext(arguments))
			STRCONCAT			(name, arguments, ".ogf");
		else
			STRCONCAT			(name, arguments);

		string_path				fn;

		if (!FS.exist(arguments) && !FS.exist(fn, "$level$", name) && !FS.exist(fn, "$game_meshes$", name)) {
			Msg					("! Cannot find visual \"%s\"",arguments);
			return;
		}

		IRenderVisual			*visual = Render->model_Create(arguments);
		IKinematics				*kinematics = smart_cast<IKinematics*>(visual);
		if (!kinematics) {
			Render->model_Delete(visual);
			Msg					("! Invalid visual type \"%s\" (not a IKinematics)",arguments);
			return;
		}

		Msg						("bones for model \"%s\"",arguments);
		for (u16 i=0, n=kinematics->LL_BoneCount(); i<n; ++i)
			Msg					("%s",*kinematics->LL_GetData(i).name);
		
		Render->model_Delete	(visual);
	}
};

extern void show_animation_stats	();

class CCC_ShowAnimationStats : public IConsole_Command {
public:
	CCC_ShowAnimationStats(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR)
	{
		show_animation_stats	();
	}
};

class CCC_InvUpgradesHierarchy : public IConsole_Command
{
public:
	CCC_InvUpgradesHierarchy(LPCSTR N) : IConsole_Command(N)	{ bEmptyArgsHandled = TRUE; };
	virtual void Execute( LPCSTR args )
	{
		if ( ai().get_alife() )
		{
			ai().alife().inventory_upgrade_manager().log_hierarchy();
		}
	}

};

class CCC_InvUpgradesCurItem : public IConsole_Command
{
public:
	CCC_InvUpgradesCurItem(LPCSTR N) : IConsole_Command(N)	{ bEmptyArgsHandled = TRUE; };
	virtual void Execute( LPCSTR args )
	{
		if ( !g_pGameLevel )
		{
			return;
		}

		PIItem item = GameUI()->ActorMenu().get_upgrade_item();
		if ( item )
		{
			item->log_upgrades();
		}
		else
		{
			Msg( "- Current item in ActorMenu is unknown!" );
		}
	}
};

class CCC_InvDropAllItems : public IConsole_Command
{
public:
	CCC_InvDropAllItems(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args)
	{
		if (!g_pGameLevel)
		{
			return;
		}

		int d = 0;
		sscanf(args, "%d", &d);
		if (GameUI()->ActorMenu().DropAllItemsFromRuck(d == 1))
		{
			Msg("- All items from ruck of Actor is dropping now.");
		}
		else
		{
			Msg("! ActorMenu is not in state `Inventory`");
		}
	}
};

class CCC_DumpObjects : public IConsole_Command {
public:
	CCC_DumpObjects(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR)
	{
		Level().Objects.dump_all_objects();
	}
};

void DBG_CashedClear();
class CCC_DBGDrawCashedClear : public IConsole_Command {
public:
	CCC_DBGDrawCashedClear(LPCSTR N) :IConsole_Command(N)  { bEmptyArgsHandled =true; }
private:
	 virtual void	Execute	(LPCSTR args)
	 {
		 DBG_CashedClear();
	 }
};

class CCC_DbgVar : public IConsole_Command {
public:
	CCC_DbgVar(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR arguments) 
	{
		if (!arguments || !*arguments)
		{
			return;
		}

		if ( _GetItemCount(arguments, ' ') == 1 )
		{
			ai_dbg::show_var(arguments);
		}
		else
		{
			char  name[1024];
			float f;
			sscanf	(arguments, "%s %f", name, &f);
			ai_dbg::set_var(name, f);
		}

	}
};
#endif

// Change weather immediately
class CCC_SetWeather : public IConsole_Command
{
public:
    CCC_SetWeather(LPCSTR N) : IConsole_Command(N) {};
    virtual void Execute(LPCSTR args)
    {
        if (!xr_strlen(args))
            return;

        if (!Device.editor())
            g_pGamePersistent->Environment().SetWeather(args, true);
    }

	virtual void fill_tips(vecTips& tips, u32 mode)
	{
		get_files_list(tips, "$game_weathers$", ".ltx");
	}
};

void CCC_RegisterCommands()
{
	// game
	CMD3(CCC_Mask, "g_crouch_toggle", &psActorFlags, AF_CROUCH_TOGGLE);
	CMD3(CCC_Mask, "g_colored_feel", &psActorFlags, AF_COLORED_FEEL);
	CMD1(CCC_GameDifficulty, "g_game_difficulty");
	CMD1(CCC_GameLanguage, "g_game_languages");

	CMD3(CCC_Mask, "g_backrun", &psActorFlags, AF_RUN_BACKWARD);

	// alife
#ifdef DEBUG
	CMD1(CCC_ALifePath, "al_path");		// build path

#endif // DEBUG

	CMD1(CCC_ALifeSave, "save");		// save game
	CMD1(CCC_ALifeLoadFrom, "load");		// load game from ...
	CMD1(CCC_LoadLastSave, "load_last_save");		// load last saved game from ...

	CMD1(CCC_FlushLog, "flush");		// flush log
	CMD1(CCC_ClearLog, "clear_log");


	CMD1(CCC_ALifeTimeFactor, "al_time_factor");		// set time factor
	CMD1(CCC_ALifeSwitchDistance, "al_switch_distance");		// set switch distance
	CMD1(CCC_ALifeProcessTime, "al_process_time");		// set process time
	CMD1(CCC_ALifeObjectsPerUpdate, "al_objects_per_update");		// set process time
	CMD1(CCC_ALifeSwitchFactor, "al_switch_factor");		// set switch factor



	CMD3(CCC_Mask, "hud_weapon", &psHUD_Flags, HUD_WEAPON);
	CMD3(CCC_Mask, "hud_info", &psHUD_Flags, HUD_INFO);
	CMD3(CCC_Mask, "hud_draw", &psHUD_Flags, HUD_DRAW);

	// hud
	psHUD_Flags.set(HUD_CROSSHAIR, true);
	psHUD_Flags.set(HUD_WEAPON, true);
	psHUD_Flags.set(HUD_DRAW, true);
	psHUD_Flags.set(HUD_INFO, true);

	CMD3(CCC_Mask, "hud_crosshair", &psHUD_Flags, HUD_CROSSHAIR);
	CMD3(CCC_Mask, "hud_crosshair_dist", &psHUD_Flags, HUD_CROSSHAIR_DIST);

	CMD4(CCC_Float, "hud_fov", &psHUD_FOV_def, 0.1f, 1.0f);
	CMD4(CCC_Float, "fov", &g_fov, 5.0f, 120.0f);

	// Demo
	CMD1(CCC_DemoPlay, "demo_play");
	CMD1(CCC_DemoRecord, "demo_record");
	CMD1(CCC_DemoRecordSetPos, "demo_set_cam_position");

#ifndef MASTER_GOLD
	CMD3(CCC_Mask, "ai_obstacles_avoiding", &psAI_Flags, aiObstaclesAvoiding);
	CMD3(CCC_Mask, "ai_obstacles_avoiding_static", &psAI_Flags, aiObstaclesAvoidingStatic);
	CMD3(CCC_Mask, "ai_use_smart_covers", &psAI_Flags, aiUseSmartCovers);
	CMD3(CCC_Mask, "ai_use_smart_covers_animation_slots", &psAI_Flags, (u32)aiUseSmartCoversAnimationSlot);
	CMD4(CCC_Float, "ai_smart_factor", &g_smart_cover_factor, 0.f, 1000000.f);
	CMD3(CCC_Mask, "ai_dbg_lua", &psAI_Flags, aiLua);
#endif // MASTER_GOLD

#ifdef DEBUG
	CMD4(CCC_Integer, "lua_gcstep", &psLUA_GCSTEP, 1, 1000);
	CMD3(CCC_Mask, "ai_debug", &psAI_Flags, aiDebug);
	CMD3(CCC_Mask, "ai_dbg_brain", &psAI_Flags, aiBrain);
	CMD3(CCC_Mask, "ai_dbg_motion", &psAI_Flags, aiMotion);
	CMD3(CCC_Mask, "ai_dbg_frustum", &psAI_Flags, aiFrustum);
	CMD3(CCC_Mask, "ai_dbg_funcs", &psAI_Flags, aiFuncs);
	CMD3(CCC_Mask, "ai_dbg_alife", &psAI_Flags, aiALife);
	CMD3(CCC_Mask, "ai_dbg_goap", &psAI_Flags, aiGOAP);
	CMD3(CCC_Mask, "ai_dbg_goap_script", &psAI_Flags, aiGOAPScript);
	CMD3(CCC_Mask, "ai_dbg_goap_object", &psAI_Flags, aiGOAPObject);
	CMD3(CCC_Mask, "ai_dbg_cover", &psAI_Flags, aiCover);
	CMD3(CCC_Mask, "ai_dbg_anim", &psAI_Flags, aiAnimation);
	CMD3(CCC_Mask, "ai_dbg_vision", &psAI_Flags, aiVision);
	CMD3(CCC_Mask, "ai_dbg_monster", &psAI_Flags, aiMonsterDebug);
	CMD3(CCC_Mask, "ai_dbg_stalker", &psAI_Flags, aiStalker);
	CMD3(CCC_Mask, "ai_stats", &psAI_Flags, aiStats);
	CMD3(CCC_Mask, "ai_dbg_destroy", &psAI_Flags, aiDestroy);
	CMD3(CCC_Mask, "ai_dbg_serialize", &psAI_Flags, aiSerialize);
	CMD3(CCC_Mask, "ai_dbg_dialogs", &psAI_Flags, aiDialogs);
	CMD3(CCC_Mask, "ai_dbg_infoportion", &psAI_Flags, aiInfoPortion);

	CMD3(CCC_Mask, "ai_draw_game_graph", &psAI_Flags, aiDrawGameGraph);
	CMD3(CCC_Mask, "ai_draw_game_graph_stalkers", &psAI_Flags, aiDrawGameGraphStalkers);
	CMD3(CCC_Mask, "ai_draw_game_graph_objects", &psAI_Flags, aiDrawGameGraphObjects);
	CMD3(CCC_Mask, "ai_draw_game_graph_real_pos", &psAI_Flags, aiDrawGameGraphRealPos);

	CMD3(CCC_Mask, "ai_nil_object_access", &psAI_Flags, aiNilObjectAccess);

	CMD3(CCC_Mask, "ai_draw_visibility_rays", &psAI_Flags, aiDrawVisibilityRays);
	CMD3(CCC_Mask, "ai_animation_stats", &psAI_Flags, aiAnimationStats);

	// HIT ANIMATION
	CMD4(CCC_Float, "hit_anims_power", &ghit_anims_params.power_factor, 0.0f, 100.0f);
	CMD4(CCC_Float, "hit_anims_rotational_power", &ghit_anims_params.rotational_power_factor, 0.0f, 100.0f);
	CMD4(CCC_Float, "hit_anims_side_sensitivity_threshold", &ghit_anims_params.side_sensitivity_threshold, 0.0f, 10.0f);
	CMD4(CCC_Float, "hit_anims_channel_factor", &ghit_anims_params.anim_channel_factor, 0.0f, 100.0f);
	CMD4(CCC_Float, "hit_anims_block_blend", &ghit_anims_params.block_blend, 0.f, 1.f);
	CMD4(CCC_Float, "hit_anims_reduce_blend", &ghit_anims_params.reduce_blend, 0.f, 1.f);
	CMD4(CCC_Float, "hit_anims_reduce_blend_factor", &ghit_anims_params.reduce_power_factor, 0.0f, 1.0f);
	CMD4(CCC_Integer, "hit_anims_tune", &tune_hit_anims, 0, 1);
	/////////////////////////////////////////////HIT ANIMATION END////////////////////////////////////////////////////
	CMD1(CCC_DumpModelBones, "debug_dump_model_bones");

	CMD1(CCC_DrawGameGraphAll, "ai_draw_game_graph_all");
	CMD1(CCC_DrawGameGraphCurrent, "ai_draw_game_graph_current_level");
	CMD1(CCC_DrawGameGraphLevel, "ai_draw_game_graph_level");

	CMD4(CCC_Integer, "ai_dbg_inactive_time", &g_AI_inactive_time, 0, 1000000);

	CMD1(CCC_DebugNode, "ai_dbg_node");

	CMD1(CCC_ShowMonsterInfo, "ai_monster_info");
	CMD1(CCC_DebugFonts, "debug_fonts");

	CMD1(CCC_ShowAnimationStats, "ai_show_animation_stats");
#endif // DEBUG

	CMD1(CCC_TuneAttachableItem, "dbg_adjust_attachable_item");

#ifndef MASTER_GOLD
	CMD3(CCC_Mask, "ai_ignore_actor", &psAI_Flags, aiIgnoreActor);
#endif // MASTER_GOLD

	// Physics
	CMD1(CCC_PHFps, "ph_frequency");
	CMD1(CCC_PHIterations, "ph_iterations");
	CMD1(CCC_PHGravity, "ph_gravity");
	CMD4(CCC_FloatBlock, "ph_timefactor", &phTimefactor, 0.000001f, 1000.f);

#ifdef DEBUG
	CMD4(CCC_FloatBlock, "ph_break_common_factor", &ph_console::phBreakCommonFactor, 0.f, 1000000000.f);
	CMD4(CCC_FloatBlock, "ph_rigid_break_weapon_factor", &ph_console::phRigidBreakWeaponFactor, 0.f, 1000000000.f);
	CMD4(CCC_Integer, "ph_tri_clear_disable_count", &ph_console::ph_tri_clear_disable_count, 0, 255);
	CMD4(CCC_FloatBlock, "ph_tri_query_ex_aabb_rate", &ph_console::ph_tri_query_ex_aabb_rate, 1.01f, 3.f);
#endif // DEBUG
	CMD3(CCC_Mask, "g_no_clip", &psActorFlags, AF_NO_CLIP);
	CMD3(CCC_Mask, "rs_car_info", &psActorFlags, AF_CAR_INFO);
	CMD1(CCC_JumpToLevel, "jump_to_level");
	CMD3(CCC_Mask, "g_god", &psActorFlags, AF_GODMODE);
	CMD3(CCC_Mask, "g_unlimitedammo", &psActorFlags, AF_UNLIMITEDAMMO);
	CMD3(CCC_Mask, "g_reload_on_sprint", &psActorFlags, AF_RELOADONSPRINT);
	CMD3(CCC_Mask, "cursor_instead_crosshair", &psActorFlags, AF_CUR_INS_CROS);
	CMD1(CCC_Script, "run_script");
	CMD1(CCC_ScriptCommand, "run_string");
	CMD3(CCC_Mask, "rs_show_cursor_pos", &psActorFlags, AF_SHOW_CURPOS);
	CMD3(CCC_Mask, "g_hardcore_mode", &psActorFlags, AF_HARDCORE);
    CMD3(CCC_Mask, "rs_wip", &psActorFlags, AF_WORKINPROGRESS);
    CMD3(CCC_Mask, "rs_clearskyinterface", &psActorFlags, AF_CLEARSKYINTERFACE);
    CMD3(CCC_Mask, "rs_showdate", &psActorFlags, AF_SHOWDATE);
	CMD1(CCC_TimeFactor, "time_factor");
	CMD1(CCC_Spawn, "g_spawn");
	CMD1(CCC_Spawn_to_inventory, "g_spawn_to_inventory");
	CMD1(CCC_Giveinfo, "g_info");
	CMD1(CCC_Disinfo, "d_info");
	CMD3(CCC_Mask, "g_autopickup", &psActorFlags, AF_AUTOPICKUP);
	CMD3(CCC_Mask, "g_dynamic_music", &psActorFlags, AF_DYNAMIC_MUSIC);
	CMD3(CCC_Mask, "g_important_save", &psActorFlags, AF_IMPORTANT_SAVE);

	CMD3(CCC_Mask, "ts_get_object_params", &psActorFlags, AF_GET_OBJECT_PARAMS);
	CMD3(CCC_Mask, "ts_show_boss_health", &psActorFlags, AF_SHOW_BOSS_HEALTH);
	CMD3(CCC_Mask, "g_right_shoulder", &psActorFlags, AF_RIGHT_SHOULDER);
	CMD3(CCC_Mask, "g_fp2_zoom_forced", &psActorFlags, AF_FP2ZOOM_FORCED);


#ifdef DEBUG

	CMD3(CCC_Mask, "dbg_draw_actor_alive", &dbg_net_Draw_Flags, dbg_draw_actor_alive);
	CMD3(CCC_Mask, "dbg_draw_actor_dead", &dbg_net_Draw_Flags, dbg_draw_actor_dead);
	CMD3(CCC_Mask, "dbg_draw_customzone", &dbg_net_Draw_Flags, dbg_draw_customzone);
	CMD3(CCC_Mask, "dbg_draw_teamzone", &dbg_net_Draw_Flags, dbg_draw_teamzone);
	CMD3(CCC_Mask, "dbg_draw_invitem", &dbg_net_Draw_Flags, dbg_draw_invitem);
	CMD3(CCC_Mask, "dbg_draw_actor_phys", &dbg_net_Draw_Flags, dbg_draw_actor_phys);
	CMD3(CCC_Mask, "dbg_draw_customdetector", &dbg_net_Draw_Flags, dbg_draw_customdetector);
	CMD3(CCC_Mask, "dbg_destroy", &dbg_net_Draw_Flags, dbg_destroy);
	CMD3(CCC_Mask, "dbg_draw_autopickupbox", &dbg_net_Draw_Flags, dbg_draw_autopickupbox);
	CMD3(CCC_Mask, "dbg_draw_rp", &dbg_net_Draw_Flags, dbg_draw_rp);
	CMD3(CCC_Mask, "dbg_draw_climbable", &dbg_net_Draw_Flags, dbg_draw_climbable);
	CMD3(CCC_Mask, "dbg_draw_skeleton", &dbg_net_Draw_Flags, dbg_draw_skeleton);


	CMD3(CCC_Mask, "dbg_draw_ph_contacts", &ph_dbg_draw_mask, phDbgDrawContacts);
	CMD3(CCC_Mask, "dbg_draw_ph_enabled_aabbs", &ph_dbg_draw_mask, phDbgDrawEnabledAABBS);
	CMD3(CCC_Mask, "dbg_draw_ph_intersected_tries", &ph_dbg_draw_mask, phDBgDrawIntersectedTries);
	CMD3(CCC_Mask, "dbg_draw_ph_saved_tries", &ph_dbg_draw_mask, phDbgDrawSavedTries);
	CMD3(CCC_Mask, "dbg_draw_ph_tri_trace", &ph_dbg_draw_mask, phDbgDrawTriTrace);
	CMD3(CCC_Mask, "dbg_draw_ph_positive_tries", &ph_dbg_draw_mask, phDBgDrawPositiveTries);
	CMD3(CCC_Mask, "dbg_draw_ph_negative_tries", &ph_dbg_draw_mask, phDBgDrawNegativeTries);
	CMD3(CCC_Mask, "dbg_draw_ph_tri_test_aabb", &ph_dbg_draw_mask, phDbgDrawTriTestAABB);
	CMD3(CCC_Mask, "dbg_draw_ph_tries_changes_sign", &ph_dbg_draw_mask, phDBgDrawTriesChangesSign);
	CMD3(CCC_Mask, "dbg_draw_ph_tri_point", &ph_dbg_draw_mask, phDbgDrawTriPoint);
	CMD3(CCC_Mask, "dbg_draw_ph_explosion_position", &ph_dbg_draw_mask, phDbgDrawExplosionPos);
	CMD3(CCC_Mask, "dbg_draw_ph_statistics", &ph_dbg_draw_mask, phDbgDrawObjectStatistics);
	CMD3(CCC_Mask, "dbg_draw_ph_mass_centres", &ph_dbg_draw_mask, phDbgDrawMassCenters);
	CMD3(CCC_Mask, "dbg_draw_ph_death_boxes", &ph_dbg_draw_mask, phDbgDrawDeathActivationBox);
	CMD3(CCC_Mask, "dbg_draw_ph_hit_app_pos", &ph_dbg_draw_mask, phHitApplicationPoints);
	CMD3(CCC_Mask, "dbg_draw_ph_cashed_tries_stats", &ph_dbg_draw_mask, phDbgDrawCashedTriesStat);
	CMD3(CCC_Mask, "dbg_draw_ph_car_dynamics", &ph_dbg_draw_mask, phDbgDrawCarDynamics);
	CMD3(CCC_Mask, "dbg_draw_ph_car_plots", &ph_dbg_draw_mask, phDbgDrawCarPlots);
	CMD3(CCC_Mask, "dbg_ph_ladder", &ph_dbg_draw_mask, phDbgLadder);
	CMD3(CCC_Mask, "dbg_draw_ph_explosions", &ph_dbg_draw_mask, phDbgDrawExplosions);
	CMD3(CCC_Mask, "dbg_draw_car_plots_all_trans", &ph_dbg_draw_mask, phDbgDrawCarAllTrnsm);
	CMD3(CCC_Mask, "dbg_draw_ph_zbuffer_disable", &ph_dbg_draw_mask, phDbgDrawZDisable);
	CMD3(CCC_Mask, "dbg_ph_obj_collision_damage", &ph_dbg_draw_mask, phDbgDispObjCollisionDammage);
	CMD_RADIOGROUPMASK2("dbg_ph_ai_always_phmove", &ph_dbg_draw_mask, phDbgAlwaysUseAiPhMove, "dbg_ph_ai_never_phmove", &ph_dbg_draw_mask, phDbgNeverUseAiPhMove);
	CMD3(CCC_Mask, "dbg_ph_ik", &ph_dbg_draw_mask, phDbgIK);
	CMD3(CCC_Mask, "dbg_ph_ik_off", &ph_dbg_draw_mask1, phDbgIKOff);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_goal", &ph_dbg_draw_mask, phDbgDrawIKGoal);
	CMD3(CCC_Mask, "dbg_ph_ik_limits", &ph_dbg_draw_mask, phDbgIKLimits);
	CMD3(CCC_Mask, "dbg_ph_character_control", &ph_dbg_draw_mask, phDbgCharacterControl);
	CMD3(CCC_Mask, "dbg_draw_ph_ray_motions", &ph_dbg_draw_mask, phDbgDrawRayMotions);
	CMD4(CCC_Float, "dbg_ph_vel_collid_damage_to_display", &dbg_vel_collid_damage_to_display, 0.f, 1000.f);
	CMD4(CCC_DbgBullets, "dbg_draw_bullet_hit", &g_bDrawBulletHit, 0, 1);
	CMD4(CCC_Integer, "dbg_draw_fb_crosshair", &g_bDrawFirstBulletCrosshair, 0, 1);
	CMD1(CCC_DbgPhTrackObj, "dbg_track_obj");
	CMD3(CCC_Mask, "dbg_ph_actor_restriction", &ph_dbg_draw_mask1, ph_m1_DbgActorRestriction);
	CMD3(CCC_Mask, "dbg_draw_ph_hit_anims", &ph_dbg_draw_mask1, phDbgHitAnims);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_limits", &ph_dbg_draw_mask1, phDbgDrawIKLimits);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_predict", &ph_dbg_draw_mask1, phDbgDrawIKPredict);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_collision", &ph_dbg_draw_mask1, phDbgDrawIKCollision);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_shift_object", &ph_dbg_draw_mask1, phDbgDrawIKSHiftObject);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_blending", &ph_dbg_draw_mask1, phDbgDrawIKBlending);
	CMD1(CCC_DBGDrawCashedClear, "dbg_ph_cashed_clear");
	extern BOOL dbg_draw_character_bones;
	extern BOOL dbg_draw_character_physics;
	extern BOOL dbg_draw_character_binds;
	extern BOOL dbg_draw_character_physics_pones;
	extern BOOL ik_cam_shift;
	CMD4(CCC_Integer, "dbg_draw_character_bones", &dbg_draw_character_bones, FALSE, TRUE);
	CMD4(CCC_Integer, "dbg_draw_character_physics", &dbg_draw_character_physics, FALSE, TRUE);
	CMD4(CCC_Integer, "dbg_draw_character_binds", &dbg_draw_character_binds, FALSE, TRUE);
	CMD4(CCC_Integer, "dbg_draw_character_physics_pones", &dbg_draw_character_physics_pones, FALSE, TRUE);

	CMD4(CCC_Integer, "ik_cam_shift", &ik_cam_shift, FALSE, TRUE);

	extern	float ik_cam_shift_tolerance;
	CMD4(CCC_Float, "ik_cam_shift_tolerance", &ik_cam_shift_tolerance, 0.f, 2.f);
	float ik_cam_shift_speed;
	CMD4(CCC_Float, "ik_cam_shift_speed", &ik_cam_shift_speed, 0.f, 1.f);
	extern	BOOL dbg_draw_doors;
	CMD4(CCC_Integer, "dbg_draw_doors", &dbg_draw_doors, FALSE, TRUE);

	extern 	BOOL dbg_draw_ragdoll_spawn;
	CMD4(CCC_Integer, "dbg_draw_ragdoll_spawn", &dbg_draw_ragdoll_spawn, FALSE, TRUE);
	extern BOOL debug_step_info;
	extern BOOL debug_step_info_load;
	CMD4(CCC_Integer, "debug_step_info", &debug_step_info, FALSE, TRUE);
	CMD4(CCC_Integer, "debug_step_info_load", &debug_step_info_load, FALSE, TRUE);
	extern BOOL debug_character_material_load;
	CMD4(CCC_Integer, "debug_character_material_load", &debug_character_material_load, FALSE, TRUE);
	extern XRPHYSICS_API BOOL dbg_draw_camera_collision;
	CMD4(CCC_Integer, "dbg_draw_camera_collision", &dbg_draw_camera_collision, FALSE, TRUE);
	extern XRPHYSICS_API float	camera_collision_character_skin_depth;
	extern XRPHYSICS_API float	camera_collision_character_shift_z;
	CMD4(CCC_FloatBlock, "camera_collision_character_shift_z", &camera_collision_character_shift_z, 0.f, 1.f);
	CMD4(CCC_FloatBlock, "camera_collision_character_skin_depth", &camera_collision_character_skin_depth, 0.f, 1.f);
	extern XRPHYSICS_API BOOL	dbg_draw_animation_movement_controller;
	CMD4(CCC_Integer, "dbg_draw_animation_movement_controller", &dbg_draw_animation_movement_controller, FALSE, TRUE);

	extern Flags32	dbg_track_obj_flags;
	CMD3(CCC_Mask, "dbg_track_obj_blends_bp_0", &dbg_track_obj_flags, dbg_track_obj_blends_bp_0);
	CMD3(CCC_Mask, "dbg_track_obj_blends_bp_1", &dbg_track_obj_flags, dbg_track_obj_blends_bp_1);
	CMD3(CCC_Mask, "dbg_track_obj_blends_bp_2", &dbg_track_obj_flags, dbg_track_obj_blends_bp_2);
	CMD3(CCC_Mask, "dbg_track_obj_blends_bp_3", &dbg_track_obj_flags, dbg_track_obj_blends_bp_3);
	CMD3(CCC_Mask, "dbg_track_obj_blends_motion_name", &dbg_track_obj_flags, dbg_track_obj_blends_motion_name);
	CMD3(CCC_Mask, "dbg_track_obj_blends_time", &dbg_track_obj_flags, dbg_track_obj_blends_time);

	CMD3(CCC_Mask, "dbg_track_obj_blends_ammount", &dbg_track_obj_flags, dbg_track_obj_blends_ammount);
	CMD3(CCC_Mask, "dbg_track_obj_blends_mix_params", &dbg_track_obj_flags, dbg_track_obj_blends_mix_params);
	CMD3(CCC_Mask, "dbg_track_obj_blends_flags", &dbg_track_obj_flags, dbg_track_obj_blends_flags);
	CMD3(CCC_Mask, "dbg_track_obj_blends_state", &dbg_track_obj_flags, dbg_track_obj_blends_state);
	CMD3(CCC_Mask, "dbg_track_obj_blends_dump", &dbg_track_obj_flags, dbg_track_obj_blends_dump);

	CMD1(CCC_DbgVar, "dbg_var");

	extern float	dbg_text_height_scale;
	CMD4(CCC_FloatBlock, "dbg_text_height_scale", &dbg_text_height_scale, 0.2f, 5.f);
#endif


	CMD3(CCC_Mask, "cl_dynamiccrosshair", &psHUD_Flags, HUD_CROSSHAIR_DYNAMIC);
	CMD1(CCC_MainMenu, "main_menu");

#ifndef MASTER_GOLD
	CMD1(CCC_StartTimeSingle, "start_time_single");
	CMD4(CCC_TimeFactorSingle, "time_factor_single", &g_fTimeFactor, 0.f, 1000.0f);
	CMD4(CCC_Vector3, "psp_cam_offset", &CCameraLook2::m_cam_offset, Fvector().set(-1000, -1000, -1000), Fvector().set(1000, 1000, 1000));
#endif // MASTER_GOLD

#ifdef DEBUG
	CMD1(CCC_Crash, "crash");
	CMD1(CCC_DumpObjects, "dump_all_objects");
	CMD3(CCC_String, "stalker_death_anim", dbg_stalker_death_anim, 32);
	CMD4(CCC_Integer, "death_anim_debug", &death_anim_debug, FALSE, TRUE);
	CMD4(CCC_Integer, "death_anim_velocity", &b_death_anim_velocity, FALSE, TRUE);
	CMD4(CCC_Integer, "dbg_imotion_draw_velocity", &dbg_imotion_draw_velocity, FALSE, TRUE);
	CMD4(CCC_Integer, "dbg_imotion_collide_debug", &dbg_imotion_collide_debug, FALSE, TRUE);

	CMD4(CCC_Integer, "dbg_imotion_draw_skeleton", &dbg_imotion_draw_skeleton, FALSE, TRUE);
	CMD4(CCC_Float, "dbg_imotion_draw_velocity_scale", &dbg_imotion_draw_velocity_scale, 0.0001f, 100.0f);

	CMD4(CCC_Integer, "dbg_dump_physics_step", &ph_console::g_bDebugDumpPhysicsStep, 0, 1);
	CMD1(CCC_InvUpgradesHierarchy, "inv_upgrades_hierarchy");
	CMD1(CCC_InvUpgradesCurItem, "inv_upgrades_cur_item");
	CMD4(CCC_Integer, "inv_upgrades_log", &g_upgrades_log, 0, 1);
	CMD1(CCC_InvDropAllItems, "inv_drop_all_items");

	CMD1(CCC_DumpInfos, "dump_infos");
	CMD1(CCC_DumpTasks, "dump_tasks");
	CMD1(CCC_DumpMap, "dump_map");
	CMD1(CCC_DumpCreatures, "dump_creatures");

#endif

	CMD3(CCC_Mask, "cl_dynamiccrosshair", &psHUD_Flags, HUD_CROSSHAIR_DYNAMIC);
	CMD1(CCC_MainMenu, "main_menu");

#ifndef MASTER_GOLD
	CMD1(CCC_StartTimeSingle, "start_time_single");
	CMD4(CCC_TimeFactorSingle, "time_factor_single", &g_fTimeFactor, 0.f, 1000.0f);
	CMD4(CCC_Vector3, "psp_cam_offset", &CCameraLook2::m_cam_offset, Fvector().set(-1000, -1000, -1000), Fvector().set(1000, 1000, 1000));
#endif

	CMD4(CCC_Float, "con_sensitive",     &g_console_sensitive, 0.01f, 1.0f);
	CMD4(CCC_Integer, "wpn_aim_toggle",  &b_toggle_weapon_aim, 0, 1);
//	CMD4(CCC_Integer,	"hud_old_style", &g_old_style_ui_hud, 0, 1);

#ifdef DEBUG
	CMD4(CCC_Float, "ai_smart_cover_animation_speed_factor", &g_smart_cover_animation_speed_factor, .1f, 10.f);
	CMD4(CCC_Float, "air_resistance_epsilon", &air_resistance_epsilon, .0f, 1.f);

	CMD4(CCC_Integer, "show_wnd_rect_all", &g_show_wnd_rect2, 0, 1);
	CMD4(CCC_Integer, "dbg_show_ani_info", &g_ShowAnimationInfo, 0, 1);

	extern BOOL dbg_moving_bones_snd_player;
	CMD4(CCC_Integer, "dbg_bones_snd_player", &dbg_moving_bones_snd_player, FALSE, TRUE);
#endif // #ifdef DEBUG

	CMD4(CCC_Integer, 	"g_sleep_time", 		&psActorSleepTime, 1, 24);
	CMD4(CCC_Integer, 	"ai_use_old_vision", 	&g_ai_use_old_vision, 0, 1);
	CMD4(CCC_Float, 	"ai_aim_predict_time", 	&g_aim_predict_time, 0.f, 10.f);

	extern float g_bullet_time_factor;
	CMD4(CCC_Float, "g_bullet_time_factor", &g_bullet_time_factor, 0.f, 10.f);


#ifdef DEBUG
	extern BOOL g_ai_dbg_sight;
	extern BOOL g_ai_aim_use_smooth_aim;

	CMD4(CCC_Integer, "ai_dbg_sight", &g_ai_dbg_sight, 0, 1);
	CMD4(CCC_Integer, "ai_aim_use_smooth_aim", &g_ai_aim_use_smooth_aim, 0, 1);
#endif // #ifdef DEBUG

	extern float g_ai_aim_min_speed;
	extern float g_ai_aim_min_angle;
	extern float g_ai_aim_max_angle;

	CMD4(CCC_Float, "ai_aim_min_speed", &g_ai_aim_min_speed, 0.f, 10.f*PI);
	CMD4(CCC_Float, "ai_aim_min_angle", &g_ai_aim_min_angle, 0.f, 10.f*PI);
	CMD4(CCC_Float, "ai_aim_max_angle", &g_ai_aim_max_angle, 0.f, 10.f*PI);

#ifdef DEBUG
	extern BOOL g_debug_doors;
	CMD4(CCC_Integer, "ai_debug_doors", &g_debug_doors, 0, 1);
#endif // #ifdef DEBUG

	*g_last_saved_game = 0;

	CMD3(CCC_String, "slot_0", g_quick_use_slots[0], 32);
	CMD3(CCC_String, "slot_1", g_quick_use_slots[1], 32);
	CMD3(CCC_String, "slot_2", g_quick_use_slots[2], 32);
	CMD3(CCC_String, "slot_3", g_quick_use_slots[3], 32);

	CMD4(CCC_Integer, "keypress_on_start", &g_keypress_on_start, 0, 1);

	// Oxy:
	CMD1(CCC_SetWeather, "set_weather");
	CMD3(CCC_MaskNoSave, "game_extra_ruck", &g_extraFeatures, GAME_EXTRA_RUCK);
    CMD3(CCC_MaskNoSave, "game_extra_monster_inventory", &g_extraFeatures, GAME_EXTRA_MONSTER_INVENTORY);
    CMD3(CCC_MaskNoSave, "game_extra_spawn_antifreeze", &g_extraFeatures, GAME_EXTRA_SPAWN_ANTIFREEZE);
    CMD3(CCC_MaskNoSave, "game_extra_weapon_autoreload", &g_extraFeatures, GAME_EXTRA_WEAPON_AUTORELOAD);
    CMD3(CCC_MaskNoSave, "game_extra_dynamic_sun_movement", &g_extraFeatures, GAME_EXTRA_DYNAMIC_SUN);
    CMD3(CCC_MaskNoSave, "game_extra_hold_to_pickup", &g_extraFeatures, GAME_EXTRA_HOLD_TO_PICKUP);
    CMD3(CCC_MaskNoSave, "game_extra_polter_show_particles_on_dead", &g_extraFeatures, GAME_EXTRA_POLTER_SHOW_PARTICLES_ON_DEAD);
    CMD3(CCC_MaskNoSave, "game_extra_soc_talk_wnd", &g_extraFeatures, GAME_EXTRA_SOC_WND);
    CMD3(CCC_MaskNoSave, "game_extra_vertical_belts", &g_extraFeatures, GAME_EXTRA_VERTICAL_BELTS);
	CMD4(CCC_U32, "hud_adjust_mode", &hud_adj_mode, 0, 5); /// adjust mode support
}


void LoadGameExtraFeatures()
{
    g_extraFeatures.zero();
    string_path configFilePath;
    FS.update_path(configFilePath, "$game_config$", "GameExtra.ltx");

    string_path cmdLoadCfg;
    strconcat(sizeof(cmdLoadCfg), cmdLoadCfg, "cfg_load", " ", configFilePath);
    Console->Execute(cmdLoadCfg);
}