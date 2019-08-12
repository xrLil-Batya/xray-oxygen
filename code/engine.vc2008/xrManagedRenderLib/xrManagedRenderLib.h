#pragma once
#include "EditorOcclusion.h"

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Interop;
using namespace System::Windows::Input;
using namespace System::Windows::Media;
using namespace System::Runtime::InteropServices;

namespace XRay
{
	/// <summary>
	/// Expose render to WPF
	///	</summary>
	public ref class XRayRenderHost : public HwndHost, IKeyboardInputSink
	{
	protected:

		virtual HandleRef BuildWindowCore(HandleRef hwndParent) override;
		virtual Void DestroyWindowCore(HandleRef hwnd) override;
		virtual IntPtr WndProc(IntPtr hwnd, Int32 msg, IntPtr wParam, IntPtr lParam, Boolean% handled) override;
		virtual Void OnMouseRightButtonDown(::System::Windows::Input::MouseButtonEventArgs^ e) override;
		virtual Void OnMouseRightButtonUp(::System::Windows::Input::MouseButtonEventArgs^ e) override;

	public:
		virtual ~XRayRenderHost();

		virtual bool TabIntoCore(TraversalRequest^ request) override;
		virtual bool TranslateAcceleratorCore(::System::Windows::Interop::MSG% msg, ModifierKeys modifiers) override;
		virtual bool OnMnemonicCore(::System::Windows::Interop::MSG% msg, ModifierKeys modifiers) override;
		void SetOcclusion(XRay::Editor::EditorOcclusion^ InOcclusion);

		void UpdateEngine();

	private:
		::MSG ConvertMessage(::System::Windows::Interop::MSG% msg);

		bool isEditor = false;
		XRay::Editor::EditorOcclusion^ _editorOcclusion = nullptr;

	public:
		/// <summary>This is xrEditor?</summary> 
		property bool EditorMode
		{
			bool get()
			{
				return isEditor;
			}
			void set(bool val)
			{
				isEditor = val;
			}
		}
	};

	public ref class IManagedRenderLib abstract sealed
	{
	public:
		static void RenderTick();
	};
}
