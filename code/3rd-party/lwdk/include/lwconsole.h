/*
 * LWSDK Header File
 *
 * LWCONSOLE.H -- LightWave Console Access
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_PCORECONSOLE_H
#define LWSDK_PCORECONSOLE_H

#define LWPCORECONSOLE_GLOBAL    "LW PCore Console"

typedef struct st_LWPCoreConsole
{
    void        (*info)(const char *message /* utf8 encoded */);
    void        (*error)(const char *message /* ascii encoded */);
    void        (*clear)();
    void        (*show)();
    void        (*hide)();
    int         (*visible)();
} LWPCoreConsole;

#endif