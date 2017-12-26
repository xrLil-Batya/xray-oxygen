/*
 * LWSDK Header File
 *
 * LWANIMLOD.H -- LightWave Animation Loaders
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_ANIMLOD_H
#define LWSDK_ANIMLOD_H

#include <lwimageio.h>
#include <lwhandler.h>

#define LWANIMLOADER_HCLASS "AnimLoaderHandler"
#define LWANIMLOADER_ICLASS "AnimLoaderInterface"
#define LWANIMLOADER_VERSION    4

typedef struct st_LWAnimFrameAccess
{
    void *priv_data;
    LWImageProtocolID   (*begin) (void *, int type);
    void                (*done)  (void *, LWImageProtocolID);
} LWAnimFrameAccess;

typedef struct st_LWAnimLoaderHandler {
    LWInstanceFuncs *inst;
    int             (*frameCount) (LWInstance);
    double          (*frameRate)  (LWInstance);
    double          (*aspect)     (LWInstance, int *w, int *h, double *pixAspect);
    void            (*evaluate)   (LWInstance, double, LWAnimFrameAccess *);
} LWAnimLoaderHandler;

#endif