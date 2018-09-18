////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_wrapper.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_action_wrapper.h"
#include "script_game_object.h"
#include "ai_space.h"
#include "script_engine.h"
#include <luabind/luabind.hpp>

void CScriptActionWrapper::setup				(CScriptGameObject *object, CPropertyStorage *storage)
{
	luabind::call_member<void>			(this,"setup",object,storage);
}

void CScriptActionWrapper::setup_static			(CScriptActionBase *action, CScriptGameObject *object, CPropertyStorage *storage)
{
	action->CScriptActionBase::setup		(object,storage);
}

void CScriptActionWrapper::initialize			()
{
	luabind::call_member<void>			(this,"initialize");
}

void CScriptActionWrapper::initialize_static	(CScriptActionBase *action)
{
	action->CScriptActionBase::initialize	();
}

void CScriptActionWrapper::execute				()
{
	luabind::call_member<void>			(this,"execute");
}

void CScriptActionWrapper::execute_static		(CScriptActionBase *action)
{
	action->CScriptActionBase::execute		();
}

void CScriptActionWrapper::finalize				()
{
	luabind::call_member<void>			(this,"finalize");
}

void CScriptActionWrapper::finalize_static		(CScriptActionBase *action)
{
	action->CScriptActionBase::finalize		();
}