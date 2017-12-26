/*
 * LWSDK Header File
 *
 * LWTEXT.H -- LightWave Text
 *
 * Jamie L. Finch
 * Senile Programmer
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_LWTEXT_H
#define LWSDK_LWTEXT_H

#include <stdio.h>
#include <string.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LWTEXTFUNCS_GLOBAL "LWTextureFuncs"

/*
 *      Mac Operating system.
 */

#if( defined( _MACOS ) )

#define LW_CHAR wchar_t

#define LW_STRLEN( A )            wcslen( A )
#define LW_STRCMP( A, B )         wcscmp( A, B )
//#define LW_STRICMP( A, B )      _wcsicmp( A, B )
#define LW_STRNCMP( A, B, C )    wcsncmp( A, B, C )
//#define LW_STRNICMP( A, B, C ) _wcsnicmp( A, B, C )
#define LW_STRCAT( A, B )         wcscat( A, B )
#define LW_STRCPY( A, B )         wcscpy( A, B )
#define LW_STRSTR( A, B )         wcsstr( A, B )
#define LW_TOUPPER( A )         towupper( A )
#define LW_TOLOWER( A )         towlower( A )

/*
 *      Microsoft Windows.
 */

#else

#define LW_CHAR wchar_t

#define LW_STRLEN( A )            wcslen( A )
#define LW_STRCMP( A, B )         wcscmp( A, B )
#define LW_STRICMP( A, B )      _wcsicmp( A, B )
#define LW_STRNCMP( A, B, C )    wcsncmp( A, B, C )
#define LW_STRNICMP( A, B, C ) _wcsnicmp( A, B, C )
#define LW_STRCAT( A, B )         wcscat( A, B )
#define LW_STRCPY( A, B )         wcscpy( A, B )
#define LW_STRSTR( A, B )         wcsstr( A, B )
#define LW_TOUPPER( A )         towupper( A )
#define LW_TOLOWER( A )         towlower( A )

#endif

/*
 *      String conversion functions.
 */

typedef struct st_LWTextFuncs {
    int      (*stringToAscii )( const LW_CHAR *, char *, int );   /* Converts a  UTF16  string to an ascii  string. */
    int      (*stringToUTF8  )( const LW_CHAR *, char *, int );   /* Converts a  UTF16  string to a  UTF8   string. */
    int      (*stringToExport)( const LW_CHAR *, char *, int );   /* Converts a  UTF16  string to an export string. */
    int      (*asciiToString )( const char *, LW_CHAR *, int );   /* Converts an ascii  string to a  UTF16  string. */
    int      (*uTF8ToString  )( const char *, LW_CHAR *, int );   /* Converts a  UTF8   string to a  UTF16  string. */
    int      (*importToString)( const char *, LW_CHAR *, int );   /* Converts an import string to a  UTF16  string. */
    LW_CHAR *(*stringCopy    )( const LW_CHAR * );                /* Allocates a new string and copies to it.       */
    int      (*stringFree    )( LW_CHAR * );                      /* Free a string that was allocated.              */
} LWTextFuncs;

#ifdef __cplusplus
}
#endif

#endif