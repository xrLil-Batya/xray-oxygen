#pragma once
#include "linker.h"
#include "UIView.h"

template <typename T, typename allocator = xalloc<T>>
using list = xr_list<T, allocator>;

class UI_API CUIViewGroup : public CUIView
{
public:
	using CHILD_LIST = list<CUIView*>;
	using CHILD_LIST_IT = CHILD_LIST::iterator;

private:
	CHILD_LIST m_childViews;

public:
	CUIViewGroup(CUIXml& xml, XML_NODE* node);
	virtual ~CUIViewGroup();

	virtual void AddView(CUIView* child);
	virtual void RemoveView(CUIView* child);

protected:
	virtual CUIView* FindViewByNameInternal(shared_str name) override;

	virtual void Draw() override;
};