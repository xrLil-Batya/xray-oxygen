#include "stdafx.h"
#include "HudItem.h"
#include "physic_item.h"
#include "actor.h"
#include "actoreffector.h"
#include "Missile.h"
#include "xrmessages.h"
#include "level.h"
#include "inventory.h"
#include "../xrEngine/CameraBase.h"
#include "player_hud.h"
#include "../xrEngine/SkeletonMotions.h"
#include "../FrayBuildConfig.hpp"
#include "../xrUICore/ui_base.h"

CHudItem::CHudItem()
{
	RenderHud(TRUE);
	EnableHudInertion(TRUE);
	AllowHudInertion(TRUE);
	m_bStopAtEndAnimIsRunning = false;
	m_current_motion_def = nullptr;
	m_started_rnd_anim_idx = u8(-1);
}

DLL_Pure *CHudItem::_construct()
{
	m_object = smart_cast<CPhysicItem*>(this);
	VERIFY(m_object);

	m_item = smart_cast<CInventoryItem*>(this);
	VERIFY(m_item);

	return				(m_object);
}

CHudItem::~CHudItem()
{
}

void CHudItem::Load(LPCSTR section)
{
	hud_sect = pSettings->r_string(section, "hud");
	m_animation_slot = pSettings->r_u32(section, "animation_slot");

	m_sounds.LoadSound(section, "snd_bore", "sndBore", true);
}

void CHudItem::PlaySound(LPCSTR alias, const Fvector& position)
{
	m_sounds.PlaySound(alias, position, object().H_Root(), !!GetHUDmode());
}

void CHudItem::renderable_Render()
{
	UpdateXForm();
	bool bHudRender = ::Render->get_HUD() && GetHUDmode();

	if (!(bHudRender && !IsHidden()))
	{
		if (!object().H_Parent() || (!bHudRender && !IsHidden()))
		{
			on_renderable_Render();
			debug_draw_firedeps();
		}
		else if (object().H_Parent())
		{
			CInventoryOwner	*owner = smart_cast<CInventoryOwner*>(object().H_Parent());
			VERIFY(owner);
			CInventoryItem	*self = smart_cast<CInventoryItem*>(this);

			if (item().BaseSlot() == INV_SLOT_3 || owner->attached(self))
			{
				on_renderable_Render();
			}
		}
	}
}

void CHudItem::SwitchState(u32 S)
{
	SetNextState(S);

	if (object().Local() && !object().getDestroy())
	{
		// !!! Just single entry for given state !!!
		NET_Packet P;
		object().u_EventGen(P, GE_WPN_STATE_CHANGE, object().ID());
		P.w_u8(u8(S));
		object().u_EventSend(P);
	}
}

void CHudItem::OnEvent(NET_Packet& P, u16 type)
{
	if (type == GE_WPN_STATE_CHANGE)
	{
		OnStateSwitch(P.r_u8(), GetState());
	}
}

void CHudItem::OnStateSwitch(u32 uState, u32 oldState)
{
	SetState(uState);

	if (object().Remote())
		SetNextState(uState);

	if (uState == eBore)
	{
		SetPending(FALSE);

		PlayAnimBore();
		if (HudItemData())
		{
			Fvector P = HudItemData()->m_item_transform.c;
			m_sounds.PlaySound("sndBore", P, object().H_Root(), !!GetHUDmode(), false, m_started_rnd_anim_idx);
		}
	}
}

void CHudItem::OnAnimationEnd(u32 uState)
{
	if (uState == eBore)
	{
		SwitchState(eIdle);
	}
}

void CHudItem::PlayAnimBore()
{
	PlayHUDMotion("anm_bore", TRUE, this, GetState());
}

bool CHudItem::ActivateItem()
{
	OnActiveItem();
	return true;
}

void CHudItem::DeactivateItem()
{
	OnHiddenItem();
}

void CHudItem::OnMoveToRuck(const SInvItemPlace& prev)
{
	SwitchState(eHidden);
}

void CHudItem::SendDeactivateItem()
{
	SendHiddenItem();
}

void CHudItem::SendHiddenItem()
{
	if (!object().getDestroy())
	{
		NET_Packet P;
		object().u_EventGen(P, GE_WPN_STATE_CHANGE, object().ID());
		P.w_u8(u8(eHiding));
		object().u_EventSend(P);
	}
}

void CHudItem::UpdateHudAdditonal(Fmatrix& hud_trans)
{
}

void CHudItem::UpdateCL()
{
	if (m_current_motion_def)
	{
		if (m_bStopAtEndAnimIsRunning)
		{
			const xr_vector<motion_marks>&	marks = m_current_motion_def->marks;
			if (!marks.empty())
			{
				float motion_prev_time = ((float)m_dwMotionCurrTm - (float)m_dwMotionStartTm) / 1000.0f;
				float motion_curr_time = ((float)Device.dwTimeGlobal - (float)m_dwMotionStartTm) / 1000.0f;

				for (const motion_marks& M: marks)
				{
					if (M.is_empty())
						continue;

					if (!M.pick_mark(motion_prev_time) && M.pick_mark(motion_curr_time))
					{
						OnMotionMark(m_startedMotionState, M);
					}
				}
			}

			m_dwMotionCurrTm = Device.dwTimeGlobal;
			if (m_dwMotionCurrTm > m_dwMotionEndTm)
			{
				m_current_motion_def = nullptr;
				m_dwMotionStartTm = 0;
				m_dwMotionEndTm = 0;
				m_dwMotionCurrTm = 0;
				m_bStopAtEndAnimIsRunning = false;
				OnAnimationEnd(m_startedMotionState);
			}
		}
	}
}

void CHudItem::OnH_A_Chield()
{}

void CHudItem::OnH_B_Chield()
{
	StopCurrentAnimWithoutCallback();
}

void CHudItem::OnH_B_Independent(bool just_before_destroy)
{
	m_sounds.StopAllSounds();
}

void CHudItem::OnH_A_Independent()
{
	if (HudItemData())
		g_player_hud->detach_item(this);
	StopCurrentAnimWithoutCallback();
}

void CHudItem::on_b_hud_detach()
{
	m_sounds.StopAllSounds();
}

void CHudItem::on_a_hud_attach()
{
	if (m_current_motion_def)
	{
		PlayHUDMotion_noCB(m_current_motion, FALSE);
	}
}

u32 CHudItem::PlayHUDMotion(const shared_str& M, BOOL bMixIn, CHudItem*  W, u32 state)
{
	u32 anim_time = PlayHUDMotion_noCB(M, bMixIn);
	if (anim_time>0)
	{
		m_bStopAtEndAnimIsRunning = true;
		m_dwMotionStartTm = Device.dwTimeGlobal;
		m_dwMotionCurrTm = m_dwMotionStartTm;
		m_dwMotionEndTm = m_dwMotionStartTm + anim_time;
		m_startedMotionState = state;
	}
	else m_bStopAtEndAnimIsRunning = false;

	return anim_time;
}


u32 CHudItem::PlayHUDMotion_noCB(const shared_str& motion_name, BOOL bMixIn)
{
	m_current_motion = motion_name;

	if (bDebug && item().m_pInventory)
	{
		Msg("-[%s] as[%d] [%d]anim_play [%s][%d]", HudItemData() ? "HUD" : "Simulating", item().m_pInventory->GetActiveSlot(),
			item().object_id(), motion_name.c_str(), Device.dwFrame);
	}
	if (HudItemData())
	{
		return HudItemData()->anim_play(motion_name, bMixIn, m_current_motion_def, m_started_rnd_anim_idx);
	}
	else
	{
		m_started_rnd_anim_idx = 0;
		return g_player_hud->motion_length(motion_name, HudSection(), m_current_motion_def);
	}
}

void CHudItem::StopCurrentAnimWithoutCallback()
{
	m_dwMotionStartTm = 0;
	m_dwMotionEndTm = 0;
	m_dwMotionCurrTm = 0;
	m_bStopAtEndAnimIsRunning = false;
	m_current_motion_def = nullptr;
}

bool CHudItem::GetHUDmode()
{
	CActor* pAct = smart_cast<CActor*>(object().H_Parent());
	return (pAct && pAct->HUDview() && HudItemData());
}

void CHudItem::PlayAnimIdle()
{
	if (TryPlayAnimIdle()) return;

	PlayHUDMotion("anm_idle", TRUE, nullptr, GetState());
}

bool CHudItem::TryPlayAnimIdle()
{
	if (MovingAnimAllowedNow())
	{
		CActor* pActor = smart_cast<CActor*>(object().H_Parent());
		if (pActor)
		{
			CEntity::SEntityState st;
			pActor->g_State(st);
			if (st.bSprint)
			{
				PlayAnimIdleSprint();
				return true;
			}
			else if (pActor->AnyMove())
			{
				PlayAnimIdleMoving();
				return true;
			}
		}
	}
	return false;
}

void CHudItem::PlayAnimIdleMoving()
{
	CActor* pActor = smart_cast<CActor*>(object().H_Parent());

	if (pActor)
	{
		CEntity::SEntityState st;
		pActor->g_State(st);

		if (st.bCrouch && AnimIsFound("anm_idle_moving_crouch"))
			PlayHUDMotion("anm_idle_moving_crouch", TRUE, NULL, GetState());
		else
			PlayHUDMotion("anm_idle_moving", TRUE, nullptr, GetState());
	}
}

void CHudItem::PlayAnimIdleSprint()
{
	PlayHUDMotion("anm_idle_sprint", TRUE, nullptr, GetState());
}

bool CHudItem::AnimIsFound(const char * AnimName) noexcept
{
	if (HudItemData())
	{
		string256 anim_name_r;
		u16 attach_place_idx = pSettings->r_u16(HudItemData()->m_sect_name, "attach_place_idx");
		xr_sprintf(anim_name_r, "%s%s", AnimName, ((attach_place_idx == 1) && UI().is_widescreen()) ? "_16x9" : "");
		player_hud_motion* anm = HudItemData()->m_hand_motions.find_motion(anim_name_r);

		if (anm) return true;
	}
	else if (g_player_hud->motion_length(AnimName, HudSection(), m_current_motion_def) > 100)
		return true;

	return false;
}

void CHudItem::OnMovementChanged(ACTOR_DEFS::EMoveCommand cmd)
{
	if (GetState() == eIdle && !m_bStopAtEndAnimIsRunning)
	{
		if ((cmd == ACTOR_DEFS::mcSprint) || (cmd == ACTOR_DEFS::mcAnyMove))
		{
			PlayAnimIdle();
			ResetSubStateTime();
		}
	}
}

attachable_hud_item* CHudItem::HudItemData()
{
	attachable_hud_item* hi = nullptr;
	if (!g_player_hud)
		return				hi;

	hi = g_player_hud->attached_item(0);
	if (hi && hi->m_parent_hud_item == this)
		return hi;

	hi = g_player_hud->attached_item(1);
	if (hi && hi->m_parent_hud_item == this)
		return hi;

	return nullptr;
}
