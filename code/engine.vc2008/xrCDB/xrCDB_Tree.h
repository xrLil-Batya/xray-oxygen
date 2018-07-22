#pragma once

class CDB_OptimizeTree: public Opcode::AABBNoLeafTree
{
public:
	CDB_OptimizeTree();

	void Store	(IWriter* pWriter);
	bool Restore(IReader* pReader);

	bool Build	(Opcode::AABBTree* tree);

};
