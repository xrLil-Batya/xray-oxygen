#pragma once

class CDB_OptimizeTree: public Opcode::AABBNoLeafTree
{
public:
	CDB_OptimizeTree();

	void Store	(IWriter* pWriter);
	bool Restore(IReader* pReader);

	bool Build	(Opcode::AABBTree* tree);
};

class CDB_Model: public Opcode::Model
{
public:
	void Store	(IWriter* pWriter);
	void Restore(IReader* pReader);

public:
	CDB_Model	();
	bool Build	(const Opcode::OPCODECREATE& create);
IC  CDB_OptimizeTree*	GetTree() { return pTree; }

protected:
    CDB_OptimizeTree * pTree;
};