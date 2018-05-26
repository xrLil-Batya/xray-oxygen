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
					CVMLua();
	virtual			~CVMLua();

	// Returned lua virtual machine
inline lua_State*	LSVM() { return this->m_virtual_machine; }

private:
	// Open lua namespaces
	void			OpenLib();
};

extern SCRIPT_API CVMLua* LVM;