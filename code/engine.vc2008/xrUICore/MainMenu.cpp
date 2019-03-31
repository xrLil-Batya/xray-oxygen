#include "stdafx.h"
#include "MainMenu.h"
#include "UIDialogWnd.h"
#include "UIMessageBoxEx.h"
#include "../xrEngine/xr_IOConsole.h"
#include "../xrEngine/IGame_Level.h"
#include "../xrEngine/CameraManager.h"
#include "../xrEngine/xr_Level_controller.h"
#include "../xrEngine/xr_input.h"
#include "UITextureMaster.h"
#include "UIXmlInit.h"
#include "UIBtnHint.h"
#include "UICursor.h"
#include "..\xrEngine\string_table.h"
#include "../xrCore/os_clipboard.h"
#include <shellapi.h>
#include "../xrEngine/DiscordRichPresense.h"
#pragma comment(lib, "shell32.lib")

extern bool b_shniaganeed_pp;

static const char* month_id[12] = 
{
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

XRCORE_API u32 build_id;
XRCORE_API const char* build_date;

UI_API CMainMenu* MainMenu() { return (CMainMenu*) g_pGamePersistent->m_pMainMenu; };
//----------------------------------------------------------------------------------
#define INIT_MSGBOX(_box, _template)	{ _box = xr_new<CUIMessageBoxEx>(); _box->InitMessageBox(_template);}
//----------------------------------------------------------------------------------

CMainMenu::CMainMenu()
{
	m_Flags.zero();
	m_lastLeftThumbstickValue.setZero();
	m_startDialog = NULL;
	m_screenshotFrame = u32(-1);
	g_pGamePersistent->m_pMainMenu = this;
	if (Device.b_is_Ready)			OnDeviceCreate();
	ReadTextureInfo();
	CUIXmlInit::InitColorDefs();
	g_btnHint = NULL;
	g_statHint = NULL;
	m_deactivated_frame = 0;

	//-------------------------------------------
	m_start_time = 0;

    g_btnHint = xr_new<CUIButtonHint>();
    g_statHint = xr_new<CUIButtonHint>();

	Device.seqFrame.Add(this, REG_PRIORITY_LOW - 1000);
	Device.seqAppActivate.Add(this, REG_PRIORITY_LOW - 1000);
}

CMainMenu::~CMainMenu()
{
	Device.seqAppActivate.Remove(this);
	Device.seqFrame.Remove(this);
	xr_delete(g_btnHint);
	xr_delete(g_statHint);
	xr_delete(m_startDialog);
	g_pGamePersistent->m_pMainMenu = nullptr;

	for (CUIMessageBoxEx* pMsgBox : m_pMB_ErrDlgs)
		xr_delete(pMsgBox);
	m_pMB_ErrDlgs.clear();

	g_discord.SetStatus(xrDiscordPresense::StatusId::In_Game);
}

void CMainMenu::ReadTextureInfo()
{
	FS_FileSet fset;
	FS.file_list(fset, "$game_config$", FS_ListFiles, "ui\\textures_descr\\*.xml");
	auto fit = fset.begin();
	auto fit_e = fset.end();

	for (; fit != fit_e; ++fit)
	{
		string_path	fn1, fn2, fn3;
		_splitpath((*fit).name.c_str(), fn1, fn2, fn3, 0);
		xr_strcat(fn3, ".xml");

		CUITextureMaster::ParseShTexInfo(fn3);
	}

}

extern ENGINE_API BOOL bShowPauseString;
void CMainMenu::Activate(bool bActivate)
{
	if (!!m_Flags.test(flActive) == bActivate)		return;
	if (m_Flags.test(flGameSaveScreenshot))		return;
	if ((m_screenshotFrame == Device.dwFrame) ||
		(m_screenshotFrame == Device.dwFrame - 1) ||
		(m_screenshotFrame == Device.dwFrame + 1))	return;

	if (bActivate)
	{
		b_shniaganeed_pp = true;
		Device.Pause(TRUE, FALSE, TRUE, "mm_activate1");
		m_Flags.set(flActive | flNeedChangeCapture, TRUE);

		m_Flags.set(flRestoreCursor, GetUICursor().IsVisible());

		if (!ReloadUI())				return;

		m_Flags.set(flRestoreConsole, Console->bVisible);
		m_Flags.set(flRestorePause, Device.Paused());

		Console->Hide();

		m_Flags.set(flRestorePauseStr, bShowPauseString);
		bShowPauseString = FALSE;
		if (!m_Flags.test(flRestorePause))
			Device.Pause(TRUE, TRUE, FALSE, "mm_activate2");

		if (g_pGameLevel)
		{
			Device.seqFrame.Remove(g_pGameLevel);
			Device.seqRender.Remove(g_pGameLevel);
			CCameraManager::ResetPP();
		};
		Device.seqRender.Add(this, 4); // 1-console 2-cursor 3-tutorial

		if (g_pGameLevel == nullptr)
		{
			g_discord.SetStatus(xrDiscordPresense::StatusId::Menu);
		}
		pInput->SetAllowAccessToBorders(true);
		pInput->UnlockMouse();
	}
	else {
		m_deactivated_frame = Device.dwFrame;
		m_Flags.set(flActive, FALSE);
		m_Flags.set(flNeedChangeCapture, TRUE);

		Device.seqRender.Remove(this);

		bool b = !!Console->bVisible;
		if (b) 
		{
			Console->Hide();
		}

		IR_Release();
		
		if(b) Console->Show();

		if (m_startDialog->IsShown())
			m_startDialog->HideDialog();

		CleanInternals();
		if (g_pGameLevel)
		{
			Device.seqFrame.Add(g_pGameLevel);
			Device.seqRender.Add(g_pGameLevel);
		};
		if (m_Flags.test(flRestoreConsole))
			Console->Show();

		if (!m_Flags.test(flRestorePause))
			Device.Pause(FALSE, TRUE, FALSE, "mm_deactivate1");

		bShowPauseString = m_Flags.test(flRestorePauseStr);

		if (m_Flags.test(flRestoreCursor))
			GetUICursor().Show();

		Device.Pause(FALSE, TRUE, TRUE, "mm_deactivate2");

		if (m_Flags.test(flNeedVidRestart))
		{
			m_Flags.set(flNeedVidRestart, FALSE);
			Console->Execute("vid_restart");
		}
		//g_discord.SetStatus(xrDiscordPresense::StatusId::In_Game);
		pInput->SetAllowAccessToBorders(false);
		pInput->LockMouse();
	}
}

bool CMainMenu::ReloadUI()
{
	if (m_startDialog)
	{
		if (m_startDialog->IsShown())
			m_startDialog->HideDialog();
		CleanInternals();
	}
	DLL_Pure* dlg = NEW_INSTANCE(TEXT2CLSID("MAIN_MNU"));
	if (!dlg)
	{
		m_Flags.set(flActive | flNeedChangeCapture, FALSE);
		return false;
	}
	xr_delete(m_startDialog);
	m_startDialog = dynamic_cast<CUIDialogWnd*>(dlg);
	VERIFY(m_startDialog);
	m_startDialog->m_bWorkInPause = true;
	m_startDialog->ShowDialog(true);

	m_activatedScreenRatio = (float)Device.dwWidth / (float)Device.dwHeight > (UI_BASE_WIDTH / UI_BASE_HEIGHT + 0.01f);
	return true;
}

bool CMainMenu::IsActive()
{
	return !!m_Flags.test(flActive);
}

bool CMainMenu::CanSkipSceneRendering()
{
	return IsActive() && !m_Flags.test(flGameSaveScreenshot);
}

void CMainMenu::OnDeviceReset()
{
	if (IsActive() && g_pGameLevel)
		m_Flags.set(flNeedVidRestart, TRUE);
}

void CMainMenu::OnAppActivate()
{
	// set gui mouse to center screen
	// because we should synchronize with system mouse to prevent unwanted sizing
	GetUICursor().SetUICursorPosition(Fvector2().set(512.0f, 384.0f));
}

void CMainMenu::IR_OnThumbstickChanged(GamepadThumbstickType type, const Fvector2& position)
{
	if (IsActive() && type == GamepadThumbstickType::Left)
	{
		float scaledX = position.x * 3.0f;
		float scaledY = position.y * 3.0f;
		m_lastLeftThumbstickValue.set(scaledX, scaledY);
	}
}

void CMainMenu::IR_OnMouseMove(int x, int y)
{
	if (IsActive())
		CDialogHolder::IR_UIOnMouseMove(x, y);
};

void CMainMenu::IR_OnKeyboardPress(u8 dik)
{
	if (IsActive())
	{
		if (is_binded(kCONSOLE, dik))
		{
			Console->Show();
			return;
		}
		if (is_binded(kSCREENSHOT, dik))
		{
			Render->Screenshot();
			return;
		}

		// threat PAD_A as Mouse_LButton
		if (dik == VK_GAMEPAD_A)
		{
			dik = VK_LBUTTON;
		}

		CDialogHolder::IR_UIOnKeyboardPress(dik);
	}
}

void CMainMenu::IR_OnKeyboardRelease(u8 dik)
{
	if (IsActive())
	{
		// threat PAD_A as Mouse_LButton
		if (dik == VK_GAMEPAD_A)
		{
			dik = VK_LBUTTON;
		}
		CDialogHolder::IR_UIOnKeyboardRelease(dik);
	}
};

void CMainMenu::IR_OnKeyboardHold(u8 dik)
{
	if (IsActive())
		CDialogHolder::IR_UIOnKeyboardHold(dik);
};

void CMainMenu::IR_OnMouseWheel(int direction)
{
	if (IsActive())
		CDialogHolder::IR_UIOnMouseWheel(direction);
}


bool CMainMenu::OnRenderPPUI_query()
{
	return IsActive() && !m_Flags.test(flGameSaveScreenshot) && b_shniaganeed_pp;
}

extern void draw_wnds_rects();
void CMainMenu::OnRender()
{
	if (m_Flags.test(flGameSaveScreenshot))
		return;

	if (g_pGameLevel)
		Render->Calculate();

	Render->Render();
	if (!OnRenderPPUI_query())
	{
		DoRenderDialogs();
		UI().RenderFont();
		draw_wnds_rects();
	}
}

void CMainMenu::OnRenderPPUI_main()
{
	if (!IsActive()) return;

	if (m_Flags.test(flGameSaveScreenshot))
		return;

	UI().pp_start();

	if (OnRenderPPUI_query())
	{
		DoRenderDialogs();
		UI().RenderFont();
	}

	UI().pp_stop();
}

//pureFrame
void CMainMenu::OnFrame()
{
	ScopeStatTimer frameTimer(Device.Statistic->Engine_MenuFrame);
	if (m_Flags.test(flNeedChangeCapture))
	{
		m_Flags.set(flNeedChangeCapture, FALSE);
		if (m_Flags.test(flActive))	IR_Capture();
		else						IR_Release();
	}
	CDialogHolder::OnFrame();


	//screenshot stuff
	if (m_Flags.test(flGameSaveScreenshot) && Device.dwFrame > m_screenshotFrame)
	{
		m_Flags.set(flGameSaveScreenshot, FALSE);
		::Render->Screenshot(IRender_interface::SM_FOR_GAMESAVE, m_screenshot_name);

		if (g_pGameLevel && m_Flags.test(flActive))
		{
			Device.seqFrame.Remove(g_pGameLevel);
			Device.seqRender.Remove(g_pGameLevel);
		};

		if (m_Flags.test(flRestoreConsole))
			Console->Show();
	}

	if (IsActive())
	{
		bool b_is_16_9 = (float)Device.dwWidth / (float)Device.dwHeight > (UI_BASE_WIDTH / UI_BASE_HEIGHT + 0.01f);
		if (b_is_16_9 != m_activatedScreenRatio)
		{
			ReloadUI();
			m_startDialog->SendMessageToWnd(m_startDialog, MAIN_MENU_RELOADED, NULL);
		}
	}

	// for gamepad test
	if (m_lastLeftThumbstickValue.square_magnitude() > EPS_L)
	{
		CDialogHolder::IR_UIOnMouseMove((int)m_lastLeftThumbstickValue.x, (int)m_lastLeftThumbstickValue.y);
	}
}

void CMainMenu::OnDeviceCreate()
{
}

void CMainMenu::Screenshot(IRender_interface::ScreenshotMode mode, LPCSTR name)
{
	if (mode != IRender_interface::SM_FOR_GAMESAVE)
	{
		::Render->Screenshot(mode, name);
	}
	else {
		m_Flags.set(flGameSaveScreenshot, TRUE);
		xr_strcpy(m_screenshot_name, name);
		if (g_pGameLevel && m_Flags.test(flActive)) {
			Device.seqFrame.Add(g_pGameLevel);
			Device.seqRender.Add(g_pGameLevel);
		};
		m_screenshotFrame = Device.dwFrame + 1;
		m_Flags.set(flRestoreConsole, Console->bVisible);
		Console->Hide();
	}
}

void CMainMenu::RegisterPPDraw(CUIWindow* w)
{
	UnregisterPPDraw(w);
	m_pp_draw_wnds.push_back(w);
}

void CMainMenu::UnregisterPPDraw(CUIWindow* w)
{
	m_pp_draw_wnds.erase(
		std::remove(
			m_pp_draw_wnds.begin(),
			m_pp_draw_wnds.end(),
			w
		),
		m_pp_draw_wnds.end()
	);
}

void CMainMenu::DestroyInternal(bool bForce)
{
	if (m_startDialog && ((m_deactivated_frame < Device.dwFrame + 4) || bForce))
		xr_delete(m_startDialog);
}

void CMainMenu::OnRenderPPUI_PP()
 {
 	if (!IsActive()) return;
 
 	if (m_Flags.test(flGameSaveScreenshot))	return;
 
 	UI().pp_start();
 
 	xr_vector<CUIWindow*>::iterator it = m_pp_draw_wnds.begin();
 	for (; it != m_pp_draw_wnds.end(); ++it)
 	{
 		(*it)->Draw();
 	}
 	UI().pp_stop();
 }

u32 CMainMenu::GetEngineBuild()
{
	return build_id;
}

const char* CMainMenu::GetEngineBuildDate()
{
	static string256 buffer;

	int	days;
	int	months = 0;
	int	years;
	string16 month;
	sscanf(build_date, "%s %d %d", month, &days, &years);

	string16 days_s;
	string16 months_s;

	for (int i = 0; i < 12; i++) {
		if (_stricmp(month_id[i], month))
			continue;

		months = i;
		break;
	}

	months++; // correcting month number

	if (days < 10)
		xr_sprintf(days_s, "0%d", days);
	else
		xr_sprintf(days_s, "%d", days);

	if (months < 10)
		xr_sprintf(months_s, "0%d", months);
	else
		xr_sprintf(months_s, "%d", months);

	xr_sprintf(buffer, "%s.%s.%d", days_s, months_s, years);

	return buffer;
}