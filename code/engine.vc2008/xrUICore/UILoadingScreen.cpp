////////////////////////////////////////////////////////////////////////////
//  Created     : 19.06.2018
//  Authors     : Xottab_DUTY (OpenXRay project)
//                FozeSt
//				  ForserX (Oxygen Project)
//                Unfainthful (Oxygen Project)
//
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "UILoadingScreen.h"
#include "UIXmlInit.h"
#include "xrUIXmlParser.h"
#include "../xrEngine/x_ray.h"
#include "../xrEngine/GameFont.h"
#include "UIHelper.h"
#include "MainMenu.h"

extern ENGINE_API int ps_rs_loading_stages;

UILoadingScreen::UILoadingScreen() : loadingLogo(nullptr), loadingProgress(nullptr), loadingStage(nullptr), loadingHeader(nullptr),
loadingTipNumber(nullptr), loadingTip(nullptr)
{
	UILoadingScreen::Initialize();
}

void UILoadingScreen::Initialize()
{
	CUIXml uiXml;
	uiXml.Load(CONFIG_PATH, UI_PATH, "ui_mm_loading_screen.xml");

	loadingProgressBackground = UIHelper::CreateStatic(uiXml, "loading_progress_background", this);
	loadingProgress = UIHelper::CreateProgressBar(uiXml, "loading_progress", this);

	CUIXmlInit::InitWindow(uiXml, "background", 0, this);

	loadingLogo = UIHelper::CreateStatic(uiXml, "loading_logo", this);
	loadingProgressPercent = UIHelper::CreateStatic(uiXml, "loading_progress_percent", this);
	loadingStage = UIHelper::CreateStatic(uiXml, "loading_stage", this);
	loadingHeader = UIHelper::CreateStatic(uiXml, "loading_header", this);
	loadingTipNumber = UIHelper::CreateStatic(uiXml, "loading_tip_number", this);
	loadingTip = UIHelper::CreateStatic(uiXml, "loading_tip", this);

	if (ps_rs_loading_stages)
	{
		engineVersion = UIHelper::CreateStatic(uiXml, "engine_version", this);
		engineVersion->TextItemControl()->SetText(MainMenu()->GetGSVer());
	}
}

void UILoadingScreen::Update(const int stagesCompleted, const int stagesTotal)
{
	const float progress = float(stagesCompleted) / stagesTotal * loadingProgress->GetRange_max();
	if (loadingProgress->GetProgressPos() < progress)
		loadingProgress->SetProgressPos(progress);

	if (ps_rs_loading_stages)
	{
		char buf[5];
		xr_sprintf(buf, "%.0f%%", loadingProgress->GetProgressPos());
		loadingProgressPercent->TextItemControl()->SetText(buf);
	}

	CUIWindow::Update();
	Draw();
}

void UILoadingScreen::ForceFinish()
{
	loadingProgress->SetProgressPos(loadingProgress->GetRange_max());
}

void UILoadingScreen::SetLevelLogo(const char* name) const
{
	loadingLogo->InitTexture(name);
}

void UILoadingScreen::SetStageTitle(const char* title) const
{
	loadingStage->TextItemControl()->SetText(title);
}

void UILoadingScreen::SetStageTip(const char* header, const char* tipNumber, const char* tip) const
{
	loadingHeader->TextItemControl()->SetText(header);
	loadingTipNumber->TextItemControl()->SetText(tipNumber);
	loadingTip->TextItemControl()->SetText(tip);
}