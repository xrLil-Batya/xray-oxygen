////////////////////////////////////////////////////////////////////////////
//	Module 		: UIHelper.h
//	Created 	: 17.01.2008
//	Author		: Evgeniy Sokolov
//	Description : UI Helper class
////////////////////////////////////////////////////////////////////////////
#pragma once
class CXml;
class CUIWindow;
class CUIStatic;
class CUITextWnd;
class CUIProgressBar;
class CUIFrameLineWnd;
class CUIFrameWindow;
class CUI3tButton;
class CUICheckButton;
class UIHint;
class CUIDragDropListEx;
class CUIDragDropReferenceList;
class CUIEditBox;

class UIHelper
{
public:
	UIHelper		() {};
	~UIHelper		() {};

	static	CUIStatic*			CreateStatic		( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	static	CUITextWnd*			CreateTextWnd		( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	static	CUIProgressBar*		CreateProgressBar	( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	static	CUIFrameLineWnd*	CreateFrameLine		( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	static	CUIFrameWindow*		CreateFrameWindow	( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	static	CUI3tButton*		Create3tButton		( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	static	CUICheckButton*		CreateCheck			( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	static	CUIEditBox*			CreateEditBox		( CXml& xml, LPCSTR ui_path, CUIWindow* parent );

	static	UIHint*				CreateHint			( CXml& xml, LPCSTR ui_path /*, CUIWindow* parent*/ );
	static	CUIDragDropListEx*	CreateDragDropListEx( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	static	CUIDragDropReferenceList*	CreateDragDropReferenceList( CXml& xml, LPCSTR ui_path, CUIWindow* parent );

}; // class UIHelper

#endif // UI_HELPER_H_INCLUDED
