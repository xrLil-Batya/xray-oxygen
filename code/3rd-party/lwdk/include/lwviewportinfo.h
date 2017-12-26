/*
 * LWSDK Header File
 *
 * lwviewportinfo.h -- LightWave Viewport Info
 *
 *---------------------------------------------------------------------------
 * Copyright © 2015 NewTek, Inc. and its licensors. All rights reserved.
 *
 * This file contains confidential and proprietary information of
 * NewTek, Inc., and is subject to the terms of the LightWave End User
 * License Agreement (EULA).
 */
#ifndef LWSDK_VIEWPORT_INFO_H
#define LWSDK_VIEWPORT_INFO_H

#include <lwtypes.h>
#include <lwpicking.h>

#define LWVIEWPORTINFO_GLOBAL "LW Viewport Info 7"

#define LVVIEWT_NONE 0
#define LVVIEWT_TOP 1
#define LVVIEWT_BOTTOM 2
#define LVVIEWT_BACK 3
#define LVVIEWT_FRONT 4
#define LVVIEWT_RIGHT 5
#define LVVIEWT_LEFT 6
#define LVVIEWT_PERSPECTIVE 7
#define LVVIEWT_LIGHT 8
#define LVVIEWT_CAMERA 9
#define LVVIEWT_SCHEMATIC 10
#define LVVIEWT_UV 11

#define LWVIEWF_CENTER (1<<0)
#define LWVIEWF_WEIGHTSHADE (1<<1)
#define LWVIEWF_XRAY (1<<2)
#define LWVIEWF_HEADLIGHT (1<<3)

#define LWOVIS_HIDDEN 0
#define LWOVIS_BOUNDINGBOX 1
#define LWOVIS_VERTICES 2
#define LWOVIS_WIREFRAME 3
#define LWOVIS_FFWIREFRAME 4
#define LWOVIS_SHADED 5
#define LWOVIS_TEXTURED 6
#define LWOVIS_TEXTURED_WIRE 7
#define LWOVIS_VIEWPORTOBJ 8
#define LWOVIS_SKETCH 9
#define LWOVIS_WEIGHTS 10
#define LWOVIS_UV_WIRE 11
#define LWOVIS_COLOR_WIRE 11
#define LWOVIS_FLAT_SHADED 12
#define LWOVIS_SHADED_WIRE 13

typedef struct st_LWViewportInfo
{
    // Number of viewports available.
    int numViewports;

    /// @return The type of the nth viewport (ex: LWVIEWT_PERSPECTIVE).
    int (*type)(int n);

    /// @return The flags associated with the nth viewport (ex: LVVIEWT_PERSPECTIVE).
    unsigned int (*flags)(int n);

    /// Fills 'spot' with the viewing position of the nth viewport.
    void (*pos)(int n, LWDVector spot);

    /// Fills 'mat' with a 3x3 transformation from viewport coordinates to world
    /// coordinates for the nth viewport.
    void (*xfrm)(int n, double mat[9]);

    /// Fills 'hither' and 'yon' with the near and far Z clipping distances for
    /// the nth viewport.
    void (*clip)(int n, double* hither, double* yon);

    /// Fills 'left', 'top', 'width' and 'height' with pixel coordinates of the
    /// nth viewport.
    void (*rect)(int n, int* left, int* top, int* width, int* height);

    /// @return The view level for the nth viewport. The returned value is one of the
    /// LWOVIS_* values.
    int (*viewLevel)(int n);

    /// Fills the matrices with the projection matrix and the inverse of the projection
    /// matrix for the nth viewport. It is safe to pass in NULL for either of the matrices.
    /// The matrices follow the OpenGL convention of m[column][row] with column vectors.
    /// @return true if the asked for matrices were retrieved or false on failure which
    /// usually means that an inverse matrix was asked for for a matrix that could not
    /// be inverted.
    int (*projection)(int n, LWDMatrix4 projection, LWDMatrix4 inverse_projection);

    /// Fills the matrices with the modelview matrix and the inverse of the modelview matrix
    /// for the nth viewport. It is safe to pass in NULL for either of the matrices.
    /// The matrices follow the OpenGL convention of m[column][row] with column vectors.
    /// @return true if the asked for matrices were retrieved or false on failure which
    /// usually means that an inverse matrix was asked for for a matrix that could not
    /// be inverted.
    int (*modelview)(int n, LWDMatrix4 modelview, LWDMatrix4 inverse_modelview);

    /// Computes the projected position of the given world coordinates for the nth viewport.
    /// The computed 'winx' and 'winy' position is in pixels, relative to the upper-left corner
    /// of the viewport.
    int (*project)(int n, LWDVector world, double* winx, double* winy, double* winz);

    /// Computes the world coordinates of a given pixel position of the nth viewport. The 'winz'
    /// value ranges from 0 for a spot on the near clipping plane, to 1 for a spot on the far clipping
    /// plane. Note that drawing on the near or far clipping plane may cause flickering due to floating
    /// point imprecision causing random clipping.
    int (*unproject)(int n, double winx, double winy, double winz, LWDVector world);

    /// @return The world size of the given number of pixels at the reference position in world for
    /// the nth viewport. A sphere with a diameter of the returned size, placed at the reference
    /// position, will have a diameter of approximately the given number of pixels when projected.
    double (*pixelSize) (int n, double pixels, LWDVector refpos);

    /// @return The standard handle size for the nth viewport, in pixels. This is the size used
    /// for drawing the translation and rotation tool handles, e.g.
    int (*handleSize)(int n);

    /// @return The size of the grid spacing for the ith viewport, in meters.
    double (*gridSize)(int n);

    /// For viewports which have a view through a scene item (LWVIEWT_LIGHT and LWVIEWT_CAMERA)
    /// the ID of the item is returned. Otherwise LWITEM_NULL is returned.
    LWItemID (*viewItem)(int n);

    /// @return The grid type of the nth viewport.
    int (*gridType)(int n);

    /// @return The picking interface for the nth viewport. Note: this interface is only
    /// currently valid in Modeler. The function returns NULL if no picking interface is
    /// available for the nth viewport.
    struct LWPicking* (*viewPicking)(int n);

    /// @return The index of the viewport currently being hovered over or -1 if there is none.
    /// 'out_cursor_pos' will be filled with the cursor position of the mouse relative to
    /// the hover viewport if there is one.
    int (*hoverIndex)(double out_cursor_pos[2]);

    /// @return The image, if any, shown in the background of the viewport, as well as its
    /// position and size in pixels in the viewport.
    LWPixmapID (*bgImage)(int n, double* x, double* y, double* width, double* height);

    /// @return The wire handle smoothing setting, given as a line thickness, or 0 if off.
    float (*handleSmoothing)(int n);

} LWViewportInfo;

#endif