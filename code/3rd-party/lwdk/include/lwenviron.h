/*
 * LWSDK Header File
 *
 * LWENVIRON.H -- LightWave Environments
 *
 * This header defines the enviroment render handler for backdrops and
 * fog.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_ENVIRON_H
#define LWSDK_ENVIRON_H

#include <lwrender.h>

#define LWENVIRONMENT_HCLASS    "EnvironmentHandler"
#define LWENVIRONMENT_ICLASS    "EnvironmentInterface"
#define LWENVIRONMENT_GCLASS    "EnvironmentGizmo"
#define LWENVIRONMENT_VERSION   4


typedef enum en_LWEnvironmentMode {
    EHMODE_PREVIEW,
    EHMODE_REAL
} LWEnvironmentMode;

typedef struct st_LWEnvironmentAccess {
    LWEnvironmentMode mode;
    double        h[2], p[2];
    LWDVector     dir;
    double        colRect[4][3];
    double        color[3];
} LWEnvironmentAccess;

typedef struct st_LWEnvironmentHandler {
    LWInstanceFuncs  *inst;
    LWItemFuncs  *item;
    LWRenderFuncs    *rend;
    LWError     (*evaluate) (LWInstance, LWEnvironmentAccess *);
    unsigned int    (*flags)    (LWInstance);
} LWEnvironmentHandler;

#define LWENF_TRANSPARENT   (1<<0)

#endif