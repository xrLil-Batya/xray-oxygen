#pragma once
#include "linker.h"
#include "UIView.h"
#include "UIWindow.h"

class UI_API CUILayoutInflater
{
public:
	CUILayoutInflater();
	virtual ~CUILayoutInflater();

	virtual CUIView* Inflate(const char* filename, CUIWindow* root);
	virtual CUIView* Inflate(const char* filename, CUIWindow* root, bool attachToRoot);
	virtual CUIView* Inflate(CUIXml& xml, CUIWindow* root);
	virtual CUIView* Inflate(CUIXml& xml, CUIWindow* root, bool attachToRoot);
};

