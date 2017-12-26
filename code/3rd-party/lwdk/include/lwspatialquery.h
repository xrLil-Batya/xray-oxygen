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
#ifndef LWSPATIALQUERY_H
#define LWSPATIALQUERY_H

/* Spatial Query SDK header */

#include <lwtypes.h>
#include <lwmeshes.h>
#include <lwserver.h>

#define LWSPATIALQUERYFUNCS_SERVICE "LWSpatialQueryService"

/// RayCast structure
typedef struct st_LWRayCastInfo {
    LWPolID polygon;
    LWDVector hitpoint;
    double distance;
} LWRayCastInfo;

typedef void* LWSpatialQueryID;

/// SpatialQuery struct
typedef struct st_LWSpatialQueryFuncs
{
    ///@brief Acquire a Spatial Query ID to be passed to the raycast function.
    LWSpatialQueryID (*acquire)(void);

    ///@brief Release query ID
    ///
    ///This is required to clean up unused raycast structures
    void (*release) ( LWSpatialQueryID );

    ///@brief Casts a ray at the object only.
    ///
    ///If there is no item registered the system will create an entry for it.
    ///
    ///@param id Is the spatial query ID obtained through acquire().
    ///@param item Is the item we're raycasting against.
    ///@param origin Is the ray origin.
    ///@param direction Is the normalized ray direction.
    ///@param data Is a pointer to the return value to be filled by the function.
    ///@return 0 for no hit, 1 for a hit.
    int (*raycast) ( LWSpatialQueryID id , LWItemID item , const LWDVector origin , const LWDVector direction , LWRayCastInfo* data );

    ///@brief Updates spatial structures.
    ///
    ///Use this from an item's deform function to call the update.
    ///
    ///@param item The item to be updated. @todo deprecate?
    void (*update) ( LWItemID item );

    ///@brief Removes spatial structures. @todo deprecate?
    void (*destroy) ( LWItemID item );

    ///@brief Gets the list of items as an array.
    unsigned int (*numItems)(void);
    void (*items)( LWItemID* items_list );

} LWSpatialQueryFuncs;

#endif