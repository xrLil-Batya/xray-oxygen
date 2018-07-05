#include "stdafx.h"
#include "Level.h"
#include "Level_Bullet_Manager.h"
#include "xrserver.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "PHCommander.h"
#include "net_queue.h"
#include "MainMenu.h"
#include "space_restriction_manager.h"
#include "ai_space.h"
#include "script_engine.h"
#include "stalker_animation_data_storage.h"
#include "client_spawn_manager.h"
#include "seniority_hierarchy_holder.h"
#include "UIGame.h"
#include "string_table.h"
#include "UI/UIGameTutorial.h"
#include "ui/UIPdaWnd.h"
#include "GamePersistent.h"

#include "../xrphysics/physicscommon.h"

const int max_objects_size			= 4096;
const int max_objects_size_in_save	= 8 * 2048;

extern bool	g_b_ClearGameCaptions;

void CLevel::remove_objects()
{
	bool b_stored = psDeviceFlags.test(rsDisableObjectsAsCrows);

	int loop = 5;
	while (loop)
	{
		Server->SLS_Clear();

		for (u32 i = 0; i < 20; ++i)
		{
			snd_Events.clear();
			// ugly hack for checks that update is twice on frame
			// we need it since we do updates for checking network messages
			++(Device.dwFrame);
			psDeviceFlags.set(rsDisableObjectsAsCrows, TRUE);
			ClientReceive();
			ProcessGameEvents();
			Objects.Update(false);
#ifdef DEBUG
			Objects.dump_all_objects();
#endif
		}

		if (Objects.o_count())
		{
			--loop;
			Msg("Objects removal next loop. Active objects count=%d", Objects.o_count());
		}
		else break;

	}

	BulletManager().Clear();
	ph_commander().clear();
	ph_commander_scripts().clear();

	space_restriction_manager().clear();

	psDeviceFlags.set(rsDisableObjectsAsCrows, b_stored);
	g_b_ClearGameCaptions = true;

	ai().script_engine().collect_all_garbage();

	stalker_animation_data_storage().clear();

	VERIFY(Render);
	Render->models_Clear(FALSE);

	Render->clear_static_wallmarks();

#ifdef DEBUG
	if (!client_spawn_manager().registry().empty())
		client_spawn_manager().dump();
#endif // DEBUG
	VERIFY(client_spawn_manager().registry().empty());
	client_spawn_manager().clear();

	g_pGamePersistent->destroy_particles(false);
}

#ifdef DEBUG
	extern void	show_animation_stats	();
#endif // DEBUG

extern CUISequencer * g_tutorial;
extern CUISequencer * g_tutorial2;

void CLevel::net_Stop		()
{
	Msg							("- Disconnect");

	if (GameUI())
	{
		GameUI()->HideShownDialogs();
		GameUI()->PdaMenu().Reset();
	}

	if(g_tutorial && !g_tutorial->Persistent())
		g_tutorial->Stop();

	if(g_tutorial2 && !g_tutorial->Persistent())
		g_tutorial2->Stop();

	bReady						= false;

	remove_objects				();
	
	//WARNING ! remove_objects() uses this flag, so position of this line must e here ..
	game_configured				= FALSE;
	
	IGame_Level::net_Stop		();

	if (Server) 
	{
		Server->Disconnect		();
		xr_delete				(Server);
	}

	ai().script_engine().collect_all_garbage	();

#ifdef DEBUG
	show_animation_stats		();
#endif // DEBUG
}

void CLevel::ClientSend(bool bForce)
{
	static u32 cur_index = 0;

	if (bForce)
		cur_index = 0;

	u32 object_count = Objects.o_count();
	u32 position;
	for (u32 start = cur_index; start < (bForce ? object_count : cur_index + 20); start++)
	{
		CObject	*pO = Objects.o_get_by_iterator(cur_index);

		cur_index++;
		if (cur_index >= object_count)
			cur_index = 0;

		if (pO && !pO->getDestroy() && pO->net_Relevant())
		{
			NET_Packet P;
			P.w_begin(M_UPDATE);

			P.w_u16(u16(pO->ID()));
			P.w_chunk_open8(position);

			pO->net_Export(P);

			P.w_chunk_close8(position);
			if (max_objects_size >= (NET_PacketSizeLimit - P.w_tell()))
				continue;
			Send(P);
		}
	}
}

u32	CLevel::Objects_net_Save	(NET_Packet* _Packet, u32 start, u32 max_object_size)
{
	NET_Packet& Packet	= *_Packet;
	u32			position;
	for (; start<Objects.o_count(); start++)	{
		CObject		*_P = Objects.o_get_by_iterator(start);
		CGameObject *P = smart_cast<CGameObject*>(_P);
		if (P && !P->getDestroy() && P->net_SaveRelevant())	{
			Packet.w_u16			(u16(P->ID())	);
			Packet.w_chunk_open16	(position);
			P->net_Save				(Packet);
#ifdef DEBUG
			u32 size				= u32		(Packet.w_tell()-position)-sizeof(u16);
			if				(size>=65536)			{
				Debug.fatal	(DEBUG_INFO,"Object [%s][%d] exceed network-data limit\n size=%d, Pend=%d, Pstart=%d",
					*P->cName(), P->ID(), size, Packet.w_tell(), position);
			}
#endif
			Packet.w_chunk_close16	(position);
			if (max_object_size >= (NET_PacketSizeLimit - Packet.w_tell()))
				break;
		}
	}
	return	++start;
}

void CLevel::ClientSave()
{
	u32 position;
	for (u32 start = 0; start < Objects.o_count(); start++)
	{
		CObject	*pO = Objects.o_get_by_iterator(start);
		CGameObject *pGO = smart_cast<CGameObject*>(pO);

		if (pGO && !pGO->getDestroy() && pGO->net_SaveRelevant())
		{
			NET_Packet P;
			P.w_begin(M_SAVE_PACKET);

			P.w_u16(pGO->ID());
			P.w_chunk_open16(position);

			pGO->net_Save(P);
			P.w_chunk_close16(position);
			if (max_objects_size >= (NET_PacketSizeLimit - P.w_tell()))
				continue;
			Send(P);
		}
	}
}

void CLevel::Send(NET_Packet& P)
{
	Server->OnMessage(P);
}

void CLevel::net_Update()
{
	if(game_configured)
	{
		// If we have enought bandwidth - replicate client data on to server
		Device.Statistic->netClient2.Begin	();
		ClientSend					();
		Device.Statistic->netClient2.End		();
	}
	// If server - perform server-update
	if (Server)	
	{
		Device.Statistic->netServer.Begin();
		Server->Update					();
		Device.Statistic->netServer.End	();
	}
}

struct _NetworkProcessor : public pureFrame
{
	virtual void	_BCL OnFrame	( )
	{
		if (g_pGameLevel && !Device.Paused() )	g_pGameLevel->net_Update();
	}
}	NET_processor;

pureFrame*	g_pNetProcessor	= &NET_processor;

void CLevel::ClearAllObjects()
{
	u32 CLObjNum = Level().Objects.o_count();

	bool ParentFound = true;
	
	while (ParentFound)
	{	
		ParentFound = false;
		for (u32 i=0; i<CLObjNum; i++)
		{
			CObject* pObj = Level().Objects.o_get_by_iterator(i);
			if (!pObj->H_Parent()) continue;
			//-----------------------------------------------------------
			NET_Packet			GEN;
			GEN.w_begin			(M_EVENT);
			//---------------------------------------------		
			GEN.w_u32			(Level().timeServer());
			GEN.w_u16			(GE_OWNERSHIP_REJECT);
			GEN.w_u16			(pObj->H_Parent()->ID());
			GEN.w_u16			(u16(pObj->ID()));
			game_events->insert	(GEN);
			if (g_bDebugEvents)	ProcessGameEvents();
			//-------------------------------------------------------------
			ParentFound = true;
			//-------------------------------------------------------------
		};
		ProcessGameEvents();
	};

	CLObjNum = Level().Objects.o_count();

	for (u32 i=0; i<CLObjNum; i++)
	{
		CObject* pObj = Level().Objects.o_get_by_iterator(i);
		if (pObj->H_Parent() != nullptr)
			FATAL("pObj->H_Parent()==NULL");

		//-----------------------------------------------------------
		NET_Packet			GEN;
		GEN.w_begin			(M_EVENT);
		//---------------------------------------------		
		GEN.w_u32			(Level().timeServer());
		GEN.w_u16			(GE_DESTROY);
		GEN.w_u16			(u16(pObj->ID()));
		game_events->insert	(GEN);
		if (g_bDebugEvents)	ProcessGameEvents();
		//-------------------------------------------------------------
		ParentFound = true;
		//-------------------------------------------------------------
	};
	ProcessGameEvents();
};
