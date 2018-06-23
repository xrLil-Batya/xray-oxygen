#include "stdafx.h"
#include "xrserver.h"
#include "xrmessages.h"
#include "xrserver_objects.h"
#include "xrServer_Objects_Alife_Monsters.h"
#include "Level.h"
#include "GamePersistent.h"


void xrServer::Perform_connect_spawn(CSE_Abstract* E, CClient* CL, NET_Packet& P, bool bHardProcessed)
{
	P.B.count = 0;
	if(std::find(conn_spawned_ids.begin(), conn_spawned_ids.end(), E->ID) != conn_spawned_ids.end())
		return;
	
	conn_spawned_ids.push_back(E->ID);
	
	if (bHardProcessed)
	{
		E->net_Processed = true;
	}
	else if (E->net_Processed)
	{
		return;
	}

	if (E->s_flags.is(M_SPAWN_OBJECT_PHANTOM))	return;

	// Connectivity order
	CSE_Abstract* Parent = ID_to_entity	(E->ID_Parent);
	if (Parent)
	{
		Perform_connect_spawn(Parent, CL, P, bHardProcessed);
	}

	// Process
	Flags16			save = E->s_flags;
	//-------------------------------------------------
	E->s_flags.set	(M_SPAWN_UPDATE,TRUE);
	if (!E->owner)	
	{
		// PROCESS NAME; Name this entity
		if (E->s_flags.is(M_SPAWN_OBJECT_ASPLAYER))
		{
			CL->owner			= E;
			E->set_name_replace	("");
		}

		// Associate
		E->owner		= CL;
		E->Spawn_Write	(P,TRUE	);
		E->UPDATE_Write	(P);

		CSE_ALifeObject*	object = smart_cast<CSE_ALifeObject*>(E);
		VERIFY				(object);
		if (!object->keep_saved_data_anyway())
			object->client_data.clear	();
	}
	else				
	{
		E->Spawn_Write	(P, FALSE);
		E->UPDATE_Write	(P);
	}
	//-----------------------------------------------------
	E->s_flags = save;
	Level().OnMessage(P.B.data, (u32)P.B.count);
	E->net_Processed = TRUE;
}

void xrServer::SendConfigFinished()
{
	NET_Packet	P;
	P.w_begin	(M_SV_CONFIG_FINISHED);
	Level().OnMessage(P.B.data, (u32)P.B.count);
}

void xrServer::SendConnectionData(CClient* _CL)
{
	conn_spawned_ids.clear();
	NET_Packet P;

	// Replicate current entities on to this client
	for (auto &xrSe_it : entities)
		Perform_connect_spawn(xrSe_it.second, _CL, P, true);

	// Start to send server logo and rules
	SendConfigFinished();
};

void xrServer::OnCL_Connected()
{
	if (!SV_Client)
	{
		Msg("! ERROR: Player state not created - incorect message sequence!");
		return;
	}

    SV_Client->net_Accepted = TRUE;

	// Export Game Type
	NET_Packet P;
	P.w_begin(M_SV_CONFIG_NEW_CLIENT);
	P.w_stringZ(game->type_name());
	Level().OnMessage(P.B.data, (u32)P.B.count);
	// end

	Perform_game_export();
	SendConnectionData(SV_Client);

	game->OnPlayerConnect(SV_Client->ID);
}
