#include "stdafx.h"
#include "CDB_Model.h"
#include "../../3rd-party/OPCODE/OPC_TreeBuilders.h"
#include "../../3rd-party/OPCODE/Opcode.h"

CDB_Model::CDB_Model() : Opcode::Model()
{
	pTree = new CDB_OptimizeTree();
}

void CDB_Model::Store(IWriter* writer)
{
	writer->w_u64(this->mModelCode);
	this->pTree->Store(writer);
}

void CDB_Model::Restore(IReader* reader)
{
	this->mModelCode = reader->r_u64();
	this->pTree->Restore(reader);
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

void CDB_OptimizeTree::Store(IWriter * pWriter)
{
	pWriter->w_s64(mNbNodes);
	pWriter->w(mNodes, mNbNodes * sizeof(Opcode::AABBNoLeafNode));
}

CDB_OptimizeTree::CDB_OptimizeTree() : Opcode::AABBNoLeafTree()
{
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
	ASSERT(CurID == mNbNodes);

	return true;
}

bool CDB_OptimizeTree::Restore(IReader * pReader)
{
	// Read nodes
	mNbNodes = pReader->r_s64();
	mNodes = xr_alloc<Opcode::AABBNoLeafNode>(mNbNodes);
	if (!mNodes)
		return false;

	ZeroMemory(mNodes, mNbNodes * sizeof(Opcode::AABBNoLeafNode));
	pReader->r(mNodes, mNbNodes * sizeof(Opcode::AABBNoLeafNode));

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