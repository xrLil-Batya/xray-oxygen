#pragma once

class CDB_OptimizeTree: public Opcode::AABBNoLeafTree
{
public:
	CDB_OptimizeTree(Opcode::AABBNoLeafTree* pParent);

	void Store	(IWriter* pWriter);
	bool Restore(IReader* pReader);
};

class CDB_Model: public Opcode::Model
{
public:
	void Store	(IWriter* pWriter);
	void Restore(IReader* pReader);

public:
	CDB_Model();
};