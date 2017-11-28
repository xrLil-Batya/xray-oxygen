#pragma once
#include "UIStatic.h"

class CUIXml;

class CUIMoneyIndicator: public CUIWindow 
{
public:
						CUIMoneyIndicator		();
	virtual				~CUIMoneyIndicator		() = default;
	virtual void 		Update					();
			void 		InitFromXML				(CUIXml& xml_doc);
			void 		SetMoneyAmount			(LPCSTR money);
			void 		SetMoneyChange			(LPCSTR money);

protected:
	CUIStatic			m_back;
	CUITextWnd			m_money_amount;
	CUITextWnd			m_money_change;
};