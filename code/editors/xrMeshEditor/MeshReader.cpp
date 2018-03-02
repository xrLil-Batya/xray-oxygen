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
		mesh = new CFileReader(path.c_str());
	}

	void ExportOgf()
	{
		CEditableObject obj(path.c_str());

		obj.LoadObject(std::string(path).c_str());
		obj.ExportOGF(std::string(path + ".ogf").c_str(), 4);
	}
private:
	std::string path;
	CFileReader* mesh;
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