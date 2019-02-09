#pragma once
#include "linker.h"
#include "UIView.h"
#include "UIWindow.h"

class UI_API CUIViewInflater
{
public:
	CUIViewInflater();
	virtual ~CUIViewInflater();

public:
	CUIView* CreateView(CUIWindow* context, CUIXml& xml, XML_NODE* node, CUIView* parent);

};

