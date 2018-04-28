#include "stdafx.h"
#include "xrCompressWnd.h"
#include "IParserSystem.h"
#include <msclr\marshal.h>

using namespace xrCompress;
extern bool SecondThreadWorking;
const char* FolderName;
const char* KeysList;

void Startup(void*);

System::Void xrCompressWnd::PackBtn_Click(System::Object^ sender, System::EventArgs^ e)
{
	msclr::interop::marshal_context marsh;
	KeysList = nullptr;
	FolderName = marsh.marshal_as<const char*>(textBox1->Text);

	System::String^ keys = "-pack " + "-ltx " + textBox2->Text + (bFastMode->Checked ? " -fast " : " ") + textBox3->Text;
	keys += bDb->Checked ? " -db " : " -xdb ";
	keys += checkBox1->Checked ? "-delete " : " ";
	keys += comboBox1->Text;

	KeysList = marsh.marshal_as<const char*>(keys);

	// xrCompressor thread
	thread_spawn(Startup, "xrCompressorSecondThread", 0, nullptr);
	progressBar1->Visible = true;
	while (SecondThreadWorking)
	{
		if(progressBar1->Maximum == progressBar1->Value)
		{
			progressBar1->Value = 0;
			continue;
		}

		progressBar1->Value += 1;
		Sleep(50);
	}
	progressBar1->Visible = false;

	// Save
	string_path appdata;
	FS.update_path(appdata, "$app_data_root$", "xrCompressor.ltx");
	config loader(appdata, true);

	loader.WriteSect("compress", "fast", marsh.marshal_as<const char*>(this->bFastMode->Checked.ToString()));
	loader.WriteSect("compress", "db", marsh.marshal_as<const char*>(this->bDb->Checked.ToString()));
	loader.WriteSect("compress", "delete", marsh.marshal_as<const char*>(this->checkBox1->Checked.ToString()));
	loader.WriteSect("compress", "xdb", marsh.marshal_as<const char*>(this->bXdb->Checked.ToString()));
	loader.WriteSect("compress", "pack", marsh.marshal_as<const char*>(this->textBox1->Text));
	loader.WriteSect("compress", "cfg", marsh.marshal_as<const char*>(this->textBox2->Text));
	loader.WriteSect("compress", "size", marsh.marshal_as<const char*>(this->comboBox1->Text));
	if (textBox3->Text == "AnothresKeys")
	{
		loader.WriteSect("compress", "keys", "");
	}
	else
	{
		loader.WriteSect("compress", "keys", marsh.marshal_as<const char*>(this->textBox3->Text));
	}
	// End
}

System::Void xrCompressWnd::xrCompressWnd_Load(System::Object^ sender, System::EventArgs^ e)
{
	string_path appdata;
	FS.update_path(appdata, "$app_data_root$", "xrCompressor.ltx");

	if (GetFileAttributes(appdata) != -1)
	{
		config loader(appdata);
		this->bFastMode->Checked = loader.get_logic("compress", "fast");
		this->bDb->Checked = loader.get_logic("compress", "db");
		this->bXdb->Checked = loader.get_logic("compress", "xdb");
		this->checkBox1->Checked = loader.get_logic("compress", "xdb");
		this->textBox1->Text = gcnew System::String(loader.get_value("compress", "pack").c_str());
		this->textBox2->Text = gcnew System::String(loader.get_value("compress", "cfg").c_str());
		this->textBox3->Text = gcnew System::String(loader.get_value("compress", "keys").c_str());
		this->comboBox1->Text = gcnew System::String(loader.get_value("compress", "size").c_str());
	}
}