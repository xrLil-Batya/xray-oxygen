#ifndef _MRenderSetup
#define _MRenderSetup
//
//-
// ===========================================================================
// Copyright 2016 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.
// ===========================================================================
//+
//
// ****************************************************************************

#if defined __cplusplus

// ****************************************************************************
// INCLUDED HEADER FILES


// For NULL.
#include <maya/MTypes.h>

// ****************************************************************************
// FORWARD DECLARATIONS

class MStatus;
class MString;
class MStringArray;

// ****************************************************************************
// CLASS DECLARATION (MRenderSetup)
//
//! \ingroup OpenMayaRender
//! \brief Utilities for obtaining render setup information.

class OPENMAYARENDER_EXPORT MRenderSetup
{
public:
	static const char* className();

    static MStringArray getEnabledSelectedNodeNames(
        const MString& renderLayerName,
        MStatus*       status = NULL
    );

private:

    ~MRenderSetup();

#ifdef __GNUC__
	friend class shutUpAboutPrivateDestructors;
#endif
};

#endif /* __cplusplus */
#endif /* _MRenderSetup */
