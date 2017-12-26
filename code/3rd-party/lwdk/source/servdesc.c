/*
 * LWSDK Library Source File
 *
 * Default 'ServerDesc' array is defined by assuming the existence of
 * some other static globals for the class name, server name, user
 * names and activation entry point for a single server.  Multiple
 * server modules will define their own 'ServerDesc' arrays.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#include <lwserver.h>


extern char             ServerClass[];
extern char             ServerName[];
extern ServerUserName   UserNames[];
extern XCALL_(int)      Activate (int, GlobalFunc *, void *, void *);


ServerRecord ServerDesc[] = {
    { ServerClass, ServerName, Activate, UserNames },
    { 0 }
};