#include "../../engine.vc2008/xrCore/xrCore.h"
#include "../../engine.vc2008/xrManagedLib/HelperFuncs.h"
#include "Kernel.h"
#include "MainForm.h"

//#define gcdelete(arg) arg->~

[System::STAThreadAttribute]
int main()
{
#ifdef DEBUG
	Core._initialize("XFont Cast", 0, 0);
	//Debug._initializeAfterFS();
#else
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif 

	XFontForm::Application::EnableVisualStyles();
	XFontForm::Application::SetCompatibleTextRenderingDefault(false);
	XFontForm::Application::Run(gcnew XFontForm::MeshEdit);
	return 0;
}

System::Void XFontForm::MeshEdit::MakeDraftBox()
{
	if (this->TestText->Text == "" || this->openFileDialog1->FileName == "") return;

	Bitmap^ pImage = gcnew Bitmap(TexFontBox->Width, TexFontBox->Height);
	pImage->MakeTransparent(Color::White);

	Graphics^ pGraphics = Graphics::FromImage(pImage);
	System::Drawing::Text::PrivateFontCollection^ pLatestFont = gcnew System::Drawing::Text::PrivateFontCollection();
	pLatestFont->AddFontFile(this->openFileDialog1->FileName);

	pGraphics->DrawString
	(
		/* Text */  this->TestText->Text,
		/* Stule */ gcnew System::Drawing::Font(pLatestFont->Families[0], (int)numericUpDown1->Value, FontStyle::Regular),
		/* Color */ gcnew SolidBrush(Color::White),
		/* Size */  10, 10
	);

	TexFontBox->Image = pImage;
	delete pGraphics;
}

System::Void XFontForm::MeshEdit::button1_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (this->openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		this->InputBox->Text = this->openFileDialog1->FileName;
		
		this->MakeDraftBox();
		this->TexFontBox->Visible = true;
		this->Width = 618;
	}
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

	// @ Если успешно загружен файл
	if (XRay::CFontGen::bSucFile)
	{
		FrontSystem.CreateFolder();
		FrontSystem.CheckTexConv();

		// @ Если успешно создана папка
		if (XRay::CFontGen::bSucDir)
			FrontSystem.InitFont();
	}
	FrontSystem.ReleaseFreeType();
	System::String^ ImgPath = gcnew System::String((
		FrontSystem.PathSystem.PathOutName + 
		FrontSystem.PathSystem.FileName +  "_" +
		std::to_string(FrontSystem.PathSystem.FontSize).c_str() + "_1600.png"
		).c_str());

	
	if (System::IO::File::Exists(ImgPath))
	{
		this->TexFontBox->Image = gcnew System::Drawing::Bitmap(ImgPath);
		this->TexFontBox->Visible = true;
		this->Width = 618;
	}
	else
	{
		this->TexFontBox->Visible = false;
		this->Width = 357;
	}
}
