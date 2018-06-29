#pragma once

#include "game_base.h"
#include "alife_space.h"
#include "../xrScripts/export/script_export_space.h"
#include "../xrCore/client_id.h"
#include "alife_simulator.h"

class CSE_Abstract;
class xrServer;

class	game_sv_GameState	: public game_GameState
{
	typedef game_GameState inherited;

public:
	BOOL							sv_force_sync;
protected:
	xrServer*						m_server;
	CALifeSimulator					*m_alife_simulator;

	//Events
	virtual		void				OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender );
public:
	virtual		void				OnPlayerConnect			(ClientID id_who);

public:
									game_sv_GameState		();
	virtual							~game_sv_GameState		();
	// Main accessors
	virtual		void*				get_client				(u16 id);
				CSE_Abstract*		get_entity_from_eid		(u16 id);
	// Signals
	virtual		void				signal_Syncronize		();

#ifdef DEBUG
	virtual		void				OnRender				();
#endif

	// Events
	virtual		void				OnCreate				(u16 id_who);
	virtual		void				OnTouch					(u16 eid_who, u16 eid_target);			// TRUE=allow ownership, FALSE=denied
	virtual		void				OnDetach				(u16 eid_who, u16 eid_target);

	// Main
	virtual		void				Create					(shared_str& options);
	virtual		void				Update					();
	virtual		void				net_Export_State		(NET_Packet& P, ClientID id_to);				// full state
	virtual		void				net_Export_Update		(NET_Packet& P, ClientID id_to, ClientID id);		// just incremental update for specific client
	virtual		void				net_Export_GameTime		(NET_Packet& P);						// update GameTime only for remote clients

	virtual		bool				change_level			(NET_Packet &net_packet, ClientID sender);
	virtual		void				save_game				(NET_Packet &net_packet, ClientID sender);
	virtual		bool				load_game				(NET_Packet &net_packet, ClientID sender);
	virtual		void				switch_distance			(NET_Packet &net_packet);

	virtual		void				teleport_object			(NET_Packet &packet, u16 id);
	virtual		void				add_restriction			(RestrictionSpace::ERestrictorTypes type, u16 restriction_id, u16 id);
	virtual		void				remove_restriction		(RestrictionSpace::ERestrictorTypes type, u16 restriction_id, u16 id);
	virtual		void				remove_all_restrictions	(RestrictionSpace::ERestrictorTypes type, u16 id);

	virtual		void				sls_default				();
	virtual		shared_str			level_name				(const shared_str &server_options) const;
	
	static		shared_str			parse_level_version		(const shared_str &server_options);

	virtual		void				on_death				(CSE_Abstract *e_dest, CSE_Abstract *e_src);
	
	// Single State
	IC			xrServer			&server() const 		{ return (*m_server); }
	IC			CALifeSimulator		&alife() const			{ return (*m_alife_simulator); }
	void		restart_simulator							(LPCSTR saved_game_name);
	
	// Times
	virtual		ALife::_TIME_ID		GetStartGameTime();
	virtual		ALife::_TIME_ID		GetGameTime();
	virtual		float				GetGameTimeFactor();
	virtual		void				SetGameTimeFactor(const float fTimeFactor);

	virtual		ALife::_TIME_ID		GetEnvironmentGameTime();
	virtual		float				GetEnvironmentGameTimeFactor();
	virtual		void				SetEnvironmentGameTimeFactor(const float fTimeFactor);

};
