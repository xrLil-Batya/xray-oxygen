/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHShellSplitter.h"
#include "PHFracture.h"
#include "PHJointDestroyInfo.h"
#include "SpaceUtils.h"
#include "MathUtils.h"
#include "iPhysicsShellHolder.h"
#include "../Include/xrRender/Kinematics.h"
#include "PHCollideValidator.h"
#include "../xrengine/bone.h"
///////////////////////////////////////////////////////////////////
#include "ExtendedGeom.h"
#include "PHElement.h"
#include "PHShell.h"
#include "PHCollideValidator.h"
#include "PHElementInline.h"
#include "PhysicsShellAnimator.h"
#include "phshellbuildjoint.h"

#ifdef DEBUG
#include	"debug_output.h"
#endif

IC bool	PhOutOfBoundaries(const Fvector& v)
{
	return v.y < phBoundaries.y1;
}

CPHShell::~CPHShell()
{
	m_pKinematics = nullptr;
	VERIFY(!isActive());
	for (CPHElement* it : elements)
		xr_delete(it);

	elements.clear();

	for (CPHJoint* iter : joints)
		xr_delete(iter);

	joints.clear();

	xr_delete(m_spliter_holder);
}

CPHShell::CPHShell()
{
	m_flags.assign(0);
	m_flags.set(flActivating, FALSE);
	m_flags.set(flActive, FALSE);
	m_space = nullptr;
	m_pKinematics = nullptr;
	m_spliter_holder = nullptr;
	m_object_in_root.identity();
	m_active_count = 0;
	m_pPhysicsShellAnimatorC = nullptr;
}

void CPHShell::EnableObject(CPHObject* obj)
{
	CPHObject::activate();
	if (m_spliter_holder)m_spliter_holder->Activate();
}

void CPHShell::DisableObject()
{
	IPhysicsShellHolder* ref_object = (*elements.begin())->PhysicsRefObject();

	if (ref_object)
		ref_object->on_physics_disable();

	CPHObject::deactivate();
	if (m_spliter_holder)m_spliter_holder->Deactivate();
	if (m_flags.test(flRemoveCharacterCollisionAfterDisable))
		DisableCharacterCollision();
}

void  CPHShell::DisableCharacterCollision()
{
	CPHCollideValidator::SetCharacterClassNotCollide(*this);
}

void CPHShell::Disable()
{
	DisableObject();

	for (CPHElement* it : elements)
	{ 
		it->Disable();
	}

	ClearCashedTries();
}

void CPHShell::DisableCollision()
{
	CPHObject::collision_disable();
}

void CPHShell::EnableCollision()
{
	CPHObject::collision_enable();
}

void CPHShell::ReanableObject()
{
}

void CPHShell::vis_update_activate()
{
	++m_active_count;
	IPhysicsShellHolder* ref_object = (*elements.begin())->PhysicsRefObject();
	if (ref_object&&m_active_count>0)
	{
		m_active_count = 0;
		ref_object->ObjectProcessingActivate();
	}
}

void CPHShell::vis_update_deactivate()
{
	--m_active_count;
}

void CPHShell::setDensity(float M)
{
	for (CPHElement* it : elements)
		it->setDensity(M);
}


void CPHShell::setMass(float M) 
{
	float volume = 0.f; 

	for (CPHElement* it : elements)
		volume += it->get_volume();

	for (CPHElement* it : elements)
		it->setMass(it->get_volume() / volume * M);
}

void CPHShell::setMass1(float M)
{
	for (CPHElement* it : elements)
		it->setMass(M / elements.size());
}

void CPHShell::MassAddBox(float mass, const Fvector &full_size)
{
	dMass m;
	dMassSetBox(&m, mass, full_size.x, full_size.y, full_size.z);
	addEquelInertiaToEls(m);
}

float CPHShell::getMass()
{
	float m = 0.f;

	for (CPHElement* it : elements)
		m += it->getMass();

	return m;
}

void  CPHShell::get_spatial_params()
{
	spatialParsFromDGeom((dGeomID)m_space, spatial.sphere.P, AABB, spatial.sphere.R);
}

float CPHShell::getVolume()
{
	float v = 0.f;

	for (CPHElement* it : elements)
		v += it->getVolume();

	return v;
}

float	CPHShell::getDensity()
{
	return getMass() / getVolume();
}

void CPHShell::PhDataUpdate(dReal step) 
{
	bool disable = true;
	for (CPHElement* it : elements)
	{
		it->PhDataUpdate(step);
		dBodyID body = it->get_body();
		if (body&&disable&&dBodyIsEnabled(body))
			disable = false;
	}

	if (disable)
	{
		DisableObject();
		CPHObject::put_in_recently_deactivated();
	}
	else ReanableObject();

	if (PhOutOfBoundaries(cast_fv(dBodyGetPosition((*elements.begin())->get_body()))))
		Disable();
}

void CPHShell::PhTune(dReal step)
{
	for (CPHElement* it : elements)
		it->PhTune(step);
}

void CPHShell::Update() 
{
	if (!isActive()) return;
	if (m_flags.test(flActivating)) m_flags.set(flActivating, FALSE);

	for (CPHElement* it : elements)
		it->Update();

	mXFORM.set((*elements.begin())->mXFORM);
	VERIFY2(_valid(mXFORM), "invalid position in update");
}

void CPHShell::Freeze()
{
	CPHObject::Freeze();
}

void CPHShell::UnFreeze()
{
	CPHObject::UnFreeze();
}

void CPHShell::FreezeContent()
{
	CPHObject::FreezeContent();
	for (CPHElement* it : elements)
		it->Freeze();
}

void CPHShell::UnFreezeContent()
{
	CPHObject::UnFreezeContent();
	auto i = elements.begin(), e = elements.end();
	for (CPHElement* it : elements)
		it->UnFreeze();

}

void CPHShell::applyForce(const Fvector& dir, float val)
{
	if (!isActive()) return;
	auto i = elements.begin(), e = elements.end();
	val /= getMass();

	for (CPHElement* it : elements)
		it->applyForce(dir, val * it->getMass());

	EnableObject(0);
}

void CPHShell::applyForce(float x, float y, float z)
{
	Fvector dir; dir.set(x, y, z);
	float val = dir.magnitude();
	if (!fis_zero(val))
	{
		dir.mul(1.f / val);
		applyForce(dir, val);
	}
}

void CPHShell::applyImpulse(const Fvector& dir, float val)
{
	if (!isActive()) return;
	(*elements.begin())->applyImpulse(dir, val);
	EnableObject(0);
}

void CPHShell::applyImpulseTrace(const Fvector& pos, const Fvector& dir, float val)
{
	if (!isActive()) return;

	(*elements.begin())->applyImpulseTrace(pos, dir, val, 0);
	EnableObject(0);
}

void CPHShell::applyImpulseTrace(const Fvector& pos, const Fvector& dir, float val, const u16 id)
{
	if (!isActive()) return;
	VERIFY(m_pKinematics);
	CBoneInstance& instance = m_pKinematics->LL_GetBoneInstance(id);
	if (instance.callback_type() != bctPhysics || !instance.callback_param()) return;

	((IPhysicsElementEx*)instance.callback_param())->applyImpulseTrace(pos, dir, val, id);
	EnableObject(nullptr);
}

IPhysicsElementEx* CPHShell::get_Element(const shared_str & bone_name)
{
	VERIFY(m_pKinematics);
	return get_Element(m_pKinematics->LL_BoneID(bone_name));
}

IPhysicsElementEx* CPHShell::get_Element(LPCSTR bone_name)
{
	return get_Element((const shared_str&)(bone_name));
}

IPhysicsElementEx* CPHShell::get_ElementByStoreOrder(u16 num)
{
	R_ASSERT2(num<elements.size(), "argument is out of range");
	return cast_PhysicsElement(elements[num]);
}

const IPhysicsElementEx *CPHShell::get_ElementByStoreOrder(u16 num) const
{
	R_ASSERT2(num<elements.size(), "argument is out of range");
	return cast_PhysicsElement(elements[num]);
}

CPHSynchronize*	CPHShell::get_ElementSync(u16 element)
{
	return smart_cast<CPHSynchronize*>(elements[element]);
}

IPhysicsElementEx* CPHShell::get_PhysicsParrentElement(u16 bone_id)
{
	VERIFY(PKinematics());
	IPhysicsElementEx* E = get_Element(bone_id);
	u16 bid = bone_id;
	while (!E && bid != PKinematics()->LL_GetBoneRoot())
	{
		CBoneData	&bd = PKinematics()->LL_GetData(bid);
		bid = bd.GetParentID();
		E = get_Element(bid);
	}
	return E;
}

IPhysicsElementEx* CPHShell::get_Element(u16 bone_id)
{
	if (m_pKinematics&& isActive())
	{
		CBoneInstance& instance = m_pKinematics->LL_GetBoneInstance(bone_id);
		if (instance.callback() == BonesCallback || instance.callback() == StataticRootBonesCallBack)
		{
			return (instance.callback_type() == bctPhysics) ? static_cast<IPhysicsElementEx*>(instance.callback_param()) : nullptr;
		}
	}

	for (CPHElement* iter: elements)
	{
		if (iter->m_SelfID == bone_id)
			return static_cast<IPhysicsElementEx*>(iter);
	}
	return nullptr;
}

IPhysicsJoint* CPHShell::get_Joint(u16 bone_id)
{
	for (CPHJoint* iter : joints)
	{
		if (iter->BoneID() == bone_id)
			return static_cast<IPhysicsJoint*>(iter);
	}

	return nullptr;
}

IPhysicsJoint* CPHShell::get_Joint(const shared_str &bone_name)
{
	VERIFY(m_pKinematics);
	return get_Joint(m_pKinematics->LL_BoneID(bone_name));
}

IPhysicsJoint* CPHShell::get_Joint(LPCSTR bone_name)
{
	return get_Joint((const shared_str&)bone_name);
}

IPhysicsJoint* CPHShell::get_JointByStoreOrder(u16 num)
{
	R_ASSERT(num < joints.size());
	return (IPhysicsJoint*)joints[num];
}

u16 CPHShell::get_JointsNumber()
{
	return u16(joints.size());
}

void CPHShell::update_root_transforms()
{
	u16 anim_root = PKinematics()->LL_GetBoneRoot();
	u16 phys_root = root_element().m_SelfID;
	VERIFY(BI_NONE != anim_root);
	VERIFY(BI_NONE != phys_root);

	if (anim_root == phys_root)
	{
		mXFORM.set(root_element().mXFORM);
		return;
	}
}

void  CPHShell::BonesCallback(CBoneInstance* B) 
{
	CPHElement*	E = cast_PHElement(B->callback_param());

	E->BonesCallBack(B);
	VERIFY2(_valid(B->mTransform), "CPHShell:: BonesCallback");
}

void  CPHShell::StataticRootBonesCallBack(CBoneInstance* B) 
{
	CPHElement*	E = cast_PHElement(B->callback_param());
	E->StataticRootBonesCallBack(B);
}

void CPHShell::SetTransform(const Fmatrix& m0, motion_history_state history_state)
{
	mXFORM.set(m0);

	for (CPHElement* it : elements)
		it->SetTransform(m0, history_state);

	spatial_move();
}

void CPHShell::Enable()
{
	if (!isActive())
		return;

	for (CPHElement* it : elements)
		it->Enable();

	EnableObject(nullptr);
}

void CPHShell::set_PhysicsRefObject(IPhysicsShellHolder* ref_object)
{
	if (elements.empty()) return;
	if ((*elements.begin())->PhysicsRefObject() == ref_object) return;

	for (CPHElement* it : elements)
		it->set_PhysicsRefObject(ref_object);
}

void CPHShell::set_ContactCallback(ContactCallbackFun* callback)
{
	for (CPHElement* it : elements)
		it->set_ContactCallback(callback);
}

void CPHShell::set_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	for (CPHElement* it : elements)
		it->set_ObjectContactCallback(callback);
}

void CPHShell::add_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	for (CPHElement* it : elements)
		it->add_ObjectContactCallback(callback);
}

void CPHShell::remove_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	for (CPHElement* it : elements)
		it->remove_ObjectContactCallback(callback);
}

void CPHShell::set_CallbackData(void *cd)
{
	for (CPHElement* it : elements)
		it->set_CallbackData(cd);
}

void CPHShell::SetPhObjectInElements()
{
	if (!isActive()) return;
	for (CPHElement* it : elements)
		it->SetPhObjectInGeomData((CPHObject*)this);
}

void CPHShell::SetMaterial(LPCSTR m)
{
	for (CPHElement* it : elements)
		it->SetMaterial(m);
}

void CPHShell::SetMaterial(u16 m)
{
	for (CPHElement* it : elements)
		it->SetMaterial(m);
}

void CPHShell::get_LinearVel(Fvector& velocity) const
{
	(*elements.begin())->get_LinearVel(velocity);
}

void CPHShell::get_AngularVel(Fvector& velocity) const
{
	(*elements.begin())->get_AngularVel(velocity);
}

void CPHShell::set_LinearVel(const Fvector& velocity)
{
	for (CPHElement* it : elements)
		it->set_LinearVel(velocity);
}

void CPHShell::set_AngularVel(const Fvector& velocity)
{
	for (CPHElement* it : elements)
		it->set_AngularVel(velocity);
}

void CPHShell::TransformPosition(const Fmatrix &form, motion_history_state history_state)
{
	for (CPHElement* it : elements) 
		it->TransformPosition(form, history_state);
}

void CPHShell::SetGlTransformDynamic(const Fmatrix &form)
{
	VERIFY(isActive());
	VERIFY(_valid(form));
	Fmatrix current, replace;
	GetGlobalTransformDynamic(&current);
	current.invert();
	replace.mul(form, current);
	TransformPosition(replace, mh_clear);
}

void CPHShell::SmoothElementsInertia(float k)
{
	dMass m_avrg;
	dReal krc = 1.f - k;
	dMassSetZero(&m_avrg);

	for (CPHElement* it : elements)
	{
		dMassAdd(&m_avrg, it->getMassTensor());
	}

	int n = (int)elements.size();
	m_avrg.mass *= k / float(n);
	for (int j = 0; j<4 * 3; ++j) m_avrg.I[j] *= k / float(n);

	for (CPHElement* it : elements)
	{
		dVector3 tmp;
		dMass* m = it->getMassTensor();
		dVectorSet(tmp, m->c);

		m->mass *= krc;
		for (int j = 0; j < 4 * 3; ++j) 
			m->I[j] *= krc;

		dMassAdd(m, &m_avrg);
		dVectorSet(m->c, tmp);
	}
}

void CPHShell::setEquelInertiaForEls(const dMass& M)
{
	for (CPHElement* it : elements)
	{
		it->setInertia(M);
	}
}

void CPHShell::addEquelInertiaToEls(const dMass& M)
{
	for (CPHElement* it: elements)
	{
		it->addInertia(M);
	}
}

static BONE_P_MAP* spGetingMap = nullptr;

void CPHShell::build_FromKinematics(IKinematics* K, BONE_P_MAP* p_geting_map)
{
	VERIFY(K);
	phys_shell_verify_model(*K);
	m_pKinematics = K;
	spGetingMap = p_geting_map;

	if (!m_spliter_holder)
		m_spliter_holder = xr_new<CPHShellSplitterHolder>(this);

	bool vis_check = false;
	AddElementRecursive(0, m_pKinematics->LL_GetBoneRoot(), Fidentity, 0, &vis_check);

	if (m_spliter_holder->isEmpty())
		ClearBreakInfo();
}

void CPHShell::preBuild_FromKinematics(IKinematics* K, BONE_P_MAP* p_geting_map)
{
	VERIFY(K);
	phys_shell_verify_model(*K);
	m_pKinematics = K;
	spGetingMap = p_geting_map;

	if (!m_spliter_holder) 
		m_spliter_holder = xr_new<CPHShellSplitterHolder>(this);

	bool vis_check = false;
	AddElementRecursive(0, m_pKinematics->LL_GetBoneRoot(), Fidentity, 0, &vis_check);
	if (m_spliter_holder->isEmpty())
		ClearBreakInfo();

	m_pKinematics = 0;
}

void CPHShell::ClearBreakInfo()
{
	for (CPHElement* it : elements)
		it->ClearDestroyInfo();

	for (CPHJoint* iter : joints)
		iter->ClearDestroyInfo();

	xr_delete(m_spliter_holder);
}

ICF bool no_physics_shape(const SBoneShape& shape)
{
	return shape.type == SBoneShape::stNone || shape.flags.test(SBoneShape::sfNoPhysics);
}

bool shape_is_physic(const SBoneShape& shape)
{
	return !no_physics_shape(shape);
}

void CPHShell::AddElementRecursive(IPhysicsElementEx* root_e, u16 id, Fmatrix global_parent, u16 element_number, bool* vis_check)
{
	const IBoneData& bone_data = m_pKinematics->GetBoneData(u16(id));
	const SJointIKData& joint_data = bone_data.get_IK_data();

	Fmatrix fm_position;
	fm_position.set(bone_data.get_bind_transform());
	fm_position.mulA_43(global_parent);

	// Set bone visible
	VisMask mask = m_pKinematics->LL_GetBonesVisible();
	const bool no_visible = !mask.is(id);
	bool lvis_check = false;

	if (no_visible)
	{
		u16	num_children = bone_data.GetNumChildren();
		for (u16 i = 0; i < num_children; ++i)
		{
			AddElementRecursive(root_e, bone_data.GetChild(i).GetSelfID(), fm_position, element_number, &lvis_check);
		}
		return;
	}

	IPhysicsElementEx* E = 0;
	IPhysicsJoint*   J = 0;
	bool breakable = joint_data.ik_flags.test(SJointIKData::flBreakable) && root_e 
					 && !(no_physics_shape(bone_data.get_shape()) && joint_data.type == jtRigid);
	///////////////////////////////////////////////////////////////
	lvis_check = (check_obb_sise(bone_data.get_obb()));

	bool *arg_check = vis_check;
	if (breakable || !root_e)
		arg_check = &lvis_check;
	else
		*vis_check = *vis_check || lvis_check;

	//set true when if elemen created and added by this call
	bool element_added = false;

	u16	 splitter_position = 0;
	u16 fracture_num = u16(-1);

	if (!no_physics_shape(bone_data.get_shape()) || !root_e)
	{
		if (joint_data.type == jtRigid && root_e)
		{
			Fmatrix vs_root_position;
			vs_root_position.set(root_e->mXFORM);
			vs_root_position.invert();
			vs_root_position.mulB_43(fm_position);

			E = root_e;
			if (breakable)
			{
				CPHFracture fracture;
				fracture.m_bone_id = id;
				R_ASSERT2(id<64, "ower 64 bones in breacable are not supported");
				fracture.m_start_geom_num = E->numberOfGeoms();
				fracture.m_end_geom_num = u16(-1);
				fracture.m_start_el_num = u16(elements.size());
				fracture.m_start_jt_num = u16(joints.size());
				fracture.MassSetFirst(*(E->getMassTensor()));
				fracture.m_pos_in_element.set(vs_root_position.c);
				VERIFY(u16(-1) != fracture.m_start_geom_num);
				fracture.m_break_force = joint_data.break_force;
				fracture.m_break_torque = joint_data.break_torque;
				root_e->add_Shape(bone_data.get_shape(), vs_root_position);
				root_e->add_Mass(bone_data.get_shape(), vs_root_position, bone_data.get_center_of_mass(), bone_data.get_mass(), &fracture);

				fracture_num = E->setGeomFracturable(fracture);
			}
			else
			{
				root_e->add_Shape(bone_data.get_shape(), vs_root_position);
				root_e->add_Mass(bone_data.get_shape(), vs_root_position, bone_data.get_center_of_mass(), bone_data.get_mass());
			}
		}
		else
		{
			E = P_create_Element();
			E->m_SelfID = id;
			E->mXFORM.set(fm_position);
			E->SetMaterial(bone_data.get_game_mtl_idx());

			E->set_ParentElement(root_e);

			if (!no_physics_shape(bone_data.get_shape())) 
			{
				E->add_Shape(bone_data.get_shape());
				E->setMassMC(bone_data.get_mass(), bone_data.get_center_of_mass());
			}

			element_number = u16(elements.size());
			add_Element(E);
			element_added = true;

			if (root_e)
			{

				J = BuildJoint(bone_data, root_e, E);
				if (J)
				{
					SetJointRootGeom(root_e, J);
					J->SetBoneID(id);
					add_Joint(J);
					if (breakable)
					{
						setEndJointSplitter();
						J->SetBreakable(joint_data.break_force, joint_data.break_torque);
					}
				}
			}
			if (m_spliter_holder)
			{
				splitter_position = u16(m_spliter_holder->m_splitters.size());
			}
		}
	}
	else
	{
		E = root_e;
	}

	if (!no_physics_shape(bone_data.get_shape()))
	{
		CODEGeom* added_geom = E->last_geom();
		if (added_geom)
		{
			added_geom->set_bone_id(id);
			added_geom->set_shape_flags(bone_data.get_shape().flags);
		}
	}
#ifdef DEBUG
	if (E->last_geom())
		VERIFY(E->last_geom()->bone_id() != u16(-1));
#endif
	if (m_spliter_holder&&E->has_geoms())
	{
		m_spliter_holder->AddToGeomMap(std::make_pair(id, E->last_geom()));
	}

	if (spGetingMap)
	{
		const auto c_iter = spGetingMap->find(id);
		if (spGetingMap->end() != c_iter)
		{
			c_iter->second.joint = J;
			c_iter->second.element = E;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
	u16	num_children = bone_data.GetNumChildren();
	for (u16 i = 0; i<num_children; ++i)
		AddElementRecursive(E, bone_data.GetChild(i).GetSelfID(), fm_position, element_number, arg_check);
	/////////////////////////////////////////////////////////////////////////////////////
	if (breakable)
	{
		if (joint_data.type == jtRigid)
		{
			CPHFracture& fracture = E->Fracture(fracture_num);
			fracture.m_bone_id = id;
			fracture.m_end_geom_num = E->numberOfGeoms();
			fracture.m_end_el_num = u16(elements.size());//just after this el = current+1
			fracture.m_end_jt_num = u16(joints.size());	 //current+1

		}
		else if (J)
		{
			J->JointDestroyInfo()->m_end_element = u16(elements.size());
			J->JointDestroyInfo()->m_end_joint = u16(joints.size());
		}
	}

	if (element_added&&E->isBreakable())setElementSplitter(element_number, splitter_position);
}

void CPHShell::ResetCallbacks(u16 id, VisMask &mask)
{
	ResetCallbacksRecursive(id, u16(-1), mask);
}

void CPHShell::ResetCallbacksRecursive(u16 id, u16 element, VisMask &mask)
{
	CBoneInstance& B = m_pKinematics->LL_GetBoneInstance(u16(id));
	const IBoneData& bone_data = m_pKinematics->GetBoneData(u16(id));
	const SJointIKData& joint_data = bone_data.get_IK_data();

	if (mask.is(id))
	{
		if ((no_physics_shape(bone_data.get_shape()) || joint_data.type == jtRigid) && element != u16(-1))
		{
			B.set_callback(bctPhysics, 0, cast_PhysicsElement(elements[element]));
		}
		else
		{
			element++;
			R_ASSERT2(element<elements.size(), "Out of elements!!");
			B.set_callback(bctPhysics, BonesCallback, cast_PhysicsElement(elements[element]));
			B.set_callback_overwrite(TRUE);
		}
	}

	for (u16 i = 0; i < bone_data.GetNumChildren(); ++i)
	{
		ResetCallbacksRecursive(bone_data.GetChild(i).GetSelfID(), element, mask);
	}
}

void CPHShell::EnabledCallbacks(BOOL val)
{
	if (val) 
	{
		SetCallbacks();
		// set callback owervrite in used bones
		for (CPHElement* it : elements)
		{
			CBoneInstance& B = m_pKinematics->LL_GetBoneInstance(it->m_SelfID);
			B.set_callback_overwrite(TRUE);
		}
	}
	else ZeroCallbacks();
}

template< typename T>
void for_each_bone_id(IKinematics &K, T op)
{
	u16 bn = K.LL_BoneCount();
	for (u16 i = 0; i < bn; ++i)
		op(i);
}

CPHElement* get_physics_parent(IKinematics &k, u16 id)
{
	VERIFY(BI_NONE != id);

	while (true)
	{
		CBoneInstance	& B = k.LL_GetBoneInstance(u16(id));
		const IBoneData		& bone_data = k.GetBoneData(u16(id));
		if (B.callback_type() == bctPhysics && B.callback_param())
			return cast_PHElement(B.callback_param());

		if (k.LL_GetBoneRoot() == id)
			return nullptr;

		id = bone_data.GetParentID();

		if (BI_NONE == id)
			return nullptr;
	}
}

static u16 element_position_in_set_calbacks = u16(-1);

void CPHShell::SetCallbacks()
{
	std::for_each(elements.begin(), elements.end(), [](CPHElement* e) { e->SetBoneCallback(); });

	struct set_bone_reference
	{
		IKinematics &K;
		set_bone_reference(IKinematics &K_) : K(K_) {}
		set_bone_reference(set_bone_reference&& other)  : K(other.K) {}
		set_bone_reference(const set_bone_reference& other1) = delete;
		set_bone_reference& operator=(const set_bone_reference& other1) = delete;

		void operator() (u16 id) const
		{
			CBoneInstance &bi = K.LL_GetBoneInstance(id);
			if (!bi.callback() || bi.callback_type() != bctPhysics)
			{
				CPHElement *root_e = get_physics_parent(K, id);
				if (root_e && K.LL_GetBoneVisible(id))
					bi.set_callback(bctPhysics, nullptr, cast_PhysicsElement(root_e));
			}
		}
	};
	for_each_bone_id(*PKinematics(), set_bone_reference(*PKinematics()));
}

void CPHShell::SetCallbacksRecursive(u16 id, u16 element)
{
	VERIFY(false);
	CBoneInstance& B = m_pKinematics->LL_GetBoneInstance(u16(id));
	const IBoneData& bone_data = m_pKinematics->GetBoneData(u16(id));
	const SJointIKData& joint_data = bone_data.get_IK_data();

	VisMask mask = m_pKinematics->LL_GetBonesVisible();
	if (mask.is(id))
	{
		if ((no_physics_shape(bone_data.get_shape()) || joint_data.type == jtRigid) && element != u16(-1)) 
		{
			B.set_callback(bctPhysics, 0, cast_PhysicsElement(elements[element]));
		}
		else 
		{
			element_position_in_set_calbacks++;
			element = element_position_in_set_calbacks;
			R_ASSERT2(element<elements.size(), "Out of elements!!");
			B.set_callback(bctPhysics, BonesCallback, cast_PhysicsElement(elements[element]));
		}
	}

	u16	num_children = bone_data.GetNumChildren();
	for (u16 i = 0; i<num_children; ++i)
		SetCallbacksRecursive(bone_data.GetChild(i).GetSelfID(), element);
}

void CPHShell::ZeroCallbacks()
{
	if (m_pKinematics) ZeroCallbacksRecursive(m_pKinematics->LL_GetBoneRoot());
}

void CPHShell::ZeroCallbacksRecursive(u16 id)
{
	CBoneInstance& B = m_pKinematics->LL_GetBoneInstance(u16(id));
	const IBoneData& bone_data = m_pKinematics->GetBoneData(u16(id));

	if (B.callback_type() == bctPhysics)
	{
		B.reset_callback();
	}

	u16	num_children = bone_data.GetNumChildren();
	for (u16 i = 0; i<num_children; ++i)
		ZeroCallbacksRecursive(bone_data.GetChild(i).GetSelfID());
}

void CPHShell::set_DynamicLimits(float l_limit, float w_limit)
{
	for (CPHElement* it : elements)
		it->set_DynamicLimits(l_limit, w_limit);
}

void CPHShell::set_DynamicScales(float l_scale/* =default_l_scale */, float w_scale/* =default_w_scale */)
{
	for (CPHElement* it : elements)
		it->set_DynamicScales(l_scale, w_scale);
}

void CPHShell::set_DisableParams(const SAllDDOParams& params)
{
	for (CPHElement* it : elements)
		it->set_DisableParams(params);
}

void CPHShell::UpdateRoot()
{
	auto i = elements.begin();
	if (!(*i)->isFullActive()) return;

	(*i)->InterpolateGlobalTransform(&mXFORM);

}

Fmatrix& CPHShell::get_animation_root_matrix(Fmatrix& m)
{
	return m;
}

void CPHShell::InterpolateGlobalTransform(Fmatrix* m)
{
	for (CPHElement* it : elements)
		it->InterpolateGlobalTransform(&it->mXFORM);

	m->set(root_element().mXFORM);

	m->mulB_43(m_object_in_root);

	mXFORM.set(*m);

	VERIFY2(_valid(*m), "not valide transform");

	IPhysicsShellHolder* ref_object = (*elements.begin())->PhysicsRefObject();
	if (ref_object&&m_active_count<0)
	{
		ref_object->ObjectProcessingDeactivate();
		ref_object->ObjectSpatialMove();
		m_active_count = 0;
	}

}

void CPHShell::GetGlobalTransformDynamic(Fmatrix* m)
{
	for (CPHElement* it : elements)
		it->GetGlobalTransformDynamic(&it->mXFORM);

	m->set((*elements.begin())->mXFORM);

	m->mulB_43(m_object_in_root);
	mXFORM.set(*m);

	VERIFY2(_valid(*m), "not valide transform");

}

void CPHShell::InterpolateGlobalPosition(Fvector* v)
{
	(*elements.begin())->InterpolateGlobalPosition(v);

	v->add(m_object_in_root.c);

	VERIFY2(_valid(*v), "not valide result position");
}

void CPHShell::GetGlobalPositionDynamic(Fvector* v)
{
	(*elements.begin())->GetGlobalPositionDynamic(v);
	VERIFY2(_valid(*v), "not valide result position");
}

void CPHShell::ObjectToRootForm(const Fmatrix& form)
{
	Fmatrix M;
	Fmatrix ILF;
	(*elements.begin())->InverceLocalForm(ILF);
	M.mul(m_object_in_root, ILF);
	M.invert();
	mXFORM.mul(form, M);
	VERIFY2(_valid(form), "not valide transform");
}

IPhysicsElementEx* CPHShell::NearestToPoint(const Fvector& point, NearestToPointCallback *cb /*=0*/)
{
	float min_distance = dInfinity;
	CPHElement* nearest_element = nullptr;

	for (CPHElement* it : elements)
	{
		Fvector tmp;
		float	distance;
		if (cb && !(*cb)(it))
			continue;

		it->GetGlobalPositionDynamic(&tmp);
		tmp.sub(point);
		distance = tmp.magnitude();
		if (distance<min_distance)
		{
			min_distance = distance;
			nearest_element = it;
		}
	}
	return nearest_element;
}

void CPHShell::CreateSpace()
{
	if (!m_space)
	{
		m_space = dSimpleSpaceCreate(0);
		dSpaceSetCleanup(m_space, 0);
	}
}

void CPHShell::PassEndElements(u16 from, u16 to, CPHShell *dest)
{
	auto i_from = elements.begin() + from, e = elements.begin() + to;
	if (from != to)
	{
		if (!dest->elements.empty()) (*i_from)->set_ParentElement(dest->elements.back());
		else (*i_from)->set_ParentElement(nullptr);
	}
	for (auto i = i_from; i != e; ++i)
	{
		dGeomID spaced_geom = (*i)->dSpacedGeometry();
		//for active elems
		if (spaced_geom)
		{
			dSpaceRemove(m_space, spaced_geom);
			dSpaceAdd(dest->m_space, spaced_geom);
		}
		VERIFY(_valid(dest->mXFORM));
		(*i)->SetShell(dest);
	}
	dest->elements.insert(dest->elements.end(), i_from, e);
	elements.erase(i_from, e);
}

void CPHShell::PassEndJoints(u16 from, u16 to, CPHShell *dest)
{
	auto i_from = joints.begin() + from, e = joints.begin() + to;
	auto i = i_from;
	for (; i != e; i++)
	{
		(*i)->SetShell(dest);
	}
	dest->joints.insert(dest->joints.end(), i_from, e);
	joints.erase(i_from, e);
}

void CPHShell::DeleteElement(u16 element)
{
	elements[element]->Deactivate();
	xr_delete(elements[element]);
	elements.erase(elements.begin() + element);
}

void CPHShell::DeleteJoint(u16 joint)
{
	joints[joint]->Deactivate();
	xr_delete(joints[joint]);
	joints.erase(joints.begin() + joint);
}

void CPHShell::setEndElementSplitter()
{
	//adding fracture for element supposed before adding splitter. Need only one splitter for an element
	if (!elements.back()->FracturesHolder())
		AddSplitter(CPHShellSplitter::splElement, u16(elements.size() - 1), u16(joints.size() - 1));
}

void CPHShell::setElementSplitter(u16 element_number, u16 splitter_position)
{
	AddSplitter(CPHShellSplitter::splElement, element_number, element_number - 1, splitter_position);
}

void CPHShell::AddSplitter(CPHShellSplitter::EType type, u16 element, u16 joint)
{
	if (!m_spliter_holder) m_spliter_holder = xr_new<CPHShellSplitterHolder>(this);
	m_spliter_holder->AddSplitter(type, element, joint);
}

void CPHShell::AddSplitter(CPHShellSplitter::EType type, u16 element, u16 joint, u16 position)
{
	if (!m_spliter_holder) m_spliter_holder = xr_new<CPHShellSplitterHolder>(this);
	m_spliter_holder->AddSplitter(type, element, joint, position);
}

void CPHShell::setEndJointSplitter()
{
	//setting joint breacable supposed before adding splitter. Need only one splitter for a joint
	if (!joints.back()->JointDestroyInfo())
		AddSplitter(CPHShellSplitter::splJoint, u16(elements.size() - 1), u16(joints.size() - 1));
}

bool CPHShell::isBreakable()
{
	return (m_spliter_holder && !m_spliter_holder->IsUnbreakable());
}

bool CPHShell::isFractured()
{
	return(m_spliter_holder&&m_spliter_holder->Breaked());
}

void CPHShell::SplitProcess(PHSHELL_PAIR_VECTOR &out_shels)
{
	if (!m_spliter_holder) return;
	m_spliter_holder->SplitProcess(out_shels);
	if (!m_spliter_holder->m_splitters.size()) xr_delete(m_spliter_holder);
}

void CPHShell::SplitterHolderActivate()
{
	CPHShellSplitterHolder*sh = SplitterHolder();
	if (sh)
		sh->Activate();
}

void CPHShell::SplitterHolderDeactivate()
{

	CPHShellSplitterHolder*sh = SplitterHolder();
	if (sh)
		sh->Deactivate();
}

u16 CPHShell::BoneIdToRootGeom(u16 id)
{
	
	return (m_spliter_holder) ? m_spliter_holder->FindRootGeom(id) : std::numeric_limits<u16>::max();
}

void CPHShell::SetJointRootGeom(IPhysicsElementEx* root_e, IPhysicsJoint* J)
{
	R_ASSERT(root_e);
	R_ASSERT(J);
	CPHElement* e = cast_PHElement(root_e);
	CPHJoint*	j = static_cast<CPHJoint*>(J);

	CPHFracturesHolder* f_holder = e->FracturesHolder();
	if (!f_holder) return;
	j->RootGeom() = e->Geom(f_holder->LastFracture().m_start_geom_num);
}

void CPHShell::set_ApplyByGravity(bool flag)
{
	for (CPHElement* it : elements)
		it->set_ApplyByGravity(flag);
}

bool CPHShell::get_ApplyByGravity()
{
	if (elements.empty())
		return	(false);

	VERIFY(elements.front());
	return(elements.front()->get_ApplyByGravity());
}

void CPHShell::applyGravityAccel(const Fvector& accel)
{
	if (!isActive())return;
	Fvector a;
	a.set(accel);
	a.mul((float)elements.size());

	for (CPHElement* it : elements)
		it->applyGravityAccel(a);
	EnableObject(0);
}

void CPHShell::PlaceBindToElForms()
{
	VisMask Mask = m_pKinematics->LL_GetBonesVisible();
	PlaceBindToElFormsRecursive(Fidentity, m_pKinematics->LL_GetBoneRoot(), 0, Mask);
}

void CPHShell::setTorque(const Fvector& torque)
{
	for (CPHElement* it : elements)
		it->setTorque(torque);
}

void CPHShell::setForce(const Fvector& force)
{
	for (CPHElement* it : elements)
		it->setForce(force);
}

void CPHShell::PlaceBindToElFormsRecursive(Fmatrix parent, u16 id, u16 element, VisMask &mask)
{
	CBoneData& bone_data = m_pKinematics->LL_GetData(u16(id));
	SJointIKData& joint_data = bone_data.IK_data;

	if (mask.is(id))
	{
		if ((no_physics_shape(bone_data.shape) || joint_data.type == jtRigid) && element != u16(-1))
		{
			/* Non implement */
		}
		else
		{
			element++;
			R_ASSERT2(element<elements.size(), "Out of elements!!");
			CPHElement* E = (elements[element]);
			E->mXFORM.mul(parent, bone_data.bind_transform);
		}
	}

	IBoneData	&ibone_data = bone_data;
	u16	num_children = ibone_data.GetNumChildren();
	for (u16 i = 0; i<num_children; ++i)
		PlaceBindToElFormsRecursive(mXFORM, ibone_data.GetChild(i).GetSelfID(), element, mask);
}

void CPHShell::BonesBindCalculate(u16 id_from)
{
	BonesBindCalculateRecursive(Fidentity, 0);
}

void CPHShell::BonesBindCalculateRecursive(Fmatrix parent, u16 id)
{
	CBoneInstance& bone_instance = m_pKinematics->LL_GetBoneInstance(id);
	CBoneData& bone_data = m_pKinematics->LL_GetData(u16(id));

	bone_instance.mTransform.mul(parent, bone_data.bind_transform);

	IBoneData	&ibone_data = bone_data;
	u16	num_children = ibone_data.GetNumChildren();
	for (u16 i = 0; i<num_children; ++i)
		BonesBindCalculateRecursive(bone_instance.mTransform, ibone_data.GetChild(i).GetSelfID());
}

void CPHShell::AddTracedGeom(u16 element, u16 geom)
{
	CODEGeom* g = elements[element]->Geom(geom);
	g->set_ph_object(this);
	m_traced_geoms.add(g);
	EnableGeomTrace();
}

void CPHShell::SetAllGeomTraced()
{
	auto b = elements.begin();
	auto e = elements.end();
	for (auto i = b; i != e; ++i)
	{
		u16 gn = (*i)->numberOfGeoms();
		for (u16 j = 0; j<gn; ++j)
			AddTracedGeom(u16(i - b), j);
	}

}

void CPHShell::ClearTracedGeoms()
{
	m_traced_geoms.clear();
	DisableGeomTrace();
}

void CPHShell::DisableGeomTrace()
{
	CPHObject::UnsetRayMotions();
}

void CPHShell::EnableGeomTrace()
{
	if (!m_traced_geoms.empty())
		CPHObject::SetRayMotions();
}

void CPHShell::SetPrefereExactIntegration()
{
	CPHObject::SetPrefereExactIntegration();
}

void CPHShell::add_Element(IPhysicsElementEx* E)
{
	CPHElement* ph_element = cast_PHElement(E);
	ph_element->SetShell(this);
	elements.push_back(ph_element);
}

void CPHShell::add_Joint(IPhysicsJoint* J) 
{
	if (!J)return;
	joints.push_back(static_cast<CPHJoint*>(J));
	joints.back()->SetShell(this);
}

CODEGeom* CPHShell::get_GeomByID(u16 bone_id)
{
	for (CPHElement* it : elements)
	{
		CODEGeom* ret = it->GeomByBoneID(bone_id);
		if (ret) return ret;
	}
	return nullptr;
}

void CPHShell::PureStep(float step)
{
	CPHObject::step(step);
}

void CPHShell::CollideAll()
{
	CPHObject::Collide();
	CPHObject::reinit_single();
}

void CPHShell::RegisterToCLGroup(CGID g)
{
	CPHCollideValidator::RegisterObjToGroup(g, *static_cast<CPHObject*>(this));
}

bool CPHShell::IsGroupObject()
{
	return CPHCollideValidator::IsGroupObject(*this);
}

void CPHShell::SetIgnoreStatic()
{
	CPHCollideValidator::SetStaticNotCollide(*this);
}

void CPHShell::SetIgnoreDynamic()
{
	CPHCollideValidator::SetDynamicNotCollide(*this);
}

void CPHShell::SetRagDoll()
{
	CPHCollideValidator::SetRagDollClass(*this);
}

void CPHShell::SetIgnoreRagDoll()
{
	CPHCollideValidator::SetRagDollClassNotCollide(*this);
}

//Делает данный физический объек анимированным 
void CPHShell::CreateShellAnimator(CInifile* ini, LPCSTR section)
{
	//Для фильтра коллизий относим данный объект к классу анимированных
	CPHCollideValidator::SetAnimatedClass(*this);
	m_pPhysicsShellAnimatorC = xr_new<CPhysicsShellAnimator>(this, ini, section);
	VERIFY(PhysicsRefObject());
	PhysicsRefObject()->ObjectProcessingActivate();
	//m_pPhysicsShellAnimatorC->ResetCallbacks();
}

//Настраивает фильтр коллизий на игнорирование столкновенний данного
//физического объекта с анимированным физическим объектом
void CPHShell::SetIgnoreAnimated()
{
	//Для фильтра коллизий указываем, что данный
	//физический объект игнорирует анимированные физические тела

	CPHCollideValidator::SetAnimatedClassNotCollide(*this);
}

//Выдает информацию о том является ли данный объект анимированным
void CPHShell::SetSmall()
{
	CPHCollideValidator::SetClassSmall(*this);
}

void CPHShell::SetIgnoreSmall()
{
	CPHCollideValidator::SetClassSmallNotCollide(*this);
}

void CPHShell::CutVelocity(float l_limit, float a_limit)
{
	for (CPHElement* it : elements)
		it->CutVelocity(l_limit, a_limit);
}

void CPHShell::ClearRecentlyDeactivated()
{
	ClearCashedTries();
}

void CPHShell::ClearCashedTries()
{
	for (CPHElement* it : elements)
		it->clear_cashed_tries();
}

void CPHShell::get_Extensions(const Fvector& axis, float center_prg, float& lo_ext, float& hi_ext) const
{
	t_get_extensions(elements, axis, center_prg, lo_ext, hi_ext);
}

const CGID&	CPHShell::GetCLGroup()const
{
	return CPHCollideValidator::GetGroup(*this);
}

void* CPHShell::get_CallbackData()
{
	VERIFY(isActive());
	return	(*elements.begin())->get_CallbackData();
}

void CPHShell::SetBonesCallbacksOverwrite(bool v)
{
	for (CPHElement* it : elements)
		it->SetBoneCallbackOverwrite(v);
}

void CPHShell::ToAnimBonesPositions(motion_history_state history_state)
{
	VERIFY(PKinematics());
	for (CPHElement* it : elements)
		it->ToBonePos(&PKinematics()->LL_GetBoneInstance(it->m_SelfID), history_state);
}

bool CPHShell::AnimToVelocityState(float dt, float l_limit, float a_limit)
{
	bool ret = true;
	for (CPHElement* it : elements)
		ret = it->AnimToVel(dt, l_limit, a_limit) && ret;

	return ret;
}

void CPHShell::SetAnimated(bool v)
{
	for (CPHElement* it : elements)
		it->SetAnimated(v);
}


void CPHShell::AnimatorOnFrame()
{
	VERIFY(PPhysicsShellAnimator());
	PPhysicsShellAnimator()->OnFrame();
}

void CPHShell::dbg_draw_velocity(float scale, u32 color)
{
	for (CPHElement* it : elements)
		it->dbg_draw_velocity(scale, color);
}

void CPHShell::dbg_draw_force(float scale, u32 color)
{
	for (CPHElement* it : elements)
		it->dbg_draw_force(scale, color);
}

void CPHShell::dbg_draw_geometry(float scale, u32 color, Flags32 flags) const
{
	for (CPHElement* it: elements)
		it->dbg_draw_geometry(scale, color, flags);
}