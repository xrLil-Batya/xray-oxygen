////////////////////////////////////////////////////////////////////////////
//  Created     : 19.06.2018
//  Authors     : Xottab_DUTY (OpenXRay Project)
//                FozeSt
//                Unfainthful (Oxygen Project)
//
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../xrEngine/ILoadingScreen.h"
#include "UIStatic.h"
#include "UIProgressBar.h"
#include "UIWindow.h"

class CApplication;

class UI_API UILoadingScreen : public ILoadingScreen, public CUIWindow
{
	CUIStatic* loadingProgressBackground;
	CUIStatic* engineVersion;
	CUIStatic* loadingLogo;
	CUIStatic* loadingProgressPercent;

    CUIStatic* loadingStage;
    CUIStatic* loadingHeader;
    CUIStatic* loadingTipNumber;
    CUIStatic* loadingTip;
	CUIProgressBar* loadingProgress;

public:
	UILoadingScreen();

	void Initialize() override;

	void Update(const int stagesCompleted, const int stagesTotal) override;
	void ForceFinish() override;

	void SetLevelLogo(const char* name) const override;
	void SetStageTitle(const char* title) const override;
	void SetStageTip(const char* header, const char* tipNumber, const char* tip) const override;
};