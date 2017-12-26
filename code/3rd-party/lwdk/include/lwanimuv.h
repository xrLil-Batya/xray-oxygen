/*
 * LWSDK Header File
 *
 * LWANIMUV.H -- LightWave Animation UV
 *
 * Jamie L. Finch
 * Senile Programmer
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_ANIMUV_H
#define LWSDK_ANIMUV_H

#include <lwrender.h>

#define LWANIMUV_HCLASS  "AnimUVHandler"
#define LWANIMUV_ICLASS  "AnimUVInterface"
#define LWANIMUV_GCLASS  "AnimUVGizmo"
#define LWANIMUV_VERSION 5

typedef struct st_LWAnimUVHandler {
    LWInstanceFuncs *inst;
    LWItemFuncs     *item;
    int     (*GetOptions)( LWInstance, char *option_bytes /* ? encoded */ );
    int     (*SetOptions)( LWInstance, char *option_bytes /* ? encoded */ );
    int     (*Begin     )( LWInstance, char *option_bytes /* ? encoded */, double, int, int, int, double aspect, int width, int height );
    int     (*Evaluate  )( LWInstance, int, double * );
    int     (*End       )( LWInstance );
} LWAnimUVHandler;

#endif
