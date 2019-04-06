////////////////////////////////////////////////////////////////////////////
//	Module 		: UIMapLegend.h
//	Created 	: 03.06.2008
//	Author		: Evgeniy Sokolov
//	Description : UI Map Legend Wnd (PDA : Task) class
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../xrUICore/UIWindow.h"

class CUIXml;
class CUIFrameWindow;
class CUIScrollView;
class CUIStatic;
class CUI3tButton;

class UIMapLegend : public CUIWindow
{
private:
	using inherited = CUIWindow;

public:
					UIMapLegend			();
	virtual			~UIMapLegend		();

			void	init_from_xml		( CUIXml& xml, LPCSTR path );

	virtual void	Show				( bool status );
	virtual void	SendMessageToWnd			( CUIWindow* pWnd, s16 msg, void* pData );

private: // m_
	CUIFrameWindow*		m_background;
	CUIScrollView*		m_list;

	CUIStatic*			m_caption;
	CUI3tButton*		m_btn_close;

}; // class UIMapLegend

// -------------------------------------------------------------------------------------------------

class UIMapLegendItem : public CUIWindow
{
private:
	using inherited = CUIWindow;

public:
					UIMapLegendItem		();
	virtual			~UIMapLegendItem	();

			void	init_from_xml		( CUIXml& xml, int index );

private: // m_
	CUIStatic*		m_image[4];
	CUIStatic*		m_text;

}; // class UIMapLegendItem
