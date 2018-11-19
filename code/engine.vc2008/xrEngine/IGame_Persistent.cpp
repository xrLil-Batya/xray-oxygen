#include "stdafx.h"
#pragma hdrstop

#include "IGame_Persistent.h"

#ifndef _EDITOR
#include "environment.h"
#	include "x_ray.h"
#	include "IGame_Level.h"
#	include "XR_IOConsole.h"
#	include "Render.h"
#	include "ps_instance.h"
#	include "CustomHUD.h"
#endif
#include "Spectre\Spectre.h"
#ifdef INGAME_EDITOR
#	include "editor_environment_manager.hpp"
#endif // INGAME_EDITOR

ENGINE_API	IGame_Persistent*		g_pGamePersistent	= nullptr;

IGame_Persistent::IGame_Persistent	()
{
	RDEVICE.seqAppStart.Add			(this);
	RDEVICE.seqAppEnd.Add			(this);
	RDEVICE.seqFrame.Add			(this,REG_PRIORITY_HIGH+1);
	RDEVICE.seqAppActivate.Add		(this);
	RDEVICE.seqAppDeactivate.Add	(this);

	m_pMainMenu						= nullptr;
	Environment().load();
	m_pGShaderConstants = ShadersExternalData();
}

IGame_Persistent::~IGame_Persistent()
{
	RDEVICE.seqFrame.Remove(this);
	RDEVICE.seqAppStart.Remove(this);
	RDEVICE.seqAppEnd.Remove(this);
	RDEVICE.seqAppActivate.Remove(this);
	RDEVICE.seqAppDeactivate.Remove(this);
}

void IGame_Persistent::OnAppActivate		()
{
}

void IGame_Persistent::OnAppDeactivate		()
{
}

void IGame_Persistent::OnAppStart	()
{
}

void IGame_Persistent::OnAppEnd		()
{
#ifndef _EDITOR
	Environment().unload			 ();
#endif    
	OnGameEnd						();

#ifndef _EDITOR
	DEL_INSTANCE					(g_hud);
#endif    
}


void IGame_Persistent::PreStart		(LPCSTR op)
{
	string256						prev_type;
	params							new_game_params;
	xr_strcpy							(prev_type,m_game_params.m_game_type);
	new_game_params.parse_cmd_line	(op);

	// change game type
	if (0!=xr_strcmp(prev_type,new_game_params.m_game_type)){
		OnGameEnd					();
	}
}
void IGame_Persistent::Start(LPCSTR op)
{
	string256 prev_type;
	xr_strcpy (prev_type,m_game_params.m_game_type);
	m_game_params.parse_cmd_line	(op);
	// change game type
	if (xr_strcmp(prev_type,m_game_params.m_game_type))
	{
		if (*m_game_params.m_game_type)
			OnGameStart();

		if(g_hud)
			DEL_INSTANCE(g_hud);
	}
	else UpdateGameType();

	VERIFY							(ps_destroy.empty());
}

void IGame_Persistent::Disconnect	()
{
#ifndef _EDITOR
	// clear "need to play" particles
	destroy_particles					(true);

	if(g_hud)
			DEL_INSTANCE			(g_hud);
//.		g_hud->OnDisconnected			();
#endif
	ObjectPool.clear();
 	Render->models_Clear(TRUE); 
}

void IGame_Persistent::OnGameStart()
{
#ifndef _EDITOR
	SetLoadStageTitle("st_prefetching_objects");
	LoadTitle();
	if(!strstr(Core.Params,"-noprefetch"))
		Prefetch();
#endif
}

#ifndef _EDITOR
void IGame_Persistent::Prefetch()
{
	// prefetch game objects & models
	float	p_time		=			1000.f*Device.GetTimerGlobal()->GetElapsed_sec();
	u32	mem_0			=			Memory.mem_usage()	;

	Log				("Loading objects...");
	ObjectPool.prefetch					();
	Log				("Loading models...");
	Render->models_Prefetch				();
	//Device.Resources->DeferredUpload	();
	Device.m_pRender->ResourcesDeferredUpload();

	p_time				=			1000.f*Device.GetTimerGlobal()->GetElapsed_sec() - p_time;
	u32		p_mem		=			Memory.mem_usage() - mem_0	;

	Msg					("* [prefetch] time:    %d ms",	iFloor(p_time));
	Msg					("* [prefetch] memory:  %dKb",	p_mem/1024);
}
#endif


void IGame_Persistent::OnGameEnd	()
{
#ifndef _EDITOR
	ObjectPool.clear					();
	Render->models_Clear				(TRUE);
#endif
}

void IGame_Persistent::OnFrame()
{
	if (!Device.Paused() || Device.dwPrecacheFrame)
	{
		Environment().OnFrame();
		SpectreCallback::shedule_update->Invoke(SpectreObjectId, Device.dwTimeDelta);
	}

	Device.Statistic->Particles_starting = (u32)ps_needtoplay.size();
	Device.Statistic->Particles_active = (u32)ps_active.size();
	Device.Statistic->Particles_destroy = (u32)ps_destroy.size();

	// Play req particle systems
	while (ps_needtoplay.size())
	{
		CPS_Instance* pInstance = ps_needtoplay.back();
		ps_needtoplay.pop_back();
		pInstance->Play(false);
	}

	// Destroy inactive particle systems
	while (ps_destroy.size())
	{
		CPS_Instance* pInstance = ps_destroy.back();
		if (pInstance->Locked())
		{
			Log("--locked");
			break;
		}
		ps_destroy.pop_back();
		pInstance->PSI_internal_delete();
	}
}

void IGame_Persistent::destroy_particles		(const bool &all_particles)
{
#ifndef _EDITOR
	ps_needtoplay.clear				();

	while (ps_destroy.size())
	{
		CPS_Instance*	psi		= ps_destroy.back	();		
		VERIFY					(psi);
		VERIFY					(!psi->Locked());
		ps_destroy.pop_back		();
		psi->PSI_internal_delete();
	}

	// delete active particles
	if (all_particles) 
	{
		while (!ps_active.empty())
		{
			(*ps_active.begin())->PSI_internal_delete();
		}
	}
	else
	{
		size_t active_size = ps_active.size();
		CPS_Instance **I = (CPS_Instance**)_alloca(active_size * sizeof(CPS_Instance*));
		std::copy(ps_active.begin(), ps_active.end(), I);

		CPS_Instance **E = std::remove_if(I, I + active_size, [](CPS_Instance*const& object){ return (!object->destroy_on_game_load()); });
		for (; I != E; ++I)
			(*I)->PSI_internal_delete();
	}

	VERIFY(ps_needtoplay.empty() && ps_destroy.empty() && (!all_particles || ps_active.empty()));
#endif
}

void IGame_Persistent::OnAssetsChanged()
{
#ifndef _EDITOR
	Device.m_pRender->OnAssetsChanged(); //Resources->m_textures_description.Load();
#endif    
}
