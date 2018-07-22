#pragma once
#include "xrCDB_Tree.h"

class CDB_Model: public Opcode::Model
{
public:
	void Store	(IWriter* pWriter);
	bool Restore(IReader* pReader);

public:
	CDB_Model	();
	bool Build	(const Opcode::OPCODECREATE& create);
IC  CDB_OptimizeTree*	GetTree() { return pTree; }

protected:
    CDB_OptimizeTree * pTree;
};