/*
 * LWSDK Header File
 *
 * LWGLOBSERV.H -- LightWave Global Server
 *
 * This header contains declarations necessary to define a "Global"
 * class server.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_GLOBSERV_H
#define LWSDK_GLOBSERV_H

#define LWGLOBALSERVICE_CLASS   "Global"
#define LWGLOBALSERVICE_VERSION 1

typedef struct st_LWGlobalService {
    const char  *id /* ascii encoded */;
    void        *data;
} LWGlobalService;

#endif
