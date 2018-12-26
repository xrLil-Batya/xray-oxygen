#include "stdafx.h"
#include "physicobject.h"
#include "../xrphysics/PhysicsShell.h"
#include "xrserver_objects_alife.h"
#include "Level.h"
#include "../Include/xrRender/Kinematics.h"
#include "../Include/xrRender/KinematicsAnimated.h"
#include "../xrEngine/xr_collide_form.h"
#include "../xrEngine/cf_dynamic_mesh.h"
#include "PHSynchronize.h"
#include "game_object_space.h"
#include "moving_bones_snd_player.h"
#include "../xrphysics/extendedgeom.h"
#ifdef	DEBUG
#	include "phdebug.h"
#	include "../xrengine/objectdump.h"
#endif
BOOL dbg_draw_doors = false;

CPhysicObject::CPhysicObject() :
	m_anim_blend(nullptr), m_type(epotBox), m_mass(10.f), m_collision_hit_callback(nullptr), bones_snd_player(nullptr), m_net_updateData(nullptr)
{
}

CPhysicObject::~CPhysicObject()
{
	xr_delete(m_net_updateData);
}

BOOL CPhysicObject::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract			*e = (CSE_Abstract*)(DC);
	CSE_ALifeObjectPhysic	*po = smart_cast<CSE_ALifeObjectPhysic*>(e);
	R_ASSERT(po);
	m_type = EPOType(po->type);
	m_mass = po->mass;
	m_collision_hit_callback = nullptr;
	m_anim_blend = nullptr;
	inherited::net_Spawn(DC);

	create_collision_model();


	CPHSkeleton::Spawn(e);
	setVisible(TRUE);
	setEnabled(TRUE);

	if (!PPhysicsShell()->isBreakable() && !CScriptBinder::object() && !CPHSkeleton::IsRemoving())
		SheduleUnregister();

	bones_snd_player = create_moving_bones_snd_player(*this);
	if (bones_snd_player) play_bones_sound();

	m_just_after_spawn = true;
	m_activated = false;

	if (DC->s_flags.is(M_SPAWN_UPDATE)) {
		NET_Packet				temp;
		temp.B.count = 0;
		DC->UPDATE_Write(temp);
		if (temp.B.count > 0)
		{
			temp.r_seek(0);
			net_Import(temp);
		}
	}
#ifdef	DEBUG
	if (dbg_draw_doors)
	{
		DBG_OpenCashedDraw();
		Fvector closed, open;
		get_door_vectors(closed, open);
		DBG_ClosedCashedDraw(50000000);
	}
#endif	
	return TRUE;

}
void CPhysicObject::create_collision_model()
{
	xr_delete(collidable.model);

	VERIFY(Visual());
	IKinematics *K = Visual()->dcast_PKinematics();
	VERIFY(K);

	CInifile* ini = K->LL_UserData();
	if (ini && ini->section_exist("collide") && ini->line_exist("collide", "mesh") && ini->r_bool("collide", "mesh"))
	{
		collidable.model = xr_new<CCF_DynamicMesh>(this);
		return;
	}

	collidable.model = xr_new<CCF_Skeleton>(this);
}
void CPhysicObject::play_bones_sound()
{
	if (!bones_snd_player)
	{
		Msg("! no sound loaded for obj: %s, model :%s - can not play", cName().c_str(), cNameVisual().c_str());
		return;
	}
	if (is_active(bones_snd_player))
		return;
	//processing_activate();
	bones_snd_player->play(*this);
}

void CPhysicObject::stop_bones_sound()
{
	if (!is_active(bones_snd_player))
		return;
	bones_snd_player->stop();
}

static CPhysicsShellHolder* retrive_collide_object(bool bo1, dContact& c)
{
	CPhysicsShellHolder* collide_obj = nullptr;
	dxGeomUserData* ud = nullptr;

	if (bo1)
		ud = PHRetrieveGeomUserData(c.geom.g2);
	else
		ud = PHRetrieveGeomUserData(c.geom.g1);

	if (ud)
		collide_obj = static_cast<CPhysicsShellHolder*>(ud->ph_ref_object);
	else
		collide_obj = nullptr;
	return collide_obj;
}

static void door_ignore(bool& do_collide, bool bo1, dContact& c, SGameMtl * /*material_1*/, SGameMtl * /*material_2*/)
{
	CPhysicsShellHolder* collide_obj = retrive_collide_object(bo1, c);
	if (!collide_obj || collide_obj->cast_actor())
		return;

	IPhysicsShellEx *ph_shell = collide_obj->PPhysicsShell();
	if (!ph_shell)
	{
		do_collide = false;//? must be AI
		return;
	}
	VERIFY(ph_shell);

	if (ph_shell->HasTracedGeoms())
		return;

	do_collide = false;
}

void CPhysicObject::set_door_ignore_dynamics()
{
	R_ASSERT(PPhysicsShell());
	PPhysicsShell()->remove_ObjectContactCallback(door_ignore);
	PPhysicsShell()->add_ObjectContactCallback(door_ignore);
}

void CPhysicObject::unset_door_ignore_dynamics()
{
	R_ASSERT(PPhysicsShell());
	PPhysicsShell()->remove_ObjectContactCallback(door_ignore);
}

void CPhysicObject::SpawnInitPhysics(CSE_Abstract* D)
{
	CreatePhysicsShell(D);
	RunStartupAnim(D);
}

void CPhysicObject::RunStartupAnim(CSE_Abstract *D)
{
	if (Visual() && smart_cast<IKinematics*>(Visual()))
	{
		IKinematicsAnimated*	PKinematicsAnimated = nullptr;
		R_ASSERT(Visual() && smart_cast<IKinematics*>(Visual()));
		PKinematicsAnimated = smart_cast<IKinematicsAnimated*>(Visual());
		if (PKinematicsAnimated)
		{
			CSE_Visual					*visual = smart_cast<CSE_Visual*>(D);
			R_ASSERT(visual);
			R_ASSERT2(*visual->startup_animation, "no startup animation");

			VERIFY_FORMAT(!!PKinematicsAnimated->LL_MotionID(visual->startup_animation.c_str()).valid(), 
				"animation %s not faund %s", 
				visual->startup_animation.c_str(), dbg_object_base_dump_string(this).c_str());
			m_anim_blend = m_anim_script_callback.play_cycle(PKinematicsAnimated, visual->startup_animation);
		}
		smart_cast<IKinematics*>(Visual())->CalculateBones_Invalidate();
		smart_cast<IKinematics*>(Visual())->CalculateBones(TRUE);
	}
}

IC	bool check_blend(CBlend * b, LPCSTR name, LPCSTR sect, LPCSTR visual)
{
	return !!b;
}

void CPhysicObject::run_anim_forward()
{
	if (!check_blend(m_anim_blend, cName().c_str(), cNameSect().c_str(), cNameVisual().c_str()))
		return;
	m_anim_blend->playing = TRUE;
	m_anim_blend->stop_at_end_callback = TRUE;
	if (m_anim_blend->speed < 0.f)
		m_anim_blend->speed = -m_anim_blend->speed;

}
void CPhysicObject::run_anim_back()
{
	if (!check_blend(m_anim_blend, cName().c_str(), cNameSect().c_str(), cNameVisual().c_str()))
		return;
	m_anim_blend->playing = TRUE;
	m_anim_blend->stop_at_end_callback = TRUE;
	if (m_anim_blend->speed > 0.f)
		m_anim_blend->speed = -m_anim_blend->speed;
}

void CPhysicObject::stop_anim()
{
	if (!check_blend(m_anim_blend, cName().c_str(), cNameSect().c_str(), cNameVisual().c_str()))
		return;
	m_anim_blend->playing = FALSE;
}

float	CPhysicObject::anim_time_get()
{
	if (!check_blend(m_anim_blend, cName().c_str(), cNameSect().c_str(), cNameVisual().c_str()))
		return 0.f;
	return m_anim_blend->timeCurrent;
}

void CPhysicObject::anim_time_set(float time)
{
	if (!check_blend(m_anim_blend, cName().c_str(), cNameSect().c_str(), cNameVisual().c_str()))
		return;

	if (time < 0.f || time > m_anim_blend->timeTotal)
		return;

	m_anim_blend->timeCurrent = time;
	IKinematics *K = smart_cast<IKinematics*>(Visual());
	VERIFY(K);
	K->CalculateBones_Invalidate();
	K->CalculateBones(TRUE);
}

void CPhysicObject::net_Destroy()
{
	inherited::net_Destroy();
	CPHSkeleton::RespawnInit();
	xr_delete(bones_snd_player);
}

void CPhysicObject::net_Save(NET_Packet& P)
{
	inherited::net_Save(P);
	CPHSkeleton::SaveNetState(P);
}
void CPhysicObject::CreatePhysicsShell(CSE_Abstract* e)
{
	CSE_ALifeObjectPhysic	*po = smart_cast<CSE_ALifeObjectPhysic*>(e);
	CreateBody(po);
}

void CPhysicObject::CreateSkeleton(CSE_ALifeObjectPhysic* po)
{
	if (m_pPhysicsShell) return;
	if (!Visual()) return;
	LPCSTR	fixed_bones = *po->fixed_bones;
	m_pPhysicsShell = P_build_Shell(this, !po->_flags.test(CSE_PHSkeleton::flActive), fixed_bones);
	ApplySpawnIniToPhysicShell(&po->spawn_ini(), m_pPhysicsShell, fixed_bones[0] != '\0');
	ApplySpawnIniToPhysicShell(smart_cast<IKinematics*>(Visual())->LL_UserData(), m_pPhysicsShell, fixed_bones[0] != '\0');
}

void CPhysicObject::Load(LPCSTR section)
{
	inherited::Load(section);
	CPHSkeleton::Load(section);
}


void CPhysicObject::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	CPHSkeleton::Update(dt);
#ifdef	DEBUG
	if (dbg_draw_doors)
	{
		Fvector c, o;
		get_door_vectors(c, o);
	}
#endif
}

void CPhysicObject::UpdateCL()
{
	inherited::UpdateCL();

	//Если наш физический объект анимированный, то 
	//двигаем объект за анимацией
	if (m_pPhysicsShell->PPhysicsShellAnimator())
		m_pPhysicsShell->AnimatorOnFrame();

	m_anim_script_callback.update(*this);
	PHObjectPositionUpdate();

#ifdef	DEBUG
	if (dbg_draw_doors)
	{
		Fvector c, o;
		get_door_vectors(c, o);
	}
#endif

	if (!is_active(bones_snd_player))
		return;
	bones_snd_player->update(Device.fTimeDelta, *this);

}

void CPhysicObject::PHObjectPositionUpdate()
{
	if (m_pPhysicsShell)
	{
		if (m_type == epotBox)
		{
			m_pPhysicsShell->Update();
			XFORM().set(m_pPhysicsShell->mXFORM);
		}
		else if (m_pPhysicsShell->PPhysicsShellAnimator())
		{
			Fmatrix m;
			m_pPhysicsShell->InterpolateGlobalTransform(&m);
			XFORM().set(m);
		}
		else
			m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());
	}
}

void CPhysicObject::AddElement(IPhysicsElementEx* root_e, int id)
{
	IKinematics* K = smart_cast<IKinematics*>(Visual());

	IPhysicsElementEx* E = P_create_Element();
	CBoneInstance& B = K->LL_GetBoneInstance(u16(id));
	E->mXFORM.set(K->LL_GetTransform(u16(id)));
	Fobb bb = K->LL_GetBox(u16(id));

	if (bb.m_halfsize.magnitude()<0.05f)
	{
		bb.m_halfsize.add(0.05f);

	}
	E->add_Box(bb);
	E->setMass(10.f);
	E->set_ParentElement(root_e);
	B.set_callback(bctPhysics, m_pPhysicsShell->GetBonesCallback(), E);
	m_pPhysicsShell->add_Element(E);
	if (!(m_type == epotFreeChain && root_e == nullptr))
	{
		IPhysicsJoint* J = P_create_Joint(IPhysicsJoint::full_control, root_e, E);
		J->SetAnchorVsSecondElement(0, 0, 0);
		J->SetAxisDirVsSecondElement(1, 0, 0, 0);
		J->SetAxisDirVsSecondElement(0, 1, 0, 2);
		J->SetLimits(-M_PI / 2, M_PI / 2, 0);
		J->SetLimits(-M_PI / 2, M_PI / 2, 1);
		J->SetLimits(-M_PI / 2, M_PI / 2, 2);
		m_pPhysicsShell->add_Joint(J);
	}

	CBoneData& BD = K->LL_GetData(u16(id));
	for (vecBonesIt it = BD.children.begin(); BD.children.end() != it; ++it) {
		AddElement(E, (*it)->GetSelfID());
	}
}


void CPhysicObject::CreateBody(CSE_ALifeObjectPhysic* po) {

	if (m_pPhysicsShell) return;
	IKinematics* pKinematics = smart_cast<IKinematics*>(Visual());
	switch (m_type)
	{
	case epotBox:  m_pPhysicsShell = P_build_SimpleShell(this, m_mass, !po->_flags.test(CSE_ALifeObjectPhysic::flActive)); break;
	case epotFixedChain:
	case epotFreeChain:
	{
		m_pPhysicsShell = P_create_Shell();
		m_pPhysicsShell->set_Kinematics(pKinematics);
		AddElement(nullptr, pKinematics->LL_GetBoneRoot());
		m_pPhysicsShell->setMass1(m_mass);
	} break;
	case epotSkeleton: CreateSkeleton(po); break;
	default: break;

	}

	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->SetAirResistance(0.001f, 0.02f);
	if (pKinematics)
	{

		SAllDDOParams disable_params;
		disable_params.Load(pKinematics->LL_UserData());
		m_pPhysicsShell->set_DisableParams(disable_params);
	}
}

BOOL CPhysicObject::net_SaveRelevant()
{
	return TRUE;//!m_flags.test(CSE_ALifeObjectPhysic::flSpawnCopy);
}


BOOL CPhysicObject::UsedAI_Locations()
{
	return					(FALSE);
}


void CPhysicObject::InitServerObject(CSE_Abstract * D)
{
	CPHSkeleton::InitServerObject(D);
	CSE_ALifeObjectPhysic		*l_tpALifePhysicObject = smart_cast<CSE_ALifeObjectPhysic*>(D);
	if (!l_tpALifePhysicObject)return;
	l_tpALifePhysicObject->type = u32(m_type);
}
ICollisionHitCallback*	CPhysicObject::get_collision_hit_callback()
{
	return m_collision_hit_callback;
}
void CPhysicObject::set_collision_hit_callback(ICollisionHitCallback *cc)
{
	xr_delete(m_collision_hit_callback);
	m_collision_hit_callback = cc;
}

//////////////////////////////////////////////////////////////////////////
bool CPhysicObject::is_ai_obstacle() const
{
	return							!!(READ_IF_EXISTS(pSettings, r_bool, cNameSect(), "is_ai_obstacle", true));
}

// network synchronization ----------------------------
void CPhysicObject::net_Export(NET_Packet& P)
{
	P.w_u8(0);
}

void CPhysicObject::net_Import(NET_Packet& P)
{
	u8 NumItems = P.r_u8();
	if (NumItems)
	{
		CSE_ALifeObjectPhysic::mask_num_items num_items;
		num_items.common = NumItems;
		NumItems = num_items.num_items;

		////////////////////////////////////////////
		P.r_u8();	// freezed or not..
	}
}

//-----------
float CPhysicObject::interpolate_states(net_update_PItem const & first, net_update_PItem const & last, SPHNetState & current)
{
	float ret_val = 0.f;
	u32 CurTime = Device.dwTimeGlobal;

	if (CurTime == last.dwTimeStamp)
		return 0.f;

	float factor = float(CurTime - last.dwTimeStamp) / float(last.dwTimeStamp - first.dwTimeStamp);

	ret_val = factor;
	if (factor > 1.f)
	{
		factor = 1.f;
	}
	else if (factor < 0.f)
	{
		factor = 0.f;
	}

	current.position.x = first.State.position.x + (factor * (last.State.position.x - first.State.position.x));
	current.position.y = first.State.position.y + (factor * (last.State.position.y - first.State.position.y));
	current.position.z = first.State.position.z + (factor * (last.State.position.z - first.State.position.z));
	current.previous_position = current.position;

	current.quaternion.slerp(first.State.quaternion, last.State.quaternion, factor);
	current.previous_quaternion = current.quaternion;
	return ret_val;
}

bool	CPhysicObject::get_door_vectors(Fvector& closed, Fvector& open) const
{
	VERIFY(Visual());
	IKinematics *K = Visual()->dcast_PKinematics();
	VERIFY(K);
	u16 door_bone = K->LL_BoneID("door");
	if (door_bone == BI_NONE)
		return false;
	const CBoneData &bd = K->LL_GetData(door_bone);
	const SBoneShape &shape = bd.shape;
	if (shape.type != SBoneShape::stBox)
		return false;

	if (shape.flags.test(SBoneShape::sfNoPhysics))
		return false;

	Fmatrix start_bone_pos;
	K->Bone_GetAnimPos(start_bone_pos, door_bone, u8(-1), true);

	Fmatrix start_pos = Fmatrix().mul_43(XFORM(), start_bone_pos);

	const Fobb &box = shape.box;

	Fvector center_pos;
	start_pos.transform_tiny(center_pos, box.m_translate);

	Fvector door_dir;  start_pos.transform_dir(door_dir, box.m_rotate.i);
	Fvector door_dir_local = box.m_rotate.i;

	const Fvector det_vector = Fvector().sub(center_pos, start_pos.c);

	if (door_dir.dotproduct(det_vector) < 0.f)
	{
		door_dir.invert();
		door_dir_local.invert();
	}

	const SJointIKData &joint = bd.IK_data;

	if (joint.type != jtJoint)
		return false;
	const Fvector2& limits = joint.limits[1].limit;

	if (M_PI - limits.y < EPS && M_PI + limits.x < EPS)
		return false;

	Fmatrix to_hi = Fmatrix().rotateY(-limits.x);
	to_hi.transform_dir(open, door_dir_local);

	Fmatrix to_lo = Fmatrix().rotateY(-limits.y);
	to_lo.transform_dir(closed, door_dir_local);

	start_pos.transform_dir(open);
	start_pos.transform_dir(closed);

#ifdef	DEBUG
	if (dbg_draw_doors)
	{
		DBG_DrawMatrix(Fidentity, 10.0f);

		DBG_DrawMatrix(XFORM(), .5f, 100);

		DBG_DrawMatrix(start_pos, 0.2f, 100);

		const Fvector pos = start_pos.c.add(Fvector().set(0, 0.2f, 0));
		const Fvector pos1 = start_pos.c.add(Fvector().set(0, 0.3f, 0));

		DBG_DrawLine(pos, Fvector().add(pos, open), D3DCOLOR_XRGB(0, 255, 0));
		DBG_DrawLine(pos, Fvector().add(pos, closed), D3DCOLOR_XRGB(255, 0, 0));

		DBG_DrawLine(pos1, Fvector().add(pos1, det_vector), D3DCOLOR_XRGB(255, 255, 0));
	}
#endif
	return true;
}

// For CDestroyablePhysicsObject
#include "hit_immunity.h"
#include "damage_manager.h"
#include "PHDestroyable.h"
#include "PHCollisionDamageReceiver.h"
#include "DestroyablePhysicsObject.h"

#include "luabind/luabind.hpp"
using namespace luabind;

#pragma optimize("gyts",on)
void CPhysicObject::script_register(lua_State *L)
{
	module(L)
	[
		class_<CDestroyablePhysicsObject,CGameObject>("CDestroyablePhysicsObject")
			.def(constructor<>()),

		class_<CPhysicObject,CGameObject>("CPhysicObject")
			.def(constructor<>())
			.def("run_anim_forward",				&CPhysicObject::run_anim_forward)
			.def("run_anim_back",					&CPhysicObject::run_anim_back)
			.def("stop_anim",						&CPhysicObject::stop_anim)
			.def("anim_time_get",					&CPhysicObject::anim_time_get)
			.def("anim_time_set",					&CPhysicObject::anim_time_set)
			.def("play_bones_sound",				&CPhysicObject::play_bones_sound)
			.def("stop_bones_sound",				&CPhysicObject::stop_bones_sound)
			.def("set_door_ignore_dynamics",		&CPhysicObject::set_door_ignore_dynamics)
			.def("unset_door_ignore_dynamics",		&CPhysicObject::unset_door_ignore_dynamics)
	];
}
