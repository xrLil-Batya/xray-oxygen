// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once
#pragma warning (disable:4661)
#include "xrLC_Light.h"
void __cdecl	clMsg( const char *format, ...);
void __cdecl	clLog( const char *format, ...);
void __cdecl	Status( const char *format, ...);
void			Progress	( const float F );
void			Phase		( LPCSTR phase_name );
// TODO: reference additional headers your program requires here

#ifdef DEBUG
#	define CL_NET_LOG
#endif
