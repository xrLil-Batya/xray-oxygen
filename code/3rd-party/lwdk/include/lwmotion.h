/*
 * LWSDK Header File
 *
 * LWMOTION.H -- LightWave Item Motions
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_MOTION_H
#define LWSDK_MOTION_H

#include <lwrender.h>

#define LWITEMMOTION_HCLASS "ItemMotionHandler"
#define LWITEMMOTION_ICLASS "ItemMotionInterface"
#define LWITEMMOTION_GCLASS "ItemMotionGizmo"
#define LWITEMMOTION_VERSION    5


typedef struct st_LWItemMotionAccess {
    LWItemID      item;
    LWFrame       frame;
    LWTime        time;
    void        (*getParam) (LWItemParam, LWTime, LWDVector);
    void        (*setParam) (LWItemParam, const LWDVector);
    int       animationPass;
} LWItemMotionAccess;

typedef struct st_LWItemMotionHandler {
    LWInstanceFuncs  *inst;
    LWItemFuncs  *item;
    void        (*evaluate) (LWInstance, const LWItemMotionAccess *);
    unsigned int    (*flags)    (LWInstance);
} LWItemMotionHandler;

#define LWIMF_AFTERIK   (1<<0)


#endif
