#pragma once
struct Mesh;
namespace ECore {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	/// <summary>
	/// Сводка для MeshEdit
	/// </summary>
	public ref class MeshEdit : public System::Windows::Forms::Form
	{
	public:
		MeshEdit(void)
		{
			InitializeComponent();
			mesh = nullptr;
		}
	private: System::Windows::Forms::ListBox^  GMtList;
	public:

	private: System::Windows::Forms::ToolStripMenuItem^  gameMaterialsToolStripMenuItem;
	private: System::Windows::Forms::ListBox^  BonesList;
	private: System::Windows::Forms::ToolStripMenuItem^  bonesListToolStripMenuItem;
	private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
	private: System::Windows::Forms::ToolStripMenuItem^  newToolStripMenuItem;
	public:

	public:

	protected:
		Mesh * mesh;
		/// <summary>
		/// Освободить все используемые ресурсы.
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
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  filesToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  meshToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  loadToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  saveToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  exportToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  toolsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  xRayIngameToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  xRayAnimGameToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  xRayAnimSDKToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  xRayObjectToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  xRayAnimLibraryToolStripMenuItem;

	private: System::ComponentModel::IContainer^  components;
	protected:

	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->BonesList = (gcnew System::Windows::Forms::ListBox());
			this->GMtList = (gcnew System::Windows::Forms::ListBox());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->filesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->meshToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->xRayObjectToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->xRayAnimLibraryToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exportToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->xRayIngameToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->xRayAnimGameToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->xRayAnimSDKToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->gameMaterialsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->bonesListToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->newToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->groupBox1->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(70)));
			this->groupBox1->Controls->Add(this->BonesList);
			this->groupBox1->Controls->Add(this->GMtList);
			this->groupBox1->Location = System::Drawing::Point(748, 27);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(200, 507);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Menu";
			// 
			// BonesList
			// 
			this->BonesList->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(69)),
				static_cast<System::Int32>(static_cast<System::Byte>(70)));
			this->BonesList->FormattingEnabled = true;
			this->BonesList->Location = System::Drawing::Point(1, 212);
			this->BonesList->Name = L"BonesList";
			this->BonesList->Size = System::Drawing::Size(200, 147);
			this->BonesList->TabIndex = 1;
			this->BonesList->Visible = false;
			this->BonesList->SelectedIndexChanged += gcnew System::EventHandler(this, &MeshEdit::BonesList_SelectedIndexChanged);
			// 
			// GMtList
			// 
			this->GMtList->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(69)),
				static_cast<System::Int32>(static_cast<System::Byte>(70)));
			this->GMtList->FormattingEnabled = true;
			this->GMtList->Location = System::Drawing::Point(1, 360);
			this->GMtList->Name = L"GMtList";
			this->GMtList->Size = System::Drawing::Size(200, 147);
			this->GMtList->TabIndex = 0;
			this->GMtList->Visible = false;
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			this->openFileDialog1->Filter = L"Stalker Objects (*.object)|*.object|OGF (*.ogf)|*.ogf|OMF (*.omf)|*.omf";
			this->openFileDialog1->InitialDirectory = L".\\";
			this->openFileDialog1->RestoreDirectory = true;
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->filesToolStripMenuItem,
					this->toolsToolStripMenuItem, this->helpToolStripMenuItem
			});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(949, 24);
			this->menuStrip1->TabIndex = 1;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// filesToolStripMenuItem
			// 
			this->filesToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->meshToolStripMenuItem,
					this->newToolStripMenuItem
			});
			this->filesToolStripMenuItem->Name = L"filesToolStripMenuItem";
			this->filesToolStripMenuItem->Size = System::Drawing::Size(42, 20);
			this->filesToolStripMenuItem->Text = L"Files";
			// 
			// meshToolStripMenuItem
			// 
			this->meshToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->loadToolStripMenuItem,
					this->saveToolStripMenuItem, this->exportToolStripMenuItem
			});
			this->meshToolStripMenuItem->Name = L"meshToolStripMenuItem";
			this->meshToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->meshToolStripMenuItem->Text = L"Mesh";
			// 
			// loadToolStripMenuItem
			// 
			this->loadToolStripMenuItem->Name = L"loadToolStripMenuItem";
			this->loadToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->loadToolStripMenuItem->Text = L"Load";
			this->loadToolStripMenuItem->Click += gcnew System::EventHandler(this, &MeshEdit::loadToolStripMenuItem_Click);
			// 
			// saveToolStripMenuItem
			// 
			this->saveToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->xRayObjectToolStripMenuItem,
					this->xRayAnimLibraryToolStripMenuItem
			});
			this->saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
			this->saveToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->saveToolStripMenuItem->Text = L"Save";
			// 
			// xRayObjectToolStripMenuItem
			// 
			this->xRayObjectToolStripMenuItem->Name = L"xRayObjectToolStripMenuItem";
			this->xRayObjectToolStripMenuItem->Size = System::Drawing::Size(173, 22);
			this->xRayObjectToolStripMenuItem->Text = L"X-Ray Object";
			// 
			// xRayAnimLibraryToolStripMenuItem
			// 
			this->xRayAnimLibraryToolStripMenuItem->Name = L"xRayAnimLibraryToolStripMenuItem";
			this->xRayAnimLibraryToolStripMenuItem->Size = System::Drawing::Size(173, 22);
			this->xRayAnimLibraryToolStripMenuItem->Text = L"X-Ray AnimLibrary";
			// 
			// exportToolStripMenuItem
			// 
			this->exportToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->xRayIngameToolStripMenuItem,
					this->xRayAnimGameToolStripMenuItem, this->xRayAnimSDKToolStripMenuItem
			});
			this->exportToolStripMenuItem->Name = L"exportToolStripMenuItem";
			this->exportToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->exportToolStripMenuItem->Text = L"Export";
			// 
			// xRayIngameToolStripMenuItem
			// 
			this->xRayIngameToolStripMenuItem->Name = L"xRayIngameToolStripMenuItem";
			this->xRayIngameToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->xRayIngameToolStripMenuItem->Text = L"X-Ray inGame";
			this->xRayIngameToolStripMenuItem->Click += gcnew System::EventHandler(this, &MeshEdit::xRayIngameToolStripMenuItem_Click);
			// 
			// xRayAnimGameToolStripMenuItem
			// 
			this->xRayAnimGameToolStripMenuItem->Name = L"xRayAnimGameToolStripMenuItem";
			this->xRayAnimGameToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->xRayAnimGameToolStripMenuItem->Text = L"X-Ray animGame";
			// 
			// xRayAnimSDKToolStripMenuItem
			// 
			this->xRayAnimSDKToolStripMenuItem->Name = L"xRayAnimSDKToolStripMenuItem";
			this->xRayAnimSDKToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->xRayAnimSDKToolStripMenuItem->Text = L"X-Ray animSDK";
			// 
			// toolsToolStripMenuItem
			// 
			this->toolsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->gameMaterialsToolStripMenuItem,
					this->bonesListToolStripMenuItem
			});
			this->toolsToolStripMenuItem->Name = L"toolsToolStripMenuItem";
			this->toolsToolStripMenuItem->Size = System::Drawing::Size(48, 20);
			this->toolsToolStripMenuItem->Text = L"Tools";
			// 
			// gameMaterialsToolStripMenuItem
			// 
			this->gameMaterialsToolStripMenuItem->Name = L"gameMaterialsToolStripMenuItem";
			this->gameMaterialsToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->gameMaterialsToolStripMenuItem->Text = L"GameMaterials";
			this->gameMaterialsToolStripMenuItem->Click += gcnew System::EventHandler(this, &MeshEdit::gameMaterialsToolStripMenuItem_Click);
			// 
			// bonesListToolStripMenuItem
			// 
			this->bonesListToolStripMenuItem->Name = L"bonesListToolStripMenuItem";
			this->bonesListToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->bonesListToolStripMenuItem->Text = L"Bones list";
			this->bonesListToolStripMenuItem->Click += gcnew System::EventHandler(this, &MeshEdit::bonesListToolStripMenuItem_Click);
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			this->helpToolStripMenuItem->Size = System::Drawing::Size(44, 20);
			this->helpToolStripMenuItem->Text = L"Help";
			// 
			// newToolStripMenuItem
			// 
			this->newToolStripMenuItem->Name = L"newToolStripMenuItem";
			this->newToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->newToolStripMenuItem->Text = L"New";
			this->newToolStripMenuItem->Click += gcnew System::EventHandler(this, &MeshEdit::newToolStripMenuItem_Click);
			// 
			// MeshEdit
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->ClientSize = System::Drawing::Size(949, 536);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->menuStrip1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MeshEdit";
			this->Text = L"MeshEdit";
			this->Load += gcnew System::EventHandler(this, &MeshEdit::MeshEdit_Load);
			this->groupBox1->ResumeLayout(false);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		this->openFileDialog1->OpenFile();
	}
	private: System::Void loadToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void xRayIngameToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MeshEdit_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void gameMaterialsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void BonesList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void bonesListToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void newToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
};
}