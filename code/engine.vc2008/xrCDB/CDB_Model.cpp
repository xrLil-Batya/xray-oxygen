#include "stdafx.h"
#include "CDB_Model.h"
#include "../../3rd-party/OPCODE/OPC_TreeBuilders.h"
#include "../../3rd-party/OPCODE/Opcode.h"

CDB_Model::CDB_Model()
{
	pTree = new CDB_OptimizeTree();
}

void CDB_Model::Store(IWriter* writer)
{
	writer->w_u64(mModelCode);
	pTree->Store(writer);
}

void CDB_Model::Restore(IReader* reader)
{
	mModelCode = reader->r_u64();
	pTree->Restore(reader);
}

bool CDB_Model::Build(const Opcode::OPCODECREATE& create)
{
	if (!create.mIMesh || !create.mIMesh->IsValid())	return false;

	if (create.mSettings.mLimit != 1)	return SetIceError("OPCODE WARNING: supports complete trees only! Use mLimit = 1.\n", null);

	uqword NbDegenerate = create.mIMesh->CheckTopology();
	if (NbDegenerate)	Msg("OPCODE WARNING: found %d degenerate faces in model! Collision might report wrong results!\n", NbDegenerate);

	Release();
	SetMeshInterface(create.mIMesh);

	uqword NbTris = create.mIMesh->GetNbTriangles();
	if (NbTris == 1)
	{
		mModelCode |= Opcode::OPC_SINGLE_NODE;
		return true;
	}

	mSource = new Opcode::AABBTree;
	CHECKALLOC(mSource);

	{
		Opcode::AABBTreeOfTrianglesBuilder TB;
		TB.mIMesh = create.mIMesh;
		TB.mSettings = create.mSettings;
		TB.mNbPrimitives = NbTris;
		if (!mSource->Build(&TB))	return false;
	}

	if (!CreateTree(create.mNoLeaf, create.mQuantized))	return false;

	if (!pTree->Build(mSource))	return false;

	if (!create.mKeepOriginal)	DELETESINGLE(mSource);

	return true;
}

////////////////////////
// CDB_OptimizeTree

CDB_OptimizeTree::CDB_OptimizeTree()
{
	// Base constructor
}

void CDB_OptimizeTree::Store(IWriter * pWriter)
{
	pWriter->w_s64(mNbNodes);
	pWriter->w(mNodes, mNbNodes * sizeof(Opcode::AABBNoLeafNode));
#if 0
	pWriter->w_float(mNodes->mAABB.mCenter.x);
	pWriter->w_float(mNodes->mAABB.mCenter.y);
	pWriter->w_float(mNodes->mAABB.mCenter.z);

	pWriter->w_float(mNodes->mAABB.mExtents.x);
	pWriter->w_float(mNodes->mAABB.mExtents.y);
	pWriter->w_float(mNodes->mAABB.mExtents.z);

	pWriter->w_s64(mNodes->mNegData);
	pWriter->w_s64(mNodes->mPosData);
#endif
}

bool CDB_OptimizeTree::Restore(IReader * pReader)
{
	// Read nodes
	mNbNodes = pReader->r_s64();
	mNodes = xr_alloc<Opcode::AABBNoLeafNode>(mNbNodes);
	if (!mNodes)
		return false;

	ZeroMemory(mNodes, mNbNodes * sizeof(Opcode::AABBNoLeafNode));
#if 0
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
#endif
	pReader->r(mNodes, mNbNodes * sizeof(Opcode::AABBNoLeafNode));
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
		Opcode::AABBNoLeafNode& node = mNodes[CurID];

		// Positive non-leaf node
		if (!node.HasPosLeaf())
		{
			node.mPosData = newbase + (node.mPosData - oldbase);
		}

		// Negative non-leaf node
		if (!node.HasNegLeaf())
		{
			node.mNegData = newbase + (node.mNegData - oldbase);
		}
	}
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
