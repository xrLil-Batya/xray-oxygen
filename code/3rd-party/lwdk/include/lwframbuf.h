/*
 * LWSDK Header File
 *
 * LWFRAMBUF.H -- LightWave Framebuffers
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_FRAMBUF_H
#define LWSDK_FRAMBUF_H

#include <lwrender.h>

#define LWFRAMEBUFFER_HCLASS    "FrameBufferHandler"
#define LWFRAMEBUFFER_ICLASS    "FrameBufferInterface"
#define LWFRAMEBUFFER_GCLASS    "FrameBufferGizmo"
#define LWFRAMEBUFFER_VERSION   5

typedef struct st_LWFrameBufferHandler {
    LWInstanceFuncs  *inst;
    LWItemFuncs  *item;
    int       type;
    LWError         (*open) (LWInstance, int, int );
    void            (*close) (LWInstance);
    LWError         (*begin) (LWInstance);
    LWError         (*write) (LWInstance, const void *, const void *, const void *, const void *);
    void            (*pause) (LWInstance);
    LWPixmapID      (*getPixelMap) (LWInstance);
} LWFrameBufferHandler;

#define LWFBT_UBYTE 0
#define LWFBT_FLOAT 1

#endif