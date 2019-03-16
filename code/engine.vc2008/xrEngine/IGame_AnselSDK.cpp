#include "stdafx.h"
#include "IGame_AnselSDK.h"

CGameAnsel* pGameAnsel = new CGameAnsel();

CGameAnsel::CGameAnsel() : isActive(false), offsetX(0.f), offsetY(0.f), viewportNear(VIEWPORT_NEAR)
{

}