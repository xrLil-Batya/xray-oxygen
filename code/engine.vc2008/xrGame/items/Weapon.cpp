#include "stdafx.h"
#include "items/Weapon.h"
#include "items/WeaponMagazined.h"
#include "../xrParticles/psystem.h"
#include "../xrParticles/ParticlesObject.h"
#include "entity_alive.h"
#include "inventory_item_impl.h"
#include "inventory.h"
#include "xrserver_objects_alife_items.h"
#include "actor.h"
#include "actoreffector.h"
#include "level.h"
#include "..\xrEngine\xr_level_controller.h"

#include "../Include/xrRender/Kinematics.h"
#include "ai_object_location.h"
#include "../xrphysics/mathutils.h"
#include "object_broker.h"
#include "player_hud.h"
#include "gamepersistent.h"
#include "effectorFall.h"
#include "debug_renderer.h"
#include "clsid_game.h"
#include "weaponBinocularsVision.h"
#include "../xrUICore/UIWindow.h"
#include "../xrUICore/UIXmlInit.h"
#include "Torch.h"
#include "cameralook.h"
#include "items/CustomOutfit.h"
#include "HUDManager.h"
#include "../FrayBuildConfig.hpp"

ENGINE_API extern float psHUD_FOV_def;
static const int WEAPON_REMOVE_TIME = 60000;
static const float ROTATION_TIME = 0.25f;

BOOL b_toggle_weapon_aim = FALSE;
extern CUIXml*	pWpnScopeXml;

int	g_iWeaponRemove = 1;

CWeapon::CWeapon()
{
	SetState				(eHidden);
	SetNextState			(eHidden);
	m_sub_state				= eSubstateReloadBegin;
	m_bTriStateReload		= false;
	SetDefaults				();

	m_Offset.identity		();
	m_StrapOffset.identity	();

	
	m_BriefInfo_CalcFrame	= 0;

	
	bScopeHasBeenLoaded = false;

	eHandDependence			= hdNone;

	m_zoom_params.m_fCurrentZoomFactor			= g_fov;
	m_zoom_params.m_fZoomRotationFactor			= 0.f;
	m_zoom_params.m_pVision						= nullptr;
	m_zoom_params.m_pNight_vision				= nullptr;

	m_pCurrentAmmo			= nullptr;

	m_pFlameParticles2		= nullptr;
	m_sFlameParticles2		= "";
	bScopeHasTexture         = false;

	m_fCurrentCartirdgeDisp = 1.f;

	m_strap_bone0			= nullptr;
	m_strap_bone1			= nullptr;
	m_StrapOffset.identity	();
	m_strapped_mode			= false;
	m_strapped_mode_rifle		= false;
	m_can_be_strapped_rifle		= false;
	m_can_be_strapped		= false;
	m_ef_main_weapon_type	= u32(-1);
	m_ef_weapon_type		= u32(-1);
	m_UIScope				= nullptr;
	m_set_next_ammoType_on_reload = undefined_ammo_type;
	m_crosshair_inertion	= 0.f;
	m_activation_speed_is_overriden	=	false;
	m_cur_scope				= NULL;
	m_bRememberActorNVisnStatus = false;
	m_nearwall_last_hud_fov = psHUD_FOV_def;
	EnableHudInertion(TRUE);
	m_strap_bone0_id = -1;
	m_strap_bone1_id = -1;
	current_mark = 0;
	bMarkIsLoaded = false;
}

CWeapon::~CWeapon()
{
	xr_delete				(m_UIScope);
	delete_data				( m_scopes );
	delete_data				( marks	   );
}

void CWeapon::Hit(SHit* pHDS)
{
	inherited::Hit(pHDS);
}

// Обновление необходимости включения второго вьюпорта +SecondVP+
// Вызывается только для активного оружия игрока
void CWeapon::UpdateSecondVP()
{
	// + CActor::UpdateCL();
	bool b_is_active_item = (m_pInventory != nullptr) && (m_pInventory->ActiveItem() == this);
	if(!ParentIsActor() || !b_is_active_item) return; // Эта функция должна вызываться только для оружия в руках нашего игрока

	CActor* pActor = smart_cast<CActor*>(H_Parent());

	bool bCond_1 = bInZoomRightNow();							   // Мы должны целиться
	bool bCond_2 = m_zoom_params.m_fSecondVP_FovFactor > 0.0f;     // В конфиге должен быть прописан фактор зума (scope_lense_fov_factor) больше чем 0
	bool bCond_3 = pActor->cam_Active() == pActor->cam_FirstEye(); // Мы должны быть от 1-го лица
	//bool bCond_4 = m_bGrenadeMode == false;                        // Мы не должны быть в режиме подствольника

	Device.m_SecondViewport.SetSVPActive(bCond_1 && bCond_2 && bCond_3 /*&& bCond_4*/);
}

void CWeapon::UpdateXForm()
{
	if (Device.dwFrame == dwXF_Frame)
		return;

	dwXF_Frame = Device.dwFrame;

	if (!H_Parent())
		return;

	// Get access to entity and its visual
	CEntityAlive* E = smart_cast<CEntityAlive*>(H_Parent());
	
    if (!E) return;

	const CInventoryOwner *parent = smart_cast<const CInventoryOwner*>(E);
    if (parent && parent->use_simplified_visual()) return;

    if (!m_can_be_strapped_rifle && parent->attached(this)) return;

	IKinematics* V = smart_cast<IKinematics*>(E->Visual());
	VERIFY(V);

	// Get matrices
	int	boneL = -1, boneR = -1, boneR2 = -1;
	
	if ((m_strap_bone0_id == -1 || m_strap_bone1_id == -1) && m_can_be_strapped_rifle)
	{
		m_strap_bone0_id = V->LL_BoneID(m_strap_bone0);
		m_strap_bone1_id = V->LL_BoneID(m_strap_bone1);
	}

	if (parent->inventory().GetActiveSlot() != INV_SLOT_3 && m_can_be_strapped_rifle && parent->inventory().InSlot(this))
	{
		boneR = m_strap_bone0_id;
		boneR2 = m_strap_bone1_id;
		boneL = boneR;

		if (!m_strapped_mode_rifle)
			m_strapped_mode_rifle = true;
	}
	else
	{
		E->g_WeaponBones(boneL, boneR, boneR2);
		if (m_strapped_mode_rifle)
			m_strapped_mode_rifle = false;
	}

	if (boneR == (u16(-1)))
		return;

	if ((HandDependence() == hd1Hand) || (GetState() == eReload) || (!E->g_Alive()))
		boneL = boneR2;

	V->CalculateBones();

	Fmatrix& mL	= V->LL_GetTransform(u16(boneL));
	Fmatrix& mR = V->LL_GetTransform(u16(boneR));

	// Calculate
	Fmatrix	mRes;
	Fvector	R, D, N;
	D.sub(mL.c,mR.c);	

	if (fis_zero(D.magnitude()))
	{
		mRes.set(E->XFORM());
		mRes.c.set(mR.c);
	}
	else
	{
		D.normalize();
		R.crossproduct(mR.j, D);

		N.crossproduct(D, R);
		N.normalize();

		mRes.set(R, N, D, mR.c);
		mRes.mulA_43(E->XFORM());
	}

	UpdatePosition(mRes);
}

void CWeapon::UpdateFireDependencies_internal()
{
	if (Device.dwFrame != dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		UpdateXForm();

		if (GetHUDmode())
		{
			HudItemData()->setup_firedeps(m_current_firedeps);
			VERIFY(_valid(m_current_firedeps.m_FireParticlesXForm));
		} 
		else
		{
			// 3rd person or no parent
			Fmatrix& parent = XFORM();
			Fvector& fp = vLoadedFirePoint;
			Fvector& fp2 = vLoadedFirePoint2;
			Fvector& sp = vLoadedShellPoint;

			parent.transform_tiny(m_current_firedeps.vLastFP, fp);
			parent.transform_tiny(m_current_firedeps.vLastFP2, fp2);
			parent.transform_tiny(m_current_firedeps.vLastSP, sp);

			m_current_firedeps.vLastFD.set(0.f, 0.f, 1.f);
			parent.transform_dir(m_current_firedeps.vLastFD);

			m_current_firedeps.m_FireParticlesXForm.set(parent);
			VERIFY(_valid(m_current_firedeps.m_FireParticlesXForm));
		}
	}
}

void CWeapon::ForceUpdateFireParticles()
{
	if (!GetHUDmode())
	{
		//update particlesXFORM real bullet direction

		if (!H_Parent())		
			return;

		Fvector	p, d;
		smart_cast<CEntity*>(H_Parent())->g_fireParams(this, p, d);

		Fmatrix	_pxf;
		_pxf.k = d;
		_pxf.i.crossproduct(Fvector().set(0.0f, 1.0f, 0.0f), _pxf.k);
		_pxf.j.crossproduct(_pxf.k, _pxf.i);
		_pxf.c = XFORM().c;

		m_current_firedeps.m_FireParticlesXForm.set(_pxf);
	}
}

void CWeapon::Load(LPCSTR section)
{
	inherited::Load(section);
	CShootingObject::Load(section);
	
	// Загрузка партиклов выстрела
	if (pSettings->line_exist(section, "flame_particles_2"))
		m_sFlameParticles2 = pSettings->r_string(section, "flame_particles_2");

	// Загрузка классов боеприпасов
	m_ammoTypes.clear(); 
	LPCSTR S = pSettings->r_string(section,"ammo_class");
	if (S && S[0])
	{
		string128 _ammoItem;
		int				count = _GetItemCount(S);
		for (int it = 0; it < count; ++it)
		{
			_GetItem(S, it, _ammoItem);
			m_ammoTypes.emplace_back(_ammoItem);
		}
	}

	// Загрузка текущего количества патронов
	iAmmoElapsed = pSettings->r_s32(section, "ammo_elapsed");

	// Загрузка вместительности магазина
	iMagazineSize = pSettings->r_s32(section, "ammo_mag_size");
	
	////////////////////////////////////////////////////
	// Дисперсия стрельбы

	// Подбрасывание камеры во время отдачи
	u8 rm = READ_IF_EXISTS(pSettings, r_u8, section, "cam_return", 1);
	cam_recoil.ReturnMode = (rm == 1);
	
	rm = READ_IF_EXISTS(pSettings, r_u8, section, "cam_return_stop", 0);
	cam_recoil.StopReturn = (rm == 1);

	// Скорость восстановления позиции камеры
	float temp_f = 0.0f;
	temp_f = pSettings->r_float(section, "cam_relax_speed");
	cam_recoil.RelaxSpeed = _abs(deg2rad(temp_f));

	VERIFY(!fis_zero(cam_recoil.RelaxSpeed));
	if (fis_zero(cam_recoil.RelaxSpeed))
		cam_recoil.RelaxSpeed = EPS_L;

	cam_recoil.RelaxSpeed_AI = cam_recoil.RelaxSpeed;
	if (pSettings->line_exist(section, "cam_relax_speed_ai"))
	{
		temp_f = pSettings->r_float(section, "cam_relax_speed_ai");
		cam_recoil.RelaxSpeed_AI = _abs(deg2rad(temp_f));

		VERIFY(!fis_zero(cam_recoil.RelaxSpeed_AI));
		if (fis_zero(cam_recoil.RelaxSpeed_AI))
			cam_recoil.RelaxSpeed_AI = EPS_L;
	}

	temp_f = pSettings->r_float(section, "cam_max_angle");
	cam_recoil.MaxAngleVert = _abs(deg2rad(temp_f));

	VERIFY(!fis_zero(cam_recoil.MaxAngleVert));
	if (fis_zero(cam_recoil.MaxAngleVert))
		cam_recoil.MaxAngleVert = EPS;
	
	temp_f = pSettings->r_float(section, "cam_max_angle_horz");
	cam_recoil.MaxAngleHorz	= _abs(deg2rad(temp_f));

	VERIFY(!fis_zero(cam_recoil.MaxAngleHorz));
	if (fis_zero(cam_recoil.MaxAngleHorz))
		cam_recoil.MaxAngleHorz = EPS;
	
	temp_f = pSettings->r_float(section, "cam_step_angle_horz");
	cam_recoil.StepAngleHorz = deg2rad(temp_f);
	
	cam_recoil.DispersionFrac = _abs(READ_IF_EXISTS(pSettings, r_float, section, "cam_dispersion_frac", 0.7f));

	// подбрасывание камеры во время отдачи в режиме zoom ==> ironsight or scope
	// zoom_cam_recoil.Clone( cam_recoil ); ==== нельзя !!!!!!!!!!
	zoom_cam_recoil.RelaxSpeed		= cam_recoil.RelaxSpeed;
	zoom_cam_recoil.RelaxSpeed_AI	= cam_recoil.RelaxSpeed_AI;
	zoom_cam_recoil.DispersionFrac	= cam_recoil.DispersionFrac;
	zoom_cam_recoil.MaxAngleVert	= cam_recoil.MaxAngleVert;
	zoom_cam_recoil.MaxAngleHorz	= cam_recoil.MaxAngleHorz;
	zoom_cam_recoil.StepAngleHorz	= cam_recoil.StepAngleHorz;

	zoom_cam_recoil.ReturnMode		= cam_recoil.ReturnMode;
	zoom_cam_recoil.StopReturn		= cam_recoil.StopReturn;

	
	if (pSettings->line_exist(section, "zoom_cam_relax_speed"))
	{
		zoom_cam_recoil.RelaxSpeed = _abs(deg2rad(pSettings->r_float(section, "zoom_cam_relax_speed")));
		VERIFY(!fis_zero(zoom_cam_recoil.RelaxSpeed));
		if (fis_zero(zoom_cam_recoil.RelaxSpeed))
			zoom_cam_recoil.RelaxSpeed = EPS_L;
	}

	if (pSettings->line_exist(section, "zoom_cam_relax_speed_ai"))
	{
		zoom_cam_recoil.RelaxSpeed_AI = _abs(deg2rad(pSettings->r_float(section, "zoom_cam_relax_speed_ai")));
		VERIFY(!fis_zero(zoom_cam_recoil.RelaxSpeed_AI));
		if (fis_zero(zoom_cam_recoil.RelaxSpeed_AI))
			zoom_cam_recoil.RelaxSpeed_AI = EPS_L;
	}

	if (pSettings->line_exist(section, "zoom_cam_max_angle"))
	{
		zoom_cam_recoil.MaxAngleVert = _abs(deg2rad(pSettings->r_float(section, "zoom_cam_max_angle")));
		VERIFY(!fis_zero(zoom_cam_recoil.MaxAngleVert));
		if (fis_zero(zoom_cam_recoil.MaxAngleVert))
			zoom_cam_recoil.MaxAngleVert = EPS;
	}

	if (pSettings->line_exist(section, "zoom_cam_max_angle_horz"))
	{
		zoom_cam_recoil.MaxAngleHorz = _abs(deg2rad(pSettings->r_float(section, "zoom_cam_max_angle_horz")));
		VERIFY(!fis_zero(zoom_cam_recoil.MaxAngleHorz));
		if (fis_zero(zoom_cam_recoil.MaxAngleHorz))
			zoom_cam_recoil.MaxAngleHorz = EPS;
	}

	if (pSettings->line_exist(section, "zoom_cam_step_angle_horz"))
		zoom_cam_recoil.StepAngleHorz = deg2rad(pSettings->r_float(section, "zoom_cam_step_angle_horz"));

	if (pSettings->line_exist(section, "zoom_cam_dispersion_frac"))
		zoom_cam_recoil.DispersionFrac = _abs(pSettings->r_float(section, "zoom_cam_dispersion_frac"));

	m_pdm.m_fPDM_disp_base			= pSettings->r_float( section, "PDM_disp_base"			);
	m_pdm.m_fPDM_disp_vel_factor	= pSettings->r_float( section, "PDM_disp_vel_factor"	);
	m_pdm.m_fPDM_disp_accel_factor	= pSettings->r_float( section, "PDM_disp_accel_factor"	);
	m_pdm.m_fPDM_disp_crouch		= pSettings->r_float( section, "PDM_disp_crouch"		);
	m_pdm.m_fPDM_disp_crouch_no_acc	= pSettings->r_float( section, "PDM_disp_crouch_no_acc" );
	m_crosshair_inertion			= READ_IF_EXISTS(pSettings, r_float, section, "crosshair_inertion",	5.91f);
	m_first_bullet_controller.load	(section);
	fireDispersionConditionFactor = pSettings->r_float(section,"fire_dispersion_condition_factor");

	// Модификатор для HUD FOV от бедра
    m_hud_fov_add_mod = READ_IF_EXISTS(pSettings, r_float, section, "hud_fov_addition_modifier", 0.f);

	// Параметры изменения HUD FOV, когда игрок стоит вплотную к стене
	m_nearwall_dist_min = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_min", 0.5f);
	m_nearwall_dist_max = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_max", 1.f);
	m_nearwall_target_hud_fov = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_target_hud_fov", 0.27f);
	m_nearwall_speed_mod = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_speed_mod", 10.f);

	// FOV второго вьюпорта при зуме
	m_zoom_params.m_fSecondVP_FovFactor = READ_IF_EXISTS(pSettings, r_float, section, "scope_lense_fov_factor", 0.0f);

// modified by Peacemaker [17.10.08]
//	misfireProbability			  = pSettings->r_float(section,"misfire_probability"); 
//	misfireConditionK			  = READ_IF_EXISTS(pSettings, r_float, section, "misfire_condition_k",	1.0f);
	misfireStartCondition			= pSettings->r_float(section, "misfire_start_condition");
	misfireEndCondition				= READ_IF_EXISTS(pSettings, r_float, section, "misfire_end_condition", 0.f);
	misfireStartProbability			= READ_IF_EXISTS(pSettings, r_float, section, "misfire_start_prob", 0.f);
	misfireEndProbability			= pSettings->r_float(section, "misfire_end_prob");
	conditionDecreasePerShot		= pSettings->r_float(section,"condition_shot_dec"); 
	conditionDecreasePerQueueShot	= READ_IF_EXISTS(pSettings, r_float, section, "condition_queue_shot_dec", conditionDecreasePerShot); 

	vLoadedFirePoint = pSettings->r_fvector3(section, "fire_point");
	vLoadedFirePoint2 = pSettings->line_exist(section, "fire_point2") ? pSettings->r_fvector3(section, "fire_point2") : vLoadedFirePoint;

	// hands
	eHandDependence	= EHandDependence(pSettings->r_s32(section, "hand_dependence"));
	m_bIsSingleHanded = true;

	if (pSettings->line_exist(section, "single_handed"))
		m_bIsSingleHanded = !!pSettings->r_bool(section, "single_handed");

	m_fMinRadius = pSettings->r_float(section, "min_radius");
	m_fMaxRadius = pSettings->r_float(section, "max_radius");

	// информация о возможных апгрейдах и их визуализации в инвентаре
	m_eScopeStatus = (ALife::EWeaponAddonStatus)pSettings->r_s32(section, "scope_status");
	m_eSilencerStatus = (ALife::EWeaponAddonStatus)pSettings->r_s32(section, "silencer_status");
	m_eGrenadeLauncherStatus = (ALife::EWeaponAddonStatus)pSettings->r_s32(section, "grenade_launcher_status");

	m_zoom_params.m_bZoomEnabled = !!pSettings->r_bool(section, "zoom_enabled");
	m_zoom_params.m_fZoomRotateTime	= pSettings->r_float(section, "zoom_rotate_time");

	bUseAltScope = pSettings->line_exist(section, "scopes");

	if (m_eScopeStatus == ALife::eAddonAttachable)
	{
		if (bUseAltScope)
		{
			LPCSTR str = pSettings->r_string(section, "scopes");
			for (int i = 0, count = _GetItemCount(str); i < count; ++i)
			{
				string128 scope_section;
				_GetItem(str, i, scope_section);

				if (xr_strcmp(scope_section, "none") == 0)
				{
					bUseAltScope = false;
				}
				else
				{
					m_scopes.emplace_back(scope_section);
				}

			}
		}
		else
		{
			if (pSettings->line_exist(section, "scopes_sect"))
			{
				LPCSTR str = pSettings->r_string(section, "scopes_sect");
				for (int i = 0, count = _GetItemCount(str); i < count; ++i)
				{
					string128 scope_section;
					_GetItem(str, i, scope_section);
					m_scopes.emplace_back(scope_section);
				}
			}
			else
				m_scopes.emplace_back(section);
		}
	}
	else if (m_eScopeStatus == ALife::eAddonPermanent)
	{
		xr_string scope_tex_name = pSettings->r_string(cNameSect(), "scope_texture");

		if (xr_strcmp(scope_tex_name.c_str(), "none") != 0)
			bScopeHasTexture = true;
		else
			bScopeHasTexture = false;

		m_zoom_params.m_fScopeZoomFactor = pSettings->r_float( cNameSect(), "scope_zoom_factor");

		if (UseScopeTexture())
		{
			m_UIScope = xr_new<CUIWindow>();

			if (!pWpnScopeXml)
			{
				pWpnScopeXml = xr_new<CUIXml>();
				pWpnScopeXml->Load(CONFIG_PATH, UI_PATH, "scopes.xml");
			}

			CUIXmlInit::InitWindow(*pWpnScopeXml, scope_tex_name.c_str(), 0, m_UIScope);
		}
	}
    
	if (m_eSilencerStatus == ALife::eAddonAttachable)
	{
		m_sSilencerName = pSettings->r_string(section, "silencer_name");
		m_iSilencerX = pSettings->r_s32(section, "silencer_x");
		m_iSilencerY = pSettings->r_s32(section, "silencer_y");
	}

	if (m_eGrenadeLauncherStatus == ALife::eAddonAttachable)
	{
		m_sGrenadeLauncherName = pSettings->r_string(section, "grenade_launcher_name");
		m_iGrenadeLauncherX = pSettings->r_s32(section, "grenade_launcher_x");
		m_iGrenadeLauncherY = pSettings->r_s32(section, "grenade_launcher_y");
	}
	UpdateAltScope();
	InitAddons();

	m_dwWeaponRemoveTime = pSettings->line_exist(section, "weapon_remove_time") ? pSettings->r_u32(section, "weapon_remove_time") : WEAPON_REMOVE_TIME;
	m_bAutoSpawnAmmo = pSettings->line_exist(section, "auto_spawn_ammo") ? pSettings->r_bool(section, "auto_spawn_ammo") : TRUE;

	m_zoom_params.m_bHideCrosshairInZoom = true;

	if (pSettings->line_exist(hud_sect, "zoom_hide_crosshair"))
		m_zoom_params.m_bHideCrosshairInZoom = !!pSettings->r_bool(hud_sect, "zoom_hide_crosshair");	

	Fvector	def_dof;
	def_dof.set(-1, -1, -1);
	m_zoom_params.m_ZoomDof	= READ_IF_EXISTS(pSettings, r_fvector3, section, "zoom_dof", Fvector().set(-1, -1, -1));
	m_zoom_params.m_bZoomDofEnabled	= !def_dof.similar(m_zoom_params.m_ZoomDof);

	m_zoom_params.m_ReloadDof = READ_IF_EXISTS(pSettings, r_fvector4, section, "reload_dof", Fvector4().set(-1, -1, -1, -1));

	m_bHasTracers = !!READ_IF_EXISTS(pSettings, r_bool, section, "tracers", true);
	m_u8TracerColorID = READ_IF_EXISTS(pSettings, r_u8, section, "tracers_color_ID", u8(-1));

	string256 temp;
	for (int i = egdNovice; i < egdCount; ++i) 
	{
		xr_strconcat( temp, "hit_probability_", get_token_name(difficulty_type_token, i));
		m_hit_probability[i] = READ_IF_EXISTS(pSettings, r_float, section, temp, 1.f);
	}
	
	m_zoom_params.m_bUseDynamicZoom	= READ_IF_EXISTS(pSettings, r_bool, section, "scope_dynamic_zoom", FALSE);
	m_zoom_params.m_sUseZoomPostprocess	= "";
	m_zoom_params.m_sUseBinocularVision	= "";
}

void CWeapon::LoadFireParams(LPCSTR section)
{
	cam_recoil.Dispersion = deg2rad(pSettings->r_float(section, "cam_dispersion")); 
	cam_recoil.DispersionInc = 0.0f;

	if (pSettings->line_exist(section, "cam_dispersion_inc"))
		cam_recoil.DispersionInc = deg2rad(pSettings->r_float(section, "cam_dispersion_inc")); 
	
	zoom_cam_recoil.Dispersion = cam_recoil.Dispersion;
	zoom_cam_recoil.DispersionInc = cam_recoil.DispersionInc;

	if (pSettings->line_exist(section, "zoom_cam_dispersion"))
		zoom_cam_recoil.Dispersion = deg2rad(pSettings->r_float(section, "zoom_cam_dispersion")); 

	if (pSettings->line_exist(section, "zoom_cam_dispersion_inc"))
		zoom_cam_recoil.DispersionInc = deg2rad(pSettings->r_float(section, "zoom_cam_dispersion_inc")); 

	CShootingObject::LoadFireParams(section);
};

void CWeapon::UpdateAltScope()
{
	if (!bUseAltScope)
		return;

	if (m_eScopeStatus == ALife::eAddonAttachable)
	{
		shared_str sectionNeedLoad;

		if (IsScopeAttached())
		{
			if (pSettings->section_exist(GetNameWithAttachment().c_str()))
			{
				sectionNeedLoad = GetNameWithAttachment().c_str();
			}
			else
			{
				return;
			}
		}
		else
		{
			sectionNeedLoad = m_section_id.c_str();
		}

		if (!pSettings->section_exist(sectionNeedLoad))
			return;

		if (pSettings->line_exist(sectionNeedLoad, "visual"))
		{
			shared_str vis = pSettings->r_string(sectionNeedLoad, "visual");

			if (vis != cNameVisual())
			{
				cNameVisual_set(vis);
			}
		}

		if (pSettings->line_exist(sectionNeedLoad, "hud"))
		{
			shared_str new_hud = pSettings->r_string(sectionNeedLoad, "hud");

			if (new_hud != hud_sect)
			{
				hud_sect = new_hud;
			}
		}
	}
	else
	{
		return;
	}
}

bool CWeapon::bInZoomRightNow() const
{
	return m_zoom_params.m_fZoomRotationFactor > 0.05f;
}

void CWeapon::ChangeNextMark()
{
	if (marks.empty())
		return;

	if (current_mark < marks.size() - 1)
		++current_mark;
	else
		current_mark = 0;
}

void CWeapon::ChangePrevMark()
{
	if (marks.empty())
		return;

	if (current_mark > 0)
		--current_mark;
	else if (current_mark == 0 && marks.size() > 1)
		current_mark = u8 (marks.size() - 1);
}

void CWeapon::UpdateMark()
{
	bool b_is_active_item = m_pInventory && (m_pInventory->ActiveItem() == this);

	if (!b_is_active_item || !IsScopeAttached() || bScopeHasTexture)
		return;

	if (!marks.empty())
		ChangeCurrentMark(marks[current_mark].c_str());
	else
		LoadDefaultMark();
}

const xr_string CWeapon::GetScopeName() const
{
	if (IsScopeAttached())
	{
		return bUseAltScope ? m_scopes[m_cur_scope] : pSettings->r_string(m_scopes[m_cur_scope].c_str(), "scope_name");
	}
	else
	{
		return m_section_id.c_str();
	}

}

int CWeapon::GetScopeX()
{
	if (IsScopeAttached())
	{
		if (bUseAltScope)
		{
			if (m_eScopeStatus != ALife::eAddonPermanent)
			{
				return pSettings->r_s32(GetNameWithAttachment().c_str(), "scope_x");
			}
		}
		else
		{
			return pSettings->r_s32(m_scopes[m_cur_scope].c_str(), "scope_x");
		}
	}
	return 0;
}

int CWeapon::GetScopeY()
{
	if (IsScopeAttached())
	{
		if (bUseAltScope)
		{
			if (m_eScopeStatus != ALife::eAddonPermanent)
			{
				return pSettings->r_s32(GetNameWithAttachment().c_str(), "scope_y");
			}
		}
		else
		{
			return pSettings->r_s32(m_scopes[m_cur_scope].c_str(), "scope_y");
		}
	}
	return 0;
}

xr_string CWeapon::GetNameWithAttachment()
{
	string128 str;

	if(IsScopeAttached())
		xr_sprintf(str, "%s_%s", m_section_id.c_str(), GetScopeName().c_str());
	else
		xr_sprintf(str, "%s", m_section_id.c_str());

	return xr_string(str);
}

BOOL CWeapon::net_Spawn(CSE_Abstract* DC)
{
	m_fRTZoomFactor = m_zoom_params.m_fScopeZoomFactor;
	BOOL bResult = inherited::net_Spawn(DC);
	CSE_Abstract *e = (CSE_Abstract*)(DC);
	CSE_ALifeItemWeapon	*E = smart_cast<CSE_ALifeItemWeapon*>(e);

	iAmmoElapsed = E->a_elapsed;
	m_flagsAddOnState = E->m_addon_flags.get();
	m_ammoType = E->ammo_type;

	u8 scope = E->m_scope_idx;
	if (scope < m_scopes.size() && !bScopeHasBeenLoaded)
	{
		m_cur_scope = scope;
	}

	SetState(E->wpn_state);
	SetNextState(E->wpn_state);

	m_DefaultCartridge.Load(m_ammoTypes[m_ammoType].c_str(), m_ammoType);

	if (iAmmoElapsed)
	{
		m_fCurrentCartirdgeDisp = m_DefaultCartridge.param_s.kDisp;

		for (int i = 0; i < iAmmoElapsed; ++i)
			m_magazine.push_back(m_DefaultCartridge);
	}
	UpdateAltScope();
	UpdateAddonsVisibility();
	InitAddons();

	if (IsScopeAttached() && !bScopeHasTexture && !bMarkIsLoaded)
	{
		LoadDefaultMark();
	}


	m_dwWeaponIndependencyTime = 0;

	VERIFY((u32)iAmmoElapsed == m_magazine.size());
	m_bAmmoWasSpawned = false;

	return bResult;
}

void CWeapon::net_Destroy()
{
	inherited::net_Destroy();

	//удалить объекты партиклов
	StopFlameParticles();
	StopFlameParticles2();
	StopLight();
	Light_Destroy();

	// Charsi82: 29 Oct 2015
	m_magazine.clear();
	m_magazine.shrink_to_fit();
}

BOOL CWeapon::IsUpdating()
{	
	bool bIsActiveItem = m_pInventory && m_pInventory->ActiveItem()==this;
	return bIsActiveItem || bWorking;
}

void CWeapon::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);

	P.w_float_q8(GetCondition(), 0.0f, 1.0f);

	u8 need_upd = IsUpdating() ? 1 : 0;
	P.w_u8(need_upd);
	P.w_u16(u16(iAmmoElapsed));
	P.w_u8(m_flagsAddOnState);
	P.w_u8(m_ammoType);
	P.w_u8((u8)GetState());
	P.w_u8((u8)IsZoomed());
	//P.w_u8(u8(m_cur_scope));
}

void CWeapon::save(NET_Packet &output_packet)
{
	inherited::save	(output_packet);
	save_data		(iAmmoElapsed,					 output_packet);
	save_data       (m_cur_scope,                    output_packet);
	save_data		(m_flagsAddOnState, 			 output_packet);
	save_data		(m_ammoType,					 output_packet);
	save_data		(m_zoom_params.m_bIsZoomModeNow, output_packet);
	save_data		(m_bRememberActorNVisnStatus,	 output_packet);
	save_data		(current_mark,					 output_packet);
}

void CWeapon::load(IReader &input_packet)
{
	inherited::load	(input_packet);
	load_data		(iAmmoElapsed,					 input_packet);
	load_data       (m_cur_scope,                    input_packet);
	bScopeHasBeenLoaded = true;
	load_data		(m_flagsAddOnState,				 input_packet);
	load_data		(m_ammoType,					 input_packet);
	load_data		(m_zoom_params.m_bIsZoomModeNow, input_packet);

	if (m_zoom_params.m_bIsZoomModeNow)	
		OnZoomIn();
	else			
		OnZoomOut();

	load_data		(m_bRememberActorNVisnStatus,	 input_packet);

	u8 temp;

	load_data		(temp,							 input_packet);

	if (temp >= marks.size() && !marks.empty())
		current_mark = 0;
	else
		current_mark = temp;

	bMarkIsLoaded = true;
}


void CWeapon::OnEvent(NET_Packet& P, u16 type)
{
	switch (type)
	{
		case GE_ADDON_CHANGE:
		{
			P.r_u8(m_flagsAddOnState);
			InitAddons();
			UpdateAddonsVisibility();
		} break;

		case GE_WPN_STATE_CHANGE:
		{
			u8				state;
			P.r_u8(state);
			P.r_u8(m_sub_state);
			P.r_u8(); //Объясните дяде, зачем это нужно, когда можно просто не записывать в буффер?
			u8 AmmoElapsed = P.r_u8();
			u8 NextAmmo = P.r_u8();

			m_set_next_ammoType_on_reload = (NextAmmo == undefined_ammo_type) ? undefined_ammo_type : NextAmmo;
			OnStateSwitch(state, GetState());
		} break;

		default:
		{
			inherited::OnEvent(P, type);
		} break;
	}
};

void CWeapon::shedule_Update(u32 dT)
{
	inherited::shedule_Update(dT);
}

void CWeapon::OnH_B_Independent(bool just_before_destroy)
{
	RemoveShotEffector();

	inherited::OnH_B_Independent(just_before_destroy);

	FireEnd();
	SetPending(FALSE);
	SwitchState(eHidden);

	m_strapped_mode = false;
	m_strapped_mode_rifle = false;
	m_zoom_params.m_bIsZoomModeNow = false;
	UpdateXForm();
	m_nearwall_last_hud_fov = psHUD_FOV_def;
}

void CWeapon::OnH_A_Independent()
{
	m_dwWeaponIndependencyTime = Level().timeServer();
	inherited::OnH_A_Independent();
	Light_Destroy();
	UpdateAddonsVisibility();
};

void CWeapon::OnH_A_Chield()
{
	inherited::OnH_A_Chield();
	UpdateAddonsVisibility();
};

void CWeapon::OnActiveItem()
{
	// from Activate
	UpdateAddonsVisibility();
	m_BriefInfo_CalcFrame = 0;

	SwitchState(eShowing);

	inherited::OnActiveItem();
}

void CWeapon::OnHiddenItem()
{
	m_BriefInfo_CalcFrame = 0;

	SwitchState(eHiding);

	OnZoomOut();
	inherited::OnHiddenItem();

	m_set_next_ammoType_on_reload = undefined_ammo_type;
}

void CWeapon::SendHiddenItem()
{
	if (!CHudItem::object().getDestroy() && m_pInventory)
	{
		// !!! Just single entry for given state !!!
		NET_Packet		P;
		CHudItem::object().u_EventGen(P, GE_WPN_STATE_CHANGE, CHudItem::object().ID());
		P.w_u8(u8(eHiding));
		P.w_u8(u8(m_sub_state));
		P.w_u8(m_ammoType); // И вот зачем это записывать?...
		P.w_u8(u8(iAmmoElapsed & 0xff));
		P.w_u8(m_set_next_ammoType_on_reload);
		CHudItem::object().u_EventSend(P);
		SetPending(TRUE);
	}
}


void CWeapon::OnH_B_Chield()
{
	m_dwWeaponIndependencyTime = 0;
	inherited::OnH_B_Chield();

	OnZoomOut();
	m_set_next_ammoType_on_reload = undefined_ammo_type;
	m_nearwall_last_hud_fov = psHUD_FOV_def;
}

extern u32 hud_adj_mode;

// TODO: расхардкорить
bool CWeapon::AllowBore()
{
	return true;
}

void CWeapon::UpdateCL()
{
	// Время, через которое "состояние" оружия
	// изменится на "расслабленное"
	int iTimeForSwitchState = 20000;

	inherited::UpdateCL();
	UpdateHUDAddonsVisibility();

	//подсветка от выстрела
	UpdateLight();

	//нарисовать партиклы
	UpdateFlameParticles();
	UpdateFlameParticles2();

	if ((GetNextState() == GetState()) && H_Parent() == Level().CurrentEntity())
	{
		CActor* pActor = smart_cast<CActor*>(H_Parent());
		if (pActor && !pActor->AnyMove() && this == pActor->inventory().ActiveItem())
		{
			if (!hud_adj_mode && GetState() == eIdle && (Device.dwTimeGlobal - m_dw_curr_substate_time > iTimeForSwitchState) 
				&& !IsZoomed() && !g_player_hud->attached_item(1))
			{
				if (AllowBore())
					SwitchState(eBore);

				ResetSubStateTime();
			}
		}
	}

	if (m_zoom_params.m_pNight_vision && IsZoomed() && ZoomTexture() && !IsRotatingToZoom())
	{
		if (!m_zoom_params.m_pNight_vision->IsActive())
		{
			CActor *pA = smart_cast<CActor *>(H_Parent());
			R_ASSERT(pA);
			CTorch* pTorch = smart_cast<CTorch*>(pA->inventory().ItemFromSlot(TORCH_SLOT));

			if (pTorch && pTorch->GetNightVisionStatus())
			{
				m_bRememberActorNVisnStatus = pTorch->GetNightVisionStatus();
				pTorch->SwitchNightVision(false, false);
			}

			m_zoom_params.m_pNight_vision->Start(m_zoom_params.m_sUseZoomPostprocess.c_str(), pA, false);
		}

	}
	else if (m_bRememberActorNVisnStatus)
	{
		m_bRememberActorNVisnStatus = false;
		EnableActorNVisnAfterZoom();
	}

	if (m_zoom_params.m_pVision)
		m_zoom_params.m_pVision->Update();
}

void CWeapon::EnableActorNVisnAfterZoom()
{
	CActor *pA = Actor();

	if (pA)
	{
		CTorch* pTorch = smart_cast<CTorch*>(pA->inventory().ItemFromSlot(TORCH_SLOT));
		if (pTorch)
		{
			pTorch->SwitchNightVision(true, false);
			pTorch->GetNightVision()->PlaySounds(CNightVisionEffector::eIdleSound);
		}
	}
}

bool CWeapon::need_renderable()
{
	return !Device.m_SecondViewport.IsSVPFrame() && !(IsZoomed() && ZoomTexture() && !IsRotatingToZoom());
}

void CWeapon::renderable_Render()
{
	if (Device.m_SecondViewport.IsSVPFrame() && m_zoom_params.m_fZoomRotationFactor > 0.05f)
		return;

	// обновить xForm
	UpdateXForm();

	// нарисовать подсветку
	RenderLight();

	inherited::renderable_Render();
}

void CWeapon::signal_HideComplete()
{
	if (H_Parent())
		setVisible(FALSE);

	SetPending(FALSE);
}

void CWeapon::SetDefaults()
{
	SetPending(FALSE);

	m_flags.set(FUsingCondition, true);
	bMisfire = false;
	m_flagsAddOnState = 0;
	m_zoom_params.m_bIsZoomModeNow = false;
}

void CWeapon::UpdatePosition(const Fmatrix& trans)
{
	Position().set(trans.c);
	if (m_strapped_mode || m_strapped_mode_rifle)
		XFORM().mul(trans, m_StrapOffset);
	else
		XFORM().mul(trans, m_Offset);
	VERIFY(!fis_zero(DET(renderable.xform)));
}


bool CWeapon::Action(u16 cmd, u32 flags) 
{
	if (inherited::Action(cmd, flags)) 
		return true;

	switch (cmd)
	{
		case kWPN_FIRE:
		{
			//если оружие чем-то занято, то ничего не делать
			if (IsPending())
				return false;

			if (flags & CMD_START)
				FireStart();
			else
				FireEnd();

			return true;
		}

		case kWPN_NEXT:
		{
			return SwitchAmmoType(flags);
		}

		case kWPN_ZOOM:
		{
			if (IsZoomEnabled())
			{
				switch (flags)
				{
					case CMD_START:
					{
						if (!IsZoomed())
						{
							if (!IsPending())
							{
								if (GetState() != eIdle)
								{
									SwitchState(eIdle);
								}

								OnZoomIn();
							}
						}
						else if (!b_toggle_weapon_aim)
						{
							OnZoomOut();
						}

						break;
					}

					case CMD_IN:
					{
						if (!IsZoomEnabled() || !IsZoomed())
						{
							return false;
						}

						ZoomInc();
						break;
					}

					case CMD_OUT:
					{
						if (!IsZoomEnabled() || !IsZoomed())
						{
							return false;
						}

						ZoomDec();
						break;
					}

					default:
					{
						if (!b_toggle_weapon_aim && IsZoomed())
						{
							OnZoomOut();
						}
						break;
					}
				}
				return true;
			}
		}
	}
	return false;
}

bool CWeapon::SwitchAmmoType(u32 flags)
{
	if (IsPending() || !(flags & CMD_START))
		return false;

	u8 l_newType = m_ammoType;
	bool b1, b2;
	do
	{
		l_newType = u8((u32(l_newType + 1)) % m_ammoTypes.size());
		b1 = (l_newType != m_ammoType);
		b2 = !unlimited_ammo() && !m_pInventory->GetAny(m_ammoTypes[l_newType].c_str());
	} while (b1 && b2);

	if (l_newType != m_ammoType)
	{
		m_set_next_ammoType_on_reload = l_newType;
		Reload();
	}

	return true;
}

void CWeapon::LoadDefaultMark()
{
	shared_str mark = READ_IF_EXISTS(pSettings, r_string, GetScopeName().c_str(), "mark1", "wpn\\wpn_addon_scope_red_dot");

	::Render->ChangeMark(mark.c_str());
}

void CWeapon::ChangeCurrentMark(pcstr mark)
{
	::Render->ChangeMark(mark);
}

bool CWeapon::LoadMarks(pcstr section)
{
	if (!marks.empty())
		marks.clear();

	// В данный момент количество марок зашито в движок - не более 10
	if (!pSettings->line_exist(section, "mark1"))
		return false;
	xr_string Mark = pSettings->r_string(section, "mark1");
	marks.emplace_back(Mark);

	for (u32 i = 2; i <= 10; i++)
	{
		string16 it = "mark";
		xr_sprintf(it, "%s%d", it, i);

		if (pSettings->line_exist(section, it))
		{
			Mark = pSettings->r_string(section, it);
			marks.emplace_back(Mark);
		}
		else break;
	}

	return true;
}

void CWeapon::SpawnAmmo(u32 boxCurr, LPCSTR ammoSect, u32 ParentID) 
{
	if (m_ammoTypes.empty())			
		return;

	m_bAmmoWasSpawned = true;
	
	u32 l_type = 0;
	l_type %= m_ammoTypes.size();

	if (!ammoSect) 
		ammoSect = m_ammoTypes[l_type].c_str(); 
	
	++l_type; 
	l_type %= m_ammoTypes.size();

	CSE_Abstract *D	= F_entity_Create(ammoSect);

	{
		CSE_ALifeItemAmmo *l_pA		= smart_cast<CSE_ALifeItemAmmo*>(D);
		R_ASSERT					(l_pA);
		l_pA->m_boxSize				= (u16)pSettings->r_s32(ammoSect, "box_size");
		D->s_name					= ammoSect;
		D->set_name_replace			("");
		D->s_RP						= 0xff;
		D->ID						= 0xffff;

		if (ParentID == 0xffffffff)	
			D->ID_Parent			= (u16)H_Parent()->ID();
		else
			D->ID_Parent			= (u16)ParentID;

		D->ID_Phantom				= 0xffff;
		D->s_flags.assign			(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime				= 0;
		l_pA->m_tNodeID				= ai_location().level_vertex_id();

		if (boxCurr == 0xffffffff) 	
			boxCurr	= l_pA->m_boxSize;

		while (boxCurr)
		{
			l_pA->a_elapsed = (u16)(boxCurr > l_pA->m_boxSize ? l_pA->m_boxSize : boxCurr);
			NET_Packet P;
			D->Spawn_Write(P, TRUE);
			Level().Send(P);

			if (boxCurr > l_pA->m_boxSize)
				boxCurr -= l_pA->m_boxSize;
			else
				boxCurr = 0;
		}
	}

	F_entity_Destroy(D);
}

int CWeapon::GetSuitableAmmoTotal( bool use_item_to_spawn ) const
{
	if (!m_pInventory)
		return iAmmoElapsed;

	// Чтоб не делать лишних пересчетов
	if (m_pInventory->ModifyFrame() <= m_BriefInfo_CalcFrame)
		return iAmmoElapsed + m_iAmmoCurrentTotal;

	m_BriefInfo_CalcFrame = Device.dwFrame;
	m_iAmmoCurrentTotal = 0;

	for (u8 i = 0; i < u8(m_ammoTypes.size()); ++i)
	{
		m_iAmmoCurrentTotal += GetAmmoCount_forType(m_ammoTypes[i]);

		if (!use_item_to_spawn || !inventory_owner().item_to_spawn())
			continue;

		m_iAmmoCurrentTotal += inventory_owner().ammo_in_box_to_spawn();
	}

	return iAmmoElapsed + m_iAmmoCurrentTotal;
}

int CWeapon::GetAmmoCount(u8 ammo_type) const
{
	VERIFY(m_pInventory);
	R_ASSERT(ammo_type < m_ammoTypes.size());

	return GetAmmoCount_forType(m_ammoTypes[ammo_type]);
}

int CWeapon::GetAmmoCount_forType(shared_str const& ammo_type) const
{
	int res = 0;

	for (PIItem it : m_pInventory->m_belt)
	{
		CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(it);
		if (pAmmo && (pAmmo->cNameSect() == ammo_type.c_str()))
			res += pAmmo->m_boxCurr;
	}

	for (PIItem it : m_pInventory->m_ruck)
	{
		CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(it);
		if (pAmmo && (pAmmo->cNameSect() == ammo_type.c_str()))
			res += pAmmo->m_boxCurr;
	}

	return res;
}

float CWeapon::GetConditionMisfireProbability() const
{
	// modified by Peacemaker [17.10.08]

	if (GetCondition() > misfireStartCondition)
		return 0.0f;

	if (GetCondition() < misfireEndCondition)
		return misfireEndProbability;

	// condition goes from 1.f to 0.f
	// probability goes from 0.f to 1.f
	float mis = misfireStartProbability + ((misfireStartCondition - GetCondition()) * (misfireEndProbability - misfireStartProbability) /	
		((misfireStartCondition == misfireEndCondition) ? misfireStartCondition : (misfireStartCondition - misfireEndCondition)));

	clamp(mis, 0.0f, 0.99f);
	return mis;
}

BOOL CWeapon::CheckForMisfire()
{
	float rnd = ::Random.randF(0.f,1.f);
	float mp = GetConditionMisfireProbability();

	if (rnd < mp)
	{
		float cm;
		FireEnd();

		bMisfire = true;
		SwitchState(eMisfire);

		// С каждой осечкой чутка "убиваем" ствол
		if (GetCondition() >= 0.10)
		{
			cm = ::Random.randF(0.01f, 0.10f);
			SetCondition(GetCondition() - cm);
		}
		else if (GetCondition() >= 0.02 && GetCondition() < 0.10)
		{
			cm = ::Random.randF(0.01f, 0.02f);
			SetCondition(GetCondition() - cm);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CWeapon::IsMisfire() const
{	
	return bMisfire;
}

void CWeapon::Reload()
{
	OnZoomOut();
}

bool CWeapon::IsGrenadeLauncherAttached() const
{
	return (ALife::eAddonAttachable == m_eGrenadeLauncherStatus && 0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher)) || 
			ALife::eAddonPermanent == m_eGrenadeLauncherStatus;
}

bool CWeapon::IsScopeAttached() const
{
	return (ALife::eAddonAttachable == m_eScopeStatus && 0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope)) || 
			ALife::eAddonPermanent == m_eScopeStatus;
}

bool CWeapon::IsSilencerAttached() const
{
	return (ALife::eAddonAttachable == m_eSilencerStatus && 0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer)) || 
		ALife::eAddonPermanent == m_eSilencerStatus;
}

bool CWeapon::GrenadeLauncherAttachable()
{
	return (ALife::eAddonAttachable == m_eGrenadeLauncherStatus);
}

bool CWeapon::ScopeAttachable()
{
	return (ALife::eAddonAttachable == m_eScopeStatus);
}

bool CWeapon::SilencerAttachable()
{
	return (ALife::eAddonAttachable == m_eSilencerStatus);
}

xr_string wpn_scope = "wpn_scope";
xr_string wpn_silencer = "wpn_silencer";
xr_string wpn_grenade_launcher = "wpn_launcher";

// Только для актора
void CWeapon::UpdateHUDAddonsVisibility()
{
	if (!GetHUDmode())
		return;
	u16 bone_id = HudItemData()->m_model->LL_BoneID(wpn_scope.c_str());
	// Прицел
	if (ScopeAttachable() && bone_id != BI_NONE)
			HudItemData()->set_bone_visible(wpn_scope.c_str(), IsScopeAttached());

	if (m_eScopeStatus == ALife::eAddonDisabled && bone_id != BI_NONE)
		HudItemData()->set_bone_visible(wpn_scope.c_str(), FALSE, TRUE);
	else if (m_eScopeStatus == ALife::eAddonPermanent && bone_id != BI_NONE)
		HudItemData()->set_bone_visible(wpn_scope.c_str(), TRUE, TRUE);

	// Глушитель
	if (SilencerAttachable())
		HudItemData()->set_bone_visible(wpn_silencer.c_str(), IsSilencerAttached());

	if (m_eSilencerStatus == ALife::eAddonDisabled)
		HudItemData()->set_bone_visible(wpn_silencer.c_str(), FALSE, TRUE);
	else if (m_eSilencerStatus == ALife::eAddonPermanent)
		HudItemData()->set_bone_visible(wpn_silencer.c_str(), TRUE, TRUE);

	// Подствольный гранатомет
	if (GrenadeLauncherAttachable())
		HudItemData()->set_bone_visible(wpn_grenade_launcher.c_str(), IsGrenadeLauncherAttached());

	if (m_eGrenadeLauncherStatus == ALife::eAddonDisabled)
		HudItemData()->set_bone_visible(wpn_grenade_launcher.c_str(), FALSE, TRUE);
	else if (m_eGrenadeLauncherStatus == ALife::eAddonPermanent)
		HudItemData()->set_bone_visible(wpn_grenade_launcher.c_str(), TRUE, TRUE);
}

void CWeapon::UpdateAddonsVisibility()
{
	IKinematics* pWeaponVisual = smart_cast<IKinematics*>(Visual()); 
	R_ASSERT(pWeaponVisual);

	u16  bone_id;
	UpdateHUDAddonsVisibility();

	pWeaponVisual->CalculateBones_Invalidate();

	// Прицел
	bone_id = pWeaponVisual->LL_BoneID(wpn_scope.c_str());
	if (ScopeAttachable() && bone_id != BI_NONE)
	{
		if (IsScopeAttached())
		{
			if (!pWeaponVisual->LL_GetBoneVisible(bone_id))
				pWeaponVisual->LL_SetBoneVisible(bone_id, TRUE, TRUE);
		}
		else 
		{
			if (pWeaponVisual->LL_GetBoneVisible(bone_id))
				pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
		}
	}
	if (m_eScopeStatus == ALife::eAddonDisabled && bone_id != BI_NONE && pWeaponVisual->LL_GetBoneVisible(bone_id))
		pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);

	// Глушитель
	bone_id = pWeaponVisual->LL_BoneID(wpn_silencer.c_str());
	if (SilencerAttachable())
	{
		const bool isBoneVisible = pWeaponVisual->LL_GetBoneVisible(bone_id);
		if (IsSilencerAttached()) 
		{
			if (!isBoneVisible)
				pWeaponVisual->LL_SetBoneVisible(bone_id, TRUE, TRUE);
		}
		else 
		{
			if (isBoneVisible)
				pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
		}
	}

	if (m_eSilencerStatus == ALife::eAddonDisabled && bone_id != BI_NONE && pWeaponVisual->LL_GetBoneVisible(bone_id))
		pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);

	// Подствольный гранатомет
	bone_id = pWeaponVisual->LL_BoneID(wpn_grenade_launcher.c_str());
	if (GrenadeLauncherAttachable())
	{
		if (IsGrenadeLauncherAttached())
		{
			if (!pWeaponVisual->LL_GetBoneVisible(bone_id))
				pWeaponVisual->LL_SetBoneVisible(bone_id, TRUE, TRUE);
		}
		else 
		{
			if (pWeaponVisual->LL_GetBoneVisible(bone_id))
				pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
		}
	}

	if (m_eGrenadeLauncherStatus == ALife::eAddonDisabled && bone_id != BI_NONE && pWeaponVisual->LL_GetBoneVisible(bone_id))
		pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);

	pWeaponVisual->CalculateBones_Invalidate();
	pWeaponVisual->CalculateBones(TRUE);
}


void CWeapon::InitAddons() {}

float CWeapon::CurrentZoomFactor()
{
	return IsScopeAttached() ? m_zoom_params.m_fScopeZoomFactor : m_zoom_params.m_fIronSightZoomFactor;
}

void GetZoomData(const float scope_factor, float& delta, float& min_zoom_factor);

void CWeapon::OnZoomIn()
{
	m_zoom_params.m_bIsZoomModeNow = true;
	psActorFlags.set(AF_ZOOM_NEW_FD, true);
	if (m_zoom_params.m_bUseDynamicZoom)
	{
		if (!m_fRTZoomFactor)
			m_fRTZoomFactor = CurrentZoomFactor();

		SetZoomFactor(m_fRTZoomFactor);
	}
	else
	{
		m_zoom_params.m_fCurrentZoomFactor = CurrentZoomFactor();
	}

	if (m_zoom_params.m_bZoomDofEnabled && !IsScopeAttached())
		GamePersistent().SetEffectorDOF(m_zoom_params.m_ZoomDof);


		GamePersistent().SetPickableEffectorDOF(true);

	if (m_zoom_params.m_sUseBinocularVision.size() && IsScopeAttached() && m_zoom_params.m_pVision == nullptr) 
		m_zoom_params.m_pVision	= xr_new<CBinocularsVision>(m_zoom_params.m_sUseBinocularVision.c_str());
	
	// Включаем ПНВ
    if (m_zoom_params.m_sUseZoomPostprocess.size() && IsScopeAttached())
    {
		CActor *pA = smart_cast<CActor *>(H_Parent());
        if (pA && nullptr == m_zoom_params.m_pNight_vision)
            m_zoom_params.m_pNight_vision = xr_new<CNightVisionEffector>(m_zoom_params.m_sUseZoomPostprocess.c_str());
    }
}

void CWeapon::OnZoomOut()
{
	m_zoom_params.m_bIsZoomModeNow = false;
	m_fRTZoomFactor = GetZoomFactor(); //store current
	m_zoom_params.m_fCurrentZoomFactor = g_fov;
	psActorFlags.set(AF_ZOOM_NEW_FD, false);

 	GamePersistent().RestoreEffectorDOF();


		GamePersistent().SetPickableEffectorDOF(false);

	ResetSubStateTime();

	xr_delete(m_zoom_params.m_pVision);

	// Выключаем ПНВ
	if (m_zoom_params.m_pNight_vision)
	{
		m_zoom_params.m_pNight_vision->Stop(100000.0f, false);
		xr_delete(m_zoom_params.m_pNight_vision);
	}
}

CUIWindow* CWeapon::ZoomTexture()
{
	return UseScopeTexture() ? m_UIScope : nullptr;
}

void CWeapon::SwitchState(u32 S)
{
	SetNextState(S);
	if (CHudItem::object().Local() && !CHudItem::object().getDestroy() && m_pInventory)
	{
		// !!! Just single entry for given state !!!
		NET_Packet P;
		CHudItem::object().u_EventGen(P, GE_WPN_STATE_CHANGE, CHudItem::object().ID());
		P.w_u8(u8(S));
		P.w_u8(u8(m_sub_state));
		P.w_u8(m_ammoType); // Пишем, но не читаем, класс
		P.w_u8(u8(iAmmoElapsed & 0xff));
		P.w_u8(m_set_next_ammoType_on_reload);
		CHudItem::object().u_EventSend(P);
	}
}

void CWeapon::OnMagazineEmpty()
{
	VERIFY(iAmmoElapsed == m_magazine.size());
}

void CWeapon::reinit()
{
	CShootingObject::reinit();
	CHudItemObject::reinit();
}

void CWeapon::reload(LPCSTR section)
{
	CShootingObject::reload(section);
	CHudItemObject::reload(section);
	
	m_can_be_strapped = true;
	m_can_be_strapped_rifle	= (BaseSlot() == INV_SLOT_3);		   
	m_strapped_mode	= false;
	m_strapped_mode_rifle = false;
	
	if (pSettings->line_exist(section, "strap_bone0"))
		m_strap_bone0 = pSettings->r_string(section, "strap_bone0");
	else
	{
		m_can_be_strapped = false;
		m_can_be_strapped_rifle	= false;
	}
	
	if (pSettings->line_exist(section,"strap_bone1"))
		m_strap_bone1 = pSettings->r_string(section,"strap_bone1");
	else
	{
		m_can_be_strapped = false;
		m_can_be_strapped_rifle = false;
	}

	if (m_eScopeStatus == ALife::eAddonAttachable) 
	{
		m_addon_holder_range_modifier = READ_IF_EXISTS(pSettings, r_float, GetScopeName().c_str(), "holder_range_modifier", m_holder_range_modifier);
		m_addon_holder_fov_modifier = READ_IF_EXISTS(pSettings, r_float, GetScopeName().c_str(), "holder_fov_modifier", m_holder_fov_modifier);
	}
	else 
	{
		m_addon_holder_range_modifier = m_holder_range_modifier;
		m_addon_holder_fov_modifier = m_holder_fov_modifier;
	}

	Fvector pos, ypr;
	pos	= pSettings->r_fvector3(section, "position");
	ypr	= pSettings->r_fvector3(section, "orientation");
	ypr.mul(PI / 180.f);

	m_Offset.setHPB(ypr.x, ypr.y, ypr.z);
	m_Offset.translate_over(pos);

	m_StrapOffset = m_Offset;
	if (pSettings->line_exist(section, "strap_position") && pSettings->line_exist(section, "strap_orientation")) 
	{
		Fvector	pos, ypr;
		pos = pSettings->r_fvector3(section, "strap_position");
		ypr = pSettings->r_fvector3(section, "strap_orientation");
		ypr.mul(PI / 180.f);

		m_StrapOffset.setHPB(ypr.x, ypr.y, ypr.z);
		m_StrapOffset.translate_over(pos);
	}
	else
	{
		m_can_be_strapped = false;
		m_can_be_strapped_rifle = false;
	}

	m_ef_main_weapon_type = READ_IF_EXISTS(pSettings, r_u32, section, "ef_main_weapon_type", u32(-1));
	m_ef_weapon_type = READ_IF_EXISTS(pSettings, r_u32, section, "ef_weapon_type", u32(-1));
}

void CWeapon::create_physic_shell()
{
	CPhysicsShellHolder::create_physic_shell();
}

bool CWeapon::ActivationSpeedOverriden(Fvector& dest, bool clear_override)
{
	if (m_activation_speed_is_overriden)
	{
		if (clear_override)
			m_activation_speed_is_overriden	= false;

		dest = m_overriden_activation_speed;
		return true;
	}
	
	return false;
}

void CWeapon::SetActivationSpeedOverride(Fvector const& speed)
{
	m_overriden_activation_speed = speed;
	m_activation_speed_is_overriden = true;
}

void CWeapon::activate_physic_shell()
{
	UpdateXForm();
	CPhysicsShellHolder::activate_physic_shell();
}

void CWeapon::setup_physic_shell()
{
	CPhysicsShellHolder::setup_physic_shell();
}

bool CWeapon::NeedToDestroyObject()	const
{
	return false;
}

ALife::_TIME_ID	 CWeapon::TimePassedAfterIndependant()	const
{
	if(!H_Parent() && m_dwWeaponIndependencyTime != 0)
		return Level().timeServer() - m_dwWeaponIndependencyTime;
	else
		return 0;
}

bool CWeapon::can_kill() const
{
	if (GetSuitableAmmoTotal(true) || m_ammoTypes.empty())
		return(true);

	return (false);
}

CInventoryItem *CWeapon::can_kill(CInventory *inventory) const
{
	if (GetAmmoElapsed() || m_ammoTypes.empty())
		return(const_cast<CWeapon*>(this));

	TIItemContainer::iterator I = inventory->m_all.begin();
	TIItemContainer::iterator E = inventory->m_all.end();
	for (; I != E; ++I) 
	{
		CInventoryItem	*inventory_item = smart_cast<CInventoryItem*>(*I);
		if (!inventory_item)
			continue;

		xr_vector<shared_str>::const_iterator	i = std::find(m_ammoTypes.begin(), m_ammoTypes.end(), inventory_item->object().cNameSect().c_str());
		if (i != m_ammoTypes.end())
			return			(inventory_item);
	}

	return(nullptr);
}

const CInventoryItem *CWeapon::can_kill(const xr_vector<const CGameObject*> &items) const
{
	if (m_ammoTypes.empty())
		return(this);

	xr_vector<const CGameObject*>::const_iterator I = items.begin();
	xr_vector<const CGameObject*>::const_iterator E = items.end();
	for (; I != E; ++I) 
	{
		const CInventoryItem	*inventory_item = smart_cast<const CInventoryItem*>(*I);
		if (!inventory_item)
			continue;

		xr_vector<shared_str>::const_iterator i = std::find(m_ammoTypes.begin(), m_ammoTypes.end(), inventory_item->object().cNameSect().c_str());
		if (i != m_ammoTypes.end())
			return(inventory_item);
	}

	return (nullptr);
}

bool CWeapon::ready_to_kill() const
{
	return (!IsMisfire() && ((GetState() == eIdle) || (GetState() == eFire) || (GetState() == eFire2)) && GetAmmoElapsed());
}


void CWeapon::UpdateHudAdditonal(Fmatrix& trans)
{
	CActor* pActor = smart_cast<CActor*>(H_Parent());

	if (!pActor)
		return;

	if ((IsZoomed() && m_zoom_params.m_fZoomRotationFactor <= 1.f) || (!IsZoomed() && m_zoom_params.m_fZoomRotationFactor > 0.f))
	{
		attachable_hud_item* hi = HudItemData();
		R_ASSERT(hi);

		u8 idx = GetCurrentHudOffsetIdx();

		Fvector curr_offs, curr_rot;

		curr_offs = hi->m_measures.m_hands_offset[0][idx]; //pos, aim
		curr_rot = hi->m_measures.m_hands_offset[1][idx]; //rot, aim

		curr_offs.mul(m_zoom_params.m_fZoomRotationFactor);
		curr_rot.mul(m_zoom_params.m_fZoomRotationFactor);

		Fmatrix	hud_rotation;
		hud_rotation.identity();
		hud_rotation.rotateX(curr_rot.x);

		Fmatrix	hud_rotation_y;
		hud_rotation_y.identity();
		hud_rotation_y.rotateY(curr_rot.y);
		hud_rotation.mulA_43(hud_rotation_y);

		hud_rotation_y.identity();
		hud_rotation_y.rotateZ(curr_rot.z);
		hud_rotation.mulA_43(hud_rotation_y);

		hud_rotation.translate_over(curr_offs);
		trans.mulB_43(hud_rotation);

		if (pActor->IsZoomAimingMode())
			m_zoom_params.m_fZoomRotationFactor += Device.fTimeDelta / m_zoom_params.m_fZoomRotateTime;
		else
			m_zoom_params.m_fZoomRotationFactor -= Device.fTimeDelta / m_zoom_params.m_fZoomRotateTime;

		clamp(m_zoom_params.m_fZoomRotationFactor, 0.f, 1.f);
	}
}

void CWeapon::SetAmmoElapsed(int ammo_count)
{
	iAmmoElapsed = ammo_count;

	u32 uAmmo = u32(iAmmoElapsed);

	if (uAmmo != m_magazine.size())
	{
		if (uAmmo > m_magazine.size())
		{
			CCartridge l_cartridge; 
			l_cartridge.Load(m_ammoTypes[m_ammoType].c_str(), m_ammoType);
			while (uAmmo > m_magazine.size())
				m_magazine.push_back(l_cartridge);
		}
		else
		{
			while (uAmmo < m_magazine.size())
				m_magazine.pop_back();
		};
	};
}

u32	CWeapon::ef_main_weapon_type() const
{
	VERIFY(m_ef_main_weapon_type != u32(-1));
	return(m_ef_main_weapon_type);
}

u32	CWeapon::ef_weapon_type() const
{
	VERIFY(m_ef_weapon_type != u32(-1));
	return(m_ef_weapon_type);
}

bool CWeapon::IsNecessaryItem(const shared_str& item_sect)
{
	return (std::find(m_ammoTypes.begin(), m_ammoTypes.end(), item_sect) != m_ammoTypes.end());
}

void CWeapon::modify_holder_params(float &range, float &fov) const
{
	if (!IsScopeAttached()) 
	{
		inherited::modify_holder_params(range, fov);
		return;
	}

	range *= m_addon_holder_range_modifier;
	fov	*= m_addon_holder_fov_modifier;
}

bool CWeapon::render_item_ui_query()
{
	bool b_is_active_item = (m_pInventory->ActiveItem() == this);
	bool res = b_is_active_item && IsZoomed() && ZoomHideCrosshair() && ZoomTexture() && !IsRotatingToZoom();
	return res;
}

void CWeapon::render_item_ui()
{
	if (m_zoom_params.m_pVision)
		m_zoom_params.m_pVision->Draw();

	ZoomTexture()->Update();
	ZoomTexture()->Draw();
}

bool CWeapon::unlimited_ammo()
{
	if (m_pInventory)
		return inventory_owner().unlimited_ammo() && m_DefaultCartridge.m_flags.test(CCartridge::cfCanBeUnlimited);
	else
		return false;
};

float CWeapon::Weight() const
{
	float res = CInventoryItemObject::Weight();

	if (IsGrenadeLauncherAttached() && !GetGrenadeLauncherName().empty())
		res += pSettings->r_float(GetGrenadeLauncherName().c_str(), "inv_weight");

	if (IsScopeAttached() && !m_scopes.empty())
		res += pSettings->r_float(GetScopeName().c_str(), "inv_weight");

	if (IsSilencerAttached() && !GetSilencerName().empty())
		res += pSettings->r_float(GetSilencerName().c_str(), "inv_weight");

	res += GetMagazineWeight(m_magazine);

	return res;
}

// Требуется-ли отрисовывать перекрестие
bool CWeapon::show_crosshair()
{
	if (hud_adj_mode)
		return true;

	return !IsPending() && (!IsZoomed() || !ZoomHideCrosshair());
}

// Требуется-ли отображать интерфейс игрока
bool CWeapon::show_indicators()
{
	return !(IsZoomed() && ZoomTexture());
}

float CWeapon::GetConditionToShow() const
{
	return	(GetCondition());
}

BOOL CWeapon::ParentMayHaveAimBullet()
{
	CObject* O = H_Parent();
	CEntityAlive* EA = smart_cast<CEntityAlive*>(O);
	return EA->cast_actor() != nullptr;
}

BOOL CWeapon::ParentIsActor()
{
	CObject* O = H_Parent();
	CEntityAlive* EA = smart_cast<CEntityAlive*>(O);

	if (!O || !EA)
		return FALSE;

	return EA->cast_actor()!=nullptr;
}

extern u32 hud_adj_mode;

void CWeapon::debug_draw_firedeps()
{
	if (hud_adj_mode == 5 || hud_adj_mode == 6 || hud_adj_mode == 7)
	{
		CDebugRenderer &render = Level().debug_renderer();

		if (hud_adj_mode == 5)
			render.draw_aabb(get_LastFP(), 0.005f,0.005f,0.005f,D3DCOLOR_XRGB(255, 0, 0));

		if (hud_adj_mode == 6)
			render.draw_aabb(get_LastFP2(), 0.005f,0.005f,0.005f,D3DCOLOR_XRGB(0, 0, 255));

		if (hud_adj_mode == 7)
			render.draw_aabb(get_LastSP(), 0.005f,0.005f,0.005f,D3DCOLOR_XRGB(0, 255, 0));
	}
}

const float &CWeapon::hit_probability() const
{
	VERIFY((g_SingleGameDifficulty >= egdNovice) && (g_SingleGameDifficulty <= egdMaster)); 
	return (m_hit_probability[g_SingleGameDifficulty]);
}

void CWeapon::OnStateSwitch(u32 S, u32 oldState)
{
	inherited::OnStateSwitch(S, oldState);
	m_BriefInfo_CalcFrame = 0;

	if (GetState() == eReload)
	{
		if (H_Parent() == Level().CurrentEntity() && !fsimilar(m_zoom_params.m_ReloadDof.w, -1.0f))
		{
			if (CActor* pActor = smart_cast<CActor*>(H_Parent()))
				pActor->Cameras().AddCamEffector(xr_new<CEffectorDOF>(m_zoom_params.m_ReloadDof));
		}
	}
}

void CWeapon::OnAnimationEnd(u32 state) 
{
	inherited::OnAnimationEnd(state);
}

u8 CWeapon::GetCurrentHudOffsetIdx()
{
	CActor* pActor = smart_cast<CActor*>(H_Parent());

	if (!pActor)	
		return 0;
	
	bool b_aiming =  ((IsZoomed() && m_zoom_params.m_fZoomRotationFactor <= 1.f) || (!IsZoomed() && m_zoom_params.m_fZoomRotationFactor > 0.f));

	return (!b_aiming) ? 0 : 1;
}

// Получить HUD FOV текущего оружия
float CWeapon::GetHudFov()
{
	// Рассчитываем HUD FOV от бедра (с учётом упирания в стены)
	if (ParentIsActor() && Level().CurrentViewEntity() == H_Parent())
	{
		// Получаем расстояние от камеры до точки в прицеле
		collide::rq_result& RQ = HUD().GetCurrentRayQuery();
		float dist = RQ.range;

		// Интерполируем расстояние в диапазон от 0 (min) до 1 (max)
		clamp(dist, m_nearwall_dist_min, m_nearwall_dist_max);
		float fDistanceMod = ((dist - m_nearwall_dist_min) / (m_nearwall_dist_max - m_nearwall_dist_min)); // 0.f ... 1.f

																										   // Рассчитываем базовый HUD FOV от бедра
		float fBaseFov = psHUD_FOV_def + m_hud_fov_add_mod;
		clamp(fBaseFov, 0.0f, FLT_MAX);

		// Плавно высчитываем итоговый FOV от бедра
		float src = m_nearwall_speed_mod * Device.fTimeDelta;
		clamp(src, 0.f, 1.f);

		float fTrgFov = m_nearwall_target_hud_fov + fDistanceMod * (fBaseFov - m_nearwall_target_hud_fov);
		m_nearwall_last_hud_fov = m_nearwall_last_hud_fov * (1 - src) + fTrgFov * src;
	}

	/*
	// Возвращаем итоговый HUD FOV
	if (m_zoom_params.m_fZoomRotationFactor > 0.0f)
	{
		// В процессе зума
		float fDiff = m_nearwall_last_hud_fov - m_zoom_params.m_fZoomHudFov;
		return m_zoom_params.m_fZoomHudFov + (fDiff * (1 - m_zoom_params.m_fZoomRotationFactor));
	}
	else
	{*/
		// От бедра
		return m_nearwall_last_hud_fov;
	//}
}

void CWeapon::render_hud_mode()
{
	RenderLight();
}

bool CWeapon::MovingAnimAllowedNow()
{
	return !IsZoomed();
}

bool CWeapon::IsHudModeNow()
{
	return (HudItemData() != nullptr);
}

void CWeapon::ZoomInc()
{
	if (!IsScopeAttached() || !m_zoom_params.m_bUseDynamicZoom)					
		return;

	float delta, min_zoom_factor;
	GetZoomData(m_zoom_params.m_fScopeZoomFactor, delta, min_zoom_factor);

	float f	= GetZoomFactor() - delta;
	clamp(f, m_zoom_params.m_fScopeZoomFactor, min_zoom_factor);
	SetZoomFactor(f);
}

void CWeapon::ZoomDec()
{
	if (!IsScopeAttached() || !m_zoom_params.m_bUseDynamicZoom)					
		return;

	float delta, min_zoom_factor;
	GetZoomData(m_zoom_params.m_fScopeZoomFactor, delta, min_zoom_factor);

	float f	= GetZoomFactor() + delta;
	clamp(f, m_zoom_params.m_fScopeZoomFactor, min_zoom_factor);
	SetZoomFactor(f);
}

u32 CWeapon::Cost() const
{
	u32 res = CInventoryItem::Cost();

	if (IsGrenadeLauncherAttached() && !GetGrenadeLauncherName().empty())
		res += pSettings->r_u32(GetGrenadeLauncherName().c_str(), "cost");

	if (IsScopeAttached() && !m_scopes.empty())
		res += pSettings->r_u32(GetScopeName().c_str(), "cost");

	if (IsSilencerAttached() && !GetSilencerName().empty())
		res += pSettings->r_u32(GetSilencerName().c_str(), "cost");

	if (iAmmoElapsed)
	{
		float w = pSettings->r_float(m_ammoTypes[m_ammoType].c_str(), "cost");
		float bs = pSettings->r_float(m_ammoTypes[m_ammoType].c_str(), "box_size");

		res += iFloor(w * (iAmmoElapsed / bs));
	}

	return res;
}

