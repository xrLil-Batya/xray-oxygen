#pragma once

#define	MTL_EXPORT_API
#define ENGINE_API
#define DLL_API
#define ECORE_API

#include "../xrCore/xrCore.h"
#include "../xrCore/xrDelegate/xrDelegate.h"
#include "../xrServerEntities/smart_cast.h"
#include "../xrcdb/xrcdb.h"
#include "../xrsound/sound.h"
#ifndef XRPHYSICS_STATIC
#include "../xrParticles/stdafx.h"
#endif
#include "xrPhysics.h"
#include "../xrCore/xrapi.h"
#include "../xrGame/xrGame.h"

#include "d3d9types.h"

class CGameMtlLibrary;
inline CGameMtlLibrary &GMLibrary()
{
	VERIFY(PGMLib);
	return *PGMLib;
}