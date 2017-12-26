/*
 * LWSDK Library Source File
 *
 * Default 'Startup' function returns any non-zero value for success.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#include <lwserver.h>

void *Startup( void); // prototype

void *Startup (void)
{
    return (void *) 4;
}