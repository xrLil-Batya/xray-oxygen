// stdafx.cpp : source file that includes just the standard includes
// xrPhysics.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#pragma comment( lib, "sound_static.lib")
// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
#pragma comment(lib, "xrCore.lib")
#ifndef XRPHYSICS_STATIC
#pragma comment(lib, "xrParticles.lib")
#endif