#pragma once

extern "C" {
	typedef __declspec(dllimport)  ISE_Abstract*	__stdcall Factory_Create	(LPCSTR section);
	typedef __declspec(dllimport)  void				__stdcall Factory_Destroy	(ISE_Abstract *&);
};
#include "../xrSE_Factory/xrSE_Factory_import_export.h"

CSE_Abstract *F_entity_Create(LPCSTR section);

void F_entity_Destroy(CSE_Abstract *&i);
