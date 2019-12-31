#include "stdafx.h"
#include "physics_shell_animated.h"

#include "../xrPhysics/PhysicsShell.h"
#include "../Include/xrRender/Kinematics.h"

CPhShellAnimated::CPhShellAnimated(CPhysicsShellHolder* O, bool _update_velocity) : update_velocity(_update_velocity)
{
	create_shell(O);
}

bool CPhShellAnimated::update(const Fmatrix &xrorm)
{
	VERIFY(physics_shell);
	
	if(!physics_shell)
		return false;
	
	if (update_velocity)
		physics_shell->AnimToVelocityState(Device.fTimeDelta, default_l_limit * 10, default_w_limit * 10);

	physics_shell->mXFORM.set(xrorm);
	physics_shell->PKinematics()->CalculateBones();
	physics_shell->ToAnimBonesPositions(mh_unspecified);
	return true;
}

CPhShellAnimated::~CPhShellAnimated()
{
	destroy_physics_shell(physics_shell);
}

void CPhShellAnimated::create_shell(CPhysicsShellHolder* O)
{
	physics_shell = P_build_Shell((IPhysicsShellHolder*)(O), true, (BONE_P_MAP*)0, true);
	physics_shell->ToAnimBonesPositions(mh_unspecified);
	physics_shell->DisableCollision();
	physics_shell->SetAnimated(true);
}