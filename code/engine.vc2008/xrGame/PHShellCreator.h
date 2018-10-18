#pragma once
#include "../xrPhysics/ph_shell_interface.h"

class CPHShellSimpleCreator: public IPhysicShellCreator
{
public:
	virtual void CreatePhysicsShell();
	virtual ~CPHShellSimpleCreator() = default;
};
