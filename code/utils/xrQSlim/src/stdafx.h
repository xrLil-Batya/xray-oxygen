#pragma once
#include "../../engine.vc2008/xrCore/xrCore.h"

/************************************************************************

Standard base include file for the MIX library.  This defines various
common stuff that is used elsewhere.

Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.

$Id: stdmix.h,v 1.21.2.1 2002/01/31 18:38:37 garland Exp $

************************************************************************/

#if defined(_DEBUG) && defined(_MSC_VER)
#pragma warning (disable : 4786)
#endif

inline bool streq(const char *a, const char *b) { return !xr_strcmp(a,b); }