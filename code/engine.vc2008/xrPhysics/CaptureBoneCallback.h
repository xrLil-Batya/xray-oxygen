#pragma once;
#include "xrPhysics.h"
#include "../xrphysics/physicsshell.h"
#include "../xrCore/xrCore.h"

struct CPHCaptureBoneCallback : public NearestToPointCallback
{
	virtual	bool operator() (u16 bid) = 0;
	virtual	bool operator() (IPhysicsElementEx* e)
	{
		return (*this) (e->m_SelfID);
	};
};