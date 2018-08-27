///////////////////////////////////////////////////////////////
// Phrase.cpp
// класс, описывающий фразу (элемент диалога)
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "phrase.h"

#include "ai_space.h"
#include "gameobject.h"
#include "script_game_object.h"

CPhrase::CPhrase()
:m_b_finalizer(false),m_ID(""),m_iGoodwillLevel(0)
{
}

CPhrase::~CPhrase()
{
}

LPCSTR CPhrase::GetText()	const			
{
	return m_text.c_str();
}

LPCSTR CPhrase::GetScriptText()	const			
{
	return m_script_text_val.c_str();
}

bool	CPhrase::IsDummy()		const
{
	//	if( (xr_strlen(GetText()) == 0) && (xr_strlen( GetScriptText() ) == 0) )
	if( (m_text.empty()) && (m_script_text_val.empty()) && (m_script_text_id.empty()) )
	{
		return true;
	}

	return false;
}
