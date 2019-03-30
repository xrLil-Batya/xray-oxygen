#pragma once
#include "../xrGame/script_game_object.h"


namespace XRay
{
	public ref class ScriptGameObject
	{
	internal:
		CScriptGameObject* pNativeObject;



	public:
		ScriptGameObject(::System::IntPtr isNativeLevel);
		ScriptGameObject(CScriptGameObject* imp) : pNativeObject(imp) {};
	};
}