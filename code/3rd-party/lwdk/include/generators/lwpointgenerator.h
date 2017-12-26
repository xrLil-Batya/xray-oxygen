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
#ifndef LWSDK_POINT_GENERATOR_H
#define LWSDK_POINT_GENERATOR_H

#include <lwmeshes.h>

struct LWPointGenerator
{
    struct LWPointGeneratorDetail* detail;
    void (*destroy)(struct LWPointGenerator* gen);
    struct LWPointGenerator* (*clone)(struct LWPointGenerator* gen);
    LWPntID (*generate)(struct LWPointGenerator* gen);
};

#endif