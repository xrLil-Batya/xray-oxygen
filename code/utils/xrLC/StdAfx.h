// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#pragma once
#include "../xrInterface/LevelCompilerLoggerWindow.hpp"
#include "../xrInterface/xrThread.hpp"
#include "../xrLC_Light/xrLC_Light.h"

extern XRLC_LIGHT_API LevelCompilerLoggerWindow& Logger;
extern XRLC_LIGHT_API CThread::LogFunc ProxyMsg;
extern XRLC_LIGHT_API CThreadManager::ReportStatusFunc ProxyStatus;
extern XRLC_LIGHT_API CThreadManager::ReportProgressFunc ProxyProgress;

#define ENGINE_API				// fake, to enable sharing with engine
//comment - ne figa oni ne sharyatsya

#define ECORE_API				// fake, to enable sharing with editors
#define XR_EPROPS_API
#include "../../xrcore/clsid.h"
#include "defines.h"
#include "b_globals.h"
