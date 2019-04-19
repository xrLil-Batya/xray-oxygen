#pragma once

#include "Model.h"

using namespace System;
using namespace System::Collections::Generic;


namespace XRay
{
	namespace Editor
	{
		public ref class EditorOcclusion
		{
		public:
			EditorOcclusion();
			~EditorOcclusion();
			void AddObjectToRenderableList(XRay::Model^ object);
			void FillOcclusionList(IntPtr pRenderInterface);
			static EditorOcclusion^ Instance();
		private:

			List<XRay::Model^>^ _renderableList;
			IntPtr _opaqueOcclusionInterface;
			static XRay::Editor::EditorOcclusion^ _Instance;
		};
	}
}