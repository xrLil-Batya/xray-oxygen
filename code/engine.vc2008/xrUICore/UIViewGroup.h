#pragma once
#include "linker.h"
#include "UIView.h"

template <typename T, typename allocator = xalloc<T>>
using list = xr_list<T, allocator>;

class UI_API CUIViewGroup : public CUIView
{
public:
	using CHILD_LIST = list<CUIView*>;

private:
	CHILD_LIST m_childViews;

public:
	CUIViewGroup(CUIXml& xml, XML_NODE* node);
	virtual ~CUIViewGroup();

	virtual void AddView(CUIView* child);
	virtual void RemoveView(CUIView* child);
	virtual void RemoveAllViews();

	virtual bool OnMouseAction(float x, float y, EUIMessages action) override;

	virtual bool OnKeyboardHold(u8 dik) override;
	virtual bool OnKeyboardAction(u8 dik, EUIMessages action) override;

	virtual void Draw(IUIRender* render) override;
protected:
	virtual CUIView* FindViewByNameInternal(shared_str name) override;
};