#pragma once
#include "linker.h"
//#include "../xrEngine/xr_level_controller.h"
#include "UIMessages.h"
//#include "../xrScripts/export/script_export_space.h"
#include "uiabstract.h"
#include "xrUIXmlParser.h"
#include "../Include/xrRender/UIRender.h"
//#include "../xrCore/XMLCore/Expression.h"

class UI_API CUIView
{
protected:
	bool m_bVisible;
	bool m_bEnabled;
	Fvector2 m_position;
	Fvector2 m_size;

	shared_str m_name;

	CUIView* m_parent;

public:
	CUIView(CUIXml& xml, XML_NODE* node);
	virtual ~CUIView();

	IC bool IsVisible() const
	{
		return m_bVisible;
	}

	IC bool IsEnabled() const
	{
		return m_bEnabled;
	}

	const shared_str GetName() const
	{
		return m_name;
	}

	virtual CUIView* GetParent() final
	{
		return m_parent;
	}

	virtual void AssignParent(CUIView* parent) final;

	virtual CUIView* FindViewByName(shared_str name) final;

	virtual bool OnMouseAction(float x, float y, EUIMessages action);

	//virtual bool OnClick();
	//virtual bool OnDoubleClick();

	//virtual bool onMouseScroll(float direction);

	//virtual void OnMouseMove();
	//virtual void OnMouseScroll(float iDirection);
	//virtual bool OnDbClick();
	//virtual bool OnMouseDown(int mouse_btn);
	//virtual void OnMouseUp(int mouse_btn);

	virtual bool OnKeyboardHold(u8 dik);
	virtual bool OnKeyboardAction(u8 dik, EUIMessages action);

	virtual void Draw(IUIRender* render);
protected:
	virtual CUIView* FindViewByNameInternal(shared_str name);
};