#include "stdafx.h"
#include "physics_shell_scripted.h"
#include "physics_element_scripted.h"
#include "physics_joint_scripted.h"
#include <luabind\luabind.hpp>

using namespace luabind;

cphysics_element_scripted*	cphysics_shell_scripted::get_Element(LPCSTR bone_name)
{
	IPhysicsElementEx* E = physics_impl().get_Element(bone_name);
	if (!E)
		return 0;
	return get_script_wrapper<cphysics_element_scripted>(*E);
}
cphysics_element_scripted*	cphysics_shell_scripted::get_Element(u16 bone_id)
{
	IPhysicsElementEx* E = physics_impl().get_Element(bone_id);
	if (!E)
		return 0;
	return get_script_wrapper<cphysics_element_scripted>(*E);
}

cphysics_element_scripted*	cphysics_shell_scripted::get_ElementByStoreOrder(u16 idx)
{
	IPhysicsElementEx* E = physics_impl().get_ElementByStoreOrder(idx);
	R_ASSERT(E);
	return get_script_wrapper<cphysics_element_scripted>(*E);
}

cphysics_joint_scripted	*cphysics_shell_scripted::get_Joint(LPCSTR bone_name)
{
	IPhysicsJoint* J = physics_impl().get_Joint(bone_name);
	if (!J)
		return 0;
	return get_script_wrapper<cphysics_joint_scripted>(*J);
}

cphysics_joint_scripted	*cphysics_shell_scripted::get_Joint(u16 bone_id)
{
	IPhysicsJoint* J = physics_impl().get_Joint(bone_id);
	if (!J)
		return 0;
	return get_script_wrapper<cphysics_joint_scripted>(*J);
}
cphysics_joint_scripted	*cphysics_shell_scripted::get_JointByStoreOrder(u16 idx)
{
	IPhysicsJoint* J = physics_impl().get_JointByStoreOrder(idx);
	R_ASSERT(J);
	return get_script_wrapper<cphysics_joint_scripted>(*J);
}

#pragma optimize("s",on)
void cphysics_shell_scripted::script_register(lua_State *L)
{
	module(L)
		[
			class_<cphysics_shell_scripted>("physics_shell")
			.def("apply_force", (void (cphysics_shell_scripted::*)(float, float, float))(&cphysics_shell_scripted::applyForce))
		.def("get_element_by_bone_name", (cphysics_element_scripted*(cphysics_shell_scripted::*)(LPCSTR))(&cphysics_shell_scripted::get_Element))
		.def("get_element_by_bone_id", (cphysics_element_scripted*(cphysics_shell_scripted::*)(u16))(&cphysics_shell_scripted::get_Element))
		.def("get_element_by_order", (cphysics_element_scripted*(cphysics_shell_scripted::*)(u16))(&cphysics_shell_scripted::get_ElementByStoreOrder))
		.def("get_elements_number", &cphysics_shell_scripted::get_ElementsNumber)
		.def("get_joint_by_bone_name", (cphysics_joint_scripted*(cphysics_shell_scripted::*)(LPCSTR))(&cphysics_shell_scripted::get_Joint))
		.def("get_joint_by_bone_id", (cphysics_joint_scripted*(cphysics_shell_scripted::*)(u16))(&cphysics_shell_scripted::get_Joint))
		.def("get_joint_by_order", &cphysics_shell_scripted::get_JointByStoreOrder)
		.def("get_joints_number", &cphysics_shell_scripted::get_JointsNumber)
		.def("block_breaking", &cphysics_shell_scripted::BlockBreaking)
		.def("unblock_breaking", &cphysics_shell_scripted::UnblockBreaking)
		.def("is_breaking_blocked", &cphysics_shell_scripted::IsBreakingBlocked)
		.def("is_breakable", &cphysics_shell_scripted::isBreakable)
		.def("get_linear_vel", &cphysics_shell_scripted::get_LinearVel)
		.def("get_angular_vel", &cphysics_shell_scripted::get_AngularVel)
		];
}