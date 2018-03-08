#include "stdafx.h"
#include "xrserver.h"
#include "xrmessages.h"
#include "xrserver_objects.h"
#include "xrServer_Objects_Alife_Monsters.h"
#include "Level.h"
#include "GamePersistent.h"


void xrServer::Perform_connect_spawn(CSE_Abstract* E, xrClientData* CL, NET_Packet& P)
{
	P.B.count = 0;
	//xr_vector<u16>::iterator it = std::find(conn_spawned_ids.begin(), conn_spawned_ids.end(), E->ID);
	if(std::find(conn_spawned_ids.begin(), conn_spawned_ids.end(), E->ID) != conn_spawned_ids.end())
		return;
	
	conn_spawned_ids.push_back(E->ID);
	
	if (E->net_Processed)						return;
	if (E->s_flags.is(M_SPAWN_OBJECT_PHANTOM))	return;


	// Connectivity order
	CSE_Abstract* Parent = ID_to_entity	(E->ID_Parent);
	if (Parent)		Perform_connect_spawn	(Parent,CL,P);

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
	E->s_flags			= save;
	SendTo				(CL->ID,P,net_flags(TRUE,TRUE));
	E->net_Processed	= TRUE;
}

void xrServer::SendConfigFinished(ClientID const & clientId)
{
	NET_Packet	P;
	P.w_begin	(M_SV_CONFIG_FINISHED);
	SendTo		(clientId, P, net_flags(TRUE,TRUE));
}

void xrServer::SendConnectionData(IClient* _CL)
{
	conn_spawned_ids.clear();
	xrClientData*	CL = (xrClientData*)_CL;
	NET_Packet		P;
	// Replicate current entities on to this client

	for (auto &xrSe_it: entities)
		xrSe_it.second->net_Processed = FALSE;

	for (auto &xrSe_it : entities)
		Perform_connect_spawn(xrSe_it.second, CL, P);

	// Start to send server logo and rules
	SendConfigFinished(CL->ID);
};

void xrServer::OnCL_Connected(IClient* _CL)
{
	xrClientData* CL = (xrClientData*)_CL;

	if (!CL)
	{
		Msg("! ERROR: Player state not created - incorect message sequence!");
		return;
	}

	CL->net_Accepted = TRUE;

	Export_game_type(CL);
	Perform_game_export();
	SendConnectionData(CL);

	game->OnPlayerConnect(CL->ID);	
}

void	xrServer::SendConnectResult(IClient* CL, u8 res, u8 res1, char* ResultStr)
{
	NET_Packet	P;
	P.w_begin	(M_CLIENT_CONNECT_RESULT);
	P.w_u8		(res);
	P.w_u8		(res1);
	P.w_stringZ	(ResultStr);
	P.w_clientID(CL->ID);

	if (SV_Client && SV_Client == CL)
		P.w_u8(1);
	else
		P.w_u8(0);
	P.w_stringZ(GamePersistent().GetServerOption());
	
	SendTo		(CL->ID, P);

	if (!res)			//need disconnect 
	{
		Flush_Clients_Buffers	();
		DisconnectClient		(CL, ResultStr);
	}

	if (Level().IsDemoPlay())
	{
		Level().StartPlayDemo();

		return;
	}
}

void xrServer::OnBuildVersionRespond				( IClient* CL, NET_Packet& P )
{
// 	imdex NOTE: useless now
	RequestClientDigest(CL);
};

void xrServer::Check_BuildVersion_Success			( IClient* CL )
{
	CL->flags.bVerified = TRUE;
	SendConnectResult(CL, 1, 0, "All Ok");
};