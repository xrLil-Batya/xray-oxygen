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

private:
	// Open lua namespaces
	void			OpenLib();
};

extern SCRIPT_API CVMLua* LVM;