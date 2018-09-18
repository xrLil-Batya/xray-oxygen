////////////////////////////////////////////////////////////////////////////
//	Module 		: script_property_evaluator_wrapper.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script property evaluator wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "luabind/wrapper_base.hpp"
#include "property_evaluator.h"

class CScriptGameObject;

using CScriptPropertyEvaluator = CPropertyEvaluator<CScriptGameObject>;

class CScriptPropertyEvaluatorWrapper : public CScriptPropertyEvaluator, public luabind::wrap_base {
public:
	IC					CScriptPropertyEvaluatorWrapper	(CScriptGameObject *object = nullptr, LPCSTR evaluator_name = "");
	virtual void		setup						(CScriptGameObject *object, CPropertyStorage *storage);
	static	void		setup_static				(CScriptPropertyEvaluator *evaluator, CScriptGameObject *object, CPropertyStorage *storage);
	virtual bool		evaluate					();
	static	bool		evaluate_static				(CScriptPropertyEvaluator *evaluator);
};

#include "script_property_evaluator_wrapper_inline.h"