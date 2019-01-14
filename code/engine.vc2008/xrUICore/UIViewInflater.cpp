#include "stdafx.h"
#include "UIViewInflater.h"

CUIViewInflater::CUIViewInflater()
{
}


CUIViewInflater::~CUIViewInflater()
{
}

CUIView* CUIViewInflater::CreateView(CUIWindow* context, CUIXml& xml, XML_NODE* node, CUIView* parent)
{
	CUIView* view = nullptr;
	if (strstr(node->Value(), "View")) {
		view = new CUIView(xml, node);
	}
	return view;
}