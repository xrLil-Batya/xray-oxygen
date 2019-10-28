// Actor_Weapon.cpp: для работы с оружием
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "actor.h"
#include "actoreffector.h"
#include "Missile.h"
#include "inventory.h"
#include "items/Weapon.h"
#include "map_manager.h"
#include "level.h"
#include "CharacterPhysicsSupport.h"
#include "EffectorShot.h"
#include "items/WeaponMagazined.h"
#include "items/Grenade.h"
#include "game_base.h"
#include "items/Artefact.h"
#include "items/WeaponRPG7.h"
#include "items/WeaponRG6.h"
#include "items/WeaponMagazinedWGrenade.h"

static const float VEL_MAX = 10.f;
static const float VEL_A_MAX = 10.f;

#define GetWeaponParam(pWeapon, func_name, def_value)	((pWeapon) ? (pWeapon->func_name) : def_value)

//возвращает текуший разброс стрельбы (в радианах)с учетом движения
float CActor::GetWeaponAccuracy() const
{
	CWeapon* W = smart_cast<CWeapon*>(inventory().ActiveItem());

	if (IsZoomAimingMode() && W)
	{
		return m_fDispAim;
	}
	float dispersion = m_fDispBase * GetWeaponParam(W, Get_PDM_Base(), 1.0f);

	CEntity::SEntityState state;
	if (g_State(state))
	{
		//fAVelocity = angle velocity
		dispersion *= (1.0f + (state.fAVelocity / VEL_A_MAX) * m_fDispVelFactor * GetWeaponParam(W, Get_PDM_Vel_F(), 1.0f));
		//fVelocity = linear velocity
		dispersion *= (1.0f + (state.fVelocity / VEL_MAX) * m_fDispVelFactor * GetWeaponParam(W, Get_PDM_Vel_F(), 1.0f));

		bool bAccelerated = isActorAccelerated(mstate_real, IsZoomAimingMode());
		if (bAccelerated || !state.bCrouch)
		{
			dispersion *= (1.0f + m_fDispAccelFactor * GetWeaponParam(W, Get_PDM_Accel_F(), 1.0f));
		}

		if (state.bCrouch)
		{
			dispersion *= (1.0f + m_fDispCrouchFactor * GetWeaponParam(W, Get_PDM_Crouch(), 1.0f));
			if (!bAccelerated)
			{
				dispersion *= (1.0f + m_fDispCrouchNoAccelFactor * GetWeaponParam(W, Get_PDM_Crouch_NA(), 1.0f));
			}
		}
	}
	return dispersion;
}


void CActor::g_fireParams(const CHudItem* pHudItem, Fvector &fire_pos, Fvector &fire_dir)
{
	fire_dir = Cameras().Direction();
	fire_pos = Cameras().Position();
	CWeapon				*weapon = smart_cast<CWeapon*>(inventory().ActiveItem());
	const CMissile	*pMissile = smart_cast <const CMissile*> (pHudItem);
	const CMissile	*pMissileA = smart_cast <const CMissile*> (weapon);
	const CWeaponMagazinedWGrenade	*WGren = smart_cast <const CWeaponMagazinedWGrenade*> (weapon);
	const CWeaponRG6	*RG6 = smart_cast <const CWeaponRG6*> (weapon);
	const CWeaponRPG7	*RPG7 = smart_cast <const CWeaponRPG7*> (weapon);
	if (weapon)
	{
		if (eacFirstEye == cam_active && !psActorFlags.test(AF_HARDCORE) && !psActorFlags.test(AF_ZOOM_NEW_FD) || (weapon->IsZoomed() && weapon->ZoomTexture() && !weapon->IsRotatingToZoom()))
			fire_pos = Cameras().Position();
		if (!psActorFlags.test(AF_FP2ZOOM_FORCED) && (psActorFlags.test(AF_ZOOM_NEW_FD) && !(weapon->IsZoomed() && weapon->ZoomTexture() && !weapon->IsRotatingToZoom())) || (psActorFlags.test(AF_HARDCORE) && !(weapon->IsZoomed() && weapon->ZoomTexture() && !weapon->IsRotatingToZoom())))
		{
			fire_dir = weapon->get_LastFD();
			fire_pos = weapon->get_LastFP();
		}
		if (eacFirstEye == !cam_active && !psActorFlags.test(AF_HARDCORE) && !psActorFlags.test(AF_ZOOM_NEW_FD) || (weapon->IsZoomed() && weapon->ZoomTexture() && !weapon->IsRotatingToZoom()))
			fire_pos = weapon->get_LastFP();
	}
	else
	{
		if (HUDview() && pMissile)
		{
			Fvector offset;
			XFORM().transform_dir(offset, pMissile->throw_point_offset());
			fire_pos.add(offset);
		}
		if (!HUDview() && pMissileA && !RPG7 && !RG6 && !WGren && !pMissile)
		{
			fire_pos = pMissileA->XFORM().c;
		}
	}


}

void CActor::g_WeaponBones(int &L, int &R1, int &R2)
{
	R1 = m_r_hand;
	R2 = m_r_finger2;
	L = m_l_finger1;
}

BOOL CActor::g_State(SEntityState& state) const
{
	state.bJump = !!(mstate_real&mcJump);
	state.bCrouch = !!(mstate_real&mcCrouch);
	state.bFall = !!(mstate_real&mcFall);
	state.bSprint = !!(mstate_real&mcSprint);
	state.fVelocity = character_physics_support()->movement()->GetVelocityActual();
	state.fAVelocity = fCurAVelocity;
	return TRUE;
}

void CActor::SetCantRunState(bool bDisable)
{
	if (g_Alive() && this == Level().CurrentControlEntity())
	{
		NET_Packet	P;
		u_EventGen(P, GEG_PLAYER_DISABLE_SPRINT, ID());
		P.w_s8(bDisable ? 1 : -1);
		u_EventSend(P);
	};
}
void CActor::SetWeaponHideState(u16 State, bool bSet)
{
	if (g_Alive() && this == Level().CurrentControlEntity())
	{
		inventory().SetSlotsBlocked(u16(State), bSet);
	};
}

void CActor::on_weapon_shot_start(CWeapon *weapon)
{
	CameraRecoil const& camera_recoil = (IsZoomAimingMode()) ? weapon->zoom_cam_recoil : weapon->cam_recoil;

	CCameraShotEffector* effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	if (!effector)
	{
		effector = (CCameraShotEffector*)Cameras().AddCamEffector(xr_new<CCameraShotEffector>(camera_recoil));
	}
	else
	{
		if (effector->m_WeaponID != weapon->ID())
		{
			effector->Initialize(camera_recoil);
		}
	}

	effector->m_WeaponID = weapon->ID();
	R_ASSERT(effector);

	effector->SetRndSeed(GetShotRndSeed());
	effector->SetActor(this);
	effector->Shot(weapon);
}

void CActor::on_weapon_shot_update()
{
	CCameraShotEffector* effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	if (effector)
	{
		update_camera(effector);
	}
}

void CActor::on_weapon_shot_remove(CWeapon *weapon)
{
	Cameras().RemoveCamEffector(eCEShot);
}

void CActor::on_weapon_shot_stop()
{
	CCameraShotEffector *effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	if (effector && effector->IsActive())
	{
		effector->StopShoting();
	}
}

void CActor::on_weapon_hide(CWeapon *weapon)
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	if (effector && effector->IsActive())
		effector->Reset();
}

Fvector CActor::weapon_recoil_delta_angle()
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	Fvector							result = { 0.f,0.f,0.f };

	if (effector)
		effector->GetDeltaAngle(result);

	return							(result);
}

Fvector CActor::weapon_recoil_last_delta()
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	Fvector							result = { 0.f,0.f,0.f };

	if (effector)
		effector->GetLastDelta(result);

	return (result);
}
//////////////////////////////////////////////////////////////////////////

void CActor::SpawnAmmoForWeapon(CInventoryItem *pIItem)
{
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

	pWM->SpawnAmmo(0xffffffff, nullptr, ID());
}

void CActor::RemoveAmmoForWeapon(CInventoryItem *pIItem)
{
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

	CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(inventory().GetAny(pWM->m_ammoTypes[0].c_str()));
	if (pAmmo)
		pAmmo->DestroyObject();
}

#include "../xrEngine/CameraBase.h"
#include "holder_custom.h"

bool CActor::use_MountedWeapon(CHolderCustom* object)
{
	CHolderCustom* wpn = object;
	if (m_holder)
	{
		if (!wpn || (m_holder == wpn))
		{
			m_holder->detach_Actor();
			character_physics_support()->movement()->CreateCharacter();
			m_holder = nullptr;
		}
		return true;
	}
	else if (wpn)
	{
		Fvector center;	Center(center);
		if (wpn->Use(Device.vCameraPosition, Device.vCameraDirection, center))
		{
			if (wpn->attach_Actor(this))
			{
				// destroy actor character
				character_physics_support()->movement()->DestroyCharacter();
				m_holder = wpn;
				if (pCamBobbing)
				{
					Cameras().RemoveCamEffector(eCEBobbing);
					pCamBobbing = nullptr;
				}
				return true;
			}
		}
	}

	return false;
}
