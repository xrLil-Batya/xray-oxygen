// xrAPI.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "xrAPI.h"

XRCORE_API IRender_interface* Render = NULL;
XRCORE_API IRenderFactory* RenderFactory = NULL;
XRCORE_API CDUInterface* DU = NULL;
XRCORE_API xr_token* vid_mode_token = NULL;
XRCORE_API IUIRender* UIRender = NULL;

#ifndef	_EDITOR
XRCORE_API CGameMtlLibrary* PGMLib = NULL;
#endif

XRCORE_API IDebugRender* DRender = NULL;