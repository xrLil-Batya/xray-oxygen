#pragma once
#include "xrScripts.h"

// Forward Lua
extern "C"
{
	struct lua_State;
}

class SCRIPT_API CVMLua
{
	CVMLua(const CVMLua &) = delete;
	CVMLua &operator= (const CVMLua &) = delete;

protected:
	lua_State * m_virtual_machine;

public:
	using AddFun = void(__cdecl*)(lua_State* luaState);

public:
					CVMLua();
	virtual			~CVMLua();

	// Returned lua virtual machine
inline lua_State*	LSVM() { return this->m_virtual_machine; }
	void			Add(AddFun pFun);

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