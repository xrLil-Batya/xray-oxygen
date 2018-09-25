#include "stdafx.h"
#include "PhraseScript.h"
#include "script_engine.h"
#include "ai_space.h"
#include "gameobject.h"
#include "script_game_object.h"
#include "infoportion.h"
#include "inventoryowner.h"
#include "../xrUICore/xrUIXmlParser.h"
#include "actor.h"
#include <luabind/luabind.hpp>
#ifdef DEBUG
#	include "ai_debug.h"
#endif

//загрузка из XML файла
void CDialogScriptHelper::Load		(CUIXml* uiXml, XML_NODE* phrase_node)
{
	LoadSequence(uiXml,phrase_node, "precondition",		m_Preconditions);
	LoadSequence(uiXml,phrase_node, "action",			m_ScriptActions);
	
	LoadSequence(uiXml,phrase_node, "has_info",			m_HasInfo);
	LoadSequence(uiXml,phrase_node, "dont_has_info",	m_DontHasInfo);

	LoadSequence(uiXml,phrase_node, "give_info",		m_GiveInfo);
	LoadSequence(uiXml,phrase_node, "disable_info",		m_DisableInfo);
}

template<class T> 
void  CDialogScriptHelper::LoadSequence (CUIXml* uiXml, XML_NODE* phrase_node, 
								  const char* tag, T&  str_vector)
{
	int tag_num = uiXml->GetNodesNum(phrase_node, tag);
	str_vector.clear();
	for(int i=0; i<tag_num; ++i)
	{
		const char* tag_text = uiXml->Read(phrase_node, tag, i, nullptr);
		str_vector.push_back(tag_text);
	}
}

bool  CDialogScriptHelper::CheckInfo(const CInventoryOwner* pOwner) const
{
	THROW(pOwner);

	for(const auto & i : m_HasInfo) 
	{
		if (!Actor()->HasInfo(i)) {
#ifdef DEBUG
			if(psAI_Flags.test(aiDialogs) )
				Msg("----rejected: [%s] has info %s", pOwner->Name(), *i);
#endif
			return false;
		}
	}

	for(const auto & i : m_DontHasInfo) {
		if (Actor()->HasInfo(i)) {
#ifdef DEBUG
			if(psAI_Flags.test(aiDialogs) )
				Msg("----rejected: [%s] dont has info %s", pOwner->Name(), *i);
#endif
			return false;
		}
	}
	return true;
}


void  CDialogScriptHelper::TransferInfo	(const CInventoryOwner* pOwner) const
{
	THROW(pOwner);

	for(const auto & i : m_GiveInfo)
		Actor()->TransferInfo(i, true);

	for(const auto & i : m_DisableInfo)
		Actor()->TransferInfo(i, false);
}

const char* CDialogScriptHelper::GetScriptText(const char* str_to_translate, const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2, const char* dialog_id, const char* phrase_id)
{
	if(!m_sScriptTextFunc.size())
		return str_to_translate;

	luabind::functor<const char*> lua_function;
	bool functor_exists = ai().script_engine().functor(m_sScriptTextFunc.c_str() ,lua_function);
	THROW3(functor_exists, "Cannot find phrase script text ", m_sScriptTextFunc.c_str());

	const char* res = functor_exists ? lua_function(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_id) : "";

	return res;

}

bool CDialogScriptHelper::Precondition	(const CGameObject* pSpeakerGO, const char* dialog_id, const char* phrase_id) const 
{
	bool predicate_result = true;

	if(!CheckInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO)))
	{
		#ifdef DEBUG
			if (psAI_Flags.test(aiDialogs))
				Msg("dialog [%s] phrase[%s] rejected by CheckInfo",dialog_id,phrase_id);
		#endif
		return false;
	}

	for(const auto & i : Preconditions())
	{
		luabind::functor<bool>	lua_function;
		THROW(*i);
		bool functor_exists = ai().script_engine().functor(*i ,lua_function);
		THROW3(functor_exists, "Cannot find precondition", *i);
		predicate_result = lua_function	(pSpeakerGO->lua_game_object());
		if(!predicate_result){
		#ifdef DEBUG
			if (psAI_Flags.test(aiDialogs))
				Msg("dialog [%s] phrase[%s] rejected by script predicate", dialog_id, phrase_id);
		#endif
			break;
		} 
	}
	return predicate_result;
}

void CDialogScriptHelper::Action			(const CGameObject* pSpeakerGO, const char* dialog_id, const char* phrase_id) const 
{

	for(const auto & i : Actions())
	{
		luabind::functor<void>	lua_function;
		THROW(*i);
		bool functor_exists = ai().script_engine().functor(*i ,lua_function);
		THROW3(functor_exists, "Cannot find phrase dialog script function", *i);
		lua_function		(pSpeakerGO->lua_game_object(), dialog_id);
	}
	TransferInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO));
}

bool CDialogScriptHelper::Precondition(const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2, const char* dialog_id, const char* phrase_id, const char* next_phrase_id) const 
{
	bool predicate_result = true;

	if(!CheckInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO1))){
		#ifdef DEBUG
		if (psAI_Flags.test(aiDialogs))
			Msg("dialog [%s] phrase[%s] rejected by CheckInfo",dialog_id,phrase_id);
		#endif
		return false;
	}
	for(const auto & i : Preconditions())
	{
		luabind::functor<bool>	lua_function;
		THROW(*i);
		bool functor_exists = ai().script_engine().functor(*i ,lua_function);
		THROW3(functor_exists, "Cannot find phrase precondition", *i);
		if(functor_exists)
			predicate_result = lua_function	(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_id, next_phrase_id);
		
		if(!predicate_result)
		{
		#ifdef DEBUG
			if (psAI_Flags.test(aiDialogs))
				Msg("dialog [%s] phrase[%s] rejected by script predicate",dialog_id,phrase_id);
		#endif
			break;
		}
	}
	return predicate_result;
}

void CDialogScriptHelper::Action(const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2, const char* dialog_id, const char* phrase_id) const 
{
	TransferInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO1));

	for(const auto & i : Actions())
	{
		luabind::functor<void>	lua_function;
		THROW(*i);
		bool functor_exists = ai().script_engine().functor(*i ,lua_function);
		THROW3(functor_exists, "Cannot find phrase dialog script function", *i);
		if(functor_exists)
			lua_function		(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_id);
		
	}
}
