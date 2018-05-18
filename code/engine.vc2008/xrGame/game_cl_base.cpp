#include "stdafx.h"
#include "game_cl_base.h"
#include "level.h"
#include "GamePersistent.h"
#include "UIGameCustom.h"
#include "script_engine.h"
#include "xr_Level_controller.h"
#include "ui/UIMainIngameWnd.h"
#include "UI/UIGameTutorial.h"
#include "UI/UIMessagesWindow.h"
#include "UI/UIDialogWnd.h"
#include "string_table.h"

game_cl_GameState::game_cl_GameState(): cl_flags(0)
{
    m_game_type_name = "";
	shedule.t_min				= 5;
	shedule.t_max				= 20;
	m_game_ui_custom			= nullptr;
	shedule_register			();
	m_bServerControlHits		= true;
}

game_cl_GameState::~game_cl_GameState()
{
	shedule_unregister();
}

void game_cl_GameState::net_import_GameTime		(NET_Packet& P)
{
	u64				GameTime;
	P.r_u64			(GameTime);
	float			TimeFactor;
	P.r_float		(TimeFactor);

	Level().SetGameTimeFactor	(GameTime,TimeFactor);

	u64				GameEnvironmentTime;
	P.r_u64			(GameEnvironmentTime);
	float			EnvironmentTimeFactor;
	P.r_float		(EnvironmentTimeFactor);

	u64 OldTime = Level().GetEnvironmentGameTime();
	Level().SetEnvironmentGameTimeFactor	(GameEnvironmentTime,EnvironmentTimeFactor);
	if (OldTime > GameEnvironmentTime)
		GamePersistent().Environment().Invalidate();
}

void game_cl_GameState::net_import_state (NET_Packet& P)
{
	// Generic
	P.r_u32			((u32&)m_type);
	
	u16 ph;
	P.r_u16			(ph);
	
	if(Phase()!=ph)
		switch_Phase(ph);

	P.r_u32(m_start_time);
	m_bServerControlHits = !!P.r_u8();
	
	net_import_GameTime(P);
}

void	game_cl_GameState::net_import_update(NET_Packet& P)
{
	// Read
	ClientID			ID;
	P.r_clientID		(ID);

	//Syncronize GameTime
	net_import_GameTime (P);
}

void game_cl_GameState::shedule_Update		(u32 dt)
{
	ISheduled::shedule_Update	(dt);

	if(!m_game_ui_custom)
	{
		if( CurrentGameUI() )
			m_game_ui_custom = CurrentGameUI();
	} 
};

bool game_cl_GameState::OnKeyboardPress(int dik)
{
	return GetKeyState(dik) == WM_KEYUP;
}
// Fuck the logic...
bool game_cl_GameState::OnKeyboardRelease(int dik)
{
	return false;
}

void game_cl_GameState::u_EventGen(NET_Packet& P, u16 type, u16 dest)
{
	P.w_begin	(M_EVENT);
	P.w_u32		(Level().timeServer());
	P.w_u16		(type);
	P.w_u16		(dest);
}

void game_cl_GameState::u_EventSend(NET_Packet& P)
{
	Level().Send(P);
}

void game_cl_GameState::SendPickUpEvent(u16 ID_who, u16 ID_what)
{
	CObject* O		= Level().Objects.net_Find	(ID_what);
	Level().m_feel_deny.feel_touch_deny			(O, 1000);

	NET_Packet		P;
	u_EventGen		(P,GE_OWNERSHIP_TAKE, ID_who);
	P.w_u16			(ID_what);
	u_EventSend		(P);
};

void game_cl_GameState::OnConnected()
{
	m_game_ui_custom	= CurrentGameUI();
}
