#include "stdafx.h"
#include "level.h"
#include "Level_Bullet_Manager.h"
#include "xrserver.h"

#include "xrmessages.h"
#include "../xrEngine/x_ray.h"
#include "../xrEngine/device.h"
#include "../xrEngine/IGame_Persistent.h"
#include "../xrEngine/xr_ioconsole.h"
#include "..\xrEngine\string_table.h"
#include "UIGame.h"
#include "GamePersistent.h"

BOOL CLevel::net_Start(LPCSTR op_server, LPCSTR op_client)
{
	net_start_result_total = TRUE;

	pApp->LoadBegin();

	string64 player_name;
	xr_strcpy(player_name, xr_strlen(Core.UserName) ? Core.UserName : Core.CompName);

	VERIFY(xr_strlen(player_name));

	//make Client Name if options
	string512 tmp;
	xr_strcpy(tmp, op_client);
	xr_strcat(tmp, "/name=");
	xr_strcat(tmp, player_name);

	// Send options to GamePersistent
	GamePersistent().SetClientOption(tmp);
	GamePersistent().SetServerOption(op_server);
	//---------------------------------------------------------------------------
	g_loading_events.emplace_back(this, &CLevel::net_start1);
	g_loading_events.emplace_back(this, &CLevel::net_start2);
	g_loading_events.emplace_back(this, &CLevel::net_start4);
	g_loading_events.emplace_back(this, &CLevel::net_start6);

	return net_start_result_total;
}

// Start client and server
bool CLevel::net_start1()
{
    shared_str serverOption = GamePersistent().GetServerOption();

	g_pGamePersistent->SetLoadStageTitle("st_server_starting");
	g_pGamePersistent->LoadTitle();

	IGame_Persistent::params &p = g_pGamePersistent->m_game_params;

	// Connect
	Server = xr_new<xrServer>();

	if (xr_strcmp(p.m_alife, "alife"))
	{
		shared_str l_ver = game_sv_GameState::parse_level_version(serverOption);
		map_data.m_name = Server->game->level_name(serverOption);
		
		g_pGamePersistent->LoadTitle(true, map_data.m_name);

		int id = pApp->Level_ID(map_data.m_name.c_str(), l_ver.c_str(), true);

		if (id<0)
		{
			Msg("Can't find level: %s",map_data.m_name.c_str());
			net_start_result_total	= FALSE;
			return true;
		}
	}

	return true;
}

bool CLevel::net_start2()
{
    shared_str serverOption = GamePersistent().GetServerOption();
	if (net_start_result_total && serverOption.size())
	{
		if ((m_connect_server_err=Server->Connect(serverOption))!=xrServer::ErrNoError)
		{
			net_start_result_total = false;
			Msg("! Failed to start server.");
			return true;
		}
		Server->game->sls_default();
		map_data.m_name = Server->level_name(serverOption);
		g_pGamePersistent->LoadTitle(true, map_data.m_name);
		Server->createClient();
	}
	return true;
}

bool CLevel::net_start4()
{
	if(!net_start_result_total) return true;

	g_loading_events.pop_front();

    pApp->LoadBegin();

	g_loading_events.push_front	(LOADING_EVENT(this,&CLevel::net_start_client6));
	g_loading_events.push_front	(LOADING_EVENT(this,&CLevel::net_start_client5));
	g_loading_events.push_front	(LOADING_EVENT(this,&CLevel::net_start_client4));
	g_loading_events.push_front	(LOADING_EVENT(this,&CLevel::net_start_client3));
	g_loading_events.push_front	(LOADING_EVENT(this,&CLevel::net_start_client1));

	return false;
}

bool CLevel::net_start6				()
{
	//init bullet manager
	BulletManager().Clear		();
	BulletManager().Load		();

	pApp->LoadEnd				();

	if(net_start_result_total)
	{
		if (strstr(Core.Params,"-$")) 
		{
			string256 buf,cmd,param;
			sscanf(strstr(Core.Params,"-$")+2,"%[^ ] %[^ ] ",cmd,param);
			xr_strconcat(buf,cmd," ",param);
			Console->Execute(buf);
		}
	}
	else
	{
		Msg("! Failed to start client. Check the connection or level existance.");
		
// 		if (!map_data.m_map_loaded && !map_data.m_name.size())
// 		{
// 			LPCSTR level_id_string = nullptr;
// 			LPCSTR dialog_string = nullptr;
// 			CStringTable	st;
// 			LPCSTR tmp_map_ver = !!map_data.m_map_version ? map_data.m_map_version.c_str() : "";
// 
// 			STRCONCAT(level_id_string, st.translate("st_level"), ":", map_data.m_name.c_str(), "(", tmp_map_ver, "). ");
// 			STRCONCAT(dialog_string, level_id_string, st.translate("ui_st_map_not_found"));
// 		}
		DEL_INSTANCE(g_pGameLevel);
		Console->Execute("main_menu on");

		return true;
	}

	GameUI()->OnConnected();

	return true;
}

void CLevel::InitializeClientGame(NET_Packet& P)
{
	R_ASSERT(Load_GameSpecific_After());
}