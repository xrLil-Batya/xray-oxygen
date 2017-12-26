/*
 * LWSDK Header File
 *
 * LWCHANNEL.H -- LightWave Channel Filters
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_CHANNEL_H
#define LWSDK_CHANNEL_H

#include <lwrender.h>
#include <lwtypes.h>

#define LWCHANNEL_HCLASS    "ChannelHandler"
#define LWCHANNEL_ICLASS    "ChannelInterface"
#define LWCHANNEL_GCLASS    "ChannelGizmo"
#define LWCHANNEL_VERSION   4


typedef struct st_LWChannelAccess {
    LWChannelID   chan;
    LWFrame       frame;
    LWTime        time;
    double        value;
    void         (*getChannel)  (LWChannelID chan, LWTime t, double *value);
    void         (*setChannel)  (LWChannelID chan, const double value);
    const char * /* language encoded */ (*channelName) (LWChannelID chan);
} LWChannelAccess;

typedef struct st_LWChannelHandler {
    LWInstanceFuncs  *inst;
    LWItemFuncs      *item;
    void            (*evaluate) (LWInstance, const LWChannelAccess *);
    unsigned int    (*flags)    (LWInstance);
} LWChannelHandler;

#endif