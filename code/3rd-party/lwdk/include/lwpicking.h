/*
 * LWSDK Header File
 *
 * picking.h -- LightWave Picking Access
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_PICKING_H
#define LWSDK_PICKING_H

#include <lwtypes.h>
#include <lwmeshtypes.h>

#define LWPICKING_VERSION 1

enum LWPickType
{
    lwPickNone,
    lwPickPoint,
    lwPickEdge,
    lwPickPoly
};

struct LWPickResult
{
    union LWPickElement
    {
        LWPntID point;
        LWEdgeID edge;
        LWPolID poly;
    };

    /// Identifies the type of element being picked.
    enum LWPickType type;

    /// Specifies the actual element being picked.
    union LWPickElement element;

    /// Represents where the picking ray intersected the geometry.
    double intersection[3];

    /// Internal data identifying the layer in which the element resides.
    void* layer;
};

struct LWPicking
{
    struct LWPickingData* priv;

    /// @return true if the picking interface is valid.
    int (*valid)(struct LWPicking* self);

    /// @return the frontmost point under the cursor.
    struct LWPickResult (*solidPickPoint)(struct LWPicking* self);

    /// @return the frontmost edge under the cursor.
    struct LWPickResult (*solidPickEdge)(struct LWPicking* self);

    /// @return the frontmost polygon under the cursor.
    struct LWPickResult (*solidPickPoly)(struct LWPicking* self);
};

#endif