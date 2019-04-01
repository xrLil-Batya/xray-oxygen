#include "stdafx.h"
#include "Actor.h"
#include "../xrGame/Actor.h"
#include "../xrGame/ActorCondition.h"
#include "../xrGame/CameraEffector.h"
#include "../xrGame/ActorEffector.h"

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

XRay::Actor::Actor() : Actor(XRay::ClassRegistrator::GetFactoryTarget())
{

}

float XRay::Actor::Satiety::get()
{
	return ::Actor()->conditions().GetSatiety();
}

void XRay::Actor::Satiety::set(float fNewValue)
{
	//::Actor()->conditions().ChangeSatiety(fNewValue);
}

float XRay::Actor::AddCamEffector(LPCSTR fn, int id, bool cyclic, LPCSTR cb_func)
{
	CAnimatorCamEffectorScriptCB* e = new CAnimatorCamEffectorScriptCB(cb_func);
	e->SetType((ECamEffectorType)id);
	e->SetCyclic(cyclic);
	e->Start(fn);
	pNativeObject->Cameras().AddCamEffector(e);

	return	e->GetAnimatorLength();
}