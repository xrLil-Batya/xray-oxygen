//----------------------------------------------------
// file: stdafx.h
//----------------------------------------------------
#pragma once
// Std C++ headers
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <process.h>

// DirectX headers
#include <d3d9.h>

#ifdef RELEASE
#	undef _CPPUNWIND
#endif
#include "..\..\engine.vc2008\xrCore\xrCore.h"

#include <dinput.h>