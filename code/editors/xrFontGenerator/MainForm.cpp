#include "../../engine.vc2008/xrCore/xrCore.h"
#include "../../engine.vc2008/xrManagedLib/HelperFuncs.h"
#include "Kernel.h"
#include "MainForm.h"

[System::STAThreadAttribute]
int main()
{
	HWND hwnd = GetConsoleWindow();
	ShowWindow(hwnd, SW_HIDE);

	XFontForm::Application::EnableVisualStyles();
	XFontForm::Application::SetCompatibleTextRenderingDefault(false);
	XFontForm::Application::Run(gcnew XFontForm::MeshEdit);
	return 0;
}

System::Void XFontForm::MeshEdit::button3_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	string128 InPathNameStr = { 0 };
	ConvertDotNetStringToAscii(this->InputBox->Text, InPathNameStr);

	string128 OutPathNameStr = { 0 };
	ConvertDotNetStringToAscii(this->OutputBox->Text, OutPathNameStr);

	XRay::CFontGen FrontSystem;
	FrontSystem.InitFreeType();
	FrontSystem.PathSystem.PathName = InPathNameStr;
	FrontSystem.PathSystem.PathOutName = OutPathNameStr;
	FrontSystem.PathSystem.PathOutName += "\\";
	FrontSystem.PathSystem.FontSize = (unsigned int)this->numericUpDown1->Value;

	FrontSystem.ManageCreationFile();
	if (XRay::CFontGen::bSucFile)
	{
		FrontSystem.CreateFolder();
		FrontSystem.CheckTexConv();

		if (XRay::CFontGen::bSucDir)
			FrontSystem.InitFont();
	}
	FrontSystem.ReleaseFreeType();

	System::String^ ImgPath = gcnew System::String((
		FrontSystem.PathSystem.PathOutName + 
		FrontSystem.PathSystem.FileName +  "_" +
		std::to_string(FrontSystem.PathSystem.FontSize).c_str() + "_1600.png"
		).c_str());

	this->Width = 618;
	this->TexFontBox->BackgroundImage = gcnew System::Drawing::Bitmap(ImgPath);
	this->TexFontBox->Visible = true;
}
