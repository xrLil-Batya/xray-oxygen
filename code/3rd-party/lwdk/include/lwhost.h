/*
 * LWSDK Header File
 *
 * LWHOST.H -- LightWave Host Services
 *
 * This header contains the declarations for globals provided by all
 * LightWave host applications.
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_HOST_H
#define LWSDK_HOST_H

#include <lwdialog.h>

/*
 * File Request Function.  This function is returned as the "File Request"
 * global service.  It gets a simple filename from the user.
 */
#define LWFILEREQFUNC_GLOBAL "File Request"

typedef int LWFileReqFunc ( const char *hail /* language encoded */, char *name /* language encoded */, char *path /* language encoded */, char *fullName /* language encoded */, int buflen );

/*
 * File Request Activate Function.  This function is returned as the
 * "File Request 2" global service.  It takes a file request local struct
 * and version number and returns the activation code directly.
 */
#define LWFILEACTIVATEFUNC_GLOBAL "File Request 2"

typedef int LWFileActivateFunc (int version, LWFileReqLocal *);


/*
 * Color Picker Activate Function.  This function is returned as the
 * "Color Picker" global service.  It takes a color picker local struct
 * and version number and returns the activation code directly.
 */
#define LWCOLORACTIVATEFUNC_GLOBAL "Color Picker"

typedef int LWColorActivateFunc (int version, LWColorPickLocal *);

/*
 * File Type Function.  This function is returned as the "File Type Pattern"
 * global service.  It returns filename pattern strings given file type
 * code strings.
 */
#define LWFILETYPEFUNC_GLOBAL "File Type Pattern"

typedef const char * /* ascii encoded */ LWFileTypeFunc (const char *filetype /* ascii encoded */);

/*
 * Directory Function.  This function is returned as the "Directory Info"
 * global service.  It returns a directory given file type code strings.
 */
#define LWDIRINFOFUNC_GLOBAL "Directory Info"

typedef const char * /* language encoded */ LWDirInfoFunc (const char *filetype /* ascii encoded */);

/*
 * These file type code strings are recognized by the File Type and Directory Info
 * Functions. They correspond with the current configuration for the system.
 */
#define LWFTYPE_CONTENT     "Content"
#define LWFTYPE_ANIMATION   "Animations"
#define LWFTYPE_IMAGE       "Images"
#define LWFTYPE_ENVELOPE    "Envelopes"
#define LWFTYPE_MOTION      "Motions"
#define LWFTYPE_OBJECT      "Objects"
#define LWFTYPE_PLUGIN      "Plug-ins"
#define LWFTYPE_PREVIEW     "Previews"
#define LWFTYPE_PSFONT      "PSFonts"
#define LWFTYPE_SCENE       "Scenes"
#define LWFTYPE_SETTING     "Settings"
#define LWFTYPE_SURFACE     "Surfaces"
#define LWFTYPE_NODES       "Nodes"
#define LWFTYPE_SOUNDS      "Sounds"
#define LWFTYPE_DYNAMICS    "Dynamics"
#define LWFTYPE_RIGS        "Rigs"
#define LWFTYPE_INSTALL     "Install"
#define LWFTYPE_LIGHTS      "Lights"
#define LWFTYPE_RADIOSITY   "Radiosity"
#define LWFTYPE_VERTCACHE   "VertCache"
#define LWFTYPE_SHADERS     "Shaders"

/****
 * Message Functions.  This block of functions is returned as the
 * "Info Messages 3" global service.  They display various info
 * and other confirmation dialogs to the user.
 * The return codes are as follows:
 *  OKCancel                     ok(1) cancel(0)
 *  YesNo                       yes(1) no(0)
 *  YesNoCancel           yes(2) no(1) cancel(0)
 *  YesNoAll    yesAll(3) yes(2) no(1) cancel(0)
 ****
 */
#define LWMESSAGEFUNCS_GLOBAL       "Info Messages 3"

typedef struct st_LWMessageFuncs
{
    void    (*info)     (const char *line_a /* language encoded */, const char *line_b /* language encoded */);
    void    (*error)    (const char *line_a /* language encoded */, const char *line_b /* language encoded */);
    void    (*warning)  (const char *line_a /* language encoded */, const char *line_b /* language encoded */);
    int     (*okCancel) (const char *title /* language encoded */, const char *line_a /* language encoded */, const char *line_b /* language encoded */);
    int     (*yesNo)    (const char *title /* language encoded */, const char *line_a /* language encoded */, const char *line_b /* language encoded */);
    int     (*yesNoCan) (const char *title /* language encoded */, const char *line_a /* language encoded */, const char *line_b /* language encoded */);
    int     (*yesNoAll) (const char *title /* language encoded */, const char *line_a /* language encoded */, const char *line_b /* language encoded */);
    const char * /* language encoded */ (*askName) (const char *title /* language encoded */, const char *label /* language encoded */, const char *initial_name /* language encoded */);
} LWMessageFuncs;

/*
 * System Information.  The value returned as the "System ID" global is
 * a value that should be parsed into bits.  The low bits are the dongle
 * serial number and the high bits are the application code.
 */
#define LWSYSTEMID_GLOBAL   "System ID"

#define LWSYS_TYPEBITS      0xF0000000
#define LWSYS_SERIALBITS    0x0FFFFFFF

#define LWSYS_LAYOUT        0x00000000
#define LWSYS_MODELER       0x10000000
#define LWSYS_SCREAMERNET   0x20000000
#define LWSYS_OTHER         0x80000000

/*
 * Product Information.  The value returned as the "Product Info" global
 * is a value that should be parsed into bits.  The various groups of
 * bits contain codes for the specific product and revision.
 */
#define LWPRODUCTINFO_GLOBAL    "Product Info"

#define LWINF_PRODUCT           0x0000000F
#define LWINF_BUILD             0x0000FFF0
#define LWINF_MINORREV          0x000F0000
#define LWINF_MAJORREV          0x00F00000
#define LWINF_MODELYEAR         0x3F000000
#define LWINF_RESERVED          0xC0000000

#define LWINF_PRODLWAV          0x00000001
#define LWINF_PRODINSP3D        0x00000002
#define LWINF_PRODOTHER         0x00000004

#define LWINF_GETMAJOR(x)       (((x) & LWINF_MAJORREV)   >> 20)
#define LWINF_GETMINOR(x)       (((x) & LWINF_MINORREV)   >> 16)
#define LWINF_GETBUILD(x)       (((x) & LWINF_BUILD)      >>  4)
/*
 * Model year valid only when major revision is 15.
 * Model years range from 2015 to 2078.
 * Model year 2078 is an invalid model year.
 */
#define LWINF_GETMODELYEAR(x)   ((((x) & LWINF_MODELYEAR) >> 24) + 2015)

/*
 * Locale Information.  The value returned as the "Locale Info" global
 * is a value whose low bits contain the locale ID for the host
 * application.  The possible values are the LANGID codes defined in
 * lwserver.
 */
#define LWLOCALEINFO_GLOBAL "Locale Info"

#define LWLOC_LANGID            0x0000FFFF
#define LWLOC_RESERVED          0xFFFF0000

/*
 * License Information.  The value returned as the "License Info" global
 * is a value that should be parsed into bits.  The low bits are the license
 * serial number and the high bits indicate the type of licensing currently
 * active in the product.
 */
#define LWLICENSEINFO_GLOBAL    "License Info"

#define LWLIC_MODEBITS      0xF0000000
#define LWLIC_VALUEBITS     0x0FFFFFFF

#define LWLIC_MODE_P2P      0x00000000
#define LWLIC_MODE_SERVER   0x10000000

#endif