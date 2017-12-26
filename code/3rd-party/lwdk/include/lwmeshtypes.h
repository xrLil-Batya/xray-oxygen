/*
 * LWSDK Header File
 *
 * lwmeshtypes.h -- LightWave Common Mesh-Related Types
 *
 * This header contains type declarations common to mesh-related
 * aspects of LightWave.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_COMMON_MESH_TYPES_H
#define LWSDK_COMMON_MESH_TYPES_H

#define LWMESH_TYPES_VERSION 1

/*
 * Meshes are composed of points, edges, and polygons given by their internal
 * ID reference.
 */
typedef struct st_GCoreVertex* LWPntID;
typedef struct st_GCoreEdge* LWEdgeID;
typedef struct st_GCorePolygon* LWPolID;

struct LWPoints
{
    void* data;

    /// @return the pointer to the array of points.
    LWPntID* (*elements)(struct LWPoints* obj);

    /// @return the number of points.
    int (*size)(struct LWPoints* obj);

    /// Destroys the array.
    void (*destroy)(struct LWPoints* obj);
};

struct LWEdges
{
    void* data;

    /// @return the pointer to the array of edges.
    LWEdgeID* (*elements)(struct LWEdges* obj);

    /// @return the number of edges.
    int (*size)(struct LWEdges* obj);

    /// Destroys the array.
    void (*destroy)(struct LWEdges* obj);
};

struct LWPolys
{
    void* data;

    /// @return the pointer to the array of polygons.
    LWPolID* (*elements)(struct LWPolys* obj);

    /// @return the number of polygons.
    int (*size)(struct LWPolys* obj);

    /// Destroys the array.
    void (*destroy)(struct LWPolys* obj);
};

#endif