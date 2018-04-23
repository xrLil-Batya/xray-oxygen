#include "stdafx.h"
#include "xrCompressWnd.h"
#include <msclr\marshal.h>

using namespace xrCompress;

const char* FolderName;
const char* KeysList;

void Startup(const char*);

System::Void xrCompressWnd::PackBtn_Click(System::Object^ sender, System::EventArgs^ e)
{
	msclr::interop::marshal_context marsh;

	FolderName = marsh.marshal_as<const char*>(textBox1->Text);

	System::String^ keys = "-pack" + textBox1->Text + " " + "-ltx " + textBox2->Text + (bFastMode->Checked ? " -fast " : " ") + textBox3->Text;
	keys += bDb->Checked ? "-db " : "-xdb ";
	keys += comboBox1->Text;

	KeysList = marsh.marshal_as<const char*>(keys);
	Startup(KeysList);
}