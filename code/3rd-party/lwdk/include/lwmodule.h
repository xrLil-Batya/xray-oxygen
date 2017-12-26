/*
 * LWSDK Header File
 *
 * LWMODULE.H -- LightWave Plug-in Modules
 *
 * The ModuleDescriptor is the lowest-level which describes a single
 * LightWave plug-in module.  Modules can contain multiple servers
 * but have one startup and shutdown each.  The synchronization codes
 * are used to assure that the module matches the expectations of the
 * host.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_MODULE_H
#define LWSDK_MODULE_H

#include <lwserver.h>


typedef struct st_ModuleDescriptor {
    unsigned int         sysSync;
    unsigned int         sysVersion;
    unsigned int         sysMachine;
    void *             (*startup)  (void);
    void               (*shutdown) (void *);
    ServerRecord        *serverDefs;
} ModuleDescriptor;

#define MOD_SYSSYNC  0x04121994
#define MOD_SYSVER   3
#ifdef _MSWIN
 #define MOD_MACHINE     0x300
#endif
#ifdef _MACOS
 #define MOD_MACHINE     0x400
#endif
#ifdef _LINUX
 #define MOD_MACHINE     0x500
#endif

#endif