// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently

#pragma once
#include "../../../engine.vc2008/xrCore/xrCore.h"

#define ENGINE_API 

enum TMsgDlgType { mtWarning, mtError, mtInformation, mtConfirmation, mtCustom };
enum TMsgDlgBtn { mbYes, mbNo, mbOK, mbCancel, mbAbort, mbRetry, mbIgnore, mbAll, mbNoToAll, mbYesToAll, mbHelp };
typedef TMsgDlgBtn TMsgDlgButtons[mbHelp];


#define ECORE_API __declspec(dllimport)

#include "..\..\Shared\ELog.h"
#include <d3d9types.h>

//#define AnsiString std::string

xr_vector<std::string> AStringVec;

#define THROW R_ASSERT(0)

#ifdef _LW_SHADER
	#define _EDITOR_FILE_NAME_ "lw_shader"
#else
	#ifdef _LW_EXPORT
		#define _EDITOR_FILE_NAME_ "lw_export"
    #else
		#ifdef _LW_IMPORT
			#define _EDITOR_FILE_NAME_ "lw_import"
		#endif
	#endif
#endif

#define GAMEMTL_NONE		u32(-1)
#define _game_data_ "$game_data$"
