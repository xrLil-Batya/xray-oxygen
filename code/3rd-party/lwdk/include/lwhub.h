/*
 * LWSDK Header File
 *
 * LWHUB.H -- LightWave Hub
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
#ifndef LWSDK_HUB_H
#define LWSDK_HUB_H

#ifdef __cplusplus
extern "C" {
#endif

#define LWHUB_GLOBAL "LW Hub"

typedef struct st_LWHub {
    int           (*isRunning    )();                                   /*!< Checks to see of the hub is running.     */
    unsigned int  (*countOfAssets)( const char *type );                 /*!< Count of assets of type.                 */
    const char *  (*nameOfAsset  )( const char *type, unsigned int i ); /*!< Name  of assets of type at N'th position.*/
} LWHub;

#ifdef __cplusplus
}
#endif

#endif