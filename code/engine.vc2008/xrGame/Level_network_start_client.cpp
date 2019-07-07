#include "stdafx.h"
#include "level.h"
#include "../xrEngine/x_ray.h"
#include "../xrEngine/igame_persistent.h"

#include "ai_space.h"
#include "script_engine.h"

#include "NET_Queue.h"
#include "hudmanager.h"

#include "../xrphysics/iphworld.h"

#include "phcommander.h"
#include "physics_game.h"
#include "GamePersistent.h"
#include "xrServer.h"
#include "..\xrEngine\string_table.h"

extern	pureFrame* g_pNetProcessor;

bool CLevel::net_start_client1()
{
	pApp->LoadBegin();
	//ai().script_engine().init();

	// name_of_server
	string64 name_of_server = "";
	shared_str clientOption = GamePersistent().GetClientOption();
	if (strchr(*clientOption, '/'))
		strncpy_s(name_of_server, *clientOption, strchr(*clientOption, '/') - *clientOption);

	if (strchr(name_of_server, '/'))	*strchr(name_of_server, '/') = 0;

	// Startup client
	string256 temp;
	xr_sprintf(temp, "%s %s", CStringTable().translate("st_client_connecting_to").c_str(), name_of_server);

	pApp->SetLoadStageTitle(temp);
	pApp->LoadStage();

	return true;
}

bool CLevel::net_start_client3()
{
    shared_str const & server_options = Server->GetConnectOptions();

    const char* level_name = name().c_str();
    const char* level_ver = Server->level_version(server_options).c_str();

    // Determine internal level-ID
    int level_id = pApp->Level_ID(level_name, level_ver, true);

    map_data.m_name = level_name;
    map_data.m_map_version = level_ver;
    map_data.m_map_loaded = true;

    deny_m_spawn = FALSE;
    // Load level
    R_ASSERT3(Load(level_id), "Loading failed. Level: %s", level_name);

	return true;
}

bool CLevel::net_start_client4()
{
	// Begin spawn
	g_pGamePersistent->SetLoadStageTitle("st_client_spawning");
	g_pGamePersistent->LoadTitle();

	// Send physics to single or multithreaded mode

	create_physics_world(&ObjectSpace, &Objects, &Device);

	R_ASSERT(physics_world());

	m_ph_commander_physics_worldstep = xr_new<CPHCommander>();
	physics_world()->set_update_callback(m_ph_commander_physics_worldstep);

	physics_world()->set_default_contact_shotmark(ContactShotMark);
	physics_world()->set_default_character_contact_shotmark(CharacterContactShotMark);

	// Send network to single or multithreaded mode
	// *note: release version always has "mt_*" enabled
	Device.seqFrameMT.Remove(g_pNetProcessor);
	Device.seqFrame.Remove(g_pNetProcessor);
	Device.seqFrameMT.Add(g_pNetProcessor, REG_PRIORITY_HIGH + 2);

	return true;
}

bool CLevel::net_start_client5()
{
    // HUD, Textures
    g_pGamePersistent->SetLoadStageTitle("st_loading_textures");
    g_pGamePersistent->LoadTitle();
    Device.m_pRender->DeferredLoad(FALSE);
    Device.m_pRender->ResourcesDeferredUpload();
    deny_m_spawn = TRUE;

	return true;
}

bool CLevel::net_start_client6()
{
    // Sync
    if (!synchronize_client())
        return false;

    if (game_configured)
    {
		g_hud->Load();
		g_hud->OnConnected();
	
		g_pGamePersistent->SetLoadStageTitle("st_client_synchronising");
		pApp->LoadForceFinish();
		g_pGamePersistent->LoadTitle();
		Device.PreCache(60, true, true);
		net_start_result_total = TRUE;
	
    }
	
	pApp->LoadEnd(); 
	return true;
}
