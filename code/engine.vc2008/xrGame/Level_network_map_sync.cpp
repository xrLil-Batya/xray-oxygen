#include "stdafx.h"
#include "level.h"
#include "../xrCore/stream_reader.h"
#include "MainMenu.h"
#include "string_table.h"
#include "../xrEngine/xr_ioconsole.h"

bool CLevel::synchronize_map_data()
{
	map_data.m_map_sync_received = true;
	return synchronize_client();
}

bool	CLevel::synchronize_client()
{
    if (!sended_request_connection_data)
    {
        NET_Packet P;
        P.w_begin(M_CLIENT_REQUEST_CONNECTION_DATA);
        Send(P);
        sended_request_connection_data = true;
    }

    deny_m_spawn = FALSE;
	if (game_configured)
		return true;

	if (Server)
	{
		ClientReceive();
		Server->Update();

        //Don't delete this ClientReceive(). Without that, game_configured variable will not changed
        ClientReceive();
	}	// if OnClient ClientReceive method called in upper invokation

	return !!game_configured;
}
