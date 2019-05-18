#pragma once

using namespace System;

namespace XRay
{
	public ref class Model
	{
	public:

		Model(IntPtr InNativeObject);
		~Model();

		property String^ Name
		{
			String^ get();
		}

		property IntPtr NativeObject
		{
			IntPtr get()
			{
				return _nativeObject;
			}
		}

	private:
		IntPtr _nativeObject;
	};

}