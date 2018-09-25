////////////////////////////////////////////////////////////////////////////
//	Module 		: UIHelper.h
//	Created 	: 17.01.2008
//	Author		: Evgeniy Sokolov
//	Description : UI Helper class
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "linker.h"

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

class UI_API UIHelper
{
public:
	UIHelper		() = default;
	~UIHelper		() = default;

	/// <summary>Create CUIStatic</summary>
	static	CUIStatic*			CreateStatic		( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	/// <summary>Create CUITextWnd</summary>
	static	CUITextWnd*			CreateTextWnd		( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	/// <summary>Create CUIProgressBar</summary>
	static	CUIProgressBar*		CreateProgressBar	( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	/// <summary>Create CreateFrameLine</summary>
	static	CUIFrameLineWnd*	CreateFrameLine		( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	/// <summary>Create CreateFrameWindow</summary>
	static	CUIFrameWindow*		CreateFrameWindow	( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	/// <summary>Create Create3tButton</summary>
	static	CUI3tButton*		Create3tButton		( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	/// <summary>Create CreateCheck</summary>
	static	CUICheckButton*		CreateCheck			( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	/// <summary>Create CreateEditBox</summary>
	static	CUIEditBox*			CreateEditBox		( CXml& xml, LPCSTR ui_path, CUIWindow* parent );
	/// <summary>Create CreateHint</summary>
	static	UIHint*				CreateHint			( CXml& xml, LPCSTR ui_path /*, CUIWindow* parent*/ );
	/// <summary>Create CreateDragDropListEx</summary>
	static	CUIWindow*	CreateDragDropListEx( CXml& xml, LPCSTR ui_path, CUIWindow* parent, CUIWindow* pDragDrop);
	/// <summary>Create CUIDragDropReferenceList</summary>
	static	CUIWindow*	CreateDragDropReferenceList( CXml& xml, LPCSTR ui_path, CUIWindow* parent, CUIWindow* pDragDrop);

};