#include "stdafx.h"
#include "Entity.h"

XRay::Entity::Entity(IntPtr InNativeObject)
	: PhysicalGameObject(InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CEntity, InNativeObject);
}

void XRay::Entity::ChangeTeam(int team, int squad, int group)
{
	pNativeObject->ChangeTeam(team, squad, group);
}
