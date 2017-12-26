/*
 * LWSDK Header File
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_POLYGON_GENERATOR_H
#define LWSDK_POLYGON_GENERATOR_H

#include <lwmeshes.h>

struct LWPolyGenerator
{
    struct LWPolyGeneratorDetail* detail;
    void (*destroy)(struct LWPolyGenerator* gen);
    struct LWPolyGenerator* (*clone)(struct LWPolyGenerator* gen);
    LWPolID (*generate)(struct LWPolyGenerator* gen);
};

#endif