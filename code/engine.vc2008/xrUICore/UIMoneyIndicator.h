#pragma once
#include "UIStatic.h"
class CXml;

class UI_API CUIMoneyIndicator: public CUIWindow 
{
public:
						CUIMoneyIndicator		();
	virtual				~CUIMoneyIndicator		() = default;
	virtual void 		Update					();
			void 		InitFromXML				(CXml& xml_doc);
			void 		SetMoneyAmount			(LPCSTR money);
			void 		SetMoneyChange			(LPCSTR money);

protected:
	CUIStatic			m_back;
	CUITextWnd			m_money_amount;
	CUITextWnd			m_money_change;
};