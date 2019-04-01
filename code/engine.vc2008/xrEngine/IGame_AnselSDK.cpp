#include "stdafx.h"
#include "IGame_AnselSDK.h"

IGameAnsel* pGameAnsel = nullptr;

IGameAnsel::IGameAnsel() : bIsActive(false), offsetX(0.f), offsetY(0.f), viewportNear(VIEWPORT_NEAR)
{}

bool IGameAnsel::IsActive()
{
	if (pGameAnsel != nullptr)
	{
		return pGameAnsel->bIsActive;
	}

	return false;
}
