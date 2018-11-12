#include "stdafx.h"
#include "actor.h"
#include "hudmanager.h"
#include "Actor_Flags.h"
#include "inventory.h"
#include "xrserver_objects_alife_monsters.h"
#include "xrServer.h"
#include "../xrEngine/CustomHUD.h"
#include "../xrEngine/x_ray.h"
#include "CameraLook.h"

#include "ActorEffector.h"

#include "../xrPhysics/iPHWorld.h"
#include "../xrPhysics/actorcameracollision.h"
#include "level.h"
#include "..\xrEngine\xr_level_controller.h"
#include "infoportion.h"
#include "alife_registry_wrappers.h"
#include "../Include/xrRender/Kinematics.h"
#include "client_spawn_manager.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "CharacterPhysicsSupport.h"
#include "items/Grenade.h"
#include "items/WeaponMagazined.h"
#include "items/WeaponKnife.h"
#include "items/CustomOutfit.h"

#include "actor_anim_defs.h"

#include "UIGame.h"
#include "ui/UIPdaWnd.h"
#include "ui/UITaskWnd.h"

#include "map_manager.h"
#include "ui/UIMainIngameWnd.h"
#include "gamepersistent.h"
#include "game_object_space.h"
#include "GameTaskManager.h"
#include "holder_custom.h"
#include "actor_memory.h"
#include "characterphysicssupport.h"
#include "../xrEngine/xr_collide_form.h"
#ifdef DEBUG
#	include "debug_renderer.h"
#	include "../xrPhysics/phvalide.h"
#endif

int			g_dwInputUpdateDelta		= 20;
BOOL		net_cl_inputguaranteed		= FALSE;

CActor* Actor()	
{
	return (CActor*)g_actor; 
}
//--------------------------------------------------------------------
void	CActor::ConvState(u32 mstate_rl, string128 *buf)
{
	xr_strcpy(*buf,"");
	if (isActorAccelerated(mstate_rl, IsZoomAimingMode()))		xr_strcat(*buf,"Accel ");
	if (mstate_rl&mcCrouch)		xr_strcat(*buf,"Crouch ");
	if (mstate_rl&mcFwd)		xr_strcat(*buf,"Fwd ");
	if (mstate_rl&mcBack)		xr_strcat(*buf,"Back ");
	if (mstate_rl&mcLStrafe)	xr_strcat(*buf,"LStrafe ");
	if (mstate_rl&mcRStrafe)	xr_strcat(*buf,"RStrafe ");
	if (mstate_rl&mcJump)		xr_strcat(*buf,"Jump ");
	if (mstate_rl&mcFall)		xr_strcat(*buf,"Fall ");
	if (mstate_rl&mcTurn)		xr_strcat(*buf,"Turn ");
	if (mstate_rl&mcLanding)	xr_strcat(*buf,"Landing ");
	if (mstate_rl&mcLLookout)	xr_strcat(*buf,"LLookout ");
	if (mstate_rl&mcRLookout)	xr_strcat(*buf,"RLookout ");
	if (m_bJumpKeyPressed)		xr_strcat(*buf,"+Jumping ");
}
//--------------------------------------------------------------------
void CActor::net_Export	(NET_Packet& P)					// export to server
{
	//CSE_ALifeCreatureAbstract
	u8					flags = 0;
	P.w_float			(GetfHealth());
	P.w_u32				(Level().timeServer());
	P.w_u8				(flags);
	Fvector				p = Position();
	P.w_vec3			(p);

	P.w_float			(angle_normalize(r_model_yaw)); //Device.vCameraDirection.getH());//
	P.w_float			(angle_normalize(unaffected_r_torso.yaw));//(r_torso.yaw);
	P.w_float			(angle_normalize(unaffected_r_torso.pitch));//(r_torso.pitch);
	P.w_float			(angle_normalize(unaffected_r_torso.roll));//(r_torso.roll);
	P.w_u8				(u8(g_Team()));
	P.w_u8				(u8(g_Squad()));
	P.w_u8				(u8(g_Group()));

	u16 ms	= (u16)(mstate_real & 0x0000ffff);
	P.w_u16				(u16(ms));
	P.w_sdir			(NET_SavedAccel);
	Fvector				v = character_physics_support()->movement()->GetVelocity();
	P.w_sdir			(v);
	P.w_float			(g_Radiation());

	P.w_u8				(u8(inventory().GetActiveSlot()));
	/////////////////////////////////////////////////
	u16 NumItems		= 0;
	
	P.w_u16				(NumItems);
	if (!NumItems)		return;

	if (g_Alive())
	{
		SPHNetState	State;

		CPHSynchronize* pSyncObj = nullptr;
		pSyncObj = PHGetSyncItem(0);
		pSyncObj->get_State(State);

		P.w_u8					( State.enabled );

		P.w_vec3				( State.angular_vel);
		P.w_vec3				( State.linear_vel);

		P.w_vec3				( State.force);
		P.w_vec3				( State.torque);

		P.w_vec3				( State.position);

		P.w_float				( State.quaternion.x );
		P.w_float				( State.quaternion.y );
		P.w_float				( State.quaternion.z );
		P.w_float				( State.quaternion.w );
	}
	else
	{
		net_ExportDeadBody(P);
	}
}

static void w_vec_q8(NET_Packet& P,const Fvector& vec,const Fvector& min,const Fvector& max)
{
	P.w_float_q8(vec.x,min.x,max.x);
	P.w_float_q8(vec.y,min.y,max.y);
	P.w_float_q8(vec.z,min.z,max.z);
}

static void w_qt_q8(NET_Packet& P,const Fquaternion& q)
{
	///////////////////////////////////////////////////
	P.w_float_q8(q.x,-1.f,1.f);
	P.w_float_q8(q.y,-1.f,1.f);
	P.w_float_q8(q.z,-1.f,1.f);
	P.w_float_q8(q.w,-1.f,1.f);
	///////////////////////////////////////////
}

#define F_MAX         3.402823466e+38F

static void	UpdateLimits (Fvector &p, Fvector& min, Fvector& max)
{
	if(p.x<min.x)min.x=p.x;
	if(p.y<min.y)min.y=p.y;
	if(p.z<min.z)min.z=p.z;

	if(p.x>max.x)max.x=p.x;
	if(p.y>max.y)max.y=p.y;
	if(p.z>max.z)max.z=p.z;

	for (int k=0; k<3; k++)
	{
		if (p[k]<min[k] || p[k]>max[k])
		{
			R_ASSERT2(0, "Fuck");
			UpdateLimits(p, min, max);
		}
	}
};

void		CActor::net_ExportDeadBody		(NET_Packet &P)
{
	/////////////////////////////
	Fvector min,max;

	min.set(F_MAX,F_MAX,F_MAX);
	max.set(-F_MAX,-F_MAX,-F_MAX);
	/////////////////////////////////////
	u16 bones_number		= PHGetSyncItemsNumber();
	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);

		Fvector& p=state.position;
		UpdateLimits (p, min, max);

		Fvector px =state.linear_vel;
		px.div(10.0f);
		px.add(state.position);
		UpdateLimits (px, min, max);
	};

	P.w_u8(10);
	P.w_vec3(min);
	P.w_vec3(max);

	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);
//		state.net_Save(P,min,max);
		w_vec_q8(P,state.position,min,max);
		w_qt_q8(P,state.quaternion);

		//---------------------------------
		Fvector px =state.linear_vel;
		px.div(10.0f);
		px.add(state.position);
		w_vec_q8(P,px,min,max);
	};	
};

BOOL CActor::net_Spawn(CSE_Abstract* DC)
{
	m_holder_id = ALife::_OBJECT_ID(-1);
	m_feel_touch_characters = 0;
	m_snd_noise = 0.0f;
	m_sndShockEffector = nullptr;

	if (m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
	}

	//force actor to be local on server client
	CSE_Abstract			*e = (CSE_Abstract*)(DC);
	CSE_ALifeCreatureActor	*E = smart_cast<CSE_ALifeCreatureActor*>(e);
	E->s_flags.set(M_SPAWN_OBJECT_LOCAL, true);	

	if (E->s_flags.is(M_SPAWN_OBJECT_ASPLAYER))
		g_actor = this;

	VERIFY(!m_pActorEffector);

	m_pActorEffector = xr_new<CActorCameraManager>();

	// motions
	m_bAnimTorsoPlayed = false;
	m_current_legs_blend = nullptr;
	m_current_jump_blend = nullptr;
	m_current_legs.invalidate();
	m_current_torso.invalidate();
	m_current_head.invalidate();
	//-------------------------------------
	game_news_registry->registry().init(ID());

	if (!CInventoryOwner::net_Spawn(DC)) return FALSE;
	if (!inherited::net_Spawn(DC))	return FALSE;

	CSE_ALifeTraderAbstract	 *pTA = smart_cast<CSE_ALifeTraderAbstract*>(e);
	set_money(pTA->m_dwMoney, false);

	ROS()->force_mode(IRender_ObjectSpecific::TRACE_ALL);

	mstate_wishful = E->mstate&(mcCrouch | mcAccel);
	mstate_old = mstate_real = mstate_wishful;
	set_state_box(mstate_real);
	m_pPhysics_support->in_NetSpawn(e);

	if (E->m_holderID != u16(-1))
	{
		character_physics_support()->movement()->DestroyCharacter();
	}
	if (m_bOutBorder)character_physics_support()->movement()->setOutBorder();
	r_torso_tgt_roll = 0;

	r_model_yaw = E->o_torso.yaw;
	r_torso.yaw = E->o_torso.yaw;
	r_torso.pitch = E->o_torso.pitch;
	r_torso.roll = 0.0f;//E->o_Angle.z;

	unaffected_r_torso.yaw = r_torso.yaw;
	unaffected_r_torso.pitch = r_torso.pitch;
	unaffected_r_torso.roll = r_torso.roll;

	cam_Active()->Set(-E->o_torso.yaw, E->o_torso.pitch, 0);

	// *** movement state - respawn
	m_bJumpKeyPressed = FALSE;
	NET_SavedAccel.set(0, 0, 0);

	setEnabled(E->s_flags.is(M_SPAWN_OBJECT_LOCAL));
	Engine.Sheduler.Register(this, TRUE);
	m_hit_slowmo = 0.f;

	OnChangeVisual();
	//----------------------------------
	m_bAllowDeathRemove = false;

	processing_activate();

#ifdef DEBUG
	LastPosS.clear();
	LastPosH.clear();
	LastPosL.clear();
#endif
	
	SetDefaultVisualOutfit(cNameVisual());

	smart_cast<IKinematics*>(Visual())->CalculateBones();

	//--------------------------------------------------------------
	inventory().SetPrevActiveSlot(NO_ACTIVE_SLOT);
	//-------------------------------------
	if (!g_Alive())
	{
		mstate_wishful &= ~mcAnyMove;
		mstate_real &= ~mcAnyMove;
		IKinematicsAnimated* K = smart_cast<IKinematicsAnimated*>(Visual());
		K->PlayCycle("death_init");

		m_HeavyBreathSnd.stop();
	}

	using CALLBACK_TYPE = CClientSpawnManager::CALLBACK_TYPE;
	CALLBACK_TYPE	callback;
	callback.bind(this, &CActor::on_requested_spawn);
	m_holder_id = E->m_holderID;
	if (E->m_holderID != ALife::_OBJECT_ID(-1))
		Level().client_spawn_manager().add(E->m_holderID, ID(), callback);
	//-------------------------------------------------------------
	m_iLastHitterID = u16(-1);
	m_iLastHittingWeaponID = u16(-1);
	m_s16LastHittedElement = -1;
	m_bWasHitted = false;

	Level().MapManager().AddMapLocation("actor_location", ID());
	Level().MapManager().AddMapLocation("actor_location_p", ID());

	spatial.type |= STYPE_REACTTOSOUND;
	psHUD_Flags.set(HUD_WEAPON_RT, true);
	psHUD_Flags.set(HUD_WEAPON_RT2, true);
	
	return TRUE;
}

void CActor::net_Destroy()
{
	inherited::net_Destroy();

	if (m_holder_id != ALife::_OBJECT_ID(-1))
		Level().client_spawn_manager().remove(m_holder_id, ID());

	Level().MapManager().OnObjectDestroyNotify(ID());

	CInventoryOwner::net_Destroy();
	cam_UnsetLadder();
	character_physics_support()->movement()->DestroyCharacter();

	if (m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate();
		xr_delete<IPhysicsShellEx>(m_pPhysicsShell);
	}

	m_pPhysics_support->in_NetDestroy();

	xr_delete(m_sndShockEffector);
	xr_delete(pStatGraph);
	xr_delete(m_pActorEffector);
	pCamBobbing = nullptr;

#ifdef DEBUG	
	LastPosS.clear();
	LastPosH.clear();
	LastPosL.clear();
#endif

	processing_deactivate();
	m_holder = nullptr;
	m_holderID = u16(-1);

	SetDefaultVisualOutfit(nullptr);

	if (g_actor == this) g_actor = nullptr;

	Engine.Sheduler.Unregister(this);

	if (actor_camera_shell && actor_camera_shell->get_ElementByStoreOrder(0)->PhysicsRefObject() == this)
		destroy_physics_shell(actor_camera_shell);
}

void CActor::net_Relcase (CObject* Object)
{
 	VERIFY(Object);

	if (Object != nullptr)
	{
		if (m_pObjectWeLookingAt == Object)
		{
			m_pObjectWeLookingAt = nullptr;
		}

		if ((void*)m_pVehicleWeLookingAt == Object)
		{
			m_pVehicleWeLookingAt = nullptr;
		}

		if ((void*)m_holder == Object)
		{
			m_holder->detach_Actor();
			m_holder = nullptr;
		}
	}

	inherited::net_Relcase	(Object);
	memory().remove_links(Object);
	m_pPhysics_support->in_NetRelcase(Object);
	HUD().net_Relcase	(Object);
}

BOOL CActor::net_Relevant()				// relevant for export to server
{ 
	return getSVU() | getLocal(); 
};

void	CActor::SetCallbacks()
{
	IKinematics* V		= smart_cast<IKinematics*>(Visual());
	VERIFY				(V);
	u16 spine0_bone		= V->LL_BoneID("bip01_spine");
	u16 spine1_bone		= V->LL_BoneID("bip01_spine1");
	u16 shoulder_bone	= V->LL_BoneID("bip01_spine2");
	u16 head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetBoneInstance(u16(spine0_bone)).set_callback	(bctCustom,Spin0Callback,this);
	V->LL_GetBoneInstance(u16(spine1_bone)).set_callback	(bctCustom,Spin1Callback,this);
	V->LL_GetBoneInstance(u16(shoulder_bone)).set_callback	(bctCustom,ShoulderCallback,this);
	V->LL_GetBoneInstance(u16(head_bone)).set_callback		(bctCustom,HeadCallback,this);
}
void	CActor::ResetCallbacks()
{
	IKinematics* V		= smart_cast<IKinematics*>(Visual());
	VERIFY				(V);
	u16 spine0_bone		= V->LL_BoneID("bip01_spine");
	u16 spine1_bone		= V->LL_BoneID("bip01_spine1");
	u16 shoulder_bone	= V->LL_BoneID("bip01_spine2");
	u16 head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetBoneInstance(u16(spine0_bone)).reset_callback	();
	V->LL_GetBoneInstance(u16(spine1_bone)).reset_callback	();
	V->LL_GetBoneInstance(u16(shoulder_bone)).reset_callback();
	V->LL_GetBoneInstance(u16(head_bone)).reset_callback	();
}

void CActor::OnChangeVisual()
{
	{
		IPhysicsShellEx* tmp_shell = PPhysicsShell();
		PPhysicsShell() = nullptr;
		inherited::OnChangeVisual();
		PPhysicsShell() = tmp_shell;
		tmp_shell = nullptr;
	}
	
	if (IKinematicsAnimated* V = smart_cast<IKinematicsAnimated*>(Visual()))
	{
		CStepManager::reload(cNameSect().c_str());
		SetCallbacks		();
		m_anims->Create		(V);
		m_vehicle_anims->Create			(V);
		CDamageManager::reload(*cNameSect(),"damage",pSettings);
		//-------------------------------------------------------------------------------
		m_head				= smart_cast<IKinematics*>(Visual())->LL_BoneID("bip01_head");
		m_eye_left			= smart_cast<IKinematics*>(Visual())->LL_BoneID("eye_left");
		m_eye_right			= smart_cast<IKinematics*>(Visual())->LL_BoneID("eye_right");
		m_r_hand			= smart_cast<IKinematics*>(Visual())->LL_BoneID(pSettings->r_string(*cNameSect(),"weapon_bone0"));
		m_l_finger1			= smart_cast<IKinematics*>(Visual())->LL_BoneID(pSettings->r_string(*cNameSect(),"weapon_bone1"));
		m_r_finger2			= smart_cast<IKinematics*>(Visual())->LL_BoneID(pSettings->r_string(*cNameSect(),"weapon_bone2"));
		//-------------------------------------------------------------------------------
		m_neck				= smart_cast<IKinematics*>(Visual())->LL_BoneID("bip01_neck");
		m_l_clavicle		= smart_cast<IKinematics*>(Visual())->LL_BoneID("bip01_l_clavicle");
		m_r_clavicle		= smart_cast<IKinematics*>(Visual())->LL_BoneID("bip01_r_clavicle");
		m_spine2			= smart_cast<IKinematics*>(Visual())->LL_BoneID("bip01_spine2");
		m_spine1			= smart_cast<IKinematics*>(Visual())->LL_BoneID("bip01_spine1");
		m_spine				= smart_cast<IKinematics*>(Visual())->LL_BoneID("bip01_spine");
		//-------------------------------------------------------------------------------
		reattach_items();
		//-------------------------------------------------------------------------------
		m_pPhysics_support->in_ChangeVisual();
		//-------------------------------------------------------------------------------
		SetCallbacks		();
		//-------------------------------------------------------------------------------
		m_current_head.invalidate	();
		m_current_legs.invalidate	();
		m_current_torso.invalidate	();
		m_current_legs_blend		= nullptr;
		m_current_torso_blend		= nullptr;
		m_current_jump_blend		= nullptr;
	}
};

void CActor::ChangeVisual ( shared_str NewVisual )
{
	if (!NewVisual.size()) return;
	if (cNameVisual().size() )
	{
		if (cNameVisual() == NewVisual) return;
	}

	cNameVisual_set(NewVisual);

	g_SetAnimation(mstate_real);
	Visual()->dcast_PKinematics()->CalculateBones_Invalidate();
	Visual()->dcast_PKinematics()->CalculateBones(TRUE);
};

void CActor::save(NET_Packet &output_packet)
{
	inherited::save(output_packet);
	CInventoryOwner::save(output_packet);
	output_packet.w_u8(u8(m_bOutBorder));
	CUITaskWnd* task_wnd = HUD().GetGameUI()->PdaMenu().pUITaskWnd;
	output_packet.w_u8(task_wnd->IsTreasuresEnabled() ? 1 : 0);
	output_packet.w_u8(task_wnd->IsQuestNpcsEnabled() ? 1 : 0);
	output_packet.w_u8(task_wnd->IsSecondaryTasksEnabled() ? 1 : 0);
	output_packet.w_u8(task_wnd->IsPrimaryObjectsEnabled() ? 1 : 0);

	output_packet.w_stringZ(g_quick_use_slots[0]);
	output_packet.w_stringZ(g_quick_use_slots[1]);
	output_packet.w_stringZ(g_quick_use_slots[2]);
	output_packet.w_stringZ(g_quick_use_slots[3]);
}

void CActor::load(IReader &input_packet)
{
	inherited::load(input_packet);
	CInventoryOwner::load(input_packet);
	m_bOutBorder=!!(input_packet.r_u8());
	CUITaskWnd* task_wnd = HUD().GetGameUI()->PdaMenu().pUITaskWnd;
	task_wnd->TreasuresEnabled(!!input_packet.r_u8());
	task_wnd->QuestNpcsEnabled(!!input_packet.r_u8());
	task_wnd->SecondaryTasksEnabled(!!input_packet.r_u8());
	task_wnd->PrimaryObjectsEnabled(!!input_packet.r_u8());
	//need_quick_slot_reload = true;

	input_packet.r_stringZ(g_quick_use_slots[0], sizeof(g_quick_use_slots[0]));
	input_packet.r_stringZ(g_quick_use_slots[1], sizeof(g_quick_use_slots[1]));
	input_packet.r_stringZ(g_quick_use_slots[2], sizeof(g_quick_use_slots[2]));
	input_packet.r_stringZ(g_quick_use_slots[3], sizeof(g_quick_use_slots[3]));
}

void CActor::net_Save(NET_Packet& P)
{
	inherited::net_Save	(P);
	m_pPhysics_support->in_NetSave(P);
	P.w_u16(m_holderID);
}

BOOL CActor::net_SaveRelevant()
{
	return TRUE;
}

void CActor::SetHitInfo(CObject* who, CObject* weapon, s16 element, Fvector Pos, Fvector Dir)
{
	m_iLastHitterID = (who!= nullptr) ? who->ID() : u16(-1);
	m_iLastHittingWeaponID = (weapon != nullptr) ? weapon->ID() : u16(-1);
	m_s16LastHittedElement = element;
	m_fLastHealth = GetfHealth();
	m_bWasHitted = true;
	m_vLastHitDir = Dir;
	m_vLastHitPos = Pos;
};

bool CActor::InventoryAllowSprint()
{
	PIItem pActiveItem = inventory().ActiveItem();
	if (pActiveItem && !pActiveItem->IsSprintAllowed())
		return false;

	CCustomOutfit* pOutfitItem = GetOutfit();
	if (pOutfitItem && !pOutfitItem->IsSprintAllowed())
		return false;

	return true;
};

BOOL CActor::BonePassBullet(int boneID)
{
	return inherited::BonePassBullet(boneID);
}

void CActor::On_B_NotCurrentEntity()
{
	inventory().Items_SetCurrentEntityHud(false);
};
