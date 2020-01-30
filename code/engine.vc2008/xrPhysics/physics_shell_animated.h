#pragma once

class IPhysicsShellEx;
class CPhysicsShellHolder;

class XRPHYSICS_API CPhShellAnimated
{
protected:
	IPhysicsShellEx * physics_shell;
	bool update_velocity;
	
public:
								CPhShellAnimated(CPhysicsShellHolder* ca, bool bVelocity);
	virtual 					~CPhShellAnimated();

	const	IPhysicsShellEx*	shell() const 	{ return physics_shell; }
			IPhysicsShellEx*	shell() 		{ return physics_shell; }
		
			bool 				update(const Fmatrix &xrorm);
protected:
	virtual	void 				create_shell(CPhysicsShellHolder* O);
};
