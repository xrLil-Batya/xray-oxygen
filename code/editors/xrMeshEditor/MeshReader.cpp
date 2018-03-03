#include "MainForm.h"
#include "xrCore/xrCore.h"
#include "xrCore/fs_internal.h"
#define ECORE_API
#define ENGINE_API
#include "../xrECoreLite/EditObject.h"

#pragma comment(lib, "xrECoreLite.lib")
using namespace ECore;
#include <string>
struct Mesh
{
public:
	Mesh(const char* file)
	{
		path = file;
		mesh = new CEditableObject(path.c_str());
		mesh->LoadObject(std::string(path).c_str());
	}

	void ExportOgf()
	{
		mesh->ExportOGF(std::string(path + ".ogf").c_str(), 4);
	}

	inline CBone* GetBone(int idx) const
	{
		return mesh->m_Bones[idx];
	}

	inline size_t GetBoneSize() const
	{
		return mesh->m_Bones.size();
	}
private:
	std::string path;
	CEditableObject* mesh;
};

#pragma warning(push)
#pragma warning(disable : 4995) // ignore deprecation warnings
#include <msclr/marshal.h>
using msclr::interop::marshal_context;
#pragma warning(pop)

System::Void MeshEdit::MeshEdit_Load(System::Object^  sender, System::EventArgs^  e)
{
	GEMLib.Load();
}

System::Void MeshEdit::gameMaterialsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	for (u32 it = 0; it < GEMLib.CountMaterial(); it++)
	{
		System::String^ name = gcnew System::String(GEMLib.GetMaterialByIdx(it)->m_Name.c_str());
		this->GMtList->Items->Add(name);
	}
	this->GMtList->Visible = !this->GMtList->Visible;
}

System::Void MeshEdit::BonesList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
}

System::Void MeshEdit::bonesListToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (mesh)
	{
		for (u32 it = 0; it < mesh->GetBoneSize(); it++)
		{
			System::String^ name = gcnew System::String(mesh->GetBone(it)->Name().c_str());
			this->BonesList->Items->Add(name);
		}
	}
	this->BonesList->Visible = !this->BonesList->Visible;
}

System::Void MeshEdit::loadToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		marshal_context context;
		const char* tmpStr = context.marshal_as<const char*>(openFileDialog1->FileName);
		mesh = new Mesh(tmpStr);
	}
}

System::Void MeshEdit::xRayIngameToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) 
{
	mesh->ExportOgf();
}