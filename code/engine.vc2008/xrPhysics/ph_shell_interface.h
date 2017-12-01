#pragma once

interface IPhysicShellCreator 
{
public:
	IPhysicShellCreator() {}
	virtual void CreatePhysicsShell() = 0;
};

