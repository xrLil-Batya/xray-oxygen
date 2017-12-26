/*
 * LWSDK Header File
 *
 * LWMODTOOL.H -- Modeler Interactive Tools
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_MODTOOL_H
#define LWSDK_MODTOOL_H

#include <lwtool.h>
#include <lwmeshedt.h>

#define LWMESHEDITTOOL_CLASS "MeshEditTool"
#define LWMESHEDITTOOL_VERSION 4

typedef struct st_LWMeshEditTool LWMeshEditTool;

/// The MeshEdit tool is a LightWave viewport tool that performs its
/// action through a MeshEditOp.
struct st_LWMeshEditTool
{
    /// Set this to point to your instance data. Typically this is
    /// a structure that holds all of the data your plug-in needs
    /// to perform its function.
    LWInstance instance;

    /// A set of tool callbacks you need to define.
    LWToolFuncs* tool;

    /// Return a code for the edit action that needs to be
    /// performed. Actions given below.
    int (*test)(LWInstance);

    /// Perform the mesh edit operation to reflect the current
    /// tool settings.
    LWError (*build)(LWInstance, MeshEditOp* op);

    /// Clear the state when the last edit action is completed.
    /// This can be a result of the 'test' update code or it can
    /// be triggered by an external action.
    void (*end)(LWInstance, int keep);

    /// Perform a mesh edit operation to reflect the current
    /// tool's selection.
    LWError (*select)(LWInstance, MeshEditOp* op);

    /// Flags that affect tool operation. This can contain one or more
    /// of the flags named with LWT_FLAGS_*.
    unsigned int flags;
};

/// The test function can return one of the following codes for the next action
/// to be performed to modify the edit state.
/// @{
    /// Do nothing.  The edit state remains unchanged.
    #define LWT_TEST_NOTHING 0

    /// Reapply the operation with new settings.  The 'build' function is
    /// called.
    #define LWT_TEST_UPDATE 1

    /// Keep the last operation.  The 'end' callback is called with  the
    /// 'keep' argument true.
    #define LWT_TEST_ACCEPT 2

    /// Discard the last operation.  The 'end' callback is called with the
    /// 'keep' argument false.
    #define LWT_TEST_REJECT 3

    /// keep the last operation and begin a new one.  'End' is called with a
    /// true 'keep' parameter, and then 'build' is called again.
    #define LWT_TEST_CLONE 4
/// @}

// The user memory normally passed to the MeshEdit->begin() is set with
// the return from the test() function. OR these values with the flag
// bits above to get per-vertex and per-polygon data allocated and accessed
// through userData pointers in the EDPointInfo and EDPolygonInfo structures.
#define LWT_VMEM(siz) ((((siz+7)>>3)<<24)&0xFF000000)
#define LWT_PMEM(siz) ((((siz+7)>>3)<<16)&0x00FF0000)

/// Indicates that only directly selected elements should be marked as
/// selected in the build operation.
#define LWT_FLAGS_DIRECT_SELECTION (1 << 0)

/// Grants the tool full control over geometry deletion. Without this flag,
/// the system may reject certain point removals. If specified, the user
/// becomes responsible for ensuring that any removed points will not belong
/// to any polygons when the build operation is applied.
#define LWT_FLAGS_DIRECT_DELETE (1 << 1)

#endif