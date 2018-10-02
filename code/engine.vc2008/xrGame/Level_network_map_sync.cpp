#include "stdafx.h"
#include "level.h"
#include "../xrCore/stream_reader.h"
#include "../xrEngine/string_table.h"
#include "../xrEngine/xr_ioconsole.h"

bool CLevel::synchronize_client()
{
    map_data.m_map_sync_received = true;
    deny_m_spawn = FALSE;
	if (!game_configured)
	{
		//Don't delete this ClientReceive(). Without that, game_configured variable will not changed
        ClientReceive();
        Server->OnCL_Connected();
		Server->Update();

		ClientReceive();
	}

	return !!game_configured;
}
