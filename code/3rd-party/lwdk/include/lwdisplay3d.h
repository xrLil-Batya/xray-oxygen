/*
 * LWSDK Header File
 *
 * LWDISPLAY3D.H -- LightWave Display 3D
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_DISPLAY3D
#define LWSDK_DISPLAY3D

#ifdef __cplusplus
extern "C" {
#endif

#include <lwimage.h>
#include <lwrender.h>

#define LWDISPLAY3D_HCLASS  "Display3DHandler"
#define LWDISPLAY3D_ICLASS  "Display3DInterface"
#define LWDISPLAY3D_VERSION 2

enum en_LW3dDisplay {
    LW3D_show = 0,              /*!< Show window.          */
    LW3D_hide,                  /*!< Hide window.          */
    LW3D_sizeof
};


typedef struct st_LWDisplay3DLocal {
    void           *priv_data;  /*!< Private data pointer. */
    int             viewNumber; /*!< Viewport number.      */
} LWDisplay3DLocal;

/**
 *  @short renderEye
 *
 *   LWCAMEYE_CENTER
 *   LWCAMEYE_LEFT
 *   LWCAMEYE_RIGHT
 */

typedef struct st_LWDisplay3DHandler {
    LWInstanceFuncs *inst;
    LWItemFuncs     *item;      /*!< create, destroy, copy, load, save, descln. */
    int            (*setImage )( LWInstance, int renderEye, LWPixmapID  image );
    int            (*getImage )( LWInstance, int renderEye, LWPixmapID *image );
    int            (*showImage)( LWInstance, int mode );
    int            (*viewPort )( LWInstance );
} LWDisplay3DHandler;

#ifdef __cplusplus
}
#endif

#endif