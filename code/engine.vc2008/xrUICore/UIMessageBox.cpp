#include "stdafx.h"
#include "UIMessageBox.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "UIEditBox.h"
#include "../xrEngine/string_table.h"
#include "UIXmlInit.h"

CUIMessageBox::CUIMessageBox()
{
	m_UIButtonYesOk		= nullptr;
	m_UIButtonNo		= nullptr;
	m_UIButtonCancel	= nullptr;
	m_UIButtonCopy		= nullptr;
	m_UIStaticPicture	= nullptr;
	m_UIStaticText		= nullptr;
}

CUIMessageBox::~CUIMessageBox()
{
	Clear();
}

#define BUTTON_UP_OFFSET 75
#define BUTTON_WIDTH 140

void CUIMessageBox::Clear(){
	xr_delete(m_UIButtonYesOk);
	xr_delete(m_UIButtonNo);
	xr_delete(m_UIButtonCancel);
	xr_delete(m_UIButtonCopy);
	xr_delete(m_UIStaticPicture);
	xr_delete(m_UIStaticText);
}

bool CUIMessageBox::OnMouseAction(float x, float y, EUIMessages mouse_action)
{
	return inherited::OnMouseAction(x, y, mouse_action);
}

void CUIMessageBox::InitMessageBox(LPCSTR box_template)
{
	Clear							();
	CXml							uiXml;
	uiXml.Load						(CONFIG_PATH, UI_PATH, "message_box.xml");
	CUIXmlInit						xml_init;

	string512 str;

	xr_strconcat								(str, box_template, ":picture");
	if (uiXml.NavigateToNode(str,0)){
		m_UIStaticPicture						= xr_new<CUIStatic>();AttachChild(m_UIStaticPicture);
		xml_init.InitStatic						(uiXml, str, 0, m_UIStaticPicture);
	}

	xr_strconcat								(str, box_template, ":message_text");
	if (uiXml.NavigateToNode(str,0)){
        m_UIStaticText						= xr_new<CUITextWnd>();
		AttachChild							(m_UIStaticText);
        xml_init.InitTextWnd				(uiXml, str, 0, m_UIStaticText);
	}

	xr_strcpy		(str,box_template);
	xml_init.InitStatic						(uiXml, str, 0, this);

	LPCSTR _type							= uiXml.ReadAttrib(str,0,"type",nullptr);
	R_ASSERT								(_type);
	
	m_eMessageBoxStyle	= MESSAGEBOX_OK;
	if(0==stricmp(_type,"ok")){
		m_eMessageBoxStyle	= MESSAGEBOX_OK;
	}else
	if(0==stricmp(_type,"yes_no")){
		m_eMessageBoxStyle	= MESSAGEBOX_YES_NO;
	}else
	if(0==stricmp(_type,"yes_no_cancel")){
		m_eMessageBoxStyle	= MESSAGEBOX_YES_NO_CANCEL;
	}else
	if(0==stricmp(_type,"yes_no_copy")){
		m_eMessageBoxStyle	= MESSAGEBOX_YES_NO_COPY;
	}else
	if(0==stricmp(_type,"quit_windows")){
		m_eMessageBoxStyle	= MESSAGEBOX_QUIT_WINDOWS;
	}else 
	if(0==stricmp(_type,"quit_game")){
		m_eMessageBoxStyle	= MESSAGEBOX_QUIT_GAME;
	}else 
		if(0==stricmp(_type,"info")){
			m_eMessageBoxStyle	= MESSAGEBOX_INFO;
		};
	

	switch (m_eMessageBoxStyle){

		case MESSAGEBOX_OK:
		{
			xr_strconcat						(str, box_template, ":button_ok");
			m_UIButtonYesOk						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonYesOk);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonYesOk);
		}break;

		case MESSAGEBOX_INFO:
		{
			break;
		}

		case MESSAGEBOX_QUIT_WINDOWS:
		case MESSAGEBOX_QUIT_GAME:
		case MESSAGEBOX_YES_NO:
			{
			xr_strconcat						(str,box_template,":button_yes");
			m_UIButtonYesOk						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonYesOk);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonYesOk);

			xr_strconcat						(str,box_template,":button_no");
			m_UIButtonNo						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonNo);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonNo);
		}break;

		case MESSAGEBOX_YES_NO_CANCEL:{
			xr_strconcat						(str,box_template,":button_yes");
			m_UIButtonYesOk						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonYesOk);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonYesOk);

			xr_strconcat						(str,box_template,":button_no");
			m_UIButtonNo						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonNo);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonNo);

			xr_strconcat						(str,box_template,":button_cancel");
			m_UIButtonCancel					= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonCancel);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonCancel);
		}break;

		case MESSAGEBOX_YES_NO_COPY:{
			xr_strconcat						(str,box_template,":button_yes");
			m_UIButtonYesOk						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonYesOk);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonYesOk);

			xr_strconcat						(str,box_template,":button_no");
			m_UIButtonNo						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonNo);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonNo);

			xr_strconcat						(str,box_template,":button_copy");
			m_UIButtonCopy						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonCopy);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonCopy);
		}break;
	};
}


void CUIMessageBox::OnYesOk()
{
	switch (m_eMessageBoxStyle)
	{
	case 		MESSAGEBOX_OK:
	case 		MESSAGEBOX_INFO:
		GetMessageTarget()->SendMessageToWnd(m_UIButtonYesOk, MESSAGE_BOX_OK_CLICKED);
		GetMessageTarget()->SendMessageToWnd(this, MESSAGE_BOX_OK_CLICKED);
		break;

	case 		MESSAGEBOX_YES_NO_CANCEL:
	case 		MESSAGEBOX_YES_NO:
		GetMessageTarget()->SendMessageToWnd(m_UIButtonYesOk, MESSAGE_BOX_YES_CLICKED);
		GetMessageTarget()->SendMessageToWnd(this, MESSAGE_BOX_YES_CLICKED);
		break;
	case 		MESSAGEBOX_QUIT_WINDOWS:
		GetMessageTarget()->SendMessageToWnd(this, MESSAGE_BOX_QUIT_WIN_CLICKED);
		break;
	case 		MESSAGEBOX_QUIT_GAME:
		GetMessageTarget()->SendMessageToWnd(this, MESSAGE_BOX_QUIT_GAME_CLICKED);
		break;
	};
}

void CUIMessageBox::SendMessageToWnd(CUIWindow *pWnd, s16 msg, void *pData)
{
	if ( msg == BUTTON_CLICKED )
	{
		switch(m_eMessageBoxStyle)
		{
		case MESSAGEBOX_OK:
			if(pWnd == m_UIButtonYesOk)
			{
				OnYesOk();
			}
			break;
		case MESSAGEBOX_YES_NO:
		case MESSAGEBOX_QUIT_WINDOWS:
			if(pWnd == m_UIButtonYesOk)
			{
				OnYesOk();
			}
			else if(pWnd == m_UIButtonNo)
			{
				GetMessageTarget()->SendMessageToWnd(m_UIButtonNo, MESSAGE_BOX_NO_CLICKED);
				GetMessageTarget()->SendMessageToWnd(this, MESSAGE_BOX_NO_CLICKED);
			}
			break;
		case MESSAGEBOX_YES_NO_CANCEL:
			if(pWnd == m_UIButtonYesOk)
			{
				OnYesOk();
			}
			else if(pWnd == m_UIButtonNo)
			{
				GetMessageTarget()->SendMessageToWnd(m_UIButtonNo, MESSAGE_BOX_NO_CLICKED);
				GetMessageTarget()->SendMessageToWnd(this, MESSAGE_BOX_NO_CLICKED);
			}
			else if(pWnd == m_UIButtonCancel)
			{
				GetMessageTarget()->SendMessageToWnd(m_UIButtonCancel, MESSAGE_BOX_CANCEL_CLICKED);
				GetMessageTarget()->SendMessageToWnd(this, MESSAGE_BOX_CANCEL_CLICKED);
			}
			break;
		case MESSAGEBOX_YES_NO_COPY:
			if(pWnd == m_UIButtonYesOk)
			{
				OnYesOk();
			}
			else if(pWnd == m_UIButtonNo)
			{
				GetMessageTarget()->SendMessageToWnd(m_UIButtonNo, MESSAGE_BOX_NO_CLICKED);
				GetMessageTarget()->SendMessageToWnd(this, MESSAGE_BOX_NO_CLICKED);
			}
			else if(pWnd == m_UIButtonCopy)
			{
				GetMessageTarget()->SendMessageToWnd(m_UIButtonCopy, MESSAGE_BOX_COPY_CLICKED);
				GetMessageTarget()->SendMessageToWnd(this, MESSAGE_BOX_COPY_CLICKED);
			}
			break;
		};
	};
	inherited::SendMessageToWnd(pWnd, msg, pData);
}

void CUIMessageBox::SetText(LPCSTR str)
{
	m_UIStaticText->SetTextST(str);
}

LPCSTR CUIMessageBox::GetText()
{
	return m_UIStaticText->GetText();
}

#include "UIMessageBoxEx.h"
#include "luabind/luabind.hpp"
using namespace luabind;

#pragma optimize("s",on)
void CUIMessageBox::script_register(lua_State *L)
{
	module(L)
		[
		class_<CUIMessageBox, CUIStatic>("CUIMessageBox")
			.def(constructor<>())
			.def("InitMessageBox", &CUIMessageBox::InitMessageBox)
			.def("SetText", &CUIMessageBox::SetText),
			
		class_<CUIMessageBoxEx, CUIDialogWnd>("CUIMessageBoxEx")
			.def(constructor<>())
			.def("InitMessageBox", &CUIMessageBoxEx::InitMessageBox)
			.def("SetText", &CUIMessageBoxEx::SetText)
		];

}