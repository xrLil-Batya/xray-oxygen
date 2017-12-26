/*
 * LWSDK Startup File
 * Copyright 1995,1997  NewTek, Inc.
 */
#include <lwmodule.h>


extern void *        Startup (void);
extern void        Shutdown (void *serverData);
extern ServerRecord     ServerDesc[];

ModuleDescriptor _mod_descrip =
 {
    MOD_SYSSYNC,
    MOD_SYSVER,
    MOD_MACHINE,
    Startup,
    Shutdown,
    ServerDesc
 };
