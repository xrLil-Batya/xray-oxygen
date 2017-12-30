#ifndef _MGraphNodeIterator
#define _MGraphNodeIterator
// ===========================================================================
// Copyright 2015 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.
//

// ****************************************************************************
// INCLUDED HEADER FILES

#include <maya/MTypes.h>
#include <maya/MEvaluationNode.h>
#include <maya/MEvaluationGraph.h>
#include <maya/MEvaluationNodeIterator.h>
#include <maya/MCustomEvaluatorClusterNode.h>
#include <maya/MObject.h>

// ****************************************************************************
// CLASS DECLARATION (MGraphNodeIterator)

//! \ingroup OpenMaya
//! \brief Provides access to the cluster nodes in a given evaluation graph.
class OPENMAYA_EXPORT MGraphNodeIterator
{
public:
   ~MGraphNodeIterator();
   MGraphNodeIterator(const MCustomEvaluatorClusterNode* cluster, MStatus* ReturnStatus);
   MGraphNodeIterator(const MGraphNodeIterator& iterator, MStatus* ReturnStatus);
   bool isDone(MStatus* ReturnStatus = NULL) const;
   MEvaluationNode currentEvaluationNode(MStatus* ReturnStatus = NULL) const;
   void next(MStatus* ReturnStatus = NULL) const;
   void reset(MStatus* ReturnStatus = NULL);
   
   static const char*	className();

private:
    friend class MCustomEvaluatorClusterNode;

    MGraphNodeIterator(const void* internalData);

    const void* fGraphNodeIterator;
};

#endif /* _MGraphNodeIterator */