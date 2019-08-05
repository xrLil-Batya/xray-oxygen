#include "stdafx.h"
#include "game_base.h"
#include "ai_space.h"
#include "script_engine.h"
#include "level.h"
#include "xrMessages.h"
#include "../xrEngine/string_table.h"

u64		g_qwStartGameTime		= 12*60*60*1000;
float	g_fTimeFactor			= pSettings->r_float("alife","time_factor");
u64		g_qwEStartGameTime		= 12*60*60*1000;

game_GameState::game_GameState()
{
	m_type						= EGameIDs(u32(0));
	m_phase						= GAME_PHASE_NONE;
	m_round_start_time_str[0]	= 0;

	VERIFY						(g_pGameLevel);
	m_qwStartProcessorTime		= Level().timeServer_Async();
	m_qwStartGameTime			= g_qwStartGameTime;
	m_fTimeFactor				= g_fTimeFactor;
	m_qwEStartProcessorTime		= m_qwStartProcessorTime;	
	m_qwEStartGameTime			= g_qwEStartGameTime	;
	m_fETimeFactor				= m_fTimeFactor			;
}

void game_GameState::net_import_state(NET_Packet & P)
{
	// Generic
	u16 ph;
	P.r_u16(ph);

	if (Phase() != ph)
		switch_Phase(ph);

	P.r_u32(m_start_time);

	net_import_GameTime(P);
}

void game_GameState::net_import_GameTime(NET_Packet & P)
{
	u64	GameTime;
	P.r_u64(GameTime);
	float TimeFactor;
	P.r_float(TimeFactor);

	Level().SetGameTimeFactor(GameTime, TimeFactor);

	u64	GameEnvironmentTime;
	P.r_u64(GameEnvironmentTime);
	float EnvironmentTimeFactor;
	P.r_float(EnvironmentTimeFactor);

	u64 OldTime = Level().GetEnvironmentGameTime();
	Level().SetEnvironmentGameTimeFactor(GameEnvironmentTime, EnvironmentTimeFactor);

	if (OldTime > GameEnvironmentTime)
		Environment().Invalidate();
}

CLASS_ID game_GameState::getCLASS_ID(LPCSTR game_type_name, bool isServer)
{
	return(isServer)?TEXT2CLSID("SV_SINGL"):TEXT2CLSID("CL_SINGL");
}

void game_GameState::switch_Phase		(u32 new_phase)
{
	m_phase				= u16(new_phase);
	m_start_time		= Level().timeServer();
}

u64 game_GameState::GetStartGameTime()
{
	return			(m_qwStartGameTime);
}

u64 game_GameState::GetGameTime()
{
	return (m_qwStartGameTime + u64(m_fTimeFactor*float(Level().timeServer_Async() - m_qwStartProcessorTime)));
}

float game_GameState::GetGameTimeFactor()
{
	return			(m_fTimeFactor);
}

void game_GameState::SetGameTimeFactor (const float fTimeFactor)
{
	m_qwStartGameTime			= GetGameTime();
	m_qwStartProcessorTime		= Level().timeServer_Async();
	m_fTimeFactor				= fTimeFactor;
}

void game_GameState::SetGameTimeFactor	(u64 GameTime, const float fTimeFactor)
{
	m_qwStartGameTime			= GameTime;
	m_qwStartProcessorTime		= Level().timeServer_Async();
	m_fTimeFactor				= fTimeFactor;
}

u64 game_GameState::GetEnvironmentGameTime()
{
	return (m_qwEStartGameTime + u64(m_fETimeFactor*float(Level().timeServer_Async() - m_qwEStartProcessorTime)));
}

float game_GameState::GetEnvironmentGameTimeFactor()
{
	return (m_fETimeFactor);
}

void game_GameState::SetEnvironmentGameTimeFactor (const float fTimeFactor)
{
	m_qwEStartGameTime			= GetEnvironmentGameTime();
	m_qwEStartProcessorTime		= Level().timeServer_Async();
	m_fETimeFactor				= fTimeFactor;
}

void game_GameState::SendPickUpEvent(u16 ID_who, u16 ID_what)
{
	CObject* O = Level().Objects.net_Find(ID_what);
	Level().m_feel_deny.feel_touch_deny(O, 1000);

	NET_Packet P; 
	auto u_EventGen = [](NET_Packet& P, u16 type, u16 dest)
	{
		P.w_begin(M_EVENT);
		P.w_u32(Level().timeServer());
		P.w_u16(type);
		P.w_u16(dest);
	};

	u_EventGen(P, GE_OWNERSHIP_TAKE, ID_who);
	P.w_u16(ID_what);
	Level().Send(P);
}

void game_GameState::SetEnvironmentGameTimeFactor	(u64 GameTime, const float fTimeFactor)
{
	m_qwEStartGameTime			= GameTime;
	m_qwEStartProcessorTime		= Level().timeServer_Async();
	m_fETimeFactor				= fTimeFactor;
}

#include <luabind\luabind.hpp>
using namespace luabind;

GAME_API ESingleGameDifficulty g_SingleGameDifficulty = egdStalker;

xr_token	difficulty_type_token[] = 
{
	{ "gd_novice",	egdNovice },
	{ "gd_stalker",	egdStalker },
	{ "gd_veteran",	egdVeteran },
	{ "gd_master",	egdMaster },
	{ 0,			0 } 
};

#pragma optimize("gyts",on)
void CScriptGameDifficulty::script_register(lua_State *L)
{
	module(L)
		[
			class_<enum_exporter<ESingleGameDifficulty> >("game_difficulty")
			.enum_("game_difficulty")
		[
			value("novice", int(egdNovice)),
			value("stalker", int(egdStalker)),
			value("veteran", int(egdVeteran)),
			value("master", int(egdMaster))
		]
		];
}