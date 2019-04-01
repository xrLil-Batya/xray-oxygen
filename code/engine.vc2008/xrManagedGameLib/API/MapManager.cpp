#include "stdafx.h"
#include "MapManager.h"

#include "Level.h"


#include "../xrGame/UIGame.h"
#include "../xrGame/map_location.h"
#include "../xrGame/map_manager.h"

XRay::MapManager::MapManager()
{

}


void XRay::MapManager::HideIndicators()
{
	if (((CUIGame*)UIDialogWnd::GetGameUI().ToPointer()))
	{
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->HideShownDialogs();
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowGameIndicators(false);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowCrosshair(false);
	}
}
void XRay::MapManager::HideIndicatorsSafe()
{
	if ((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())
	{
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowGameIndicators(false);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowCrosshair(false);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->OnExternalHideIndicators();
	}
}
void XRay::MapManager::ShowIndicators()
{
	if (((CUIGame*)UIDialogWnd::GetGameUI().ToPointer()))
	{
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowGameIndicators(true);
		((CUIGame*)UIDialogWnd::GetGameUI().ToPointer())->ShowCrosshair(true);
	}
}

void XRay::MapManager::MapAddObjectSpot(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().AddMapLocation(spot_type, id));
	if (xr_strlen(text))
	{
		ml->SetHint(text);
	}
}
void XRay::MapManager::MapAddObjectSpotSer(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().AddMapLocation(spot_type, id));
	if (xr_strlen(text))
		ml->SetHint(text);

	ml->SetSerializable(true);
}
void XRay::MapManager::MapChangeSpotHint(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().GetMapLocation(spot_type, id));
	if (!ml)				return;
	ml->SetHint(text);
}
void XRay::MapManager::MapRemoveObjectSpot(u16 id, LPCSTR spot_type)
{
	(::Level().MapManager().RemoveMapLocation(spot_type, id));
}
bool XRay::MapManager::MapHasObjectSpot(u16 id, LPCSTR spot_type)
{
	return (::Level().MapManager().HasMapLocation(spot_type, id));
}