#pragma once

namespace DockForms
{
	using namespace WeifenLuo::WinFormsUI;

	public ref class FormContent : DockContent
	{
	public:
		FormContent()
		{
			// empty code
		}
	};
	
	public ref class FormFloat : FloatWindow
	{
	public:
		FormFloat(WeifenLuo::WinFormsUI::DockPanel ^panel, WeifenLuo::WinFormsUI::DockPane ^pane) : FloatWindow(panel, pane)
		{
			// empty code
		}

		FormFloat(WeifenLuo::WinFormsUI::DockPanel ^panel, WeifenLuo::WinFormsUI::DockPane ^pane, System::Drawing::Rectangle object) : FloatWindow(panel, pane, object)
		{
			// empty code
		}
	};
}