#include "stdafx.h"
#include "UIXmlInit.h"
//#include "../level.h"
#include "../xrEngine/string_table.h"
#include "UIFrameWindow.h"
#include "UICheckButton.h"
#include "UICustomSpin.h"
#include "UIRadioButton.h"
#include "UIProgressBar.h"
#include "UIProgressShape.h"
#include "UITabControl.h"
#include "UIXmlInit.h"
#include "UIAnimatedStatic.h"
#include "UIListBox.h"
#include "UIComboBox.h"
#include "UITrackBar.h"
#include "UIHint.h"
#include "UIScrollView.h"
#include "UICustomEdit.h"
#include "UIEditBox.h"
//#include "game_base.h"

#include "UITextureMaster.h"
//#include "UIDragDropListEx.h"
//#include "UIDragDropReferenceList.h"
#include "UILines.h"

#define DI_FONT_NAME "di"

#define MEDIUM_FONT_NAME "medium"
#define SMALL_FONT_NAME "small"

#define ARIAL14_FONT_NAME "arial14"
#define ARIAL21_FONT_NAME "arial21"

#define GRAFFITI19_FONT_NAME "graffiti19"
#define GRAFFITI22_FONT_NAME "graffiti22"
#define GRAFFITI32_FONT_NAME "graffiti32"
#define GRAFFITI50_FONT_NAME "graffiti50"

#define LETTERICA16_FONT_NAME "letterica16"
#define LETTERICA18_FONT_NAME "letterica18"
#define LETTERICA25_FONT_NAME "letterica25"

const char* const COLOR_DEFINITIONS = "color_defs.xml";
CUIXmlInit::ColorDefs* CUIXmlInit::m_pColorDefs = nullptr;

CUIXmlInit::CUIXmlInit()
{
	InitColorDefs();
}

CUIXmlInit::~CUIXmlInit()
{
}

Frect CUIXmlInit::GetFRect(CXml& xml_doc, LPCSTR path, int index)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	Frect rect;
	rect.set(0, 0, 0, 0);
	rect.x1 = xml_doc.ReadAttribFlt(path, index, "x");
	rect.y1 = xml_doc.ReadAttribFlt(path, index, "y");
	rect.x2 = rect.x1 + xml_doc.ReadAttribFlt(path, index, "width");
	rect.y2 = rect.y1 + xml_doc.ReadAttribFlt(path, index, "height");

	return rect;
}

bool CUIXmlInit::InitWindow(CXml& xml_doc, LPCSTR path, int index, CUIWindow* pWnd)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	Fvector2 pos, size;
	pos.x = xml_doc.ReadAttribFlt(path, index, "x");
	pos.y = xml_doc.ReadAttribFlt(path, index, "y");
	InitAlignment(xml_doc, path, index, pos.x, pos.y, pWnd);
	size.x = xml_doc.ReadAttribFlt(path, index, "width");
	size.y = xml_doc.ReadAttribFlt(path, index, "height");
	pWnd->SetWndPos(pos);
	pWnd->SetWndSize(size);

	LPCSTR expressionStr = xml_doc.ReadAttrib(path, index, "expression");
	if (expressionStr != nullptr)
	{
		xr_string expression(expressionStr);
		if (!expression.empty())
		{
			pWnd->m_expression.CompileExpression(expression);
		}
	}

	string512 buf;
	xr_strconcat(buf, path, ":window_name");
	if (xml_doc.NavigateToNode(buf, index))
		pWnd->SetWindowName(xml_doc.Read(buf, index, nullptr));

	InitAutoStaticGroup(xml_doc, path, index, pWnd);

	return true;
}

bool CUIXmlInit::InitFrameWindow(CXml& xml_doc, LPCSTR path, int index, CUIFrameWindow* pWnd)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	InitTexture(xml_doc, path, index, pWnd);
	InitWindow(xml_doc, path, index, pWnd);
	return true;
}

bool CUIXmlInit::InitOptionsItem(CXml& xml_doc, LPCSTR path, int index, CUIOptionsItem* pWnd)
{
	string256 buf;
	xr_strconcat(buf, path, ":options_item");

	if (xml_doc.NavigateToNode(buf, index))
	{
		shared_str entry = xml_doc.ReadAttrib(buf, index, "entry");
		shared_str group = xml_doc.ReadAttrib(buf, index, "group");
		pWnd->AssignProps(entry, group);

		LPCSTR depends = xml_doc.ReadAttrib(buf, index, "depend", nullptr);
		if (depends)
		{
			CUIOptionsItem::ESystemDepends d = CUIOptionsItem::sdNothing;

			if (stricmp(depends, "vid") == 0)
				d = CUIOptionsItem::sdVidRestart;
			else if (stricmp(depends, "snd") == 0)
				d = CUIOptionsItem::sdSndRestart;
			else if (stricmp(depends, "restart") == 0)
				d = CUIOptionsItem::sdSystemRestart;
			else if (stricmp(depends, "runtime") == 0)
				d = CUIOptionsItem::sdApplyOnChange;
			else
				Msg("! unknown param [%s] in optionsItem [%s]", depends, entry.c_str());

			pWnd->SetSystemDepends(d);
		}
		return true;
	}
	else
		return false;
}

bool CUIXmlInit::InitTextureOffset(CXml &xml_doc, LPCSTR path, int index, CUIStatic* pWnd)
{
	string256 textureOffset;
	if (xr_strcmp(path, "") == 0)
		xr_strcpy(textureOffset, "texture_offset");
	else
		xr_strconcat(textureOffset, path, ":texture_offset");

	float x = xml_doc.ReadAttribFlt(textureOffset, index, "x");
	float y = xml_doc.ReadAttribFlt(textureOffset, index, "y");
	pWnd->SetTextureOffset(x, y);

	return true;
}

bool CUIXmlInit::InitStatic(CXml& xml_doc, LPCSTR path, int index, CUIStatic* pWnd)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	InitWindow(xml_doc, path, index, pWnd);

	string256 buf;
	InitText(xml_doc, xr_strconcat(buf, path, ":text"), index, pWnd);
	InitTexture(xml_doc, path, index, pWnd);
	InitTextureOffset(xml_doc, path, index, pWnd);

	float heading_angle = xml_doc.ReadAttribFlt(path, index, "heading_angle");
	if (!fis_zero(heading_angle))
	{
		pWnd->EnableHeading(true);
		pWnd->SetConstHeading(true);
		pWnd->SetHeading(deg2rad(heading_angle));
	}
	else
	{
		pWnd->EnableHeading(xml_doc.ReadAttribBool(path, index, "heading"));
	}

	LPCSTR str_flag = xml_doc.ReadAttrib(path, index, "light_anim");
	int flag_cyclic = xml_doc.ReadAttribInt(path, index, "la_cyclic", 1);
	int flag_text = xml_doc.ReadAttribInt(path, index, "la_text", 1);
	int flag_texture = xml_doc.ReadAttribInt(path, index, "la_texture", 1);
	int flag_alpha = xml_doc.ReadAttribInt(path, index, "la_alpha");

	u8 flags = 0;
	if (flag_cyclic)
		flags |= LA_CYCLIC;

	if (flag_alpha)
		flags |= LA_ONLYALPHA;

	if (flag_text)
		flags |= LA_TEXTCOLOR;

	if (flag_texture)
		flags |= LA_TEXTURECOLOR;

	pWnd->SetColorAnimation(str_flag, flags);

	str_flag = xml_doc.ReadAttrib(path, index, "xform_anim");
	flag_cyclic = xml_doc.ReadAttribInt(path, index, "xform_anim_cyclic", 1);

	pWnd->SetXformLightAnim(str_flag, (flag_cyclic) ? true : false);

	bool complexMode = xml_doc.ReadAttribBool(path, index, "complex_mode");
	if (complexMode)
		pWnd->TextItemControl()->SetTextComplexMode(complexMode);

	pWnd->m_stat_hint_text = xml_doc.ReadAttrib(path, index, "hint", "");

	return true;
}

bool CUIXmlInit::InitTextWnd(CXml& xml_doc, LPCSTR path, int index, CUITextWnd* pWnd)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	InitWindow(xml_doc, path, index, pWnd);

	string256 buf;
	InitText(xml_doc, xr_strconcat(buf, path, ":text"), index, &pWnd->TextItemControl());

	LPCSTR str_flag = xml_doc.ReadAttrib(path, index, "light_anim");
	int flag_cyclic = xml_doc.ReadAttribInt(path, index, "la_cyclic", 1);
	int flag_alpha = xml_doc.ReadAttribInt(path, index, "la_alpha");

	u8 flags = LA_TEXTCOLOR;
	if (flag_cyclic)
		flags |= LA_CYCLIC;

	if (flag_alpha)
		flags |= LA_ONLYALPHA;

	pWnd->SetColorAnimation(str_flag, flags);

	bool complexMode = xml_doc.ReadAttribBool(path, index, "complex_mode");
	if (complexMode)
		pWnd->SetTextComplexMode(complexMode);

	xr_strconcat(buf, path, ":texture");
	R_ASSERT3(!xml_doc.NavigateToNode(buf, index), xml_doc.m_xml_file_name, buf);

	R_ASSERT(pWnd->GetChildWndList().size() == 0);
	return true;
}

bool CUIXmlInit::InitCheck(CXml& xml_doc, LPCSTR path, int index, CUICheckButton* pWnd)
{
	InitStatic(xml_doc, path, index, pWnd);

	string256 buf;
	xr_strconcat(buf, path, ":texture");
	LPCSTR texture = xml_doc.Read(buf, index, "ui_checker");

	pWnd->InitCheckButton(pWnd->GetWndPos(), pWnd->GetWndSize(), texture);

	u32 color;
	xr_strconcat(buf, path, ":text_color:e");
	if (xml_doc.NavigateToNode(buf, index)) {
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetStateTextColor(color, S_Enabled);
	}

	xr_strconcat(buf, path, ":text_color:d");
	if (xml_doc.NavigateToNode(buf, index)) {
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetStateTextColor(color, S_Disabled);
	}

	xr_strconcat(buf, path, ":text_color:t");
	if (xml_doc.NavigateToNode(buf, index)) {
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetStateTextColor(color, S_Touched);
	}

	xr_strconcat(buf, path, ":text_color:h");
	if (xml_doc.NavigateToNode(buf, index)) {
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetStateTextColor(color, S_Highlighted);
	}

	InitOptionsItem(xml_doc, path, index, pWnd);

	return true;
}

bool CUIXmlInit::InitSpin(CXml& xml_doc, LPCSTR path, int index, CUICustomSpin* pWnd)
{
	InitWindow(xml_doc, path, index, pWnd);
	InitOptionsItem(xml_doc, path, index, pWnd);
	pWnd->InitSpin(pWnd->GetWndPos(), pWnd->GetWndSize());

	string256 foo;
	u32 color;
	xr_strconcat(foo, path, ":text_color:e");
	if (xml_doc.NavigateToNode(foo, index)) {
		color = GetColor(xml_doc, foo, index, 0x00);
		pWnd->SetTextColor(color);
	}
	xr_strconcat(foo, path, ":text_color:d");
	if (xml_doc.NavigateToNode(foo, index)) {
		color = GetColor(xml_doc, foo, index, 0x00);
		pWnd->SetTextColorD(color);
	}

	return true;
}

bool CUIXmlInit::InitText(CXml& xml_doc, LPCSTR path, int index, CUIStatic* pWnd)
{
	if (!xml_doc.NavigateToNode(path, index))
		return false;

	return InitText(xml_doc, path, index, pWnd->TextItemControl());
}

bool CUIXmlInit::InitText(CXml& xml_doc, LPCSTR path, int index, CUILines* pLines)
{
	if (!xml_doc.NavigateToNode(path, index))
		return false;

	u32	color;
	CGameFont* pTmpFont = nullptr;
	InitFont(xml_doc, path, index, color, pTmpFont);
	pLines->SetTextColor(color);
	R_ASSERT(pTmpFont);
	pLines->SetFont(pTmpFont);

	// Load font alignment
	shared_str al = xml_doc.ReadAttrib(path, index, "align");
	if (xr_strcmp(al, "c") == 0)
		pLines->SetTextAlignment(CGameFont::alCenter);
	else if (xr_strcmp(al, "r") == 0)
		pLines->SetTextAlignment(CGameFont::alRight);
	else if (xr_strcmp(al, "l") == 0)
		pLines->SetTextAlignment(CGameFont::alLeft);

	al = xml_doc.ReadAttrib(path, index, "vert_align");

	if (xr_strcmp(al, "c") == 0)
		pLines->SetVTextAlignment(valCenter);
	else if (xr_strcmp(al, "b") == 0)
		pLines->SetVTextAlignment(valBotton);
	else if (xr_strcmp(al, "t") == 0)
		pLines->SetVTextAlignment(valTop);

	pLines->SetTextComplexMode(xml_doc.ReadAttribBool(path, index, "complex_mode"));

	// Text coordinates
	float text_x = xml_doc.ReadAttribFlt(path, index, "x");
	float text_y = xml_doc.ReadAttribFlt(path, index, "y");

	pLines->m_TextOffset.set(text_x, text_y);

	shared_str text = xml_doc.Read(path, index, nullptr);
	if (text.size())
		pLines->SetText(CStringTable().translate(text).c_str());

	return true;
}

bool CUIXmlInit::Init3tButton(CXml& xml_doc, LPCSTR path, int index, CUI3tButton* pWnd)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	pWnd->m_frameline_mode = xml_doc.ReadAttribBool(path, index, "frame_mode");
	pWnd->vertical = xml_doc.ReadAttribBool(path, index, "vertical");

	InitWindow(xml_doc, path, index, pWnd);
	pWnd->InitButton(pWnd->GetWndPos(), pWnd->GetWndSize());

	string256 buf;
	InitText(xml_doc, xr_strconcat(buf, path, ":text"), index, pWnd);
	u32 color;

	xr_strconcat(buf, path, ":text_color:e");
	if (xml_doc.NavigateToNode(buf, index)) {
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetStateTextColor(color, S_Enabled);
	}

	xr_strconcat(buf, path, ":text_color:d");
	if (xml_doc.NavigateToNode(buf, index)) {
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetStateTextColor(color, S_Disabled);
	}

	xr_strconcat(buf, path, ":text_color:t");
	if (xml_doc.NavigateToNode(buf, index)) {
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetStateTextColor(color, S_Touched);
	}

	xr_strconcat(buf, path, ":text_color:h");
	if (xml_doc.NavigateToNode(buf, index)) {
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetStateTextColor(color, S_Highlighted);
	}

	InitMultiTexture(xml_doc, path, index, pWnd);
	InitTextureOffset(xml_doc, path, index, pWnd);
	InitSound(xml_doc, path, index, pWnd);

	LPCSTR accel = xml_doc.ReadAttrib(path, index, "accel", nullptr);
	if (accel)
	{
		int acc = keyname_to_dik(accel);
		pWnd->SetAccelerator(acc, 0);
	}
	accel = xml_doc.ReadAttrib(path, index, "accel_ext", nullptr);
	if (accel)
	{
		int acc = keyname_to_dik(accel);
		pWnd->SetAccelerator(acc, 1);
	}

	LPCSTR hint = xml_doc.ReadAttrib(path, index, "hint", nullptr);
	if (hint)
		pWnd->m_hint_text = CStringTable().translate(hint);

	return true;
}

bool CUIXmlInit::InitSound(CXml& xml_doc, LPCSTR path, int index, CUI3tButton* pWnd)
{
	string256 sound_h;
	string256 sound_t;
	xr_strconcat(sound_h, path, ":sound_h");
	xr_strconcat(sound_t, path, ":sound_t");

	shared_str sound_h_result = xml_doc.Read(sound_h, index, "");
	shared_str sound_t_result = xml_doc.Read(sound_t, index, "");

	if (xr_strlen(sound_h_result) != 0)
		pWnd->InitSoundH(*sound_h_result);

	if (xr_strlen(sound_t_result) != 0)
		pWnd->InitSoundT(*sound_t_result);

	return true;
}

bool CUIXmlInit::InitProgressBar(CXml& xml_doc, LPCSTR path, int index, CUIProgressBar* pWnd)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	InitAutoStaticGroup(xml_doc, path, index, pWnd);

	string256 buf;
	Fvector2 pos, size;
	pos.x = xml_doc.ReadAttribFlt(path, index, "x");
	pos.y = xml_doc.ReadAttribFlt(path, index, "y");

	InitAlignment(xml_doc, path, index, pos.x, pos.y, pWnd);

	size.x = xml_doc.ReadAttribFlt(path, index, "width");
	size.y = xml_doc.ReadAttribFlt(path, index, "height");

	LPCSTR expressionStr = xml_doc.ReadAttrib(path, index, "expression");
	if (expressionStr != nullptr)
	{
		xr_string expression(expressionStr);
		if (!expression.empty())
		{
			pWnd->m_expression.CompileExpression(expression);
		}
	}

	CUIProgressBar::EOrientMode mode = CUIProgressBar::om_vert;
	bool horizontal = xml_doc.ReadAttribBool(path, index, "horz");
	LPCSTR mode_str = xml_doc.ReadAttrib(path, index, "mode");
	if (horizontal) // om_horz
	{
		mode = CUIProgressBar::om_horz;
	}
	else if (stricmp(mode_str, "horz") == 0) { mode = CUIProgressBar::om_horz; }
	else if (stricmp(mode_str, "vert") == 0) { mode = CUIProgressBar::om_vert; }
	else if (stricmp(mode_str, "back") == 0) { mode = CUIProgressBar::om_back; }
	else if (stricmp(mode_str, "down") == 0) { mode = CUIProgressBar::om_down; }
	else if (stricmp(mode_str, "from_center") == 0) { mode = CUIProgressBar::om_fromcenter; }
	else if (stricmp(mode_str, "vert_from_center") == 0) { mode = CUIProgressBar::om_vfromcenter; }

	pWnd->InitProgressBar(pos, size, mode);

	float min = xml_doc.ReadAttribFlt(path, index, "min");
	float max = xml_doc.ReadAttribFlt(path, index, "max");
	float ppos = xml_doc.ReadAttribFlt(path, index, "pos");

	pWnd->SetRange(min, max);
	pWnd->SetProgressPos(ppos);
	pWnd->m_inertion = xml_doc.ReadAttribFlt(path, index, "inertion");
	pWnd->colorSmoothing = xml_doc.ReadAttribInt(path, index, "color_smoothing");
	// progress
	xr_strconcat(buf, path, ":progress");

	if (!xml_doc.NavigateToNode(buf, index))
		return false;

	InitStatic(xml_doc, buf, index, &pWnd->m_UIProgressItem);

	pWnd->m_UIProgressItem.SetWndSize(pWnd->GetWndSize());

	// background
	xr_strconcat(buf, path, ":background");

	if (xml_doc.NavigateToNode(buf, index))
	{
		InitStatic(xml_doc, buf, index, &pWnd->m_UIBackgroundItem);
		pWnd->m_bBackgroundPresent = true;
		pWnd->m_UIBackgroundItem.SetWndSize(pWnd->GetWndSize());
	}

	xr_strconcat(buf, path, ":min_color");

	if (xml_doc.NavigateToNode(buf, index))
	{
		pWnd->m_bUseColor = true;

		u32 color = GetColor(xml_doc, buf, index, 0xff);
		pWnd->m_minColor.set(color);

		xr_strconcat(buf, path, ":middle_color");

		color = GetColor(xml_doc, buf, index, 0xff);
		pWnd->m_middleColor.set(color);

		xr_strconcat(buf, path, ":max_color");

		color = GetColor(xml_doc, buf, index, 0xff);
		pWnd->m_maxColor.set(color);
	}

	return true;
}

bool CUIXmlInit::InitProgressShape(CXml& xml_doc, LPCSTR path, int index, CUIProgressShape* pWnd)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	InitStatic(xml_doc, path, index, pWnd);

	if (xml_doc.ReadAttribInt(path, index, "text"))
		pWnd->SetTextVisible(true);

	string256 _path;

	if (xml_doc.NavigateToNode(xr_strconcat(_path, path, ":back"), index))
	{
		R_ASSERT2(0, "unused <back> node in progress shape ");
	}

	if (xml_doc.NavigateToNode(xr_strconcat(_path, path, ":front"), index))
	{
		R_ASSERT2(0, "unused <front> node in progress shape ");
	}

	pWnd->m_sectorCount = xml_doc.ReadAttribInt(path, index, "sector_count", 8);
	pWnd->m_bClockwise = xml_doc.ReadAttribBool(path, index, "clockwise");

	pWnd->m_blend = xml_doc.ReadAttribBool(path, index, "blend", true);
	pWnd->m_angle_begin = xml_doc.ReadAttribFlt(path, index, "begin_angle");
	pWnd->m_angle_end = xml_doc.ReadAttribFlt(path, index, "end_angle", PI_MUL_2);

	return true;
}

void CUIXmlInit::InitAutoStaticGroup(CXml& xml_doc, LPCSTR path, int index, CUIWindow* pParentWnd)
{
	XML_NODE* _stored_root = xml_doc.GetLocalRoot();
	xml_doc.SetLocalRoot(xml_doc.NavigateToNode(path, index));

	XML_NODE* curr_root = xml_doc.GetLocalRoot();
	if (!curr_root)
		curr_root = xml_doc.GetRoot();

	XML_NODE* node = curr_root->FirstChildElement(); // = curr_root->IterateChildren(NULL);

	int cnt_static = 0;
	int cnt_frameline = 0;
	int cnt_text = 0;
	string512 buff;

	while (node)
	{
		LPCSTR node_name = node->Value();
		if (!stricmp(node_name, "auto_static"))
		{
			CUIStatic* pUIStatic = new CUIStatic();
			InitStatic(xml_doc, "auto_static", cnt_static, pUIStatic);
			xr_sprintf(buff, "auto_static_%d", cnt_static);
			pUIStatic->SetWindowName(buff);
			pUIStatic->SetAutoDelete(true);
			pParentWnd->AttachChild(pUIStatic);

			++cnt_static;
		}
		else if (!stricmp(node_name, "auto_frameline"))
		{
			CUIFrameLineWnd* pUIFrameline = new CUIFrameLineWnd();
			InitFrameLine(xml_doc, "auto_frameline", cnt_frameline, pUIFrameline);
			xr_sprintf(buff, "auto_frameline_%d", cnt_frameline);
			pUIFrameline->SetWindowName(buff);
			pUIFrameline->SetAutoDelete(true);
			pParentWnd->AttachChild(pUIFrameline);

			++cnt_frameline;
		}
		else if (!stricmp(node_name, "auto_text"))
			++cnt_text;

		//FX: tinyxml::IterateChildren code:
		R_ASSERT(node->Parent() == curr_root);
		node = node->NextSiblingElement();
		// end
	}
	xml_doc.SetLocalRoot(_stored_root);
}

void CUIXmlInit::InitAutoFrameLineGroup(CXml& xml_doc, LPCSTR path, int index, CUIWindow* pParentWnd)
{
	int items_num = xml_doc.GetNodesNum(path, index, "auto_frameline");
	if (items_num == 0)
	{
		return;
	}
	XML_NODE* _stored_root = xml_doc.GetLocalRoot();
	xml_doc.SetLocalRoot(xml_doc.NavigateToNode(path, index));

	CUIFrameLineWnd* pUIFL = nullptr;
	string64 sname;
	for (int i = 0; i < items_num; ++i)
	{
		pUIFL = new CUIFrameLineWnd();
		InitFrameLine(xml_doc, "auto_frameline", i, pUIFL);
		xr_sprintf(sname, "auto_frameline_%d", i);
		pUIFL->SetWindowName(sname);
		pUIFL->SetAutoDelete(true);
		pParentWnd->AttachChild(pUIFL);
		pUIFL = nullptr;
	}

	xml_doc.SetLocalRoot(_stored_root);
}

bool CUIXmlInit::InitFont(CXml& xml_doc, LPCSTR path, int index, u32& color, CGameFont*& pFnt)
{
	color = GetColor(xml_doc, path, index, 0xff);

	LPCSTR font_name = xml_doc.ReadAttrib(path, index, "font", nullptr);
	if (!font_name)
	{
		pFnt = nullptr;
		return false;
	}
	else
	{
		if (!xr_strcmp(font_name, GRAFFITI19_FONT_NAME))
		{
			pFnt = UI().Font().GetFont("ui_font_graffiti19_russian");
		}
		else if (!xr_strcmp(font_name, GRAFFITI22_FONT_NAME))
		{
			pFnt = UI().Font().GetFont("ui_font_graffiti22_russian");
		}
		else if (!xr_strcmp(font_name, GRAFFITI32_FONT_NAME))
		{
			pFnt = UI().Font().GetFont("ui_font_graff_32");
		}
		else if (!xr_strcmp(font_name, GRAFFITI50_FONT_NAME))
		{
			pFnt = UI().Font().GetFont("ui_font_graff_50");
		}
		else if (!xr_strcmp(font_name, ARIAL14_FONT_NAME))
		{
			pFnt = UI().Font().GetFont("ui_font_arial_14");
		}
		else if (!xr_strcmp(font_name, ARIAL21_FONT_NAME))
		{
			pFnt = UI().Font().GetFont("ui_font_arial_21");
		}
		else if (!xr_strcmp(font_name, MEDIUM_FONT_NAME))
		{
			pFnt = UI().Font().pFontMedium;
		}
		else if (!xr_strcmp(font_name, SMALL_FONT_NAME))
		{
			pFnt = UI().Font().GetFont("stat_font");
		}
		else if (!xr_strcmp(font_name, LETTERICA16_FONT_NAME))
		{
			pFnt = UI().Font().GetFont("ui_font_letterica16_russian");
		}
		else if (!xr_strcmp(font_name, LETTERICA18_FONT_NAME))
		{
			pFnt = UI().Font().GetFont("ui_font_letterica18_russian");
		}
		else if (!xr_strcmp(font_name, LETTERICA25_FONT_NAME))
		{
			pFnt = UI().Font().GetFont("ui_font_letter_25");
		}
		else if (!xr_strcmp(font_name, DI_FONT_NAME))
		{
			pFnt = UI().Font().pFontDI;
		}
		else
		{
			pFnt = UI().Font().GetFont(font_name);
		}
	}
	return true;
}

bool CUIXmlInit::InitTabControl(CXml& xml_doc, LPCSTR path, int index, CUITabControl* pWnd)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	bool status = true;

	status &= InitWindow(xml_doc, path, index, pWnd);
	InitOptionsItem(xml_doc, path, index, pWnd);
	int tabsCount = xml_doc.GetNodesNum(path, index, "button");
	int radio = xml_doc.ReadAttribInt(path, index, "radio");

	XML_NODE* tab_node = xml_doc.NavigateToNode(path, index);
	xml_doc.SetLocalRoot(tab_node);

	CUITabButton* newButton;

	for (int i = 0; i < tabsCount; ++i)
	{
		newButton = radio ? new CUIRadioButton() : new CUITabButton();
		status &= Init3tButton(xml_doc, "button", i, newButton);
		newButton->m_btn_id = xml_doc.ReadAttrib("button", i, "id");
		R_ASSERT3(newButton->m_btn_id.size(), xml_doc.m_xml_file_name, path);
		pWnd->AddItem(newButton);
	}

	xml_doc.SetLocalRoot(xml_doc.GetRoot());

	return status;
}

bool CUIXmlInit::InitFrameLine(CXml& xml_doc, LPCSTR path, int index, CUIFrameLineWnd* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);
	R_ASSERT(!xml_doc.ReadAttribBool(path, index, "stretch"));

	string256 buf;

	Fvector2 pos, size;
	pos.x = xml_doc.ReadAttribFlt(path, index, "x");
	pos.y = xml_doc.ReadAttribFlt(path, index, "y");

	InitAlignment(xml_doc, path, index, pos.x, pos.y, pWnd);

	size.x = xml_doc.ReadAttribFlt(path, index, "width");
	size.y = xml_doc.ReadAttribFlt(path, index, "height");
	bool vertical = xml_doc.ReadAttribBool(path, index, "vertical");

	xr_strconcat(buf, path, ":texture");
	shared_str base_name = xml_doc.Read(buf, index, nullptr);

	VERIFY(base_name);

	u32 color = GetColor(xml_doc, buf, index, 0xff);
	pWnd->SetTextureColor(color);

	InitWindow(xml_doc, path, index, pWnd);
	pWnd->InitFrameLineWnd(*base_name, pos, size, !vertical);
	return true;
}

bool CUIXmlInit::InitCustomEdit(CXml& xml_doc, LPCSTR path, int index, CUICustomEdit* pWnd)
{
	InitStatic(xml_doc, path, index, pWnd);
	pWnd->InitCustomEdit(pWnd->GetWndPos(), pWnd->GetWndSize());

	string256 foo;
	u32 color;
	xr_strconcat(foo, path, ":text_color:e");
	if (xml_doc.NavigateToNode(foo, index))
	{
		color = GetColor(xml_doc, foo, index, 0x00);
		pWnd->TextItemControl()->SetTextColor(color);
	}

	int max_count = xml_doc.ReadAttribInt(path, index, "max_symb_count");
	bool num_only = xml_doc.ReadAttribBool(path, index, "num_only");
	bool read_only = xml_doc.ReadAttribBool(path, index, "read_only");
	bool file_name_mode = xml_doc.ReadAttribBool(path, index, "file_name_mode");

	if (file_name_mode || read_only || num_only || 0 < max_count)
	{
		if (max_count <= 0)
		{
			max_count = 32;
		}
		pWnd->Init(max_count, num_only, read_only, file_name_mode);
	}
	return true;
}

bool CUIXmlInit::InitEditBox(CXml& xml_doc, LPCSTR path, int index, CUIEditBox* pWnd)
{
	InitCustomEdit(xml_doc, path, index, pWnd);

	InitTexture(xml_doc, path, index, pWnd);
	InitOptionsItem(xml_doc, path, index, pWnd);

	return true;
}

bool CUIXmlInit::InitAnimatedStatic(CXml&xml_doc, const char* path, int index, CUIAnimatedStatic* pWnd)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	InitStatic(xml_doc, path, index, pWnd);

	float x = xml_doc.ReadAttribFlt(path, index, "x_offset");
	float y = xml_doc.ReadAttribFlt(path, index, "y_offset");
	u32 framesCount = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "frames"));
	u32 animDuration = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "duration"));
	u32 animCols = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "columns"));
	float frameWidth = xml_doc.ReadAttribFlt(path, index, "frame_width");
	float frameHeight = xml_doc.ReadAttribFlt(path, index, "frame_height");
	bool cyclic = xml_doc.ReadAttribBool(path, index, "cyclic");
	bool play = xml_doc.ReadAttribBool(path, index, "autoplay");

	pWnd->SetFrameDimentions(frameWidth, frameHeight);
	pWnd->SetFramesCount(framesCount);
	pWnd->m_bCyclic = cyclic;
	pWnd->SetAnimCols(animCols);
	pWnd->SetAnimationDuration(animDuration);
	pWnd->SetOffset(x, y);
	pWnd->SetAnimPos(0.0f);

	if (play)
		pWnd->Play();

	return true;
}

bool CUIXmlInit::InitSleepStatic(CXml& xml_doc, const char* path, int index, CUISleepStatic* pWnd)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	InitStatic(xml_doc, path, index, pWnd);

	return true;
}

bool CUIXmlInit::InitTexture(CXml& xml_doc, const char* path, int index, ITextureOwner* pWnd)
{
	int nodes_num = xml_doc.GetNodesNum(path, index, "texture");
	if (nodes_num > 0)
	{
		XML_NODE* root = xml_doc.GetLocalRoot();
		xml_doc.SetLocalRoot(xml_doc.NavigateToNode(path, index));

		int ind = Random.randI(nodes_num);

		const char* texture = xml_doc.Read("texture", ind, nullptr);
		const char* shader = xml_doc.ReadAttrib("texture", ind, "shader", nullptr);

		if (texture)
		{
			if (shader)
				pWnd->InitTextureEx(texture, shader);
			else
				pWnd->InitTexture(texture);
		}

		Frect rect;
		rect.x1 = xml_doc.ReadAttribFlt("texture", ind, "x");
		rect.y1 = xml_doc.ReadAttribFlt("texture", ind, "y");
		rect.x2 = rect.x1 + xml_doc.ReadAttribFlt("texture", ind, "width");
		rect.y2 = rect.y1 + xml_doc.ReadAttribFlt("texture", ind, "height");

		if (rect.width() != 0 && rect.height() != 0)
			pWnd->SetTextureRect(rect);

		u32 color = GetColor(xml_doc, "texture", ind, 0xff);
		pWnd->SetTextureColor(color);

		xml_doc.SetLocalRoot(root);
	}
	pWnd->SetStretchTexture(xml_doc.ReadAttribBool(path, index, "stretch"));

	return true;
}

bool CUIXmlInit::InitMultiTexture(CXml& xml_doc, LPCSTR path, int index, CUI3tButton* pWnd)
{
	string256 buff;
	bool success = false;

	xr_strconcat(buff, path, ":texture");
	shared_str texture = xml_doc.Read(buff, index, nullptr);

	if (texture.size() > 0)
	{
		pWnd->InitTexture(*texture);
		return true;
	}

	xr_strconcat(buff, path, ":texture_e");
	texture = xml_doc.Read(buff, index, nullptr);
	if (texture.size())
	{
		if (pWnd->m_background)
		{
			pWnd->m_background->InitState(S_Enabled, texture.c_str());
		}
		else if (pWnd->m_back_frameline)
		{
			pWnd->m_back_frameline->InitState(S_Enabled, texture.c_str());
			pWnd->m_back_frameline->Get(S_Enabled)->SetHorizontal(!(pWnd->vertical));
		}
		success = true;
	}

	xr_strconcat(buff, path, ":texture_t");
	texture = xml_doc.Read(buff, index, nullptr);
	if (texture.size())
	{
		if (pWnd->m_background)
		{
			pWnd->m_background->InitState(S_Touched, texture.c_str());
		}
		else if (pWnd->m_back_frameline)
		{
			pWnd->m_back_frameline->InitState(S_Touched, texture.c_str());
			pWnd->m_back_frameline->Get(S_Touched)->SetHorizontal(!(pWnd->vertical));
		}
		success = true;
	}

	xr_strconcat(buff, path, ":texture_d");
	texture = xml_doc.Read(buff, index, nullptr);
	if (texture.size())
	{
		if (pWnd->m_background)
		{
			pWnd->m_background->InitState(S_Disabled, texture.c_str());
		}
		else if (pWnd->m_back_frameline)
		{
			pWnd->m_back_frameline->InitState(S_Disabled, texture.c_str());
			pWnd->m_back_frameline->Get(S_Disabled)->SetHorizontal(!(pWnd->vertical));
		}
		success = true;
	}

	xr_strconcat(buff, path, ":texture_h");
	texture = xml_doc.Read(buff, index, nullptr);
	if (texture.size())
	{
		if (pWnd->m_background)
		{
			pWnd->m_background->InitState(S_Highlighted, texture.c_str());
		}
		else if (pWnd->m_back_frameline)
		{
			pWnd->m_back_frameline->InitState(S_Highlighted, texture.c_str());
			pWnd->m_back_frameline->Get(S_Highlighted)->SetHorizontal(!(pWnd->vertical));
		}
		success = true;
	}

	if (success)
		pWnd->TextureOn();

	return success;
}

float CUIXmlInit::ApplyAlignX(float coord, u32 align)
{
	return coord;
}

float CUIXmlInit::ApplyAlignY(float coord, u32 align)
{
	return coord;
}

void CUIXmlInit::ApplyAlign(float& x, float& y, u32 align)
{
	x = ApplyAlignX(x, align);
	y = ApplyAlignY(y, align);
}

bool CUIXmlInit::InitAlignment(CXml& xml_doc, const char* path, int index, float& x, float& y, CUIWindow* pWnd)
{
	xr_string wnd_alignment = xml_doc.ReadAttrib(path, index, "alignment");

	if (strchr(wnd_alignment.c_str(), 'c'))
		pWnd->SetAlignment(waCenter);

	// Alignment: right: "r", bottom: "b". Top, left - useless
	shared_str alignStr = xml_doc.ReadAttrib(path, index, "align");

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
}

void CUIXmlInit::InitColorDefs()
{
	if (m_pColorDefs) return;

	m_pColorDefs = new ColorDefs();

	CXml uiXml;
	uiXml.Load(CONFIG_PATH, UI_PATH, COLOR_DEFINITIONS);

	int num = uiXml.GetNodesNum("colors", 0, "color");

	shared_str name;
	int r, b, g, a;

	for (int i = 0; i < num; ++i)
	{
		name = uiXml.ReadAttrib("color", i, "name");
		r = uiXml.ReadAttribInt("color", i, "r");
		g = uiXml.ReadAttribInt("color", i, "g");
		b = uiXml.ReadAttribInt("color", i, "b");
		a = uiXml.ReadAttribInt("color", i, "a", 255);

		(*m_pColorDefs)[name] = color_argb(a, r, g, b);
	}
}

bool CUIXmlInit::InitScrollView(CXml& xml_doc, LPCSTR path, int index, CUIScrollView* pWnd)
{
	R_ASSERT4(xml_doc.NavigateToNode(path, index), "XML node not found", path, xml_doc.m_xml_file_name);

	InitWindow(xml_doc, path, index, pWnd);
	pWnd->SetRightIndention(xml_doc.ReadAttribFlt(path, index, "right_ident"));
	pWnd->SetLeftIndention(xml_doc.ReadAttribFlt(path, index, "left_ident"));
	pWnd->SetUpIndention(xml_doc.ReadAttribFlt(path, index, "top_indent"));
	pWnd->SetDownIndention(xml_doc.ReadAttribFlt(path, index, "bottom_indent"));
	pWnd->m_vertInterval = xml_doc.ReadAttribFlt(path, index, "vert_interval");
	pWnd->m_flags.set(CUIScrollView::eInverseDir, xml_doc.ReadAttribBool(path, index, "inverse_dir"));
	pWnd->SetScrollBarProfile(xml_doc.ReadAttrib(path, index, "scroll_profile", "default"));
	pWnd->InitScrollView();
	pWnd->SetVertFlip(xml_doc.ReadAttribBool(path, index, "flip_vert"));
	pWnd->SetFixedScrollBar(xml_doc.ReadAttribBool(path, index, "always_show_scroll", true));
	pWnd->m_flags.set(CUIScrollView::eItemsSelectabe, xml_doc.ReadAttribBool(path, index, "can_select"));

	int tabsCount = xml_doc.GetNodesNum(path, index, "text");

	XML_NODE* _stored_root = xml_doc.GetLocalRoot();
	xml_doc.SetLocalRoot(xml_doc.NavigateToNode(path, index));

	for (int i = 0; i < tabsCount; ++i)
	{
		CUITextWnd* newText = new CUITextWnd();
		InitText(xml_doc, "text", i, &newText->TextItemControl());
		newText->SetTextComplexMode(true);
		newText->SetWidth(pWnd->GetDesiredChildWidth());
		newText->AdjustHeightToText();
		pWnd->AddWindow(newText, true);
	}
	xml_doc.SetLocalRoot(_stored_root);
	return true;
}

bool CUIXmlInit::InitListBox(CXml& xml_doc, LPCSTR path, int index, CUIListBox* pWnd)
{
	InitScrollView(xml_doc, path, index, pWnd);

	string512 _path;
	u32 t_color;
	CGameFont* pFnt;
	xr_strconcat(_path, path, ":font");
	InitFont(xml_doc, _path, index, t_color, pFnt);

	pWnd->SetTextColor(t_color);
	pWnd->SetFont(pFnt);

	float h = xml_doc.ReadAttribFlt(path, index, "item_height", 20.0f);
	pWnd->SetItemHeight(h);
	return true;
}

bool CUIXmlInit::InitTrackBar(CXml& xml_doc, LPCSTR path, int index, CUITrackBar* pWnd)
{
	InitWindow(xml_doc, path, 0, pWnd);
	pWnd->InitTrackBar(pWnd->GetWndPos(), pWnd->GetWndSize());
	pWnd->SetType(!xml_doc.ReadAttribBool(path, index, "is_integer"));
	InitOptionsItem(xml_doc, path, 0, pWnd);

	pWnd->SetInvert(xml_doc.ReadAttribBool(path, index, "invert"));
	pWnd->SetStep(xml_doc.ReadAttribFlt(path, index, "step", 0.1f));

	return true;
}

bool CUIXmlInit::InitComboBox(CXml& xml_doc, LPCSTR path, int index, CUIComboBox* pWnd)
{
	u32	color;
	CGameFont* pFont;

	pWnd->SetListLength(xml_doc.ReadAttribInt(path, index, "list_length", 4));

	InitWindow(xml_doc, path, index, pWnd);
	pWnd->InitComboBox(pWnd->GetWndPos(), pWnd->GetWidth());
	InitOptionsItem(xml_doc, path, index, pWnd);

	pWnd->m_list_box.SetFixedScrollBar(xml_doc.ReadAttribBool(path, index, "always_show_scroll", true));

	string512 _path;
	xr_strconcat(_path, path, ":list_font");
	InitFont(xml_doc, _path, index, color, pFont);
	pWnd->m_list_box.SetFont(pFont);
	pWnd->m_list_box.SetTextColor(color);

	xr_strconcat(_path, path, ":text_color:e");
	if (xml_doc.NavigateToNode(_path, index))
	{
		color = GetColor(xml_doc, _path, index, 0x00);
		pWnd->SetTextColor(color);
	}

	xr_strconcat(_path, path, ":text_color:d");
	if (xml_doc.NavigateToNode(_path, index))
	{
		color = GetColor(xml_doc, _path, index, 0x00);
		pWnd->SetTextColorD(color);
	}

	return true;
}

void CUIXmlInit::AssignColor(LPCSTR name, u32 clr)
{
	(*m_pColorDefs)[name] = clr;
}

u32	CUIXmlInit::GetColor(CXml& xml_doc, LPCSTR path, int index, u32 def_clr)
{
	LPCSTR clr_def = xml_doc.ReadAttrib(path, index, "color", nullptr);
	if (clr_def)
	{
		VERIFY(GetColorDefs()->find(clr_def) != GetColorDefs()->end());
		return (*m_pColorDefs)[clr_def];
	}
	else
	{
		int r = xml_doc.ReadAttribInt(path, index, "r", def_clr);
		int g = xml_doc.ReadAttribInt(path, index, "g", def_clr);
		int b = xml_doc.ReadAttribInt(path, index, "b", def_clr);
		int a = xml_doc.ReadAttribInt(path, index, "a", 0xff);
		return color_argb(a, r, g, b);
	}
}

bool CUIXmlInit::InitHintWindow(CXml& xml_doc, LPCSTR path, int index, UIHintWindow* pWnd)
{
	VERIFY(pWnd);
	InitWindow(xml_doc, path, index, pWnd);
	pWnd->set_hint_text_ST(xml_doc.Read(path, index, "no hint"));
	pWnd->set_hint_delay((u32)xml_doc.ReadAttribInt(path, index, "delay"));
	return true;
}