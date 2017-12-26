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
#ifndef LWSDK_EDGE_GENERATOR_H
#define LWSDK_EDGE_GENERATOR_H

#include <lwmeshes.h>

struct LWEdgeGenerator
{
    struct LWEdgeGeneratorDetail* detail;
    void (*destroy)(struct LWEdgeGenerator* gen);
    struct LWEdgeGenerator* (*clone)(struct LWEdgeGenerator* gen);
    LWEdgeID (*generate)(struct LWEdgeGenerator* gen);
};

#endif