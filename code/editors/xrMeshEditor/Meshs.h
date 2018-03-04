#pragma once
#include <string>
#include "../xrECoreLite/EditObject.h"

struct Mesh
{
private:
	std::string path;
	CEditableObject* mesh;

public:
	Mesh(const char* file);

	void ExportOgf();

	CBone* GetBone(int idx) const;
	size_t GetBoneSize() const;
};
