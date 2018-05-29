#pragma once

#include "../xrengine/bone.h"
#include "PHJoint.h"

static const Fvector 	X = { 1, 0, 0 };
static const Fvector 	Y = { 0, 1, 0 };
static const Fvector 	Z = { 0, 0, 1 };
static const Fvector	basis[3] = { X, Y, Z };

inline void SetJoint(IPhysicsJoint	&J, const SJointIKData& joint_data)
{
	J.SetAnchorVsSecondElement(0, 0, 0);
	J.SetJointSDfactors(joint_data.spring_factor, joint_data.damping_factor);
}

inline void SetJointLimit(IPhysicsJoint	&J, const IBoneData &bone_data, u8 limit_num, u8 axis_num)
{
	const SJointIKData& joint_data = bone_data.get_IK_data();
	const SJointLimit&	limit = joint_data.limits[limit_num];
	float lo = bone_data.lo_limit(limit_num);//limit.x;
	float hi = bone_data.hi_limit(limit_num);//limit.y;

	J.SetLimits(lo, hi, axis_num);
	J.SetAxisSDfactors(limit.spring_factor, limit.damping_factor, axis_num);
}

inline bool IsFreeRLimit(const IBoneData &bone_data, u8 limit_num)
{
	float lo = bone_data.lo_limit(limit_num);//limit.x;
	float hi = bone_data.hi_limit(limit_num);//limit.y;
	return !(hi - lo < M_PI*2.f);
}

inline void SetJointRLimit(IPhysicsJoint	&J, const IBoneData &bone_data, u8 limit_num, u8 axis_num)
{
	if (!IsFreeRLimit(bone_data, limit_num))
	{
		SetJointLimit(J, bone_data, limit_num, axis_num);
	}
}

inline IPhysicsJoint	*CtreateHinge(const IBoneData &bone_data, u8 limit_num, IPhysicsElementEx* root_e, IPhysicsElementEx* E)
{
	u8 axis_num = 0;
	const Fvector axis = basis[limit_num];

	const SJointIKData& joint_data = bone_data.get_IK_data();
	IPhysicsJoint	* J = P_create_Joint(IPhysicsJoint::hinge, root_e, E);

	SetJoint(*J, joint_data);

	J->SetAxisDirVsSecondElement(axis.x, axis.y, axis.z, axis_num);

	SetJointLimit(*J, bone_data, limit_num, axis_num);
	return J;
}

inline IPhysicsJoint	*CtreateFullControl(const IBoneData &bone_data, u8 limit_num[3], IPhysicsElementEx* root_e, IPhysicsElementEx* E)
{
	const SJointIKData& joint_data = bone_data.get_IK_data();

	IPhysicsJoint	*J = P_create_Joint(IPhysicsJoint::full_control, root_e, E);
	SetJoint(*J, joint_data);

	const bool set_axis[3] = { true, false, true };
	for (u8 i = 0; i < 3; ++i)
		if (set_axis[i])
			J->SetAxisDirVsSecondElement(basis[limit_num[i]], i);

	for (u8 i = 0; i < 3; ++i)
		SetJointLimit(*J, bone_data, limit_num[i], i);

	return J;
}

inline IPhysicsJoint	*BuildWheelJoint(const IBoneData &bone_data, IPhysicsElementEx* root_e, IPhysicsElementEx* E)
{
	const SJointIKData& joint_data = bone_data.get_IK_data();
	IPhysicsJoint	*J = P_create_Joint(IPhysicsJoint::hinge2, root_e, E);

	SetJoint(*J, joint_data);

	J->SetAxisDirVsSecondElement(1, 0, 0, 0);
	J->SetAxisDirVsSecondElement(0, 0, 1, 1);

	SetJointLimit(*J, bone_data, 0, 0);
	return J;
}
inline IPhysicsJoint	*BuildSliderJoint(const IBoneData &bone_data, IPhysicsElementEx* root_e, IPhysicsElementEx* E)
{
	const SJointIKData& joint_data = bone_data.get_IK_data();
	IPhysicsJoint	*J = P_create_Joint(IPhysicsJoint::slider, root_e, E);

	SetJoint(*J, joint_data);

	J->SetLimits(joint_data.limits[0].limit.x, joint_data.limits[0].limit.y, 0);
	J->SetAxisSDfactors(joint_data.limits[0].spring_factor, joint_data.limits[0].damping_factor, 0);

	SetJointLimit(*J, bone_data, 1, 1);
	return J;
}

inline IPhysicsJoint	*BuildBallJoint(const IBoneData &bone_data, IPhysicsElementEx* root_e, IPhysicsElementEx* E)
{
	const SJointIKData& joint_data = bone_data.get_IK_data();
	IPhysicsJoint	*J = P_create_Joint(IPhysicsJoint::ball, root_e, E);
	SetJoint(*J, joint_data);

	return J;
}

inline IPhysicsJoint	*BuildGenericJoint(const IBoneData &bone_data, IPhysicsElementEx* root_e, IPhysicsElementEx* E)
{
	const SJointIKData& joint_data = bone_data.get_IK_data();

	bool	eqx = !!fsimilar(joint_data.limits[0].limit.x, joint_data.limits[0].limit.y),
		eqy = !!fsimilar(joint_data.limits[1].limit.x, joint_data.limits[1].limit.y),
		eqz = !!fsimilar(joint_data.limits[2].limit.x, joint_data.limits[2].limit.y);

	if (eqx)
	{
		if (eqy)
			return CtreateHinge(bone_data, 2, root_e, E);

		if (eqz)
			return CtreateHinge(bone_data, 1, root_e, E);

		u8 axis_limits[3] = { 2, 0, 1 };
		return CtreateFullControl(bone_data, axis_limits, root_e, E);
	}

	if (eqy)
	{
		if (eqz)
		{
			return CtreateHinge(bone_data, 0, root_e, E);
		}

		u8 axis_limits[3] = { 2, 1, 0 };
		return CtreateFullControl(bone_data, axis_limits, root_e, E);
	}

	if (eqz)
	{
		u8 axis_limits[3] = { 0, 2, 1 };
		return CtreateFullControl(bone_data, axis_limits, root_e, E);
	}

	u8 axis_limits[3] = { 2, 0, 1 };
	return CtreateFullControl(bone_data, axis_limits, root_e, E);

	//return J;
}

inline IPhysicsJoint	*BuildJoint(const IBoneData &bone_data, IPhysicsElementEx* root_e, IPhysicsElementEx* E)
{
	IPhysicsJoint	*J = 0;
	const SJointIKData& joint_data = bone_data.get_IK_data();
	switch (joint_data.type)
	{
	case jtSlider:
		J = BuildSliderJoint(bone_data, root_e, E);
		VERIFY(J);
		break;
	case jtCloth:
		J = BuildBallJoint(bone_data, root_e, E);
		VERIFY(J);
		break;
	case jtJoint:
		J = BuildGenericJoint(bone_data, root_e, E);
		VERIFY(J);
		break;
	case jtWheel:
		J = BuildWheelJoint(bone_data, root_e, E);
		VERIFY(J);
		break;
	case jtNone: break;

	default: NODEFAULT;
	}

	if (J)
		J->SetForceAndVelocity(joint_data.friction);//joint_data.friction

	return J;
}