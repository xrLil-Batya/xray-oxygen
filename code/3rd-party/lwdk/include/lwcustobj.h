/*
 * LWSDK Header File
 *
 * LWCUSTOBJ.H -- LightWave Custom Objects
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_CUSTOBJ_H
#define LWSDK_CUSTOBJ_H

#include <lwrender.h>

#define LWCUSTOMOBJ_HCLASS  "CustomObjHandler"
#define LWCUSTOMOBJ_ICLASS  "CustomObjInterface"
#define LWCUSTOMOBJ_GCLASS  "CustomObjGizmo"
#define LWCUSTOMOBJ_VERSION 6

typedef struct st_LWCustomObjAccess
{
    int           view;
    int           flags;
    void         *dispData;
    void        (*setColor)     (void *, float rgba[4]);
    void        (*setPattern)   (void *, int lpat);
    void        (*setTexture)   (void *, int, unsigned char *rgba_bytes);
    void        (*setUVs)       (void *, double[2], double[2], double[2], double[2]);
    void        (*point)        (void *, double[3], int csys);
    void        (*line)         (void *, double[3], double[3], int csys);
    void        (*triangle)     (void *, double[3], double[3], double[3], int csys);
    void        (*quad)         (void *, double[3], double[3], double[3], double[3], int csys);
    void        (*circle)       (void *, double[3], double, int csys);
    void        (*text)         (void *, double[3], const char *text /* language encoded */, int just, int csys);
    LWDVector     viewPos, viewDir;
    void        (*setCSysItem)  (void *, LWItemID item);
    void        (*polygon)      (void *, unsigned int numv, double[][3], int csys);
    void        (*polyIndexed)  (void *, unsigned int numv, unsigned int verts[], double[][3], int csys);
    void        (*setDrawMode)  (void *, unsigned int mode);
    void        (*disk)         (void *, double[3], double, int csys);
    void        (*setPart)      (void *, unsigned int part);
    int         (*measureText)  (void *, const char *text /* language encoded */, int *width, int *height, int *offset_y);
    void        (*setThickness) (void *, float point_size, float line_width);
    void        (*circle2)      (void *, double[3], int csys, double, int radcsys);
    void        (*disk2)        (void *, double[3], int csys, double, int radcsys);
    void        (*setColorCC)   (void *, float rgba[4]);
    void        (*getColor)     (void *, unsigned int index, float rgba[4]);

} LWCustomObjAccess;

#define LWVIEW_ZY        0
#define LWVIEW_XZ        1
#define LWVIEW_XY        2
#define LWVIEW_PERSP     3
#define LWVIEW_LIGHT     4
#define LWVIEW_CAMERA    5
#define LWVIEW_SCHEMA    6

#define LWCOFL_SELECTED (1<<0)
#define LWCOFL_PICKING  (1<<1)
#define LWCOFL_NO_DEPTH_WRITE  (1<<2)

#define LWLPAT_SOLID     0
#define LWLPAT_DOT       1
#define LWLPAT_DASH      2
#define LWLPAT_LONGDOT   3

#define LWCSYS_WORLD     0
#define LWCSYS_OBJECT    1
#define LWCSYS_ICON      2
#define LWCSYS_VIEWPORT  3

#define LWJUST_LEFT      0
#define LWJUST_CENTER    1
#define LWJUST_RIGHT     2

#define LWDRAWMODE_CULLFACE   (1<<0)
#define LWDRAWMODE_OUTLINE    (1<<1)
#define LWDRAWMODE_DEPTHTEST  (1<<2)
#define LWDRAWMODE_DEPTHWRITE (1<<3)
#define LWDRAWMODE_SMOOTH     (1<<4)

#define LWIC_INDEX(i)                   (4096 + i)
#define LWIC_CHAN_MOTION_X              LWIC_INDEX(50)
#define LWIC_CHAN_MOTION_Y              LWIC_INDEX(51)
#define LWIC_CHAN_MOTION_Z              LWIC_INDEX(52)
#define LWIC_CHAN_ROTATION_X            LWIC_INDEX(53)
#define LWIC_CHAN_ROTATION_Y            LWIC_INDEX(54)
#define LWIC_CHAN_ROTATION_Z            LWIC_INDEX(55)
#define LWIC_CHAN_SCALE_X               LWIC_INDEX(56)
#define LWIC_CHAN_SCALE_Y               LWIC_INDEX(57)
#define LWIC_CHAN_SCALE_Z               LWIC_INDEX(58)
#define LWIC_CHAN_MOTION_HILITE_X       LWIC_INDEX(59)
#define LWIC_CHAN_MOTION_HILITE_Y       LWIC_INDEX(60)
#define LWIC_CHAN_MOTION_HILITE_Z       LWIC_INDEX(61)
#define LWIC_CHAN_ROTATION_HILITE_X     LWIC_INDEX(62)
#define LWIC_CHAN_ROTATION_HILITE_Y     LWIC_INDEX(63)
#define LWIC_CHAN_ROTATION_HILITE_Z     LWIC_INDEX(64)
#define LWIC_CHAN_SCALE_HILITE_X        LWIC_INDEX(65)
#define LWIC_CHAN_SCALE_HILITE_Y        LWIC_INDEX(66)
#define LWIC_CHAN_SCALE_HILITE_Z        LWIC_INDEX(67)
#define LWIC_VIEWPORT_BACKGROUND        LWIC_INDEX(79)
#define LWIC_VIEWPORT_GRID              LWIC_INDEX(80)
#define LWIC_VIEWPORT_AXES              LWIC_INDEX(81)

typedef struct st_LWCustomObjHandler
{
    LWInstanceFuncs  *inst;
    LWItemFuncs      *item;
    LWRenderFuncs    *rend;
    void            (*evaluate) (LWInstance, const LWCustomObjAccess *);
    unsigned int    (*flags)    (LWInstance);

} LWCustomObjHandler;

#define LWCOF_SCHEMA_OK         (1<<0)
#define LWCOF_VIEWPORT_INDEX    (1<<1)
#define LWCOF_NO_DEPTH_BUFFER   (1<<2)
#define LWCOF_OVERLAY           (1<<3)
#define LWCOF_XRAY              (1<<4)

#endif