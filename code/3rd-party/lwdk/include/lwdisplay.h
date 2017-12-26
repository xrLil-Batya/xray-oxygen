/*
 * LWSDK Header File
 *
 * LWDISPLAY.H -- LightWave Host Display Info
 *
 * The host application provides a variety of services for getting user
 * input, but if all else fails the plug-in may need to open windows.
 * Since it runs in the host's context, it needs to get the host's display
 * information to do this.  This info, which can be normally accessed
 * with the "Host Display Info" global service, contains information about
 * the windows and display context used by the host.  If this ID yeilds a
 * null pointer, the server is probably running in a batch mode and has no
 * display context.
 *
 * Try to avoid using this, as it is not future-proof.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_DISPLAY_H
#define LWSDK_DISPLAY_H

#ifdef _MSWIN
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0502
    #endif
    #include <windows.h>
#endif

#ifdef _LINUX
    #warning not available for _LINUX
#endif

#if defined( _MACOS)
    #include <ApplicationServices/ApplicationServices.h>
#endif

/*
 * The fields of the HostDisplayInfo structure vary from system to system,
 * but all include the window pointer of the main application window or
 * null if there is none.  On X systems, the window session handle is
 * passed.  On Win32 systems, the application instance is provided, even
 * though it belongs to the host and is probably useless.
 */
#define LWHOSTDISPLAYINFO_GLOBAL    "Host Display Info"

typedef struct st_HostDisplayInfo {

#ifdef _MSWIN
    HANDLE       instance;
    HWND         window;
#endif

#ifdef _MACOS
    union {
        void *view;   /* Cocoa-based host 9.6.1+ (NSView*) */
    } host;
#endif

} HostDisplayInfo;

#endif
