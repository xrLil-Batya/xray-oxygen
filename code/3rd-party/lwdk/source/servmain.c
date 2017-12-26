/*
 * LWSDK Startup File
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#include <lwmodule.h>

extern void *        Startup (void);
extern void        Shutdown (void *serverData);
extern ServerRecord     ServerDesc[];

#if defined(__BORLANDC__)
ModuleDescriptor mod_descrip =
#elif defined __GNUC__
__attribute__ ((visibility("default"))) ModuleDescriptor _mod_descrip =
#else
ModuleDescriptor _mod_descrip =
#endif
{
    MOD_SYSSYNC,
    MOD_SYSVER,
    MOD_MACHINE,
    Startup,
    Shutdown,
    ServerDesc
};