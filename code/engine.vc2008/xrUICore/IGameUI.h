#pragma once
#include "UIDialogHolder.h"

class game_cl_GameState;
class UI_API IGameUI: public CDialogHolder
{
	bool m_bShowGameIndicators;

public:
	virtual ~IGameUI() = default;

	virtual void OnFrame() {}
	virtual void Render() {}
	virtual void SetClGame(game_cl_GameState*) {}

	virtual void Load() {}
	virtual void UnLoad() {}
	virtual void OnConnected() {}

	virtual void HideShownDialogs() {}
	void ShowGameIndicators(bool b) { m_bShowGameIndicators = b; };
	bool GameIndicatorsShown() { return m_bShowGameIndicators; };
};

extern IGameUI* pUIHud;