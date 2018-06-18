#define ECORE_API
#define ENGINE_API
#include "Meshs.h"

Mesh::Mesh(const char* file) 
{
	path = file;
	mesh = new CEditableObject(path.c_str());
	mesh->LoadObject(path.c_str());
}

void Mesh::ExportOgf()
{
	mesh->ExportOGF(std::string(path + ".ogf").c_str(), 4);
}

CBone* Mesh::GetBone(int idx) const
{
	return mesh->m_Bones[idx];
}

size_t Mesh::GetBoneSize() const
{
	return mesh->m_Bones.size();
}