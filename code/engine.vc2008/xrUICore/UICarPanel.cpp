#include "stdafx.h"
#include "UICarPanel.h"
#include "UIXmlInit.h"

constexpr LPCSTR CAR_PANEL_XML = "car_panel.xml";
constexpr LPCSTR POINTER_ARROW_TEX = "ui\\hud_map_arrow";
static CXml* uiXml;

void CUICarPanel::Init(float x, float y, float width, float height)
{
	uiXml = xr_new<CXml>();
	uiXml->Load(CONFIG_PATH, UI_PATH, CAR_PANEL_XML);

	CUIXmlInit	xml_init;
	////////////////////////////////////////////////////////////////////
	AttachChild(&UIStaticCarHealth);
	xml_init.InitStatic(*uiXml, "car_health_static", 0, &UIStaticCarHealth);

	UIStaticCarHealth.AttachChild(&UICarHealthBar);
	xml_init.InitProgressBar(*uiXml, "car_health_progress_bar", 0, &UICarHealthBar);

	Show(false);
	Enable(false);

	inherited::SetWndPos(Fvector2({ x, y }));
	inherited::SetWidth(width);
	inherited::SetHeight(height);
}

//////////////////////////////////////////////////////////////////////////
void CUICarPanel::SetCarHealth(float value)
{
	float pos = value * 100;
	clamp(pos, 0.0f, 100.0f);
	UICarHealthBar.SetProgressPos(pos);
}
//////////////////////////////////////////////////////////////////////////
void CUICarPanel::SetSpeed(float speed)
{
	//	clamp(speed,0.f,1.f);
	//	UISpeedometer.SetValue(speed);
}

//////////////////////////////////////////////////////////////////////////

void CUICarPanel::SetRPM(float rpm)
{
	//	clamp(rpm,0.f,1.f);
	//	UITachometer.SetValue(rpm);
}