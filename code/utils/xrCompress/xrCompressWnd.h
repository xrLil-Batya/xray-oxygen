#pragma once

namespace xrCompress
{

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Сводка для xrCompressWnd
	/// </summary>
	public ref class xrCompressWnd : public System::Windows::Forms::Form
	{
	public:
		xrCompressWnd(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~xrCompressWnd()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  CloseBtn;
	protected:
	private: System::Windows::Forms::Button^  PackBtn;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  textBox1;

	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  textBox3;
	private: System::Windows::Forms::CheckBox^  bFastMode;
	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::RadioButton^  bDb;
	private: System::Windows::Forms::RadioButton^  bXdb;
	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::ProgressBar^  progressBar1;




	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->CloseBtn = (gcnew System::Windows::Forms::Button());
			this->PackBtn = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->textBox3 = (gcnew System::Windows::Forms::TextBox());
			this->bFastMode = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->bDb = (gcnew System::Windows::Forms::RadioButton());
			this->bXdb = (gcnew System::Windows::Forms::RadioButton());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->SuspendLayout();
			// 
			// CloseBtn
			// 
			this->CloseBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->CloseBtn->Location = System::Drawing::Point(372, 95);
			this->CloseBtn->Name = L"CloseBtn";
			this->CloseBtn->Size = System::Drawing::Size(75, 23);
			this->CloseBtn->TabIndex = 0;
			this->CloseBtn->Text = L"Close";
			this->CloseBtn->UseVisualStyleBackColor = true;
			this->CloseBtn->Click += gcnew System::EventHandler(this, &xrCompressWnd::CloseBtn_Click);
			// 
			// PackBtn
			// 
			this->PackBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->PackBtn->Location = System::Drawing::Point(12, 95);
			this->PackBtn->Name = L"PackBtn";
			this->PackBtn->Size = System::Drawing::Size(75, 23);
			this->PackBtn->TabIndex = 1;
			this->PackBtn->Text = L"Pack";
			this->PackBtn->UseVisualStyleBackColor = true;
			this->PackBtn->Click += gcnew System::EventHandler(this, &xrCompressWnd::PackBtn_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(70, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Folder Name:";
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(15, 25);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(149, 20);
			this->textBox1->TabIndex = 3;
			this->textBox1->Text = L"pack_gamedata";
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(170, 25);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(117, 20);
			this->textBox2->TabIndex = 5;
			this->textBox2->Text = L"datapack.ltx";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(168, 9);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(69, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Config name:";
			// 
			// textBox3
			// 
			this->textBox3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox3->Location = System::Drawing::Point(93, 97);
			this->textBox3->Name = L"textBox3";
			this->textBox3->Size = System::Drawing::Size(273, 20);
			this->textBox3->TabIndex = 6;
			this->textBox3->Text = L"AnothersKeys";
			this->textBox3->Click += gcnew System::EventHandler(this, &xrCompressWnd::textBox3_TextChanged);
			// 
			// bFastMode
			// 
			this->bFastMode->AutoSize = true;
			this->bFastMode->Location = System::Drawing::Point(15, 61);
			this->bFastMode->Name = L"bFastMode";
			this->bFastMode->Size = System::Drawing::Size(149, 17);
			this->bFastMode->TabIndex = 7;
			this->bFastMode->Text = L"Compression only text files";
			this->bFastMode->UseVisualStyleBackColor = true;
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(170, 61);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(117, 17);
			this->checkBox1->TabIndex = 8;
			this->checkBox1->Text = L"Delete old archives";
			this->checkBox1->UseVisualStyleBackColor = true;
			// 
			// bDb
			// 
			this->bDb->AutoSize = true;
			this->bDb->Checked = true;
			this->bDb->Location = System::Drawing::Point(362, 12);
			this->bDb->Name = L"bDb";
			this->bDb->Size = System::Drawing::Size(80, 17);
			this->bDb->TabIndex = 9;
			this->bDb->TabStop = true;
			this->bDb->Text = L"Pack to .db";
			this->bDb->UseVisualStyleBackColor = true;
			// 
			// bXdb
			// 
			this->bXdb->AutoSize = true;
			this->bXdb->Location = System::Drawing::Point(362, 35);
			this->bXdb->Name = L"bXdb";
			this->bXdb->Size = System::Drawing::Size(85, 17);
			this->bXdb->TabIndex = 10;
			this->bXdb->Text = L"Pack to .xdb";
			this->bXdb->UseVisualStyleBackColor = true;
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(6) { L"-128", L"-256", L"-512", L"-640", L"-768", L"-1024" });
			this->comboBox1->Location = System::Drawing::Point(362, 61);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(80, 21);
			this->comboBox1->TabIndex = 11;
			this->comboBox1->Text = L"-512";
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(91, 97);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(275, 20);
			this->progressBar1->TabIndex = 12;
			this->progressBar1->Visible = false;
			// 
			// xrCompressWnd
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ControlLight;
			this->ClientSize = System::Drawing::Size(459, 130);
			this->ControlBox = false;
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->comboBox1);
			this->Controls->Add(this->bXdb);
			this->Controls->Add(this->bDb);
			this->Controls->Add(this->checkBox1);
			this->Controls->Add(this->bFastMode);
			this->Controls->Add(this->textBox3);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->PackBtn);
			this->Controls->Add(this->CloseBtn);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"xrCompressWnd";
			this->Load += gcnew System::EventHandler(this, &xrCompressWnd::xrCompressWnd_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void CloseBtn_Click(System::Object^  sender, System::EventArgs^  e)
	{
		this->Close();
	}
	private: System::Void PackBtn_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void textBox3_TextChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		this->textBox3->Text = nullptr;
	}
	private: System::Void xrCompressWnd_Load(System::Object^  sender, System::EventArgs^  e);
};
}
