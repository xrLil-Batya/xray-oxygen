////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extension.h
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script extensions
////////////////////////////////////////////////////////////////////////////

#pragma once
#if defined(XR_SCRIPTS) || defined(ENGINE_BUILD) || defined(XRRENDER_R1_EXPORTS) || defined(XRRENDER_R2_EXPORTS)|| defined(XRRENDER_R3_EXPORTS) || defined(XRRENDER_R4_EXPORTS)
#	include "../xrScripts.h"
#else
#	define SCRIPT_API
#endif
#include "ai_script_space.h"
#include <Windows.h>
namespace Lua 
{
	SCRIPT_API int __cdecl LuaOut(ELuaMessageType tLuaMessageType, char* caFormat, ...);
}
namespace Script {
#if !defined(XR_SCRIPTS) && !defined(ENGINE_BUILD)
	void				vfExportGlobals				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportFvector				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportFmatrix				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportGame				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportLevel				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportDevice				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportParticles			(CLuaVirtualMachine *tpLuaVM);
	void				vfExportSound				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportHit					(CLuaVirtualMachine *tpLuaVM);
	void				vfExportActions				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportObject				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportEffector			(CLuaVirtualMachine *tpLuaVM);
	void				vfExportArtifactMerger		(CLuaVirtualMachine *tpLuaVM);
	void				vfLoadStandardScripts		(CLuaVirtualMachine *tpLuaVM);
	void				vfExportMemoryObjects		(CLuaVirtualMachine *tpLuaVM);
	void				vfExportToLua				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportActionManagement	(CLuaVirtualMachine *tpLuaVM);
	void				vfExportMotivationManagement(CLuaVirtualMachine *tpLuaVM);
	bool				bfLoadFile					(CLuaVirtualMachine *tpLuaVM, LPCSTR	caScriptName,	bool	bCall = true);
	void				LuaHookCall					(CLuaVirtualMachine *tpLuaVM, lua_Debug *tpLuaDebug);
	int					LuaPanic					(CLuaVirtualMachine *tpLuaVM);
#endif
	SCRIPT_API bool				bfPrintOutput				(CLuaVirtualMachine *tpLuaVM, LPCSTR	caScriptName, int iErorCode = 0);
	SCRIPT_API LPCSTR				cafEventToString			(int				iEventCode);
	SCRIPT_API void				vfPrintError				(CLuaVirtualMachine *tpLuaVM, int		iErrorCode);
	SCRIPT_API bool				bfListLevelVars				(CLuaVirtualMachine *tpLuaVM, int		iStackLevel);
	SCRIPT_API bool				bfLoadBuffer				(CLuaVirtualMachine *tpLuaVM, LPCSTR	caBuffer,		size_t	tSize,				LPCSTR	caScriptName, LPCSTR caNameSpaceName = 0);
	SCRIPT_API bool				bfLoadFileIntoNamespace		(CLuaVirtualMachine *tpLuaVM, LPCSTR	caScriptName,	LPCSTR	caNamespaceName,	bool	bCall);
	SCRIPT_API bool				bfGetNamespaceTable			(CLuaVirtualMachine *tpLuaVM, LPCSTR	caName);
	SCRIPT_API CLuaVirtualMachine	*get_namespace_table		(CLuaVirtualMachine *tpLuaVM, LPCSTR	caName);
	SCRIPT_API bool				bfIsObjectPresent			(CLuaVirtualMachine *tpLuaVM, LPCSTR	caIdentifier,	int type);
	SCRIPT_API bool				bfIsObjectPresent			(CLuaVirtualMachine *tpLuaVM, LPCSTR	caNamespaceName, LPCSTR	caIdentifier, int type);
	SCRIPT_API luabind::object		lua_namespace_table			(CLuaVirtualMachine *tpLuaVM, LPCSTR namespace_name);
};
