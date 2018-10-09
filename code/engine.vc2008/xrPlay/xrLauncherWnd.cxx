////////////////////////////////////////
#include "xrLauncherWnd.h"
#include "../xrCore/xrCore.h"
////////////////////////////////////////
xrPlay::returned_values xrPlay::ret_values;
////////////////////////////////////////
using namespace xrPlay;
////////////////////////////////////////


/// <summary>
/// Validating CPU instructions
/// </summary>
void xrLauncherWnd::CPUTest()
{
	if (!CPU::Info.hasFeature(CPUFeature::SSE3))
	{
		this->label2->Text = L"SSE3 and AVX instructions aren't found. That may be affect on stability.";
		this->label2->ForeColor = System::Drawing::Color::Red;
	}
	else if (!CPU::Info.hasFeature(CPUFeature::AVX))
	{
		this->label2->Text = L"AVX instructions aren't found. That may be affect on stability.";
		this->label2->ForeColor = System::Drawing::Color::DarkOrange;
	}
}


// Close Button
System::Void xrLauncherWnd::BtnClose_Click(System::Object^  sender, System::EventArgs^  e)
{
	ret_values.type_ptr = 0;
	this->Close();
}


// Showing the window minimized
System::Void xrLauncherWnd::label1_Click(System::Object^  sender, System::EventArgs^  e)
{
	ShowWindow((HWND)this->Handle.ToInt64(), SW_MINIMIZE);
}

////////////////////////////////////////
returned_values values;
////////////////////////////////////////
#include "../xrCore/xrCore.h"
////////////////////////////////////////


//  Method for main button (run xrEngine)
System::Void xrLauncherWnd::button1_Click(System::Object^  sender, System::EventArgs^  e)
{
	ret_values.type_ptr = 1;
	msclr::interop::marshal_context marsh;

	System::String^ rendered = "-r2";
	if (radioButton3->Checked) rendered = "-r2.5";
	if (radioButton4->Checked) rendered = "-r3";
	if (radioButton5->Checked) rendered = "-r4";

	// Saving launch data to launc.ltx
	const char* render	= marsh.marshal_as<const char*>(rendered);
	const char* cmdline = marsh.marshal_as<const char*>(textBox1->Text);

	launch->WriteSect("global", "render", render);
	if (textBox1->Text != "")
	{
		launch->WriteSect("global", "cmdline", cmdline);
	}
	delete launch;
	// end

	const char* pCmdLine = marsh.marshal_as<char const*>(textBox1->Text + " " + rendered);
	ret_values.params_list = strdup(pCmdLine);
	this->Close();
}


System::Void xrLauncherWnd::xrLauncherWnd_Load(System::Object^  sender, System::EventArgs^  e)
{
	this->Init();
}


/// <summary>
///  Init xrEngine
/// </summary>
void xrLauncherWnd::Init()
{
	// Launcher data init
	string_path user_data;
	FS.update_path(user_data, "$app_data_root$", "launch.ltx");
	launch = new config(user_data, true);

	// Radiobox init
	if (launch->get_value("global", "render") == "-r2.5")
		radioButton3->Checked = true;
	else if (launch->get_value("global", "render") == "-r3")	
		radioButton4->Checked = true;
	else if (launch->get_value("global", "render") == "-r4")	
		radioButton5->Checked = true;
	else														
		radioButton2->Checked = true;

	// Command line init
	const std::string cmd = launch->get_value("global", "cmdline");
	if (cmd != "Error reading! Section: global Key: cmdline")
		textBox1->Text = gcnew System::String(cmd.c_str());
	this->CPUTest();
}