#include "MainForm.h"
#include "xrCore/xrCore.h"
#include "xrCore/fs_internal.h"
#define ECORE_API
#define ENGINE_API
#include "../xrECoreLite/EditObject.h"
#pragma comment(lib, "xrECoreLite.lib")
using namespace ECore;

struct Mesh
{
public:
	Mesh(const char* file)
	{
		path = file;
		mesh = new CFileReader(path);
	}

	void ExportOgf()
	{
		CEditableObject obj(path);
		obj.LoadObject(path);
		obj.ExportOGF(path, 4);
	}
private:
	const char* path;
	CFileReader* mesh;
};

#pragma warning(push)
#pragma warning(disable : 4995) // ignore deprecation warnings
#include <msclr/marshal.h>
using msclr::interop::marshal_context;
#pragma warning(pop)

System::Void MeshEdit::loadToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		marshal_context context;
		const char* tmpStr = context.marshal_as<const char*>(openFileDialog1->FileName);
		mesh = new Mesh(tmpStr);
		mesh->ExportOgf();
	}
}