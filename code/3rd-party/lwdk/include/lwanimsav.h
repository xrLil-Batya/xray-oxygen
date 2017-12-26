/*
 * LWSDK Header File
 *
 * LWANIMSAV.H -- LightWave Animation Saver
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_ANIMSAV_H
#define LWSDK_ANIMSAV_H

#include <lwhandler.h>
#include <lwrender.h>

#define LWANIMSAVER_HCLASS  "AnimSaverHandler"
#define LWANIMSAVER_ICLASS  "AnimSaverInterface"
#define LWANIMSAVER_VERSION 4

typedef struct st_LWAnimSaverHandler
{
    LWInstanceFuncs  *inst;
    LWItemFuncs      *item;
    int               type;
    LWError         (*open)  (LWInstance, int w, int h, const char *filename /* language encoded */);
    void            (*close) (LWInstance);
    LWError         (*begin) (LWInstance);
    LWError         (*write) (LWInstance, const void *R, const void *G, const void *B, const void *alpha);

} LWAnimSaverHandler;

/*
 * The saver selects what kind of data it wants with the 'type' field.
 * The value of that field selects the type of the data that will be
 * passed to the 'write' function.
 */
#define LWAST_UBYTE 0
#define LWAST_FLOAT 1

#endif
