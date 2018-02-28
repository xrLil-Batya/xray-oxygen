#pragma once

namespace ECore {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	/// <summary>
	/// —водка дл€ MeshEdit
	/// </summary>
	public ref class MeshEdit : public System::Windows::Forms::Form
	{
	public:
		MeshEdit(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// ќсвободить все используемые ресурсы.
		/// </summary>
		~MeshEdit()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;





	private: System::ComponentModel::IContainer^  components;
	protected:

	private:
		/// <summary>
		/// ќб€зательна€ переменна€ конструктора.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// “ребуемый метод дл€ поддержки конструктора Ч не измен€йте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(70)));
			this->groupBox1->Location = System::Drawing::Point(748, -1);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(200, 535);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Menu";
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// MeshEdit
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->ClientSize = System::Drawing::Size(949, 536);
			this->Controls->Add(this->groupBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"MeshEdit";
			this->Text = L"MeshEdit";
			this->ResumeLayout(false);

		}
#pragma endregion
private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
	this->openFileDialog1->OpenFile();
}
};
}
