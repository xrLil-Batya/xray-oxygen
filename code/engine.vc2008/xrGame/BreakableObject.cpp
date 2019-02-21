#include "stdafx.h"
#include "BreakableObject.h"
#include "../xrServerEntities/xrMessages.h"
#include "../xrphysics/IPHStaticGeomShell.h"
#include "../xrphysics/PhysicsShell.h"
#include "../xrEngine/xr_collide_form.h"
#include "../Include/xrRender/Kinematics.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
u32		CBreakableObject::m_remove_time = 0;
float	CBreakableObject::m_damage_threshold = 5.f;
float	CBreakableObject::m_health_threshhold = 0.f;
float CBreakableObject::m_immunity_factor = 0.1f;

CBreakableObject::CBreakableObject()
{
	Init();
}

CBreakableObject::~CBreakableObject()
{
}

void CBreakableObject::Load(LPCSTR section)
{
	inherited::Load(section);
	m_remove_time = pSettings->r_u32(section, "remove_time") * 1000;
	m_health_threshhold = pSettings->r_float(section, "hit_break_threthhold");
	m_damage_threshold = pSettings->r_float(section, "collision_break_threthhold");
	m_immunity_factor = pSettings->r_float(section, "immunity_factor");
	this->shedule.t_min = 1000;
	this->shedule.t_max = 1000;
}

#include "xrServer_Objects_ALife.h"
BOOL CBreakableObject::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract			*e = (CSE_Abstract*)(DC);
	CSE_ALifeObjectBreakable *obj = imdexlib::fast_dynamic_cast<CSE_ALifeObjectBreakable*>(e);
	R_ASSERT(obj);
	inherited::net_Spawn(DC);
	VERIFY(!collidable.model);
	collidable.model = xr_new<CCF_Skeleton>(this);
	// set bone id
	R_ASSERT(Visual() && imdexlib::fast_dynamic_cast<IKinematics*>(Visual()));
	fHealth = obj->m_health;
	processing_deactivate();
	setVisible(TRUE);
	setEnabled(TRUE);
	CreateUnbroken();
	bRemoved = false;
	return (TRUE);
}

void CBreakableObject::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	if (m_pPhysicsShell && !bRemoved&&Device.dwTimeGlobal - m_break_time>m_remove_time) SendDestroy();
}

void CBreakableObject::UpdateCL()
{
	inherited::UpdateCL();
	if (m_pPhysicsShell&&m_pPhysicsShell->isFullActive())m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());
}

void CBreakableObject::enable_notificate()
{
	if (b_resived_damage)ProcessDamage();
}

void	CBreakableObject::Hit(SHit* pHDS)
{
	CheckHitBreak(pHDS->damage(), pHDS->hit_type);
	if (m_pPhysicsShell)
	{
		if (pHDS->hit_type == ALife::eHitTypeExplosion)
		{
			ApplyExplosion(pHDS->dir, pHDS->impulse);
		}
		else
		{
			//. hack: slipch ???
			if ((pHDS->impulse>EPS) && (BI_NONE != pHDS->bone()))
				m_pPhysicsShell->applyImpulseTrace(pHDS->p_in_bone_space, pHDS->dir, pHDS->impulse, pHDS->bone());
		}
	}
}

void CBreakableObject::net_Export(NET_Packet& P)
{
	VERIFY(Local());
}

BOOL CBreakableObject::UsedAI_Locations()
{
	return					(FALSE);
}

void CBreakableObject::CreateUnbroken()
{
	m_pUnbrokenObject = P_BuildStaticGeomShell((this), BreakableObjectCollisionCallback);
}
void CBreakableObject::DestroyUnbroken()
{
	DestroyStaticGeomShell(m_pUnbrokenObject);
}

void CBreakableObject::CreateBroken()
{
	phys_shell_verify_object_model(*this);
	processing_activate();
	m_Shell = P_create_splited_Shell();
	m_Shell->preBuild_FromKinematics(imdexlib::fast_dynamic_cast<IKinematics*>(Visual()));
	m_Shell->mXFORM.set(XFORM());

	m_Shell->set_PhysicsRefObject(this);
	m_Shell->Build();
	m_Shell->setMass(m_Shell->getMass()*0.1f*100.f);

	m_Shell->MassAddBox(m_Shell->getMass() / 100.f, Fvector().set(1, 1, 1));
	m_Shell->SmoothElementsInertia(0.3f);
	Fobb b;
	Visual()->getVisData().box.getradius(b.m_halfsize);
	m_Shell->SetMaxAABBRadius(std::max(std::max(b.m_halfsize.x, b.m_halfsize.y), b.m_halfsize.z)*2.f);//+2.f

}

void CBreakableObject::ActivateBroken()
{
	m_pPhysicsShell = m_Shell;
	IKinematics* K = imdexlib::fast_dynamic_cast<IKinematics*>(Visual());
	m_pPhysicsShell->set_Kinematics(K);
	m_pPhysicsShell->RunSimulation();
	m_pPhysicsShell->SetCallbacks();
	K->CalculateBones_Invalidate();
	K->CalculateBones(TRUE);
	m_pPhysicsShell->GetGlobalTransformDynamic(&XFORM());
}

void CBreakableObject::net_Destroy()
{
	DestroyUnbroken();
	if (m_Shell)
	{
		m_Shell->Deactivate();
		xr_delete(m_Shell);
		SheduleUnregister();
	}

	m_pPhysicsShell = nullptr;
	inherited::net_Destroy();
	xr_delete(collidable.model);
	Init();
	Render->model_Delete(renderable.visual, TRUE);
	cNameVisual_set("");
}

void CBreakableObject::Break()
{
	if (m_pPhysicsShell)return;
	DestroyUnbroken();
	CreateBroken();
	ActivateBroken();
	u16 el_num = m_pPhysicsShell->get_ElementsNumber();
	for (u16 i = 0; i<el_num; i++)
	{
		Fvector pos, dir;
		pos.set(Random.randF(-0.3f, 0.3f), Random.randF(-0.3f, 0.3f), Random.randF(-0.3f, 0.3f));
		dir.set(Random.randF(-0.3f, 0.3f), Random.randF(-0.3f, 0.3f), Random.randF(-0.3f, 0.3f));
		dir.normalize();
		m_pPhysicsShell->get_ElementByStoreOrder(i)->applyImpulseTrace(pos, dir, Random.randF(0.5f, 3.f), 0);
	}
	m_break_time = Device.dwTimeGlobal;
	SheduleRegister();
}

void CBreakableObject::SendDestroy()
{
	if (Local())	DestroyObject();
	bRemoved = true;
}

void CBreakableObject::CollisionHit(u16 source_id, u16 bone_id, float c_damage, const Fvector &dir, Fvector &pos)
{
	VERIFY(source_id == u16(-1));
	VERIFY(bone_id == u16(-1));
	VERIFY(m_pUnbrokenObject);

	if (m_damage_threshold<c_damage&&
		m_max_frame_damage<c_damage
		) {
		b_resived_damage = true;
		m_max_frame_damage = c_damage;
		m_contact_damage_pos.set(pos);
		m_contact_damage_dir.set(dir);
	}
}

void CBreakableObject::ProcessDamage()
{
	NET_Packet			P;
	SHit				HS;
	HS.GenHeader(GE_HIT, ID());
	HS.whoID = (ID());
	HS.weaponID = (ID());
	HS.dir = (m_contact_damage_dir);
	HS.power = (m_max_frame_damage);
	HS.boneID = (PKinematics(Visual())->LL_GetBoneRoot());
	HS.p_in_bone_space = (m_contact_damage_pos);
	HS.impulse = (0.f);
	HS.hit_type = (ALife::eHitTypeStrike);
	HS.Write_Packet(P);

	u_EventSend(P);

	m_max_frame_damage = 0.f;
	b_resived_damage = false;
}

void CBreakableObject::CheckHitBreak(float power, ALife::EHitType hit_type)
{
	if (hit_type != ALife::eHitTypeStrike)
	{
		float res_power = power * m_immunity_factor;
		if (power>m_health_threshhold) fHealth -= res_power;
	}
	if (fHealth <= 0.f)
	{
		Break(); return;
	}

	if (hit_type == ALife::eHitTypeStrike)Break();
}

void CBreakableObject::ApplyExplosion(const Fvector &dir, float impulse)
{
	if (!m_pPhysicsShell) return;
	Fvector pos; pos.set(0.f, 0.f, 0.f);
	u16 el_num = m_pPhysicsShell->get_ElementsNumber();
	for (u16 i = 0; i<el_num; i++)
	{
		Fvector max_area_dir;
		IPhysicsElementEx* element = m_pPhysicsShell->get_ElementByStoreOrder(i);
		element->get_MaxAreaDir(max_area_dir);
		float	sign = max_area_dir.dotproduct(dir)>0.f ? 1.f : -1.f;
		max_area_dir.mul(sign);
		element->applyImpulseTrace(pos, max_area_dir, impulse / el_num, 0);
	}
}

void CBreakableObject::Init()
{
	fHealth = 1.f;
	m_pUnbrokenObject = nullptr;
	m_Shell = nullptr;
	bRemoved = false;
	m_max_frame_damage = 0.f;
	b_resived_damage = false;
}

#include <luabind\luabind.hpp>
using namespace luabind;
#pragma optimize("gyts",on)

void CBreakableObject::script_register(lua_State *L)
{
	module(L)
	[
		class_<CBreakableObject, CGameObject>("CBreakableObject")
		.def(constructor<>())
	];
}