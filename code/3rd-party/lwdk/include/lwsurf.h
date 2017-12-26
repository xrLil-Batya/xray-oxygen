/*
 * LWSDK Header File
 *
 * LWSURF.H -- LightWave Surfaces
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_SURF_H
#define LWSDK_SURF_H

typedef void *LWSurfaceID;
typedef void *LWSurfLibID;

#include    <lwrender.h>
#include    <lwtxtr.h>
#include    <lwenvel.h>
#include    <lwimage.h>
#include    <lwnodeeditor.h>

#define LWSURFACEFUNCS_GLOBAL   "Surface Functions 4"

#define SURF_COLR    "BaseColor"
#define SURF_LUMI    "Luminosity"
#define SURF_DIFF    "Diffuse"
#define SURF_SPEC    "Specularity"
#define SURF_REFL    "Reflectivity"
#define SURF_TRAN    "Transparency"
#define SURF_TRNL    "Translucency"
#define SURF_RIND    "IOR"
#define SURF_BUMP    "Bump"
#define SURF_GLOS    "Glossiness"
#define SURF_BUF1    "SpecialBuffer1"
#define SURF_BUF2    "SpecialBuffer2"
#define SURF_BUF3    "SpecialBuffer3"
#define SURF_BUF4    "SpecialBuffer4"
#define SURF_SHRP    "DiffuseSharpness"
#define SURF_BDRP    "BumpDropoff"
#define SURF_SMAN    "SmoothingAngle"
#define SURF_RSAN    "ReflectionSeamAngle"
#define SURF_TSAN    "RefractionSeamAngle"
#define SURF_RBLR    "ReflectionBlurring"
#define SURF_TBLR    "RefractionBlurring"
#define SURF_CLRF    "ColorFilter"
#define SURF_CLRH    "ColorHighlights"
#define SURF_ADTR    "AdditiveTransparency"
#define SURF_AVAL    "AlphaValue"
#define SURF_GVAL    "GlowValue"
#define SURF_LCOL    "LineColor"
#define SURF_LSIZ    "LineSize"
#define SURF_ALPH    "AlphaOptions"
#define SURF_RFOP    "ReflectionOptions"
#define SURF_TROP    "RefractionOptions"
#define SURF_SIDE    "Sidedness"
#define SURF_NVSK    "ExcludeFromVStack"
#define SURF_GLOW    "Glow"
#define SURF_LINE    "RenderOutlines"
#define SURF_RIMG    "ReflectionImage"
#define SURF_TIMG    "RefractionImage"
#define SURF_VCOL    "VertexColoring"
#define SURF_NORM    "VertexNormal"
#define SURF_CMAP    "ClipMap"

typedef struct st_LWSurfaceFuncs {
    LWSurfaceID   (*create)(const char *objName /* language encoded */ ,const char *surfName /* language encoded */);
    LWSurfaceID   (*first)(void);
    LWSurfaceID   (*next)(LWSurfaceID surf);
    LWSurfaceID  *(*byName)(const char *name /* language encoded */ ,const char *objName /* language encoded */);
    LWSurfaceID  *(*byObject)(const char *name /* language encoded */);
    const char   * /* language encoded */ (*name)(LWSurfaceID surf);
    const char   * /* language encoded */ (*sceneObject)(LWSurfaceID surf);

    int           (*getInt)(LWSurfaceID surf,const char *channel /* language encoded */);
    double       *(*getFlt)(LWSurfaceID surf,const char *channel /* language encoded */);
    LWEnvelopeID  (*getEnv)(LWSurfaceID surf,const char *channel /* language encoded */);
    LWTextureID   (*getTex)(LWSurfaceID surf,const char *channel /* language encoded */);
    LWImageID     (*getImg)(LWSurfaceID surf,const char *channel /* language encoded */);

    LWChanGroupID (*chanGrp)(LWSurfaceID surf);
    const char   * /* language encoded */ (*getColorVMap)(LWSurfaceID surf);
    void          (*setColorVMap)(LWSurfaceID surf,const char *vmapName /* language encoded */,int type);

    LWSurfLibID   (*createLib)(void);
    void          (*destroyLib)(LWSurfLibID lib);
    void          (*copyLib)(LWSurfLibID to, LWSurfLibID from);
    LWSurfLibID   (*objectLib)(const char *objname /* language encoded */);
    LWSurfLibID   (*loadLib)(const char *name /* language encoded */);
    LWError       (*saveLib)(LWSurfLibID lib, const char *name /* language encoded */);
    int           (*slibCount)(LWSurfLibID lib);
    LWSurfaceID   (*slibByIndex)(LWSurfLibID lib,int idx);
    NodeEditorID  (*getNodeEditor)( LWSurfaceID surf);

    int           (*setInt)(LWSurfaceID,const char * /* language encoded */, int);
    int           (*setFlt)(LWSurfaceID,const char * /* language encoded */, double *);
    int           (*setEnv)(LWSurfaceID,const char * /* language encoded */, LWEnvelopeID);
    int           (*setTex)(LWSurfaceID,const char * /* language encoded */, LWTextureID);
    int           (*setImg)(LWSurfaceID,const char * /* language encoded */, LWImageID);
    int           (*rename)(LWSurfaceID,const char * /* language encoded */);
    int           (*copy)(LWSurfaceID,LWSurfaceID);

    void          (*copyLibByName)  (LWSurfLibID to, LWSurfLibID from);
    const char* /* language encoded */ (*getNormalVMap)
                                    (LWSurfaceID surf);
    const char* /* language encoded */ (*server)
                                    (LWSurfaceID, int);
    unsigned int  (*serverFlags)    (LWSurfaceID, int);
    LWInstance    (*serverInstance) (LWSurfaceID, int);
} LWSurfaceFuncs;

#endif