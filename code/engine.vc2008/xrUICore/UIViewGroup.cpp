#include "stdafx.h"
#include "UIViewGroup.h"
#include "ui_base.h"

CUIViewGroup::CUIViewGroup(CUIXml& xml, XML_NODE* node) : CUIView(xml, node)
{

}

CUIViewGroup::~CUIViewGroup()
{
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

	list<CUIView*>::iterator it = std::find(m_childViews.begin(), m_childViews.end(), child);
	R_ASSERT(it != m_childViews.end());
	m_childViews.erase(it);

	child->AssignParent(nullptr);

	/*if (m_pMouseCapturer == pChild)
		SetCapture(pChild, false);

	if (pChild->IsAutoDelete())
		xr_delete(pChild);*/
}

CUIView* CUIViewGroup::FindViewByNameInternal(shared_str name)
{
	R_ASSERT(name);
	if (!name) return nullptr;
	if (name == m_name) return this;

	CHILD_LIST::const_iterator it = m_childViews.begin();
	CHILD_LIST::const_iterator it_e = m_childViews.end();
	for (; it != it_e; ++it) {
		CUIView* child = (*it)->FindViewByName(name);
		if (child) return child;
	}
	return nullptr;
}

void CUIViewGroup::Draw()
{

}
