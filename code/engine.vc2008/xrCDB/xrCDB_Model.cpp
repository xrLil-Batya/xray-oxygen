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

	if (create.mSettings.mLimit != 1)	
	{
		Msg("OPCODE WARNING: supports complete trees only! Use mLimit = 1.\n", null);
		return false;
	}

	uqword NbDegenerate = create.mIMesh->CheckTopology();
	
	if (NbDegenerate)	
		Msg("OPCODE WARNING: found %d degenerate faces in model! Collision might report wrong results!\n", NbDegenerate);

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
