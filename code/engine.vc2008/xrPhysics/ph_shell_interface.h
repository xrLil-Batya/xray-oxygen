#pragma once
#define _interface class __declspec(novtable)
_interface IPhysicShellCreator
{
public:
	IPhysicShellCreator() {}
	virtual void CreatePhysicsShell() = 0;
};

