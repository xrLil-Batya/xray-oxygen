/*
 * LWSDK Header File
 *
 * LWCHANGE.H -- LightWave change.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_CHANGE_H
#define LWSDK_CHANGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lwrender.h>

#define LWCHANGEFUNCS_GLOBAL    "Change Functions"


typedef struct st_LWChangeFuncs {
    int (*getObjectChangedState)( LWItemID );   /* Gets object save state. */
    int (*getSceneChangedState )( void );       /* Gets scene  save state. */
    int (*anyChangesAtAll      )( void );       /* Checks for any changes at all in scene. */
} LWChangeFuncs;

#ifdef __cplusplus
}
#endif

#endif