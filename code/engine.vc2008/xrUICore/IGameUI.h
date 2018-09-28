#pragma once
#include "UIDialogHolder.h"

class UI_API IGameUI: public CDialogHolder
{
	bool m_bShowGameIndicators;

public:
	virtual ~IGameUI() = default;

	virtual void OnFrame() {}
	virtual void Render() {}
	virtual void Load() {}
	virtual void UnLoad() {}
	virtual void OnConnected() {}

	virtual void HideShownDialogs() {}
	void ShowGameIndicators(bool b) { m_bShowGameIndicators = b; };
	bool GameIndicatorsShown() { return m_bShowGameIndicators; };
};

extern UI_API IGameUI* pUIHud;