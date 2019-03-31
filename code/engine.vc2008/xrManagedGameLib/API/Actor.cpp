#include "stdafx.h"
#include "Actor.h"
#include "../xrGame/Actor.h"
#include "../xrGame/ActorCondition.h"

XRay::Actor::Actor(IntPtr InNativeObject)
	:EntityAlive(InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CActor, InNativeObject);

	inventory = (Inventory^)Inventory::Create(InNativeObject, Inventory::typeid);

	AddVirtualMethod("TradeStart");
	AddVirtualMethod("TradeStop");
	AddVirtualMethod("TradeSellBuyItem");
	AddVirtualMethod("TradePerformOperation");

	AddVirtualMethod("TraderGlobalAnimRequest");
	AddVirtualMethod("TraderHeadAnimRequest");
	AddVirtualMethod("TraderSoundEend");

	AddVirtualMethod("ZoneEnter");
	AddVirtualMethod("ZoneExit");

	AddVirtualMethod("UseObject");
}

XRay::Actor::Actor() : Actor(XRay::ClassRegistrator::GetFactoryTarget())
{

}

void XRay::Actor::bInputActive::set(bool bVal)
{
}

bool XRay::Actor::bInputActive::get()
{
	return false;
}

float XRay::Actor::Satiety::get()
{
	return ::Actor()->conditions().GetSatiety();
}

void XRay::Actor::Satiety::set(float fNewValue)
{
	::Actor()->conditions().ChangeSatiety(fNewValue);
}
////////////////////////////////////////////////////
//			Callbacks
////////////////////////////////////////////////////

void XRay::Actor::UseObject(Object ^ pObj)
{
}

void XRay::Actor::TradeStart()
{

}

void XRay::Actor::TradeStop()
{
}

void XRay::Actor::TradeSellBuyItem()
{

}

void XRay::Actor::TradePerformOperation()
{

}

void XRay::Actor::TraderGlobalAnimRequest()
{

}

void XRay::Actor::TraderHeadAnimRequest()
{

}

void XRay::Actor::TraderSoundEend()
{

}

void XRay::Actor::ZoneEnter()
{

}

void XRay::Actor::ZoneExit()
{

}