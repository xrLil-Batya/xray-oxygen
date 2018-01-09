#include "stdafx.h"
#include "LevelGameDef.h"
#include "script_process.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "script_engine.h"
#include "script_engine_space.h"
#include "level.h"
#include "xrserver.h"
#include "ai_space.h"
#include "game_sv_event_queue.h"
#include "../xrEngine/XR_IOConsole.h"
#include "../xrEngine/xr_ioc_cmd.h"
#include "string_table.h"

#include "debug_renderer.h"
ENGINE_API	bool g_dedicated_server;

#define			MAPROT_LIST_NAME		"maprot_list.ltx"
string_path		MAPROT_LIST		= "";
BOOL	net_sv_control_hit	= FALSE		;
BOOL	g_bCollectStatisticData = TRUE;
//-----------------------------------------------------------------
u32		g_sv_base_dwRPointFreezeTime	= 0;
int		g_sv_base_iVotingEnabled		= 0x00ff;
//-----------------------------------------------------------------
LPCSTR				game_sv_GameState::get_name_id				(ClientID id)							
{
	return Core.UserName;
}

LPCSTR				game_sv_GameState::get_player_name_id				(ClientID id)								
{
	xrClientData* xrCData	=	m_server->ID_to_client(id);
	return xrCData == NULL ? "unknown" : get_name_id(id);
}

u32					game_sv_GameState::get_players_count		()
{
	return				m_server->GetClientsCount();
}

u16					game_sv_GameState::get_id_2_eid				(ClientID id)
{
	xrClientData*	C	= (xrClientData*)m_server->ID_to_client	(id);
	if (0==C)			return 0xffff;
	CSE_Abstract*	E	= C->owner;
	if (0==E)			return 0xffff;
	return E->ID;
}

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

// Utilities
u32 game_sv_GameState::get_alive_count(u32 team)
{
	return 1;
}

xr_vector<u16>*		game_sv_GameState::get_children				(ClientID id)
{
	xrClientData*	C	= (xrClientData*)m_server->ID_to_client	(id);
	if (0==C)			return 0;
	CSE_Abstract* E	= C->owner;
	if (0==E)			return 0;
	return	&(E->children);
}

s32					game_sv_GameState::get_option_i				(LPCSTR lst, LPCSTR name, s32 def)
{
	string64		op;
	strconcat		(sizeof(op),op,"/",name,"=");
	if (strstr(lst,op))	return atoi	(strstr(lst,op)+xr_strlen(op));
	else				return def;
}

float					game_sv_GameState::get_option_f				(LPCSTR lst, LPCSTR name, float def)
{
	string64		op;
	strconcat		(sizeof(op),op,"/",name,"=");
	LPCSTR			found =	strstr(lst,op);

	if (found)
	{	
		float		val;
		int cnt		= sscanf(found+xr_strlen(op),"%f",&val);
		VERIFY		(cnt==1);
		return		val;
//.		return atoi	(strstr(lst,op)+xr_strlen(op));
	}else
		return def;
}

string64&			game_sv_GameState::get_option_s				(LPCSTR lst, LPCSTR name, LPCSTR def)
{
	static string64	ret;

	string64		op;
	strconcat		(sizeof(op),op,"/",name,"=");
	LPCSTR			start	= strstr(lst,op);
	if (start)		
	{
		LPCSTR			begin	= start + xr_strlen(op); 
		sscanf			(begin, "%[^/]",ret);
	}
	else			
	{
		if (def)	xr_strcpy		(ret,def);
		else		ret[0]=0;
	}
	return ret;
}
void				game_sv_GameState::signal_Syncronize		()
{
	sv_force_sync	= TRUE;
}

// Network

struct player_exporter
{
	u16					counter;
	ClientID			to_client;
	NET_Packet*			p_to_send;

	void __stdcall count_players(IClient* client)
	{
		xrClientData* tmp_client = static_cast<xrClientData*>(client);
		if (!tmp_client->net_Ready)
		{
			return;
		}
		++counter;
	}
	void __stdcall export_players(IClient* client)
	{
	}
};

void game_sv_GameState::net_Export_State						(NET_Packet& P, ClientID to)
{
	// Generic
	P.w_clientID	(to);
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

void game_sv_GameState::net_Export_GameTime						(NET_Packet& P)
{
//#pragma todo("It should be done via single message, why always pass this data?")
//#if 0
	//Syncronize GameTime 
	P.w_u64(GetGameTime());
	P.w_float(GetGameTimeFactor());
	//Syncronize EnvironmentGameTime 
	P.w_u64(GetEnvironmentGameTime());
	P.w_float(GetEnvironmentGameTimeFactor());
//#endif
};


void game_sv_GameState::OnPlayerConnect			(ClientID /**id_who/**/)
{
	signal_Syncronize	();
}

void game_sv_GameState::OnPlayerDisconnect		(ClientID id_who, LPSTR, u16 )
{
	signal_Syncronize	();
}

static float							rpoints_Dist [TEAM_COUNT] = {1000.f, 1000.f, 1000.f, 1000.f};
void game_sv_GameState::Create					(shared_str &options)
{
	string_path	fn_game;
	m_item_respawner.clear_respawns();
	if (FS.exist(fn_game, "$level$", "level.game")) 
	{
		IReader *F = FS.r_open	(fn_game);
		IReader *O = 0;

		// Load RPoints
		if (0!=(O = F->open_chunk	(RPOINT_CHUNK)))
		{ 
			for (int id=0; O->find_chunk(id); ++id)
			{
				RPoint					R;
				u8						team;
				u8						type;
				u16						GameType;
				shared_str				rp_profile;

				O->r_fvector3			(R.P);
				O->r_fvector3			(R.A);
				team					= O->r_u8	();	
				type					= O->r_u8	();
				GameType				= O->r_u16	();
				if(type==rptItemSpawn)
					O->r_stringZ		(rp_profile);

				switch (type)
				{
				case rptActorSpawn:
					{
						rpoints[team].push_back	(R);
						for (int i=0; i<int(rpoints[team].size())-1; i++)
						{
							RPoint rp = rpoints[team][i];
							float dist = R.P.distance_to_xz(rp.P)/2;
							if (dist<rpoints_MinDist[team])
								rpoints_MinDist[team] = dist;
							dist = R.P.distance_to(rp.P)/2;
							if (dist<rpoints_Dist[team])
								rpoints_Dist[team] = dist;
						};
					}break;
				case rptItemSpawn:
					{
						m_item_respawner.add_new_rpoint(rp_profile, R);
					}
				};
			};
			O->close();
		}

		FS.r_close	(F);
	}

	if (!g_dedicated_server)
	{
		// loading scripts
		ai().script_engine().remove_script_process(ScriptEngine::eScriptProcessorGame);
		string_path					S;
		FS.update_path				(S,"$game_config$","script.ltx");
		CInifile					*l_tpIniFile = xr_new<CInifile>(S);
		R_ASSERT					(l_tpIniFile);

		if( l_tpIniFile->section_exist( type_name() ) )
			if (l_tpIniFile->r_string(type_name(),"script"))
				ai().script_engine().add_script_process(ScriptEngine::eScriptProcessorGame,xr_new<CScriptProcess>("game",l_tpIniFile->r_string(type_name(),"script")));
			else
				ai().script_engine().add_script_process(ScriptEngine::eScriptProcessorGame,xr_new<CScriptProcess>("game",""));

		xr_delete					(l_tpIniFile);
	}

	//---------------------------------------------------------------------
	ConsoleCommands_Create();
	//---------------------------------------------------------------------
//	CCC_LoadCFG_custom*	pTmp = xr_new<CCC_LoadCFG_custom>("sv_");
//	pTmp->Execute				(Console->ConfigFile);
//	xr_delete					(pTmp);
	//---------------------------------------------------------------------
	LPCSTR		svcfg_ltx_name = "-svcfg ";
	if (strstr(Core.Params, svcfg_ltx_name))
	{
		string_path svcfg_name = "";
		int		sz = xr_strlen(svcfg_ltx_name);
		sscanf		(strstr(Core.Params,svcfg_ltx_name)+sz,"%[^ ] ",svcfg_name);
//		if (FS.exist(svcfg_name))
		{
			Console->ExecuteScript(svcfg_name);
		}
	};
	//---------------------------------------------------------------------
	ReadOptions(options);	
}

void	game_sv_GameState::ReadOptions				(shared_str &options)
{
	g_sv_base_dwRPointFreezeTime = get_option_i(*options, "rpfrz", g_sv_base_dwRPointFreezeTime/1000) * 1000;

//.	xr_strcpy(MAPROT_LIST, MAPROT_LIST_NAME);
//.	if (!FS.exist(MAPROT_LIST))
	FS.update_path(MAPROT_LIST, "$app_data_root$", MAPROT_LIST_NAME);
	if (FS.exist(MAPROT_LIST))
		Console->ExecuteScript(MAPROT_LIST);
	
	g_sv_base_iVotingEnabled = get_option_i(*options,"vote",(g_sv_base_iVotingEnabled));
	//---------------------------
	//Convert old vote param
	if (g_sv_base_iVotingEnabled != 0)
	{
		if (g_sv_base_iVotingEnabled == 1)
			g_sv_base_iVotingEnabled = 0x00ff;
	}
};
//-----------------------------------------------------------
static bool g_bConsoleCommandsCreated_SV_Base = false;
void	game_sv_GameState::ConsoleCommands_Create	()
{
};

void	game_sv_GameState::ConsoleCommands_Clear	()
{
};

bool				game_sv_GameState::IsPointFreezed			(RPoint* rp)
{
	return rp->TimeToUnfreeze > Level().timeServer();
};

void				game_sv_GameState::SetPointFreezed		(RPoint* rp)
{
	R_ASSERT(rp);
	rp->TimeToUnfreeze	= Level().timeServer() + g_sv_base_dwRPointFreezeTime;
}

CSE_Abstract*		game_sv_GameState::spawn_begin				(LPCSTR N)
{
	CSE_Abstract*	A	=   F_entity_Create(N);	R_ASSERT(A);	// create SE
	A->s_name			=   N;							// ltx-def
//.	A->s_gameid			=	u8(m_type);							// game-type
	A->s_RP				=	0xFE;								// use supplied
	A->ID				=	0xffff;								// server must generate ID
	A->ID_Parent		=	0xffff;								// no-parent
	A->ID_Phantom		=	0xffff;								// no-phantom
	A->RespawnTime		=	0;									// no-respawn
	return A;
}

CSE_Abstract*		game_sv_GameState::spawn_end				(CSE_Abstract* E, ClientID id)
{
	NET_Packet						P;
	u16								skip_header;
	E->Spawn_Write					(P,TRUE);
	P.r_begin						(skip_header);
	CSE_Abstract* N = m_server->Process_spawn	(P,id);
	F_entity_Destroy				(E);

	return N;
}

void game_sv_GameState::GenerateGameMessage (NET_Packet &P)
{ 
	P.w_begin(M_GAMEMESSAGE); 
};

void game_sv_GameState::u_EventGen(NET_Packet& P, u16 type, u16 dest)
{
	P.w_begin	(M_EVENT);
	P.w_u32		(Level().timeServer());//Device.TimerAsync());
	P.w_u16		(type);
	P.w_u16		(dest);
}

void game_sv_GameState::u_EventSend(NET_Packet& P, u32 dwFlags)
{
	m_server->SendBroadcast(BroadcastCID,P,dwFlags);
}

void game_sv_GameState::Update		()
{
	struct ping_filler
	{
		void operator()(IClient* client)
		{
			xrClientData* C = static_cast<xrClientData*>(client);
		}
	};
	ping_filler tmp_functor;
	m_server->ForEachClientDo(tmp_functor);
	
	if (!g_dedicated_server)
	{
		if (Level().game) {
			CScriptProcess				*script_process = ai().script_engine().script_process(ScriptEngine::eScriptProcessorGame);
			if (script_process)
				script_process->update	();
		}
	}
}

void game_sv_GameState::OnDestroyObject(u16 eid_who)
{
}

game_sv_GameState::game_sv_GameState()
{
	VERIFY(g_pGameLevel);
	m_server					= Level().Server;
	m_event_queue = xr_new<GameEventQueue>();

	m_bMapRotation = false;
	m_bMapSwitched = false;
	m_bMapNeedRotation = false;
	m_bFastRestart = false;
	m_pMapRotation_List.clear();

	for (int i=0; i<TEAM_COUNT; i++) rpoints_MinDist[i] = 1000.0f;	
}

game_sv_GameState::~game_sv_GameState()
{
	if (!g_dedicated_server)
		ai().script_engine().remove_script_process(ScriptEngine::eScriptProcessorGame);
	xr_delete(m_event_queue);

	SaveMapList();

	m_pMapRotation_List.clear();
	//-------------------------------------------------------
	ConsoleCommands_Clear();
}

bool game_sv_GameState::change_level (NET_Packet &net_packet, ClientID sender)
{
	return						(true);
}

void game_sv_GameState::save_game (NET_Packet &net_packet, ClientID sender)
{
}

bool game_sv_GameState::load_game (NET_Packet &net_packet, ClientID sender)
{
	return						(true);
}

void game_sv_GameState::reload_game (NET_Packet &net_packet, ClientID sender)
{
}

void game_sv_GameState::switch_distance (NET_Packet &net_packet, ClientID sender)
{
}

void game_sv_GameState::OnHit (u16 id_hitter, u16 id_hitted, NET_Packet& P)
{
	CSE_Abstract*		e_hitter		= get_entity_from_eid	(id_hitter	);
	CSE_Abstract*		e_hitted		= get_entity_from_eid	(id_hitted	);
	if (!e_hitter || !e_hitted) return;

//	CSE_ALifeCreatureActor*		a_hitter		= smart_cast <CSE_ALifeCreatureActor*> (e_hitter);
	CSE_ALifeCreatureActor*		a_hitted		= smart_cast <CSE_ALifeCreatureActor*> (e_hitted);

	if (a_hitted/* && a_hitter*/)
	{
		OnPlayerHitPlayer(id_hitter, id_hitted, P);
		return;
	};
}

void game_sv_GameState::OnEvent (NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender )
{
	switch	(type)
	{	
	case GAME_EVENT_PLAYER_CONNECTED:
		{
			ClientID ID;
			tNetPacket.r_clientID(ID);
			OnPlayerConnect(ID);
		}break;

	case GAME_EVENT_PLAYER_DISCONNECTED:
		{
			ClientID ID;
			tNetPacket.r_clientID(ID);
			string1024 PlayerName;
			tNetPacket.r_stringZ(PlayerName);
			u16		GameID = tNetPacket.r_u16();
			OnPlayerDisconnect(ID, PlayerName, GameID);
		}break;

	case GAME_EVENT_PLAYER_KILLED:
		{
		}break	;
	case GAME_EVENT_ON_HIT:
		{
			u16		id_dest				= tNetPacket.r_u16();
			u16     id_src				= tNetPacket.r_u16();
			CSE_Abstract*	e_src		= get_entity_from_eid	(id_src	);

			if(!e_src)
				break;

			OnHit(id_src, id_dest, tNetPacket);
			m_server->SendBroadcast		(BroadcastCID,tNetPacket,net_flags(TRUE,TRUE));
		}break;
	case GAME_EVENT_CREATE_CLIENT:
		{
			IClient* CL					= (IClient*)m_server->ID_to_client(sender);
			VERIFY2(CL, "bad create client message GAME_EVENT_CREATE_CLIENT");
			if ( CL == NULL ) { break; }
			
			CL->flags.bConnected		= TRUE;
			m_server->AttachNewClient	(CL);
		}break;
	case GAME_EVENT_PLAYER_AUTH:
		{
			IClient* CL				=	m_server->ID_to_client		(sender);
			m_server->OnBuildVersionRespond(CL, tNetPacket);
		}break;
	case GAME_EVENT_CREATE_PLAYER_STATE:
		{
			break;
		}break;
	default:
		{
			string16 tmp;
			R_ASSERT3	(0,"Game Event not implemented!!!", itoa(type, tmp, 10));
		};
	};
}

void game_sv_GameState::OnSwitchPhase(u32 old_phase, u32 new_phase)
{
	inherited::OnSwitchPhase(old_phase, new_phase);
	signal_Syncronize	(); 
}

void game_sv_GameState::AddDelayedEvent(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender )
{
	m_event_queue->Create(tNetPacket, type, time, sender);
	return;
}

void game_sv_GameState::ProcessDelayedEvent		()
{
	GameEvent* ge = NULL;
	while ((ge = m_event_queue->Retreive()) != 0) {
		OnEvent(ge->P,ge->type,ge->time,ge->sender);
		m_event_queue->Release();
	}
}

class EventDeleterPredicate
{
private:
	u16 id_entity_victim;
public:
	EventDeleterPredicate()
	{
		id_entity_victim = u16(-1);
	}

	EventDeleterPredicate(u16 id_entity)
	{
		id_entity_victim = id_entity;
	}

	bool __stdcall PredicateDelVictim(GameEvent* const ge)
	{
		bool ret_val = false;
		switch (ge->type)
		{
			case GAME_EVENT_PLAYER_KILLED:
			case GAME_EVENT_PLAYER_HITTED:
				{
					u32 tmp_pos			= ge->P.r_tell();
					u16 id_entity_for	= ge->P.r_u16();
					if (id_entity_for == id_entity_victim)
						ret_val = true;
					ge->P.r_seek(tmp_pos);
				} break;
		};
		return ret_val;
	}
	bool __stdcall PredicateForAll(GameEvent* const ge)
	{
		Msg("- Erasing [%d] event before start.", ge->type);
		return true;
	}

};

void game_sv_GameState::CleanDelayedEventFor(u16 id_entity_victim)
{
	EventDeleterPredicate event_deleter(id_entity_victim);
	m_event_queue->EraseEvents(
		fastdelegate::MakeDelegate(&event_deleter, &EventDeleterPredicate::PredicateDelVictim)
	);
}

class EventDeleteForClientPredicate
{
public:
	EventDeleteForClientPredicate(ClientID const & clientId) :
		m_client_id(clientId)
	{
	}
	EventDeleteForClientPredicate(EventDeleteForClientPredicate const & copy) :
		m_client_id(copy.m_client_id)
	{
	}
		
	bool __stdcall Predicate(GameEvent* const ge)
	{
		if (ge && (ge->sender == m_client_id))
		{
			Msg("- Erasing event for not valid client [0x%08x]", m_client_id.value());
			return true;
		}
		return false;
	}
private:
	EventDeleteForClientPredicate& operator = (EventDeleteForClientPredicate const & copy) {}
	ClientID const m_client_id;
};//class EventDeleteForClientPredicate

void game_sv_GameState::CleanDelayedEventFor(ClientID const & clientId)
{
	EventDeleteForClientPredicate event_deleter(clientId);
	m_event_queue->EraseEvents(
		fastdelegate::MakeDelegate(&event_deleter, &EventDeleteForClientPredicate::Predicate)
	);
}

void game_sv_GameState::CleanDelayedEvents()
{
	EventDeleterPredicate event_deleter;
	m_event_queue->EraseEvents(
		fastdelegate::MakeDelegate(&event_deleter, &EventDeleterPredicate::PredicateForAll)
	);
}

u32 game_sv_GameState::getRPcount (u16 team_idx)
{
	if ( !(team_idx<TEAM_COUNT) )
		return 0;
	else
		return rpoints[team_idx].size();
}

RPoint game_sv_GameState::getRP (u16 team_idx, u32 rp_idx)
{
	if( (team_idx<TEAM_COUNT) && (rp_idx<rpoints[team_idx].size()) )
	return rpoints[team_idx][rp_idx];
	else 
		return RPoint();
};

void game_sv_GameState::teleport_object	(NET_Packet &packet, u16 id)
{
}

void game_sv_GameState::add_restriction	(NET_Packet &packet, u16 id)
{
}

void game_sv_GameState::remove_restriction(NET_Packet &packet, u16 id)
{
}

void game_sv_GameState::remove_all_restrictions	(NET_Packet &packet, u16 id)
{
}

void game_sv_GameState::MapRotation_AddMap(LPCSTR MapName, LPCSTR MapVer)
{
	SMapRot R;
	R.map_name = MapName;
	R.map_ver = MapVer;
	m_pMapRotation_List.push_back(R);

	if (m_pMapRotation_List.size() > 1)
		m_bMapRotation = true;
	else
		m_bMapRotation = false;
};

void game_sv_GameState::MapRotation_ListMaps	()
{
	if (m_pMapRotation_List.empty())
	{
		Msg ("- Currently there are no any maps in list.");
		return;
	}
	CStringTable st;
	Msg("- ----------- Maps ---------------");
	for (u32 i=0; i<m_pMapRotation_List.size(); i++)
	{
		SMapRot& R = m_pMapRotation_List[i];
		if (i==0)
			Msg("~   %d. %s (%s) (current)", i+1, st.translate(R.map_name).c_str(), R.map_name.c_str());
		else
			Msg("  %d. %s (%s)", i+1, st.translate(R.map_name).c_str(), R.map_name.c_str());
	}
	Msg("- --------------------------------");
};

void game_sv_GameState::SaveMapList				()
{
	if (0==MAPROT_LIST[0])				return;
	if (m_pMapRotation_List.empty())	return;
	IWriter*		fs		= FS.w_open(MAPROT_LIST);

	while(m_pMapRotation_List.size())
	{
		SMapRot& R			= m_pMapRotation_List.front();
		fs->w_printf		("sv_addmap %s/ver=%s\n", R.map_name.c_str(), R.map_ver.c_str());
		m_pMapRotation_List.pop_front();
	};
	FS.w_close				(fs);
};

shared_str game_sv_GameState::level_name		(const shared_str &server_options) const
{
	return parse_level_name(server_options);
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

shared_str game_sv_GameState::parse_level_name(const shared_str &server_options)
{
	string64			l_name = "";
	VERIFY				(_GetItemCount(*server_options,'/'));
	return				(_GetItem(*server_options,0,l_name,'/'));
}

void game_sv_GameState::on_death	(CSE_Abstract *e_dest, CSE_Abstract *e_src)
{
	CSE_ALifeCreatureAbstract	*creature = smart_cast<CSE_ALifeCreatureAbstract*>(e_dest);
	if (!creature)
		return;

	VERIFY						(creature->get_killer_id() == ALife::_OBJECT_ID(-1));
	creature->set_killer_id		( e_src->ID );
}

//  [7/5/2005]
#ifdef DEBUG
extern	Flags32	dbg_net_Draw_Flags;

void		game_sv_GameState::OnRender				()
{
};
#endif

class NameSearcherPredicate
{
public:
	NameSearcherPredicate(char const * name, IClient const * to_exclude) :
		m_name(name),
		m_exclude(to_exclude)
	{
	}

	inline bool operator()(IClient* client) const
	{
		if (client == m_exclude)
			return false;

		xrClientData* tmp_cl = static_cast<xrClientData*>(client);
		if (!tmp_cl)
			return false;
		
		return (xr_strcmp(Core.UserName, m_name) == 0);
	}
private:
	char const *	m_name;
	IClient const * m_exclude;
}; //class NameSearcherPredicate

bool game_sv_GameState::FindPlayerName			(char const * name, IClient const * to_exclude)
{
	R_ASSERT(name);
	NameSearcherPredicate	tmp_predicate(name, to_exclude);
	IClient* tmp_client		= m_server->FindClient(tmp_predicate);
	return tmp_client != NULL;
}