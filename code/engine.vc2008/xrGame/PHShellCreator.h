#ifndef PHSHELL_CREATOR_H
#define PHSHELL_CREATOR_H

#include "../xrPhysics/ph_shell_interface.h"

class CPHShellSimpleCreator: public IPhysicShellCreator
{
public:
	virtual void CreatePhysicsShell();
protected:
private:
};
#endif