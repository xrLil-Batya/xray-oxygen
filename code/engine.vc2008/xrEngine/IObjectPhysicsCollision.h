#pragma once

class IPhysicsShell;
class IPhysicsElement;
xr_interface IObjectPhysicsCollision
{
public:
	virtual	const IPhysicsShell		*physics_shell		()const = 0;

	[[deprecated]]
	virtual const IPhysicsElement	*physics_character	()const = 0;
};
