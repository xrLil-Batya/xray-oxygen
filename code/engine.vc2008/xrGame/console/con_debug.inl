#	include "PHDebug.h"
#	include "../../xrUICore/UIDebugFonts.h" 
#	include "game_graph.h"
#	include "CharacterPhysicsSupport.h"

		Flags32	dbg_net_Draw_Flags		= {0};

		BOOL	g_bDebugNode			= FALSE;
		u32		g_dwDebugNodeSource		= 0;
		u32		g_dwDebugNodeDest		= 0;
extern	BOOL	g_bDrawBulletHit;
extern	BOOL	g_bDrawFirstBulletCrosshair;

		float	debug_on_frame_gather_stats_frequency	= 0.f;

extern LPSTR	dbg_stalker_death_anim;
extern BOOL		b_death_anim_velocity;
extern XRPHYSICS_API BOOL death_anim_debug;
extern BOOL		dbg_imotion_draw_skeleton;
extern BOOL		dbg_imotion_draw_velocity;
extern BOOL		dbg_imotion_collide_debug;
extern float	dbg_imotion_draw_velocity_scale;


class CCC_ALifePath : public IConsole_Command
{
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

		string128				name;

		if (0==strext(arguments))
			xr_strconcat		(name, arguments, ".ogf");
		else
			xr_strconcat		(name, arguments);

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