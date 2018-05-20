#pragma once

#include "..\..\Include\xrRender\ThunderboltDescRender.h"

class IRender_DetailModel;

class dxThunderboltDescRender : public IThunderboltDescRender
{
public:
	virtual void Copy(IThunderboltDescRender &_in);

	virtual void CreateModel(LPCSTR m_name);
	virtual void DestroyModel();

public:
	IRender_DetailModel*		l_model;
};