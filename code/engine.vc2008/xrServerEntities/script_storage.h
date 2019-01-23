////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../xrScripts/VMLua.h"
#include "script_storage_space.h"
#include "script_space_forward.h"

class CScriptThread;

#ifdef XRGAME_EXPORTS
#	ifndef MASTER_GOLD
#		define PRINT_CALL_STACK
#	endif // #ifndef MASTER_GOLD
#else // #ifdef XRGAME_EXPORTS
#	ifndef NDEBUG
#		define PRINT_CALL_STACK
#	endif // #ifndef NDEBUG
#endif // #ifdef XRGAME_EXPORTS

using namespace ScriptStorage;

class CScriptStorage
{
private:
    static CVMLua*				luaVM;
	CScriptThread*				m_current_thread	;
	BOOL						m_jit				;
    xr_set<void*>				m_dumpedObjList     ;

public:
protected:
			bool				parse_namespace				(const char* caNamespaceName, LPSTR b, u32 const b_size, LPSTR c, u32 const c_size);
			bool				do_file						(const char*	caScriptName, const char* caNameSpaceName);
			void				reinit						();

public:
			void				dump_state					();
								CScriptStorage				();
	virtual						~CScriptStorage				();
	IC		lua_State			*lua						();
	IC		void				current_thread				(CScriptThread *thread);
	IC		CScriptThread		*current_thread				() const;
			bool				load_buffer					(lua_State *L, const char* caBuffer, size_t tSize, const char* caScriptName, const char* caNameSpaceName = 0);
			bool				load_file_into_namespace	(const char*	caScriptName, const char* caNamespaceName);
			bool				namespace_loaded			(const char*	caName, bool remove_from_stack = true);
			bool				object						(const char*	caIdentifier, int type);
			bool				object						(const char*	caNamespaceName, const char*	caIdentifier, int type);
			luabind::object		name_space					(const char*	namespace_name);
			int					error_log					(const char*	caFormat, ...);
	static	int		__cdecl		script_log					(ELuaMessageType message,	const char*	caFormat, ...);
	static	bool				print_output				(lua_State *L, const char*	caScriptName, int iErorCode = 0, const char* caErrorText = "see call_stack for details!");
	static	void				print_error					(lua_State *L,		int		iErrorCode);
			void                LogTable                    (lua_State *l, const char* S, int level, int index = -1);
			void                LogVariable                 (lua_State * l, const char* name, int level, bool bOpenTable, int index = -1);
            void                ClearDumpedObjects          ();

#ifdef DEBUG
			void				flush_log					();
#endif // DEBUG
};

#include "script_storage_inline.h"