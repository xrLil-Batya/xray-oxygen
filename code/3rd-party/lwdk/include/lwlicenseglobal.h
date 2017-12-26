/*
 * LWSDK Header File
 *
 * LWLICENSEGLOBAL.H -- LightWave license global.
 *
 * This header contains the basic declarations need to define the
 * simplest LightWave plug-in server.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_LICENSEGLOBAL_H
#define LWSDK_LICENSEGLOBAL_H

#define LWLICENSE_GLOBAL "License Global"

/* This uses the new dongle-less license key .                  */
/* For plugins that require a special license key to startup.   */
/* This allows a plugin to get the name of the licensed plugin, */
/* and the version of LightWave it was intended to run in.      */


typedef struct st_LWPluginLicenseGlobal {
    int                        (*pluginValid      )( const char * /* ascii encoded */, const char * /* ascii encoded */ ); /* Checks to see if a plugin is valid, by name and version.  */
    struct st_lwPluginLicense *(*nextPluginLicense)( struct st_lwPluginLicense * );     /* Return next global in list, NULL is the head of the list. */
    const char                * /* ascii encoded */ (*pluginName       )( struct st_lwPluginLicense * );     /* Returns the name of the licensed plugin. */
    const char                * /* ascii encoded */ (*lwVersion        )( struct st_lwPluginLicense * );     /* Returns the version of the license key.  */
    unsigned int               (*hardwareLock     )( struct st_lwPluginLicense * );     /* Returns the hardware lock.               */
} LWPluginLicenseGlobal;

#endif