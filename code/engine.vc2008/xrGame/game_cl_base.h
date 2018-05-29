#pragma once
#include "game_base.h"
#include "../xrCore/client_id.h"
#include "WeaponAmmo.h"

class	NET_Packet;
class	CGameObject;
class	CUIGameCustom;
class	CUI;
class	CUIDialogWnd;

struct SZoneMapEntityData{
	Fvector	pos;
	u32		color;
	SZoneMapEntityData(){pos.set(.0f,.0f,.0f);color = 0xff00ff00;}
};

class game_cl_GameState : public game_GameState, public ISheduled
{
	typedef game_GameState	inherited;
	shared_str						m_game_type_name;
	u16								cl_flags;

protected:
	CUIGameCustom*					m_game_ui_custom;
	bool							m_bServerControlHits;

private:
				void				switch_Phase			(u32 new_phase)		{inherited::switch_Phase(new_phase);};
protected:

	virtual		shared_str			shedule_Name			() const		{ return shared_str("game_cl_GameState"); };
	virtual		float				shedule_Scale			()				{ return 1.0f;};
	virtual		bool				shedule_Needed			()				{ return true;};
public:
									game_cl_GameState		();
	virtual							~game_cl_GameState		();
				LPCSTR				type_name				() const {return *m_game_type_name;};
	virtual		void				Init					(){};
	virtual		void				net_import_state		(NET_Packet& P);
	virtual		void				net_import_update		(NET_Packet& P);
	virtual		void				net_import_GameTime		(NET_Packet& P);						// update GameTime only for remote clients

	virtual		bool				OnKeyboardPress			(int key);
	virtual		bool				OnKeyboardRelease		(int key);

	virtual		CUIGameCustom*		createGameUI			();
	virtual		void				SetGameUI				(CUIGameCustom* pUI){ m_game_ui_custom = pUI;};

	virtual		void				shedule_Update			(u32 dt);

	void							    u_EventGen				(NET_Packet& P, u16 type, u16 dest);
	void							    u_EventSend				(NET_Packet& P);

	virtual		void				OnRender				()	{};	
	virtual		void				SendPickUpEvent			(u16 ID_who, u16 ID_what);
	virtual		void				OnConnected				();
  
  // Flags
public:
	bool							GetFlag(u16 id)			{ return cl_flags & id;}
	void							SetFlag(u16 id)			{ cl_flags |= id; }
	void							ResetFlag(u16 id)		{ cl_flags &= ~ id; }
	// Single

public:
	virtual		u64					GetStartGameTime();
	virtual		u64					GetGameTime();
	virtual		float				GetGameTimeFactor();
	virtual		void				SetGameTimeFactor(const float fTimeFactor);
	virtual		void				SetGameTimeFactor(u64 GameTime, const float fTimeFactor) { inherited::SetGameTimeFactor(GameTime, fTimeFactor); }

	virtual		u64					GetEnvironmentGameTime();
	virtual		float				GetEnvironmentGameTimeFactor();
	virtual		void				SetEnvironmentGameTimeFactor(const float fTimeFactor);
	virtual		void				SetEnvironmentGameTimeFactor(u64 GameTime, const float fTimeFactor) { inherited::SetEnvironmentGameTimeFactor(GameTime, fTimeFactor); };

	void					        OnDifficultyChanged();

};

// game difficulty
enum ESingleGameDifficulty
{
	egdNovice = 0,
	egdStalker = 1,
	egdVeteran = 2,
	egdMaster = 3,
	egdCount,
	egd_force_u32 = u32(-1)
};

extern ESingleGameDifficulty g_SingleGameDifficulty;
xr_token		difficulty_type_token[];

typedef enum_exporter<ESingleGameDifficulty> CScriptGameDifficulty;
add_to_type_list(CScriptGameDifficulty)
#undef script_type_list
#define script_type_list save_type_list(CScriptGameDifficulty)