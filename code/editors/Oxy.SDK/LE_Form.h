/******************************************************************************************************************
***** Authors: Lord
***** Date of creation: 03.05.2018
***** Description: File of impelmentation of Level Editor Application
***** Copyright: GSC, OxyGen Team 2018 (C)
******************************************************************************************************************/

#pragma once

#include "DockPanels.h"

 
namespace OxySDK
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace WeifenLuo::WinFormsUI;
	
	public ref class LE_Form : public System::Windows::Forms::Form
	{
	public:
		LE_Form(void)
		{
			InitializeComponent();


			d = gcnew DockForms::FormContent();
			d->TabText = "Вкладка 1";
			d->Show(dockMain, DockState::DockRight);

			f = gcnew DockForms::FormContent();
			f->TabText = "Рендер Форма";
			f->Show(dockMain, DockState::Document);

			a = gcnew DockForms::FormContent();
			a->TabText = "Вкладка 2";
			a->Show(dockMain, DockState::DockRight);
		}
	
	protected:
		~LE_Form()
		{
			if (components)
			{
			
				delete components;
				delete d;
				delete f;
				delete a;
			}
		}
	
	private: 
		DockForms::FormContent ^d;
		DockForms::FormContent ^f;
		DockForms::FormContent ^a;

	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	protected:
	private: System::Windows::Forms::ToolStripMenuItem^  projectToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  newFileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  settingsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  quitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  somethingElseToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  somethingElse2ToolStripMenuItem;


	private: System::ComponentModel::BackgroundWorker^  backgroundWorker1;
	private: WeifenLuo::WinFormsUI::DockPanel^  dockMain;





	private:
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->projectToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->newFileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->settingsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->quitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->somethingElseToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->somethingElse2ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->dockMain = (gcnew WeifenLuo::WinFormsUI::DockPanel());
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->projectToolStripMenuItem,
					this->somethingElseToolStripMenuItem, this->somethingElse2ToolStripMenuItem
			});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(1028, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// projectToolStripMenuItem
			// 
			this->projectToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->newFileToolStripMenuItem,
					this->settingsToolStripMenuItem, this->quitToolStripMenuItem
			});
			this->projectToolStripMenuItem->Name = L"projectToolStripMenuItem";
			this->projectToolStripMenuItem->Size = System::Drawing::Size(56, 20);
			this->projectToolStripMenuItem->Text = L"Project";
			// 
			// newFileToolStripMenuItem
			// 
			this->newFileToolStripMenuItem->Name = L"newFileToolStripMenuItem";
			this->newFileToolStripMenuItem->Size = System::Drawing::Size(119, 22);
			this->newFileToolStripMenuItem->Text = L"New File";
			// 
			// settingsToolStripMenuItem
			// 
			this->settingsToolStripMenuItem->Name = L"settingsToolStripMenuItem";
			this->settingsToolStripMenuItem->Size = System::Drawing::Size(119, 22);
			this->settingsToolStripMenuItem->Text = L"Settings";
			// 
			// quitToolStripMenuItem
			// 
			this->quitToolStripMenuItem->Name = L"quitToolStripMenuItem";
			this->quitToolStripMenuItem->Size = System::Drawing::Size(119, 22);
			this->quitToolStripMenuItem->Text = L"Quit";
			// 
			// somethingElseToolStripMenuItem
			// 
			this->somethingElseToolStripMenuItem->Name = L"somethingElseToolStripMenuItem";
			this->somethingElseToolStripMenuItem->Size = System::Drawing::Size(100, 20);
			this->somethingElseToolStripMenuItem->Text = L"Something else";
			// 
			// somethingElse2ToolStripMenuItem
			// 
			this->somethingElse2ToolStripMenuItem->Name = L"somethingElse2ToolStripMenuItem";
			this->somethingElse2ToolStripMenuItem->Size = System::Drawing::Size(109, 20);
			this->somethingElse2ToolStripMenuItem->Text = L"Something else 2";
			// 
			// dockMain
			// 
			this->dockMain->ActiveAutoHideContent = nullptr;
			this->dockMain->Dock = System::Windows::Forms::DockStyle::Fill;
			this->dockMain->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::World));
			this->dockMain->Location = System::Drawing::Point(0, 24);
			this->dockMain->Name = L"dockMain";
			this->dockMain->Size = System::Drawing::Size(1028, 651);
			this->dockMain->TabIndex = 3;
			// 
			// LE_Form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::White;
			this->ClientSize = System::Drawing::Size(1028, 675);
			this->Controls->Add(this->dockMain);
			this->Controls->Add(this->menuStrip1);
			this->IsMdiContainer = true;
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"LE_Form";
			this->Text = L"LE_Form";
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};
}
