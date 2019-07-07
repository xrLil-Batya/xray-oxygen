#pragma once
class CUIWindow;
class CUIDialogWnd;
class CUICursor;
class CUIMessageBoxEx;

#include "ui_base.h"
#include "../xrEngine/IInputReceiver.h"
#include "../xrEngine/Render.h"
#include "../xrEngine/IGame_Persistent.h"
#include "UIDialogHolder.h"
#include "UIWndCallback.h"

class UI_API CMainMenu : public IMainMenu, public IInputReceiver, public pureRender, public CDialogHolder, public CUIWndCallback, public CDeviceResetNotifier,
	public pureAppActivate
{
	CUIDialogWnd*		m_startDialog;
	
	enum {
		flRestoreConsole = (1 << 0),
		flRestorePause = (1 << 1),
		flRestorePauseStr = (1 << 2),
		flActive = (1 << 3),
		flNeedChangeCapture = (1 << 4),
		flRestoreCursor = (1 << 5),
		flGameSaveScreenshot = (1 << 6),
		flNeedVidRestart = (1 << 7),
	};
	
	Flags16			m_Flags;
	string_path		m_screenshot_name;
	u32				m_screenshotFrame;
	Fvector2		m_lastLeftThumbstickValue;
	void			ReadTextureInfo();

	xr_vector<CUIWindow*> m_pp_draw_wnds;

protected:
	u32				m_start_time;

	xr_vector<CUIMessageBoxEx*>	m_pMB_ErrDlgs;
	bool			ReloadUI();
public:
	u32				m_deactivated_frame;
	bool			m_activatedScreenRatio;
	virtual void	DestroyInternal(bool bForce);
	CMainMenu();
	virtual			~CMainMenu();

	virtual void	Activate(bool bActive);
	virtual	bool	IsActive();
	virtual	bool	CanSkipSceneRendering();

	virtual bool	IgnorePause() { return true; }

	virtual void	IR_OnMouseMove(int x, int y);
	virtual void	IR_OnThumbstickChanged(GamepadThumbstickType type, const Fvector2& position) override;

	virtual void	IR_OnKeyboardPress(u8 dik);
	virtual void	IR_OnKeyboardRelease(u8 dik);
	virtual void	IR_OnKeyboardHold(u8 dik);

	virtual void	IR_OnMouseWheel(int direction);

	bool			OnRenderPPUI_query();
	void			OnRenderPPUI_main();
	void			OnRenderPPUI_PP();

	virtual void			OnRender();
	virtual void		OnFrame(void);

	virtual bool	UseIndicators() { return false; }

	void			OnDeviceCreate();

	void			Screenshot(IRender_interface::ScreenshotMode mode = IRender_interface::SM_NORMAL, LPCSTR name = 0);
	void			RegisterPPDraw(CUIWindow* w);
	void			UnregisterPPDraw(CUIWindow* w);

	u32				GetEngineBuild();
	const char*		GetEngineBuildDate();

IC	const char*		GetGSVer() { return ENGINE_VERSION; };
	virtual void	OnDeviceReset();

	virtual void OnAppActivate() override;
};

extern UI_API CMainMenu* MainMenu();