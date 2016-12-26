
#pragma once

#define	MTL_EXPORT_API
#define ENGINE_API
#define DLL_API		
#define ECORE_API

#include "../xrCore/xrCore.h"
#include "../xrServerEntities/smart_cast.h"
#include "../xrcdb/xrcdb.h"
#include "../xrsound/sound.h"

#pragma comment( lib, "xrCore.lib"	)

#include "xrPhysics.h"
#include "../xrCore/xrapi.h"

#ifdef	DEBUG
#include "d3d9types.h"
#endif

class CGameMtlLibrary;
IC CGameMtlLibrary &GMLibrary()
{
	VERIFY(PGMLib);
	return *PGMLib;
}