/*
 * LWSDK Header File
 *
 * FIBER.H -- LightWave Fibers
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_FIBER_H
#define LWSDK_FIBER_H

#include <lwglobsrv.h>
#include <lwcomring.h>
#include <lwnodes.h>
#define LWFIBERINFO_GLOBAL "Fiber Info"
/*
The fiber services provide some functions to query fiber data,
this data is made accessible to other clients by query functions that lookup fiber
data from itemIDs. These services can be used by a fiber renderer or other purposes.
*/

typedef struct st_LWFiberSys  *LWFiberSysID;

enum{FI_THIN = 0, FI_THICKXALIGN,FI_THICKYALIGN, FI_CAMERA, FI_THICKRANDOMALIGN, FI_STROKE, FI_SOLID};

typedef struct st_LWFiberInfo {
    LWFiberSysID        *(*fiberSys)(LWItemID item);                                                    /* returns null terminated list of FiberSys attached to item. */
    int                 (*vertTotal)(LWFiberSysID fsid, int smoothed);                                  /* returns number of vertices in FiberSys */
    int                 (*fiberCount)(LWFiberSysID fsid);                                               /* returns number of fibers in FiberSys */
    int                 (*vertCount)(LWFiberSysID fsid, int fiberindex, int smoothed);                  /* returns number of vertices in fiber */

    int                 (*pos)(LWFiberSysID fsid, int fiberindex, int smoothed, LWFVector *data);       /* returns position of vertices float[3]*vertCount */
    int                 (*width)(LWFiberSysID fsid, int fiberindex, int smoothed, float *data);         /* returns width of vertices in fiber float*vertCount */
    int                 (*color)(LWFiberSysID fsid, int fiberindex, int smoothed, LWFVector *data);     /* returns color of vertices in fiber float[3]*vertCount */
    LWPolID             (*weight)(LWFiberSysID fsid, int fiberindex, float *data);                      /* returns position of vertices float[4]*vertCount */
    NodeEditorID        (*nodeEditorID)(LWFiberSysID fsid);                                             /* returns nodeEditorID. */
    int                 (*fiberType)(LWFiberSysID fsid);                                                /* returns fiber type. */
} LWFiberInfo;

/*
The FiberInfo comring allows a client to receive change messages.
The event data is a LWFiberInfoRingID struct containing the
changed LWFiberSysID.
*/
#define LWFIBERINFO_CHANGEDEVENTCODE 0  //fibers have been rebuilt
#define LWFIBERINFO_CHANGINGEVENTCODE 1 //fibers are rebuilding

#define LWFIBERINFO_RING    "FiberInfo Ring"
typedef struct st_LWFiberInfoRing *LWFiberInfoRingID;
typedef struct st_LWFiberInfoRing
{
    LWFiberSysID changedID;
} LWFiberInfoRing;

#endif  // LWSDK_FIBER_H