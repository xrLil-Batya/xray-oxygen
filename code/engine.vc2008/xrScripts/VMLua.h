#pragma once
#include "xrScripts.h"
#include "../xrServerEntities/script_storage_space.h"
// Forward Lua
extern "C"
{
	struct lua_State;
}

struct SCRIPT_API raii_guard
{
	int				   m_error_code;
	const char* const& m_error_description;

	raii_guard(int error_code, const char* const& m_description) : m_error_code(error_code), m_error_description(m_description) {}
	raii_guard(const raii_guard& other) = delete;
	raii_guard& operator=(const raii_guard& other) = delete;
	~raii_guard()
	{
		static bool const break_on_assert = true;
		if (!m_error_code)
			return;

		if (break_on_assert)
			R_ASSERT2(!m_error_code, m_error_description);
		else
			Msg("! SCRIPT ERROR: %s", m_error_description);
	}
}; // struct raii_guard

class SCRIPT_API CVMLua
{
	CVMLua(const CVMLua &) = delete;
	CVMLua &operator= (const CVMLua &) = delete;

	lua_State * m_virtual_machine;

public:
	using AddFun = void(__cdecl*)(lua_State* luaState);
	using ELuaMessageType = ScriptStorage::ELuaMessageType;

public:
					CVMLua();
	virtual			~CVMLua();

	// Returned lua virtual machine
inline lua_State*	LSVM() { return this->m_virtual_machine; }
	void			Add(AddFun pFun);

	static void		ScriptLog(ELuaMessageType tLuaMessageType, const char* caFormat, ...);
	static void		PrintError(lua_State *L, int iErrorCode);
	static bool		PrintOut(lua_State *L, const char* caScriptFileName, int iErrorCode, const char* caErrorText);
	bool			GetNamespaceTable(LPCSTR N);
	bool			IsObjectPresent(const char* identifier, int type);
	bool			IsObjectPresent(LPCSTR namespace_name, LPCSTR identifier, int type);
	bool			LoadFileIntoNamespace(LPCSTR caScriptName, LPCSTR caNamespaceName, bool bCall);

private:
	// Open lua namespaces
	void			OpenLib();
	void			CopyGlobals();
	void			SetNamespace();
	// Can create namespace and create it
	bool			CreateNamespaceTable(LPCSTR caNamespaceName);
	bool			LoadBuffer(LPCSTR caBuffer, size_t tSize, LPCSTR caScriptName, LPCSTR caNameSpaceName);
	bool			DoFile(LPCSTR caScriptName, LPCSTR caNameSpaceName, bool bCall);
    static void     luabind_onerror(lua_State* lua);
};

SCRIPT_API void setup_luabind_allocator();