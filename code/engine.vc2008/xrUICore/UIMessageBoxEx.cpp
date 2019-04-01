#include "stdafx.h"

#include "UIMessageBox.h"
#include "UIMessageBoxEx.h"
#include "UIDialogHolder.h"

CUIMessageBoxEx::CUIMessageBoxEx(){
	m_pMessageBox = xr_new<CUIMessageBox>();
	m_pMessageBox->SetWindowName("msg_box");
	AttachChild(m_pMessageBox);
}

CUIMessageBoxEx::~CUIMessageBoxEx(){
	xr_delete(m_pMessageBox);
}

void CUIMessageBoxEx::InitMessageBox(LPCSTR xml_template)
{
	m_pMessageBox->InitMessageBox(xml_template);
	
	SetWndPos( m_pMessageBox->GetWndPos() );
	SetWndSize( m_pMessageBox->GetWndSize() );
	m_pMessageBox->SetWndPos( Fvector2().set(0,0) );

	AddCallback( m_pMessageBox, MESSAGE_BOX_YES_CLICKED, CUIWndCallback::void_function( this, &CUIMessageBoxEx::OnOKClicked ) );
	CUIMessageBox::E_MESSAGEBOX_STYLE style = m_pMessageBox->GetBoxStyle();
	if(style==CUIMessageBox::MESSAGEBOX_YES_NO || style==CUIMessageBox::MESSAGEBOX_QUIT_WINDOWS || style==CUIMessageBox::MESSAGEBOX_QUIT_GAME)
		AddCallback( m_pMessageBox, MESSAGE_BOX_NO_CLICKED, CUIWndCallback::void_function( this, &CUIMessageBoxEx::OnNOClicked ) );
}

void CUIMessageBoxEx::OnOKClicked( CUIWindow* w, void* d )
{
	if ( !func_on_ok.empty() )
	{
		func_on_ok( w, d );
	}
}

void CUIMessageBoxEx::OnNOClicked( CUIWindow* w, void* d )
{
	if ( !func_on_no.empty() )
	{
		func_on_no( w, d );
	}
}

void CUIMessageBoxEx::SetText(LPCSTR text){
	m_pMessageBox->SetText(text);

}

LPCSTR CUIMessageBoxEx::GetText ()
{
	return m_pMessageBox->GetText();
}

void CUIMessageBoxEx::SendMessageToWnd(CUIWindow* pWnd, s16 msg, void* pData /* = NULL */)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
	if (pWnd == m_pMessageBox)
	{
		switch (msg){
			case MESSAGE_BOX_OK_CLICKED:
			case MESSAGE_BOX_YES_CLICKED:
			case MESSAGE_BOX_NO_CLICKED:
			case MESSAGE_BOX_CANCEL_CLICKED:
			case MESSAGE_BOX_QUIT_WIN_CLICKED:
			case MESSAGE_BOX_QUIT_GAME_CLICKED:
				HideDialog();
			default:
				break;
		}

		if (GetMessageTarget())
            GetMessageTarget()->SendMessageToWnd(this,msg,pData);
	}
	
}

bool CUIMessageBoxEx::OnKeyboardAction(u8 dik, EUIMessages keyboard_action)
{
	if(keyboard_action==WINDOW_KEY_PRESSED)
	{
		if (dik == VK_RETURN || dik == VK_SPACE)
		{
			m_pMessageBox->OnYesOk();
			return true;
		}
		else
			return CUIDialogWnd::OnKeyboardAction(dik, keyboard_action);
	}
	return CUIDialogWnd::OnKeyboardAction(dik, keyboard_action);
}