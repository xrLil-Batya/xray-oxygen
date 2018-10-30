#include "stdafx.h"
#include "GamePersistence.h"
#include "../xrEngine/IGame_Persistent.h"

XRay::CGamePersistence::CGamePersistence() : NativeObject((IntPtr)g_pGamePersistent)
{
	pNativeObject = g_pGamePersistent;
}

XRay::CGamePersistence::CGamePersistence(IntPtr InNativeObject) : NativeObject(InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(IGame_Persistent, InNativeObject);
}

void XRay::CGamePersistence::shedule_update(u32 interval)
{

}
