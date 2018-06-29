#include "stdafx.h"
#include "LevelGameDef.h"
#include "script_process.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "script_engine.h"
#include "script_engine_space.h"
#include "level.h"
#include "xrserver.h"
#include "alife_object_registry.h"
#include "alife_graph_registry.h"
#include "alife_time_manager.h"
#include "ai_space.h"
#include "../xrEngine/XR_IOConsole.h"
#include "../xrEngine/xr_ioc_cmd.h"
#include "string_table.h"
#include "gamepersistent.h"
#include "debug_renderer.h"
#include "object_broker.h"
#include "alife_simulator_base.h"
#include "../xrEngine/x_ray.h"
#include "ui/UILoadingScreen.h"

//-----------------------------------------------------------------
BOOL	net_sv_control_hit	= FALSE		;
BOOL	g_bCollectStatisticData = TRUE;
//-----------------------------------------------------------------

void* game_sv_GameState::get_client (u16 id) //if exist
{
	CSE_Abstract* entity = get_entity_from_eid(id);
	if (entity && entity->owner)
		return entity->owner;
	return nullptr;
}

CSE_Abstract*		game_sv_GameState::get_entity_from_eid		(u16 id)
{
	return				m_server->ID_to_entity(id);
}

void game_sv_GameState::signal_Syncronize()
{
	sv_force_sync = TRUE;
}

// Network
void game_sv_GameState::net_Export_State(NET_Packet& P, ClientID to)
{
	// Generic
	P.w_u32			(m_type);
	P.w_u16			(m_phase);
	P.w_u32			(m_start_time);
	P.w_u8			(u8(net_sv_control_hit));

	// Players
	net_Export_GameTime(P);
}

void game_sv_GameState::net_Export_Update(NET_Packet& P, ClientID id_to, ClientID id)
{
	net_Export_GameTime			(P);
};

void game_sv_GameState::net_Export_GameTime(NET_Packet& P)
{
	//Syncronize GameTime 
	P.w_u64(GetGameTime());
	P.w_float(GetGameTimeFactor());

	//Syncronize EnvironmentGameTime 
	P.w_u64(GetEnvironmentGameTime());
	P.w_float(GetEnvironmentGameTimeFactor());
};


void game_sv_GameState::OnPlayerConnect			(ClientID /**id_who/**/)
{
	signal_Syncronize	();
}

void game_sv_GameState::Create(shared_str &options)
{
	// loading scripts
    ai().script_engine().remove_script_process(ScriptEngine::eScriptProcessorGame);
    string_path S;
    FS.update_path(S, "$game_config$", "script.ltx");
    CInifile *l_tpIniFile = xr_new<CInifile>(S);
    R_ASSERT(l_tpIniFile);

	if (l_tpIniFile->section_exist(type_name()))
	{
		if (l_tpIniFile->r_string(type_name(), "script"))
			ai().script_engine().add_script_process(ScriptEngine::eScriptProcessorGame, xr_new<CScriptProcess>("game", l_tpIniFile->r_string(type_name(), "script")));
		else
			ai().script_engine().add_script_process(ScriptEngine::eScriptProcessorGame, xr_new<CScriptProcess>("game", ""));
	}
    xr_delete(l_tpIniFile);

	if (strstr(*options, "/alife"))
		m_alife_simulator = xr_new<CALifeSimulator>(&server(), &options);

	switch_Phase(GAME_PHASE_INPROGRESS);

}

void game_sv_GameState::Update		()
{
    if (Level().game) {
        CScriptProcess				*script_process = ai().script_engine().script_process(ScriptEngine::eScriptProcessorGame);
        if (script_process)
            script_process->update();
    }
}

game_sv_GameState::game_sv_GameState()
{
	VERIFY(g_pGameLevel);
	m_server = Level().Server;
	m_alife_simulator = nullptr;
	m_type = eGameIDSingle;
}

game_sv_GameState::~game_sv_GameState()
{
	ai().script_engine().remove_script_process(ScriptEngine::eScriptProcessorGame);
	delete_data(m_alife_simulator);
}

bool game_sv_GameState::change_level (NET_Packet &net_packet, ClientID sender)
{
	if (ai().get_alife())
		return (alife().change_level(net_packet));
	else
		return (true);
}

void game_sv_GameState::save_game (NET_Packet &net_packet, ClientID sender)
{
	if (ai().get_alife())
		alife().save(net_packet);
}

bool game_sv_GameState::load_game (NET_Packet &net_packet, ClientID sender)
{
	if (!ai().get_alife())
		return true;

	shared_str game_name;
	net_packet.r_stringZ(game_name);
	return (alife().load_game(*game_name, true));
}

void game_sv_GameState::switch_distance(NET_Packet &net_packet)
{
	if (ai().get_alife())
		alife().set_switch_distance(net_packet.r_float());
}

void game_sv_GameState::OnEvent (NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender )
{
	if (type == GAME_EVENT_ON_HIT)
	{
		u16		id_dest = tNetPacket.r_u16();
		u16     id_src = tNetPacket.r_u16();
		CSE_Abstract*	e_src = get_entity_from_eid(id_src);

		if (e_src)
			m_server->SendBroadcast(BroadcastCID, tNetPacket);
	}
}

void game_sv_GameState::teleport_object	(NET_Packet &packet, u16 id)
{
	if (!ai().get_alife())
		return;

	GameGraph::_GRAPH_ID		game_vertex_id;
	u32						level_vertex_id;
	Fvector					position;

	packet.r(&game_vertex_id, sizeof(game_vertex_id));
	packet.r(&level_vertex_id, sizeof(level_vertex_id));
	packet.r_vec3(position);

	alife().teleport_object(id, game_vertex_id, level_vertex_id, position);
}

void game_sv_GameState::add_restriction(RestrictionSpace::ERestrictorTypes type, u16 restriction_id, u16 id)
{
	if (ai().get_alife())
	{
		alife().add_restriction(id, restriction_id, type);
	}
}

void game_sv_GameState::remove_restriction(RestrictionSpace::ERestrictorTypes type, u16 restriction_id, u16 id)
{
	if (ai().get_alife())
	{
		alife().remove_restriction(id, restriction_id, type);
	}
}

void game_sv_GameState::remove_all_restrictions(RestrictionSpace::ERestrictorTypes type, u16 id)
{
	if (ai().get_alife())
	{
		alife().remove_all_restrictions(id, type);
	}
}

shared_str game_sv_GameState::level_name(const shared_str &server_options) const
{
	if (!ai().get_alife())
	{
		string64 l_name = "";
		VERIFY(_GetItemCount(*server_options, '/'));
		return (_GetItem(*server_options, 0, l_name, '/'));
	}

	return (alife().level_name());
}

LPCSTR default_map_version	= "1.0";
LPCSTR map_ver_string		= "ver=";

shared_str game_sv_GameState::parse_level_version			(const shared_str &server_options)
{
	const char* map_ver = strstr(server_options.c_str(), map_ver_string);
	string128	result_version;
	if (map_ver)
	{
		map_ver += sizeof(map_ver_string);
		if (strchr(map_ver, '/'))
			strncpy_s(result_version, map_ver, strchr(map_ver, '/') - map_ver);
		else
			xr_strcpy(result_version, map_ver);
	} else
	{
		xr_strcpy(result_version, default_map_version);
	}
	return shared_str(result_version);
}

void game_sv_GameState::on_death	(CSE_Abstract *e_dest, CSE_Abstract *e_src)
{
	CSE_ALifeCreatureAbstract	*creature = smart_cast<CSE_ALifeCreatureAbstract*>(e_dest);
	if (!creature)
		return;

	VERIFY						(creature->get_killer_id() == ALife::_OBJECT_ID(-1));
	creature->set_killer_id		( e_src->ID );

	if (!ai().get_alife())
		return;

	alife().on_death(e_dest, e_src);
}

void game_sv_GameState::OnCreate(u16 id_who)
{
	if (!ai().get_alife())
		return;

	CSE_Abstract			*e_who = get_entity_from_eid(id_who);
	VERIFY(e_who);
	if (!e_who->m_bALifeControl)
		return;

	CSE_ALifeObject			*alife_object = smart_cast<CSE_ALifeObject*>(e_who);
	if (!alife_object)
		return;

	alife_object->m_bOnline = true;

	if (alife_object->ID_Parent != 0xffff) {
		CSE_ALifeDynamicObject			*parent = ai().alife().objects().object(alife_object->ID_Parent, true);
		if (parent) {
			CSE_ALifeTraderAbstract		*trader = smart_cast<CSE_ALifeTraderAbstract*>(parent);
			if (trader)
				alife().create(alife_object);
			else {
				CSE_ALifeInventoryBox* const	box = smart_cast<CSE_ALifeInventoryBox*>(parent);
				if (box)
					alife().create(alife_object);
				else
					alife_object->m_bALifeControl = false;
			}
		}
		else
			alife_object->m_bALifeControl = false;
	}
	else
		alife().create(alife_object);
}

void game_sv_GameState::restart_simulator(LPCSTR saved_game_name)
{
	shared_str options = GamePersistent().GetServerOption();

	delete_data(m_alife_simulator);
	server().clear_ids();

	xr_strcpy(g_pGamePersistent->m_game_params.m_game_or_spawn, saved_game_name);
	xr_strcpy(g_pGamePersistent->m_game_params.m_new_or_load, "load");

	pApp->SetLoadingScreen(new UILoadingScreen());
	pApp->LoadBegin();
	m_alife_simulator = xr_new<CALifeSimulator>(&server(), &options);
	g_pGamePersistent->SetLoadStageTitle("st_client_synchronising");
	pApp->LoadForceFinish();
	g_pGamePersistent->LoadTitle();
	Device.PreCache(60, true, true);
	pApp->LoadEnd();
}

void game_sv_GameState::OnTouch(u16 eid_who, u16 eid_what)
{
	CSE_Abstract*		e_who = get_entity_from_eid(eid_who);		VERIFY(e_who);
	CSE_Abstract*		e_what = get_entity_from_eid(eid_what);	VERIFY(e_what);

	if (ai().get_alife())
	{
		CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = smart_cast<CSE_ALifeInventoryItem*>	(e_what);
		CSE_ALifeDynamicObject	*l_tpDynamicObject = smart_cast<CSE_ALifeDynamicObject*>	(e_who);

		if (l_tpALifeInventoryItem && l_tpDynamicObject &&
			ai().alife().graph().level().object(l_tpALifeInventoryItem->base()->ID, true) &&
			ai().alife().objects().object(e_who->ID, true) &&
			ai().alife().objects().object(e_what->ID, true))
			alife().graph().attach(*e_who, l_tpALifeInventoryItem, l_tpDynamicObject->m_tGraphID, false, false);
	}
}

void game_sv_GameState::OnDetach(u16 eid_who, u16 eid_what)
{
	if (ai().get_alife())
	{
		CSE_Abstract*		e_who = get_entity_from_eid(eid_who);		VERIFY(e_who);
		CSE_Abstract*		e_what = get_entity_from_eid(eid_what);	VERIFY(e_what);

		CSE_ALifeInventoryItem *l_tpALifeInventoryItem = smart_cast<CSE_ALifeInventoryItem*>(e_what);
		if (!l_tpALifeInventoryItem)
			return;

		CSE_ALifeDynamicObject *l_tpDynamicObject = smart_cast<CSE_ALifeDynamicObject*>(e_who);
		if (!l_tpDynamicObject)
			return;

		if (
			ai().alife().objects().object(e_who->ID, true) &&
			!ai().alife().graph().level().object(l_tpALifeInventoryItem->base()->ID, true) &&
			ai().alife().objects().object(e_what->ID, true)
			)
			alife().graph().detach(*e_who, l_tpALifeInventoryItem, l_tpDynamicObject->m_tGraphID, false, false);
		else {
			if (!ai().alife().objects().object(e_what->ID, true)) {
				u16				id = l_tpALifeInventoryItem->base()->ID_Parent;
				l_tpALifeInventoryItem->base()->ID_Parent = 0xffff;

				CSE_ALifeDynamicObject *dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(e_what);
				VERIFY(dynamic_object);
				dynamic_object->m_tNodeID = l_tpDynamicObject->m_tNodeID;
				dynamic_object->m_tGraphID = l_tpDynamicObject->m_tGraphID;
				dynamic_object->m_bALifeControl = true;
				dynamic_object->m_bOnline = true;
				alife().create(dynamic_object);
				l_tpALifeInventoryItem->base()->ID_Parent = id;
			}
		}
	}
}

u64 game_sv_GameState::GetStartGameTime()
{
	if (ai().get_alife() && ai().alife().initialized())
		return(ai().alife().time_manager().start_game_time());
	else
		return(inherited::GetStartGameTime());
}

u64 game_sv_GameState::GetGameTime()
{
	if (ai().get_alife() && ai().alife().initialized())
		return(ai().alife().time_manager().game_time());
	else
		return(inherited::GetGameTime());
}

float game_sv_GameState::GetGameTimeFactor()
{
	if (ai().get_alife() && ai().alife().initialized())
		return(ai().alife().time_manager().time_factor());
	else
		return(inherited::GetGameTimeFactor());
}

void game_sv_GameState::SetGameTimeFactor(const float fTimeFactor)
{
	if (ai().get_alife() && ai().alife().initialized())
		return(alife().time_manager().set_time_factor(fTimeFactor));
	else
		return(inherited::SetGameTimeFactor(fTimeFactor));
}

u64 game_sv_GameState::GetEnvironmentGameTime()
{
	if (ai().get_alife() && ai().alife().initialized())
		return(alife().time_manager().game_time());
	else
		return(inherited::GetGameTime());
}

float game_sv_GameState::GetEnvironmentGameTimeFactor()
{
	return(inherited::GetGameTimeFactor());
}

void game_sv_GameState::SetEnvironmentGameTimeFactor(const float fTimeFactor)
{
	return(inherited::SetGameTimeFactor(fTimeFactor));
}

void game_sv_GameState::sls_default()
{
	alife().update_switch();
}


//  [7/5/2005]
#ifdef DEBUG
extern	Flags32	dbg_net_Draw_Flags;

void		game_sv_GameState::OnRender()
{
};
#endif
