#pragma once
#include "xrServerEntities/character_info.h"
#include "xrServerEntities/specific_character.h"
#include "xrGame/Actor.h"
#include "API/EntityAlive.h"
#include "API/Inventory.h"

using namespace System;

namespace XRay
{
	public ref class Actor : public EntityAlive
	{
	public:
		Actor();
		Actor(IntPtr InNativeObject);

		Inventory^ inventory;

	private:
		CActor* pNativeObject;
	};
}