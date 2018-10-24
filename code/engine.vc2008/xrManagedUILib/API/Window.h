#pragma once
#include "xrUICore/UIWindow.h"

using namespace System;
using namespace System::Numerics;

namespace XRay
{
	[::System::FlagsAttribute()]
	public enum class WindowAligment : int
	{
		None	= 0,
		Left	= 1,
		Right	= 2,
		Top		= 4,
		Bottom	= 8,
		Center	= 16
	};

	public ref class Window
	{
	public:

		Window(IntPtr InNativeObject);

		property Vector2 Position
		{
			Vector2 get()
			{
				Fvector2 wndPos;
				wndPos = pNativeObject->GetWndPos();

				return Vector2(wndPos.x, wndPos.y);
			}

			void set(Vector2 InPos)
			{
				Fvector2 wndPos;
				wndPos.set(InPos.X, InPos.Y);
				pNativeObject->SetWndPos(wndPos);
			}
		}

		property Vector2 Size
		{
			Vector2 get()
			{
				Fvector2 wndSize;
				wndSize = pNativeObject->GetWndSize();

				return Vector2(wndSize.x, wndSize.y);
			}

			void set(Vector2 InSize)
			{
				Fvector2 wndSize;
				wndSize.set(InSize.X, InSize.Y);
				pNativeObject->SetWndSize(wndSize);
			}
		}

		property float Width
		{
			float get()
			{
				return pNativeObject->GetWidth();
			}

			void set(float InWidth)
			{
				pNativeObject->SetWidth(InWidth);
			}
		}

		property float Height
		{
			float get()
			{
				return pNativeObject->GetHeight();
			}

			void set(float InHeight)
			{
				pNativeObject->SetHeight(InHeight);
			}
		}

		property WindowAligment Aligment
		{
			WindowAligment get()
			{
				EWindowAlignment windowAligment = pNativeObject->GetAlignment();
				return WindowAligment(windowAligment);
			}

			void set(WindowAligment value)
			{
				int eValue = (int)value;
				EWindowAlignment windowAligment = (EWindowAlignment)eValue;
				pNativeObject->SetAlignment(windowAligment);
			}
		}

	private:

		CUIWindow* pNativeObject;
	};
}