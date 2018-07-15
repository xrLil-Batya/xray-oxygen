#include "stdafx.h"
#include "torch.h"
#include "entity.h"
#include "actor.h"
#include "../xrEngine/LightAnimLibrary.h"
#include "../xrphysics/PhysicsShell.h"
#include "xrserver_objects_alife_items.h"
#include "ai_sounds.h"

#include "level.h"
#include "../Include/xrRender/Kinematics.h"
#include "../xrEngine/camerabase.h"
#include "../xrengine/xr_collide_form.h"
#include "inventory.h"
#include "game_base_space.h"

#include "UIGame.h"
#include "actorEffector.h"
#include "CustomOutfit.h"
#include "ActorHelmet.h"

static const float TORCH_INERTION_CLAMP = PI_DIV_6;
static const float TORCH_INERTION_SPEED_MAX	= 7.5f;
static const float TORCH_INERTION_SPEED_MIN	= 0.5f;
static Fvector TORCH_OFFSET	= {-0.2f,+0.1f,-0.3f};
static const Fvector OMNI_OFFSET = {-0.2f,+0.1f,-0.1f};
static const float OPTIMIZATION_DISTANCE = 100.f;

CTorch::CTorch(void)
{
	light_render = ::Render->light_create();
	light_render->set_type(IRender_Light::SPOT);
	light_render->set_shadow(true);
	light_omni = ::Render->light_create();
	light_omni->set_type(IRender_Light::POINT);
	light_omni->set_shadow(true);

	m_switched_on = false;
	glow_render = ::Render->glow_create();
	lanim = 0;
	fBrightness = 1.f;

	m_prev_hp.set(0, 0);
	m_delta_h = 0;
	m_night_vision = nullptr;
	torch_mode = 1;
}

CTorch::~CTorch()
{
	light_render.destroy();
	light_omni.destroy();
	glow_render.destroy();
	xr_delete(m_night_vision);
}

void CTorch::Load(LPCSTR section)
{
	inherited::Load(section);
	light_trace_bone = pSettings->r_string(section, "light_trace_bone");

	m_bNightVisionEnabled = !!pSettings->r_bool(section, "night_vision");
}

void CTorch::SwitchNightVision()
{
	SwitchNightVision(!m_bNightVisionOn);	
}

void CTorch::SwitchNightVision(bool vision_on, bool use_sounds)
{
	if (!m_bNightVisionEnabled) 
		return;

	m_bNightVisionOn = vision_on;

	CActor *pA = smart_cast<CActor *>(H_Parent());
	if (!pA)
		return;

	if (!m_night_vision)
		m_night_vision = xr_new<CNightVisionEffector>(cNameSect());

	LPCSTR disabled_names = pSettings->r_string(cNameSect(), "disabled_maps");
	LPCSTR curr_map = *Level().name();
	u32 cnt = _GetItemCount(disabled_names);

	bool b_allow = true;
	string512 tmp;
	for (u32 i = 0; i < cnt; ++i) 
	{
		_GetItem(disabled_names, i, tmp);
		if (0 == stricmp(tmp, curr_map)) 
		{
			b_allow = false;
			break;
		}
	}

	CHelmet* pHelmet = smart_cast<CHelmet*>(pA->inventory().ItemFromSlot(HELMET_SLOT));
	CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>(pA->inventory().ItemFromSlot(OUTFIT_SLOT));

	if (pHelmet && pHelmet->m_NightVisionSect.size() && !b_allow)
	{
		m_night_vision->OnDisabled(pA, use_sounds);
		return;
	}
	else if (pOutfit && pOutfit->m_NightVisionSect.size() && !b_allow)
	{
		m_night_vision->OnDisabled(pA, use_sounds);
		return;
	}

	bool bIsActiveNow = m_night_vision->IsActive();

	if (m_bNightVisionOn)
	{

		if (!bIsActiveNow)
		{
			if (pHelmet && pHelmet->m_NightVisionSect.size())
			{
				m_night_vision->Start(pHelmet->m_NightVisionSect, pA, use_sounds);
				return;
			}
			else if (pOutfit && pOutfit->m_NightVisionSect.size())
			{
				m_night_vision->Start(pOutfit->m_NightVisionSect, pA, use_sounds);
				return;
			}
			m_bNightVisionOn = false; // in case if there is no nightvision in helmet and outfit
		}
	}
	else
	{
		if (bIsActiveNow)
		{
			m_night_vision->Stop(100000.0f, use_sounds);
		}
	}
}

void CTorch::SwitchSoundPlay()
{
	if (pSettings->line_exist(cNameSect(), "switch_sound"))
	{
		if (m_switch_sound._feedback())
			m_switch_sound.stop();

		shared_str snd_name = pSettings->r_string(cNameSect(), "switch_sound");
		m_switch_sound.create(snd_name.c_str(), st_Effect, sg_SourceType);
		m_switch_sound.play(nullptr, sm_2D);
	}
}

void CTorch::Switch()
{
	bool bActive = !m_switched_on;
	Switch(bActive);
	SwitchSoundPlay();
}

void CTorch::Switch(bool light_on)
{
	m_switched_on = light_on;
	light_render->set_active(light_on);
	light_omni->set_active(light_on);
	glow_render->set_active(light_on);

	if (*light_trace_bone)
	{
		IKinematics* pVisual = smart_cast<IKinematics*>(Visual()); 
		VERIFY(pVisual);

		u16 bi = pVisual->LL_BoneID(light_trace_bone);

		pVisual->LL_SetBoneVisible(bi, light_on, TRUE);
		pVisual->CalculateBones(TRUE);
	}
}

bool CTorch::torch_active() const
{
	return m_switched_on;
}

BOOL CTorch::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract* e = (CSE_Abstract*)(DC);
	CSE_ALifeItemTorch* torch = smart_cast<CSE_ALifeItemTorch*>(e);
	R_ASSERT(torch);
	cNameVisual_set(torch->get_visual());

	R_ASSERT(!CFORM());
	R_ASSERT(smart_cast<IKinematics*>(Visual()));
	collidable.model = xr_new<CCF_Skeleton>(this);

	if (!inherited::net_Spawn(DC))
		return FALSE;

	IKinematics* K = smart_cast<IKinematics*>(Visual());
	CInifile* pUserData = K->LL_UserData();
	R_ASSERT3(pUserData, "Empty Torch user data!", torch->get_visual());
	lanim = LALib.FindItem(pUserData->r_string("torch_definition", "color_animator"));
	guid_bone = K->LL_BoneID(pUserData->r_string("torch_definition", "guide_bone"));	
	VERIFY(guid_bone != BI_NONE);

	Fcolor clr = pUserData->r_fcolor("torch_definition", "color");
	range = pUserData->r_float("torch_definition", "range");
	range_2 = pUserData->r_float("torch_definition", "range_2");

	Fcolor clr_o = pUserData->r_fcolor("torch_definition", "omni_color");
	range_o = pUserData->r_float("torch_definition", "omni_range");
	range_o_2 = pUserData->r_float("torch_definition", "omni_range_2");

	glow_radius = pUserData->r_float("torch_definition", "glow_radius");
	glow_radius_2 = pUserData->r_float("torch_definition", "glow_radius_2");

	fBrightness = clr.intensity();
	light_render->set_color(clr);
	light_omni->set_color(clr_o);
	light_render->set_range(range);
	light_omni->set_range(range_o);

	light_render->set_cone(deg2rad(pUserData->r_float("torch_definition", "spot_angle")));
	light_render->set_texture(pUserData->r_string("torch_definition", "spot_texture"));

	//--[[ Volumetric light
	if (smart_cast<CActor*>(H_Parent()))
		light_render->set_volumetric(!!READ_IF_EXISTS(pUserData, r_bool, "torch_definition", "volumetric_for_actor", false));
	else
		light_render->set_volumetric(!!READ_IF_EXISTS(pUserData, r_bool, "torch_definition", "volumetric", true));

	light_render->set_volumetric_distance(pUserData->r_float("torch_definition", "volumetric_distance"));
	light_render->set_volumetric_intensity(pUserData->r_float("torch_definition", "volumetric_intensity"));
	light_render->set_volumetric_quality(pUserData->r_float("torch_definition", "volumetric_quality"));
	//--]]

	glow_render->set_color(clr);
	glow_render->set_texture(pUserData->r_string("torch_definition", "glow_texture"));
	glow_render->set_radius(glow_radius);

	Switch(torch->m_active);
	VERIFY(!torch->m_active || (torch->ID_Parent != 0xffff));

	if (torch->ID_Parent == 0)
		SwitchNightVision(torch->m_nightvision_active, false);

	m_delta_h = PI_DIV_2 - atan((range * 0.5f) / _abs(TORCH_OFFSET.x));

	return TRUE;
}

void CTorch::SwitchTorchMode()
{
	if (!m_switched_on)
		return;

	SwitchSoundPlay();

	switch (torch_mode)
	{
		case 1:
		{
			torch_mode = 2;
			light_render->set_range(range_2);
			light_omni->set_range(range_o_2);
			glow_render->set_radius(glow_radius_2);
		} break;

		case 2:
		{
			torch_mode = 1;
			light_render->set_range(range);
			light_omni->set_range(range_o);
			glow_render->set_radius(glow_radius);
		} break;
	}
}

void CTorch::net_Destroy()
{
	Switch(false);
	SwitchNightVision(false);

	inherited::net_Destroy();
}

void CTorch::OnH_A_Chield()
{
	inherited::OnH_A_Chield();
	m_focus.set(Position());
}

void CTorch::OnH_B_Independent(bool just_before_destroy)
{
	inherited::OnH_B_Independent(just_before_destroy);

	Switch(false);
	SwitchNightVision(false);

	m_sounds.StopAllSounds();
}

void CTorch::UpdateCL()
{
	inherited::UpdateCL();

	if (!m_switched_on)			
		return;

	CBoneInstance &BI = smart_cast<IKinematics*>(Visual())->LL_GetBoneInstance(guid_bone);
	Fmatrix	M;

	if (H_Parent())
	{
		CActor* actor = smart_cast<CActor*>(H_Parent());

		if (actor)		
			smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones_Invalidate();

		if (H_Parent()->XFORM().c.distance_to_sqr(Device.vCameraPosition) < _sqr(OPTIMIZATION_DISTANCE))
		{
			// near camera
			smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones();
			M.mul_43(XFORM(), BI.mTransform);
		}
		else 
		{
			// approximately the same
			M = H_Parent()->XFORM();
			H_Parent()->Center(M.c);
			M.c.y += H_Parent()->Radius()*2.f / 3.f;
		}

		if (actor)
		{
			if (actor->active_cam() == eacLookAt)
			{
				m_prev_hp.x = angle_inertion_var(m_prev_hp.x, -actor->cam_Active()->yaw, TORCH_INERTION_SPEED_MIN, TORCH_INERTION_SPEED_MAX, TORCH_INERTION_CLAMP, Device.fTimeDelta);
				m_prev_hp.y = angle_inertion_var(m_prev_hp.y, -actor->cam_Active()->pitch, TORCH_INERTION_SPEED_MIN, TORCH_INERTION_SPEED_MAX, TORCH_INERTION_CLAMP, Device.fTimeDelta);
			}
			else 
			{
				m_prev_hp.x = angle_inertion_var(m_prev_hp.x, -actor->cam_FirstEye()->yaw, TORCH_INERTION_SPEED_MIN, TORCH_INERTION_SPEED_MAX, TORCH_INERTION_CLAMP, Device.fTimeDelta);
				m_prev_hp.y = angle_inertion_var(m_prev_hp.y, -actor->cam_FirstEye()->pitch, TORCH_INERTION_SPEED_MIN, TORCH_INERTION_SPEED_MAX, TORCH_INERTION_CLAMP, Device.fTimeDelta);
			}

			Fvector	dir, right, up;
			dir.setHP(m_prev_hp.x + m_delta_h, m_prev_hp.y);
			Fvector::generate_orthonormal_basis_normalized(dir, up, right);

			Fvector offset = M.c;
			offset.mad(M.i, TORCH_OFFSET.x);
			offset.mad(M.j, TORCH_OFFSET.y);
			offset.mad(M.k, TORCH_OFFSET.z);
			light_render->set_position(offset);

			offset = M.c;
			offset.mad(M.i, OMNI_OFFSET.x);
			offset.mad(M.j, OMNI_OFFSET.y);
			offset.mad(M.k, OMNI_OFFSET.z);
			light_omni->set_position(offset);

			if (actor->cam_FirstEye())
				light_omni->set_shadow(false);

			glow_render->set_position(M.c);

			light_render->set_rotation(dir, right);
			light_omni->set_rotation(dir, right);

			glow_render->set_direction(dir);

		}
		else
		{
			light_render->set_position(M.c);
			light_render->set_rotation(M.k, M.i);

			Fvector offset = M.c;
			offset.mad(M.i, OMNI_OFFSET.x);
			offset.mad(M.j, OMNI_OFFSET.y);
			offset.mad(M.k, OMNI_OFFSET.z);
			light_omni->set_position(M.c);
			light_omni->set_rotation(M.k, M.i);

			glow_render->set_position(M.c);
			glow_render->set_direction(M.k);
		}
	}
	else
	{
		if (getVisible() && m_pPhysicsShell)
		{
			M.mul(XFORM(), BI.mTransform);

			m_switched_on = false;
			light_render->set_active(false);
			light_omni->set_active(false);
			glow_render->set_active(false);
		} 
	}

	if (!m_switched_on)					
		return;

	// calc color animator
	if (!lanim)							
		return;

	int	frame;
	u32 clr = lanim->CalculateBGR(Device.fTimeGlobal, frame);

	Fcolor fclr;
	fclr.set((float)color_get_B(clr), (float)color_get_G(clr), (float)color_get_R(clr), 1.f);
	fclr.mul_rgb(fBrightness / 255.f);
	light_render->set_color(fclr);
	light_omni->set_color(fclr);
	glow_render->set_color(fclr);
}

void CTorch::create_physic_shell()
{
	CPhysicsShellHolder::create_physic_shell();
}

void CTorch::activate_physic_shell()
{
	CPhysicsShellHolder::activate_physic_shell();
}

void CTorch::setup_physic_shell()
{
	CPhysicsShellHolder::setup_physic_shell();
}

void CTorch::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);

	BYTE F = 0;
	F |= (m_switched_on ? eTorchActive : 0);
	F |= (m_bNightVisionOn ? eNightVisionActive : 0);
	const CActor *pA = smart_cast<const CActor *>(H_Parent());
	if (pA)
	{
		if (pA->attached(this))
			F |= eAttached;
	}
	P.w_u8(F);
}

bool  CTorch::can_be_attached() const
{
	const CActor *pA = smart_cast<const CActor *>(H_Parent());

	return pA ? pA->inventory().InSlot(this) : true;
}

void CTorch::afterDetach()
{
	inherited::afterDetach();
	Switch(false);
}

void CTorch::renderable_Render()
{
	inherited::renderable_Render();
}

void CTorch::enable(bool value)
{
	inherited::enable(value);

	if (!enabled() && m_switched_on)
		Switch(false);

}

CNightVisionEffector::CNightVisionEffector(const shared_str& section) : m_pActor(nullptr)
{
	m_sounds.LoadSound(section.c_str(), "snd_night_vision_on", "NightVisionOnSnd", false, SOUND_TYPE_ITEM_USING);
	m_sounds.LoadSound(section.c_str(), "snd_night_vision_off", "NightVisionOffSnd", false, SOUND_TYPE_ITEM_USING);
	m_sounds.LoadSound(section.c_str(), "snd_night_vision_idle", "NightVisionIdleSnd", false, SOUND_TYPE_ITEM_USING);
	m_sounds.LoadSound(section.c_str(), "snd_night_vision_broken", "NightVisionBrokenSnd", false, SOUND_TYPE_ITEM_USING);
}

void CNightVisionEffector::Start(const shared_str& sect, CActor* pA, bool play_sound)
{
	m_pActor = pA;
	AddEffector(m_pActor, effNightvision, sect);
	if (play_sound)
	{
		PlaySounds(eStartSound);
		PlaySounds(eIdleSound);
	}
}

void CNightVisionEffector::Stop(const float factor, bool play_sound)
{
	if (!m_pActor)
		return;

	CEffectorPP* pp = m_pActor->Cameras().GetPPEffector((EEffectorPPType)effNightvision);
	if (pp)
	{
		pp->Stop(factor);
		if (play_sound)
			PlaySounds(eStopSound);

		m_sounds.StopSound("NightVisionIdleSnd");
	}
}

bool CNightVisionEffector::IsActive()
{
	if (!m_pActor)	
		return false;

	CEffectorPP* pp = m_pActor->Cameras().GetPPEffector((EEffectorPPType)effNightvision);
	return (pp != nullptr);
}

void CNightVisionEffector::OnDisabled(CActor* pA, bool play_sound)
{
	m_pActor					= pA;
	if(play_sound)
		PlaySounds(eBrokeSound);
}

void CNightVisionEffector::PlaySounds(EPlaySounds which)
{
	if(!m_pActor)
		return;
	
	bool bPlaySoundFirstPerson = !!m_pActor->HUDview();
	switch(which)
	{
	case eStartSound:
		{
			m_sounds.PlaySound("NightVisionOnSnd", m_pActor->Position(), NULL, bPlaySoundFirstPerson);
		}break;
	case eStopSound:
		{
			m_sounds.PlaySound("NightVisionOffSnd", m_pActor->Position(), NULL, bPlaySoundFirstPerson);
		}break;
	case eIdleSound:
		{
			m_sounds.PlaySound("NightVisionIdleSnd", m_pActor->Position(), NULL, bPlaySoundFirstPerson, true);
		}break;
	case eBrokeSound:
		{
			m_sounds.PlaySound("NightVisionBrokenSnd", m_pActor->Position(), NULL, bPlaySoundFirstPerson);
		}break;
	default: NODEFAULT;
	}
}

#include "PDA.h"
#include "SimpleDetector.h"
#include "EliteDetector.h"
#include "AdvancedDetector.h"

using namespace luabind;

#pragma optimize("s",on)
void CTorch::script_register(lua_State *L)
{
	module(L)
		[
		class_<CTorch, CGameObject>("CTorch")
			.def(constructor<>()),
		class_<CPda, CGameObject>("CPda")
			.def(constructor<>()),
		class_<CScientificDetector, CGameObject>("CScientificDetector")
			.def(constructor<>()),
		class_<CEliteDetector, CGameObject>("CEliteDetector")
			.def(constructor<>()),
		class_<CAdvancedDetector, CGameObject>("CAdvancedDetector")
			.def(constructor<>()),
		class_<CSimpleDetector, CGameObject>("CSimpleDetector")
			.def(constructor<>())
		];
}
