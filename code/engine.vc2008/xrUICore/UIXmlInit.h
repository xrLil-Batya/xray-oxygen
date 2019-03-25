#pragma once
#include "linker.h"
#include "../xrCore/XMLCore/xrXMLParser.h"

class ITextureOwner;
class CUIWindow;
class CUIFrameWindow;
class CUIStaticItem;
class CUIStatic;
class CUICheckButton;
class CUICustomSpin;
class CUI3tButton;
class CUIProgressBar;
class CUIProgressShape;
class CUITabControl;
class CUIFrameLineWnd;
class CUIEditBoxEx;
class CUIEditBox;
class CUICustomEdit;
class CUIAnimatedStatic;
class CUISleepStatic;
class CUIOptionsItem;
class CUIScrollView;
class CUIListBox;
class CUIStatsPlayerList;
class CUIDragDropListEx;
class CUIComboBox;
class CUITrackBar;
class UIHintWindow;
class CUILines;
class CUITextWnd;

class UI_API CUIXmlInit
{
public:
	// Initialize and store predefined colors
	using ColorDefs = xr_map<shared_str, u32>;

private:
	static ColorDefs* m_pColorDefs;

public:
	CUIXmlInit();
	virtual ~CUIXmlInit();

	static bool InitWindow(CXml& xml_doc, LPCSTR path, int index, CUIWindow* pWnd);
	static bool InitFrameWindow(CXml& xml_doc, LPCSTR path, int index, CUIFrameWindow* pWnd);
	static bool InitFrameLine(CXml& xml_doc, LPCSTR path, int index, CUIFrameLineWnd* pWnd);
	static bool InitCustomEdit(CXml& xml_doc, LPCSTR path, int index, CUICustomEdit* pWnd);
	static bool InitEditBox(CXml& xml_doc, LPCSTR path, int index, CUIEditBox* pWnd);
	static bool InitStatic(CXml& xml_doc, LPCSTR path, int index, CUIStatic* pWnd);
	static bool InitTextWnd(CXml& xml_doc, LPCSTR path, int index, CUITextWnd* pWnd);
	static bool InitCheck(CXml& xml_doc, LPCSTR path, int index, CUICheckButton* pWnd);
	static bool InitSpin(CXml& xml_doc, LPCSTR path, int index, CUICustomSpin* pWnd);
	static bool InitText(CXml& xml_doc, LPCSTR path, int index, CUIStatic* pWnd);
	static bool InitText(CXml& xml_doc, LPCSTR path, int index, CUILines* pLines);
	static bool Init3tButton(CXml& xml_doc, LPCSTR path, int index, CUI3tButton* pWnd);
	static bool InitDragDropListEx(CXml& xml_doc, LPCSTR path, int index, CUIDragDropListEx* pWnd);
	static bool InitProgressBar(CXml& xml_doc, LPCSTR path, int index, CUIProgressBar* pWnd);
	static bool InitProgressShape(CXml& xml_doc, LPCSTR path, int index, CUIProgressShape* pWnd);
	static bool InitFont(CXml& xml_doc, LPCSTR path, int index, u32& color, CGameFont*& pFnt);
	static bool InitTabControl(CXml& xml_doc, LPCSTR path, int index, CUITabControl *pWnd);
	static bool InitAnimatedStatic(CXml& xml_doc, LPCSTR path, int index, CUIAnimatedStatic *pWnd);
	static bool InitSleepStatic(CXml& xml_doc, LPCSTR path, int index, CUISleepStatic *pWnd);
	static bool InitTextureOffset(CXml& xml_doc, LPCSTR path, int index, CUIStatic* pWnd);
	static bool InitSound(CXml& xml_doc, LPCSTR path, int index, CUI3tButton* pWnd);
	static bool InitMultiTexture(CXml& xml_doc, LPCSTR path, int index, CUI3tButton* pWnd);
	static bool InitTexture(CXml& xml_doc, LPCSTR path, int index, ITextureOwner* pWnd);
	static bool InitOptionsItem(CXml& xml_doc, LPCSTR path, int index, CUIOptionsItem* pWnd);
	static bool InitScrollView(CXml& xml_doc, LPCSTR path, int index, CUIScrollView* pWnd);
	static bool InitListBox(CXml& xml_doc, LPCSTR path, int index, CUIListBox* pWnd);
	static bool InitComboBox(CXml& xml_doc, LPCSTR path, int index, CUIComboBox* pWnd);
	static bool InitTrackBar(CXml& xml_doc, LPCSTR path, int index, CUITrackBar* pWnd);
	static bool InitHintWindow(CXml& xml_doc, LPCSTR path, int index, UIHintWindow* pWnd);
	static Frect GetFRect(CXml& xml_doc, LPCSTR path, int index);
	static u32 GetColor(CXml& xml_doc, LPCSTR path, int index, u32 def_clr);
	static bool InitAlignment(CXml& xml_doc, const char* path, int index, float& x, float& y, CUIWindow* pWnd);
	static void InitAutoStaticGroup(CXml& xml_doc, LPCSTR path, int index, CUIWindow* pParentWnd);
	static void InitAutoFrameLineGroup(CXml& xml_doc, LPCSTR path, int index, CUIWindow* pParentWnd);

	static float ApplyAlignX(float coord, u32 align);
	static float ApplyAlignY(float coord, u32 align);
	static void ApplyAlign(float& x, float& y, u32 align);

	static const ColorDefs* GetColorDefs() { R_ASSERT(m_pColorDefs); return m_pColorDefs; }

	static void InitColorDefs();
	static void DeleteColorDefs() { xr_delete(m_pColorDefs); }
	static void AssignColor(LPCSTR name, u32 clr);
};
