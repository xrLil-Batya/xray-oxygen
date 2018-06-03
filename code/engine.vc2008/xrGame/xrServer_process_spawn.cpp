#include "stdafx.h"
#include "xrServer.h"
#include "xrserver_objects.h"
#include "Level.h"
#ifdef DEBUG
#	include "xrserver_objects_alife_items.h"
#endif

CSE_Abstract* xrServer::Process_spawn(NET_Packet& P, ClientID sender, BOOL bSpawnWithClientsMainEntityAsParent, CSE_Abstract* tpExistedEntity)
{
	// create server entity
	xrClientData* CL	= ID_to_client	(sender);
	CSE_Abstract*	pAbstractE	= tpExistedEntity;
	if (!pAbstractE)
	{
		// read spawn information
		string64			s_name;
		P.r_stringZ			(s_name);
		// create entity
		pAbstractE = entity_Create	(s_name); R_ASSERT3(pAbstractE,"Can't create entity.",s_name);
		pAbstractE->Spawn_Read		(P);
		if(!pAbstractE->m_gameType.MatchType((u16)game->Type() || !pAbstractE->match_configuration()))
		{
			F_entity_Destroy(pAbstractE);
			return nullptr;
		}
	}
	else VERIFY(pAbstractE->m_bALifeControl);
	

	CSE_Abstract *e_parent = 0;
	if (pAbstractE->ID_Parent != 0xffff)
	{
		e_parent = game->get_entity_from_eid(pAbstractE->ID_Parent);
		if (!e_parent) 
		{
			R_ASSERT2(!tpExistedEntity, "Entity don't init!");
			F_entity_Destroy(pAbstractE);
			return nullptr;
		}
	}

	// check if we can assign entity to some client
	if (!CL)
	{
		CL	= (xrClientData*)SV_Client;
	}

	// check for respawn-capability and create phantom as needed
	if (pAbstractE->RespawnTime && (0xffff== pAbstractE->ID_Phantom))
	{
		// Create phantom
		CSE_Abstract* Phantom	=	entity_Create	(pAbstractE->s_name.c_str()); R_ASSERT(Phantom);
		Phantom->Spawn_Read		(P);
		Phantom->ID				=	PerformIDgen	(0xffff);
		Phantom->ID_Phantom		=	Phantom->ID;						// Self-linked to avoid phantom-breeding
		Phantom->owner			=	nullptr;
		entities.insert			(std::make_pair(Phantom->ID,Phantom));

		Phantom->s_flags.set	(M_SPAWN_OBJECT_PHANTOM,TRUE);

		// Spawn entity
		pAbstractE->ID					=	PerformIDgen(pAbstractE->ID);
		pAbstractE->ID_Phantom			=	Phantom->ID;
		pAbstractE->owner				=	CL;
		entities.insert			(std::make_pair(pAbstractE->ID, pAbstractE));
	} else {
		if (pAbstractE->s_flags.is(M_SPAWN_OBJECT_PHANTOM))
		{
			// Clone from Phantom
			pAbstractE->ID					=	PerformIDgen(0xffff);
			pAbstractE->owner				=	CL;//		= SelectBestClientToMigrateTo	(E);
			pAbstractE->s_flags.set			(M_SPAWN_OBJECT_PHANTOM,FALSE);
			entities.insert			(std::make_pair(pAbstractE->ID, pAbstractE));
		} else {
			// Simple spawn
			if (bSpawnWithClientsMainEntityAsParent)
			{
				R_ASSERT				(CL);
				CSE_Abstract* P		= CL->owner;
				R_ASSERT				(P);
				pAbstractE->ID_Parent			= P->ID;
			}
			pAbstractE->ID					=	PerformIDgen(pAbstractE->ID);
			pAbstractE->owner				=	CL;
			entities.insert			(std::make_pair(pAbstractE->ID, pAbstractE));
		}
	}

	// PROCESS NAME; Name this entity
	if (CL && (pAbstractE->s_flags.is(M_SPAWN_OBJECT_ASPLAYER)))
	{
		CL->owner = pAbstractE;
	}

	// PROCESS RP;	 3D position/orientation
	pAbstractE->s_RP					= 0xFE;	// Use supplied

	// Parent-Connect
	if (!tpExistedEntity) {
		game->OnCreate		(pAbstractE->ID);
		
		if (0xffff != pAbstractE->ID_Parent) {
			R_ASSERT					(e_parent);
			
			game->OnTouch			(pAbstractE->ID_Parent,pAbstractE->ID);

			e_parent->children.push_back(pAbstractE->ID);
		}
	}

	// create packet and broadcast packet to everybody
	NET_Packet Packet;
	if (CL) 
	{
		// For local ONLY
		pAbstractE->Spawn_Write		(Packet,TRUE	);
		if (pAbstractE->s_flags.is(M_SPAWN_UPDATE))
			pAbstractE->UPDATE_Write	(Packet);
		Level().OnMessage(Packet.B.data, (u32)Packet.B.count);

		// For everybody, except client, which contains authorative copy
		pAbstractE->Spawn_Write		(Packet,FALSE	);
		if (pAbstractE->s_flags.is(M_SPAWN_UPDATE))
			pAbstractE->UPDATE_Write	(Packet);
		SendBroadcast		(CL->ID,Packet);
	} 
	else
	{
		pAbstractE->Spawn_Write		(Packet,FALSE	);
		if (pAbstractE->s_flags.is(M_SPAWN_UPDATE))
			pAbstractE->UPDATE_Write	(Packet);
		ClientID clientID;clientID.set(0);
		SendBroadcast		(clientID, Packet);
	}

	return pAbstractE;
}
