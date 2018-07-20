#include "stdafx.h"
#include "CDB_Model.h"
#include "../../3rd-party/OPCODE/OPC_TreeBuilders.h"
#include "../../3rd-party/OPCODE/Opcode.h"

CDB_OptimizeTree::CDB_OptimizeTree(Opcode::AABBNoLeafTree* pParent) : Opcode::AABBNoLeafTree()
{
	this->mNbNodes = pParent->GetNbNodes();
	this->mNodes = const_cast<Opcode::AABBNoLeafNode*>(pParent->GetNodes());
}

CDB_Model::CDB_Model() : Opcode::Model()
{
//	pNewTree = nullptr;
}

void CDB_Model::Store(IWriter* writer)
{
	writer->w_u64(this->mModelCode);
}

void CDB_Model::Restore(IReader* reader)
{
	this->mModelCode = reader->r_u64();

}

void CDB_OptimizeTree::Store(IWriter * pWriter)
{
	pWriter->w_s64(mNbNodes);

	if (mNodes)
	{
		pWriter->w_float(mNodes->mAABB.mCenter.x);
		pWriter->w_float(mNodes->mAABB.mCenter.y);
		pWriter->w_float(mNodes->mAABB.mCenter.z);

		pWriter->w_float(mNodes->mAABB.mExtents.x);
		pWriter->w_float(mNodes->mAABB.mExtents.y);
		pWriter->w_float(mNodes->mAABB.mExtents.z);

		pWriter->w_s64(mNodes->mNegData);
		pWriter->w_s64(mNodes->mPosData);
	}
	else
	{
		for(u32 it = 0; it < 6; it++)
			pWriter->w_float(0.f);

		pWriter->w_s64(0ll);
		pWriter->w_s64(0ll);
	}
}

bool CDB_OptimizeTree::Restore(IReader * pReader)
{
	// Read nodes
	mNbNodes = pReader->r_s64();
	mNodes = xr_alloc<Opcode::AABBNoLeafNode>(mNbNodes);
	if (!mNodes)
		return false;

	ZeroMemory(mNodes, mNbNodes * sizeof(Opcode::AABBNoLeafNode));

	// Read mAABB
	mNodes->mAABB.mCenter.x = pReader->r_float();
	mNodes->mAABB.mCenter.y = pReader->r_float();
	mNodes->mAABB.mCenter.z = pReader->r_float();

	mNodes->mAABB.mExtents.x = pReader->r_float();
	mNodes->mAABB.mExtents.y = pReader->r_float();
	mNodes->mAABB.mExtents.z = pReader->r_float();

	// Read pos and neg data
	mNodes->mNegData = pReader->r_s64();
	mNodes->mPosData = pReader->r_s64();

	uintptr_t oldbase = 0, newbase = (uintptr_t)mNodes;

	if (!mNodes[0].HasPosLeaf())// Positive non-leaf node
	{
		oldbase = mNodes[0].mPosData - sizeof(Opcode::AABBNoLeafNode);
	}
	else if (!mNodes[0].HasNegLeaf())// Negative non-leaf node
	{
		oldbase = mNodes[0].mNegData - sizeof(Opcode::AABBNoLeafNode);
	}

	for (udword i = 0; i < mNbNodes; ++i)
	{
		Opcode::AABBNoLeafNode& node = mNodes[i];
		if (!mNodes[0].HasPosLeaf()) // Positive non-leaf node
		{
			node.mPosData = newbase + (node.mPosData - oldbase);
		}
		if (!mNodes[0].HasNegLeaf()) // Negative non-leaf node
		{
			node.mPosData = newbase + (node.mPosData - oldbase);
		}
	}
	return true;
}