/******************************************************************************************************************
***** Authors: Lord
***** Date of creation: 03.05.2018
***** Description: File of impelmentation of Level Editor Application
***** Copyright: GSC, OxyGen Team 2018 (C)
******************************************************************************************************************/

#pragma once

namespace OxySDK
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class LE_Form : public System::Windows::Forms::Form
	{
	public:
		LE_Form(void)
		{
			InitializeComponent();
		}

	protected:
		~LE_Form()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = gcnew System::ComponentModel::Container();
			this->Size = System::Drawing::Size(300, 300);
			this->Text = L"LE_Form";
			this->Padding = System::Windows::Forms::Padding(0);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		}
#pragma endregion
	};
}
