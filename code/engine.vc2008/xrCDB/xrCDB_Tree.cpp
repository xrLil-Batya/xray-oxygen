//////////////////////////////////////////////////////////
// Desc   : Collision Detection OptTree + Cache System
// Author : ForserX
//////////////////////////////////////////////////////////
// Oxygen Engine (2016-2019)
//////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrCDB_Tree.h"
#include "../../3rd-party/OPCODE/OPC_TreeBuilders.h"
#include "../../3rd-party/OPCODE/Opcode.h"

CDB_OptimizeTree::CDB_OptimizeTree()
{
	// Base constructor
}

void CDB_OptimizeTree::Store(IWriter * pWriter)
{
	pWriter->w_s64(mNbNodes);
	pWriter->w(mNodes, mNbNodes * sizeof(Opcode::AABBNoLeafNode));
}

bool CDB_OptimizeTree::Restore(IReader * pReader)
{
	// Read nodes
    if (pReader->elapsed() < sizeof(s64))
    {
        Msg("* Level collision cache file DB missing mNbNodes");
		FS.r_close(pReader);
        return false;
    }

	// Get nodes count and set default size
	mNbNodes = pReader->r_s64();
	const size_t mNodesSize = mNbNodes * sizeof(Opcode::AABBNoLeafNode);

    if (pReader->elapsed() < mNodesSize)
    {
        Msg("* Level collision DB cache file don't have enough nodes");
		FS.r_close(pReader);
        return false;
    }
	mNodes = xr_alloc<Opcode::AABBNoLeafNode>(mNbNodes);
	R_ASSERT2(mNodes, "Error alloc for cform cache...");

	ZeroMemory(mNodes, mNodesSize);
	pReader->r(mNodes, mNodesSize);
	
	// Validate Pos and Neg data
	uqword oldbase = 0, newbase = (uqword)mNodes;
	
	if (!mNodes[0].HasPosLeaf() || !mNodes[0].HasNegLeaf())
	{
		oldbase = mNodes[0].mPosData - sizeof(Opcode::AABBNoLeafNode);
	}
	else
	{
		R_ASSERT2(false, "Error reading mPos or mNeg!");
	}

	for (uqword CurID = 0; CurID < mNbNodes; ++CurID)
	{
		Opcode::AABBNoLeafNode& refNode = mNodes[CurID];

		// Positive non-leaf node
		if (!refNode.HasPosLeaf())
			refNode.mPosData = newbase + (refNode.mPosData - oldbase);

		// Negative non-leaf node
		if (!refNode.HasNegLeaf())
			refNode.mNegData = newbase + (refNode.mNegData - oldbase);
	}
	FS.r_close(pReader);
	return true;
}

bool CDB_OptimizeTree::Build(Opcode::AABBTree* tree)
{
	// Checkings
	if (!tree)	return false;
	// Check the input tree is complete
	uqword NbTriangles = tree->GetNbPrimitives();
	uqword NbNodes = tree->GetNbNodes();
	if (NbNodes != NbTriangles * 2 - 1)	return false;

	// Get nodes
	if (mNbNodes != NbTriangles - 1)	// Same number of nodes => keep moving
	{
		mNbNodes = NbTriangles - 1;
		DELETEARRAY(mNodes);
		mNodes = new Opcode::AABBNoLeafNode[mNbNodes];
		CHECKALLOC(mNodes);
	}

	// Build the tree
	uqword CurID = 1;
	Opcode::_BuildNoLeafTree(mNodes, 0, CurID, tree);
	R_ASSERT(CurID == mNbNodes);

	return true;
}
