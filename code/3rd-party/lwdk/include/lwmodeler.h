/*
 * LWSDK Header File
 *
 * LWMODELER.H -- LightWave Modeler Global State
 *
 * This header contains declarations for the global services and
 * internal states of the Modeler host application.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_MODELER_H
#define LWSDK_MODELER_H

#include <lwtypes.h>

typedef int EltOpLayer;
#define OPLYR_PRIMARY    0
#define OPLYR_FG         1
#define OPLYR_BG         2
#define OPLYR_SELECT     3
#define OPLYR_ALL        4
#define OPLYR_EMPTY      5
#define OPLYR_NONEMPTY   6

typedef int EltOpSelect;
#define OPSEL_GLOBAL     0
#define OPSEL_USER       1
#define OPSEL_DIRECT     2

#define LWAC_MOUSE 0
#define LWAC_ORIGIN 1
#define LWAC_SELECTION 2
#define LWAC_PIVOT 3

#define LWSTATEQUERYFUNCS_GLOBAL    "LWM: State Query 3"

typedef struct st_LWStateQueryFuncs
{
    /// @return The number of data layers for the current object.
    int (*numLayers)(void);

    /// @return The bits for the data layers included in the EltOpLayer selection.
    /// If bit i of the mask is set, then layer i + 1 of the current object belongs
    /// to the set defined by the oplayer argument. This function is provided primarily
    /// for backward compatibility. New code should use the layerList function, which is
    /// designed for multiple objects and an unlimited number of layers.
    unsigned int (*layerMask)(EltOpLayer oplayer);

    /// @return The name of the current default surface.
    const char* /* language encoded */ (*surface)(void);

    unsigned int (*bbox)(EltOpLayer, double* minmax);

    /// @return A string containing layer numbers for the given EltOpLayer and object.
    /// The layer numbers in the string are separated by spaces, with the highest
    /// numbered layer listed first. The object name is its filename, or NULL for the
    /// current object.
    const char* /* ascii encoded */ (*layerList)(EltOpLayer oplayer, const char* objname /* language encoded */);

    /// @return The filename of the current object. If the geometry in the current layers
    /// hasn't been saved to a file yet, this returns the reference name (the name that would
    /// be returned by the Object Functions refName function). If no object has been loaded
    /// into Modeler, this returns NULL.
    const char* /* language encoded */  (*object)(void);

    /// @return The state of a user interface setting.
    int (*mode)(int setting);

    const char* /* language encoded */ (*vmap)(int index, LWID* lwid);
} LWStateQueryFuncs;

#define LWM_MODE_SELECTION 0
#define LWM_MODE_SYMMETRY 1
#define LWM_MODE_ACTION_CENTER 2

#define LWM_VMAP_WEIGHT 0
#define LWM_VMAP_TEXTURE 1
#define LWM_VMAP_MORPH 2

#define LWFONTLISTFUNCS_GLOBAL "LWM: Font List"

typedef struct st_LWFontListFuncs
{
    int             (*count) (void);
    int             (*index) (const char *name /* language encoded */);
    const char * /* language encoded */ (*name)  (int in_index);
    int             (*load)  (const char *filename /* language encoded */);
    void            (*clear) (int in_index);

} LWFontListFuncs;

#endif