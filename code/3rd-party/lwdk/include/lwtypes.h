/*
 * LWSDK Header File
 *
 * LWTYPES.H -- LightWave Common Types
 *
 * This header contains type declarations common to all aspects of
 * LightWave.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_TYPES_H
#define LWSDK_TYPES_H

#define LWITEM_NULL ((LWItemID)0)

typedef unsigned short   LWDualKey;

typedef void *           LWItemID;
typedef void *           LWImageID;
typedef float            LWBufferValue;
typedef void *           LWPixmapID;
typedef void *           LWTextureID;
typedef void *           NodeEditorID;
typedef void *           ItemInstanceID;

typedef int              LWFrame;
typedef double           LWTime;

typedef float            LWFVector[3];
typedef double           LWDVector[3];
typedef float            LWFMatrix3[3][3];
typedef float            LWFMatrix4[4][4];
typedef double           LWDMatrix3[3][3];
typedef double           LWDMatrix4[4][4];
typedef double           LWDMatrix9[9];

typedef unsigned int     LWID;
#ifndef LWID_
    #define LWID_(a,b,c,d) ((((unsigned int)a)<<24)|(((unsigned int)b)<<16)|(((unsigned int)c)<<8)|((unsigned int)d))
#endif

typedef int              LWCommandCode;

typedef void *           LWChannelID;

/*
 * Persistent instances are just some opaque data object referenced
 * by void pointer.  Errors from handler functions are human-readable
 * strings, where a null string pointer indicates no error.
 */
typedef void *           LWInstance;
typedef const char *     LWError /* language encoded */;

#ifndef NULL
    #ifndef __cplusplus
        #define NULL    ((void *) 0)
    #else
        #define NULL     0
    #endif
#endif

/*
 * These are useful to support 64bit architectures of Mac universal binaries
 * where 'long' and pointers are 8bytes (The LP64 programming model)
 */
#define PTR2INT(x) ((int)(size_t)(x))
#define PTR2UINT(x) ((unsigned int)(size_t)(x))
#define INT2PTR(x) ((void*)(size_t)(x))
#define UINT2PTR(x) ((void*)(size_t)(x))

#define LWITEMID2INT(x) PTR2INT(x)
#define LWITEMID2UINT(x) PTR2UINT(x)
#define INT2LWITEMID(x) ((LWItemID)INT2PTR(x))
#define UINT2LWITEMID(x) ((LWItemID)UINT2PTR(x))

#endif
