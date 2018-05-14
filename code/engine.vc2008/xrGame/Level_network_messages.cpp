#include "stdafx.h"
#include "entity.h"
#include "xrserver_objects.h"
#include "level.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "net_queue.h"
#include "xrServer.h"
#include "Actor.h"
#include "Artefact.h"
#include "ai_space.h"
#include "saved_game_wrapper.h"
#include "level_graph.h"
#include "message_filter.h"
#include "../xrphysics/iphworld.h"
#include "GamePersistent.h"

extern LPCSTR map_ver_string;
LPSTR remove_version_option(LPCSTR opt_str, LPSTR new_opt_str, u32 new_opt_str_size)
{
	LPCSTR temp_substr = strstr(opt_str, map_ver_string);
	if (!temp_substr)
	{
		xr_strcpy(new_opt_str, new_opt_str_size, opt_str);
		return new_opt_str;
	}
	strncpy_s(new_opt_str, new_opt_str_size, opt_str, static_cast<size_t>(temp_substr - opt_str - 1));
	temp_substr = strchr(temp_substr, '/');
	if (!temp_substr)
		return new_opt_str;

	xr_strcat(new_opt_str, new_opt_str_size, temp_substr);
	return new_opt_str;
}

void CLevel::ClientReceive()
{
	m_dwRPC = 0;
	m_dwRPS = 0;
	
	if (IsDemoPlayStarted())
	{
		SimulateServerUpdate();
	}

	StartProcessQueue();
	for (NET_Packet* P = net_msg_Retreive(); P; P=net_msg_Retreive())
	{
		if (IsDemoSaveStarted())
		{
			SavePacket(*P);
		}
		//-----------------------------------------------------
		m_dwRPC++;
		m_dwRPS += (u32)P->B.count;
		//-----------------------------------------------------
		u16 m_type;
		P->r_begin	(m_type);
		switch (m_type)
		{
		case M_SPAWN:			
			{
				if (!bReady) //!m_bGameConfigStarted || 
				{
					Msg ("! Unconventional M_SPAWN received : map_data[%s] | bReady[%s] | deny_m_spawn[%s]",
						(map_data.m_map_sync_received) ? "true" : "false",
						(bReady) ? "true" : "false",
						deny_m_spawn ? "true" : "false");
					break;
				}
				game_events->insert		(*P);
				if (g_bDebugEvents)		ProcessGameEvents();
			}
			break;
		case M_EVENT:
			game_events->insert		(*P);
			if (g_bDebugEvents)		ProcessGameEvents();
			break;
		case M_EVENT_PACK:
			{
				NET_Packet	tmpP;
				while (!P->r_eof())
				{
					tmpP.B.count = P->r_u8();
					P->r(&tmpP.B.data, tmpP.B.count);
					tmpP.timeReceive = P->timeReceive;

					game_events->insert		(tmpP);
					if (g_bDebugEvents)		ProcessGameEvents();
				};			
			}break;
		case M_UPDATE:
			{
				game->net_import_update	(*P);
			}break;
		case M_SV_CONFIG_NEW_CLIENT:
			InitializeClientGame	(*P);
			break;
		case M_SV_CONFIG_GAME:
			game->net_import_state(*P);
			break;
		case M_SV_CONFIG_FINISHED:
			{
				game_configured			= TRUE;
			}break;
		case M_LOAD_GAME:
		case M_CHANGE_LEVEL:
			{
				if(m_type==M_LOAD_GAME)
				{
					string256						saved_name;
					P->r_stringZ_s					(saved_name);
					if(xr_strlen(saved_name) && ai().get_alife())
					{
						CSavedGameWrapper			wrapper(saved_name);
						if (wrapper.level_id() == ai().level_graph().level_id()) 
						{
							Engine.Event.Defer	("Game:QuickLoad", size_t(xr_strdup(saved_name)), 0);

							break;
						}
					}
				}
				MakeReconnect();
			}break;
		case M_SAVE_GAME:
			{
				ClientSave			();
			}break;
		case M_CLIENT_CONNECT_RESULT:
			{
				OnConnectResult(P);
			}break;
		case M_SV_MAP_NAME:
			{
				map_data.ReceiveServerMapSync(*P);
			}break;
		case M_BULLET_CHECK_RESPOND:
			{
			}break;
		}
		net_msg_Release();
	}	
	EndProcessQueue();

	if (g_bDebugEvents) ProcessGameSpawns();
}

void CLevel::OnMessage(void* data, u32 size)
{	
	IPureClient::OnMessage(data, size);	
};
