#pragma once
class IPhysicsShellEx;
class CPhysicsShellHolder;
class XRPHYSICS_API physics_shell_animated
{
protected:
	IPhysicsShellEx * physics_shell;
	bool update_velocity;
public:
	physics_shell_animated(CPhysicsShellHolder* ca, bool	_update_velocity);
	virtual ~physics_shell_animated();

	const	IPhysicsShellEx	*shell()const { return physics_shell; }
	IPhysicsShellEx	*shell() { return physics_shell; }
public:
	bool update(const Fmatrix	&xrorm);
protected:
	virtual	void create_shell(CPhysicsShellHolder* O);
};
