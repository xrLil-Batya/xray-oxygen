/*
 * LWSDK Header File
 *
 * LWCMDSEQ.H -- LightWave CommandSequence Server
 *
 * This header contains the types and declarations for the Modeler
 * CommandSequence class.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_CMDSEQ_H
#define LWSDK_CMDSEQ_H

#include <lwdyna.h>
#include <lwtypes.h>
#include <lwmeshedt.h>

#define LWMODCOMMAND_CLASS      "CommandSequence"
#define LWMODCOMMAND_VERSION     3

typedef struct st_LWModCommand
{
    void                 *data;
    const char           *argument /* language encoded */;
    LWCommandCode       (*lookup)           (void *, const char *cmdName /* language encoded */);
    int                 (*execute)          (void *, LWCommandCode cmd, int argc, const DynaValue *argv, EltOpSelect, DynaValue *result);
    MeshEditBegin        *editBegin;
    int                 (*evaluate)         (void *, const char *command /* language encoded */);

    void                (*undoGroupBegin)   (void);
    void                (*undoGroupEnd)     (void);

    MeshEditBegin2       *editBegin2;

} LWModCommand;

#define CSERR_NONE   0
#define CSERR_MEMORY     1
#define CSERR_IO     2
#define CSERR_USERABORT  4
#define CSERR_ARGCOUNT   2901
#define CSERR_ARGTYPE    2902
#define CSERR_ARGVALUE   2903
#define CSERR_OPFAILURE  2904
#define CSERR_BADSEL     2905

#endif