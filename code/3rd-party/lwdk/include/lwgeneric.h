/*
 * LWSDK Header File
 *
 * LWGENERIC.H -- LightWave Generic Commands
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_GENERIC_H
#define LWSDK_GENERIC_H

#include <lwtypes.h>
#include <lwdyna.h>

#define LWLAYOUTGENERIC_CLASS   "LayoutGeneric"
#define LWLAYOUTGENERIC_VERSION 6

typedef struct st_LWLayoutGeneric {
    int             (*saveScene) (const char *file /* language encoded */);
    int             (*loadScene) (const char *file /* language encoded */, const char *name /* language encoded */);

    void             *data;
    LWCommandCode   (*lookup)    (void *, const char *cmdName /* language encoded */);
    int             (*execute)   (void *, LWCommandCode cmd, int argc, const DynaValue *argv, DynaValue *result);
    int             (*evaluate)  (void *, const char *command /* language encoded */);
    const char     * /* language encoded */ (*commandArguments)(void *);
    int             (*parsedArguments)  (void *, DynaValue **argv);

} LWLayoutGeneric;

#endif
