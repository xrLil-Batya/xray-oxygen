#pragma once
#include <string>
#include "xrCore/xrCore.h"
#include "../xrECoreLite/EditObject.h"
#include "../xrECoreLite/Ogf.h"

struct Mesh
{
public:
	std::string path;
	CEditableObject* mesh;

public:
	Mesh(const char* file);

	void ExportOgf();

	CBone* GetBone(int idx) const;
	size_t GetBoneSize() const;
};
