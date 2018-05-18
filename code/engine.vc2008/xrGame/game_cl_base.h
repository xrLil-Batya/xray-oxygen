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

public:
	ClientID						local_svdpnid;

private:
				void				switch_Phase			(u32 new_phase)		{inherited::switch_Phase(new_phase);};
protected:

	virtual		void				OnSwitchPhase			(u32 old_phase, u32 new_phase);	

	virtual		shared_str			shedule_Name			() const		{ return shared_str("game_cl_GameState"); };
	virtual		float				shedule_Scale			()				{ return 1.0f;};
	virtual		bool				shedule_Needed			()				{ return true;};

				void				sv_EventSend			(NET_Packet& P);
public:
									game_cl_GameState		();
	virtual							~game_cl_GameState		();
				LPCSTR				type_name				() const {return *m_game_type_name;};
				void				set_type_name			(LPCSTR s);
	virtual		void				Init					(){};
	virtual		void				net_import_state		(NET_Packet& P);
	virtual		void				net_import_update		(NET_Packet& P);
	virtual		void				net_import_GameTime		(NET_Packet& P);						// update GameTime only for remote clients
	virtual		void				net_signal				(NET_Packet& P);

	virtual		bool				OnKeyboardPress			(int key);
	virtual		bool				OnKeyboardRelease		(int key);

				void				OnGameMessage			(NET_Packet& P);

				u32					GetPlayersCount			() const {return 1;};
	virtual		CUIGameCustom*		createGameUI			(){return nullptr;};
	virtual		void				SetGameUI				(CUIGameCustom*){};
	virtual		void				GetMapEntities			(xr_vector<SZoneMapEntityData>& dst)	{};


	virtual		void				shedule_Update			(u32 dt);

	void							u_EventGen				(NET_Packet& P, u16 type, u16 dest);
	void							u_EventSend				(NET_Packet& P);

	virtual		void				OnRender				()	{};
	virtual		bool				IsServerControlHits		()	{return m_bServerControlHits;};
	virtual		bool				IsEnemy					(CEntityAlive* ea1, CEntityAlive* ea2)	{return false;};
	virtual		bool				PlayerCanSprint			(CActor* pActor) {return true;};

	virtual		void				OnSpawn					(CObject* pObj)	{};
	virtual		void				OnDestroy				(CObject* pObj)	{};

	virtual		void				SendPickUpEvent			(u16 ID_who, u16 ID_what);

	virtual		void				OnConnected				();
public:
	bool							GetFlag(u16 id)			{ return cl_flags & id;}
	void							SetFlag(u16 id)			{ cl_flags |= id; }
	void							ResetFlag(u16 id)		{ cl_flags &= ~ id; }
};
