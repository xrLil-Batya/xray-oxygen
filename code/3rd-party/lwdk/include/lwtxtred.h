/*
 * LWSDK Header File
 *
 * LWTXTRED.H -- LightWave Texture Editor
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_TXTRED_H
#define LWSDK_TXTRED_H

#include <lwtxtr.h>

#define     LWTXTREDFUNCS_GLOBAL    "Texture Editor 2"

/*
 * Clients can edit textures by subscribing to the texture editor services.
 * The subscription returns an identifier this identifier holds information
 * about the client, some UI flags, etc, that are used in function calls.
 */
typedef struct st_TxtrEdClient  *LWTECltID;

/*
 * Clients can set call backs for the 'Remove Texture',
 * and the 'Automatic Sizing' buttons.
 * For the AutoSize call back, the client needs to fill in bbox
 * with the corners of the texture's bounding box:
 * bbox[][0] = min[],  bbox[][1] = max[]
 */
typedef void    LW_TxtrRemoveFunc(LWTextureID, void *userData);
typedef int     LW_TxtrAutoSizeFunc(LWTextureID, void *userData, double bbox[3][2]);
typedef int     LW_TxtrEventFunc(LWTextureID, void *userData, int eventCode);
typedef int     LW_GradAutoSizeFunc(LWTxtrParamDesc *param, int paramNb, void *userData);

typedef struct  st_LWTxtrEdFuncs {
    LWTECltID       (*subscribe)(const char *title /* language encoded */, int flags, void *userData, LW_TxtrRemoveFunc *, LW_TxtrAutoSizeFunc *,LW_TxtrEventFunc *);
    void            (*unsubscribe)(LWTECltID);

    void            (*open)(LWTECltID, LWTextureID, const char *title /* language encoded */);
    void            (*setTexture)(LWTECltID, LWTextureID, const char  *title /* language encoded */);
    void            (*setPosition)(LWTECltID, int, int);
    void            (*close)(LWTECltID);
    int             (*isOpen)(LWTECltID);
    int             (*refresh)(LWTECltID);                  // forces editor refresh
    LWTLayerID      (*currentLayer)(LWTECltID);             // returns currently selected texture layer

    int             (*selectAdd)(LWTECltID, LWTextureID);   // adds texture to multi-selection
    int             (*selectRem)(LWTECltID, LWTextureID);   // removes texture from multi-selection
    int             (*selectClr)(LWTECltID);                // clears multi-selection
    LWTextureID     (*selectFirst)(LWTECltID);              // returns first texture in the selection
    LWTextureID     (*selectNext)(LWTECltID, LWTextureID);  // returns next texture in the selection

    void            (*setGradientAutoSize) (LWTECltID,LW_GradAutoSizeFunc *);
} LWTxtrEdFuncs;

/*
Editor Flags
*/
#define TEF_USEBTN      (1<<0)  // adds use/remove buttons at the bottom of the pane.
#define TEF_OPACITY     (1<<1)  // adds layer opacity settings
#define TEF_BLEND       (1<<2)  // adds blend options to the layer global settings
#define TEF_TYPE        (1<<3)  // adds layer type control on top of pane.
#define TEF_LAYERS      (1<<4)  // adds layer list pane on left side of the pane.
#define TEF_ALL         31      // standard set of flags for the texture editor

/*
Event Codes passed to the event callback
*/
#define TXEV_ALTER       1
#define TXEV_TRACK       2
#define TXEV_DELETE      4

#endif