#include "stdafx.h"
#include "MapManager.h"
#include "Level.h"
#include "../xrGame/UIGame.h"
#include "../xrGame/ui/UIMainIngameWnd.h"
#include "../xrGame/ui/UIMotionIcon.h"
#include "../xrGame/HUDManager.h"
#include "../xrGame/map_location.h"
#include "../xrGame/map_manager.h"
#include "../xrGame/ui/UIMotionIcon.h"

using XRay::MapManager;
extern GAME_API CUIMotionIcon* g_pMotionIcon;

void MapManager::Indicators::set(ETypeIndicator eType)
{
	switch (eType)
	{
	case ETypeIndicator::Show:		ShowIndicators(); break;
	case ETypeIndicator::Hide:		HideIndicators(); break;
	case ETypeIndicator::HideSafe:	HideIndicatorsSafe(); break;
	}
}
void MapManager::HideIndicators()
{
	if (((CUIGame*)UIDialogWnd::GetGameUI().ToPointer()))
	{
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->HideShownDialogs();
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowGameIndicators(false);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowCrosshair(false);
	}
}
void MapManager::HideIndicatorsSafe()
{
	if ((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())
	{
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowGameIndicators(false);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowCrosshair(false);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->OnExternalHideIndicators();
	}
}

void MapManager::ShowMinimap::set(bool bShow)
{
	CUIGame* GameUI = HUD().GetGameUI();
	GameUI->UIMainIngameWnd->ShowZoneMap(bShow);
	if (g_pMotionIcon != nullptr)
	{
		g_pMotionIcon->bVisible = bShow;
	}
}

void MapManager::ShowIndicators()
{
	if (((CUIGame*)UIDialogWnd::GetGameUI().ToPointer()))
	{
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowGameIndicators(true);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowCrosshair(true);
	}
}

void MapManager::MapAddObjectSpot(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().AddMapLocation(spot_type, id));
	if (xr_strlen(text))
	{
		ml->SetHint(text);
	}
}
void MapManager::MapAddObjectSpotSer(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().AddMapLocation(spot_type, id));
	if (xr_strlen(text))
		ml->SetHint(text);

	ml->SetSerializable(true);
}
void MapManager::MapChangeSpotHint(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().GetMapLocation(spot_type, id));
	if (!ml)				return;
	ml->SetHint(text);
}
void MapManager::MapRemoveObjectSpot(u16 id, LPCSTR spot_type)
{
	(::Level().MapManager().RemoveMapLocation(spot_type, id));
}
bool MapManager::MapHasObjectSpot(u16 id, LPCSTR spot_type)
{
	return (::Level().MapManager().HasMapLocation(spot_type, id));
}