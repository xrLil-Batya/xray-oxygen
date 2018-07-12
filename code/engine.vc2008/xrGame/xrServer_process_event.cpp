#include "stdafx.h"
#include "xrServer.h"
#include "alife_simulator.h"
#include "xrserver_objects.h"
#include "game_base.h"
#include "game_cl_base.h"
#include "ai_space.h"
#include "alife_object_registry.h"
#include "xrServer_Objects_ALife_Items.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "Level.h"
#include "string_table.h"

void xrServer::Process_event	(NET_Packet& P)
{
	VERIFY					(verify_entities());

	u32			timestamp;
	u16			type;
	u16			destination;

    ClientID sender = SV_Client->ID;

	// correct timestamp with server-unique-time (note: direct message correction)
	P.r_u32		(timestamp	);

	// read generic info
	P.r_u16		(type		);
	P.r_u16		(destination);

	CSE_Abstract* receiver = game->get_entity_from_eid(destination);
	if (receiver)	
	{
		R_ASSERT(receiver->owner);
		receiver->OnEvent						(P,type,timestamp,sender);

	};

	switch		(type)
	{
	case GE_INFO_TRANSFER:
	case GE_WPN_STATE_CHANGE:
	case GE_ZONE_STATE_CHANGE:
	case GEG_PLAYER_ATTACH_HOLDER:
	case GEG_PLAYER_DETACH_HOLDER:
	case GEG_PLAYER_ITEM2SLOT:
	case GEG_PLAYER_ITEM2BELT:
	case GEG_PLAYER_ITEM2RUCK:
	case GE_GRENADE_EXPLODE:
		{
		SendBroadcast			(BroadcastCID,P);
		}break;
	case GEG_PLAYER_ACTIVATEARTEFACT: 	Process_event_activate(P,destination,P.r_u16()); break;
	case GE_INV_ACTION: 				if (SV_Client) Level().OnMessage(P.B.data, (u32)P.B.count); break;
	case GE_TRADE_BUY:
	case GE_OWNERSHIP_TAKE: 			Process_event_ownership	(P, destination); break;
	case GE_TRADE_SELL:
	case GE_OWNERSHIP_REJECT:
	case GE_LAUNCH_ROCKET: 				Process_event_reject(P,sender,timestamp,destination,P.r_u16()); break;
	case GE_DESTROY: 					Process_event_destroy(P,sender,timestamp,destination, NULL); break;
	case GE_TRANSFER_AMMO:
		{
			u16					id_entity;
			P.r_u16				(id_entity);
			CSE_Abstract*		e_parent	= receiver;	// кто забирает (для своих нужд)
			CSE_Abstract*		e_entity	= game->get_entity_from_eid	(id_entity);	// кто отдает
			if (!e_entity)
                break;
			if (0xffff != e_entity->ID_Parent)
                break;						// this item already taken
            CClient* c_parent	= e_parent->owner;
			R_ASSERT(SV_Client == c_parent);						// assure client ownership of event

			// Signal to everyone (including sender)
			SendBroadcast		(BroadcastCID,P);

			// Perfrom real destroy
			entity_Destroy		(e_entity	);
			VERIFY				(verify_entities());
		}
		break;
	case GE_HIT:
	case GE_HIT_STATISTIC:
		{
			P.r_pos -=2;
			if (type == GE_HIT_STATISTIC) 
			{
				P.B.count -= 4;
				P.w_u32(sender.value());
			};
            u16 id_src = P.r_u16();
            CSE_Abstract* e_src = game->get_entity_from_eid(id_src);

            if (e_src)
                SendBroadcast(BroadcastCID, P);
		} break;
	case GE_ASSIGN_KILLER: {
		u16							id_src;
		P.r_u16						(id_src);
		
		CSE_Abstract				*e_dest = receiver;	// кто умер
		// this is possible when hit event is sent before destroy event
		if (!e_dest)
			break;

		CSE_ALifeCreatureAbstract	*creature = smart_cast<CSE_ALifeCreatureAbstract*>(e_dest);
		if (creature)
			creature->set_killer_id( id_src );

		break;
	}
	case GE_CHANGE_VISUAL:
		{
			CSE_Visual* visual		= smart_cast<CSE_Visual*>(receiver); VERIFY(visual);
			string256 tmp;
			P.r_stringZ				(tmp);
			visual->set_visual		(tmp);
		}break;
	case GE_DIE:
		{
			// Parse message
			u16					id_dest		=	destination, id_src;
			P.r_u16				(id_src);
            
			VERIFY				(game && SV_Client);

			CSE_Abstract*		e_dest		= receiver;	// кто умер
			// this is possible when hit event is sent before destroy event
			if (!e_dest)
				break;

			CSE_Abstract* e_src = game->get_entity_from_eid(id_src);	// кто убил
			if (!e_src) 
			{
                CClient* C = (CClient*)game->get_client(id_src);
				if (C) e_src = C->owner;
			};

			if (!e_src)
			{
				Msg("! ERROR: SV: src killer not exist.");
				return;
			}

			game->on_death		(e_dest,e_src);

            CClient*		c_src		= e_src->owner;				// клиент, чей юнит убил

			if (c_src->owner->ID == id_src) {
				// Main unit
				P.w_begin			(M_EVENT);
				P.w_u32				(timestamp);
				P.w_u16				(type);
				P.w_u16				(destination);
				P.w_u16				(id_src);
				P.w_clientID		(c_src->ID);
			}

			SendBroadcast			(BroadcastCID,P);

			//////////////////////////////////////////////////////////////////////////
			// 
			P.w_begin			(M_EVENT);
			P.w_u32				(timestamp);
			P.w_u16				(GE_KILL_SOMEONE);
			P.w_u16				(id_src);
			P.w_u16				(destination); 
			Level().OnMessage(P.B.data, (u32)P.B.count);
			//////////////////////////////////////////////////////////////////////////

			VERIFY					(verify_entities());
		}
		break;
	case GE_ADDON_ATTACH:
	case GE_ADDON_DETACH:
		{
			SendBroadcast	(BroadcastCID, P);
		}break;
	case GE_CHANGE_POS: Level().OnMessage(P.B.data, (u32)P.B.count); break;
	case GE_INSTALL_UPGRADE:
		{
			shared_str				upgrade_id;
			P.r_stringZ				( upgrade_id );
			CSE_ALifeInventoryItem* iitem = smart_cast<CSE_ALifeInventoryItem*>( receiver );
			if ( !iitem )
			{
				break;
			}
			iitem->add_upgrade		( upgrade_id );
		}break;
	case GE_INV_BOX_STATUS:
		{
			u8 can_take, closed;
			P.r_u8	( can_take );
			P.r_u8	( closed );
			shared_str tip_text;
			P.r_stringZ( tip_text );

			CSE_ALifeInventoryBox* box = smart_cast<CSE_ALifeInventoryBox*>( receiver );
			if ( !box )
			{
				break;
			}
			box->m_can_take = (can_take == 1);
			box->m_closed   = (closed == 1);
			box->m_tip_text._set( tip_text );
		}break;
	case GE_INV_OWNER_STATUS:
		{
			u8 can_take, closed;
			P.r_u8	( can_take );
			P.r_u8	( closed );

			CSE_ALifeTraderAbstract* iowner = smart_cast<CSE_ALifeTraderAbstract*>( receiver );
			if ( !iowner )
			{
				break;
			}
			iowner->m_deadbody_can_take = (can_take == 1);
			iowner->m_deadbody_closed   = (closed == 1);
		}break;

	case GEG_PLAYER_DISABLE_SPRINT:
	case GEG_PLAYER_WEAPON_HIDE_STATE:
	case GEG_PLAYER_ACTIVATE_SLOT:
	case GEG_PLAYER_ITEM_EAT:			Level().OnMessage(P.B.data, (u32)P.B.count); break;
	case GE_TELEPORT_OBJECT: 			game->teleport_object(P,destination); break;
	case GEG_PLAYER_USE_BOOSTER:
		{
			if (receiver && receiver->owner && (receiver->owner != SV_Client))
			{
				NET_Packet tmp_packet;
				CGameObject::u_EventGen(tmp_packet, GEG_PLAYER_USE_BOOSTER, receiver->ID);
				Level().OnMessage(P.B.data, (u32)P.B.count);
			}
		}break;
	case GE_MONEY:
		{
			CSE_Abstract				*e_dest = receiver;
			CSE_ALifeTraderAbstract*	pTa = smart_cast<CSE_ALifeTraderAbstract*>(e_dest);
			pTa->m_dwMoney				= P.r_u32();
		}break;
	case GE_INV_OWNER_SETNAME:
	{
		shared_str name;
		P.r_stringZ(name);
		CSE_ALifeTraderAbstract* iowner = smart_cast<CSE_ALifeTraderAbstract*>(receiver);
		if (!iowner)
		{
			break;
		}
		iowner->m_character_name = *(CStringTable().translate(name));
	}break;
	
	default:
		VERIFY2	(0,"Game Event not implemented!!!");
		break;
	}
}
