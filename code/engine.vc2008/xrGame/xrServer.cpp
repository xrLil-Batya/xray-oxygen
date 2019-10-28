// xrServer.cpp: implementation of the xrServer class.
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "xrServer.h"
#include "xrMessages.h"
#include "xrServer_Objects_ALife_All.h"
#include "level.h"

#include "ai_space.h"
#include "alife_object_registry.h"
#include "../xrEngine/IGame_Persistent.h"
#include "../xrEngine/string_table.h"
#include "object_broker.h"

#include "../xrEngine/XR_IOConsole.h"
#include "ui/UIInventoryUtilities.h"

#include "../FrayBuildConfig.hpp"
#include "script_process.h"
#include "script_engine.h"
#include "script_engine_space.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#include <functional>
#pragma warning(pop)

CClient::CClient()
{
    flags.bConnected = FALSE;
	Clear();
}

void CClient::Clear()
{
	owner = nullptr;
	net_Accepted = FALSE;
};

xrServer::xrServer() : SV_Client(nullptr)
{
}

xrServer::~xrServer()
{
	entities.clear();
    xr_delete(SV_Client);
}

//--------------------------------------------------------------------
CSE_Abstract* xrServer::ID_to_entity(u16 ID)
{
	if (0xffff != ID)
	{
		xrS_entities::iterator	I = entities.find(ID);
		if (entities.end() != I)
            return I->second;
	}
	return nullptr;
}

//--------------------------------------------------------------------
#ifdef DEBUG
INT g_sv_SendUpdate = 0;
#endif

void xrServer::Update	()
{
	if (game->sv_force_sync)
        Perform_game_export();

	VERIFY(verify_entities());
}

u32 xrServer::OnMessage(NET_Packet& P)			// Non-Zero means broadcasting with "flags" as returned
{
	u16 type;
	P.r_begin(type);

	switch (type)
	{
	case M_UPDATE:
	{
		Process_update(P);						// No broadcast
	}break;
	case M_SPAWN:
	{
		Process_spawn(P);
	}break;
	case M_EVENT:
	{
		Process_event(P);
	}break;
	case M_EVENT_PACK:
	{
		NET_Packet	tmpP;
		while (!P.r_eof())
		{
			tmpP.B.count = P.r_u8();
			P.r(tmpP.B.data, tmpP.B.count);

			OnMessage(tmpP);
		};
	}break;
	//-------------------------------------------------------------------
	case M_CHANGE_LEVEL:
	{
		if (game->change_level(P))
		{
			SendBroadcast(P);
		}
	}break;
	case M_LOAD_GAME:
	{
		game->load_game(P);
		SendBroadcast(P);
	}break;
	case M_SAVE_PACKET:
	{
		Process_save(P);
	}break;
	}
	VERIFY (verify_entities());

	return 0;
}

void xrServer::SendBroadcast(NET_Packet& P)
{
    if (!SV_Client || !SV_Client->flags.bConnected)
        return;

	if (SV_Client->net_Accepted)
		Level().OnMessage(P.B.data, (u32)P.B.count);
}
//--------------------------------------------------------------------
CSE_Abstract*	xrServer::entity_Create		(LPCSTR name)
{
	return F_entity_Create(name);
}

void xrServer::entity_Destroy(CSE_Abstract *&P)
{
	R_ASSERT					(P);
	entities.erase				(P->ID);
	m_tID_Generator.vfFreeID	(P->ID,Device.TimerAsync());

	if(P->owner && P->owner->owner==P)
		P->owner->owner = nullptr;

	P->owner = nullptr;
	if (!ai().get_alife() || !P->m_bALifeControl)
	{
		F_entity_Destroy		(P);
	}
}

//--------------------------------------------------------------------
CSE_Abstract* xrServer::GetEntity(u32 Num)
{
	xrS_entities::iterator I = entities.begin(), E = entities.end();
	for (u32 C = 0; I != E; ++I, ++C)
	{
		if (C == Num) return I->second;
	}
	return nullptr;
};

#ifdef DEBUG

static	bool _ve_initialized	= false;
static	bool _ve_use			= true;

bool xrServer::verify_entities() const
{
#ifdef SLOW_VERIFY_ENTITIES
	if (!_ve_initialized) 
	{
		_ve_initialized = false;
		if (strstr(Core.Params, "-~ve"))	_ve_use = false;
	}
	if (!_ve_use)						return true;

	xrS_entities::const_iterator		I = entities.begin();
	xrS_entities::const_iterator		E = entities.end();
	for ( ; I != E; ++I) {
		VERIFY2							((*I).first != 0xffff,"SERVER : Invalid entity id as a map key - 0xffff");
		VERIFY2							((*I).second,"SERVER : Null entity object in the map");
		VERIFY3							((*I).first == (*I).second->ID,"SERVER : ID mismatch - map key doesn't correspond to the real entity ID",(*I).second->name_replace());
		verify_entity					((*I).second);
	}
#endif
	return								(true);
}

void xrServer::verify_entity				(const CSE_Abstract *entity) const
{
	VERIFY(entity->m_wVersion!=0);
	if (entity->ID_Parent != 0xffff) {
		xrS_entities::const_iterator	J = entities.find(entity->ID_Parent);
		VERIFY_FORMAT (J != entities.end(),
			"SERVER : Cannot find parent in the map [%s][%s]",entity->name_replace(),
			entity->name());
		VERIFY3							((*J).second,"SERVER : Null entity object in the map",entity->name_replace());
		VERIFY3							((*J).first == (*J).second->ID,"SERVER : ID mismatch - map key doesn't correspond to the real entity ID",(*J).second->name_replace());
		VERIFY3							(std::find((*J).second->children.begin(),(*J).second->children.end(),entity->ID) != (*J).second->children.end(),"SERVER : Parent/Children relationship mismatch - Object has parent, but corresponding parent doesn't have children",(*J).second->name_replace());
	}

	xr_vector<u16>::const_iterator		I = entity->children.begin();
	xr_vector<u16>::const_iterator		E = entity->children.end();
	for ( ; I != E; ++I) {
		VERIFY3							(*I != 0xffff,"SERVER : Invalid entity children id - 0xffff",entity->name_replace());
		xrS_entities::const_iterator	J = entities.find(*I);
		VERIFY3							(J != entities.end(),"SERVER : Cannot find children in the map",entity->name_replace());
		VERIFY3							((*J).second,"SERVER : Null entity object in the map",entity->name_replace());
		VERIFY3							((*J).first == (*J).second->ID,"SERVER : ID mismatch - map key doesn't correspond to the real entity ID",(*J).second->name_replace());
		VERIFY3							((*J).second->ID_Parent == entity->ID,"SERVER : Parent/Children relationship mismatch - Object has children, but children doesn't have parent",(*J).second->name_replace());
	}
}

#endif // DEBUG

shared_str xrServer::level_name(const shared_str &server_options) const
{
	return	(game->level_name(server_options));
}
shared_str xrServer::level_version(const shared_str &server_options) const
{
	return	(game_sv_GameState::parse_level_version(server_options));
}

void xrServer::createClient()
{
	SV_Client = xr_new<CClient>();
	SV_Client->ID.set(1);
	SV_Client->flags.bConnected = TRUE;
}

void xrServer::Disconnect()
{
	SLS_Clear();
	xr_delete(game);
}

#include "GameObject.h"
bool is_object_valid_on_svclient(u16 id_entity)
{
	CObject* tmp_obj		= Level().Objects.net_Find(id_entity);
	if (!tmp_obj)
		return false;
	
	CGameObject* tmp_gobj	= smart_cast<CGameObject*>(tmp_obj);
	if (!tmp_gobj)
		return false;

	if (tmp_obj->getDestroy())
		return false;

	if (tmp_gobj->object_removed())
		return false;
	
	return true;
}

void xrServer::Process_update(NET_Packet& P)
{
	if (!SV_Client) return;

	// while has information
	while (!P.r_eof())
	{
		// find entity
		u16 ID = P.r_u16();
		u8 size = P.r_u8();

		u32	_pos = P.r_tell();
		CSE_Abstract *pSEAbstract = ID_to_entity(ID);

		if (pSEAbstract)
		{
			pSEAbstract->net_Ready = TRUE;
			pSEAbstract->UPDATE_Read(P);

			if ((P.r_tell() - _pos) != size)
			{
				string16 tmp;
				CLSID2TEXT(pSEAbstract->m_tClassID, tmp);
				Debug.fatal(DEBUG_INFO, "Beer from the creator of '%s'; initiator: 0x%08x, r_tell() = %d, pos = %d, size = %d, objectID = %d",
					tmp, SV_Client->ID.value(), P.r_tell(), _pos, size, pSEAbstract->ID);
			}
		}
		else P.r_advance(size);
	}
}

void xrServer::Process_save(NET_Packet& P)
{
	R_ASSERT2(SV_Client, "Process_save client not found");

	// while has information
	while (!P.r_eof())
	{
		// find entity
		u16 ID = P.r_u16();
		u16 size = P.r_u16();

		u32 _pos_start = P.r_tell();
		CSE_Abstract *pSEAbstract = ID_to_entity(ID);

		if (pSEAbstract)
		{
			pSEAbstract->net_Ready = TRUE;
			pSEAbstract->load(P);
		}
		else P.r_advance(size);

		const u32 _pos_end = P.r_tell();

		if (size != (_pos_end - _pos_start)) 
		{
			Msg("! load/save mismatch, object: '%s'", pSEAbstract ? pSEAbstract->name_replace() : "unknown");
			// Rollback pos
			P.r_seek(_pos_start + size);
		}
	}
}

CSE_Abstract* xrServer::Process_spawn(NET_Packet& P, BOOL bSpawnWithClientsMainEntityAsParent, CSE_Abstract* tpExistedEntity)
{
	// create server entity
	CSE_Abstract*	pAbstractE = tpExistedEntity;
	if (!pAbstractE)
	{
		// read spawn information
		string64			s_name;
		P.r_stringZ(s_name);
		// create entity
		pAbstractE = entity_Create(s_name); R_ASSERT3(pAbstractE, "Can't create entity.", s_name);
		pAbstractE->Spawn_Read(P);
		if (!pAbstractE->m_gameType.MatchType((u16)game->Type() || !pAbstractE->match_configuration()))
		{
			F_entity_Destroy(pAbstractE);
			return nullptr;
		}
	}
	else VERIFY(pAbstractE->m_bALifeControl);


	CSE_Abstract *e_parent = nullptr;
	if (pAbstractE->ID_Parent != 0xffff)
	{
		e_parent = ID_to_entity(pAbstractE->ID_Parent);
		if (!e_parent)
		{
			R_ASSERT2(!tpExistedEntity, "Entity don't init!");
			F_entity_Destroy(pAbstractE);
			return nullptr;
		}
	}

	// check for respawn-capability and create phantom as needed
	if (pAbstractE->RespawnTime && (0xffff == pAbstractE->ID_Phantom))
	{
		// Create phantom
		CSE_Abstract* Phantom = entity_Create(pAbstractE->s_name.c_str()); R_ASSERT(Phantom);
		Phantom->Spawn_Read(P);
		Phantom->ID = PerformIDgen(0xffff);
		// Self-linked to avoid phantom-breeding
		Phantom->ID_Phantom = Phantom->ID;
		Phantom->owner = nullptr;
		entities.insert(std::make_pair(Phantom->ID, Phantom));

		Phantom->s_flags.set(M_SPAWN_OBJECT_PHANTOM, true);

		// Spawn entity
		pAbstractE->ID = PerformIDgen(pAbstractE->ID);
		pAbstractE->ID_Phantom = Phantom->ID;
		pAbstractE->owner = SV_Client;
		entities.insert(std::make_pair(pAbstractE->ID, pAbstractE));
	}
	else 
	{
		if (pAbstractE->s_flags.is(M_SPAWN_OBJECT_PHANTOM))
		{
			// Clone from Phantom
			pAbstractE->ID = PerformIDgen(0xffff);
			pAbstractE->owner = SV_Client;
			pAbstractE->s_flags.set(M_SPAWN_OBJECT_PHANTOM, false);
			entities.insert(std::make_pair(pAbstractE->ID, pAbstractE));
		}
		else
		{
			// Simple spawn
			if (bSpawnWithClientsMainEntityAsParent)
			{
				CSE_Abstract* P = SV_Client->owner;
				R_ASSERT(P);
				pAbstractE->ID_Parent = P->ID;
			}
			pAbstractE->ID = PerformIDgen(pAbstractE->ID);
			pAbstractE->owner = SV_Client;
			entities.insert(std::make_pair(pAbstractE->ID, pAbstractE));
		}
	}

	// PROCESS NAME; Name this entity
	if (SV_Client && pAbstractE->s_flags.is(M_SPAWN_OBJECT_ASPLAYER))
	{
		SV_Client->owner = pAbstractE;
	}

	// PROCESS RP;	 3D position/orientation
	pAbstractE->s_RP = 0xFE;	// Use supplied

	// Parent-Connect
	if (!tpExistedEntity) 
	{
		game->OnCreate(pAbstractE->ID);

		if (0xffff != pAbstractE->ID_Parent)
		{
			R_ASSERT(e_parent);

			game->OnTouch(pAbstractE->ID_Parent, pAbstractE->ID);

			e_parent->children.push_back(pAbstractE->ID);
		}
	}

	// create packet and broadcast packet to everybody
	NET_Packet Packet;
	if (SV_Client)
	{
		// For local ONLY
		pAbstractE->Spawn_Write(Packet, TRUE);
		if (pAbstractE->s_flags.is(M_SPAWN_UPDATE))
			pAbstractE->UPDATE_Write(Packet);
		Level().OnMessage(Packet.B.data, (u32)Packet.B.count);

		// For everybody, except client, which contains authorative copy
		pAbstractE->Spawn_Write(Packet, FALSE);
		if (pAbstractE->s_flags.is(M_SPAWN_UPDATE))
			pAbstractE->UPDATE_Write(Packet);
	}
	else // Check it
	{
		pAbstractE->Spawn_Write(Packet, FALSE);
		if (pAbstractE->s_flags.is(M_SPAWN_UPDATE))
			pAbstractE->UPDATE_Write(Packet);
	}

	return pAbstractE;
}

bool xrServer::Process_event_reject(NET_Packet& P, const u32 &time, const u16 id_parent, const u16 id_entity, bool send_message)
{
	// Parse message
	CSE_Abstract*		e_parent = ID_to_entity(id_parent);
	CSE_Abstract*		e_entity = ID_to_entity(id_entity);

	if (!e_entity)
	{
		Msg("[ERROR on rejecting]: entity not found. parent_id = [%d], entity_id = [%d], frame = [%d].", id_parent, id_entity, Device.dwFrame);
		return false;
	}

	if (!e_parent)
	{
		Msg("[ERROR on rejecting]: parent not found. parent_id = [%d], entity_id = [%d], frame = [%d].", id_parent, id_entity, Device.dwFrame);
		return false;
	}
	else if (0xffff == e_entity->ID_Parent)
		return false;

	xr_vector<u16>& C = e_parent->children;
	xr_vector<u16>::iterator c = std::find(C.begin(), C.end(), id_entity);
	if (c == C.end())
	{
		Msg("[ERROR] SV: can't find children [%d] of parent [%d]", id_entity, e_parent);
		return false;
	}


	if (e_entity->ID_Parent != id_parent)
	{
		Msg("[ERROR] e_entity->ID_Parent = [%d]  parent = [%d][%s]  entity_id = [%d]  frame = [%d]",
			e_entity->ID_Parent, id_parent, e_parent->name_replace(), id_entity, Device.dwFrame);
		//it can't be !!!
	}

	game->OnDetach(id_parent, id_entity);

	e_entity->ID_Parent = 0xffff;
	C.erase(c);

	// Signal to everyone (including sender) then is alife started
	if (send_message)
		SendBroadcast(P);

	return true;
}

void xrServer::Perform_transfer(NET_Packet &PR, NET_Packet &PT,	CSE_Abstract* what, CSE_Abstract* from, CSE_Abstract* to)
{
	// Sanity check
	R_ASSERT(what && from && to);
	R_ASSERT(from != to);
	R_ASSERT(what->ID_Parent == from->ID);
	u32 time = Device.dwTimeGlobal;

	// 2. Detach "FROM"
	xr_vector<u16>& C = from->children;
	xr_vector<u16>::iterator c	= std::find	(C.begin(),C.end(),what->ID);
	R_ASSERT(C.end()!=c);
	C.erase(c);

	PR.w_begin	(M_EVENT);
	PR.w_u32	(time);
	PR.w_u16	(GE_OWNERSHIP_REJECT);
	PR.w_u16	(from->ID);
	PR.w_u16	(what->ID);

	// 3. Attach "TO"
	what->ID_Parent= to->ID;
	to->children.push_back(what->ID);

	PT.w_begin	(M_EVENT);
	PT.w_u32	(time+1);
	PT.w_u16	(GE_OWNERSHIP_TAKE);
	PT.w_u16	(to->ID);
	PT.w_u16	(what->ID);
}

void xrServer::Perform_reject(CSE_Abstract* what, CSE_Abstract* from, int delta)
{
	R_ASSERT(what && from);
	R_ASSERT(what->ID_Parent == from->ID);

	NET_Packet P;
	u32 time = Device.dwTimeGlobal - delta;

	P.w_begin	(M_EVENT);
	P.w_u32		(time);
	P.w_u16		(GE_OWNERSHIP_REJECT);
	P.w_u16		(from->ID);
	P.w_u16		(what->ID);
	P.w_u8		(1);

	Process_event_reject(P, time, from->ID, what->ID);
}
void xrServer::Perform_destroy(CSE_Abstract* object)
{
	R_ASSERT(object);
	R_ASSERT(object->ID_Parent == 0xffff);

	while (!object->children.empty())
	{
		CSE_Abstract *child = ID_to_entity(object->children.back());
		R_ASSERT_FORMAT(child, "child registered but not found [%d] [%s]", object->children.back(), object->name());

		Perform_reject(child, object, 2 * NET_Latency);
		Perform_destroy(child);
	}
	u16						object_id = object->ID;
	entity_Destroy(object);

	VERIFY(verify_entities());

	NET_Packet				P;
	P.w_begin(M_EVENT);
	P.w_u32(Device.dwTimeGlobal - 2 * NET_Latency);
	P.w_u16(GE_DESTROY);
	P.w_u16(object_id);
	SendBroadcast(P);
}

void xrServer::SLS_Clear()
{
	while (!entities.empty())
	{
		bool found = false;

		for (auto &entities_it : entities)
		{
			if (entities_it.second->ID_Parent != 0xffff) continue;

			found = true;
			Perform_destroy(entities_it.second);
			break;
		}

		if (!found)
		{
			for (auto &entities_it : entities)
			{
				if (entities_it.second)
					Msg("! ERROR: can't destroy object [%d][%s] with parent [%d]",
						entities_it.second->ID, entities_it.second->s_name.size() ? entities_it.second->s_name.c_str() : "unknown",
						entities_it.second->ID_Parent);
				else Msg("! ERROR: can't destroy entity [%d][?] with parent[?]", entities_it.first);

			}
			Msg("! ERROR: FATAL: can't delete all entities !");
			entities.clear();
		}
	}
}

void xrServer::Process_event(NET_Packet& P)
{
	VERIFY(verify_entities());

	u32 timestamp;
	u16 type;
	u16 destination;

	// correct timestamp with server-unique-time (note: direct message correction)
	P.r_u32(timestamp);

	// read generic info
	P.r_u16(type);
	P.r_u16(destination);

	CSE_Abstract* receiver = ID_to_entity(destination);
	if (receiver)
	{
		R_ASSERT(receiver->owner);
		receiver->OnEvent(P, type, timestamp, SV_Client->ID);
	}

	switch (type)
	{
	case GEG_PLAYER_DISABLE_SPRINT:
	case GEG_PLAYER_ACTIVATE_SLOT:
	case GEG_PLAYER_ITEM_EAT:			Level().OnMessage(P.B.data, (u32)P.B.count); break;
	case GE_TELEPORT_OBJECT: 			game->teleport_object(P, destination); break;
	case GE_INFO_TRANSFER:
	case GE_WPN_STATE_CHANGE:
	case GE_ZONE_STATE_CHANGE:
	case GEG_PLAYER_ATTACH_HOLDER:
	case GEG_PLAYER_DETACH_HOLDER:
	case GEG_PLAYER_ITEM2SLOT:
	case GEG_PLAYER_ITEM2BELT:
	case GEG_PLAYER_ITEM2RUCK:
	case GE_ADDON_ATTACH:
	case GE_ADDON_DETACH:
	case GE_GRENADE_EXPLODE:			SendBroadcast(P); break;
	case GEG_PLAYER_ACTIVATEARTEFACT: 	Process_event_activate(P, destination, P.r_u16()); break;
	case GE_INV_ACTION: 				if (SV_Client) Level().OnMessage(P.B.data, (u32)P.B.count); break;
	case GE_TRADE_BUY:
	case GE_OWNERSHIP_TAKE: 			Process_event_ownership(P, destination); break;
	case GE_TRADE_SELL:
	case GE_OWNERSHIP_REJECT:
	case GE_LAUNCH_ROCKET: 				Process_event_reject(P, timestamp, destination, P.r_u16()); break;
	case GE_DESTROY: 					Process_event_destroy(P, timestamp, destination, nullptr); break;
	case GE_TRANSFER_AMMO:
	{
		u16 id_entity;
		P.r_u16(id_entity);

		// кто забирает (для своих нужд)
		CSE_Abstract* e_parent = receiver;
		// кто отдает
		CSE_Abstract* e_entity = ID_to_entity(id_entity);
		if (!e_entity) break;

		// this item already taken
		if (0xffff != e_entity->ID_Parent) break;

		// Signal to everyone (including sender)
		SendBroadcast(P);

		// Perfrom real destroy
		entity_Destroy(e_entity);
	}
	break;
	case GE_HIT:
	case GE_HIT_STATISTIC:
	{
		P.r_pos -= 2;
		if (type == GE_HIT_STATISTIC)
		{
			P.B.count -= 4;
			P.w_u32(SV_Client->ID.value());
		};
		u16 id_src = P.r_u16();
		CSE_Abstract* e_src = ID_to_entity(id_src);

		if (e_src)
			SendBroadcast(P);
	} break;
	case GE_ASSIGN_KILLER:
	{
		u16 id_src;
		P.r_u16(id_src);

		// кто умер
		CSE_Abstract *e_dest = receiver;
		// this is possible when hit event is sent before destroy event
		if (!e_dest) break;

		CSE_ALifeCreatureAbstract *creature = smart_cast<CSE_ALifeCreatureAbstract*>(e_dest);
		if (creature)
			creature->set_killer_id(id_src);
	} break;
	case GE_CHANGE_VISUAL:
	{
		CSE_Visual* visual = smart_cast<CSE_Visual*>(receiver); VERIFY(visual);
		string256 tmp;
		P.r_stringZ(tmp);
		visual->set_visual(tmp);
	} break;
	case GE_DIE:
	{
		// Parse message
		u16 id_dest = destination, id_src;
		P.r_u16(id_src);

		// кто умер
		CSE_Abstract* e_dest = receiver;
		// this is possible when hit event is sent before destroy event
		if (!e_dest) break;

		CSE_Abstract* e_src = ID_to_entity(id_src);	// кто убил

		if (!e_src)
		{
			Msg("! ERROR: SV: src killer not exist.");
			return;
		}

		game->on_death(e_dest, e_src);

		// клиент, чей юнит убил
		CClient* c_src = e_src->owner;

		if (c_src->owner->ID == id_src)
		{
			// Main unit
			P.w_begin(M_EVENT);
			P.w_u32(timestamp);
			P.w_u16(type);
			P.w_u16(destination);
			P.w_u16(id_src);
			P.w_clientID(c_src->ID);
		}

		SendBroadcast(P);
		//////////////////////////////////////////////////////////////////////////
		P.w_begin(M_EVENT);
		P.w_u32(timestamp);
		P.w_u16(GE_KILL_SOMEONE);
		P.w_u16(id_src);
		P.w_u16(destination);
		Level().OnMessage(P.B.data, (u32)P.B.count);
		//////////////////////////////////////////////////////////////////////////

		VERIFY(verify_entities());
	} break;
	case GE_INSTALL_UPGRADE:
	{
		shared_str upgrade_id;
		P.r_stringZ(upgrade_id);

		CSE_ALifeInventoryItem* iitem = smart_cast<CSE_ALifeInventoryItem*>(receiver);
		if (iitem)
			iitem->add_upgrade(upgrade_id);
	} break;
//	case GE_INV_BOX_STATUS:
	case GE_INV_OWNER_STATUS:
	{
		u8 can_take, closed;
		P.r_u8(can_take);
		P.r_u8(closed);

		CSE_ALifeTraderAbstract* iowner = smart_cast<CSE_ALifeTraderAbstract*>(receiver);
		if (iowner)
		{
			iowner->m_deadbody_can_take = (can_take == 1);
			iowner->m_deadbody_closed = (closed == 1);
		}
	} break;
	case GE_MONEY:
	{
		CSE_Abstract *e_dest = receiver;
		CSE_ALifeTraderAbstract* pTa = smart_cast<CSE_ALifeTraderAbstract*>(e_dest);
		pTa->m_dwMoney = P.r_u32();
	} break;
	case GE_INV_OWNER_SETNAME:
	{
		shared_str name;
		P.r_stringZ(name);

		CSE_ALifeTraderAbstract* iowner = smart_cast<CSE_ALifeTraderAbstract*>(receiver);
		if (iowner)
			iowner->m_character_name = *(CStringTable().translate(name));
	} break;
	default: VERIFY2(0, "Game Event not implemented!!!"); break;
	}
}

void xrServer::Process_event_activate(NET_Packet& P, const u16 id_parent, const u16 id_entity)
{
	// Parse message
	CSE_Abstract* e_parent = ID_to_entity(id_parent);
	CSE_Abstract* e_entity = ID_to_entity(id_entity);

	R_ASSERT_FORMAT(e_parent, "parent not found. id_parent=%d id_entity=%d frame=%d", id_parent, id_entity, Device.dwFrame);
	R_ASSERT_FORMAT(e_entity, "entity not found. id_parent=%d id_entity=%d frame=%d", id_parent, id_entity, Device.dwFrame);

	xr_delete(e_parent);

	if (0xffff != e_entity->ID_Parent)
	{
		// Signal to everyone (including sender)
		SendBroadcast(P);
	}
}

void xrServer::Perform_game_export()
{
	if (!SV_Client->net_Accepted)
		return;

	NET_Packet P;
	P.w_begin(M_SV_CONFIG_GAME);
	game->net_Export_State(P, SV_Client->ID);
	Level().OnMessage(P.B.data, (u32)P.B.count);

	game->sv_force_sync = FALSE;
}

void xrServer::Perform_connect_spawn(CSE_Abstract* E, CClient* CL, NET_Packet& P)
{
	P.B.count = 0;
	if (std::find(conn_spawned_ids.begin(), conn_spawned_ids.end(), E->ID) != conn_spawned_ids.end())
		return;

	conn_spawned_ids.push_back(E->ID);

	if (E->s_flags.is(M_SPAWN_OBJECT_PHANTOM))	return;

	// Connectivity order
	CSE_Abstract* Parent = ID_to_entity(E->ID_Parent);
	if (Parent)
	{
		Perform_connect_spawn(Parent, CL, P);
	}

	// Process
	Flags16 save = E->s_flags;
	//-------------------------------------------------
	E->s_flags.set(M_SPAWN_UPDATE, true);
	if (!E->owner)
	{
		// PROCESS NAME; Name this entity
		if (E->s_flags.is(M_SPAWN_OBJECT_ASPLAYER))
		{
			CL->owner = E;
			E->set_name_replace("");
		}

		// Associate
		E->owner = CL;
		E->Spawn_Write(P, TRUE);
		E->UPDATE_Write(P);

		CSE_ALifeObject*	object = smart_cast<CSE_ALifeObject*>(E);
		VERIFY(object);
		if (!object->keep_saved_data_anyway())
			object->client_data.clear();
	}
	else
	{
		E->Spawn_Write(P, FALSE);
		E->UPDATE_Write(P);
	}
	//-----------------------------------------------------
	E->s_flags = save;
	Level().OnMessage(P.B.data, (u32)P.B.count);
}

void xrServer::SendConnectionData(CClient* _CL)
{
	conn_spawned_ids.clear();
	NET_Packet P;

	// Replicate current entities on to this client
	for (auto &xrSe_it : entities)
		Perform_connect_spawn(xrSe_it.second, _CL, P);

	// Start to send server logo and rules
	Level().game_configured = true;
}

void xrServer::OnCL_Connected()
{
	if (!SV_Client)
	{
		Msg("! ERROR: Player state not created - incorect message sequence!");
		return;
	}

	SV_Client->net_Accepted = TRUE;

	// Export Game Type
	R_ASSERT(Level().Load_GameSpecific_After());
	// end

	Perform_game_export();
	SendConnectionData(SV_Client);
	game->OnPlayerConnect(SV_Client->ID);
}

xrServer::EConnect xrServer::Connect(shared_str &session_name)
{
	// Parse options and create game
	if (!strchr(*session_name, '/'))
		return ErrConnect;

	string1024 options;
	R_ASSERT2(xr_strlen(session_name) <= sizeof(options), "session_name too BIIIGGG!!!");
	xr_strcpy(options, strchr(*session_name, '/') + 1);

	// Parse game type
	string1024 type;
	R_ASSERT2(xr_strlen(options) <= sizeof(type), "session_name too BIIIGGG!!!");
	xr_strcpy(type, options);
	if (strchr(type, '/'))	*strchr(type, '/') = 0;
	game = nullptr;

	CLASS_ID clsid = game_GameState::getCLASS_ID(type, true);
	game = smart_cast<game_sv_GameState*> (NEW_INSTANCE(clsid));

	// Options
	if (!game) return ErrConnect;

	game->Create(session_name);

	connect_options = session_name;
	return ErrNoError;
}

void xrServer::Process_event_ownership(NET_Packet& P, u16 ID)
{
	u16					id_parent = ID, id_entity;
	P.r_u16(id_entity);
	CSE_Abstract*		e_parent = ID_to_entity(id_parent);
	CSE_Abstract*		e_entity = ID_to_entity(id_entity);

	if (!e_parent || !is_object_valid_on_svclient(id_parent))
	{
		Msg("! ERROR on ownership: parent object not found or is not valid. parent_id = [%d], entity_id = [%d], frame = [%d].", id_parent, id_entity, Device.dwFrame);
		return;
	}
	if (!e_entity || !is_object_valid_on_svclient(id_entity))
	{
		Msg("! ERROR on ownership: entity object not found is not valid on sv client. parent_id = [%d], entity_id = [%d], frame = [%d]", id_parent, id_entity, Device.dwFrame);
		return;
	}

	if (0xffff != e_entity->ID_Parent)
		return;

	// Game allows ownership of entity
	game->OnTouch(id_parent, id_entity);

	// Rebuild parentness
	e_entity->ID_Parent = id_parent;
	e_parent->children.push_back(id_entity);

	// Signal to everyone (including sender)
	SendBroadcast(P);
}

void xrServer::Process_event_destroy(NET_Packet& P, const u32 &time, u16 ID, NET_Packet* pEPack)
{
	// Parse message
	u16 id_dest = ID;

	// кто должен быть уничтожен
	CSE_Abstract* e_dest = ID_to_entity(id_dest);
	if (!e_dest)
	{
		return;
	};
	u16 parent_id = e_dest->ID_Parent;

	//---------------------------------------------
	NET_Packet	P2, *pEventPack = pEPack;
	P2.w_begin(M_EVENT_PACK);
	//---------------------------------------------
	// check if we have children 
	if (!e_dest->children.empty())
	{
		if (!pEventPack) pEventPack = &P2;

		while (!e_dest->children.empty())
			Process_event_destroy(P, time, *e_dest->children.begin(), pEventPack);
	}

	if (0xffff == parent_id && nullptr == pEventPack)
	{
		SendBroadcast(P);
	}
	else
	{
		NET_Packet	tmpP;
		if (0xffff != parent_id && Process_event_reject(P, time, parent_id, ID, false))
		{
			CGameObject::u_EventGen(tmpP, GE_OWNERSHIP_REJECT, parent_id);
			tmpP.w_u16(id_dest);
			tmpP.w_u8(1);

			if (!pEventPack) pEventPack = &P2;

			pEventPack->w_u8(u8(tmpP.B.count));
			pEventPack->w(tmpP.B.data, tmpP.B.count);
		};

		CGameObject::u_EventGen(tmpP, GE_DESTROY, id_dest);

		pEventPack->w_u8(u8(tmpP.B.count));
		pEventPack->w(tmpP.B.data, tmpP.B.count);
	}

	if (!pEPack && pEventPack)
	{
		SendBroadcast(*pEventPack);
	}

	if (!game)
	{
		game = new game_sv_GameState();
	}

	// Everything OK, so perform entity-destroy
	if (e_dest->m_bALifeControl && ai().get_alife())
	{
		if (ai().alife().objects().object(id_dest, true))
			game->alife().release(e_dest, false);
	}

	entity_Destroy(e_dest);
}