/*
 * LWSDK Header File
 *
 * LWGIZMO.H -- Viewport widgets
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_GIZMO_H
#define LWSDK_GIZMO_H

#include <lwtool.h>
#include <lwcustobj.h>

#define LWGIZMO_VERSION 4

typedef struct st_LWGizmoFuncs {
    void            (*done)     (LWInstance);
    void            (*draw)     (LWInstance, LWCustomObjAccess *);
    const char     * /* language encoded */ (*help)     (LWInstance, LWToolEvent *);
    int             (*dirty)    (LWInstance);
    int             (*count)    (LWInstance, LWToolEvent *);
    int             (*handle)   (LWInstance, LWToolEvent *, int i, LWDVector pos);
    int             (*start)    (LWInstance, LWToolEvent *);
    int             (*adjust)   (LWInstance, LWToolEvent *, int i);
    int             (*down)     (LWInstance, LWToolEvent *);
    void            (*move)     (LWInstance, LWToolEvent *);
    void            (*up)       (LWInstance, LWToolEvent *);
    void            (*event)    (LWInstance, int code);
    LWXPanelID      (*panel)    (LWInstance);
    int             (*end)      (LWInstance, LWToolEvent *, int i);
} LWGizmoFuncs;

typedef struct st_LWGizmo {
    LWInstance       instance;
    LWGizmoFuncs     *gizmo;
    const LWItemID* (*pickItems) (LWInstance, const LWItemID* drawitems, const unsigned int* drawparts);
} LWGizmo;


#endif