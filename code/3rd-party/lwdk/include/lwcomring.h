/*
 * LWSDK Header File
 *
 * LWCOMRING.H -- LightWave Communications Ring
 *
 * This header contains declarations necessary to engage in a
 * communications ring among plug-ins
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_COMRING_H
#define LWSDK_COMRING_H

#include <lwtypes.h>

#define LWCOMRING_GLOBAL "LW Communication Ring"

typedef void    (*RingEvent)    (void *clientData,void *portData,int eventCode,void *eventData);


typedef struct st_LWComRing
{
    int         (*ringAttach)   (char *topic /* ? encoded */,LWInstance pidata,RingEvent eventCallback);
    void        (*ringDetach)   (char *topic /* ? encoded */,LWInstance pidata);
    void        (*ringMessage)  (char *topic /* ? encoded */,int eventCode,void *eventData);
} LWComRing;

#define LW_PLUGIN_LIMBO_STATES "LW Plugin Limbo States"

#define LW_LIMBO_START 0x00000001
#define LW_LIMBO_END   0x00000002

/* Color space events. */

#define LW_PORT_COLORSPACE "color-space-change"

enum {
    LWCSEV_CHANGE = 0,
    LWCSEV_SIZEOF
};

/**
 *      For applications with multiple non-modal windows, it is often useful for
 *      the host and the client windows to be able to broadcast state information
 *      pertinent to those windows.
 *
 *      If the window data is NULL, then open or close your window.
 *      If the window data is not NULL, then the data is the name of the window that was opened or closed.
 */

#define LW_PORT_WINSTATE   "window-state"

enum {
    LWWSEV_CLOSE = 0,   /* Close window. */
    LWWSEV_OPEN,        /* Open  window. */
    LWWSEV_SIZEOF
};


/**     Master Events
 *      For applications that need access to master events without needing the overhead of a full Master class
 *      plugin. The data received from the port is the same as the data provided by the event() callback
 *      for Master class plugins, as defined in lwmaster.h
 */

#define LW_PORT_MASTER_EVENT   "master-event"


#endif