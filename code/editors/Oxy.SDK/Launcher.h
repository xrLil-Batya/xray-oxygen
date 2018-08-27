/******************************************************************************************************************
***** Authors: Lord
***** Date of creation: 03.05.2018
***** Description: File of impelmentation of Laucnher, which can run other editors
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

	/// <summary>
	/// Сводка для Launcher
	/// </summary>
	public ref class Launcher : public System::Windows::Forms::Form
	{
	public:
		Launcher(void)
		{
			InitializeComponent();
		}

	protected:
		~Launcher()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button4;




	protected:

	private:
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(127, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"ПОКА ЧТО ПРОТОТИП";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(15, 46);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(302, 157);
			this->button1->TabIndex = 1;
			this->button1->Text = L"LE";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(15, 223);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(302, 157);
			this->button3->TabIndex = 3;
			this->button3->Text = L"AE";
			this->button3->UseVisualStyleBackColor = true;
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(364, 223);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(302, 157);
			this->button2->TabIndex = 5;
			this->button2->Text = L"PE";
			this->button2->UseVisualStyleBackColor = true;
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(364, 46);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(302, 157);
			this->button4->TabIndex = 4;
			this->button4->Text = L"SE";
			this->button4->UseVisualStyleBackColor = true;
			// 
			// Launcher
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(678, 383);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label1);
			this->Name = L"Launcher";
			this->Text = L"Launcher";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};
}
