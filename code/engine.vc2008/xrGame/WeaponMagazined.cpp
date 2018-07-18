#include "stdafx.h"
#include "WeaponMagazined.h"
#include "actor.h"
#include "../xrParticles/psystem.h"
#include "../xrParticles/ParticlesObject.h"
#include "scope.h"
#include "silencer.h"
#include "GrenadeLauncher.h"
#include "inventory.h"
#include "InventoryOwner.h"
#include "xrserver_objects_alife_items.h"
#include "ActorEffector.h"
#include "EffectorZoomInertion.h"
#include "xr_level_controller.h"
#include "UIGame.h"
#include "object_broker.h"
#include "string_table.h"
#include "ui/UIXmlInit.h"
#include "ui/UIStatic.h"
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "CustomOutfit.h"
#include "InventoryOwner.h"
#include "../FrayBuildConfig.hpp"

ENGINE_API extern float psHUD_FOV;
ENGINE_API extern float psHUD_FOV_def;

CUIXml* pWpnScopeXml = nullptr;

void createWpnScopeXML()
{
	if(!pWpnScopeXml)
	{
		pWpnScopeXml			= new CUIXml();
		pWpnScopeXml->Load		(CONFIG_PATH, UI_PATH, "scopes.xml");
	}
}

CWeaponMagazined::CWeaponMagazined(ESoundTypes eSoundType) : CWeapon()
{
	m_eSoundShow				= ESoundTypes(SOUND_TYPE_ITEM_TAKING | eSoundType);
	m_eSoundHide				= ESoundTypes(SOUND_TYPE_ITEM_HIDING | eSoundType);
	m_eSoundShot				= ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING | eSoundType);
	m_eSoundEmptyClick			= ESoundTypes(SOUND_TYPE_WEAPON_EMPTY_CLICKING | eSoundType);
	m_eSoundReload				= ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING | eSoundType);

#ifdef NEW_ANIMS_WPN
	m_eSoundReloadEmpty			= ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING | eSoundType);
#endif	

	m_sounds_enabled			= true;
	
	m_sSndShotCurrent			= "";
	m_sSilencerFlameParticles	= m_sSilencerSmokeParticles = NULL;

	m_bFireSingleShot			= false;
	m_iShotNum					= 0;
	m_fOldBulletSpeed			= 0;
	m_iQueueSize				= WEAPON_ININITE_QUEUE;
	m_bLockType					= false;
}

void CWeaponMagazined::net_Destroy()
{
	inherited::net_Destroy();
}

void CWeaponMagazined::Load	(LPCSTR section)
{
	inherited::Load		(section);
		
	// Sounds
	m_sounds.LoadSound(section,"snd_draw", "sndShow"		        , false, m_eSoundShow		);
	m_sounds.LoadSound(section,"snd_holster", "sndHide"		        , false, m_eSoundHide		);
	m_sounds.LoadSound(section,"snd_shoot", "sndShot"		        , false, m_eSoundShot		);
	m_sounds.LoadSound(section,"snd_empty", "sndEmptyClick"	        , false, m_eSoundEmptyClick	);
	m_sounds.LoadSound(section,"snd_reload", "sndReload"		    , true, m_eSoundReload		);
	
#ifdef NEW_ANIMS_WPN
	m_sounds.LoadSound(section,"snd_reload_empty", "sndReloadEmpty"	, true, m_eSoundReloadEmpty	);
#endif
	
	m_sSndShotCurrent = "sndShot";
		
	// Загрузка звуков и партиклов глушителя, если таковой имеется
	if (m_eSilencerStatus == ALife::eAddonAttachable || m_eSilencerStatus == ALife::eAddonPermanent)
	{
		if (pSettings->line_exist(section, "silencer_flame_particles"))
			m_sSilencerFlameParticles = pSettings->r_string(section, "silencer_flame_particles");

		if (pSettings->line_exist(section, "silencer_smoke_particles"))
			m_sSilencerSmokeParticles = pSettings->r_string(section, "silencer_smoke_particles");
		
		m_sounds.LoadSound(section, "snd_silncer_shot", "sndSilencerShot", false, m_eSoundShot);
	}

	// Загрузка дисперсии
	m_iBaseDispersionedBulletsCount = READ_IF_EXISTS(pSettings, r_u8, section, "base_dispersioned_bullets_count", 0);
	m_fBaseDispersionedBulletsSpeed = READ_IF_EXISTS(pSettings, r_float, section, "base_dispersioned_bullets_speed", m_fStartBulletSpeed);

	if (pSettings->line_exist(section, "fire_modes"))
	{
		m_bHasDifferentFireModes = true;
		shared_str FireModesList = pSettings->r_string(section, "fire_modes");
		int ModesCount = _GetItemCount(FireModesList.c_str());
		m_aFireModes.clear();
		
		for (int i = 0; i < ModesCount; i++)
		{
			string16 sItem;
			_GetItem(FireModesList.c_str(), i, sItem);
			m_aFireModes.push_back((s8)atoi(sItem));
		}
		
		m_iCurFireMode = ModesCount - 1;
		m_iPrefferedFireMode = READ_IF_EXISTS(pSettings, r_s16,section,"preffered_fire_mode",-1);
	}
	else
		m_bHasDifferentFireModes = false;

	// Загрузка коэф. глушителя
	LoadSilencerKoeffs();
}

// Начало стрельбы
void CWeaponMagazined::FireStart()
{
	// Если нет осечки
	if (!IsMisfire())
	{
		if (IsValid()) 
		{
			if(!IsWorking() || AllowFireWhileWorking())
			{
				// Если оружие не готово - то не начинаем
				if (GetState() == eReload || GetState() == eShowing || GetState() == eHiding || GetState() == eMisfire) 
					return;

				inherited::FireStart();
				
				if (iAmmoElapsed == 0) 
					OnMagazineEmpty();
				else
				{
					R_ASSERT(H_Parent());
					SwitchState(eFire);
				}
			}
		}
		else 
		{
			if(eReload != GetState()) 
				OnMagazineEmpty();
		}
	}
	else
	{
		// Выводим актору статик с иконкой сломанного оружия
		if (smart_cast<CActor*>(this->H_Parent()) && (Level().CurrentViewEntity() == H_Parent()) )
			GameUI()->AddCustomStatic("gun_jammed",true);

		// Щелкаем 
		OnEmptyClick();
	}
}

void CWeaponMagazined::FireEnd() 
{
	inherited::FireEnd();

    if (g_extraFeatures.is(GAME_EXTRA_WEAPON_AUTORELOAD))
    {
        CActor *actor = smart_cast<CActor*>(H_Parent());
        if (m_pInventory && !iAmmoElapsed && actor && GetState() != eReload)
            Reload();
    }
}

void CWeaponMagazined::Reload() 
{
	inherited::Reload();
	TryReload();
}

bool CWeaponMagazined::TryReload() 
{
	if (m_pInventory) 
	{
		if (ParentIsActor())
		{
			int	AC = GetSuitableAmmoTotal();
			Actor()->callback(GameObject::eWeaponNoAmmoAvailable)(lua_game_object(), AC);
		}

		m_pCurrentAmmo = smart_cast<CWeaponAmmo*>(m_pInventory->GetAny( m_ammoTypes[m_ammoType].c_str() ));
		
		if (IsMisfire() && iAmmoElapsed)
		{
			SetPending(TRUE);
			SwitchState(eReload); 
			return true;
		}

		if (m_pCurrentAmmo || unlimited_ammo())  
		{
			SetPending(TRUE);
			SwitchState(eReload); 
			return true;
		}
		else for (u8 i = 0; i < u8(m_ammoTypes.size()); ++i) 
		{
			m_pCurrentAmmo = smart_cast<CWeaponAmmo*>(m_pInventory->GetAny( m_ammoTypes[i].c_str()));
			if (m_pCurrentAmmo) 
			{ 
				m_set_next_ammoType_on_reload = i; 
				SetPending(TRUE);
				SwitchState(eReload);
				return true;
			}
		}
	}
	
	// Меняем состояние оружия на "дефолтное"
	if (GetState() != eIdle)
		SwitchState(eIdle);

	return false;
}

bool CWeaponMagazined::IsAmmoAvailable()
{
	if (smart_cast<CWeaponAmmo*>(m_pInventory->GetAny(m_ammoTypes[m_ammoType].c_str())))
		return true;

	else for (u32 i = 0; i < m_ammoTypes.size(); ++i)
	{
		if (smart_cast<CWeaponAmmo*>(m_pInventory->GetAny(m_ammoTypes[i].c_str())))
			return true;

	}

	return false;
}

// Если магазин пустой
void CWeaponMagazined::OnMagazineEmpty()
{
	if (GetState() == eIdle)
	{
		OnEmptyClick();
		return;
	}

	if (GetNextState() != eMagEmpty && GetNextState() != eReload)
		SwitchState(eMagEmpty);

	inherited::OnMagazineEmpty();
}

// Разряжаем оружие
void CWeaponMagazined::UnloadMagazine(bool spawn_ammo)
{
	xr_map<LPCSTR, u16> l_ammo;
	
	while (!m_magazine.empty())
	{
		CCartridge &l_cartridge = m_magazine.back();
		xr_map<LPCSTR, u16>::iterator l_it;
		for (l_it = l_ammo.begin(); l_ammo.end() != l_it; ++l_it)
		{
			if (!strcmp(l_cartridge.m_ammoSect.c_str(), l_it->first))
			{
				++(l_it->second);
				break;
			}
		}

		if (l_it == l_ammo.end()) l_ammo[l_cartridge.m_ammoSect.c_str()] = 1;
		m_magazine.pop_back();
		--iAmmoElapsed;
	}

	VERIFY((u32)iAmmoElapsed == m_magazine.size());
	
	if (!spawn_ammo)
		return;

	xr_map<LPCSTR, u16>::iterator l_it;
	for (l_it = l_ammo.begin(); l_ammo.end() != l_it; ++l_it)
	{
		if (m_pInventory)
		{
			CWeaponAmmo *l_pA = smart_cast<CWeaponAmmo*>(m_pInventory->GetAny(l_it->first));
			if (l_pA)
			{
				u16 l_free = l_pA->m_boxSize - l_pA->m_boxCurr;
				l_pA->m_boxCurr = l_pA->m_boxCurr + (l_free < l_it->second ? l_free : l_it->second);
				l_it->second = l_it->second - (l_free < l_it->second ? l_free : l_it->second);
			}
		}
		if (l_it->second && !unlimited_ammo()) SpawnAmmo(l_it->second, l_it->first);
	}
}

void CWeaponMagazined::ReloadMagazine()
{
	m_BriefInfo_CalcFrame = 0;

	if (IsMisfire())	
		bMisfire = false;

	if (!m_bLockType)
	{
		m_pCurrentAmmo = NULL;
	}

	if (!m_pInventory) return;

	if (m_set_next_ammoType_on_reload != undefined_ammo_type)
	{
		m_ammoType = m_set_next_ammoType_on_reload;
		m_set_next_ammoType_on_reload = undefined_ammo_type;
	}

	if (!unlimited_ammo())
	{
		if (m_ammoTypes.size() <= m_ammoType)
			return;

		LPCSTR tmp_sect_name = m_ammoTypes[m_ammoType].c_str();

		if (!tmp_sect_name)
			return;

		m_pCurrentAmmo = smart_cast<CWeaponAmmo*>(m_pInventory->GetAny(tmp_sect_name));

		if (!m_pCurrentAmmo && !m_bLockType)
		{
			for (u8 i = 0; i < u8(m_ammoTypes.size()); ++i)
			{
				m_pCurrentAmmo = smart_cast<CWeaponAmmo*>(m_pInventory->GetAny(m_ammoTypes[i].c_str()));
				if (m_pCurrentAmmo)
				{
					m_ammoType = i;
					break;
				}
			}
		}
	}

	if (!m_pCurrentAmmo && !unlimited_ammo()) 
		return;

	if (!m_bLockType && !m_magazine.empty() && (!m_pCurrentAmmo || xr_strcmp(m_pCurrentAmmo->cNameSect(), m_magazine.back().m_ammoSect.c_str())))
		UnloadMagazine();

	if (m_DefaultCartridge.m_LocalAmmoType != m_ammoType)
		m_DefaultCartridge.Load(m_ammoTypes[m_ammoType].c_str(), m_ammoType);

	CCartridge l_cartridge = m_DefaultCartridge;
	while (iAmmoElapsed < iMagazineSize)
	{
		if (!unlimited_ammo())
			if (!m_pCurrentAmmo->Get(l_cartridge)) break;

		++iAmmoElapsed;
		l_cartridge.m_LocalAmmoType = m_ammoType;
		m_magazine.push_back(l_cartridge);
	}

	if (m_pCurrentAmmo && !m_pCurrentAmmo->m_boxCurr)
		m_pCurrentAmmo->SetDropManual(TRUE);

	if (iMagazineSize > iAmmoElapsed)
	{
		m_bLockType = true;
		ReloadMagazine();
		m_bLockType = false;
	}
}

void CWeaponMagazined::OnStateSwitch(u32 S)
{
	u32 old_state = GetState();
	inherited::OnStateSwitch(S);
	CInventoryOwner* owner = smart_cast<CInventoryOwner*>(this->H_Parent());

	switch (S)
	{
		case eIdle:
			switch2_Idle();
			break;
		case eFire:
			switch2_Fire();
			break;
		case eMisfire:
			if (smart_cast<CActor*>(this->H_Parent()) && (Level().CurrentViewEntity() == H_Parent()))
				GameUI()->AddCustomStatic("gun_jammed", true);
			break;
		case eMagEmpty:
			switch2_Empty();
			break;
		case eReload:
			if (owner)
				m_sounds_enabled = owner->CanPlayShHdRldSounds();
			switch2_Reload();
			break;
		case eShowing:
			if (owner)
				m_sounds_enabled = owner->CanPlayShHdRldSounds();
			switch2_Showing();
			break;
		case eHiding: // [fixed] quick changing of target slot restarts animation of hiding
			if (owner)
				m_sounds_enabled = owner->CanPlayShHdRldSounds();

			if (old_state != eHiding)
				switch2_Hiding();
			break;
		case eHidden:
			switch2_Hidden();
			break;
	}
}


void CWeaponMagazined::UpdateCL()
{
	inherited::UpdateCL();
	float dt = Device.fTimeDelta;

	// Когда происходит апдейт состояния оружия - ничего другого не делать
	if (GetNextState() == GetState())
	{
		switch (GetState())
		{
			case eShowing:
			case eHiding:
			case eReload:
			case eIdle:
			{
				fShotTimeCounter -=	dt;
				clamp(fShotTimeCounter, 0.0f, flt_max);
			} break;
			case eFire:
			{
				state_Fire(dt);
			} break;
			case eMisfire:
				state_Misfire(dt);
				break;
			case eMagEmpty:
				state_MagEmpty(dt);	
				break;
			case eHidden:
				break;
		}
	}

	UpdateSounds();
}

void CWeaponMagazined::UpdateSounds()
{
	if (Device.dwFrame == dwUpdateSounds_Frame)  
		return;
	
	dwUpdateSounds_Frame = Device.dwFrame;

	Fvector P = get_LastFP();
	m_sounds.SetPosition("sndShow", P);
	m_sounds.SetPosition("sndHide", P);
	m_sounds.SetPosition("sndReload", P);

#ifdef NEW_ANIMS_WPN
	m_sounds.SetPosition("sndReloadEmpty", P);
#endif
}

void CWeaponMagazined::state_Fire(float dt)
{
	if (iAmmoElapsed > 0)
	{
		VERIFY(fOneShotTime > 0.f);

		Fvector	p1, d;
		p1.set(get_LastFP());
		d.set(get_LastFD());

		if (!H_Parent())
			return;

		CInventoryOwner* io = smart_cast<CInventoryOwner*>(H_Parent());
		if (NULL == io->inventory().ActiveItem())
		{
			Log("current_state", GetState());
			Log("next_state", GetNextState());
			Log("item_sect", cNameSect().c_str());
			Log("H_Parent", H_Parent()->cNameSect().c_str());
		}

		CEntity* E = smart_cast<CEntity*>(H_Parent());
		E->g_fireParams(this, p1, d);

		if (!E->g_stateFire())
			StopShooting();

		if (m_iShotNum == 0)
		{
			m_vStartPos = p1;
			m_vStartDir = d;
		};

		VERIFY(!m_magazine.empty());

		while (!m_magazine.empty() && fShotTimeCounter < 0 && (IsWorking() || m_bFireSingleShot) && (m_iQueueSize < 0 || m_iShotNum < m_iQueueSize))
		{
			if (CheckForMisfire())
			{
				StopShooting();
				return;
			}

			m_bFireSingleShot = false;

			fShotTimeCounter += fOneShotTime;

			++m_iShotNum;

			OnShot();

			if (m_iShotNum > m_iBaseDispersionedBulletsCount)
				FireTrace(p1, d);
			else
				FireTrace(m_vStartPos, m_vStartDir);
		}

		if (m_iShotNum == m_iQueueSize)
			m_bStopedAfterQueueFired = true;

		UpdateSounds();
	}

	if (fShotTimeCounter < 0)
	{
		if (iAmmoElapsed == 0)
			OnMagazineEmpty();

		StopShooting();
	}
	else
		fShotTimeCounter -=	dt;
}

void CWeaponMagazined::state_Misfire	(float dt)
{
	OnEmptyClick();
	SwitchState(eIdle);
	
	bMisfire = true;

	UpdateSounds();
}

void CWeaponMagazined::state_MagEmpty(float dt)
{
}

void CWeaponMagazined::SetDefaults()
{
	CWeapon::SetDefaults();
}


void CWeaponMagazined::OnShot()
{
	// Проигрываем звук
	PlaySound(m_sSndShotCurrent.c_str(), get_LastFP());

	// .anm-эффект
	AddShotEffector();

	// анимацию выстрела
	PlayAnimShoot();
	
	// "выбрасываем" партикл гильз
	Fvector vel; 
	PHGetLinearVell(vel);
	OnShellDrop(get_LastSP(), vel);
	
	// огонь из ствола
	StartFlameParticles();

	// дым из ствола
	ForceUpdateFireParticles();
	StartSmokeParticles(get_LastFP(), vel);
}

void CWeaponMagazined::OnEmptyClick()
{
	PlaySound("sndEmptyClick", get_LastFP());
}

void CWeaponMagazined::OnAnimationEnd(u32 state) 
{
	switch(state) 
	{
		case eReload:	
			ReloadMagazine();	
			SwitchState(eIdle);	
			break;	// End of reload animation
		case eHiding:	
			SwitchState(eHidden);   
			break;	// End of Hide
		case eShowing:	
			SwitchState(eIdle);		
			break;	// End of Show
		case eIdle:		
			switch2_Idle();			
			break;  // Keep showing idle
	}

	inherited::OnAnimationEnd(state);
}

void CWeaponMagazined::switch2_Idle()
{
	m_iShotNum = 0;
	if (m_fOldBulletSpeed != 0.f)
		SetBulletSpeed(m_fOldBulletSpeed);

	SetPending(FALSE);
	PlayAnimIdle();
}

#ifdef DEBUG
#include "ai\stalker\ai_stalker.h"
#include "object_handler_planner.h"
#endif
void CWeaponMagazined::switch2_Fire	()
{
	CInventoryOwner* io	= smart_cast<CInventoryOwner*>(H_Parent());
	CInventoryItem* ii = smart_cast<CInventoryItem*>(this);

	if (!io)
		return;

#ifdef DEBUG
	if (ii != io->inventory().ActiveItem())
		Msg("! not an active item, item %s, owner %s, active item %s",*cName(),*H_Parent()->cName(),io->inventory().ActiveItem() ? *io->inventory().ActiveItem()->object().cName() : "no_active_item");

	if (!(io && (ii == io->inventory().ActiveItem()))) 
	{
		CAI_Stalker *stalker = smart_cast<CAI_Stalker*>(H_Parent());
		if (stalker) 
		{
			stalker->planner().show						();
			stalker->planner().show_current_world_state	();
			stalker->planner().show_target_world_state	();
		}
	}
#endif // DEBUG

	m_bStopedAfterQueueFired = false;
	m_bFireSingleShot = true;
	m_iShotNum = 0;
}

void CWeaponMagazined::switch2_Empty()
{
	OnZoomOut();
	
	if (!TryReload())
		OnEmptyClick();
	else
		inherited::FireEnd();
}

void CWeaponMagazined::PlayReloadSound()
{
	if (m_sounds_enabled)
#ifdef NEW_ANIMS_WPN
	{
		if (iAmmoElapsed == 0)
			PlaySound("sndReloadEmpty", get_LastFP());
		else
			PlaySound("sndReload", get_LastFP());
	}
#else 
		PlaySound("sndReload", get_LastFP());
#endif
}

void CWeaponMagazined::switch2_Reload()
{
	CWeapon::FireEnd();

	PlayReloadSound();
	PlayAnimReload();
	SetPending(TRUE);
}

void CWeaponMagazined::switch2_Hiding()
{
	OnZoomOut();
	CWeapon::FireEnd();
	
	if (m_sounds_enabled)
		PlaySound("sndHide", get_LastFP());

	PlayAnimHide();
	SetPending(TRUE);
}

void CWeaponMagazined::switch2_Hidden()
{
	CWeapon::FireEnd();

	StopCurrentAnimWithoutCallback();

	signal_HideComplete();
	RemoveShotEffector();
	RemoveZoomInertionEffector();
	m_nearwall_last_hud_fov = psHUD_FOV_def;
}

void CWeaponMagazined::switch2_Showing()
{
	if (m_sounds_enabled)
		PlaySound("sndShow", get_LastFP());

	SetPending(TRUE);
	PlayAnimShow();
}

bool CWeaponMagazined::Action(u16 cmd, u32 flags) 
{
	if (inherited::Action(cmd, flags)) 
		return true;
	
	// Если оружие чем-то занято или мы не "начинали", то ничего не делать
	if (IsPending() || !(flags&CMD_START))
		return false;

	switch(cmd)
	{
	    case kWPN_RELOAD:
	    {
			if (CActor* pActor = smart_cast<CActor*>(H_Parent()))
			{
				CCustomOutfit* pOutfit = pActor->GetOutfit();

				// Если у актора нет необходимой команды или ему запрещает костюм - не перезаряжаемся
				if (pActor->mstate_real & (mcSprint) && (!psActorFlags.test(AF_RELOADONSPRINT) || (pOutfit && !pOutfit->m_reload_on_sprint)))
					break;
			}

			if (iAmmoElapsed < iMagazineSize || IsMisfire())
				Reload();

			return true;
	    }

	    case kWPN_FIREMODE_PREV:
		{
			OnPrevFireMode();
			return true;
		}

	    case kWPN_FIREMODE_NEXT:
		{
			OnNextFireMode();
			return true;
		} 
	}

	return false;
}

bool CWeaponMagazined::CanAttach(PIItem pIItem)
{
	CScope*	pScope = smart_cast<CScope*>(pIItem);
	CSilencer* pSilencer = smart_cast<CSilencer*>(pIItem);
	CGrenadeLauncher* pGrenadeLauncher = smart_cast<CGrenadeLauncher*>(pIItem);

	// Прицел
	if (pScope && m_eScopeStatus == ALife::eAddonAttachable && (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope) == 0)
	{
		for (xr_string it : m_scopes)
		{
			shared_str scop_name = pSettings->r_string_wb(it.c_str(), "scope_name");
			if (scop_name == pIItem->object().cNameSect())
				return true;
		}
		return false;
	}
    // Глушитель
	else if (pSilencer && m_eSilencerStatus == ALife::eAddonAttachable && (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) == 0 && (m_sSilencerName == pIItem->object().cNameSect().c_str()))
			return true;
	// Подствольный гранатомет
	else if (pGrenadeLauncher && m_eGrenadeLauncherStatus == ALife::eAddonAttachable && (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) == 0 &&
		(m_sGrenadeLauncherName == std::string(pIItem->object().cNameSect().c_str())))
			return true;
	else
		return inherited::CanAttach(pIItem);
}

bool CWeaponMagazined::CanDetach(const char* item_section_name)
{
	// Прицел
	if (m_eScopeStatus == ALife::eAddonAttachable && 0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope))
	{
		for (xr_string it : m_scopes)
		{
			shared_str scope = pSettings->r_string_wb(it.c_str(), "scope_name");
			if (scope.equal(item_section_name))
				return true;
		}
		return false;
	}
	// Глушитель
	else if (m_eSilencerStatus == ALife::eAddonAttachable && 0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) && (m_sSilencerName == item_section_name))
        return true;
	// Подствольный гранатомет
	else if(m_eGrenadeLauncherStatus == ALife::eAddonAttachable && 0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) &&
	    (m_sGrenadeLauncherName._Equal(item_section_name)))
			return true;
	else
		return inherited::CanDetach(item_section_name);
}

bool CWeaponMagazined::Attach(PIItem pIItem, bool b_send_event)
{
	bool result = false;

	CScope*	pScope = smart_cast<CScope*>(pIItem);
	CSilencer* pSilencer = smart_cast<CSilencer*>(pIItem);
	CGrenadeLauncher* pGrenadeLauncher = smart_cast<CGrenadeLauncher*>(pIItem);
	
	// Прицел
	if (pScope && m_eScopeStatus == ALife::eAddonAttachable && (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope) == 0)
	{
		
		for (auto it = m_scopes.begin(); it != m_scopes.end(); it++)
		{
			if (pIItem->object().cNameSect().equal(pSettings->r_string_wb((*it).c_str(), "scope_name")))
				m_cur_scope = u8(it - m_scopes.begin());
		}

		m_flagsAddOnState |= CSE_ALifeItemWeapon::eWeaponAddonScope;
		result = true;
	}
	// Глушитель
	else if (pSilencer && m_eSilencerStatus == ALife::eAddonAttachable && (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) == 0 &&
		(m_sSilencerName == pIItem->object().cNameSect().c_str()))
	{
		m_flagsAddOnState |= CSE_ALifeItemWeapon::eWeaponAddonSilencer;
		result = true;
	}
	// Подствольный гранатомет
	else if (pGrenadeLauncher && m_eGrenadeLauncherStatus == ALife::eAddonAttachable && (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) == 0 &&
		(m_sGrenadeLauncherName == pIItem->object().cNameSect().c_str()))
	{
		m_flagsAddOnState |= CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher;
		result = true;
	}

	if (result)
	{
		if (b_send_event)
		{
			// Уничтожить подсоединенную вещь из инвентаря
			pIItem->object().DestroyObject	();
		};

		UpdateAddonsVisibility();
		InitAddons();

		return true;
	}
	else
        return inherited::Attach(pIItem, b_send_event);
}

bool CWeaponMagazined::DetachScope(const char* item_section_name, bool b_spawn_item)
{
	bool detached = false;
	for (xr_string it : m_scopes)
	{
		LPCSTR iter_scope_name = pSettings->r_string(it.c_str(), "scope_name");
		if (!xr_strcmp(iter_scope_name, item_section_name))
		{
			m_cur_scope = NULL;
			detached = true;
		}
	}

	return detached;
}

bool CWeaponMagazined::Detach(const char* item_section_name, bool b_spawn_item)
{
	if(		m_eScopeStatus == ALife::eAddonAttachable &&
			DetachScope(item_section_name, b_spawn_item))
	{
		if ((m_flagsAddOnState & CSE_ALifeItemWeapon::eWeaponAddonScope) == 0)
		{
			Msg("ERROR: scope addon already detached.");
			return true;
		}
		m_flagsAddOnState &= ~CSE_ALifeItemWeapon::eWeaponAddonScope;
		
		UpdateAddonsVisibility();
		InitAddons();

		return CInventoryItemObject::Detach(item_section_name, b_spawn_item);
	}
	else if(m_eSilencerStatus == ALife::eAddonAttachable &&
			(m_sSilencerName == item_section_name))
	{
		if ((m_flagsAddOnState & CSE_ALifeItemWeapon::eWeaponAddonSilencer) == 0)
		{
			Msg("ERROR: silencer addon already detached.");
			return true;
		}
		m_flagsAddOnState &= ~CSE_ALifeItemWeapon::eWeaponAddonSilencer;

		UpdateAddonsVisibility();
		InitAddons();
		return CInventoryItemObject::Detach(item_section_name, b_spawn_item);
	}
	else if(m_eGrenadeLauncherStatus == ALife::eAddonAttachable &&
			(m_sGrenadeLauncherName == item_section_name))
	{
		if ((m_flagsAddOnState & CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) == 0)
		{
			Msg("ERROR: grenade launcher addon already detached.");
			return true;
		}
		m_flagsAddOnState &= ~CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher;

		UpdateAddonsVisibility();
		InitAddons();
		return CInventoryItemObject::Detach(item_section_name, b_spawn_item);
	}
	else
		return inherited::Detach(item_section_name, b_spawn_item);;
}

void CWeaponMagazined::InitAddons()
{
	m_zoom_params.m_fIronSightZoomFactor = READ_IF_EXISTS(pSettings, r_float, cNameSect(), "ironsight_zoom_factor", 50.0f);

	if (IsScopeAttached())
	{
		if (m_eScopeStatus == ALife::eAddonAttachable)
		{
			shared_str scope_tex_name = pSettings->r_string(GetScopeName().c_str(), "scope_texture");
			m_zoom_params.m_fScopeZoomFactor = pSettings->r_float(GetScopeName().c_str(), "scope_zoom_factor");
			m_zoom_params.m_bUseDynamicZoom = READ_IF_EXISTS(pSettings, r_bool, GetScopeName().c_str(), "scope_dynamic_zoom", false);

			try
			{
				m_zoom_params.m_sUseZoomPostprocess = READ_IF_EXISTS(pSettings, r_string, GetScopeName().c_str(), "scope_nightvision", nullptr);
			}
			catch (...)
			{
				m_zoom_params.m_sUseZoomPostprocess = nullptr;
			}
			try
			{
				m_zoom_params.m_sUseBinocularVision = READ_IF_EXISTS(pSettings, r_string, GetScopeName().c_str(), "scope_alive_detector", nullptr);
			}
			catch (...)
			{
				m_zoom_params.m_sUseBinocularVision = nullptr;
			}

			xr_delete(m_UIScope);

			m_UIScope = xr_new<CUIWindow>();
			createWpnScopeXML();
			CUIXmlInit::InitWindow(*pWpnScopeXml, scope_tex_name.c_str(), 0, m_UIScope);
		}
	}
	else
	{
		xr_delete(m_UIScope);

		if (IsZoomEnabled())
			m_zoom_params.m_fIronSightZoomFactor = pSettings->r_float(cNameSect(), "scope_zoom_factor");
	}

	if (IsSilencerAttached())
	{		
		m_sFlameParticlesCurrent = m_sSilencerFlameParticles;
		m_sSmokeParticlesCurrent = m_sSilencerSmokeParticles;
		m_sSndShotCurrent = "sndSilencerShot";

		// Подсветка от выстрела
		LoadLights(*cNameSect(), "silencer_");
		ApplySilencerKoeffs();
	}
	else
	{
		m_sFlameParticlesCurrent = m_sFlameParticles;
		m_sSmokeParticlesCurrent = m_sSmokeParticles;
		m_sSndShotCurrent = "sndShot";

		// Подсветка от выстрела
		LoadLights(*cNameSect(), "");
		ResetSilencerKoeffs();
	}

	inherited::InitAddons();
}

void CWeaponMagazined::LoadSilencerKoeffs()
{
	if (m_eSilencerStatus == ALife::eAddonAttachable)
	{
		LPCSTR sect = m_sSilencerName.c_str();
		m_silencer_koef.hit_power		= READ_IF_EXISTS( pSettings, r_float, sect, "bullet_hit_power_k", 1.0f );
		m_silencer_koef.hit_impulse		= READ_IF_EXISTS( pSettings, r_float, sect, "bullet_hit_impulse_k", 1.0f );
		m_silencer_koef.bullet_speed	= READ_IF_EXISTS( pSettings, r_float, sect, "bullet_speed_k", 1.0f );
		m_silencer_koef.fire_dispersion	= READ_IF_EXISTS( pSettings, r_float, sect, "fire_dispersion_base_k", 1.0f );
		m_silencer_koef.cam_dispersion	= READ_IF_EXISTS( pSettings, r_float, sect, "cam_dispersion_k", 1.0f );
		m_silencer_koef.cam_disper_inc	= READ_IF_EXISTS( pSettings, r_float, sect, "cam_dispersion_inc_k", 1.0f );
	}

	clamp(m_silencer_koef.hit_power,		0.0f, 1.0f);
	clamp(m_silencer_koef.hit_impulse,		0.0f, 1.0f);
	clamp(m_silencer_koef.bullet_speed,	    0.0f, 1.0f);
	clamp(m_silencer_koef.fire_dispersion,	0.0f, 3.0f);
	clamp(m_silencer_koef.cam_dispersion,	0.0f, 1.0f);
	clamp(m_silencer_koef.cam_disper_inc,	0.0f, 1.0f);
}

void CWeaponMagazined::ApplySilencerKoeffs()
{
	cur_silencer_koef = m_silencer_koef;
}

void CWeaponMagazined::ResetSilencerKoeffs()
{
	cur_silencer_koef.Reset();
}

void CWeaponMagazined::PlayAnimShow()
{
	VERIFY(GetState() == eShowing);
	PlayHUDMotion("anm_show", FALSE, this, GetState());
}

void CWeaponMagazined::PlayAnimHide()
{
	VERIFY(GetState() == eHiding);
	PlayHUDMotion("anm_hide", TRUE, this, GetState());
}

void CWeaponMagazined::PlayAnimReload()
{
	VERIFY(GetState() == eReload);
	
#ifdef NEW_ANIMS_WPN
	if (iAmmoElapsed == 0)
		PlayHUDMotion("anm_reload_empty", TRUE, this, GetState());
	else
		PlayHUDMotion("anm_reload", TRUE, this, GetState());
#else 
	PlayHUDMotion("anm_reload", TRUE, this, GetState());
#endif
}

void CWeaponMagazined::PlayAnimAim()
{
	PlayHUDMotion("anm_idle_aim", TRUE, NULL, GetState());
}

void CWeaponMagazined::PlayAnimIdle()
{
	if (GetState() == eIdle) 
	{
		if (IsZoomed())
			PlayAnimAim();
		else
			inherited::PlayAnimIdle();
	}
}

void CWeaponMagazined::PlayAnimShoot()
{
	VERIFY(GetState() == eFire);
	PlayHUDMotion("anm_shots", FALSE, this, GetState());
}

void CWeaponMagazined::OnZoomIn()
{
	inherited::OnZoomIn();
	PlayAnimIdle();

	if (CActor* pActor = smart_cast<CActor*>(H_Parent()))
	{
		CEffectorZoomInertion* S = smart_cast<CEffectorZoomInertion*>(pActor->Cameras().GetCamEffector(eCEZoom));
		if (!S)
		{
			S = (CEffectorZoomInertion*)pActor->Cameras().AddCamEffector(xr_new<CEffectorZoomInertion>());
			R_ASSERT(S);
			S->Init(this);
		}

		R_ASSERT(S);
		S->SetRndSeed(pActor->GetZoomRndSeed());
		S->Enable(true);
	}
}

void CWeaponMagazined::OnZoomOut()
{
	if (!IsZoomed())	 
		return;

	inherited::OnZoomOut();
	PlayAnimIdle();

	if (CActor* pActor = smart_cast<CActor*>(H_Parent()))
	{
		CEffectorZoomInertion* S = smart_cast<CEffectorZoomInertion*>(pActor->Cameras().GetCamEffector(eCEZoom));
		if (S)
			S->Enable(false, m_zoom_params.m_fZoomRotateTime);
	}
}

void CWeaponMagazined::RemoveZoomInertionEffector()
{
	if (CActor* pActor = smart_cast<CActor*>(H_Parent()))
		pActor->Cameras().RemoveCamEffector(eCEZoom);
}

//переключение режимов стрельбы одиночными и очередями
bool CWeaponMagazined::SwitchMode()
{
	if (eIdle != GetState() || IsPending()) 
		return false;

	if (SingleShotMode())
		m_iQueueSize = WEAPON_ININITE_QUEUE;
	else
		m_iQueueSize = 1;

	PlaySound("sndEmptyClick", get_LastFP());

	return true;
}
 
void CWeaponMagazined::OnNextFireMode()
{
	if (!m_bHasDifferentFireModes || GetState() != eIdle)
		return;

	m_iCurFireMode = u32((m_iCurFireMode + 1 + m_aFireModes.size()) % m_aFireModes.size());
	SetQueueSize(GetCurrentFireMode());
}

void CWeaponMagazined::OnPrevFireMode()
{
	if (!m_bHasDifferentFireModes || GetState() != eIdle)
		return;

	m_iCurFireMode = (u32)((m_iCurFireMode - 1 + m_aFireModes.size()) % m_aFireModes.size());
	SetQueueSize(GetCurrentFireMode());
}

#include "weaponBM16.h"
void CWeaponMagazined::OnH_A_Chield()
{
	if (m_bHasDifferentFireModes)
	{
		if (smart_cast<CActor*>(H_Parent()))
			SetQueueSize(GetCurrentFireMode()); 
		else // НПС всегда переключает на автоматический режим, если он есть, или на максимальную очередь.
			SetQueueSize(smart_cast<CWeaponBM16*>(this) ? 1 : WEAPON_ININITE_QUEUE);
	};

	inherited::OnH_A_Chield();
}

void CWeaponMagazined::OnH_B_Independent(bool jbd)
{
	RemoveZoomInertionEffector();
	inherited::OnH_B_Independent(jbd);
}

void CWeaponMagazined::SetQueueSize(int size)
{
	m_iQueueSize = size;
}

float CWeaponMagazined::GetWeaponDeterioration()
{
	// modified by Peacemaker [17.10.08]
	return (m_iShotNum == 1) ? conditionDecreasePerShot : conditionDecreasePerQueueShot;
}

void CWeaponMagazined::save(NET_Packet &output_packet)
{
	inherited::save(output_packet);
	save_data(m_iQueueSize, output_packet);
	save_data(m_iShotNum, output_packet);
	save_data(m_iCurFireMode, output_packet);
}

void CWeaponMagazined::load(IReader &input_packet)
{
	inherited::load(input_packet);
	load_data(m_iQueueSize, input_packet); SetQueueSize(m_iQueueSize);
	load_data(m_iShotNum, input_packet);
	load_data(m_iCurFireMode, input_packet);
}

void CWeaponMagazined::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);

	P.w_u8(u8(m_iCurFireMode & 0x00ff));
}

#include "string_table.h"
bool CWeaponMagazined::GetBriefInfo(II_BriefInfo& info)
{
	VERIFY(m_pInventory);
	string32 int_str;

	int	ae = GetAmmoElapsed();
	xr_sprintf(int_str, "%d", ae);
	info.cur_ammo = int_str;

	if (HasFireModes())
	{
		if (m_iQueueSize == WEAPON_ININITE_QUEUE)
			info.fire_mode = "A";
		else
		{
			xr_sprintf(int_str, "%d", m_iQueueSize);
			info.fire_mode = int_str;
		}
	}
	else 
		info.fire_mode = "";

	if (m_pInventory->ModifyFrame() <= m_BriefInfo_CalcFrame)
		return false;

	GetSuitableAmmoTotal(); //update m_BriefInfo_CalcFrame
	info.grenade = "";

	size_t at_size = m_ammoTypes.size();

	// Если патроны бесконечны или их количество == 0,
	// то в счетчике ставим прочерк
	if (unlimited_ammo() || at_size == 0)
	{
		info.fmj_ammo._set("--");
		info.ap_ammo._set("--");
	}
	else
	{
		xr_sprintf(int_str, "%d", GetAmmoCount(0));

		if (m_ammoType == 0)
			info.fmj_ammo = int_str;
		else
			info.ap_ammo = int_str;

		if (at_size == 2)
		{
			xr_sprintf(int_str, "%d", GetAmmoCount(1));
			if (m_ammoType == 0)
				info.ap_ammo = int_str;
			else
				info.fmj_ammo = int_str;
		}
		else
			info.ap_ammo = "";
	}

	LPCSTR ammo_type = (ae != 0 && m_magazine.size() != 0) ? m_ammoTypes[m_magazine.back().m_LocalAmmoType].c_str() : m_ammoTypes[m_ammoType].c_str();
	info.name = CStringTable().translate(pSettings->r_string(ammo_type, "inv_name_short"));
	info.icon = ammo_type;
	return true;
}

bool CWeaponMagazined::install_upgrade_impl(LPCSTR section, bool test)
{
	bool result = inherited::install_upgrade_impl(section, test);

	LPCSTR str;

	// fire_modes = 1, 2, -1
	bool result2 = process_if_exists_set(section, "fire_modes", &CInifile::r_string, str, test);
	if (result2 && !test)
	{
		int ModesCount = _GetItemCount(str);
		m_aFireModes.clear();
		for (int i = 0; i < ModesCount; ++i)
		{
			string16 sItem;
			_GetItem(str, i, sItem);
			m_aFireModes.push_back((s8)atoi(sItem));
		}
		m_iCurFireMode = ModesCount - 1;
	}
	result |= result2;

	result |= process_if_exists_set(section, "base_dispersioned_bullets_count", &CInifile::r_s32, m_iBaseDispersionedBulletsCount, test);
	result |= process_if_exists_set(section, "base_dispersioned_bullets_speed", &CInifile::r_float, m_fBaseDispersionedBulletsSpeed, test);

	// sounds (name of the sound, volume (0.0 - 1.0), delay (sec))
	result2 = process_if_exists_set(section, "snd_draw", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_draw", "sndShow", false, m_eSoundShow); }
		result |= result2;

	result2 = process_if_exists_set(section, "snd_holster", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_holster", "sndHide", false, m_eSoundHide); }
		result |= result2;

	result2 = process_if_exists_set(section, "snd_shoot", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_shoot", "sndShot", false, m_eSoundShot); }
		result |= result2;

	result2 = process_if_exists_set(section, "snd_empty", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_empty", "sndEmptyClick", false, m_eSoundEmptyClick); }
		result |= result2;

	result2 = process_if_exists_set(section, "snd_reload", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_reload", "sndReload", true, m_eSoundReload); }
		result |= result2;

#ifdef NEW_ANIMS_WPN
	result2 = process_if_exists_set(section, "snd_reload_empty", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_reload_empty", "sndReloadEmpty", true, m_eSoundReloadEmpty); }
		result |= result2;
#endif

	if (m_eSilencerStatus == ALife::eAddonAttachable || m_eSilencerStatus == ALife::eAddonPermanent)
	{
		result |= process_if_exists_set(section, "silencer_flame_particles", &CInifile::r_string, m_sSilencerFlameParticles, test);
		result |= process_if_exists_set(section, "silencer_smoke_particles", &CInifile::r_string, m_sSilencerSmokeParticles, test);

		result2 = process_if_exists_set(section, "snd_silncer_shot", &CInifile::r_string, str, test);
		if (result2 && !test) { m_sounds.LoadSound(section, "snd_silncer_shot", "sndSilencerShot", false, m_eSoundShot); }
			result |= result2;
	}

	// fov for zoom mode
	result |= process_if_exists(section, "ironsight_zoom_factor", &CInifile::r_float, m_zoom_params.m_fIronSightZoomFactor, test);

	if (IsScopeAttached())
		result |= process_if_exists(section, "scope_zoom_factor", &CInifile::r_float, m_zoom_params.m_fScopeZoomFactor, test);
	else
		if (IsZoomEnabled())
			result |= process_if_exists(section, "scope_zoom_factor", &CInifile::r_float, m_zoom_params.m_fIronSightZoomFactor, test);

	return result;
}

float CWeaponMagazined::GetFireDispersion(float cartridge_k, bool for_crosshair) 
{
	float fire_disp = GetBaseDispersion(cartridge_k);

	if (for_crosshair || !m_iBaseDispersionedBulletsCount || !m_iShotNum || m_iShotNum > m_iBaseDispersionedBulletsCount)
		fire_disp = inherited::GetFireDispersion(cartridge_k);

	return fire_disp;
}

void CWeaponMagazined::FireBullet(const Fvector& pos, const Fvector& shot_dir, float fire_disp, const CCartridge& cartridge, u16 parent_id, u16 weapon_id, bool send_hit)
{
	if (m_iBaseDispersionedBulletsCount)
	{
		if (m_iShotNum <= 1)
		{
			m_fOldBulletSpeed = GetBulletSpeed();
			SetBulletSpeed(m_fBaseDispersionedBulletsSpeed);
		}
		else if (m_iShotNum > m_iBaseDispersionedBulletsCount)
			SetBulletSpeed(m_fOldBulletSpeed);
	}

	inherited::FireBullet(pos, shot_dir, fire_disp, cartridge, parent_id, weapon_id, send_hit);
}
