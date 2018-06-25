// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#pragma once
#include "../xrInterface/LevelCompilerLoggerWindow.hpp"
#include "../xrInterface/xrThread.hpp"

extern ILevelCompilerLogger& Logger;
extern CThread::LogFunc ProxyMsg;
extern CThreadManager::ReportStatusFunc ProxyStatus;
extern CThreadManager::ReportProgressFunc ProxyProgress;
#include "../xrLC_Light/xrLC_Light.h"

#define ENGINE_API				// fake, to enable sharing with engine
//comment - ne figa oni ne sharyatsya

#define ECORE_API				// fake, to enable sharing with editors
#define XR_EPROPS_API
#include "../../xrcore/clsid.h"
#include "defines.h"
#include "b_globals.h"
