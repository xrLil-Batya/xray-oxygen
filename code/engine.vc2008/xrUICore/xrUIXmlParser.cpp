#include "stdafx.h"
#include "xrUIXmlParser.h"
#include "ui_base.h"

shared_str CUIXml::correct_file_name(LPCSTR path, LPCSTR fn)
{
	if (!xr_strcmp(path, UI_PATH) || !xr_strcmp(path, "UI"))
	{
		return UI().get_xml_name(fn);
	}
	else return fn;
}

void dump_list_xmls(){}

CUIXml::CUIXml()
{
}

CUIXml::~CUIXml()
{
}
