#include "stdafx.h"
#include "UIViewGroup.h"

CUIViewGroup::CUIViewGroup(CUIXml& xml, XML_NODE* node) : CUIView(xml, node)
{
}

CUIViewGroup::~CUIViewGroup()
{
	RemoveAllViews();
}

void CUIViewGroup::AddView(CUIView* child)
{
	R_ASSERT(child);
	if (!child) return;

	R_ASSERT(!child->GetParent());
	child->AssignParent(this);
	m_childViews.push_back(child);
}

void CUIViewGroup::RemoveView(CUIView* child)
{
	R_ASSERT(child);
	if (!child) return;

	CHILD_LIST::iterator it = std::find(m_childViews.begin(), m_childViews.end(), child);
	R_ASSERT(it != m_childViews.end());
	m_childViews.erase(it);

	child->AssignParent(nullptr);

	delete child;
}

void CUIViewGroup::RemoveAllViews()
{
	while (!m_childViews.empty()) {
		RemoveView(m_childViews.back());
	}
}

bool CUIViewGroup::OnMouseAction(float x, float y, EUIMessages action)
{
	return false;
}

bool CUIViewGroup::OnKeyboardHold(u8 dik)
{
	CHILD_LIST::reverse_iterator it = m_childViews.rbegin();
	for (; it != m_childViews.rend(); ++it)
	{
		if ((*it)->IsEnabled())
		{
			if ((*it)->OnKeyboardHold(dik))	return true;
		}
	}
	return false;
}

bool CUIViewGroup::OnKeyboardAction(u8 dik, EUIMessages action)
{
	CHILD_LIST::reverse_iterator it = m_childViews.rbegin();
	for (; it != m_childViews.rend(); ++it)
	{
		if ((*it)->IsEnabled())
		{
			if ((*it)->OnKeyboardAction(dik, action)) return true;
		}
	}
	return false;
}

CUIView* CUIViewGroup::FindViewByNameInternal(shared_str name)
{
	R_ASSERT(name);
	if (!name) return nullptr;
	if (name == m_name) return this;

	CHILD_LIST::iterator it = m_childViews.begin();
	for (; it != m_childViews.end(); ++it)
	{
		CUIView* child = (*it)->FindViewByName(name);
		if (child) return child;
	}
	return nullptr;
}

void CUIViewGroup::Draw(IUIRender* render)
{
	CHILD_LIST::iterator it = m_childViews.begin();
	for (; it != m_childViews.end(); ++it)
	{
		if ((*it)->IsVisible())
		{
			(*it)->Draw(render);
		}
	}
}
