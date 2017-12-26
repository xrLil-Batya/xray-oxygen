/*
 * LWSDK Header File
 *
 * LWSCENECV.H -- LightWave Scene Converters
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_SCENECV_H
#define LWSDK_SCENECV_H

#include <lwhandler.h>

#define LWSCENECONVERTER_CLASS      "SceneConverter"
#define LWSCENECONVERTER_VERSION    1

typedef struct st_LWSceneConverter {
    const char   *filename /* language encoded */;
    LWError       readFailure;
    const char   *tmpScene /* language encoded */;
    void        (*deleteTmp) (const char *tmpScene /* language encoded */);
} LWSceneConverter;


#endif
