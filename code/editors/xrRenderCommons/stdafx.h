#pragma once
//ZDK Project (XRay 1.7)
//Giperion 2017 October
//[EUREKA] 3.3

#include "xrCore/xrCore.h"
#include "xrRenderCommons.h"

#include <d3d9.h>
#include "xrD3DDefs.h"

#define ENGINE_API
#define ECORE_API XRRENDER_COMMONS_API

#define USE_NVTT

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                  \
    ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
    ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

#define _RELEASE(x)			{ if(x) { (x)->Release();       (x)=NULL; } }