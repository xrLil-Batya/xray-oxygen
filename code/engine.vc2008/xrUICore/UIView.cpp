#include "stdafx.h"
#include "UIView.h"
#include "ui_base.h"
//#include "../Include/xrRender/DebugRender.h"
//#include "../Include/xrRender/UIRender.h"

CUIView::CUIView(CUIXml& xml, XML_NODE* node)
{
	m_bVisible = xml.ReadAttribBool(node, "visibility");
	m_bEnabled = xml.ReadAttribBool(node, "enabled");
	m_position.x = xml.ReadAttribFlt(node, "x");
	m_position.y = xml.ReadAttribFlt(node, "y");
	m_size.x = xml.ReadAttribFlt(node, "width");
	m_size.y = xml.ReadAttribFlt(node, "height");

	m_name = xml.ReadAttrib(node, "name", nullptr);

	/*xr_string alignment = xml.ReadAttrib(node, "alignment", nullptr);
	if (strchr(alignment.c_str(), 'c'))
	{
		m_alignment = waCenter;
	}*/
}

CUIView::~CUIView()
{
}

void CUIView::AssignParent(CUIView* parent)
{
	if (!m_parent) 
	{
		m_parent = parent;
	}
	else if (!parent)
	{
		m_parent = nullptr;
	}
}

CUIView* CUIView::FindViewByName(shared_str name)
{
	R_ASSERT(name);
	if (!name) return nullptr;

	return FindViewByNameInternal(name);
}

CUIView* CUIView::FindViewByNameInternal(shared_str name)
{
	if (name != nullptr && name == m_name) {
		return this;
	}
	return nullptr;
}

void CUIView::Draw()
{

}


/*bool CUIXmlInit::InitAlignment(CXml &xml_doc, const char *path, int index, float &x, float &y, CUIWindow* pWnd)
{
	xr_string wnd_alignment = xml_doc.ReadAttrib(path, index, "alignment", "");

	if (strchr(wnd_alignment.c_str(), 'c'))
		pWnd->SetAlignment(waCenter);

	// Alignment: right: "r", bottom: "b". Top, left - useless
	shared_str	alignStr = xml_doc.ReadAttrib(path, index, "align", "");

	bool result = false;

	if (strchr(*alignStr, 'r'))
	{
		x = ApplyAlignX(x, alRight);
		result = true;
	}
	if (strchr(*alignStr, 'b'))
	{
		y = ApplyAlignY(y, alBottom);
		result = true;
	}
	if (strchr(*alignStr, 'c'))
	{
		ApplyAlign(x, y, alCenter);
		result = true;
	}

	return result;
}*/